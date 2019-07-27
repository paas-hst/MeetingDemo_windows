#include "stdafx.h"
#include "SpeakerSettingWnd.h"
#include "SdkManager.h"
#include "define.h"

CSpeakerSettingWnd::CSpeakerSettingWnd()
{
}

CSpeakerSettingWnd::~CSpeakerSettingWnd()
{
}

CDuiString CSpeakerSettingWnd::GetSkinFolder()
{
	return CDuiString(L"skin");
}

CDuiString CSpeakerSettingWnd::GetSkinFile()
{
	return CDuiString(L"speaker_settingwnd.xml");
}

LPCTSTR CSpeakerSettingWnd::GetWindowClassName(void) const
{
	return L"CSpeakerSettingWnd";
}

void CSpeakerSettingWnd::Notify(TNotifyUI & msg)
{
	if (msg.sType == L"windowinit")
	{
		CSliderUI* pSlider = (CSliderUI*)(m_PaintManager.FindControl(L"speakervalue_slider"));
		if (pSlider)
		{
			pSlider->SetValue(CSdkManager::GetInstance().GetAudVol());
		}
	}
	if (msg.sType == L"valuechanged")
	{
		if (msg.pSender->GetName() == L"speakervalue_slider")
		{
			CSliderUI* pSlider = (CSliderUI*)msg.pSender;
			if (pSlider)
			{
				CSdkManager::GetInstance().SetAudVol(pSlider->GetValue());
			}
		}
	}
}

LRESULT CSpeakerSettingWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KILLFOCUS:
	{
		this->ShowWindow(false);
	}break;
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}
