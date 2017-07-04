#include <windows.h>

#include "hidemaruexe_handle.h"
#include "hidemaruexe_export.h"


HidemaruWindowHandleSearcher::HidemaruWindowHandleSearcher(wstring strClassName) {
	this->m_strClassName = strClassName;
	this->hCurWndHidemaru = NULL;
}

HWND HidemaruWindowHandleSearcher::GetCurWndHidemaru() {
	if (!hCurWndHidemaru) {
		FastSearchCurWndHidemaru(GetDesktopWindow());
	}
	if (!hCurWndHidemaru) {
		SlowSearchCurWndHidemaru(GetDesktopWindow());
	}
	return this->hCurWndHidemaru;
}



bool HidemaruWindowHandleSearcher::IsWndHidemaru32ClassType(HWND hWnd) {
	if ( !hWnd ) {
		return false;
	}

	wchar_t strTargetClassName[MAX_CLASS_NAME];
	::GetClassName(hWnd, strTargetClassName, _countof(strTargetClassName));

	if (m_strClassName == strTargetClassName) {
		return true;
	}
	return false;
}

// ������肾���A�G�ۑ��̃A�v���̎q���ɂȂ��Ă�ȂǁA�����A�P�[�X�ł������o����B
void HidemaruWindowHandleSearcher::SlowSearchCurWndHidemaru(HWND hWnd)
{
	if (hCurWndHidemaru) { return; }

	HWND hWndParent = ::GetParent(hWnd);

	// �e�����邱�Ƃ�����
	if (hWndParent) {
		// �����̃v���Z�XID�ƁA�T�[�`�Ώۂ̃v���Z�XID
		DWORD pID1 = GetCurrentProcessId();
		DWORD pID2 = 0;
		GetWindowThreadProcessId(hWnd, &pID2);
		// �����Ȃ�傫����₾
		if (pID1 == pID2) {

			// �����Ɛe�������Ƃ�Hidemaru32Class(�n)�Ȃ犮�S���Ă�
			if ( hWnd && IsWndHidemaru32ClassType(hWnd) &&
				 hWndParent && IsWndHidemaru32ClassType(hWndParent) ) {
				hCurWndHidemaru = hWnd;
			}
		}
	}

	// �q�N���X���Ȃ߂Ă����B�q�N���X�͂����܂ł��uHidemaru32Class�n�v�B
	// �X�g�A�ł͂�����ƈႤ�̂ŃC���X�^���X�ϐ��ɂȂ��Ă���B
	HWND hWndChild = FindWindowEx(hWnd, NULL, NULL, NULL);
	while (hWndChild != NULL)
	{
		SlowSearchCurWndHidemaru(hWndChild);
		if (hCurWndHidemaru) { break; }

		hWndChild = FindWindowEx(hWnd, hWndChild, NULL, NULL);
	}
}

// �����B�ʏ�Ȃ炱�̃A���S���Y���ŏ\��
void HidemaruWindowHandleSearcher::FastSearchCurWndHidemaru(HWND hWnd)
{
	if (hCurWndHidemaru) { return; }

	HWND hWndParent = ::GetParent(hWnd);

	// �e�����邱�Ƃ�����
	if (hWndParent) {
		// �����̃v���Z�XID�ƁA�T�[�`�Ώۂ̃v���Z�XID
		DWORD pID1 = GetCurrentProcessId();
		DWORD pID2 = 0;
		GetWindowThreadProcessId(hWnd, &pID2);
		// �����Ȃ�傫����₾
		if (pID1 == pID2) {

			// �������g�̐e���w��̃N���X���Ȃ�A���S�ɓ��肵���B
			if (hWndParent && IsWndHidemaru32ClassType(hWndParent))
			{
				hCurWndHidemaru = hWnd;
			}
		}
	}

	// �q�N���X���Ȃ߂Ă����B�q�N���X�͂����܂ł��uHidemaru32Class�n�v�B
	// �X�g�A�ł͂�����ƈႤ�̂ŃC���X�^���X�ϐ��ɂȂ��Ă���B
	HWND hWndChild = FindWindowEx(hWnd, NULL, m_strClassName.c_str(), NULL);
	while (hWndChild != NULL)
	{
		FastSearchCurWndHidemaru(hWndChild);
		if (hCurWndHidemaru) { break; }

		hWndChild = FindWindowEx(hWnd, hWndChild, m_strClassName.c_str(), NULL);
	}
}

static HWND hWnd = NULL;
HWND GetCurWndHidemaru() {
	if (!hWnd) {
		// ���ʂ̏G��
		HidemaruWindowHandleSearcher s1(L"Hidemaru32Class");
		hWnd = s1.GetCurWndHidemaru();
	}
	if (!hWnd) {
		// �X�g�A�A�v����
		HidemaruWindowHandleSearcher s2(L"Hidemaru32Class_Appx");
		hWnd = s2.GetCurWndHidemaru();
	}

	return hWnd;
}
