#pragma once
#include <jni.h>

#include <tchar.h>
#include <windows.h>
#include <regex>
#include <string>


using namespace std;


class CJavaVMEngine {

private:

	static HMODULE hJavaVMDll;
private:
	//-------------------------------------------------------------------------
	// jvm.dll��Java Virtual Machine�BJava����ɓ��肳��Ȃ��AJava Virtual Machine���̂��́B
	static const int jni_version = JNI_VERSION_1_8;

	static JavaVM *jvm;
	static JNIEnv* env;

private:
	//========================================================================
	/// jvm.dll����Export����Ă��邢�����̊֐��Q�̌^�ێ�
	//========================================================================

public:



public:
	static void CreateVM();
	static void DestroyVM();

private:

public:
	//-------------------------------------------------------------------------
	// �G���W���͗L��(�ǂݍ��ݍ�)���ǂ����B
	static bool IsValid();

	static bool GetStaticActionMethod(wstring class_name, wstring method_name);
	/*
	// �Ώۂ̕�����̓p�[��Eval�G���[�ۂ��B
	BOOL IsReturnBufferSeemsEvalError(char *utf8_buffer);

	struct TEvalErrorInfo {
		bool HasError;		// �G���[�����������ǂ����B
		int lineno;			// �G���[�s
		int column;			// �G���[�ʒu
		wstring errorlinecode;	// �G���[���o���s�̕�����
	};

	// �G���[�̈ʒu�Ə��𕪐�
	TEvalErrorInfo GetEvalErrorInfo(wstring strAllSrcCode, char *utf8_buffer);
	*/
};


