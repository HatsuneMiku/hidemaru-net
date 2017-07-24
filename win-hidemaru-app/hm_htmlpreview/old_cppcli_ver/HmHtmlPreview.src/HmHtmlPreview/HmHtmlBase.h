#pragma once

#include "hidemaruexe_export.h"

using namespace System;
using namespace System::IO;
using namespace System::Drawing;
using namespace System::Windows::Forms;
using namespace System::Net;

CHidemaruExeExport HMEXE;

#pragma comment(lib, "urlmon")

/// �₽��߂�����try_catch�Ȃ̂́A�u���s�ӂɏG�ۖ{�̂����Ă��\�Ȍ���ق��ďI���ł���悤�ɂ��邽�߁v
enum class hmHtmlPreviewMode { Live = 0, File = 1 };


/// <summary> ���[�h�Ɋւ�炸���� </summary>
public ref class HmHtmlBaseForm abstract : System::Windows::Forms::Form
{
public:
	static HmHtmlBaseForm^ form;
protected:
	HWND hWndHidemaru = NULL;
	String^ strCurFileFullPath = "";
	String^ strPrvFileFullPath = "";
	String^ strPrvHmEditTotalText = "";
	hmHtmlPreviewMode mode;
	Timer^ update;
	WebBrowser^ wb;
public:

	HmHtmlBaseForm(HWND hWndHidemaru)
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
		this->Text = "�G�ۗp HmHtmlPreview";

		// �A�Z���u���擾�`�A�C�R���ݒ�
		System::Reflection::Assembly^ prj_assebmly = GetType()->Assembly;
		System::Resources::ResourceManager^ r = gcnew System::Resources::ResourceManager(String::Format("{0}.Resource", prj_assebmly->GetName()->Name), prj_assebmly);
		System::Drawing::Icon^ icon = (System::Drawing::Icon^)(r->GetObject("ie"));
		this->Icon = icon;

		// ���̃t�H�[���T�C�Y
		this->Width = 400;
		this->Height = 600;

		// ���ʂȂ����������A�ꉞWait�J�[�\���͎g��Ȃ�
		Application::UseWaitCursor = false;

		// ���钼�O
		this->FormClosing += gcnew FormClosingEventHandler(this, &HmHtmlBaseForm::form_FormClosing);
	}

	/// <summary>Web�u���E�U�����ݒ�</summary>
	void SetWebBrowserAttribulte() {
		// WebBrowser�I�u�W�F�N�g��z�u
		wb = gcnew WebBrowser();
		wb->Anchor = (AnchorStyles::Left | AnchorStyles::Top | AnchorStyles::Right | AnchorStyles::Bottom);
		wb->DocumentCompleted += gcnew WebBrowserDocumentCompletedEventHandler(this, &HmHtmlBaseForm::wb_DocumentCompleted);
		wb->ClientSize = this->ClientSize;

		// �X�N���v�g�G���[�ł��_�C�A���O�Ƃ��o���Ȃ�
		wb->ScriptErrorsSuppressed = true;

		// �t�H�[���̃T�C�Y�ω��������Ǐ]
		this->SizeChanged += gcnew EventHandler(this, &HmHtmlBaseForm::form_SizeChanged);
		this->Controls->Add(wb);

		wb_DocumentInit();
	}

	/// <summary>�^�C�}�[�����ݒ�B�f�[�^�X�V�̕K�v�������邩�ǂ����̊�b�X�V�B
	/// Live���[�h�ł́A���̃^�C�~���O���f�[�^�X�V�̃^�C�~���O�B
	/// File���[�h�ł́A���̃^�C�~���O�̓��[�U�[���u�Ȃ�̃t�@�C���v���J���Ă���̂��A�̏��̍X�V�̂݁B
	///</summary>
	void SetTimerAttribute() {
		// �P�b�ɂP��̍X�V�ŏ\�����낤�B
		update = gcnew Timer();
		update->Interval = 1000 * 1;
		update->Tick += gcnew EventHandler(this, &HmHtmlBaseForm::update_Tick);
		update->Start();

		update_Tick(nullptr, nullptr);
	}

	void update_Tick(Object ^sender, EventArgs ^e) {
		update_Tick_Implements(sender, e);
	}

	virtual void update_Tick_Implements(Object ^sender, EventArgs ^e) = 0;

	//=========================================================================================
	// �t�H�[��
	//=========================================================================================
	void form_SizeChanged(Object^ o, EventArgs^ e) {
		try {
			// WebBrowser���t�H�[���S�̂ɁB
			wb->ClientSize = this->ClientSize;
		}
		catch (Exception^) {}
	}

	void form_FormClosing(Object^ o, FormClosingEventArgs^ e) {
		try {
			//�Ď����I��
			Stop();

			// �K�x�[�W�R���N�g�����Ă����B
			GC::Collect();
		}
		catch (Exception^) {}
	}

	// wb�̓h�L�������g���X�V�����ƁA�y�[�W�ʒu�������I�Ƀy�[�W�g�b�v�ւƈړ����Ă��܂��B
	// ����ăh�L�������g���X�V����O�ɁA���W��ۑ����Ă����A�h�L�������g�X�V��ɁA���ւƖ߂�����
	Point webBrowserScroll = Point(0, 0);

	// ���͓��e���X�V�����A�Ƃ������Ƃ��蓮�ŁB
	bool isDocumentChanged = false;


	virtual void wb_DocumentInit() = 0;

public:
	virtual void Stop() {
		try {
			if (update != nullptr) {
				// �t�H�[���������������B
				update->Stop();
				update = nullptr;
			}
		}
		catch (Exception^) {}
	}

protected:
	/// <summary> wb�̃h�L�������g�̍X�V�����S�Ɋ���������(�ŏ��̓ǂݍ��ݎ�����������Ƃ����ɗ���) </summary>
	void wb_DocumentCompleted(Object^ sender, WebBrowserDocumentCompletedEventArgs^ e)
	{
		try {
			if (isDocumentChanged) {
				isDocumentChanged = false;
				// �ێ����Ă������W�ւƃX�N���[��
				wb->Document->Window->ScrollTo(webBrowserScroll.X, webBrowserScroll.Y);
				// ����߂�
				CoInternetSetFeatureEnabled(FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_PROCESS, false);
			}
		}
		catch (Exception^) {}
	}
};

