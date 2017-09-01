
#include "python_hidemaru_lib.h"


namespace Hidemaru {

	double version() {
		return CHidemaruExeExport::hm_version;
	}

	// �I�u�W�F�N�g�Ƃ��đ����A������ɕϊ����ďo��
	void debuginfo(const py::object message) {
		auto str = py::str(message);
		OutputDebugStream(utf8_to_utf16(str));
	}

	std::string Edit_GetTotalText() {
		wstring utf16_value = CHidemaruExeExport::GetTotalText();
		return utf16_to_utf8(utf16_value);
	}

	BOOL Edit_SetTotalText(const std::string utf8_value) {
		BOOL success = 0;

		auto dll_invocant = CSelfDllInfo::GetInvocantString();

		wstring utf16_value = utf8_to_utf16(utf8_value);
		PushStrVar(utf16_value.data());
		wstring cmd =
			L"begingroupundo;\n"
			L"selectall;\n"
			L"insert dllfuncstrw( " + dll_invocant + L"\"PopStrVar\" );\n"
			L"endgroupundo;\n";
		success = CHidemaruExeExport::EvalMacro(cmd);

		return success;
	}

	std::string Edit_GetSelectedText() {
		wstring utf16_value = CHidemaruExeExport::GetSelectedText();
		return utf16_to_utf8(utf16_value);
	}

	BOOL Edit_SetSelectedText(const std::string utf8_value) {
		BOOL success = 0;

		auto dll_invocant = CSelfDllInfo::GetInvocantString();

		wstring utf16_value = utf8_to_utf16(utf8_value);
		PushStrVar(utf16_value.data());
		wstring cmd =
			L"if (selecting) {\n"
			L"insert dllfuncstrw( " + dll_invocant + L"\"PopStrVar\" );\n"
			L"};\n";
		success = CHidemaruExeExport::EvalMacro(cmd);

		return success;
	}

	std::string Edit_GetLineText() {
		wstring utf16_value = CHidemaruExeExport::GetLineText();
		return utf16_to_utf8(utf16_value);
	}

	BOOL Edit_SetLineText(const std::string utf8_value) {
		BOOL success = 0;

		auto dll_invocant = CSelfDllInfo::GetInvocantString();

		auto pos = CHidemaruExeExport::GetCursorPos();

		wstring utf16_value = utf8_to_utf16(utf8_value);
		PushStrVar(utf16_value.data());
		wstring cmd =
			L"begingroupundo;\n"
			L"selectline;\n"
			L"insert dllfuncstrw( " + dll_invocant + L"\"PopStrVar\" );\n"
			L"moveto2 " + std::to_wstring(pos.column) + L", " + std::to_wstring(pos.lineno) + L";\n" +
			L"endgroupundo;\n";
		success = CHidemaruExeExport::EvalMacro(cmd);

		return success;
	}


	py::object Macro_GetVar(const std::string utf8_simbol) {

		wstring utf16_simbol = utf8_to_utf16(utf8_simbol);

		TestDynamicVar.Clear();
		auto dll_invocant = CSelfDllInfo::GetInvocantString();
		wstring cmd =
			L"##_tmp_dll_id_ret = dllfuncw( " + dll_invocant + L"\"SetDynamicVar\", " + utf16_simbol + L");\n"
			L"##_tmp_dll_id_ret = 0;\n";
		BOOL success = CHidemaruExeExport::EvalMacro(cmd);

		// ���l�Ȃ�
		if (TestDynamicVar.type == CDynamicValue::TDynamicType::TypeInteger)
		{
			py::object r = py::cast(TestDynamicVar.num);
			return r;
		}
		// ������Ȃ�
		else {
			string utf8_value = utf16_to_utf8(TestDynamicVar.wstr);
			py::object r = py::cast(utf8_value);
			return r;
		}
	}

	BOOL Macro_SetVar(const std::string utf8_simbol, const py::object value) {
		BOOL success = 0;

		auto dll_invocant = CSelfDllInfo::GetInvocantString();

		wstring utf16_simbol = utf8_to_utf16(utf8_simbol);
		wchar_t start = utf16_simbol[0];
		if (start == L'#') {

			string utf8_value = py::str(value);
			wstring utf16_value = utf8_to_utf16(utf8_value);

			// �����𐔒l�Ƀg���C�B�_���Ȃ�0����B
			intHM_t n = 0;
			try {
				n = (intHM_t)std::stol(utf16_value);
			}
			catch (...) {}

			PushNumVar(n);
			wstring cmd = L" = dllfuncw( " + dll_invocant + L"\"PopNumVar\" );\n";
			cmd = utf16_simbol + cmd;
			success = CHidemaruExeExport::EvalMacro(cmd);
		}
		else if (start == L'$') {

			string utf8_value = py::str(value);
			wstring utf16_value = utf8_to_utf16(utf8_value);

			PushStrVar(utf16_value.data());
			wstring cmd = L" = dllfuncstrw( " + dll_invocant + L"\"PopStrVar\" );\n";
			cmd = utf16_simbol + cmd;
			success = CHidemaruExeExport::EvalMacro(cmd);
		}

		return success;
	}

	BOOL Macro_Eval(const std::string utf8_expression) {
		wstring utf16_expression = utf8_to_utf16(utf8_expression);

		BOOL success = CHidemaruExeExport::EvalMacro(utf16_expression);
		if (success) {
			return TRUE;
		}
		OutputDebugStream(L"�}�N���̎��s�Ɏ��s���܂����B\n");
		OutputDebugStream(L"�}�N�����e:\n");
		OutputDebugStream(utf16_expression);
		return FALSE;
	}

#pragma region
	/*
	class Job {
	public:
	std::string GetName() const { return m_name; }
	void SetName(const std::string& name) { m_name = name; }

	private:
	std::string m_name;
	};

	class Person {
	public:
	std::string GetName() const { return m_name; }
	void SetName(const std::string& name) { m_name = name; }

	std::shared_ptr<Job> GetJob() const { return m_job; }
	void SetJob(const std::shared_ptr<Job>& job) { m_job = job; }

	private:
	std::string m_name;
	std::shared_ptr<Job> m_job;
	};

	namespace py = pybind11;

	PyMODINIT_FUNC PyInit_sample() {
	py::module m("sample", "pybind11 module sample.");

	py::class_<Job, std::shared_ptr<Job>> job(m, "Job");
	job.def(py::init<>()).def_property("name", &Job::GetName, &Job::SetName);

	py::class_<Person, std::shared_ptr<Person>> person(m, "Person");
	person.def(py::init<>())
	.def_property("name", &Person::GetName, &Person::SetName)
	.def_property("job", &Person::GetJob, &Person::SetJob);

	return m.ptr();
	}
	*/
#pragma endregion 
}

PyMODINIT_FUNC PyInit_hidemaru() {
	py::module m("hm", "Hidemaru python module.");

	m.def("version", &Hidemaru::version);
	m.def("debuginfo", &Hidemaru::debuginfo);

	m.def("Edit_GetTotalText", &Hidemaru::Edit_GetTotalText);
	m.def("Edit_SetTotalText", &Hidemaru::Edit_SetTotalText);
	m.def("Edit_GetSelectedText", &Hidemaru::Edit_GetSelectedText);
	m.def("Edit_SetSelectedText", &Hidemaru::Edit_SetSelectedText);
	m.def("Edit_GetLineText", &Hidemaru::Edit_GetLineText);
	m.def("Edit_SetLineText", &Hidemaru::Edit_SetLineText);

	m.def("Macro_GetVar", &Hidemaru::Macro_GetVar);
	m.def("Macro_SetVar", &Hidemaru::Macro_SetVar);

	m.def("Macro_Eval", &Hidemaru::Macro_Eval);

#pragma region
	/*
	py::class_<Job, std::shared_ptr<Job>> job(m, "Job");
	job.def(py::init<>()).def_property("name", &Job::GetName, &Job::SetName);

	py::class_<Person, std::shared_ptr<Person>> person(m, "Person");
	person.def(py::init<>())
	.def_property("name", &Person::GetName, &Person::SetName)
	.def_property("job", &Person::GetJob, &Person::SetJob);
	*/
#pragma endregion 

	return m.ptr();
}

