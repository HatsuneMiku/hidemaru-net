#pragma once

#include <tchar.h>
#include <cstdint>

#define MACRO_DLL extern "C" __declspec(dllexport)


#define intHM_t intptr_t



//========================================================================
/// ���̃t�@�C�����猩�Ă��邽�߂�extern�B���ۂ�.cpp�̕����݂Ă�
//========================================================================
MACRO_DLL intHM_t GetNumVar(const TCHAR *sz_full_var_name);
MACRO_DLL intHM_t SetNumVar(const TCHAR *sz_full_var_name, intHM_t value);
MACRO_DLL const TCHAR * GetStrVar(const TCHAR *sz_full_var_name);
MACRO_DLL intHM_t SetStrVar(const TCHAR *sz_full_var_name, const TCHAR *value);
