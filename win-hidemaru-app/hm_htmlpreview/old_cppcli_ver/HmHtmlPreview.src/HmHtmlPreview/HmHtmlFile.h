#pragma once

#include "HmHtmlBase.h"

public ref class HmHtmlFileForm : public HmHtmlBaseForm {
public:
	HmHtmlFileForm(HWND hWndHidemaru) : HmHtmlBaseForm(hWndHidemaru) { }

protected:
	/// <summary>�t�H�[���\������WebBrowser�ɗ������B
	/// File���[�h�̎��́A������Uri�̓��e�𗬂�����
	///</summary>
	virtual void wb_DocumentInit() override {
		try {
			strCurFileFullPath = GetCurrentHidemaruFileName();

			// �G�ۂŕҏW���̃e�L�X�g���ς�����̂���B
			if (strCurFileFullPath->Length > 0) {

				// ���̏���
				CoInternetSetFeatureEnabled(FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_PROCESS, true);

				// URL�œǂݒ���
				Uri^ u = gcnew Uri(strCurFileFullPath);
				wb->Navigate(u);

				if (strPrvFileFullPath != strCurFileFullPath) {
					strPrvFileFullPath = strCurFileFullPath;
					SetWatcherAttribute();
				}
			}
		}
		catch (Exception^) {}
	}

	/// <summary>�^�C�}�[�����ݒ�B�f�[�^�X�V�̕K�v�������邩�ǂ����̊�b�X�V�B
	/// File���[�h�ł́A���̃^�C�~���O�̓��[�U�[���u�Ȃ�̃t�@�C���v���J���Ă���̂��A�̏��̍X�V�B
	/// �O��ƐH������Ă�����A�Ď��Ώۂ̃t�H���_�����ēx�ݒ肵����
	///</summary>
	virtual void update_Tick_Implements(Object ^sender, EventArgs ^e) override {
		try  {
			strCurFileFullPath = GetCurrentHidemaruFileName();

			if (strCurFileFullPath->Length > 0) {
				if (strPrvFileFullPath != strCurFileFullPath) {
					strPrvFileFullPath = strCurFileFullPath;
					SetWatcherAttribute();
				}
			}
			else {
				Stop();
			}
		}
		catch (Exception^) {}
	}



	/// <summary>File���[�h�p�̃t�@�C���̊Ď�</summary>
	IO::FileSystemWatcher^ watcher = nullptr;

	/// <summary>File���[�h�p�̃t�@�C���̊Ď��̑����B
	/// �G�ۂ��t�@�C������ύX������A�f�B���N�g����ύX�����肵�Ă��ǐՂł���悤�ɂ��邽�߁B
	/// ���A�t�@�C���̕ۑ����́AwebBrowser�̓��e���X�V����B
	/// </summary>
	void SetWatcherAttribute() {
		try {
			if (strCurFileFullPath->Length > 0) {
				if (watcher == nullptr) {
					watcher = gcnew IO::FileSystemWatcher();
					watcher->NotifyFilter =
						(System::IO::NotifyFilters::LastAccess
						| System::IO::NotifyFilters::LastWrite
						| System::IO::NotifyFilters::FileName
						| System::IO::NotifyFilters::DirectoryName);
					;
				}
				watcher->Path = System::IO::Path::GetDirectoryName(strCurFileFullPath);
				// wb�Ƃ͓��e�X�V���񓯊��Փ˂���̂ŁA�����͓�������
				watcher->SynchronizingObject = wb;
				watcher->Filter = "*.*";

				watcher->Changed += gcnew FileSystemEventHandler(this, &HmHtmlFileForm::watcher_Changed);
				watcher->IncludeSubdirectories = false;

				//�Ď����J�n����
				watcher->EnableRaisingEvents = true;
			}
		}
		catch (Exception^) {
		}
	}

	/// <summary>File���[�h�p�̃t�@�C���̊Ď��̑����B
	/// �G�ۂ��t�@�C������ύX������A�f�B���N�g����ύX�����肵�Ă��ǐՂł���悤�ɂ��邽�߁B
	/// ���A�t�@�C���̕ۑ����́AwebBrowser�̓��e���X�V����B
	/// </summary>
	void watcher_Changed(Object^ o, FileSystemEventArgs^ e) {
		try {
			if (e->ChangeType == IO::WatcherChangeTypes::Changed) {
				// �G�ۂŕҏW���̃e�L�X�g���ς�����̂���B
				if (String::Compare(e->FullPath, strCurFileFullPath, true)) {

					// ���̃X�N���[���̈ʒu����ŕ�������̂ŕۑ����Ă���
					if (wb->Document)
					{
						try {
							if (wb->Document->Body) {
								// ��i���̇@�D
								// Document->Body������p�^�[���B����ł͎��s����Ƃ�������B
								webBrowserScroll.X = wb->Document->Body->ScrollLeft;
								webBrowserScroll.Y = wb->Document->Body->ScrollTop;
							}
							// ��i���̇A�D
							// HTML�G�������g��Scroll�ʒu�����ɍs���p�^�[���B����������s����Ƃ�������B
							if (wb->Document->GetElementsByTagName("HTML")) {
								if (wb->Document->GetElementsByTagName("HTML")[0]) {
									webBrowserScroll.X = wb->Document->GetElementsByTagName("HTML")[0]->ScrollLeft;
									webBrowserScroll.Y = wb->Document->GetElementsByTagName("HTML")[0]->ScrollTop;
								}

							}
						}
						catch (Exception^) {
						}
					}

					// ���̏���
					CoInternetSetFeatureEnabled(FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_PROCESS, true);
					// URL�œǂݒ���
					Uri^ u = gcnew Uri(strCurFileFullPath);
					wb->Navigate(u);

					isDocumentChanged = true;

				}
			}
		}
		catch (Exception^) {
		}
	}

public:
	/// <summary>File���[�h�p�̃t�@�C���̊Ď��̏I���B
	/// �t�@�C�����V�K�ɂȂ��Ă��鎞(�t�@�C���̎��Ԃ�������)��A�t�H�[�����I�����钼�O�A�Ȃǂɒ�~�����B
	/// </summary>
	virtual void Stop() override {
		try {
			// �e�̕��͂���Ƃ�
			HmHtmlBaseForm::Stop();

			// watcher����ʂɒǉ����Ă�̂ł�����~�߂�
			if (watcher != nullptr) {
				//�Ď����I��
				watcher->EnableRaisingEvents = false;
				watcher = nullptr;
			}
		}
		catch (Exception^) {}
	}

};

