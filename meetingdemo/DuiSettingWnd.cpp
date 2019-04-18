/*##############################################################################
 * 文件：DuiSettingWnd.cpp
 * 描述：设置窗口的界面及逻辑实现
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#include "stdafx.h"
#include "DuiSettingWnd.h"
#include "SdkManager.h"
#include "fsp_engine.h"
#include "util.h"
#include "define.h"

using namespace fsp;

namespace
{
void RectClientToScreen(HWND hWnd, RECT& rect)
{
	CPoint ptLeftTop;
	ptLeftTop.x = rect.left;
	ptLeftTop.y = rect.top;

	ClientToScreen(hWnd, &ptLeftTop);

	CPoint ptRightBottom;
	ptRightBottom.x = rect.right;
	ptRightBottom.y = rect.bottom;

	ClientToScreen(hWnd, &ptRightBottom);

	rect.left = ptLeftTop.x;
	rect.top = ptLeftTop.y;
	rect.right = ptRightBottom.x;
	rect.bottom = ptRightBottom.y;
}
}

// 消息映射
DUI_BEGIN_MESSAGE_MAP(CDuiSettingWnd, WindowImplBase)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_VALUECHANGED, OnValueChanged)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMSELECT, OnComboItemSelected)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectedChange)
DUI_END_MESSAGE_MAP()

/*------------------------------------------------------------------------------
 * 描  述：构造函数
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
CDuiSettingWnd::CDuiSettingWnd()
	: m_dwCurRenderCamId(INVALID_CAM_INDEX)
	, m_pVideoWnd(nullptr)
{
}

/*------------------------------------------------------------------------------
 * 描  述：析构函数
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
CDuiSettingWnd::~CDuiSettingWnd()
{
	delete m_pVideoWnd;
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，DUI调用获取窗口xml布局文件目录
 * 参  数：无
 * 返回值：xml布局文件目录
 ------------------------------------------------------------------------------*/
CDuiString CDuiSettingWnd::GetSkinFolder()
{
	return CDuiString(L"skin");
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，DUI调用获取窗口xml布局文件名称
 * 参  数：无
 * 返回值：xml布局文件名称
 ------------------------------------------------------------------------------*/
CDuiString CDuiSettingWnd::GetSkinFile()
{
	return CDuiString(L"setting.xml");
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，DUI调用获取窗口类名称
 * 参  数：无
 * 返回值：窗口类名称
 ------------------------------------------------------------------------------*/
LPCTSTR CDuiSettingWnd::GetWindowClassName(void) const
{
	return L"CDuiSettingWnd";
}

void CDuiSettingWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_WINDOWINIT) {
		MoveVideoWnd();
	}
	WindowImplBase::Notify(msg);
}

/*------------------------------------------------------------------------------
 * 描  述：DUILIB滑动条消息处理
 * 参  数：[in] msg 通知消息
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiSettingWnd::OnValueChanged(TNotifyUI& msg)
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

/*------------------------------------------------------------------------------
 * 描  述：DUILIB点击消息处理
 * 参  数：[in] msg 通知消息
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiSettingWnd::OnClick(TNotifyUI& msg)
{
	WindowImplBase::OnClick(msg);

	if (msg.pSender->GetName() == L"setting_ok") {
		ScreenShareConfig screenShareConfig;
		COptionUI* pOptionShareSpeed = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("sharemodeopt_speed")));
		COptionUI* pOptionShareQuality = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("sharemodeopt_quality")));
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

/*------------------------------------------------------------------------------
 * 描  述：虚函数，自定义消息处理
 * 参  数：略
 * 返回值：LRESULT
 ------------------------------------------------------------------------------*/
LRESULT CDuiSettingWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
			CLabelUI* pLabel = (CLabelUI*)m_PaintManager.FindControl(L"mic_energy");
			pLabel->SetBkImage(szImgName);
		}

		if (CSdkManager::GetInstance().GetOpenAud() != INVALID_AUD_INDEX)
		{
			// 获取扬声器能量值
			fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();
			DWORD dwEnergy = pEngin->GetAudioEngine()->GetAudioParam(AUDIOPARAM_SPEAKER_ENERGY);

			// 显示能量值
			WCHAR szImgName[32];
			_snwprintf_s(szImgName, 32, L"img\\setting\\menu_wave%d.png", dwEnergy / 10);
			CLabelUI* pLabel = (CLabelUI*)m_PaintManager.FindControl(L"aud_energy");
			pLabel->SetBkImage(szImgName);
		}
	}
	else if (uMsg == WM_MOVE)
	{
		if (m_pTabLayout && m_pTabLayout->GetCurSel() == 0) {
			MoveVideoWnd();
		}
	}
	else if (uMsg == WM_CLOSE) {
		if (m_pVideoWnd) {
			fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();
			if (m_dwCurRenderCamId != INVALID_CAM_INDEX)
				pEngin->RemoveVideoPreview(m_dwCurRenderCamId, m_pVideoWnd->GetHWND());
		}
	}
	return 0;
}

/*------------------------------------------------------------------------------
 * 描  述：麦克风设置下拉列表消息处理
 * 参  数：[in] msg 通知消息
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiSettingWnd::OnSelectMic(TNotifyUI& msg)
{
	CComboUI* pComboMic = (CComboUI*)msg.pSender;
	DWORD dwMicIndex = pComboMic->GetCurSel();

	CSdkManager::GetInstance().SetOpenMic(dwMicIndex);

	CSliderUI* pMicSlider = (CSliderUI*)m_PaintManager.FindControl(L"slider_mic");
	if (dwMicIndex == INVALID_MIC_INDEX)
	{
		pMicSlider->SetAttribute(L"bkimage", L"img\\setting\\menu_slider_bg.png");
		pMicSlider->SetAttribute(L"thumbimage", L"img\\setting\\menu_sliderbtn_disable.png");
		pMicSlider->SetValue(0);
	}
	else
	{
		pMicSlider->SetAttribute(L"bkimage", L"img\\setting\\menu_slider_stat.png");
		pMicSlider->SetAttribute(L"thumbimage", L"img\\setting\\menu_sliderbtn.png");
		pMicSlider->SetAttribute(L"thumbhotimage", L"img\\setting\\menu_sliderbtn_hot.png");
		pMicSlider->SetAttribute(L"thumbpushedimage", L"img\\setting\\menu_sliderbtn_press.png");
		pMicSlider->SetValue(CSdkManager::GetInstance().GetMicVol());
	}
}

/*------------------------------------------------------------------------------
 * 描  述：扬声器设置下拉列表消息处理
 * 参  数：[in] msg 通知消息
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiSettingWnd::OnSelectAud(TNotifyUI& msg)
{
	CComboUI* pComboAud = (CComboUI*)msg.pSender;
	DWORD dwAudIndex = pComboAud->GetCurSel();

	CSdkManager::GetInstance().SetOpenAud(dwAudIndex);

	CSliderUI* pAudSlider = (CSliderUI*)m_PaintManager.FindControl(L"slider_aud");
	if (dwAudIndex == INVALID_AUD_INDEX)
	{
		pAudSlider->SetAttribute(L"bkimage", L"img\\setting\\menu_slider_bg.png");
		pAudSlider->SetAttribute(L"thumbimage", L"img\\setting\\menu_sliderbtn_disable.png");
		pAudSlider->SetValue(0);
	}
	else
	{
		pAudSlider->SetAttribute(L"bkimage", L"img\\setting\\menu_slider_stat.png");
		pAudSlider->SetAttribute(L"thumbimage", L"img\\setting\\menu_sliderbtn.png");
		pAudSlider->SetAttribute(L"thumbhotimage", L"img\\setting\\menu_sliderbtn_hot.png");
		pAudSlider->SetAttribute(L"thumbpushedimage", L"img\\setting\\menu_sliderbtn_press.png");
		pAudSlider->SetValue(CSdkManager::GetInstance().GetAudVol());
	}
}

/*------------------------------------------------------------------------------
 * 描  述：摄像头预览下拉列表消息处理
 * 参  数：[in] msg 通知消息
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiSettingWnd::OnSelectCam(TNotifyUI& msg)
{
	if (!m_pVideoWnd) {
		return;
	}

	MoveVideoWnd();

	fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();
	if (m_dwCurRenderCamId != INVALID_CAM_INDEX)
		pEngin->RemoveVideoPreview(m_dwCurRenderCamId, m_pVideoWnd->GetHWND());

	fsp::Vector<VideoDeviceInfo> vecCam = pEngin->GetDeviceManager()->GetCameraDevices();
	
	CComboUI* pComboCam = (CComboUI*)msg.pSender;
	pEngin->AddVideoPreview(vecCam[pComboCam->GetCurSel()].camera_id, m_pVideoWnd->GetHWND(), fsp::RENDERMODE_FIT_CENTER);
	m_dwCurRenderCamId = vecCam[pComboCam->GetCurSel()].camera_id;
}

/*------------------------------------------------------------------------------
 * 描  述：分辨率下拉列表消息处理
 * 参  数：[in] msg 通知消息
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiSettingWnd::OnSelectRes(TNotifyUI& msg)
{
	CComboUI* pComboRes = (CComboUI*)msg.pSender;
	DWORD dwResIndex = pComboRes->GetCurSel();

	CSdkManager::GetInstance().SetResolution(dwResIndex);
}

/*------------------------------------------------------------------------------
 * 描  述：下拉列表消息处理
 * 参  数：[in] msg 通知消息
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiSettingWnd::OnComboItemSelected(TNotifyUI& msg)
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

/*------------------------------------------------------------------------------
 * 描  述：DUILIB框架回调，窗口初始化处理
 * 参  数：无
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiSettingWnd::InitWindow()
{
	// 初始化分辨率下拉列表
	CComboUI* pComboResolution = (CComboUI*)m_PaintManager.FindControl(L"combo_resolution");
	for (int i = 0; i < sizeof(VideoResolutions) / sizeof(VideoResolution); i++)
	{
		WCHAR szResolution[128];
		_snwprintf_s(szResolution, 128, L"%d * %d", VideoResolutions[i].dwWidth, VideoResolutions[i].dwHeight);

		CListLabelElementUI* pLabel = new CListLabelElementUI();
		pLabel->SetAttribute(L"text", szResolution);
		pComboResolution->Add(pLabel);
	}
	pComboResolution->SelectItem(CSdkManager::GetInstance().GetResolution());

	// 初始化帧率
	CSliderUI* pComboFrameRate = (CSliderUI*)m_PaintManager.FindControl(L"slider_frame_rate");
	pComboFrameRate->SetValue(CSdkManager::GetInstance().GetFrameRate());

	WCHAR szFrameRate[32];
	_snwprintf_s(szFrameRate, 32, L"%d 帧/秒", CSdkManager::GetInstance().GetFrameRate());
	CLabelUI* pLabelFrameRate = (CLabelUI*)m_PaintManager.FindControl(L"label_frame_rate");
	pLabelFrameRate->SetText(szFrameRate);

	// 创建设置窗口但不显示
	m_pVideoWnd = new CFloatWnd(L"video_preview.xml", L"PreviewVideoWnd");
	m_pVideoWnd->CreateWnd(m_hWnd, L"PreviewWnd", UI_WNDSTYLE_FRAME, false);

	// 创建刷新设置信息的定时器
	CComboUI* pComboMic = (CComboUI*)m_PaintManager.FindControl(L"combo_mic");
	m_PaintManager.SetTimer(pComboMic, UPDATE_SETTING_INFO_TIMER_ID, UPDATE_SETTING_INFO_INTERVAL);
	m_pTabLayout = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("setting_tab")));

	//screenshare
	ScreenShareConfig screenShareConfig = CSdkManager::GetInstance().GetScreenShareConfig();
	COptionUI* pOptionShareSpeed = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("sharemodeopt_speed")));
	COptionUI* pOptionShareQuality = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("sharemodeopt_quality")));
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

void CDuiSettingWnd::OnDeviceChanged()
{
	UpdateDeviceList();
}

/*------------------------------------------------------------------------------
 * 描  述：tab标签页切换
 * 参  数：无
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiSettingWnd::OnSelectedChange(TNotifyUI& msg)
{
	CDuiString    strName = msg.pSender->GetName();
	
	//tab
	if (strName == _T("tabopt_video")) {
		m_pTabLayout->SelectItem(0);
		m_pVideoWnd->ShowWindow(true);
		MoveVideoWnd();
	}
	else if (strName == _T("tabopt_audio")) {
		m_pTabLayout->SelectItem(1);
		m_pVideoWnd->ShowWindow(false);
		UpdateAudioTab();
	}
	else if (strName == _T("tabopt_screenshare")) {
		m_pTabLayout->SelectItem(2);
		m_pVideoWnd->ShowWindow(false);
	}
}

void CDuiSettingWnd::UpdateAudioTab()
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

void CDuiSettingWnd::MoveVideoWnd()
{
	CLabelUI* pPreviewLabel = (CLabelUI*)m_PaintManager.FindControl(L"cam_previewer");
	if (m_pVideoWnd && pPreviewLabel) {
		RECT rect = pPreviewLabel->GetClientPos();
		RectClientToScreen(m_hWnd, rect);
		m_pVideoWnd->SetWndRect(rect);
	}
}

void CDuiSettingWnd::UpdateDeviceList()
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

		if (vecCam[i].camera_id == m_dwCurRenderCamId) {
			nSelectedIdx = i;
		}
	}

	pComboCam->SelectItem(nSelectedIdx);
}