#include <windows.h>
#include <tchar.h>


using namespace System;
using namespace System::Reflection;
static Assembly^ CurrentDomain_AssemblyResolve(Object^ sender, ResolveEventArgs^ args) {
	try
	{
		auto requestingAssembly = args->RequestingAssembly;
		auto requestedAssembly = gcnew AssemblyName(args->Name);

		// ����dll���̂�u���Ă���t�H���_�ɓǂݍ��ݑΏۂ̃A�Z���u�������邩������Ȃ��B
		String^ self_full_path = System::Reflection::Assembly::GetExecutingAssembly()->Location;
		String^ self_dir = System::IO::Path::GetDirectoryName(self_full_path);

		auto targetfullpath = self_dir + R"(\)" + requestedAssembly->Name + ".dll";

		if (System::IO::File::Exists(targetfullpath))
		{
			return Assembly::LoadFile(targetfullpath);
		}

		// ���̂悤�ȃt���p�X���w�肳��Ă���ꍇ(�t���p�X���w�肵��������)
		targetfullpath = requestedAssembly->Name;
		if (System::IO::File::Exists(targetfullpath))
		{
			return Assembly::LoadFile(targetfullpath);
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

// ��������͑S���}�l�[�W�h

//----------���������l�C�e�B�u
#pragma unmanaged 
// �ق��̃}�l�[�W�h�R�[�h����Ɏ��s�����邽�߁B
// ��ɂ������`���Ď��s�B�A���}�l�[�W�h
DllAssemblyResolver asmresolver;
#pragma managed
//----------���������l�C�e�B�u

// ��������͑S���}�l�[�W�h

// ���]���o�[�̌�ɁA�t�H�[���̒�`�s�̃R�[�h������悤�ɂ��Ȃ���΂Ȃ�Ȃ��B���]���o�[����������

#include "HmHtmlPreviewChrome.h"

//------------------------------------------------------------------------------------
#define MACRO_DLL extern "C" __declspec(dllexport)


// �}�N������Ă΂��
MACRO_DLL intptr_t Show(HWND hWndHidemaru, int mode) {

	HmHtmlPreviewChromeForm::form = gcnew HmHtmlPreviewChromeForm(hWndHidemaru);

	// �t�H�[���̕\���̕K�v���͂Ȃ��B// chrome�u���E�U�̕��őΉ�����
	// HmHtmlBaseForm::form->Show();

	return TRUE;
}

// �G�ۂ̊Y���v���Z�X������Ƃ�
MACRO_DLL intptr_t Dispose() {

	try {
		// �܂��c���Ă�����(�t�H�[�����蓮�ŕ��Ă�����A�c���Ă��Ȃ�)
		if (HmHtmlPreviewChromeForm::form != nullptr) {

			//�Ď����I��
			HmHtmlPreviewChromeForm::form->Stop();

			// ����Null
			HmHtmlPreviewChromeForm::form->Close();
			HmHtmlPreviewChromeForm::form = nullptr;

			// �G�ۂ�����O�܂łɉ\�Ȍ���}�l�[�W�h�̃C���X�^���X��������ĕs���G���[���o�ɂ�����ԂւƎ����Ă����B
			GC::Collect();
			GC::WaitForPendingFinalizers();

		}
	}
	catch (Exception^) {}

	return TRUE;
}

// �G�ۂ̊Y���v���Z�X������Ƃ�
MACRO_DLL intptr_t DllDetachFunc_After_Hm866() {
	return Dispose();
}