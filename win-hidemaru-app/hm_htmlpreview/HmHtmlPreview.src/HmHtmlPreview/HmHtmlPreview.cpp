#include "HmHtmlLive.h"
#include "HmHtmlFile.h"

//------------------------------------------------------------------------------------
#define MACRO_DLL extern "C" __declspec(dllexport)

// �}�N������Ă΂��
MACRO_DLL intptr_t Show(HWND hWndHidemaru, int mode) {
	hmHtmlPreviewMode kMode = (hmHtmlPreviewMode)mode;
	// �C���X�^���X���t�H�[�����g�ɕێ������āc
	if (kMode == hmHtmlPreviewMode::Live) {
		HmHtmlBaseForm::form = gcnew HmHtmlLiveForm(hWndHidemaru);
	}
	else if (kMode == hmHtmlPreviewMode::File) {
		HmHtmlBaseForm::form = gcnew HmHtmlFileForm(hWndHidemaru);
	}

	// �z��O�̐��l
	else {
		HmHtmlBaseForm::form = gcnew HmHtmlLiveForm(hWndHidemaru);
	}

	// �t�H�[����\���B
	HmHtmlBaseForm::form->Show();

	return TRUE;
}

// �G�ۂ̊Y���v���Z�X������Ƃ�
MACRO_DLL intptr_t Dispose() {

	try {
		// �܂��c���Ă�����(�t�H�[�����蓮�ŕ��Ă�����A�c���Ă��Ȃ�)
		if (HmHtmlBaseForm::form != nullptr) {

			// ����߂�
			CoInternetSetFeatureEnabled(FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_PROCESS, false);

			//�Ď����I��
			HmHtmlBaseForm::form->Stop();

			// ����Null
			HmHtmlBaseForm::form->Close();
			HmHtmlBaseForm::form = nullptr;

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