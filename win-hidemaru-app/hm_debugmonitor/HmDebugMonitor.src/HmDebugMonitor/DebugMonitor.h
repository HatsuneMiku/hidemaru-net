#pragma once

#include <windows.h>
#include <vector>

using namespace std;

void InitDebugMonitor(HWND hEdit); // �����֐�

// �X���b�h�p�֐��̐錾
unsigned __stdcall ThreadExternalProcDebugMonitor(void *);

extern BOOL isDoingRecieveFlag;
extern int iHidemaruErrorCnt;