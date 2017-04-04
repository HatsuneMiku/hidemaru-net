#pragma once

#include "HmHtmlBase.h"

public ref class HmHtmlLiveForm : public HmHtmlBaseForm {
public:
	HmHtmlLiveForm(HWND hWndHidemaru) : HmHtmlBaseForm(hWndHidemaru) {	}

protected:
	/// <summary>�t�H�[���\������WebBrowser�ɗ������B
	/// Live���[�h�̎��́ATick�ł���Ă邱�ƂƓ������Ƃ�����B
	///</summary>
	virtual  void wb_DocumentInit() override {
		try {
			update_Tick(nullptr, nullptr);
		}
		catch (Exception^) {}
	}

	/// <summary>Live���[�h�p�̃^�C�}�[�C�x���g
	/// ���W��ۑ��A���΃f�B���N�g���̓��e���΃f�B���N�g���ւƏC���A���������āA�e�L�X�g���X�V����B
	/// �e�L�X�g���X�V���ꂽ��́AwebBrowser_Compo
	///</summary>
	virtual void update_Tick_Implements(Object ^sender, EventArgs ^e) override {
		try {
			strCurFileFullPath = GetCurrentHidemaruFileName();

			// ���݂̕ҏW�̓��e���擾
			String^ curHmEditTotalText = gcnew String(HMEXE.GetTotalText().c_str());

			// �e�L�X�g���ω��������A�������́A�t�@�C�������ω��������ɍX�V
			if (strPrvHmEditTotalText != curHmEditTotalText || strCurFileFullPath != strPrvFileFullPath) {
				strPrvHmEditTotalText = curHmEditTotalText;

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

				// �t�@�C�������L���ł���Ȃ�΁A���΂�ϊ�����
				if (strCurFileFullPath->Length > 0) {
					// ���΁����
					curHmEditTotalText = RelativeToAbsolute(curHmEditTotalText);
				}

				// ���̏���
				CoInternetSetFeatureEnabled(FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_PROCESS, true);

				// �e�L�X�g�X�V
				wb->DocumentText = curHmEditTotalText;
				isDocumentChanged = true;

			}
		}
		catch (Exception^ err) {
		}
	}


private:
	/// <summary>
	/// ������Live���[�h�̂ݒʉ߂���B
	/// ���̂̂Ȃ��t�@�C���̏ꍇ�A�摜�⃊���N�͑��΃A�h���X�͕s�\�Ȃ̂ŁA��΃A�h���X�ւƒu��������B
	/// </summary>
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
				if (head) {
					head->PrependChild(base);
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
		catch (Exception^) {}

		return html;
	}

};
