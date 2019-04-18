/*##############################################################################
 * 文件：DisplayVideoWnd.cpp
 * 描述：视频小窗口实现
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#include "stdafx.h"
#include "DisplayVideoWnd.h"
#include "UIMenu.h"
#include "SdkManager.h"
#include "util.h"
#include "define.h"
#include "MediaInfoWnd.h"
#include "VideoWnd.h"


CDisplayVideoWnd::CDisplayVideoWnd()
	: m_eRenderMode(fsp::RENDERMODE_SCALE_FILL)
	, m_pInfoWnd(nullptr)
	, m_pVideoWnd(nullptr)
	, m_bHasAudio(false)
	, m_bHasVideo(false)
	, m_dwCamIndex(0xFFFFFFFF)
	, m_bIsLocal(false)
	, m_bIsVideoMaximized(false)
	, m_pEventCallback(nullptr)
{
	m_RemoteControlState = RemoteControl_No;
	ZeroMemory(&m_rectDisplay, sizeof(m_rectDisplay));
}


CDisplayVideoWnd::~CDisplayVideoWnd()
{
	fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();

	if (m_bHasAudio && m_bIsLocal)
		pEngin->StopPublishAudio();

	if (m_bHasVideo && m_bIsLocal)
		pEngin->StopPublishVideo(m_strVideoId);

	if (m_pVideoWnd != nullptr)
		delete m_pVideoWnd;

	if (m_pInfoWnd != nullptr)
		delete m_pInfoWnd;
}


void CDisplayVideoWnd::SetEventCallback(IEventCallback* pCallback)
{
	m_pEventCallback = pCallback;
}


void CDisplayVideoWnd::OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_LBUTTONDBLCLK)
		m_bIsVideoMaximized = !m_bIsVideoMaximized;

	if (m_pEventCallback)
		m_pEventCallback->OnEvent(uMsg, wParam, lParam);
}


bool CDisplayVideoWnd::IsVideoMaximized()
{
	return m_bIsVideoMaximized;
}


void CDisplayVideoWnd::ShowWindow(bool bShow)
{
	m_pInfoWnd->ShowWindow(bShow);
	m_pVideoWnd->ShowWindow(bShow);
}

void CDisplayVideoWnd::OnRemoteControlOperation(fsp::RemoteControlOperationType operation)
{
	if (operation == fsp::REMOTE_CONTROL_ACCEPT) {
		m_RemoteControlState = RemoteControl_Ing;
	}
	else if (operation == fsp::REMOTE_CONTROL_REJECT) {
		m_RemoteControlState = RemoteControl_No;
	}
}


void CDisplayVideoWnd::Init(HWND hParentWnd)
{
	m_hParentWnd = hParentWnd;

	m_pVideoWnd = new CVideoWnd(L"video_default.xml", L"VideoDefault");
	m_pVideoWnd->CreateWnd(hParentWnd, L"VideoWnd", UI_WNDSTYLE_CHILD, true);
	((CVideoWnd*)m_pVideoWnd)->SetEventCallback(this);

	m_pInfoWnd = new CMediaInfoWnd(L"video_info.xml", L"VideoInfo");
	m_pInfoWnd->CreateWnd(hParentWnd, L"InfoWnd", UI_WNDSTYLE_CHILD, true);
	m_pInfoWnd->GetPaintManager().AddNotifier(this);

	RefreshUserName();
}


void CDisplayVideoWnd::SetWndRect(const RECT& rect, bool bShow)
{
	m_rectDisplay = rect;

	RECT videoRect;
	videoRect.left	= rect.left;
	videoRect.top	= rect.top;
	videoRect.right	= rect.right;
	videoRect.bottom= rect.bottom - UI_VIDEO_INFO_HEIGHT;

	m_pVideoWnd->SetWndRect(videoRect);

	RECT infoRect;
	infoRect.left	= rect.left;
	infoRect.top	= rect.bottom - UI_VIDEO_INFO_HEIGHT;
	infoRect.right	= rect.right;
	infoRect.bottom = rect.bottom;

	m_pInfoWnd->SetWndRect(infoRect);
}


void CDisplayVideoWnd::OpenMic()
{
	CLabelUI* pMicLabel = (CLabelUI*)m_pInfoWnd->GetPaintManager().FindControl(L"label_mic");
	pMicLabel->SetBkImage(L"img\\video\\video_stat_audio_open.png");
}


void CDisplayVideoWnd::CloseMic()
{
	CLabelUI* pMicLabel = (CLabelUI*)m_pInfoWnd->GetPaintManager().FindControl(L"label_mic");
	pMicLabel->SetBkImage(L"img\\video\\video_stat_audio.png");

	CLabelUI* pVolLabel = (CLabelUI*)m_pInfoWnd->GetPaintManager().FindControl(L"label_volume");
	pVolLabel->SetBkImage(L"img\\video\\video_stats_volume_bg.png");
}


void CDisplayVideoWnd::OnMicEnergyChange(DWORD dwEnergy)
{
	WCHAR szBkImage[128];
	if (dwEnergy / 10 == 0)
		_snwprintf_s(szBkImage, 128, L"img\\video\\video_stats_volume_bg.png");
	else
		_snwprintf_s(szBkImage, 128, L"img\\video\\video_stats_volume%d.png", dwEnergy / 10);

	CLabelUI* pMicLabel = (CLabelUI*)m_pInfoWnd->GetPaintManager().FindControl(L"label_volume");
	pMicLabel->SetBkImage(szBkImage);
}


void CDisplayVideoWnd::OnVideoStatsChange(LPCTSTR szVideoStats)
{
	CLabelUI* pVideoInfoLabel = (CLabelUI*)m_pInfoWnd->GetPaintManager().FindControl(L"label_video_info");
	pVideoInfoLabel->SetText(szVideoStats);
}


CMenuElementUI* CDisplayVideoWnd::ConstructMenuItem(LPCTSTR szName, LPCTSTR szText, bool bRadio, bool bSelected, bool bHasSubMenu)
{
	CMenuElementUI* pElement = new CMenuElementUI;
	pElement->SetAttribute(L"width", L"180");
	pElement->SetAttribute(L"height", L"36");
	pElement->SetAttribute(L"name", szName);

	CHorizontalLayoutUI* pHLayout = new CHorizontalLayoutUI;
	pHLayout->SetAttribute(L"width", L"180");
	pHLayout->SetAttribute(L"height", L"36");

	if (bRadio)
	{
		COptionUI* pSelectOpt = new COptionUI;
		pSelectOpt->SetAttribute(L"name", szName);
		pSelectOpt->SetAttribute(L"height", L"16");
		pSelectOpt->SetAttribute(L"width", L"16");
		pSelectOpt->SetAttribute(L"padding", L"10, 10, 8, 10");
		pSelectOpt->SetAttribute(L"group", L"group");
		pSelectOpt->SetAttribute(L"normalimage", L"img\\video\\radio.png");
		pSelectOpt->SetAttribute(L"hotimage", L"img\\video\\radio_hot.png");
		pSelectOpt->SetAttribute(L"pushedimage", L"img\\video\\radio_pressed.png");
		pSelectOpt->SetAttribute(L"selectedimage", L"img\\video\\radio_sel.png");
		pSelectOpt->SetAttribute(L"selected", bSelected ? L"true" : L"false");

		pHLayout->Add(pSelectOpt);
	}
	else
	{
		CLabelUI* pPlaceholder = new CLabelUI;
		pPlaceholder->SetAttribute(L"name", szName);
		pPlaceholder->SetAttribute(L"width", L"34");

		pHLayout->Add(pPlaceholder);
	}

	CLabelUI* pTextLabel = new CLabelUI;
	pTextLabel->SetAttribute(L"height", L"36");
	pTextLabel->SetAttribute(L"width", L"116");
	pTextLabel->SetText(szText);
	pHLayout->Add(pTextLabel);

	if (bHasSubMenu)
	{
		CLabelUI* pSubmenuLabel = new CLabelUI;
		pSubmenuLabel->SetAttribute(L"height", L"8");
		pSubmenuLabel->SetAttribute(L"width", L"8");
		pSubmenuLabel->SetAttribute(L"padding", L"8,14,14,14");
		pSubmenuLabel->SetText(L"");
		pSubmenuLabel->SetAttribute(L"bkimage", L"img\\video\\more_menu_arrow.png");
		pHLayout->Add(pSubmenuLabel);
	}
	
	pElement->Add(pHLayout);

	return pElement;
}


void CDisplayVideoWnd::ConstructMenu(CMenuWnd* pMenu)
{
	// FIXME: xml布局文件中额外增加了一个空的MenuElement作为占位符，否则动态添加的
	// 菜单项无法显示，可能是个bug，暂时没有时间去解决，这里首先要移除占位符
	((CMenuUI*)pMenu->m_pm.GetRoot())->Remove(pMenu->m_pm.FindControl(L"placeholder"));

	if (m_strVideoId == fsp::RESERVED_VIDEOID_SCREENSHARE) {
		CDuiString strControlCaption = L"远程控制桌面";
		if (m_RemoteControlState == RemoteControl_Ing) {
			strControlCaption = L"取消控制";
		}
		else if (m_RemoteControlState == RemoteControl_Waiting) {
			strControlCaption = L"等待共享端接受";
		}
		CMenuElementUI* pRemoteControlElement = ConstructMenuItem(L"video_operate_remotecontrol",
			strControlCaption,
			false,
			false,
			false);
		((CMenuUI*)pMenu->m_pm.GetRoot())->Add(pRemoteControlElement);
	}
	else {
		// 先添加视频显示模式菜单项
		CMenuElementUI* pSfppElement = ConstructMenuItem(L"video_disp_sfpp",
			L"视频缩放平铺显示",
			true,
			m_eRenderMode == fsp::RENDERMODE_SCALE_FILL,
			false);
		CMenuElementUI* pDbcjElement = ConstructMenuItem(L"video_disp_dbcj",
			L"视频等比裁剪显示",
			true,
			m_eRenderMode == fsp::RENDERMODE_CROP_FILL,
			false);
		CMenuElementUI* pDbwzElement = ConstructMenuItem(L"video_disp_dbwz",
			L"视频等比完整显示",
			true,
			m_eRenderMode == fsp::RENDERMODE_FIT_CENTER,
			false);

		((CMenuUI*)pMenu->m_pm.GetRoot())->Add(pSfppElement);
		((CMenuUI*)pMenu->m_pm.GetRoot())->Add(pDbcjElement);
		((CMenuUI*)pMenu->m_pm.GetRoot())->Add(pDbwzElement);

		// 再添加切换摄像头菜单项
		if (m_bHasVideo && m_bIsLocal)
		{
			CMenuElementUI* pSwitchElement = ConstructMenuItem(L"cam_switch", L"切换摄像头", false, false, true);

			fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();
			fsp::Vector<fsp::VideoDeviceInfo> info = pEngin->GetDeviceManager()->GetCameraDevices();
			DWORD dwCamIndex = 0;
			for (auto iter : info)
			{
				WCHAR szOptionName[16];
				_snwprintf_s(szOptionName, 16, L"menu_cam_%d", dwCamIndex);

				WCHAR szCamName[32];
				demo::ConvertUtf8ToUnicode(iter.device_name.c_str(), szCamName, 32);

				CMenuElementUI* pElement = ConstructMenuItem(szOptionName,
					szCamName,
					true,
					dwCamIndex == m_dwCamIndex,
					false);

				pSwitchElement->Add(pElement);

				dwCamIndex++;
			}
			((CMenuUI*)pMenu->m_pm.GetRoot())->Add(pSwitchElement);
		}
	}
	
	//接收的是屏幕共享，操作菜单
	CMenuElementUI* pChangeSizeElement = ConstructMenuItem(L"video_disp_changesize",
		m_bIsVideoMaximized ? L"还原" : L"最大化",
		false,
		false,
		false);
	((CMenuUI*)pMenu->m_pm.GetRoot())->Add(pChangeSizeElement);


	pMenu->ResizeMenu();
}


void CDisplayVideoWnd::SetVideoRenderMode(fsp::RenderMode mode)
{
	if (m_bHasVideo && m_eRenderMode != mode) {
		if (m_bIsLocal) {
			CSdkManager::GetInstance().GetFspEngin()->AddVideoPreview(m_dwCamIndex, GetVideoWnd(), m_eRenderMode);
		}
		else {
			CSdkManager::GetInstance().GetFspEngin()->SetRemoteVideoRender(m_strUserId.c_str(), m_strVideoId.c_str(), GetVideoWnd(), mode);
		}
		
	}

	m_eRenderMode = mode;
}


void CDisplayVideoWnd::ChangeCamIndex(DWORD dwCamIndex)
{
	if (m_bHasVideo && m_bIsLocal)
	{
		// VideoId不变，CamIndex变化，流不会断，但是广播的摄像头已经改变
		fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();
		if (fsp::ERR_OK != pEngin->StartPublishVideo(m_strVideoId, dwCamIndex))
			return;

		// 先移除旧的preview摄像头
		pEngin->RemoveVideoPreview(m_dwCamIndex, GetVideoWnd());

		// 重新preview新的摄像头
		pEngin->AddVideoPreview(dwCamIndex, GetVideoWnd(), m_eRenderMode);

		// 通知主界面广播的摄像头变化了
		CSdkManager::GetInstance().GetFrameWnd()->SendMessageW(
			DUILIB_MSG_BROADCAST_CAM_CHANGED, m_dwCamIndex, dwCamIndex);

		m_dwCamIndex = dwCamIndex;
	}
}


void CDisplayVideoWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == L"btn_menu")
		{
			CMenuWnd* pMenu = new CMenuWnd(m_pInfoWnd->GetHWND());

			CDuiPoint point = msg.ptMouse;
			ClientToScreen(m_pInfoWnd->GetHWND(), &point);

			pMenu->Init(NULL,
				this,
				L"video_menu.xml",
				_T("xml"),
				point,
				eMenuAlignment_Right | eMenuAlignment_Bottom);

			ConstructMenu(pMenu);
		}
	}
	else if (msg.sType == DUI_MSGTYPE_SELECTCHANGED)
	{
		if (wcsncmp(msg.pSender->GetName(), L"menu_cam_", 9) == 0)
		{
			DWORD dwCamIndex = _wtoi(msg.pSender->GetName().GetData() + 9);
			ChangeCamIndex(dwCamIndex);
		}
		else if (msg.pSender->GetName() == L"video_disp_sfpp")
		{
			SetVideoRenderMode(RENDERMODE_SCALE_FILL);
		}
		else if (msg.pSender->GetName() == L"video_disp_dbcj")
		{
			SetVideoRenderMode(RENDERMODE_CROP_FILL);
		}
		else if (msg.pSender->GetName() == L"video_disp_dbwz")
		{
			SetVideoRenderMode(RENDERMODE_FIT_CENTER);
		}
	}
	else if (msg.sType == DUI_MSGTYPE_ITEMCLICK) {
		if (msg.pSender->GetName() == L"video_disp_changesize") {
			OnEvent(WM_LBUTTONDBLCLK, 0, 0);
		}
		else if (msg.pSender->GetName() == L"video_operate_remotecontrol") {
			if (m_RemoteControlState == RemoteControl_Ing) {
				CSdkManager::GetInstance().GetFspEngin()->RemoteControlOperation(m_strUserId,
					fsp::REMOTE_CONTROL_CANCEL);
				m_RemoteControlState = RemoteControl_No;
			}
			else if (m_RemoteControlState == RemoteControl_No) {
				CSdkManager::GetInstance().GetFspEngin()->RemoteControlOperation(m_strUserId,
					fsp::REMOTE_CONTROL_REQUEST);
				m_RemoteControlState = RemoteControl_Waiting;
			}
		}
	}
}


void CDisplayVideoWnd::OnTimer()
{
	fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();

	if (m_bHasAudio)
	{
		int nAudioEnergy = 0;
		if (m_bIsLocal)
			nAudioEnergy = pEngin->GetAudioEngine()->GetAudioParam(AUDIOPARAM_MICROPHONE_ENERGY);
		else
			nAudioEnergy = pEngin->GetRemoteAudioEnergy(m_strUserId);

		OnMicEnergyChange(nAudioEnergy);
	}

	if (m_bHasVideo)
	{
		fsp::VideoStatsInfo statsInfo;
		if (fsp::ERR_OK == pEngin->GetVideoStats(m_strUserId, m_strVideoId, &statsInfo))
		{
			WCHAR szVideoStats[32];
			_snwprintf_s(szVideoStats, 32,
				L"%dK %dF %d*%d",
				statsInfo.bitrate / 1024,
				statsInfo.framerate,
				statsInfo.width,
				statsInfo.height);
			OnVideoStatsChange(szVideoStats);
		}
	}
}


bool CDisplayVideoWnd::IsIdle()
{
	return !m_bHasAudio && !m_bHasVideo;
}


bool CDisplayVideoWnd::IsHoldRemoteVideo(const fsp::String& strUserId, const fsp::String& strVideoId)
{
	return m_bHasVideo 
		&& (strcmp(m_strUserId.c_str(), strUserId.c_str()) == 0) 
		&& (strcmp(m_strVideoId.c_str(), strVideoId.c_str()) == 0);
}


bool CDisplayVideoWnd::IsHoldRemoteVideo(const fsp::String& strUserId)
{
	return m_bHasVideo && (strcmp(m_strUserId.c_str(), strUserId.c_str()) == 0);
}


bool CDisplayVideoWnd::IsHoldRemoteAudio(const fsp::String& strUserId)
{
	return m_bHasAudio && (strcmp(m_strUserId.c_str(), strUserId.c_str()) == 0);
}


bool CDisplayVideoWnd::IsHoldLocalVideo(DWORD dwCamIndex)
{
	return m_bHasVideo && m_bIsLocal && m_dwCamIndex == dwCamIndex;
}


bool CDisplayVideoWnd::IsHoldLocalVideo()
{
	return m_bHasVideo && m_bIsLocal;
}


bool CDisplayVideoWnd::IsHoldLocalAudio(DWORD dwMicIndex)
{
	return m_bHasAudio && m_bIsLocal;
}


bool CDisplayVideoWnd::IsHoldLocalAudio()
{
	return m_bHasAudio && m_bIsLocal;
}


void CDisplayVideoWnd::RefreshUserName()
{
	CLabelUI* pUserLabel = (CLabelUI*)m_pInfoWnd->GetPaintManager().FindControl(L"label_user");
	CLabelUI* pIconLabel = (CLabelUI*)m_pInfoWnd->GetPaintManager().FindControl(L"label_user_icon");

	if (IsIdle())
	{
		pIconLabel->SetBkImage(L"img\\video\\video_user_disable.png");
		pUserLabel->SetText(L"Nobody");

		CLabelUI* pVideoInfoLabel = (CLabelUI*)m_pInfoWnd->GetPaintManager().FindControl(L"label_video_info");
		pVideoInfoLabel->SetText(L"0K 0F 0*0");
	}
	else
	{
		pIconLabel->SetBkImage(L"img\\video\\video_user_enable.png");
		if (m_bIsLocal)
		{
			pUserLabel->SetText(L"Local");
		}
		else
		{
			WCHAR szUserName[128];
			demo::ConvertUtf8ToUnicode(m_strUserId.c_str(), szUserName, 128);
			pUserLabel->SetText(szUserName);
		}
	}

	pUserLabel->Invalidate();
	pIconLabel->Invalidate();

	//有视频才显示菜单按钮
	CButtonUI* pVideoBtn = (CButtonUI*)m_pInfoWnd->GetPaintManager().FindControl(L"video_menu");
	if (m_bHasVideo) {
		pVideoBtn->SetVisible(true);
	}
	else {
		pVideoBtn->SetVisible(false);
	}
}


bool CDisplayVideoWnd::StartPublishCam(DWORD dwCamIndex)
{
	if (m_bHasVideo)
		return false;

	fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();

	// 广播本地视频
	char szVideoId[32];
	sprintf_s(szVideoId, "LocalCam_%d", dwCamIndex);
	if (fsp::ERR_OK != pEngin->StartPublishVideo(szVideoId, dwCamIndex))
	{
		PostMessageW(CSdkManager::GetInstance().GetFrameWnd()->GetHWND(), 
			DUILIB_MSG_BROADCAST_CAM_FAILED, 0, 0);
		return false;
	}

	// 显示本地视频
	pEngin->AddVideoPreview(dwCamIndex, GetVideoWnd(), m_eRenderMode);
	
	char szUserId[128];
	demo::ConvertUnicodeToUtf8(CSdkManager::GetInstance().GetLoginUser(), szUserId, 128);

	// 更新状态信息
	m_dwCamIndex	= dwCamIndex;
	m_bHasVideo		= true;
	m_bIsLocal		= true;
	m_strVideoId	= szVideoId;
	m_strUserId		= szUserId;

	SetVideoParam();
	RefreshUserName();

	return true;
}


bool CDisplayVideoWnd::StopPublishCam(DWORD dwCamIndex)
{
	bool bResult = false;

	fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();

	if (m_bHasVideo && m_bIsLocal && m_dwCamIndex == dwCamIndex)
	{
		pEngin->StopPublishVideo(m_strVideoId);

		// 取消广播视频
		pEngin->RemoveVideoPreview(dwCamIndex, GetVideoWnd());

		// 擦除视频背景
		::InvalidateRect(GetVideoWnd(), NULL, TRUE);

		m_bHasVideo = false;

		bResult = true;
	}

	RefreshUserName();

	return bResult;
}


void CDisplayVideoWnd::AddRemoteVideo(const fsp::String& strUserId, const fsp::String& strVideoId)
{
	fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();

	pEngin->SetRemoteVideoRender(strUserId, strVideoId, GetVideoWnd(), m_eRenderMode);

	m_dwCamIndex = 0;
	m_bHasVideo = true;
	m_bIsLocal = false;
	m_strVideoId = strVideoId;
	m_strUserId = strUserId;

	RefreshUserName();
}


void CDisplayVideoWnd::DelRemoteVideo(const fsp::String& strUserId, const fsp::String& strVideoId)
{
	if (m_bHasVideo && !m_bIsLocal
		&& strcmp(m_strVideoId.c_str(), strVideoId.c_str()) == 0
		&& strcmp(m_strUserId.c_str(), strUserId.c_str()) == 0)
	{
		// 刷新下视频窗口，否则可能会有残留视频图像
		::InvalidateRect(GetVideoWnd(), NULL, TRUE);

		m_bHasVideo = false;
	}

	RefreshUserName();
}


void CDisplayVideoWnd::AddRemoteAudio(const fsp::String& strUserId)
{
	if (m_bHasVideo && strcmp(m_strUserId.c_str(), strUserId.c_str()) != 0)
		return;

	m_bIsLocal = false;
	m_bHasAudio = true;
	m_strUserId = strUserId;

	fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();
	pEngin->MuteRemoteAudio(strUserId, false);

	OpenMic();
	RefreshUserName();
}


void CDisplayVideoWnd::DelRemoteAudio(const fsp::String& strUserId)
{
	if (m_bHasAudio && strcmp(m_strUserId.c_str(), strUserId.c_str()) == 0)
		m_bHasAudio = false;

	fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();
	pEngin->MuteRemoteAudio(strUserId, true);

	CloseMic();
	RefreshUserName();
}

void CDisplayVideoWnd::DelCurrentAV()
{
	StopPublishCam(m_dwCamIndex);
	StopPublishMic();

	DelRemoteVideo(m_strUserId.c_str(), m_strVideoId.c_str());
	DelRemoteAudio(m_strUserId.c_str());
}

void CDisplayVideoWnd::StartPublishMic(DWORD dwMicIndex)
{
	if (m_bHasAudio)
		return;

	fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();
	if (fsp::ErrCode::ERR_OK != pEngin->GetAudioEngine()->SetMicrophoneDevice(dwMicIndex))
		return;
	
	if (fsp::ErrCode::ERR_OK != pEngin->StartPublishAudio())
		return;
	
	char szUserId[128];
	demo::ConvertUnicodeToUtf8(CSdkManager::GetInstance().GetLoginUser(), szUserId, 128);

	m_bHasAudio		= true;
	m_bIsLocal		= true;
	m_strUserId		= szUserId;

	OpenMic();
	RefreshUserName();
}


bool CDisplayVideoWnd::StopPublishMic()
{
	if (!m_bHasAudio)
		return false;

	CSdkManager::GetInstance().GetFspEngin()->StopPublishAudio();

	m_bHasAudio = false;

	CloseMic();
	RefreshUserName();

	return true;
}

void CDisplayVideoWnd::SetVideoParam()
{
	CSdkManager& mgr = CSdkManager::GetInstance();

	fsp::VideoProfile profile;
	profile.framerate = mgr.GetFrameRate();
	profile.width = VideoResolutions[mgr.GetResolution()].dwWidth;
	profile.height = VideoResolutions[mgr.GetResolution()].dwHeight;

	mgr.GetFspEngin()->SetVideoProfile(m_strVideoId, profile);
}


void CDisplayVideoWnd::OnVideoParamChanged()
{
	if (m_bHasVideo && m_bIsLocal)
		SetVideoParam();
}