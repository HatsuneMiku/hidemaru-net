#pragma once

#include "hidemaruexe_export.h"

using namespace System;
using namespace System::IO;
using namespace System::Drawing;
using namespace System::Windows::Forms;
using namespace System::Net;

using namespace OpenQA::Selenium;
using namespace OpenQA::Selenium::Chrome;

CHidemaruExeExport HMEXE;


/// �₽��߂�����try_catch�Ȃ̂́A�u���s�ӂɏG�ۖ{�̂����Ă��\�Ȍ���ق��ďI���ł���悤�ɂ��邽�߁v
enum class hmHtmlPreviewMode { Live = 0, File = 1 };


/// <summary> ���[�h�Ɋւ�炸���� </summary>
public ref class HmHtmlPreviewChromeForm : System::Windows::Forms::Form
{
public:
	static HmHtmlPreviewChromeForm^ form;
protected:
	HWND hWndHidemaru = NULL;
	String^ strCurFileFullPath = "";
	String^ strPrvFileFullPath = "";
	String^ strPrvHmEditTotalText = "";
	hmHtmlPreviewMode mode;
	Timer^ update;

protected:
	IWebDriver^ driver;
	ChromeOptions^ chromeOptions;
	ChromeDriverService^ chromeService;

public:

	HmHtmlPreviewChromeForm(HWND hWndHidemaru)
	{
		this->hWndHidemaru = hWndHidemaru;

		SetFormAttribute();
		SetWebBrowserAttribulte();
		SetTimerAttribute();
	}

protected:
	/// <summary>���݂̏G�ۃv���Z�X�ł̏G�ۂ��ҏW���Ă���t�@�C�������擾����(�قȂ�v���Z�X�̏G�ۂ͊֌W�Ȃ�)</summary>
	String^ GetCurrentHidemaruFileName() {
		strPrvFileFullPath = strCurFileFullPath;
		const int WM_HIDEMARUINFO = WM_USER + 181;
		const int HIDEMARUINFO_GETFILEFULLPATH = 4;

		wchar_t szCurrentFileFullPath[MAX_PATH * 2] = L""; // �G�ۂ�SetHidemaruHandle���Ă΂ꂽ���ɍŏ��ɃX���b�g�ɓ���t�@�C�����B�r���Ńt�@�C�������ς�邱�Ƃ�����̂ŁA�����܂ł��Ȃ������ꍇ�p�r

		if (!IsWindow(hWndHidemaru)) {
			hWndHidemaru = CHidemaruExeExport::GetCurWndHidemaru();   // ���݂̏G�ۃE�B���h�E�̃t�@�C�����𓾂�B
		}
		// ���݂̏G�ۃE�B���h�E�̃t�@�C�����𓾂�B
		LRESULT cwch = SendMessage(hWndHidemaru, WM_HIDEMARUINFO, HIDEMARUINFO_GETFILEFULLPATH, (LPARAM)szCurrentFileFullPath);
		if (cwch > 0) {
			return gcnew String(szCurrentFileFullPath);
		}
		else {
			return L"";
		}
	}

	/// <summary>�t�H�[���̑����ݒ�</summary>
	void SetFormAttribute() {

		// ���ʂȂ����������A�ꉞWait�J�[�\���͎g��Ȃ�
		Application::UseWaitCursor = false;
	}

	/// <summary>Web�u���E�U�����ݒ�</summary>
	void SetWebBrowserAttribulte() {
		try {
			strCurFileFullPath = GetCurrentHidemaruFileName();

			// �G�ۂŕҏW���̃e�L�X�g���ς�����̂���B
			if (strCurFileFullPath->Length > 0) {

				if (strPrvFileFullPath != strCurFileFullPath) {
					strPrvFileFullPath = strCurFileFullPath;
					SetWatcherAttribute();
				}
			}
		}
		catch (Exception^ e) {
			System::Diagnostics::Debug::WriteLine(e->Message);
		}

		try {
			chromeOptions = gcnew ChromeOptions();
			String ^self_full_path = System::Reflection::Assembly::GetExecutingAssembly()->Location;
			String ^self_dir = System::IO::Path::GetDirectoryName(self_full_path);

			chromeService = ChromeDriverService::CreateDefaultService(self_dir);
			chromeService->HideCommandPromptWindow = true;

			driver = gcnew ChromeDriver(chromeService, chromeOptions);
			watcher_Renamed(strCurFileFullPath);
		}
		catch (Exception^ e) {
			System::Diagnostics::Debug::WriteLine(e->Message);
		}
	}

	void SetTimerAttribute() {
		// �P�b�ɂP��̍X�V�ŏ\�����낤�B
		update = gcnew Timer();
		update->Interval = 1000 * 1;
		update->Tick += gcnew EventHandler(this, &HmHtmlPreviewChromeForm::update_Tick);
		update->Start();

		update_Tick(nullptr, nullptr);
	}

	void update_Tick(Object ^sender, EventArgs ^e) {
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

public:
	virtual void Stop() {
		try {
			if (update != nullptr) {
				// �t�H�[���������������B
				update->Stop();
				update = nullptr;
			}
			// watcher����ʂɒǉ����Ă�̂ł�����~�߂�
			if (watcher != nullptr) {
				//�Ď����I��
				watcher->EnableRaisingEvents = false;
				watcher = nullptr;
			}
		}
		catch (Exception^) {}
	}

private:

	/// <summary>�t�@�C���̊Ď�</summary>
	IO::FileSystemWatcher^ watcher = nullptr;

	bool isNavigateChange = false;
	/// <summary>�t�@�C���̊Ď��̑����B
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
				watcher->Filter = "*.*";

				watcher->Changed += gcnew FileSystemEventHandler(this, &HmHtmlPreviewChromeForm::watcher_Changed);
				watcher->IncludeSubdirectories = false;

				//�Ď����J�n����
				watcher->EnableRaisingEvents = true;

				watcher_Renamed(strCurFileFullPath);
			}
		}
		catch (Exception^) {
		}
	}

	/// <summary>�t�@�C���̊Ď��̑����B
	/// �G�ۂ��t�@�C������ύX������A�f�B���N�g����ύX�����肵�Ă��ǐՂł���悤�ɂ��邽�߁B
	/// ���A�t�@�C���̕ۑ����́AwebBrowser�̓��e���X�V����B
	/// </summary>
	void watcher_Changed(Object^ o, FileSystemEventArgs^ e) {
		try {
			System::Diagnostics::Trace::WriteLine(e->ChangeType);
			System::Diagnostics::Trace::WriteLine("\n");

			if (e->ChangeType == IO::WatcherChangeTypes::Changed) {
				// �G�ۂŕҏW���̃e�L�X�g���ς�����̂���B
				if (String::Compare(e->FullPath, strCurFileFullPath, true)) {
					driver->Navigate()->Refresh();
				}
			}
		}
		catch (Exception^) {
		}
	}

	void watcher_Renamed(String^ strCurFileFullPath) {
		// ������ʉ߂���Ƃ������Ƃ̓t�@�C���̃t���p�X���ύX�ƂȂ����̂ł��낤����AUrl�̍X�V
		driver->Url = strCurFileFullPath;
	}

};

