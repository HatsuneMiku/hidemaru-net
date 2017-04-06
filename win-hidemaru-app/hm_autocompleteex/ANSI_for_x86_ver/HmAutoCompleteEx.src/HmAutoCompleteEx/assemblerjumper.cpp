#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <shlwapi.h>
#include <string>
#include "assemblerjumper.h"
#pragma comment(lib, "shlwapi.lib")

#include "outputdebugstream.h"
#include "autocompletewindow.h"
#include "hookwin32api.h"
#include "outlinerwindow.h"
#include "programmemory.h"
#include "assemblerjumper.h"

#include "OnigRegex.h"


using namespace std;

extern HANDLE curProcess;

HWND hWndHidemaruOutLinerListBox = NULL;
static BOOL _stdcall EnumChildProc(HWND wnd, LPARAM lParam) {
	char cname[250];
	GetClassName(wnd, cname, sizeof(cname));

	char stext[250];
	GetWindowText(wnd, stext, sizeof(stext));

	OutputDebugStream("�E�C���h�E%x, �N���X��:%s �e�L�X�g%s:\n", wnd, cname, stext);
	if (strcmp(cname, "ListBox") == 0) {
		OutputDebugStream("���X�g�{�b�N�X����%x, �N���X��:%s\n", wnd, cname);

		HWND parent = GetParent(wnd);
		GetClassName(parent, cname, sizeof(cname));
		OutputDebugStream("���X�g�{�b�N�X�̐e%x, �N���X��:%s\n", parent, cname);

		// �e���A�E�g���C�i�ŁA�����̓��X�g�{�b�N�X�B�m���ɃA�E�g���C�i�p�̕��͘g
		if (strcmp(cname, "HM32OUTLINE") == 0) {
			hWndHidemaruOutLinerListBox = wnd;
		}
	}


	return TRUE;
}

/*
*/


static int iEAXOfHidemaruTitleAdd = 0;

void OnCallBackedOfHidemaruTitleAdd() {
	OutputDebugStream("���^�C�g���X�V");
	strcpy(szCurrentFileOnCallSetHidemaruHandle, ""); // �ŏ��ɂ�������^�C�g���͂�����ʉ߂������_�ŏ���

	if (iEAXOfHidemaruTitleAdd) {
		OutputDebugStream("����%s", (char *)iEAXOfHidemaruTitleAdd);
		char *pFileName = (char *)iEAXOfHidemaruTitleAdd;

		// ���΃p�X���΃p�X��
		char szAbsolutePath[MAX_PATH * 2] = "";
		if (PathIsRelative(pFileName)) {
			OutputDebugStream("����%s�͑��΃p�X�A��΃p�X�������݂�\n", pFileName);
			DWORD dwRet = GetFullPathName(
				pFileName,
				sizeof(szAbsolutePath),
				szAbsolutePath,
				NULL);
			OutputDebugStream("������΃p�X��%s\n", szAbsolutePath);
			pFileName = szAbsolutePath;
		}

		// ���݂���B
		if (PathFileExists(pFileName)) {
			strcpy(szCurrentFileOnCallSetHidemaruHandle, pFileName);
		}

		// ���݂��Ȃ��B�u�����]�v�ȕ������t���Ă���v
		else {
			OutputDebugStream("����%s�͑��݂��܂���A���K�������݂�\n", pFileName);

			Matches m;
			if (OnigMatch(pFileName, "^(.+) \\([^\\)]+?\\)$", &m)) {
				OutputDebugStream("�������K���̌��ʂ�%s\n", m[1]);

				// ���ꂪ�t�@�C���Ƃ��đ��݂���Ȃ�A����ɂ��Ă����B
				if (PathFileExists(m[1].data())) {
					strcpy(szCurrentFileOnCallSetHidemaruHandle, m[1].data());
				}
			}
		}
	}
	else {
		OutputDebugStream("�ʂ��\n");
	}

	PathRemoveBlanks(szCurrentFileOnCallSetHidemaruHandle); // �擪�Ɩ����̋󔒏���
	OutputDebugStream("�ŏI���ʂ�%s�ł�\n", szCurrentFileOnCallSetHidemaruHandle);
}

int iCallFuncOfHidemaruTitleAdd = 0;

static int pJumpFromOfHidemaruTitleAdd = NULL;
static int pRetnBackOfHidemaruTitleAdd = NULL;
static int iPuashBackValueOfHidemaruTitleAdd = 0;

extern "C" __declspec(dllexport) __declspec(naked) void JumpOnCallBackedOfHidemaruTitleAdd() {

	__asm {
		mov iEAXOfHidemaruTitleAdd, eax			// �^�C�g��
		push iPuashBackValueOfHidemaruTitleAdd	// �u - �G�ہv���Ă��������̃v�b�V��(�p�ꂾ�Ɓu - Maruo�v

		PUSHAD
		PUSHFD
	}

	OnCallBackedOfHidemaruTitleAdd();

	__asm {

		POPFD
		POPAD

		jmp pRetnBackOfHidemaruTitleAdd
	}
}









extern void HookWin32apiFuncs();
extern int ProgramMemoryDstImageBase;
extern int ProgramMemoryDstBaseOfData;
extern char szHidemaru[MAX_PATH*2];

char szCurrentFileOnCallSetHidemaruHandle[MAX_PATH * 2] = ""; // �G�ۂ�SetHidemaruHandle���Ă΂ꂽ���ɍŏ��ɃX���b�g�ɓ���t�@�C�����B�r���Ńt�@�C�������ς�邱�Ƃ�����̂ŁA�����܂ł��Ȃ������ꍇ�p�r
char *GetCurrentFileName() {
	return szCurrentFileOnCallSetHidemaruHandle;
}

char szHidemaruAdditionTitle[] = " - �G��";

byte pRefOfByteArrayOfHidemaruTitileAdd[] = { 0x68, 0x00, 0x00, 0x00, 0x00, 0x50, 0xE8 };

extern "C" __declspec(dllexport) int SetHidemaruHandle(int pWndHidemaru, char *szCurFileName) {
	OutputDebugStream("���ς�����u�Ԃ̃t�@�C����%s\n", szCurFileName);
	strcpy(szCurrentFileOnCallSetHidemaruHandle, szCurFileName);
	HookWin32apiFuncs();

	pWndHidemaru = (int)GetForegroundWindow();
	char sz[100];
	GetClassName((HWND)pWndHidemaru, sz, sizeof(sz));
	OutputDebugStream("�n���ꂽ�E�B���h�E%x, ���O%s\n\n", pWndHidemaru, sz);

	HWND wnd = GetForegroundWindow();
	GetClassName((HWND)wnd, sz, sizeof(sz));
	OutputDebugStream("�t�H�A�ȃE�B���h�E%x:���O%s\n", wnd, sz);

	if (wnd) {
		EnumChildWindows((HWND)wnd, EnumChildProc, 0);
		// ������
		AnalizeOutlinerListBox(hWndHidemaruOutLinerListBox);
	}

	if (iCallFuncOfHidemaruTitleAdd == 0) {
		checkProgramMemoryImage(szHidemaru);

		OutputDebugStream("�x�[�X�A�h���X%x\n", ProgramMemoryDstImageBase);
		OutputDebugStream("�f�[�^�A�h���X%x\n", ProgramMemoryDstBaseOfData);

		// " - �G��" ��T��
		void *pHidemaruTitileAddInBaseOfData = memmem((void *)ProgramMemoryDstBaseOfData, iProgramMemorySearchSize, szHidemaruAdditionTitle, sizeof(szHidemaruAdditionTitle));
		if (pHidemaruTitileAddInBaseOfData == NULL) {
			pHidemaruTitileAddInBaseOfData = memmem((void *)ProgramMemoryDstBaseOfData, iProgramMemorySearchSize, " - Maruo", sizeof(szHidemaruAdditionTitle)); // �p���
		}
		OutputDebugStream("pHidemaruTitileAddInBaseOfData�̈ʒu%x\n", pHidemaruTitileAddInBaseOfData);
		iPuashBackValueOfHidemaruTitleAdd = (int)pHidemaruTitileAddInBaseOfData;

		// �T�[�`���邽�߂̓��e�����肷��
		byte *p = pRefOfByteArrayOfHidemaruTitileAdd;
		p++;
		int *ip = (int *)p;
		(*ip) = (int)pHidemaruTitileAddInBaseOfData;

		/*
		OutputDebugStream("%x", pRefOfByteArrayOfHidemaruTitileAdd[0]);
		OutputDebugStream("%x", pRefOfByteArrayOfHidemaruTitileAdd[1]);
		OutputDebugStream("%x", pRefOfByteArrayOfHidemaruTitileAdd[2]);
		OutputDebugStream("%x", pRefOfByteArrayOfHidemaruTitileAdd[3]);
		OutputDebugStream("%x", pRefOfByteArrayOfHidemaruTitileAdd[4]);
		OutputDebugStream("%x", pRefOfByteArrayOfHidemaruTitileAdd[5]);
		OutputDebugStream("%x\n", pRefOfByteArrayOfHidemaruTitileAdd[6]);
		*/

		void *pPushAsmOfHidemaruTitileAdd = memmem((void *)ProgramMemoryDstImageBase, (ProgramMemoryDstBaseOfData - ProgramMemoryDstImageBase), pRefOfByteArrayOfHidemaruTitileAdd, sizeof(pRefOfByteArrayOfHidemaruTitileAdd));
		if (pPushAsmOfHidemaruTitileAdd == NULL) {
			void *pPushAsmOfHidemaruTitileAdd = memmem((void *)ProgramMemoryDstImageBase, (ProgramMemoryDstBaseOfData - ProgramMemoryDstImageBase), pRefOfByteArrayOfHidemaruTitileAdd, sizeof(pRefOfByteArrayOfHidemaruTitileAdd));
		}
		OutputDebugStream("�G�̃^�C�g����push�̃v���O�����̏ꏊ%x\n", pPushAsmOfHidemaruTitileAdd);
		iCallFuncOfHidemaruTitleAdd = (int)pPushAsmOfHidemaruTitileAdd;
	}

	char cmdOnTenshouExeJumpFrom[10] = "\xE9\x90\x90\x90\x90\x90\x90\x90\x90"; // ���߂ɂƂ��Ă���

	pJumpFromOfHidemaruTitleAdd = iCallFuncOfHidemaruTitleAdd;
	pRetnBackOfHidemaruTitleAdd = pJumpFromOfHidemaruTitleAdd + 5;

	// �܂��A�h���X�𐔎��Ƃ��Ĉ���
	int iAddress = (int)JumpOnCallBackedOfHidemaruTitleAdd;
	int SubAddress = iAddress - (pJumpFromOfHidemaruTitleAdd + 5);

	// �T�Ƃ����̂́A0046C194  -E9 ????????  JMP TSMod.OnTSExeCreateFileA01  �̖��߂ɕK�v�ȃo�C�g���B�v����ɂT�o�C�g�����Ǝ��̃j�[���j�b�N���ߌQ�Ɉړ�����̂��B�����Ă�������̍������W�����v����ۂ̖ړI�i�Ƃ��ė��p�����B
	memcpy(cmdOnTenshouExeJumpFrom + 1, &SubAddress, 4); // +1 ��E9�̎�����4�o�C�g�����������邩��B

	/*
	OutputDebugStream("�W�����v��%x", JumpOnCallBackedOfHidemaruTitleAdd);
	OutputDebugStream("%x,", cmdOnTenshouExeJumpFrom[0]);
	OutputDebugStream("%x,", cmdOnTenshouExeJumpFrom[1]);
	OutputDebugStream("%x,", cmdOnTenshouExeJumpFrom[2]);
	OutputDebugStream("%x,", cmdOnTenshouExeJumpFrom[3]);
	OutputDebugStream("%x,", cmdOnTenshouExeJumpFrom[4]);
	OutputDebugStream("%x\n", 0);
	*/

	// �\�z�����j�[���j�b�N���߂�TENSHOU.EXE�̃������ɏ���������
	OutputDebugStream("��������%x\n", pJumpFromOfHidemaruTitleAdd);
	WriteProcessMemory(curProcess, (LPVOID)(pJumpFromOfHidemaruTitleAdd), cmdOnTenshouExeJumpFrom, 5, NULL); //5�o�C�g�̂ݏ�������

	return TRUE;
}

