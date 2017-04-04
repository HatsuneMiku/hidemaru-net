#include "HmExeExport.h"

using namespace System;
using namespace System::IO;
using namespace System::Drawing;
using namespace System::Windows::Forms;
using namespace System::Net;

CHidemaruExeExport HMEXE;

#pragma comment(lib, "urlmon")


public ref class HmMarkDownPreviewForm : System::Windows::Forms::Form
{
public:
	static HmMarkDownPreviewForm^ form;
private:
	HWND hWndHidemaru = NULL;
	String^ strCurFileFullPath = "";
	String^ strPrvFileFullPath = "";
	String^ strPrvHmEditTotalText = "";
public:
	SplitContainer^ sc;

	Timer^ update;
	WebBrowser^ wb;
	RichTextBox^ tb;

public:

	HmMarkDownPreviewForm(HWND hWndHidemaru)
	{
		this->hWndHidemaru = hWndHidemaru;

		SetFormAttribute();
		SetTextBoxAttribute();
		SetWebBrowserAttribulte();
		SetTimerAttribute();
	}

	void SetSplitContainer() {
		sc = gcnew SplitContainer();
		sc->BorderStyle = BorderStyle::FixedSingle;
		this->Controls->Add(sc);
		sc->Anchor = (AnchorStyles::Left | AnchorStyles::Top | AnchorStyles::Right | AnchorStyles::Bottom);

		sc->SplitterMoved += gcnew SplitterEventHandler(this, &HmMarkDownPreviewForm::sc_SplitterMoved);
	}

	String^ GetCurrentHidemaruFileName() {
		strPrvFileFullPath = strCurFileFullPath;
		const int WM_HIDEMARUINFO = WM_USER + 181;
		const int HIDEMARUINFO_GETFILEFULLPATH = 4;

		wchar_t szCurrentFileFullPath[MAX_PATH * 2] = L""; // �G�ۂ�SetHidemaruHandle���Ă΂ꂽ���ɍŏ��ɃX���b�g�ɓ���t�@�C�����B�r���Ńt�@�C�������ς�邱�Ƃ�����̂ŁA�����܂ł��Ȃ������ꍇ�p�r

														   // ���݂̏G�ۃE�B���h�E�̃t�@�C�����𓾂�B
		LRESULT cwch = SendMessage(hWndHidemaru, WM_HIDEMARUINFO, HIDEMARUINFO_GETFILEFULLPATH, (LPARAM)szCurrentFileFullPath);
		if (cwch > 0) {
			return gcnew String(szCurrentFileFullPath);
		}
		else {
			return L"";
		}
	}


	void SetFormAttribute() {

		SetSplitContainer();

		this->Text = "�G�ۗp HmMarkDownPreview";

		// �A�Z���u���擾�`�A�C�R���ݒ�
		System::Reflection::Assembly^ prj_assebmly = GetType()->Assembly;
		System::Resources::ResourceManager^ r = gcnew System::Resources::ResourceManager(String::Format("{0}.Resource", prj_assebmly->GetName()->Name), prj_assebmly);
		System::Drawing::Icon^ icon = (System::Drawing::Icon^)(r->GetObject("md"));
		this->Icon = icon;

		// ���̃t�H�[���T�C�Y
		this->Width = 600;
		this->Height = 600;
		sc->ClientSize = this->ClientSize;

		// ���ʂȂ����������A�ꉞWait�J�[�\���͎g��Ȃ�
		Application::UseWaitCursor = false;

		// ���钼�O
		this->FormClosing += gcnew FormClosingEventHandler(this, &HmMarkDownPreviewForm::form_FormClosing);
	}

	void SetTextBoxAttribute() {
		tb = gcnew RichTextBox();
		tb->Multiline = true;
		tb->ScrollBars = ::RichTextBoxScrollBars::Vertical;
		sc->Panel1->Controls->Add(tb);
		tb->ClientSize = sc->Panel1->ClientSize;
	}

	void SetWebBrowserAttribulte() {
		// WebBrowser�I�u�W�F�N�g��z�u
		wb = gcnew WebBrowser();
		wb->Anchor = (AnchorStyles::Left | AnchorStyles::Top | AnchorStyles::Right | AnchorStyles::Bottom);
		wb->DocumentCompleted += gcnew WebBrowserDocumentCompletedEventHandler(this, &HmMarkDownPreviewForm::wb_DocumentCompleted);

		// �E���ɔz�u�B�ڂ����ς�
		wb->ClientSize = sc->Panel2->ClientSize;

		// �X�N���v�g�G���[�ł��_�C�A���O�Ƃ��o���Ȃ�
		wb->ScriptErrorsSuppressed = true;

		// �t�H�[���̃T�C�Y�ω��������Ǐ]
		this->SizeChanged += gcnew EventHandler(this, &HmMarkDownPreviewForm::form_SizeChanged);
		sc->Panel2->Controls->Add(wb);
		

	}

	void SetTimerAttribute() {
		// �P�b�ɂP��̍X�V�ŏ\�����낤�B
		update = gcnew Timer();
		update->Interval = 1000 * 1;
		update->Tick += gcnew EventHandler(this, &HmMarkDownPreviewForm::update_Tick);
		update->Start();

		update_Tick(nullptr, nullptr);
	}

	//=========================================================================================
	// �t�H�[��
	//=========================================================================================
private:

	void form_SizeChanged(Object^ o, EventArgs^ e) {
		try {
			sc->ClientSize = this->ClientSize;

			// �����̃T�C�Y����
			tb->ClientSize = sc->Panel1->ClientSize;

			// WebBrowser���E���S�̂ɁB
			wb->ClientSize = sc->Panel2->ClientSize;

		}
		catch (Exception^) {}
	}

	void sc_SplitterMoved(Object^ o, SplitterEventArgs^ e) {
		form_SizeChanged(nullptr, nullptr);
	}

	void form_FormClosing(Object^ o, FormClosingEventArgs^ e) {
		try {
			// �t�H�[���������������B
			update->Stop();
			update = nullptr;

			// �K�x�[�W�R���N�g�����Ă����B
			GC::Collect();
		}
		catch (Exception^) {}
	}

	Point webBrowserScroll = Point(0, 0);
	bool isDocumentChanged = false;
	//=========================================================================================
	// �^�C�}�[�C�x���g
	//=========================================================================================
	void update_Tick(Object ^sender, EventArgs ^e)
	{
		try {
			String^ curHmEditTotalText = gcnew String(HMEXE.GetTotalText().c_str());

			strCurFileFullPath = GetCurrentHidemaruFileName();

			// �e�L�X�g���ω��������A�������́A�t�@�C�������ω��������ɍX�V
			if (strPrvHmEditTotalText != curHmEditTotalText || strCurFileFullPath != strPrvFileFullPath) {

				strPrvHmEditTotalText = curHmEditTotalText;

				Markdown^ md = gcnew Markdown();

				// ���݂̕ҏW�̓��e���擾
				String^ curHmHTMLTotalText = md->Transform(curHmEditTotalText);

				if (wb->Document != nullptr)
				{
					// ��i���̇@�D
					// Document->Body������p�^�[���B����ł͎��s����Ƃ�������B
					webBrowserScroll.X = wb->Document->Body->ScrollLeft;
					webBrowserScroll.Y = wb->Document->Body->ScrollTop;

					// ��i���̇A�D
					// HTML�G�������g��Scroll�ʒu�����ɍs���p�^�[���B����������s����Ƃ�������B
					if (webBrowserScroll.Y == 0) {
						webBrowserScroll.X = wb->Document->GetElementsByTagName("HTML")[0]->ScrollLeft;
						webBrowserScroll.Y = wb->Document->GetElementsByTagName("HTML")[0]->ScrollTop;
					}
				}

				// �e�L�X�g�X�V
				tb->Text = curHmHTMLTotalText;

				// �t�@�C�������L���ł���Ȃ�΁A���΂�ϊ�����
				if (strCurFileFullPath->Length > 0) {
					// ���΁����
					curHmHTMLTotalText = RelativeToAbsolute(curHmHTMLTotalText);
				}

				// ���̏���
				CoInternetSetFeatureEnabled(FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_PROCESS, true);

				// �e�L�X�g�X�V
				wb->DocumentText = curHmHTMLTotalText;
				isDocumentChanged = true;
			}
		}
		catch (Exception^ err) {
			System::Diagnostics::Debug::WriteLine(err->Message + "\n");
		}
	}

	// �e�L�X�g�X�V������������(�ŏ��̓ǂݍ��ݎ��������ɗ���)
	void wb_DocumentCompleted(Object^ sender, WebBrowserDocumentCompletedEventArgs^ e)
	{
		try {
			if (isDocumentChanged) {
				isDocumentChanged = false;

				// ����߂�
				CoInternetSetFeatureEnabled(FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_PROCESS, false);

				// �ێ����Ă������W�ւƃX�N���[��
				wb->Document->Window->ScrollTo(webBrowserScroll.X, webBrowserScroll.Y);
			}
		}
		catch (Exception^) {}
	}

	//=========================================================================================
	// ���̂̂Ȃ��t�@�C���̏ꍇ�A�摜�⃊���N�͑��΃A�h���X�͕s�\�Ȃ̂ŁA��΃A�h���X�ւƒu��������B
	//=========================================================================================
	String^ RelativeToAbsolute(String^ html) {

		try {
			String^ fn = strCurFileFullPath;
			fn = fn->Replace("\\", "/");
			String ^strBaseUrl = fn;
			HtmlAgilityPack::HtmlDocument ^doc = gcnew HtmlAgilityPack::HtmlDocument();
			doc->LoadHtml(html);

			bool isExistBaseHref = false;
			for each (auto base in doc->DocumentNode->Descendants("base"))
			{
				if (base->Attributes["href"]->Value->Length > 0) {
					isExistBaseHref = true;
				}
				else {

				}
			}

			// base href�̎w�肪�����̂ł���΁A���݂̊J���Ă���t�@�C���̃f�B���N�g���������ɂ��Ă������ƂŁA���΃f�B���N�g�����t�H���[����B
			if (!isExistBaseHref) {
				String^ basedir = System::IO::Path::GetDirectoryName(strBaseUrl);
				HtmlAgilityPack::HtmlNode^ base = HtmlAgilityPack::HtmlNode::CreateNode("<base href=''>");
				base->Attributes["href"]->Value = basedir + "\\";

				// Head�^�O������΂����ɂ���
				HtmlAgilityPack::HtmlNode^ head = doc->DocumentNode->SelectSingleNode("/html/head");
				HtmlAgilityPack::HtmlNode^ html = doc->DocumentNode->SelectSingleNode("/html");
				if (head) {
					head->PrependChild(base);
				}
				else if (html) {
					html->PrependChild(base);
				}
				// Head�^�O���Ȃ��Ȃ�A�g�b�v�ɂ�����������Ȃ����낤
				else {
					doc->DocumentNode->PrependChild(base);
				}
			}

			/*
			if (!isExistBaseHref) {
				auto objBaseURI = gcnew Uri(strBaseUrl);
				for each (auto img in doc->DocumentNode->Descendants("img"))
				{
				img->Attributes["src"]->Value = (gcnew Uri(objBaseURI, img->Attributes["src"]->Value))->AbsoluteUri;
				}

				for each (auto a in doc->DocumentNode->Descendants("a"))
				{
				a->Attributes["href"]->Value = (gcnew Uri(objBaseURI, a->Attributes["href"]->Value))->AbsoluteUri;
				}

				for each (auto script in doc->DocumentNode->Descendants("script"))
				{
				script->Attributes["src"]->Value = (gcnew Uri(objBaseURI, script->Attributes["src"]->Value))->AbsoluteUri;
				}

				for each (auto link in doc->DocumentNode->Descendants("link"))
				{
				link->Attributes["href"]->Value = (gcnew Uri(objBaseURI, link->Attributes["href"]->Value))->AbsoluteUri;
				}
			}
			*/

			StringWriter ^writer = gcnew StringWriter();
			doc->Save(writer);

			String ^newHtml = writer->ToString();
			return newHtml;
		}
		catch (Exception^ err) {
		}

		return html;
	}
};


//------------------------------------------------------------------------------------
#define MACRO_DLL extern "C" __declspec(dllexport)


MACRO_DLL intptr_t Show(HWND hWndHidemaru) {
	// �C���X�^���X���t�H�[�����g�ɕێ������āA�t�H�[����\���B
	HmMarkDownPreviewForm::form = gcnew HmMarkDownPreviewForm(hWndHidemaru);
	HmMarkDownPreviewForm::form->Show();

	return TRUE;
}

MACRO_DLL intptr_t Dispose() {

	try {
		if (HmMarkDownPreviewForm::form != nullptr) {
			// ����߂�
			CoInternetSetFeatureEnabled(FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_PROCESS, false);

			// ��Ƀ^�C�}�[���X�g�b�v
			HmMarkDownPreviewForm::form->update->Stop();
			HmMarkDownPreviewForm::form->update->Enabled = false;

			// ����Null
			HmMarkDownPreviewForm::form->Close();
			HmMarkDownPreviewForm::form = nullptr;

			// �G�ۂ�����O�܂łɉ\�Ȍ���}�l�[�W�h�̃C���X�^���X��������ĕs���G���[���o�ɂ�����ԂւƎ����Ă����B
			GC::Collect();
			GC::WaitForPendingFinalizers();
		}
	}
	catch (Exception^) {}

	return TRUE;
}


MACRO_DLL intptr_t DllDetachFunc_After_Hm866() {
	return Dispose();
}