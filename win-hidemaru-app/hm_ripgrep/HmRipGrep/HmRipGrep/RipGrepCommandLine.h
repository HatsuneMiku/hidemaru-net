#pragma once

#pragma comment(lib, "winmm")

namespace HmRipGrep {

	wstring g_data = L"";

	ref class RipGrepCommanLine {
		static System::Diagnostics::Process^ p;
	public:
		static void Clear() {
			if (hs != nullptr) {
				hs->Clear();
				CHidemaruExeExport::SetTotalText(L"");
			}
		}
		static bool isStop = false;
		static void Stop() {
			if (p) {
				isStop = true;
				p->Close();

				delete p;
			}
		}

		static bool m_is_add = false;
		static void Grep(String^ searcText, bool is_add)
		{
			try {
				m_is_add = is_add;

				// �ǉ����[�h�Ȃ̂�Stop�������Ă�����A�������Ȃ�
				if (m_is_add && isStop) {
					return;
				}

				isStop = false;

				//Process�I�u�W�F�N�g���쐬����
				p = gcnew System::Diagnostics::Process();
				//�N������t�@�C�����w�肷��
				p->StartInfo->FileName = gcnew String(CSelfDllInfo::GetSelfModuleDir().c_str()) + L"\\rg.exe";

				List<String^>^ list = gcnew List<String^>();
				if (m_is_add) {
					list->Add("-E");
					list->Add("utf8");
				}
				list->Add("-n");
				list->Add("-e");
				list->Add(Regex::Escape(searcText));
				list->Add("-S");
				list->Add(R"(C:\usr\web)");

				String^ arg_line = EncodeCommandLineValues(list);
				p->StartInfo->Arguments = arg_line;

				p->StartInfo->CreateNoWindow = true;
				p->StartInfo->UseShellExecute = false;

				//�C�x���g�n���h���̒ǉ�
				p->StartInfo->RedirectStandardError = true;
				p->StartInfo->RedirectStandardOutput = true;

				p->ErrorDataReceived += gcnew System::Diagnostics::DataReceivedEventHandler(&P_ErrorDataReceived);
				p->OutputDataReceived += gcnew System::Diagnostics::DataReceivedEventHandler(&P_OutputDataReceived);
				p->StartInfo->StandardOutputEncoding = Encoding::UTF8;
				p->StartInfo->StandardErrorEncoding = Encoding::UTF8;

				p->Exited += gcnew System::EventHandler(&p_Exited);
				//�v���Z�X���I�������Ƃ��� Exited �C�x���g�𔭐�������
				p->EnableRaisingEvents = true;

				//�N������
				p->Start();
				p->BeginOutputReadLine();

				startTime = timeGetTime();

				p->WaitForExit();

				p->Close();
			}
			catch (Exception^ e) {
				System::Diagnostics::Trace::WriteLine(e->Message);
			}
		}

	private:
		static String^ alldata = "";
		static int cnt = 0;
		static DWORD startTime = 0;
		static Dictionary<String^, Boolean>^ hs = gcnew Dictionary<String^, Boolean>();
		static Regex^ r = gcnew Regex(R"MATCH(^[\s\S]+?:\d+:)MATCH");
		static const int nInterValMilliSecond = 700;

		static void AddTotalText(wstring data) {
			if (isStop) {
				return;
			}
			if (CHidemaruExeExport::Hidemaru_CheckQueueStatus()) {
				Threading::Thread::Sleep(50);
				if (CHidemaruExeExport::Hidemaru_CheckQueueStatus()) {
					Threading::Thread::Sleep(50);
					if (CHidemaruExeExport::Hidemaru_CheckQueueStatus()) {
						Threading::Thread::Sleep(50);
						if (CHidemaruExeExport::Hidemaru_CheckQueueStatus()) {
							Threading::Thread::Sleep(50);
							if (CHidemaruExeExport::Hidemaru_CheckQueueStatus()) {
								Threading::Thread::Sleep(50);
								if (CHidemaruExeExport::Hidemaru_CheckQueueStatus()) {
									Threading::Thread::Sleep(50);
								}
							}
						}
					}
				}
			}

			// ���v�����Ȃ珑������
			if (!CHidemaruExeExport::Hidemaru_CheckQueueStatus()) {
				CHidemaruExeExport::AddTotalText(data);
			}

		}

		static void P_OutputDataReceived(Object^ sender, System::Diagnostics::DataReceivedEventArgs^ e)
		{
			if (isStop) {
				return;
			}
			String^ data = e->Data;
			if (data == nullptr) {
				return;
			}

			try {

				String^ s = r->Match(data)->Value;
				if (s != nullptr) {
					bool is_must_add = false;
					if (m_is_add) {
						// ���x�́A�܂��o�^����Ă��Ȃ��������ASJIS�ł�f���o��
						if (!hs->ContainsKey(s)) {
							is_must_add = true;
						}
					}
					else {
						hs[s] = true;
						System::Diagnostics::Trace::WriteLine(s);
						is_must_add = true;
					}

					if (is_must_add) {
						alldata += data + L"\n";
					}

				}

				DWORD currentTime = timeGetTime();

				if (currentTime - startTime > nInterValMilliSecond) {
					startTime = currentTime;
					wstring data = String_to_wstring(alldata);
					alldata = "";

					// ���v�����Ȃ珑������
					AddTotalText(data);
				}
			}
			catch (Exception ^e) {
				System::Diagnostics::Trace::WriteLine(e->Message);
			}
		}

		static void P_ErrorDataReceived(Object^ sender, System::Diagnostics::DataReceivedEventArgs^ e)
		{
			P_OutputDataReceived(sender, e);
		}

		static void p_Exited(Object^ sender, EventArgs^ e)
		{
			try {
				wstring data = String_to_wstring(alldata);
				alldata = "";

				AddTotalText(data);
				if (m_is_add) {
					AddTotalText(L"�����I��");
				}
			}
			catch (Exception^ e) {
				System::Diagnostics::Trace::WriteLine(e->Message);
			}

			// �܂��傫�����낤���瑁�߂�
			if (m_is_add) {
				hs->Clear();
			}
		}
		/// 
		/// �R�}���h���C������ 1 ���G���R�[�h
		/// 
	public:
		static String^ EncodeCommandLineValue(String^ value)
		{
			if (String::IsNullOrEmpty(value))
			{
				return "";
			}
			auto containsSpace = value->IndexOfAny(gcnew cli::array<Char> {' ', '\t'}) != -1;

			// �u\�c\"�v���G�X�P�[�v
			// ����Ă邱�Ƃ́A�u"�v���O�́u\�v�̐��� 2�{+1
			value = _commandLineEscapePattern->Replace(value, R"($1\$&)");

			// �X�y�[�X�^�^�u���܂܂��ꍇ�̓f���~�^�ň͂݁A�������u\�v�������ꍇ�A�G�X�P�[�v
			if (containsSpace)
			{
				value = "\"" + _lastBackSlashPattern->Replace(value, "$1$1") + "\"";
			}
			return value;
		}
	private:
		static initonly Regex^ _commandLineEscapePattern = gcnew Regex("(\\\\*)\"");
		static initonly Regex^ _lastBackSlashPattern = gcnew Regex(R"((\\ + )$)");

		/// 
		/// �R�}���h���C�������������G���R�[�h���āA�X�y�[�X�Ō���
		/// 
	public:
		static String^ EncodeCommandLineValues(IEnumerable<String^>^ values)
		{
			if (values == nullptr)
			{
				throw gcnew ArgumentNullException("values");
			}
			String^ ret;
			for each (String^ str in values) {
				str = EncodeCommandLineValue(str);
				ret = ret + str + " ";
			}

			return ret;
		}
	};

}