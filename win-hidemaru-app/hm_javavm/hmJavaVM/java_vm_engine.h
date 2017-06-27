#pragma once
#include <jni.h>

#include <tchar.h>
#include <windows.h>
#include <regex>
#include <string>


using namespace std;


class CJavaVMEngine {

private:
	//-------------------------------------------------------------------------
	// jvm.dll��Java Virtual Machine�BJava����ɓ��肳��Ȃ��AJava Virtual Machine���̂��́B
	static const int jni_version = JNI_VERSION_1_8;

	static HMODULE hJavaVMDll;

	static JavaVM *jvm;
	static JNIEnv* env;

public:
	static void CreateVM();
	static void DestroyVM();

public:
	//-------------------------------------------------------------------------
	// Java Virtual Machine�͓ǂݍ��ݍς݂ł��L�����ǂ����B
	static bool IsValid();

	// �Y���N���X�̊Y��static���\�b�h�����s���Ă݂悤�B
	static bool GetStaticActionMethod(wstring class_name, wstring method_name);

private:
	static wstring GetErrorMessage();
};


