#include "stdafx.h"
#include "SettingWnd.h"
#include "SdkManager.h"
#include "fsp_engine.h"
#include "util.h"
#include "define.h"
#include "UIVideoPanel.h"

using namespace fsp;


// 消息映射
DUI_BEGIN_MESSAGE_MAP(CSettingWnd, WindowImplBase)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_VALUECHANGED, OnValueChanged)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMSELECT, OnComboItemSelected)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectedChange)
DUI_END_MESSAGE_MAP()

CSettingWnd::CSettingWnd()
	:m_dwCurRenderCamId(INVALID_CAM_INDEX)
{
}

CSettingWnd::~CSettingWnd()
{
}

CDuiString CSettingWnd::GetSkinFolder()
{
	return CDuiString(L"skin");
}

CDuiString CSettingWnd::GetSkinFile()
{
	return CDuiString(L"setting_dlg.xml");
}

LPCTSTR CSettingWnd::GetWindowClassName(void) const
{
	return L"CDuiSettingWnd";
}

CControlUI* CSettingWnd::CreateControl(LPCTSTR pstrClass)
{
	CControlUI * pUI = NULL;
	if (_tcsicmp(pstrClass, CVideoPanelUI::K_UI_INTERFACE_NAME) == 0)
	{
		pUI = new CVideoPanelUI();
	}
	return pUI;
}

void CSettingWnd::OnValueChanged(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == L"slider_mic")
	{
		CSliderUI* pSliderMic = (CSliderUI*)m_PaintManager.FindControl(L"slider_mic");
		CSdkManager::GetInstance().SetMicVol(pSliderMic->GetValue());
	}
	else if (msg.pSender->GetName() == L"slider_aud")
	{
		CSliderUI* pSliderAud = (CSliderUI*)m_PaintManager.FindControl(L"slider_aud");
		CSdkManager::GetInstance().SetAudVol(pSliderAud->GetValue());
	}
	else if (msg.pSender->GetName() == L"slider_frame_rate")
	{
		DWORD dwFrameRate = ((CSliderUI*)msg.pSender)->GetValue();

		WCHAR szFrameRate[16]; // 更新滑动条后的帧率显示
		CLabelUI* pLabelFrameRate = (CLabelUI*)m_PaintManager.FindControl(L"label_frame_rate");
		_snwprintf_s(szFrameRate, 16, L"%d 帧/秒", dwFrameRate);
		pLabelFrameRate->SetText(szFrameRate);

		CSdkManager::GetInstance().SetFrameRate(dwFrameRate);
	}
}

void CSettingWnd::OnClick(TNotifyUI& msg)
{
	WindowImplBase::OnClick(msg);

	if (msg.pSender->GetName() == L"setting_ok") {
		ScreenShareConfig screenShareConfig;
		COptionUI* pOptionShareSpeed = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("qos_mode_speed")));
		COptionUI* pOptionShareQuality = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("qos_mode_quality")));
		CRichEditUI* pEditLeft = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("share_left_edit")));
		CRichEditUI* pEditTop = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("share_top_edit")));
		CRichEditUI* pEditRight = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("share_right_edit")));
		CRichEditUI* pEditBottom = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("share_bottom_edit")));
		if (pOptionShareSpeed->IsSelected()) {
			screenShareConfig.qualityBias = fsp::SCREEN_SHARE_BIAS_SPEED;
		}
		else {
			screenShareConfig.qualityBias = fsp::SCREEN_SHARE_BIAS_QUALITY;
		}

		screenShareConfig.left = _tstoi(pEditLeft->GetText());
		screenShareConfig.top = _tstoi(pEditTop->GetText());
		screenShareConfig.right = _tstoi(pEditRight->GetText());
		screenShareConfig.bottom = _tstoi(pEditBottom->GetText());

		CSdkManager::GetInstance().SetScreenShareConfig(screenShareConfig);

		Close();
	}
}

LRESULT CSettingWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (uMsg == WM_TIMER)
	{
		if (CSdkManager::GetInstance().GetOpenMic() != INVALID_MIC_INDEX)
		{
			// 获取麦克风能量值
			fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();
			DWORD dwEnergy = pEngin->GetAudioEngine()->GetAudioParam(AUDIOPARAM_MICROPHONE_ENERGY);

			// 显示能量值
			WCHAR szImgName[32];
			_snwprintf_s(szImgName, 32, L"img\\setting\\menu_wave%d.png", dwEnergy / 10);
			//CLabelUI* pLabel = (CLabelUI*)m_PaintManager.FindControl(L"mic_energy");
			//pLabel->SetBkImage(szImgName);
			CProgressUI* pEnergyProgress = (CProgressUI*)m_PaintManager.FindControl(_T("progres_mic_energy"));
			pEnergyProgress->SetValue(dwEnergy);
		}

		if (CSdkManager::GetInstance().GetOpenAud() != INVALID_AUD_INDEX)
		{
			// 获取扬声器能量值
			fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();
			DWORD dwEnergy = pEngin->GetAudioEngine()->GetAudioParam(AUDIOPARAM_SPEAKER_ENERGY);

			// 显示能量值
			WCHAR szImgName[32];
			_snwprintf_s(szImgName, 32, L"img\\setting\\menu_wave%d.png", dwEnergy / 10);
			//CLabelUI* pLabel = (CLabelUI*)m_PaintManager.FindControl(L"aud_energy");
			//pLabel->SetBkImage(szImgName);

			CProgressUI* pEnergyProgress = (CProgressUI*)m_PaintManager.FindControl(_T("progres_aud_energy"));
			pEnergyProgress->SetValue(dwEnergy);
		}
	}
	else if (uMsg == WM_MOVE || uMsg == WM_SIZE)
	{
		if (m_pTabLayout && m_pTabLayout->GetCurSel() == 0) {
			m_pVideoPanel->UpdateVideoPos();
		}
	}
	else if (uMsg == WM_CTLCOLOR)
		return (BOOL)(HBRUSH)GetStockObject(BLACK_BRUSH);

	return 0;
}

void CSettingWnd::OnSelectMic(TNotifyUI& msg)
{
	CComboUI* pComboMic = (CComboUI*)msg.pSender;
	DWORD dwMicIndex = pComboMic->GetCurSel();

	CSdkManager::GetInstance().SetOpenMic(dwMicIndex);

	CSliderUI* pMicSlider = (CSliderUI*)m_PaintManager.FindControl(L"slider_mic");
	if (dwMicIndex == INVALID_MIC_INDEX)
	{
		pMicSlider->SetEnabled(false);
		pMicSlider->SetValue(0);
	}
	else
	{
		pMicSlider->SetEnabled(true);
		pMicSlider->SetValue(CSdkManager::GetInstance().GetMicVol());
	}
}

void CSettingWnd::OnSelectAud(TNotifyUI& msg)
{
	CComboUI* pComboAud = (CComboUI*)msg.pSender;
	DWORD dwAudIndex = pComboAud->GetCurSel();

	CSdkManager::GetInstance().SetOpenAud(dwAudIndex);

	CSliderUI* pAudSlider = (CSliderUI*)m_PaintManager.FindControl(L"slider_aud");
	if (dwAudIndex == INVALID_AUD_INDEX)
	{
		pAudSlider->SetEnabled(false);
		pAudSlider->SetValue(0);
	}
	else
	{
		pAudSlider->SetEnabled(true);
		pAudSlider->SetValue(CSdkManager::GetInstance().GetAudVol());
	}
}

void CSettingWnd::OnSelectCam(TNotifyUI& msg)
{
	CVideoPanelUI* pVideoRenderUi = (CVideoPanelUI*)m_PaintManager.FindControl(L"vrender_previewer");

	fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();
	if (m_dwCurRenderCamId != INVALID_CAM_INDEX)
		pEngin->RemoveVideoPreview(m_dwCurRenderCamId, pVideoRenderUi->GetRenderHwnd());

	fsp::Vector<VideoDeviceInfo> vecCam = pEngin->GetDeviceManager()->GetCameraDevices();
	
	CComboUI* pComboCam = (CComboUI*)msg.pSender;
	pEngin->AddVideoPreview(vecCam[pComboCam->GetCurSel()].camera_id, pVideoRenderUi->GetRenderHwnd(), fsp::RENDERMODE_FIT_CENTER);
	m_dwCurRenderCamId = vecCam[pComboCam->GetCurSel()].camera_id;
	
	m_pVideoPanel->ShowLocalPreview(m_dwCurRenderCamId);
}

void CSettingWnd::OnSelectRes(TNotifyUI& msg)
{
	CComboUI* pComboRes = (CComboUI*)msg.pSender;
	DWORD dwResIndex = pComboRes->GetCurSel();

	CSdkManager::GetInstance().SetResolution(dwResIndex);
}

void CSettingWnd::OnComboItemSelected(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == L"combo_cam")
	{
		OnSelectCam(msg);
	}
	else if (msg.pSender->GetName() == L"combo_mic")
	{
		OnSelectMic(msg);
	}
	else if (msg.pSender->GetName() == L"combo_aud")
	{
		OnSelectAud(msg);
	}
	else if (msg.pSender->GetName() == L"combo_resolution")
	{
		OnSelectRes(msg);
	}
}

void CSettingWnd::InitWindow()
{
	m_wndShadow.Create(m_hWnd);
	RECT rcCorner = { 6,6,6,6 };
	RECT rcHoleOffset = { 7,7,7,7 };
	m_wndShadow.SetImage(_T("img\\wnd_bg.png"), rcCorner, rcHoleOffset);

	m_pVideoPanel = (CVideoPanelUI*)m_PaintManager.FindControl(L"vrender_previewer");

	// 初始化分辨率下拉列表
	CComboUI* pComboResolution = (CComboUI*)m_PaintManager.FindControl(L"combo_resolution");
	for (int i = 0; i < sizeof(VideoResolutions) / sizeof(VideoResolution); i++)
	{
		WCHAR szResolution[128];
		_snwprintf_s(szResolution, 128, L"%d * %d", VideoResolutions[i].dwWidth, VideoResolutions[i].dwHeight);

		CListLabelElementUI* pLabel = new CListLabelElementUI();
		pLabel->SetAttribute(L"text", szResolution);
		pComboResolution->Add(pLabel);
		pComboResolution->SetInternVisible(true);
	}
	pComboResolution->SelectItem(CSdkManager::GetInstance().GetResolution());

	// 初始化帧率
	CSliderUI* pComboFrameRate = (CSliderUI*)m_PaintManager.FindControl(L"slider_frame_rate");
	pComboFrameRate->SetValue(CSdkManager::GetInstance().GetFrameRate());

	WCHAR szFrameRate[32];
	_snwprintf_s(szFrameRate, 32, L"%d 帧/秒", CSdkManager::GetInstance().GetFrameRate());
	CLabelUI* pLabelFrameRate = (CLabelUI*)m_PaintManager.FindControl(L"label_frame_rate");
	pLabelFrameRate->SetText(szFrameRate);


	// 创建刷新设置信息的定时器
	CComboUI* pComboMic = (CComboUI*)m_PaintManager.FindControl(L"combo_mic");
	m_PaintManager.SetTimer(pComboMic, UPDATE_SETTING_INFO_TIMER_ID, UPDATE_SETTING_INFO_INTERVAL);
	m_pTabLayout = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("setting_tab")));

	//screenshare
	ScreenShareConfig screenShareConfig = CSdkManager::GetInstance().GetScreenShareConfig();
	COptionUI* pOptionShareSpeed = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("qos_mode_speed")));
	COptionUI* pOptionShareQuality = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("qos_mode_quality")));
	CRichEditUI* pEditLeft = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("share_left_edit")));
	CRichEditUI* pEditTop = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("share_top_edit")));
	CRichEditUI* pEditRight = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("share_right_edit")));
	CRichEditUI* pEditBottom = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("share_bottom_edit")));
	if (screenShareConfig.qualityBias == fsp::SCREEN_SHARE_BIAS_SPEED) {
		pOptionShareSpeed->Selected(true);
	}
	else {
		pOptionShareQuality->Selected(true);
	}
	CDuiString strPosVal;
	strPosVal.Format(L"%d", screenShareConfig.left);
	pEditLeft->SetText(strPosVal);

	strPosVal.Format(L"%d", screenShareConfig.top);
	pEditTop->SetText(strPosVal);

	strPosVal.Format(L"%d", screenShareConfig.right);
	pEditRight->SetText(strPosVal);

	strPosVal.Format(L"%d", screenShareConfig.bottom);
	pEditBottom->SetText(strPosVal);

	UpdateDeviceList();
}

LRESULT CSettingWnd::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	m_pVideoPanel->StopLocalPreview();
	bHandled = FALSE;
	return 0;
}

void CSettingWnd::OnDeviceChanged()
{
	UpdateDeviceList();
}

void CSettingWnd::OnSelectedChange(TNotifyUI& msg)
{
	CDuiString    strName = msg.pSender->GetName();
	
	//tab
	if (strName == _T("tabopt_audio")) {
		m_pTabLayout->SelectItem(0);
		UpdateAudioTab();
	}
	else if (strName == _T("tabopt_video")) {

		m_pTabLayout->SelectItem(1);

		m_pVideoPanel->ShowLocalPreview(m_dwCurRenderCamId);
	}
	else if (strName == _T("tabopt_screenshare")) {
		m_pTabLayout->SelectItem(2);
	}
}

void CSettingWnd::UpdateAudioTab()
{
	CComboUI* pComboMic = (CComboUI*)m_PaintManager.FindControl(L"combo_mic");
	// 如果已经有麦克风被广播，则选择相应麦克风
	if (CSdkManager::GetInstance().GetOpenMic() != INVALID_MIC_INDEX)
		pComboMic->SelectItem(CSdkManager::GetInstance().GetOpenMic());

	CComboUI* pComboAud = (CComboUI*)m_PaintManager.FindControl(L"combo_aud");
	// 如果扬声器被设置，则选择相应扬声器
	if (CSdkManager::GetInstance().GetOpenAud() != INVALID_AUD_INDEX)
		pComboAud->SelectItem(CSdkManager::GetInstance().GetOpenAud());
}

void CSettingWnd::UpdateDeviceList()
{
	fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();

	// 初始化麦克风下拉列表框
	CComboUI* pComboMic = (CComboUI*)m_PaintManager.FindControl(L"combo_mic");
	pComboMic->RemoveAll();
	fsp::Vector<AudioDeviceInfo> vecMic = pEngin->GetDeviceManager()->GetMicrophoneDevices();
	for (auto iter = vecMic.begin(); iter != vecMic.end(); ++iter)
	{
		WCHAR szMicName[128];
		demo::ConvertUtf8ToUnicode(iter->device_name.c_str(), szMicName, 128);

		CListLabelElementUI* pLabel = new CListLabelElementUI();
		pLabel->SetAttribute(L"text", szMicName);
		pComboMic->Add(pLabel);
	}
	pComboMic->SelectItem(0);
	////////////////////////////////////////////////////////////////////////////

	// 初始化扬声器下拉列表框
	CComboUI* pComboAud = (CComboUI*)m_PaintManager.FindControl(L"combo_aud");
	pComboAud->RemoveAll();
	fsp::Vector<AudioDeviceInfo> vecAud = pEngin->GetDeviceManager()->GetSpeakerDevices();
	for (auto iter = vecAud.begin(); iter != vecAud.end(); ++iter)
	{
		WCHAR szAudName[128];
		demo::ConvertUtf8ToUnicode(iter->device_name.c_str(), szAudName, 128);

		CListLabelElementUI* pLabel = new CListLabelElementUI();
		pLabel->SetAttribute(L"text", szAudName);
		pComboAud->Add(pLabel);
	}
	pComboAud->SelectItem(0);
	////////////////////////////////////////////////////////////////////////////

	// 初始化摄像头下拉列表框
	CComboUI* pComboCam = (CComboUI*)m_PaintManager.FindControl(L"combo_cam");
	pComboCam->RemoveAll();

	fsp::Vector<VideoDeviceInfo> vecCam = pEngin->GetDeviceManager()->GetCameraDevices();
	int nSelectedIdx = 0;
	for (int i = 0; i < vecCam.size(); i++)
	{
		WCHAR szCamName[128];
		demo::ConvertUtf8ToUnicode(vecCam[i].device_name.c_str(), szCamName, 128);

		CListLabelElementUI* pLabel = new CListLabelElementUI();
		pLabel->SetAttribute(L"text", szCamName);
		pComboCam->Add(pLabel);
		pComboCam->SetInternVisible(true);
		
		if (vecCam[i].camera_id == m_dwCurRenderCamId) {
			nSelectedIdx = i;
		}
	}

	pComboCam->SelectItem(nSelectedIdx);
}