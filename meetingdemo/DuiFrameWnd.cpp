/*##############################################################################
 * 文件：DuiFrameWnd.cpp
 * 描述：主窗口实现
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/

#include "stdafx.h"
#include "DuiFrameWnd.h"
#include "core/UIDefine.h"
#include "UIMenu.h"
#include "ToolbarMenu.h"
#include "fsp_engine.h"
#include "SdkManager.h"
#include "DuiSettingWnd.h"
#include "util.h"
#include "define.h"

#define MENU_ITEM_HEIGHT	36
#define MENU_ITEM_WIDTH		220

// 消息映射定义
DUI_BEGIN_MESSAGE_MAP(CDuiFrameWnd, WindowImplBase)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_END_MESSAGE_MAP()

/*------------------------------------------------------------------------------
 * 描  述：构造函数
 * 参  数：无
 * 返回值：无
 ------------------------------------------------------------------------------*/
CDuiFrameWnd::CDuiFrameWnd()
	: m_bVideoWndInitFlag(false)
	, m_bBroadcastMic(false)
	, m_bOpenSpeaker(false)
	, m_bPlayAudio(false)
	, m_bRecord(false)
{
}

/*------------------------------------------------------------------------------
 * 描  述：析构函数
 * 参  数：无
 * 返回值：无
 ------------------------------------------------------------------------------*/
CDuiFrameWnd::~CDuiFrameWnd()
{
	if (m_bRecord)
		StopRecord();
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，DUI调用获取窗口xml布局文件目录
 * 参  数：无
 * 返回值：xml布局文件目录
 ------------------------------------------------------------------------------*/
CDuiString CDuiFrameWnd::GetSkinFolder()
{
	return CDuiString(L"skin");
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，DUI调用获取窗口xml布局文件名称
 * 参  数：无
 * 返回值：xml布局文件名称
 ------------------------------------------------------------------------------*/
CDuiString CDuiFrameWnd::GetSkinFile()
{
	return CDuiString(L"main.xml");
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，DUI调用获取窗口类名称
 * 参  数：无
 * 返回值：窗口类名称
 ------------------------------------------------------------------------------*/
LPCTSTR CDuiFrameWnd::GetWindowClassName(void) const
{
	return L"CDuiFrameWnd";
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，DUI调用完成窗口初始化
 * 参  数：无
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiFrameWnd::InitWindow()
{
	if (!m_bVideoWndInitFlag)
	{
		m_VideoWndMgr.Init(GetHWND());
		m_bVideoWndInitFlag = true;
	}

	WCHAR szCaption[128];
	_snwprintf(szCaption, 64, L"Group ID：%s    User ID：%s", 
		CSdkManager::GetInstance().GetLoginGroup().GetData(), 
		CSdkManager::GetInstance().GetLoginUser().GetData());

	CLabelUI* pCapLabel = (CLabelUI*)m_PaintManager.FindControl(L"group_user");
	pCapLabel->SetText(szCaption);

	m_PaintManager.SetTimer(m_PaintManager.FindControl(L"btn_mic"), 
		UPDATE_VIDEO_INFO_TIMER_ID, UPDATE_VIDEO_INFO_INTERVAL);

	SetIcon(IDI_FSPCLIENT);
}

/*------------------------------------------------------------------------------
 * 描  述：刷新主窗口工具栏麦克风背景图片，广播状态和未广播状态的背景图片不一样
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
void CDuiFrameWnd::RefreshMicBtnBkImg()
{
	CControlUI* pBtnMic = m_PaintManager.FindControl(L"btn_mic");
	if (m_bBroadcastMic)
	{
		pBtnMic->SetAttribute(L"normalimage", L"img\\video\\toolbar_mic_open.png");
		pBtnMic->SetAttribute(L"hotimage", L"img\\video\\toolbar_mic_open_hot.png");
		pBtnMic->SetAttribute(L"pushedimage", L"img\\video\\toolbar_mic_open_pressed.png");
	}
	else
	{
		pBtnMic->SetAttribute(L"normalimage", L"img\\video\\toolbar_mic.png");
		pBtnMic->SetAttribute(L"hotimage", L"img\\video\\toolbar_mic_hot.png");
		pBtnMic->SetAttribute(L"pushedimage", L"img\\video\\toolbar_mic_pressed.png");
	}
}

/*------------------------------------------------------------------------------
 * 描  述：刷新主窗口工具栏录制背景图片
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
void CDuiFrameWnd::RefreshRecordBtnBkImg()
{
	CControlUI* pBtnRecord = m_PaintManager.FindControl(L"btn_record");
	if (m_bRecord)
	{
		pBtnRecord->SetAttribute(L"normalimage", L"img\\video\\toolbar_recorder_open.png");
		pBtnRecord->SetAttribute(L"hotimage", L"img\\video\\toolbar_recorder_open_hot.png");
		pBtnRecord->SetAttribute(L"pushedimage", L"img\\video\\toolbar_recorder_open_pressed.png");
	}
	else
	{
		pBtnRecord->SetAttribute(L"normalimage", L"img\\video\\toolbar_recorder.png");
		pBtnRecord->SetAttribute(L"hotimage", L"img\\video\\toolbar_recorder_hot.png");
		pBtnRecord->SetAttribute(L"pushedimage", L"img\\video\\toolbar_recorder_pressed.png");
	}
}

/*------------------------------------------------------------------------------
 * 描  述：刷新主窗口工具栏摄像头背景图片，广播状态和未广播状态的背景图片不一样
 * 参  数：无
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiFrameWnd::RefreshCamBtnBkImg()
{
	CControlUI* pBtnAud = m_PaintManager.FindControl(L"btn_cam");
	if (!m_setCamOpenIndexes.empty())
	{
		pBtnAud->SetAttribute(L"normalimage", L"img\\video\\toolbar_cam_open.png");
		pBtnAud->SetAttribute(L"hotimage", L"img\\video\\toolbar_cam_open_hot.png");
		pBtnAud->SetAttribute(L"pushedimage", L"img\\video\\toolbar_cam_open_pressed.png");
	}
	else
	{
		pBtnAud->SetAttribute(L"normalimage", L"img\\video\\toolbar_cam.png");
		pBtnAud->SetAttribute(L"hotimage", L"img\\video\\toolbar_cam_hot.png");
		pBtnAud->SetAttribute(L"pushedimage", L"img\\video\\toolbar_cam_pressed.png");
	}
}

/*------------------------------------------------------------------------------
 * 描  述：刷新主窗口工具栏播放音频背景图片
 * 参  数：无
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiFrameWnd::RefreshAudBtnBkImg()
{
	CControlUI* pBtnAud = m_PaintManager.FindControl(L"btn_aud");
	if (m_bPlayAudio)
	{
		pBtnAud->SetAttribute(L"normalimage", L"img\\video\\toolbar_audio_open.png");
		pBtnAud->SetAttribute(L"hotimage", L"img\\video\\toolbar_audio_open_hot.png");
		pBtnAud->SetAttribute(L"pushedimage", L"img\\video\\toolbar_audio_open_pressed.png");
	}
	else
	{
		pBtnAud->SetAttribute(L"normalimage", L"img\\video\\toolbar_audio.png");
		pBtnAud->SetAttribute(L"hotimage", L"img\\video\\toolbar_audio_hot.png");
		pBtnAud->SetAttribute(L"pushedimage", L"img\\video\\toolbar_audio_pressed.png");
	}
}

/*------------------------------------------------------------------------------
 * 描  述：摄像头选择变化处理（通过菜单）
 * 参  数：[in] msg 通知消息
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiFrameWnd::OnCamSelectChanged(TNotifyUI& msg)
{
	DWORD dwCamIndex = _wtoi(msg.pSender->GetName().GetData() + 4);

	COptionUI* pCamOpt = (COptionUI*)msg.pSender;

	if (pCamOpt->IsSelected())
	{
		if (m_setCamOpenIndexes.end() != m_setCamOpenIndexes.find(dwCamIndex))
			return; // 异常情况
		
		//最多允许广播两路视频!
		if (m_setCamOpenIndexes.size() < 2)
		{
			m_setCamOpenIndexes.insert(dwCamIndex);
			if (!m_VideoWndMgr.AddBroadcastCam(dwCamIndex))
				pCamOpt->Selected(false);
		}
		else
		{
			pCamOpt->SetAttribute(L"selected", L"false");

			// 菜单消息处理，不能直接弹出MessageBox，否则会出现因为菜单失去焦点
			// 销毁后又进行消息处理而导致崩溃，所以这里发消息到主线程弹框提示
			this->PostMessageW(DUILIB_MSG_TOO_MANY_CAM, 0, 0);
		}
	}
	else // 未选中状态也要处理
	{
		if (m_setCamOpenIndexes.end() != m_setCamOpenIndexes.find(dwCamIndex))
		{
			m_setCamOpenIndexes.erase(dwCamIndex);
			m_VideoWndMgr.DelBroadcastCam(dwCamIndex);
		}
	}

	RefreshCamBtnBkImg();
}

/*------------------------------------------------------------------------------
 * 描  述：Option控件选择变化通知处理，消息映射处理方法
 * 参  数：[in] msg 通知消息
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiFrameWnd::OnSelectChanged(TNotifyUI& msg)
{
	if (_tcsncmp(msg.pSender->GetName(), L"cam_", 4) == 0)
	{
		OnCamSelectChanged(msg);
	}
}

/*------------------------------------------------------------------------------
 * 描  述：点击主窗口工具栏麦克风按钮处理方法
 * 参  数：[in] msg 通知消息
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiFrameWnd::OnClickMicBtn(TNotifyUI& msg)
{
	DWORD dwMicIndex = CSdkManager::GetInstance().GetOpenMic();

	if (dwMicIndex == INVALID_MIC_INDEX)
	{
		m_bBroadcastMic = false;
		return;
	}
		
	if (m_bBroadcastMic)
	{
		m_VideoWndMgr.DelBroadcastMic(dwMicIndex);
		m_bBroadcastMic = false;
	}
	else
	{
		m_VideoWndMgr.AddBroadcastMic(dwMicIndex);
		m_bBroadcastMic = true;
	}

	RefreshMicBtnBkImg();
}

/*------------------------------------------------------------------------------
 * 描  述：点击主窗口工具栏播放音频按钮处理方法
 * 参  数：[in] msg 通知消息
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiFrameWnd::OnClickAudBtn(TNotifyUI& msg)
{
	if (m_bPlayAudio)
	{
		CSdkManager::GetInstance().GetFspEngin()->StopPlayAudio();
		m_bPlayAudio = false;
	}
	else
	{
		CSdkManager::GetInstance().GetFspEngin()->StartPlayAudio();
		m_bPlayAudio = true;
	}

	RefreshAudBtnBkImg();
}

/*------------------------------------------------------------------------------
 * 描  述：摄像头是否已经被广播了
 * 参  数：[in] dwCamIndex 摄像头索引
 * 返回值：是/否
 ------------------------------------------------------------------------------*/
bool CDuiFrameWnd::IsCamOpened(DWORD dwCamIndex)
{
	return m_setCamOpenIndexes.end() != m_setCamOpenIndexes.find(dwCamIndex);
}

/*------------------------------------------------------------------------------
 * 描  述：点击主界面工具栏摄像头按钮处理
 * 参  数：[in] msg 通知消息
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiFrameWnd::OnClickCamBtn(TNotifyUI& msg)
{
	// 创建菜单
	CToolbarMenu* pMenu = new CToolbarMenu(GetHWND(), &m_PaintManager, this);
	if (!pMenu || !pMenu->Init(MENU_ITEM_HEIGHT, MENU_ITEM_WIDTH, 20))
		return;

	// 从SDK获取所有可用摄像头
	fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();
	fsp::Vector<VideoDeviceInfo> vecCam = pEngin->GetDeviceManager()->GetCameraDevices();

	// 将摄像头添加到菜单项
	DWORD dwMenuItemIndex = 0;
	for (auto iter = vecCam.begin(); iter != vecCam.end(); ++iter)
	{
		WCHAR szMenuItemName[64];
		swprintf(szMenuItemName, L"cam_menu_%d", dwMenuItemIndex);

		WCHAR szOptionName[64];
		swprintf(szOptionName, L"cam_%d", dwMenuItemIndex);

		WCHAR szCamName[128];
		demo::ConvertUtf8ToUnicode(iter->device_name.c_str(), szCamName, 128);

		pMenu->AddMenuItem(szMenuItemName,
			CToolbarMenu::MENU_ITEM_TYPE_CHECKBOX,
			szOptionName,
			szCamName,
			L"",
			IsCamOpened(dwMenuItemIndex));

		dwMenuItemIndex++;
	}

	// 计算菜单显示位置（居中显示）
	RECT rcBtnPos = msg.pSender->GetClientPos();
	CDuiPoint point;
	point.x = (rcBtnPos.right + rcBtnPos.left) / 2 - MENU_ITEM_WIDTH / 2;
	point.y = rcBtnPos.top - 5 - MENU_ITEM_HEIGHT * dwMenuItemIndex;
	ClientToScreen(m_hWnd, &point);

	// 显示菜单
	pMenu->ShowMenu(point);
}

/*------------------------------------------------------------------------------
 * 描  述：点击主界面工具栏设置按钮处理
 * 参  数：[in] msg 通知消息
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiFrameWnd::OnClickSettingBtn(TNotifyUI& msg)
{
	CDuiSettingWnd wndSetting;
	wndSetting.Create(m_hWnd, _T("CDuiSettingWnd"), UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
	wndSetting.CenterWindow();
	wndSetting.ShowModal();
}

/*------------------------------------------------------------------------------
 * 描  述：停止录制
 * 参  数：无
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiFrameWnd::StopRecord()
{
	// 停止录制本端视频
	fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();
	auto vecCam = pEngin->GetDeviceManager()->GetCameraDevices();
	for (auto iter = vecCam.begin(); iter != vecCam.end(); ++iter)
	{
		CSdkManager::GetInstance().StopRecordLocalVideo(iter->camera_id);
	}

	// 停止录制远端视频
	for each (RemoteVideoInfo info in m_vecRemoteVideoInfo)
	{
		CSdkManager::GetInstance().StopRecordRemoteVideo(
			info.strUserId.c_str(), info.strVideoId.c_str());
	}

	// 停止录制本端音频
	CSdkManager::GetInstance().StopRecordLocalAudio();

	// 停止录制远端音频
	for each (RemoteAudioInfo info in m_vecRemoteAudioInfo)
	{
		CSdkManager::GetInstance().StopRecordRemoteAudio(info.strUserId.c_str());
	}
}

/*------------------------------------------------------------------------------
 * 描  述：点击主界面工具栏录制按钮处理
 * 参  数：[in] msg 通知消息
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiFrameWnd::OnClickRecordBtn(TNotifyUI& msg)
{
	if (m_bRecord)
	{
		StopRecord();

		m_bRecord = false;
	}
	else
	{
		// 录制本地视频
		fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();
		auto vecCam = pEngin->GetDeviceManager()->GetCameraDevices();
		for (auto iter = vecCam.begin(); iter != vecCam.end(); ++iter)
		{
			CSdkManager::GetInstance().StartRecordLocalVideo(iter->camera_id);
		}

		// 录制远端视频
		for each (RemoteVideoInfo info in m_vecRemoteVideoInfo)
		{
			CSdkManager::GetInstance().StartRecordRemoteVideo(
				info.strUserId.c_str(), info.strVideoId.c_str());
		}

		// 录制本地音频
		CSdkManager::GetInstance().StartRecordLocalAudio();

		// 录制远端音频
		for each (RemoteAudioInfo info in m_vecRemoteAudioInfo)
		{
			CSdkManager::GetInstance().StartRecordRemoteAudio(info.strUserId.c_str());
		}

		m_bRecord = true;
	}

	RefreshRecordBtnBkImg();
}

/*------------------------------------------------------------------------------
 * 描  述：消息映射处理函数
 * 参  数：[in] msg 通知消息
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiFrameWnd::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == L"btn_mic")
	{
		OnClickMicBtn(msg);
	}
	else if (msg.pSender->GetName() == L"btn_cam")
	{
		OnClickCamBtn(msg);
	}
	else if (msg.pSender->GetName() == L"btn_aud")
	{
		OnClickAudBtn(msg);
	}
	else if (msg.pSender->GetName() == L"btn_setting")
	{
		OnClickSettingBtn(msg);
	}
	else if (msg.pSender->GetName() == L"btn_record")
	{
		OnClickRecordBtn(msg);
	}
	else
	{
		WindowImplBase::OnClick(msg);
	}
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，主界面退出时，程序需要退出
 * 参  数：[in] UINT
 *         [in] WPARAM
 *         [in] LPARAM
 *         [out] bHandled 消息是否已经被处理
 * 返回值：处理结果(LRESULT)
 ------------------------------------------------------------------------------*/
LRESULT CDuiFrameWnd::OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	::SendMessage(GetHWND(), WM_QUIT, 0, 0);
	return S_OK;
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，窗口退出之前的回调
 * 参  数：[in] hWnd 窗口句柄
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiFrameWnd::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

/*------------------------------------------------------------------------------
 * 描  述：SDK通知应用层有用户广播了音频的处理
 * 参  数：[in] wParam 携带RemoteAudioInfo结构体指针
 *         [in] lParam 没有使用
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiFrameWnd::OnAddRemoteAudio(WPARAM wParam, LPARAM lParam)
{
	RemoteAudioInfo* pInfo = (RemoteAudioInfo*)wParam;

	m_VideoWndMgr.AddRemoteAudio(pInfo->strUserId);

	m_vecRemoteAudioInfo.push_back(*pInfo);

	if (m_bRecord) // 添加录制
	{
		CSdkManager::GetInstance().StartRecordRemoteAudio(pInfo->strUserId.c_str());
	}

	delete pInfo;
}

/*------------------------------------------------------------------------------
 * 描  述：SDK通知应用层有用户取消广播了音频的处理
 * 参  数：[in] wParam 携带RemoteAudioInfo结构体指针
 *         [in] lParam 没有使用
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiFrameWnd::OnDelRemoteAudio(WPARAM wParam, LPARAM lParam)
{
	RemoteAudioInfo* pInfo = (RemoteAudioInfo*)wParam;

	m_VideoWndMgr.DelRemoteAudio(pInfo->strUserId);

	for (auto iter = m_vecRemoteAudioInfo.begin(); iter != m_vecRemoteAudioInfo.end(); ++iter)
	{
		if (iter->strUserId == pInfo->strUserId)
		{
			m_vecRemoteAudioInfo.erase(iter);
			break;
		}
	}

	if (m_bRecord) // 停止录制
	{
		CSdkManager::GetInstance().StopRecordRemoteAudio(pInfo->strUserId.c_str());
	}

	delete pInfo;
}

/*------------------------------------------------------------------------------
 * 描  述：SDK通知应用层有用户广播了视频的处理
 * 参  数：[in] wParam 携带RemoteVideoInfo结构体指针
 *         [in] lParam 没有使用
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiFrameWnd::OnAddRemoteVideo(WPARAM wParam, LPARAM lParam)
{
	RemoteVideoInfo* pInfo = (RemoteVideoInfo*)wParam;

	m_VideoWndMgr.AddRemoteVideo(pInfo->strUserId, pInfo->strVideoId);

	m_vecRemoteVideoInfo.push_back(*pInfo);

	if (m_bRecord) // 添加录制
	{
		CSdkManager::GetInstance().StartRecordRemoteVideo(
			pInfo->strUserId.c_str(), pInfo->strVideoId.c_str());
	}

	delete pInfo;
}

/*------------------------------------------------------------------------------
 * 描  述：SDK通知应用层有用户取消广播了视频的处理
 * 参  数：[in] wParam 携带RemoteVideoInfo结构体指针
 *         [in] lParam 没有使用
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiFrameWnd::OnDelRemoteVideo(WPARAM wParam, LPARAM lParam)
{
	RemoteVideoInfo* pInfo = (RemoteVideoInfo*)wParam;

	m_VideoWndMgr.DelRemoteVideo(pInfo->strUserId, pInfo->strVideoId);

	for (auto iter = m_vecRemoteVideoInfo.begin(); iter != m_vecRemoteVideoInfo.end(); ++iter)
	{
		if (iter->strUserId == pInfo->strUserId && iter->strVideoId == pInfo->strVideoId)
		{
			m_vecRemoteVideoInfo.erase(iter);
			break;
		}
	}

	if (m_bRecord) // 停止录制
	{
		CSdkManager::GetInstance().StopRecordRemoteVideo(
			pInfo->strUserId.c_str(), pInfo->strVideoId.c_str());
	}

	delete pInfo;
}

/*------------------------------------------------------------------------------
 * 描  述：切换广播摄像头处理
 * 参  数：[in] wParam 切换之前的摄像头索引
 *         [in] lParam 切换之后的摄像头索引
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiFrameWnd::OnBroadcastCamChanged(WPARAM wParam, LPARAM lParam)
{
	DWORD dwOldCamIndex = (DWORD)wParam;
	DWORD dwNewCamIndex = (DWORD)lParam;

	if (m_setCamOpenIndexes.end() != m_setCamOpenIndexes.find(dwOldCamIndex))
	{
		m_setCamOpenIndexes.erase(dwOldCamIndex);
	}

	if (m_setCamOpenIndexes.end() == m_setCamOpenIndexes.find(dwNewCamIndex))
	{
		m_setCamOpenIndexes.insert(dwNewCamIndex);
	}
}

/*------------------------------------------------------------------------------
 * 描  述：获取主窗口中间部门显示区域
 * 参  数：无
 * 返回值：矩形区域
 ------------------------------------------------------------------------------*/
RECT CDuiFrameWnd::GetDisplayRect()
{
	RECT rect;
	::GetClientRect(GetHWND(), &rect);
	rect.top += UI_MAIN_FRAME_TITLE_HEIGHT;	
	rect.bottom -= UI_MAIN_FRAME_TOOLBAR_HEIGHT;

	return rect;
}

/*------------------------------------------------------------------------------
 * 描  述：主窗口大小变化，调整Group ID和User ID显示位置
 * 参  数：无
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiFrameWnd::AdjustTitleGroupUser()
{
	RECT rcClient = GetDisplayRect();

	RECT rectPadding;
	ZeroMemory(&rectPadding, sizeof(rectPadding));

	// 魔鬼数字参考duilib的布局文件“main.xml”
	rectPadding.left = (rcClient.right - rcClient.left) / 2 - 400 - 130;

	(m_PaintManager.FindControl(L"group_user"))->SetPadding(rectPadding);
}

/*------------------------------------------------------------------------------
 * 描  述：主窗口大小变化，调整Toolbar按钮的显示位置
 * 参  数：无
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiFrameWnd::AdjustToolbarBtn()
{
	RECT rcClient = GetDisplayRect();

	RECT rectPadding;
	ZeroMemory(&rectPadding, sizeof(rectPadding));

	// 魔鬼数字参考duilib的布局文件“main.xml”
	rectPadding.left	= (rcClient.right - rcClient.left - 460) / 2;
	rectPadding.top		= 15;
	rectPadding.right	= 20;
	rectPadding.bottom	= 15;

	(m_PaintManager.FindControl(L"btn_mic"))->SetPadding(rectPadding);
}

/*------------------------------------------------------------------------------
 * 描  述：定时器回调处理，包含刷新标题栏的上传下载速率
 * 参  数：无
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiFrameWnd::OnTimer()
{
	static int count = 0; // 用来计时1秒

	if (++count >= 5) // Timer间隔是200ms，这里设置每秒刷新上传下载速率
	{
		StreamStats stats = CSdkManager::GetInstance().GetFspEngin()->GetStreamStats(true);

		WCHAR szCaption[256];
		ZeroMemory(szCaption, sizeof(szCaption));

		_snwprintf(szCaption, 256, L"Group ID：%s    User ID：%s    Up：%d kbps    Down：%d kbps",
			CSdkManager::GetInstance().GetLoginGroup().GetData(),
			CSdkManager::GetInstance().GetLoginUser().GetData(),
			stats.send_data_size * 8 / 1024, stats.recv_data_size * 8 / 1024);

		CLabelUI* pCapLabel = (CLabelUI*)m_PaintManager.FindControl(L"group_user");
		pCapLabel->SetText(szCaption);

		count = 0;
	}
	
	m_VideoWndMgr.OnTimer(); // 界面线程刷新音视频状态信息
}

/*------------------------------------------------------------------------------
 * 描  述：SDK通知应用层有用户广播了视频的处理
 * 参  数：[in] uMsg		消息类型
 *         [in] wParam		自定义
 *         [in] lParam		自定义
 *         [out] bHandled	消息是否已经被处理
 * 返回值：处理结果
 ------------------------------------------------------------------------------*/
LRESULT CDuiFrameWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_bVideoWndInitFlag)
		return S_OK;

	switch (uMsg)
	{
	case WM_TIMER:
		OnTimer();
		break;

	case WM_SIZE:
		m_VideoWndMgr.SetWndRect(GetDisplayRect());
		AdjustToolbarBtn();
		AdjustTitleGroupUser();
		break;

	case WM_GETMINMAXINFO:
	{
		RECT rt;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);
		((MINMAXINFO*)lParam)->ptMaxSize.y = (rt.bottom - rt.top);
		break;
	}

	case DUILIB_MSG_ADD_REMOTE_VIDEO:
		OnAddRemoteVideo(wParam, lParam);
		break;

	case DUILIB_MSG_DEL_REMOTE_VIDEO:
		OnDelRemoteVideo(wParam, lParam);
		break;

	case DUILIB_MSG_ADD_REMOTE_AUDIO:
		OnAddRemoteAudio(wParam, lParam);
		break;

	case DUILIB_MSG_DEL_REMOTE_AUDIO:
		OnDelRemoteAudio(wParam, lParam);
		break;

	case DUILIB_MSG_TOO_MANY_CAM:
		demo::ShowMessageBox(m_hWnd, STRING_TOO_MANY_CAM);
		break;

	case DUILIB_MSG_BROADCAST_CAM_CHANGED:
		OnBroadcastCamChanged(wParam, lParam);
		break;

	case DUILIB_MSG_CHANGE_CAM_FAILED:
		demo::ShowMessageBox(m_hWnd, STRING_CHANGE_CAM_FAILED);
		break;

	case DUILIB_MSG_BROADCAST_CAM_FAILED:
		demo::ShowMessageBox(m_hWnd, STRING_BROADCAST_CAM_FAILED);
		break;

	case DUILIB_MSG_VIDEO_PARAM_CHANGED:
		m_VideoWndMgr.OnVideoParamChanged();
		break;

	default:
		break;
	}

	return S_OK;
}