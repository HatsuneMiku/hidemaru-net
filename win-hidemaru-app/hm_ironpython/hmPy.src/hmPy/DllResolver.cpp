//------------------------------------------------------------------------------------------------
#include <windows.h>
#include "string_converter.h"

using namespace System;
using namespace System::Reflection;
static Assembly^ CurrentDomain_AssemblyResolve(Object^ sender, ResolveEventArgs^ args) {
	try
	{
		auto requestingAssembly = args->RequestingAssembly;
		auto requestedAssembly = gcnew AssemblyName(args->Name);

		// IronPython.dll�����߂��Ă���A�Ȃ����APublic Key Token�������Ɛݒ肳��Ă���B
		if (requestedAssembly->Name->ToUpper() == "IRONPYTHON" && requestedAssembly->GetPublicKeyToken()->ToString() != String::Empty ) {
			System::Diagnostics::Trace::WriteLine(args->Name);
			wchar_t message[256] = L"";

			wsprintf
			(
				message,
				L"http://IronPython.net/" L"����A\n"
				L"IronPython %d.%d.%d���u.msi�C���X�g�[���[���g���āv�C���X�g�[�����Ă��������B\n"
				L"(������ : �o�[�W�����͐��m�ɁA�u%d.%d.%d�v�̕K�v��������܂�)",
				requestedAssembly->Version->Major, requestedAssembly->Version->Minor, requestedAssembly->Version->Build,
				requestedAssembly->Version->Major, requestedAssembly->Version->Minor, requestedAssembly->Version->Build
			);

			MessageBox(NULL, message, L"IronPython.dll��������Ȃ�!!", MB_ICONERROR);
		}
		else {

		}
	}
	catch (...)
	{
		return nullptr;
	}
	return nullptr;
}


struct __declspec(dllexport) DllAssemblyResolver {
	DllAssemblyResolver() {
		AppDomain::CurrentDomain->AssemblyResolve += gcnew System::ResolveEventHandler(&CurrentDomain_AssemblyResolve);
	}

	~DllAssemblyResolver() {
		AppDomain::CurrentDomain->AssemblyResolve -= gcnew System::ResolveEventHandler(&CurrentDomain_AssemblyResolve);
	}
};



//----------���������l�C�e�B�u
#pragma unmanaged 
// �ق��̃}�l�[�W�h�R�[�h����Ɏ��s�����邽�߁B
// ��ɂ������`���Ď��s�B�A���}�l�[�W�h
DllAssemblyResolver asmresolver;
#pragma managed
//----------���������l�C�e�B�u

//------------------------------------------------------------------------------------------------
