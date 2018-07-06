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

/*------------------------------------------------------------------------------
 * 描  述：构造函数
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
CDisplayVideoWnd::CDisplayVideoWnd()
	: m_eRenderMode(fsp::RENDERMODE_SCALE_FILL)
	, m_pInfoWnd(nullptr)
	, m_pVideoWnd(nullptr)
	, m_bHasAudio(false)
	, m_bHasVideo(false)
	, m_dwCamIndex(0xFFFFFFFF)
	, m_dwMicIndex(0xFFFFFFFF)
	, m_bIsLocal(false)
	, m_bIsVideoMaximized(false)
	, m_pEventCallback(nullptr)
{
	ZeroMemory(&m_rectDisplay, sizeof(m_rectDisplay));
}

/*------------------------------------------------------------------------------
 * 描  述：析构函数
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------
 * 描  述：设置事件处理回调对象
 * 参  数：[in] pCallback 回调对象
 * 返回值：无
------------------------------------------------------------------------------*/
void CDisplayVideoWnd::SetEventCallback(IEventCallback* pCallback)
{
	m_pEventCallback = pCallback;
}

/*------------------------------------------------------------------------------
 * 描  述：事件处理
 * 参  数：[in] uMsg	消息类型
 *         [in] wParam	参数
 *         [in] lParam	参数
 * 返回值：无
------------------------------------------------------------------------------*/
void CDisplayVideoWnd::OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_LBUTTONDBLCLK)
		m_bIsVideoMaximized = !m_bIsVideoMaximized;

	if (m_pEventCallback)
		m_pEventCallback->OnEvent(uMsg, wParam, lParam);
}

/*------------------------------------------------------------------------------
 * 描  述：窗口是否被双击放大了
 * 参  数：无
 * 返回值：是/否
------------------------------------------------------------------------------*/
bool CDisplayVideoWnd::IsVideoMaximized()
{
	return m_bIsVideoMaximized;
}

/*------------------------------------------------------------------------------
 * 描  述：显示或隐藏窗口
 * 参  数：[in] bShow 是否显示窗口
 * 返回值：无
------------------------------------------------------------------------------*/
void CDisplayVideoWnd::ShowWindow(bool bShow)
{
	m_pInfoWnd->ShowWindow(bShow);
	m_pVideoWnd->ShowWindow(bShow);
}

/*------------------------------------------------------------------------------
 * 描  述：初始化
 * 参  数：[in] hParentWnd	父窗口句柄
 * 返回值：无
------------------------------------------------------------------------------*/
void CDisplayVideoWnd::Init(HWND hParentWnd)
{
	m_hParentWnd = hParentWnd;

	m_pVideoWnd = new CVideoWnd(L"video_default.xml", L"VideoDefault");
	m_pVideoWnd->CreateWnd(hParentWnd, L"VideoWnd", UI_WNDSTYLE_CHILD, true);
	((CVideoWnd*)m_pVideoWnd)->SetEventCallback(this);

	m_pInfoWnd = new CMediaInfoWnd(L"video_info.xml", L"VideoInfo");
	m_pInfoWnd->CreateWnd(hParentWnd, L"InfoWnd", UI_WNDSTYLE_CHILD, true);
	m_pInfoWnd->GetPaintManager().AddNotifier(this);
}

/*------------------------------------------------------------------------------
 * 描  述：设置窗口的矩形区域
 * 参  数：[in] rect 矩形区域
 *         [in] bShow 改变矩形区域后，是否立即显示
 * 返回值：无
------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------
 * 描  述：广播麦克风后，更改麦克风图标
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
void CDisplayVideoWnd::OpenMic()
{
	CLabelUI* pMicLabel = (CLabelUI*)m_pInfoWnd->GetPaintManager().FindControl(L"label_mic");
	pMicLabel->SetBkImage(L"img\\video\\video_stat_audio_open.png");
}

/*------------------------------------------------------------------------------
 * 描  述：取消广播麦克风后，更改麦克风图标
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
void CDisplayVideoWnd::CloseMic()
{
	CLabelUI* pMicLabel = (CLabelUI*)m_pInfoWnd->GetPaintManager().FindControl(L"label_mic");
	pMicLabel->SetBkImage(L"img\\video\\video_stat_audio.png");

	CLabelUI* pVolLabel = (CLabelUI*)m_pInfoWnd->GetPaintManager().FindControl(L"label_volume");
	pVolLabel->SetBkImage(L"img\\video\\video_stats_volume_bg.png");
}

/*------------------------------------------------------------------------------
 * 描  述：根据麦克风声音能量值大小，改变能量值图片
 * 参  数：[in] dwEnergy 麦克风声音能量值大小
 * 返回值：无
------------------------------------------------------------------------------*/
void CDisplayVideoWnd::OnMicEnergyChange(DWORD dwEnergy)
{
	WCHAR szBkImage[128];
	if (dwEnergy / 10 == 0)
		_snwprintf(szBkImage, 128, L"img\\video\\video_stats_volume_bg.png");
	else
		_snwprintf(szBkImage, 128, L"img\\video\\video_stats_volume%d.png", dwEnergy / 10);

	CLabelUI* pMicLabel = (CLabelUI*)m_pInfoWnd->GetPaintManager().FindControl(L"label_volume");
	pMicLabel->SetBkImage(szBkImage);
}

/*------------------------------------------------------------------------------
 * 描  述：刷新流量/帧率/分辨率显示
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
void CDisplayVideoWnd::OnVideoStatsChange(LPCTSTR szVideoStats)
{
	CLabelUI* pVideoInfoLabel = (CLabelUI*)m_pInfoWnd->GetPaintManager().FindControl(L"label_video_info");
	pVideoInfoLabel->SetText(szVideoStats);
}

/*------------------------------------------------------------------------------
 * 描  述：构建菜单项
 * 参  数：[in] szName		菜单项名字
 *         [in] szText		显示的文本
 *         [in] bRadio		是否是单选按钮
 *         [in] bSelected	是否默认选择
 *         [in] bHasSubMenu	是否有子菜单项
 * 返回值：菜单项指针
------------------------------------------------------------------------------*/
CMenuElementUI* CDisplayVideoWnd::ConstructMenuItem(LPCTSTR szName, LPCTSTR szText, bool bRadio, bool bSelected, bool bHasSubMenu)
{
	CMenuElementUI* pElement = new CMenuElementUI;
	pElement->SetAttribute(L"width", L"180");
	pElement->SetAttribute(L"height", L"36");

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

/*------------------------------------------------------------------------------
 * 描  述：构建菜单
 * 参  数：[in] pMenu 菜单对象
 * 返回值：无
------------------------------------------------------------------------------*/
void CDisplayVideoWnd::ConstructMenu(CMenuWnd* pMenu)
{
	// FIXME: xml布局文件中额外增加了一个空的MenuElement作为占位符，否则动态添加的
	// 菜单项无法显示，可能是个bug，暂时没有时间去解决，这里首先要移除占位符
	((CMenuUI*)pMenu->m_pm.GetRoot())->Remove(pMenu->m_pm.FindControl(L"placeholder"));

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
			_snwprintf(szOptionName, 16, L"menu_cam_%d", dwCamIndex);

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

	pMenu->ResizeMenu();
}

/*------------------------------------------------------------------------------
 * 描  述：设置视频显示模式：屏幕/等比裁剪/等比完整
 * 参  数：[in] mode 显示模式
 * 返回值：无
------------------------------------------------------------------------------*/
void CDisplayVideoWnd::SetVideoRenderMode(fsp::RenderMode mode)
{
	if (m_bHasVideo && m_eRenderMode != mode)
		CSdkManager::GetInstance().GetFspEngin()->SetRenderMode(GetVideoWnd(), mode);

	m_eRenderMode = mode;
}

/*------------------------------------------------------------------------------
 * 描  述：切换摄像头处理
 * 参  数：[in] dwCamIndex 新的摄像头索引
 * 返回值：无
------------------------------------------------------------------------------*/
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
		pEngin->AddVideoPreview(dwCamIndex, GetVideoWnd());
		pEngin->SetRenderMode(GetVideoWnd(), m_eRenderMode);

		// 通知主界面广播的摄像头变化了
		CSdkManager::GetInstance().GetFrameWnd()->SendMessageW(
			DUILIB_MSG_BROADCAST_CAM_CHANGED, m_dwCamIndex, dwCamIndex);

		m_dwCamIndex = dwCamIndex;
	}
}

/*------------------------------------------------------------------------------
 * 描  述：DUILIB的通知消息处理
 * 参  数：[in] msg 通知消息
 * 返回值：无
------------------------------------------------------------------------------*/
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
}

/*------------------------------------------------------------------------------
 * 描  述：定时器处理
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
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
			_snwprintf(szVideoStats, 32,
				L"%dK %dF %d*%d",
				statsInfo.bitrate / 1024,
				statsInfo.framerate,
				statsInfo.width,
				statsInfo.height);
			OnVideoStatsChange(szVideoStats);
		}
	}
}

/*------------------------------------------------------------------------------
 * 描  述：窗口是否空闲，既没有音频也没有视频
 * 参  数：无
 * 返回值：是/否
------------------------------------------------------------------------------*/
bool CDisplayVideoWnd::IsIdle()
{
	return !m_bHasAudio && !m_bHasVideo;
}

/*------------------------------------------------------------------------------
 * 描  述：窗口是否正在显示远端用户的某一路视频
 * 参  数：[in] strUserId	用户ID
 *         [in] strVideoId	视频ID
 * 返回值：是/否
------------------------------------------------------------------------------*/
bool CDisplayVideoWnd::IsHoldRemoteVideo(const fsp::String& strUserId, const fsp::String& strVideoId)
{
	return m_bHasVideo 
		&& (strcmp(m_strUserId.c_str(), strUserId.c_str()) == 0) 
		&& (strcmp(m_strVideoId.c_str(), strVideoId.c_str()) == 0);
}

/*------------------------------------------------------------------------------
 * 描  述：窗口是否正在显示远端用户的视频
 * 参  数：[in] strUserId	用户ID
 * 返回值：是/否
------------------------------------------------------------------------------*/
bool CDisplayVideoWnd::IsHoldRemoteVideo(const fsp::String& strUserId)
{
	return m_bHasVideo && (strcmp(m_strUserId.c_str(), strUserId.c_str()) == 0);
}

/*------------------------------------------------------------------------------
 * 描  述：窗口是否正在显示远端用户的音频
 * 参  数：[in] strUserId	用户ID
 * 返回值：是/否
------------------------------------------------------------------------------*/
bool CDisplayVideoWnd::IsHoldRemoteAudio(const fsp::String& strUserId)
{
	return m_bHasAudio && (strcmp(m_strUserId.c_str(), strUserId.c_str()) == 0);
}

/*------------------------------------------------------------------------------
 * 描  述：窗口是否正在显示本地某一路视频
 * 参  数：[in] dwCamIndex 摄像头索引
 * 返回值：是/否
------------------------------------------------------------------------------*/
bool CDisplayVideoWnd::IsHoldLocalVideo(DWORD dwCamIndex)
{
	return m_bHasVideo && m_bIsLocal && m_dwCamIndex == dwCamIndex;
}

/*------------------------------------------------------------------------------
 * 描  述：窗口是否正在显示本地视频
 * 参  数：无
 * 返回值：是/否
------------------------------------------------------------------------------*/
bool CDisplayVideoWnd::IsHoldLocalVideo()
{
	return m_bHasVideo && m_bIsLocal;
}

/*------------------------------------------------------------------------------
 * 描  述：窗口是否正在显示本地某一路音频
 * 参  数：[in] dwMicIndex 麦克风索引
 * 返回值：是/否
------------------------------------------------------------------------------*/
bool CDisplayVideoWnd::IsHoldLocalAudio(DWORD dwMicIndex)
{
	return m_bHasAudio && m_bIsLocal && m_dwMicIndex == dwMicIndex;
}

/*------------------------------------------------------------------------------
 * 描  述：窗口是否正在显示本地音频
 * 参  数：无
 * 返回值：是/否
------------------------------------------------------------------------------*/
bool CDisplayVideoWnd::IsHoldLocalAudio()
{
	return m_bHasAudio && m_bIsLocal;
}

/*------------------------------------------------------------------------------
 * 描  述：刷新用户图标和用户名称
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
void CDisplayVideoWnd::RefreshUserName()
{
	CLabelUI* pUserLabel = (CLabelUI*)m_pInfoWnd->GetPaintManager().FindControl(L"label_user");
	CLabelUI* pIconLabel = (CLabelUI*)m_pInfoWnd->GetPaintManager().FindControl(L"label_user_icon");

	if (IsIdle())
	{
		pIconLabel->SetBkImage(L"img\\video\\video_user_disable.png");
		pUserLabel->SetText(L"Nobody");
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
}

/*------------------------------------------------------------------------------
 * 描  述：广播摄像头处理
 * 参  数：[in] dwCamIndex 摄像头索引
 * 返回值：成功/失败
------------------------------------------------------------------------------*/
bool CDisplayVideoWnd::StartPublishCam(DWORD dwCamIndex)
{
	if (m_bHasVideo)
		return false;

	fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();

	// 广播本地视频
	char szVideoId[32];
	sprintf(szVideoId, "LocalCam_%d", dwCamIndex);
	if (fsp::ERR_OK != pEngin->StartPublishVideo(szVideoId, dwCamIndex))
	{
		PostMessageW(CSdkManager::GetInstance().GetFrameWnd()->GetHWND(), 
			DUILIB_MSG_BROADCAST_CAM_FAILED, 0, 0);
		return false;
	}

	// 显示本地视频
	pEngin->AddVideoPreview(dwCamIndex, GetVideoWnd());
	pEngin->SetRenderMode(GetVideoWnd(), m_eRenderMode);
	
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

/*------------------------------------------------------------------------------
 * 描  述：停止广播摄像头处理
 * 参  数：[in] dwCamIndex 摄像头索引
 * 返回值：成功/失败
------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------
 * 描  述：添加远端某用户的某一路视频
 * 参  数：[in] strUserId	用户ID
 *         [in] strVideoId	视频ID
 * 返回值：无
------------------------------------------------------------------------------*/
void CDisplayVideoWnd::AddRemoteVideo(const fsp::String& strUserId, const fsp::String& strVideoId)
{
	fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();

	pEngin->SetRemoteVideoRender(strUserId, strVideoId, GetVideoWnd());
	pEngin->SetRenderMode(GetVideoWnd(), m_eRenderMode);

	m_dwCamIndex = 0;
	m_bHasVideo = true;
	m_bIsLocal = false;
	m_strVideoId = strVideoId;
	m_strUserId = strUserId;

	RefreshUserName();
}

/*------------------------------------------------------------------------------
 * 描  述：删除远端某用户的某一路视频
 * 参  数：[in] strUserId	用户ID
 *         [in] strVideoId	视频ID
 * 返回值：无
------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------
 * 描  述：添加远端某用户的音频
 * 参  数：[in] strUserId	用户ID
 * 返回值：无
------------------------------------------------------------------------------*/
void CDisplayVideoWnd::AddRemoteAudio(const fsp::String& strUserId)
{
	if (m_bHasVideo && strcmp(m_strUserId.c_str(), strUserId.c_str()) != 0)
		return;

	m_bIsLocal = false;
	m_bHasAudio = true;
	m_strUserId = strUserId;

	OpenMic();
	RefreshUserName();
}

/*------------------------------------------------------------------------------
 * 描  述：删除远端某用户的音频
 * 参  数：[in] strUserId	用户ID
 * 返回值：无
------------------------------------------------------------------------------*/
void CDisplayVideoWnd::DelRemoteAudio(const fsp::String& strUserId)
{
	if (m_bHasAudio && strcmp(m_strUserId.c_str(), strUserId.c_str()) == 0)
		m_bHasAudio = false;

	CloseMic();
	RefreshUserName();
}

/*------------------------------------------------------------------------------
 * 描  述：广播本地麦克风
 * 参  数：[in] dwMicIndex 麦克风索引
 * 返回值：无
------------------------------------------------------------------------------*/
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
	m_dwMicIndex	= dwMicIndex;
	m_bIsLocal		= true;
	m_strUserId		= szUserId;

	OpenMic();
	RefreshUserName();
}

/*------------------------------------------------------------------------------
 * 描  述：取消广播本地麦克风
 * 参  数：[in] dwMicIndex 麦克风索引
 * 返回值：成功/失败
------------------------------------------------------------------------------*/
bool CDisplayVideoWnd::StopPublishMic(DWORD dwMicIndex)
{
	if (!m_bHasAudio || m_dwMicIndex != dwMicIndex)
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

/*------------------------------------------------------------------------------
 * 描  述：视频参数改变，要重新设置
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
void CDisplayVideoWnd::OnVideoParamChanged()
{
	if (m_bHasVideo && m_bIsLocal)
		SetVideoParam();
}