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


CDuiFrameWnd::CDuiFrameWnd()
{
	m_bVideoWndInitFlag = false;
	m_bBroadcastMic = false;
	m_isScreenSharing = false;
}


CDuiFrameWnd::~CDuiFrameWnd()
{

}

void CDuiFrameWnd::ResetWindowStatus()
{
	m_VideoWndMgr.DelAllAV();
	
	SetToolbarCamBtnStatus(false);
	SetToolbarMicBtnStatus(false);
	SetToolbarScreenshareBtnStatus(false);
}

CDuiString CDuiFrameWnd::GetSkinFolder()
{
	return CDuiString(L"skin");
}


CDuiString CDuiFrameWnd::GetSkinFile()
{
	return CDuiString(L"main.xml");
}


LPCTSTR CDuiFrameWnd::GetWindowClassName(void) const
{
	return L"CDuiFrameWnd";
}


void CDuiFrameWnd::InitWindow()
{
	if (!m_bVideoWndInitFlag)
	{
		m_VideoWndMgr.Init(GetHWND());
		m_bVideoWndInitFlag = true;
	}

	m_PaintManager.SetTimer(m_PaintManager.FindControl(L"btn_mic"), 
		UPDATE_VIDEO_INFO_TIMER_ID, UPDATE_VIDEO_INFO_INTERVAL);

	SetIcon(IDI_FSPCLIENT);
}


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

	SetToolbarCamBtnStatus(!m_setCamOpenIndexes.empty());
}


void CDuiFrameWnd::OnSelectChanged(TNotifyUI& msg)
{
	if (_tcsncmp(msg.pSender->GetName(), L"cam_", 4) == 0)
	{
		OnCamSelectChanged(msg);
	}
}


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

	SetToolbarMicBtnStatus(m_bBroadcastMic);
}


bool CDuiFrameWnd::IsCamOpened(DWORD dwCamIndex)
{
	return m_setCamOpenIndexes.end() != m_setCamOpenIndexes.find(dwCamIndex);
}


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
		swprintf_s(szMenuItemName, L"cam_menu_%d", dwMenuItemIndex);

		WCHAR szOptionName[64];
		swprintf_s(szOptionName, L"cam_%d", dwMenuItemIndex);

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


void CDuiFrameWnd::OnClickSettingBtn(TNotifyUI& msg)
{
	m_pSettingWnd = new CDuiSettingWnd();
	m_pSettingWnd->Create(m_hWnd, _T("CDuiSettingWnd"), UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
	m_pSettingWnd->CenterWindow();
	m_pSettingWnd->ShowModal();

	//如果改变了共享区域，生效共享设置
	if (m_isScreenSharing) {
		ScreenShareConfig config = CSdkManager::GetInstance().GetScreenShareConfig();
		CSdkManager::GetInstance().GetFspEngin()->StartPublishScreenShare(
			config.left, config.top, config.right, config.bottom, config.qualityBias);
	}

	delete m_pSettingWnd;
	m_pSettingWnd = nullptr;
}

void CDuiFrameWnd::OnClickScreenShareBtn(TNotifyUI& msg)
{
	if (m_isScreenSharing) {
		CSdkManager::GetInstance().GetFspEngin()->StopPublishScreenShare();
		m_isScreenSharing = false;
	}
	else {
		ScreenShareConfig config = CSdkManager::GetInstance().GetScreenShareConfig();
		CSdkManager::GetInstance().GetFspEngin()->StartPublishScreenShare(
			config.left, config.top, config.right, config.bottom, config.qualityBias);
		m_isScreenSharing = true;
	}

	SetToolbarScreenshareBtnStatus(m_isScreenSharing);
}


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
	else if (msg.pSender->GetName() == L"btn_screenshare") {
		OnClickScreenShareBtn(msg);
	}
	else if (msg.pSender->GetName() == L"btn_setting")
	{
		OnClickSettingBtn(msg);
	}
	else
	{
		WindowImplBase::OnClick(msg);
	}
}


void CDuiFrameWnd::OnAddRemoteAudio(WPARAM wParam, LPARAM lParam)
{
	RemoteAudioInfo* pInfo = (RemoteAudioInfo*)wParam;

	m_VideoWndMgr.AddRemoteAudio(pInfo->strUserId);

	m_vecRemoteAudioInfo.push_back(*pInfo);

	delete pInfo;
}


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
	
	delete pInfo;
}


void CDuiFrameWnd::OnAddRemoteVideo(WPARAM wParam, LPARAM lParam)
{
	RemoteVideoInfo* pInfo = (RemoteVideoInfo*)wParam;

	m_VideoWndMgr.AddRemoteVideo(pInfo->strUserId, pInfo->strVideoId);

	m_vecRemoteVideoInfo.push_back(*pInfo);

	delete pInfo;
}


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
	delete pInfo;
}


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


RECT CDuiFrameWnd::GetDisplayRect()
{
	RECT rect;
	::GetClientRect(GetHWND(), &rect);
	rect.top += UI_MAIN_FRAME_TITLE_HEIGHT;	
	rect.bottom -= UI_MAIN_FRAME_TOOLBAR_HEIGHT;

	return rect;
}


void CDuiFrameWnd::AdjustTitleGroupUser()
{
	RECT rcClient = GetDisplayRect();

	RECT rectPadding;
	ZeroMemory(&rectPadding, sizeof(rectPadding));

	// 魔鬼数字参考duilib的布局文件“main.xml”
	rectPadding.left = (rcClient.right - rcClient.left) / 2 - 400 - 130;

	(m_PaintManager.FindControl(L"group_user"))->SetPadding(rectPadding);
}


void CDuiFrameWnd::OnTimer()
{
	m_VideoWndMgr.OnTimer(); // 界面线程刷新音视频状态信息
}

void CDuiFrameWnd::SetToolbarCamBtnStatus(bool isOpen)
{
	CControlUI* pBtnAud = m_PaintManager.FindControl(L"btn_cam");
	if (isOpen)
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

void CDuiFrameWnd::SetToolbarMicBtnStatus(bool isOpen)
{
	CControlUI* pBtnMic = m_PaintManager.FindControl(L"btn_mic");
	if (isOpen)
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

void CDuiFrameWnd::SetToolbarScreenshareBtnStatus(bool isOpen)
{
	CControlUI* pBtnMic = m_PaintManager.FindControl(L"btn_screenshare");
	if (isOpen)
	{
		pBtnMic->SetAttribute(L"normalimage", L"img\\video\\toolbar_share_open.png");
		pBtnMic->SetAttribute(L"hotimage", L"img\\video\\toolbar_share_open_hot.png");
		pBtnMic->SetAttribute(L"pushedimage", L"img\\video\\toolbar_share_open_pressed.png");
	}
	else
	{
		pBtnMic->SetAttribute(L"normalimage", L"img\\video\\toolbar_share.png");
		pBtnMic->SetAttribute(L"hotimage", L"img\\video\\toolbar_share_hot.png");
		pBtnMic->SetAttribute(L"pushedimage", L"img\\video\\toolbar_share_pressed.png");
	}
}

LRESULT CDuiFrameWnd::OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	PostQuitMessage(0);
	return 0;
}


LRESULT CDuiFrameWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_bVideoWndInitFlag)
		return S_OK;

	switch (uMsg)
	{
	case WM_SHOWWINDOW:
	{
		WCHAR szCaption[128];
		_snwprintf_s(szCaption, 64, L"Group ID：%s    User ID：%s",
			CSdkManager::GetInstance().GetLoginGroup().GetData(),
			CSdkManager::GetInstance().GetLoginUser().GetData());

		CLabelUI* pCapLabel = (CLabelUI*)m_PaintManager.FindControl(L"group_user");
		pCapLabel->SetText(szCaption);
		::SetWindowText(GetHWND(), szCaption);
		break;
	}
	case WM_TIMER:
		OnTimer();
		break;

	case WM_SIZE:
		m_VideoWndMgr.SetWndRect(GetDisplayRect());
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

	case DUILIB_MSG_REMOTECONTROL_EVENT: {
		RemoteControlInfo* pInfo = (RemoteControlInfo*)wParam;
	
		WCHAR wszRemoteUserId[128] = { 0 };
		demo::ConvertUtf8ToUnicode(pInfo->strUserId.c_str(),
			wszRemoteUserId, 128);

		if (pInfo->operationType == fsp::REMOTE_CONTROL_REQUEST) {
			CMessageBox* pMB = new CMessageBox;
			CDuiString strTip;
			strTip.Format(L"%s 请求远程控制", wszRemoteUserId);
			pMB->Create(GetHWND(), _T("CDuiLoginWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
			pMB->SetText(strTip);
			pMB->SetBtnText(L"拒绝", L"接受");
			pMB->CenterWindow();
			if (IDOK == pMB->ShowModal()) {
				CSdkManager::GetInstance().GetFspEngin()->RemoteControlOperation(
					pInfo->strUserId, fsp::REMOTE_CONTROL_ACCEPT);
			}
			else {
				CSdkManager::GetInstance().GetFspEngin()->RemoteControlOperation(
					pInfo->strUserId, fsp::REMOTE_CONTROL_REJECT);
			}
		}
		else if (pInfo->operationType == fsp::REMOTE_CONTROL_CANCEL) {
			CDuiString strTip;
			strTip.Format(L"%s 取消了远程控制", wszRemoteUserId);
			demo::ShowMessageBox(GetHWND(), strTip);
		}
		else if (pInfo->operationType == fsp::REMOTE_CONTROL_ACCEPT) {
			CDuiString strTip;
			strTip.Format(L"%s 接受了远程控制", wszRemoteUserId);
			demo::ShowMessageBox(GetHWND(), strTip);
		}
		else if (pInfo->operationType == fsp::REMOTE_CONTROL_REJECT) {
			CDuiString strTip;
			strTip.Format(L"%s 拒绝了远程控制", wszRemoteUserId);
			demo::ShowMessageBox(GetHWND(), strTip);
		}

		m_VideoWndMgr.OnRemoteControlOperation(pInfo->strUserId, pInfo->operationType);
		
		delete pInfo;
	}
	case DUILIB_MSG_DEVICECHANGE:
	{
		if (m_pSettingWnd) {
			m_pSettingWnd->OnDeviceChanged();
		}
	}
	default:
		break;
	}

	return S_OK;
}