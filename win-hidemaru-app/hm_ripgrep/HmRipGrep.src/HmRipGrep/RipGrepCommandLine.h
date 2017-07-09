#pragma once

#pragma comment(lib, "winmm")

namespace HmRipGrep {

	wstring g_data = L"";

	ref class RipGrepCommanLine {
		System::Diagnostics::Process^ p;
		String^ searcText = "";
		String^ dirText = "";
	public:
		RipGrepCommanLine(String^ searcText, String^ dirText, Dictionary<String^, Boolean>^ prev) {

			this->searcText = searcText;
			this->dirText = dirText;

			// ���v�����Ȃ珑������
			if (!CHidemaruExeExport::Hidemaru_CheckQueueStatus()) {
				CHidemaruExeExport::SetTotalText(L"");
			}
		}
	public:
	
		bool isStop = false;
		void Stop() {
			if (p) {
				try {
					isStop = true;
					p->Close();
					p->Kill();
					delete p;
				}
				catch (Exception ^e) {

				}
			}
		}
		bool IsStop() {
			return isStop;
		}

	public:
		void Grep()
		{
			try {
				//Process�I�u�W�F�N�g���쐬����
				p = gcnew System::Diagnostics::Process();
				//�N������t�@�C�����w�肷��
				p->StartInfo->FileName = gcnew String(CSelfDllInfo::GetSelfModuleDir().c_str()) + L"\\IronRg.exe";

				List<String^>^ list = gcnew List<String^>();
				list->Add(Regex::Escape(this->searcText));
				list->Add(this->dirText);

				String^ arg_line = EncodeCommandLineValues(list);
				p->StartInfo->Arguments = arg_line;

				p->StartInfo->CreateNoWindow = true;
				p->StartInfo->UseShellExecute = false;

				//�C�x���g�n���h���̒ǉ�
				p->StartInfo->RedirectStandardError = true;
				p->StartInfo->RedirectStandardOutput = true;

				p->ErrorDataReceived += gcnew System::Diagnostics::DataReceivedEventHandler(this, &RipGrepCommanLine::proc_ErrorDataReceived);
				p->OutputDataReceived += gcnew System::Diagnostics::DataReceivedEventHandler(this, &RipGrepCommanLine::proc_OutputDataReceived);
				p->StartInfo->StandardOutputEncoding = Encoding::UTF8;
				p->StartInfo->StandardErrorEncoding = Encoding::UTF8;

				p->Exited += gcnew System::EventHandler(this, &RipGrepCommanLine::proc_Exited);
				//�v���Z�X���I�������Ƃ��� Exited �C�x���g�𔭐�������
				p->EnableRaisingEvents = true;

				//�N������
				p->Start();
				p->BeginOutputReadLine();

				startTime = timeGetTime();

				p->WaitForExit();

				try {
					if (p) {
						p->Close();
						p->Kill();
					}
				}
				catch (Exception^ e) {

				}
			}
			catch (Exception^ e) {
				System::Diagnostics::Trace::WriteLine(e->Message);
			}
		}

	private:
		String^ alldata = "";
		DWORD startTime = 0;
		Regex^ r = gcnew Regex(R"MATCH(^[\s\S]+?:\d+:)MATCH");
		const int nInterValMilliSecond = 1000;
		Mutex^ mut = gcnew Mutex();

		void AddTotalText(wstring data) {
			if (isStop) {
				return;
			}

			try {
				if (mut->WaitOne(500)) {

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

					// ���b�N���
					mut->ReleaseMutex();
				}

			}
			catch (Exception ^e) {
				System::Diagnostics::Trace::WriteLine(e->Message);
			}

		}

		int nRecieveCounter = 0;
		void proc_OutputDataReceived(Object^ sender, System::Diagnostics::DataReceivedEventArgs^ e)
		{
			if (isStop) {
				return;
			}
			String^ data = e->Data;
			if (data == nullptr) {
				return;
			}

			try {
				alldata += data + L"\n";

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

		void proc_ErrorDataReceived(Object^ sender, System::Diagnostics::DataReceivedEventArgs^ e)
		{
			// P_OutputDataReceived(sender, e);
		}

		void proc_Exited(Object^ sender, EventArgs^ e)
		{
			try {
				wstring data = String_to_wstring(alldata);
				alldata = "";

				AddTotalText(data);
				AddTotalText(L"�����I��");
			}
			catch (Exception^ e) {
				System::Diagnostics::Trace::WriteLine(e->Message);
			}
		}
		/// 
		/// �R�}���h���C������ 1 ���G���R�[�h
		/// 
	public:
		String^ EncodeCommandLineValue(String^ value)
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
		initonly Regex^ _commandLineEscapePattern = gcnew Regex("(\\\\*)\"");
		initonly Regex^ _lastBackSlashPattern = gcnew Regex(R"((\\ + )$)");

		/// 
		/// �R�}���h���C�������������G���R�[�h���āA�X�y�[�X�Ō���
		/// 
	public:
		String^ EncodeCommandLineValues(IEnumerable<String^>^ values)
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