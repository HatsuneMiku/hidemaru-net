#include <shlwapi.h>
#include <vector>

#include "java_vm_engine.h"
#include "self_dll_info.h"
#include "convert_string.h"
#include "output_debugstream.h"



using namespace std;

#pragma comment(lib, "shlwapi.lib")

// static�̏�����
HMODULE CJavaVMEngine::hJavaVMDll = NULL;

char CJavaVMEngine::szJavaClassPathBuffer[512] = "-Djava.class.path=";

JavaVM *CJavaVMEngine::jvm = NULL;
JNIEnv* CJavaVMEngine::env = NULL;

bool CJavaVMEngine::HmCalled = false;

wstring CJavaVMEngine::GetHavaVMDllPath() {
	wstring xmlpath = CSelfDllInfo::GetSelfModuleDir() + L"\\hmJavaXM.xml";
	if (PathFileExists(xmlpath.c_str())) {
		return LR"(C:\Program Files (x86)\Java\jdk1.8.0_131\jre\bin\client\jvm.dll)";
	}
	else {
		MessageBox(NULL, L"hmJavaXM.xml������܂���B", L"hmJavaXM.xml������܂���B", NULL);
		return L"";
	}
}

void CJavaVMEngine::CreateVM() {
	if (IsValid()) {
		return;
	}

	// �ǂݎ�����l���p�X�Ƃ��ē��I��jvm.dll���C�u�����[�����[�h
	hJavaVMDll = LoadLibrary(GetHavaVMDllPath().c_str());
	if (!hJavaVMDll) {
		MessageBox(NULL, L"jvm.dll��������܂���B", L"jvm.dll��������܂���B", NULL);
		return;
	}

	using TJNI_CreateJavaVM = decltype(JNI_CreateJavaVM);
	auto pCreateJavaVM = (TJNI_CreateJavaVM *)GetProcAddress(hJavaVMDll, "JNI_CreateJavaVM");
	if (!pCreateJavaVM) {
		MessageBox(NULL, L"jvm.dll��JNI_CreateJavaVM�֐���������܂���B", L"jvm.dll��JNI_CreateJavaVM�֐���������܂���B", NULL);
		return;
	}

	// JNI_CreateJavaVM���Ăяo��
	JavaVMOption options[2];
	options[0].optionString = "-Xms8m-Xmx512m";

//	options[1].optionString = "-Djava.class.path=C:\\usr\\hidemaru;C:\\usr\\hidemaru\\hmJavaVM.jar"; // �Ƃ����`�ɂ���B
	wstring dir = CSelfDllInfo::GetSelfModuleDir();
	string cp932_dir = utf16_to_cp932(dir);
	strcat_s(szJavaClassPathBuffer, cp932_dir.c_str());
	strcat_s(szJavaClassPathBuffer, ";");
	strcat_s(szJavaClassPathBuffer, cp932_dir.c_str());
	strcat_s(szJavaClassPathBuffer, "\\hmJavaVM.jar");
	MessageBoxA(NULL, szJavaClassPathBuffer, szJavaClassPathBuffer, NULL);
	options[1].optionString = szJavaClassPathBuffer;

	JavaVMInitArgs vm_args;
	vm_args.version = jni_version;
	vm_args.options = options;
	vm_args.nOptions = 2;

	MessageBox(NULL, L"OK2", L"OK2", NULL);
	env = NULL;
	jvm = NULL;

	try {
		int isValid = pCreateJavaVM(&jvm, (void**)&env, &vm_args);
		if (isValid < 0) {
			wstring msg = L"JNI_CreateJavaVM Error: " + std::to_wstring(isValid);
			MessageBox(NULL, msg.c_str(), msg.c_str(), NULL);
			return;
		}
	}
	catch (exception e) {
		MessageBoxA(NULL, e.what(), e.what(), NULL);
	}

	MessageBox(NULL, L"OK3", L"OK3", NULL);
}


void CJavaVMEngine::DestroyVM() {

	// �t���[����ƃo�O��̂ŉ������Ȃ�
	return;

	//-------------------------------------------------------------------------
	// �G���W����j�����Ă���c
	//-------------------------------------------------------------------------
	if (jvm) {
		// JVM�j��
		jvm->DestroyJavaVM();
		env = NULL;
	}

	//-------------------------------------------------------------------------
	// dll��j���B
	//-------------------------------------------------------------------------
	if (hJavaVMDll) {
		FreeLibrary(hJavaVMDll);
		hJavaVMDll = NULL;
	}
}


bool CJavaVMEngine::IsValid() {
	return env != NULL;
}


bool CJavaVMEngine::CallStaticEntryMethod(wstring class_name, wstring method_name) {

	// utf16��utf8�ւ̕ϊ�
	string utf8_class_name = utf16_to_utf8(class_name);
	string utf8_method_name = utf16_to_utf8(method_name);

	// Hello�N���X�̃��[�h
	jclass clazz = env->FindClass(utf8_class_name.c_str());
	if (clazz == 0) {
		wstring message = wstring(L"FindClass Error for `") + class_name + wstring(L"`");
		MessageBox(NULL, message.c_str(), message.c_str(), NULL);
		return false;
	}
	// Hello�N���X��main���\�b�h�擾
	jmethodID mid = env->GetStaticMethodID(clazz, utf8_method_name.c_str(), "([Ljava/lang/String;)V" );
	if (mid == 0) {
		wstring message = wstring(L"GetStaticMethodID Error for `static void ") + method_name + wstring(L"(String args[])`");
		MessageBox(NULL, message.c_str(), message.c_str(), NULL);
		return false;
	}
	// main���\�b�h���s
	env->CallStaticVoidMethod(clazz, mid, NULL);

	wstring errormsg = GetErrorMessage();
	if (errormsg.size() > 0) {
		OutputDebugStream(errormsg);
	}

	return true;
}

