
#include "stdafx.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <string>
#include <sstream>

#include "MeetingMainWnd.h"
#include "core/UIDefine.h"
#include "UIMenu.h"
#include "ToolbarMenu.h"
#include "fsp_engine.h"
#include "SdkManager.h"
#include "SettingWnd.h"
#include "util.h"
#include "define.h"
#include "UIVideoPanel.h"
#include "UIVideoPanelLayout.h"

#define MENU_ITEM_HEIGHT	36
#define MENU_ITEM_WIDTH		220

#define USER_ITEM_ICON_AUDIO       1
#define USER_ITEM_ICON_VIDEO       2
#define USER_ITEM_ICON_SCREENSHARE 3

// 消息映射定义
DUI_BEGIN_MESSAGE_MAP(CMeetingMainWnd, WindowImplBase)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SETFOCUS, OnSetFocus)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_KILLFOCUS, OnKillFocus)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMSELECT,OnItemSelectChange)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnCamItemClicked)
DUI_END_MESSAGE_MAP()

CMeetingMainWnd::CMeetingMainWnd()
{
	m_bVideoWndInitFlag = false;
	m_bBroadcastMic = false;
	m_isScreenSharing = false;
	m_isRecording = false;

	m_pVideoLayout = nullptr;
	m_pScreenShareLayout = nullptr;
}

CMeetingMainWnd::~CMeetingMainWnd()
{
}

void CMeetingMainWnd::ShowGroupJoined()
{
	CenterWindow();
	ShowWindow(true);

	CDuiString strGroupInfo;

	strGroupInfo.Format(L"分组ID：%s",
		demo::Utf82WStr(CSdkManager::GetInstance().GetLoginGroupId().c_str()));
	CLabelUI* pLabelGroupInfo = (CLabelUI*)m_PaintManager.FindControl(L"label_group_info");
	pLabelGroupInfo->SetText(strGroupInfo);
}
 
void CMeetingMainWnd::ShowInviteResponse(const InviteResponseInfo& info)
{
	CDuiString strMsg;
	if (info.isAccept) {
		strMsg.Format(_T("%s 已同意加入组"), demo::Utf82WStr(info.strRemoteUserId.c_str()).GetWStr());
	}
	else {
		strMsg.Format(_T("%s 拒绝加入组"), demo::Utf82WStr(info.strRemoteUserId.c_str()).GetWStr());
	}

	AppendMsg(strMsg);
}

void CMeetingMainWnd::ResetWindowStatus()
{
	m_pVideoLayout->StopAllVideo();
	m_pScreenShareLayout->StopAllVideo();
	
	SetToolbarCamBtnStatus(false);
	SetToolbarMicBtnStatus(false);
	SetToolbarScreenshareBtnStatus(false);
}

void CMeetingMainWnd::InitUserList(const fsp::Vector<fsp::String>& user_ids)
{
	InitMainUserList(user_ids);
	InitChatUserList(user_ids);
}

void CMeetingMainWnd::InitMainUserList(const fsp::Vector<fsp::String>& user_ids)
{
	if (user_ids.empty()) 
		return;

	AddMainUserItem(CSdkManager::GetInstance().GetLoginUserId());

	auto itstart = user_ids.begin();
	auto itend = user_ids.end();
	while (itstart != itend)
	{
		AddMainUserItem(itstart->c_str());
		itstart++;
	}
}

void CMeetingMainWnd::InitChatUserList(const fsp::Vector<fsp::String>& user_ids)
{
	CComboUI *pUserCombo = (CComboUI*)(m_PaintManager.FindControl(L"main_msg_selectuser"));
	if (!pUserCombo)
		return;

	//pUserCombo->SetInternVisible(false);

	pUserCombo->RemoveAll();

	CListLabelElementUI* lte = new CListLabelElementUI();
	lte->SetText(L"所有人");
	lte->SetTag(0);//0 标识所有人
	pUserCombo->Add(lte);
	pUserCombo->SetInternVisible(true);

	if (user_ids.empty())
		return;

	auto itstart = user_ids.begin();
	auto itend = user_ids.end();
	int nSelectIndex = 0;
	std::string loginUserId = CSdkManager::GetInstance().GetLoginUserId();
	fsp::String selectuserId = CSdkManager::GetInstance().GetCurSelectChatUser();

	for(int i=1;;i++)
	{
		if (itstart == itend)
			break;

		fsp::String userid = itstart->c_str();

		if (userid == CSdkManager::GetInstance().GetLoginUserId().c_str())
		{
			itstart++;
			i--;
			continue;//聊天选择列表，不添加自己
		}


		CListLabelElementUI* pCombItem = new CListLabelElementUI;
		pCombItem->SetTag(i);

		CDuiString wstrUsrId = demo::Utf82WStr(userid.c_str()).GetWStr();
		pCombItem->SetText(wstrUsrId);
		pUserCombo->Add(pCombItem);
		pUserCombo->SetInternVisible(true);

		if (userid == selectuserId)
		{
			nSelectIndex = i;
		}
		itstart++;
	}
	pUserCombo->SelectItem(nSelectIndex);
}

CDuiString CMeetingMainWnd::GetSkinFolder()
{
	return CDuiString(L"skin");
}

CDuiString CMeetingMainWnd::GetSkinFile()
{
	return CDuiString(L"meeting_main_wnd.xml");
}

LPCTSTR CMeetingMainWnd::GetWindowClassName(void) const
{
	return L"CDuiFrameWnd";
}

void CMeetingMainWnd::InitWindow()
{
	if (!m_bVideoWndInitFlag)
	{
		m_bVideoWndInitFlag = true;
	}

	m_PaintManager.SetTimer(m_PaintManager.GetRoot(), 
		UPDATE_VIDEO_INFO_TIMER_ID, UPDATE_VIDEO_INFO_INTERVAL);

	SetIcon(IDI_FSPCLIENT);

	m_wndShadow.Create(m_hWnd);
	RECT rcCorner = { 6,6,6,6 };
	RECT rcHoleOffset = { 7,7,7,7 };
	m_wndShadow.SetImage(_T("img\\wnd_bg.png"), rcCorner, rcHoleOffset);

	m_pScreenShareLayout = (CVideoPanelLayoutUI*)m_PaintManager.FindControl(L"main_screenshare_container");
	m_pVideoLayout = (CVideoPanelLayoutUI*)m_PaintManager.FindControl(L"main_video_container");	

	//临时切换到屏幕共享tab, 是为了初始化ui和窗口
	CTabLayoutUI* pTabMain = (CTabLayoutUI*)m_PaintManager.FindControl(L"main_tab");
	m_pScreenShareLayout->InitVideoPanles();
	m_pVideoLayout->InitVideoPanles();
}

LRESULT CMeetingMainWnd::OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	PostQuitMessage(0);
	return 0;
}

CControlUI* CMeetingMainWnd::CreateControl(LPCTSTR pstrClass)
{
	if (_tcsicmp(pstrClass, CVideoPanelUI::K_UI_INTERFACE_NAME) == 0)
	{
		return new CVideoPanelUI();
	}
	else if (_tcsicmp(pstrClass, CVideoPanelLayoutUI::K_UI_INTERFACE_NAME) == 0)
	{
		return new CVideoPanelLayoutUI();
	}
	else {
		return WindowImplBase::CreateControl(pstrClass);
	}
}

void CMeetingMainWnd::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == L"btn_mic")
	{
		//底部麦克风按钮， 音频广播，停止广播按钮
		DWORD dwMicIndex = CSdkManager::GetInstance().GetOpenMic();
		if (dwMicIndex == INVALID_MIC_INDEX)
		{
			m_bBroadcastMic = false;
			return;
		}
		fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();
		if (m_bBroadcastMic)
		{
			if (fsp::ErrCode::ERR_OK == pEngin->StopPublishAudio())
			{
				m_bBroadcastMic = false;
			}
		}
		else
		{
			if (fsp::ErrCode::ERR_OK == pEngin->StartPublishAudio())
			{
				m_bBroadcastMic = true;
			}
		}

		UpdateUserItemIcon(CSdkManager::GetInstance().GetLoginUserId(), USER_ITEM_ICON_AUDIO, m_bBroadcastMic);
		SetToolbarMicBtnStatus(m_bBroadcastMic);
	}
	else if (msg.pSender->GetName() == L"btn_cam")
	{
		//底部摄像头按钮
		OnClickCamBtn(msg);
	}
	else if (msg.pSender->GetName() == L"btn_screenshare") {
		//底部屏幕共享按钮
		if (m_isScreenSharing) {
			CSdkManager::GetInstance().GetFspEngin()->StopPublishScreenShare();
			m_isScreenSharing = false;
		}
		else {
			ScreenShareConfig config = CSdkManager::GetInstance().GetScreenShareConfig();
			CSdkManager::GetInstance().GetFspEngin()->StartPublishScreenShare(config.left, config.top,
				config.right, config.bottom, config.qualityBias);
			m_isScreenSharing = true;
		}

		SetToolbarScreenshareBtnStatus(m_isScreenSharing);
		UpdateUserItemIcon(CSdkManager::GetInstance().GetLoginUserId(), USER_ITEM_ICON_SCREENSHARE, m_isScreenSharing);
	}

	else if (msg.pSender->GetName() == L"btn_record")
	{
		CControlUI* pBtnRecord = m_PaintManager.FindControl(L"btn_record");

		//底部录制按钮
		if (m_isRecording)
		{
			CSdkManager::GetInstance().GetFspEngin()->StopRecord();
			m_isRecording = false;

			pBtnRecord->SetAttribute(L"normalimage", L"img\\main_toolbar_record_normal.png");
			pBtnRecord->SetAttribute(L"hotimage", L"img\\main_toolbar_record_hover.png");
			pBtnRecord->SetAttribute(L"pushedimage", L"img\\main_toolbar_record_selected.png");
		}
		else
		{
			m_isRecording = true;
			TCHAR szFilePath[MAX_PATH * 2];
			GetCurrentDirectory(MAX_PATH * 2, szFilePath);
			CHAR sfilePath[MAX_PATH * 2];
			demo::ConvertUnicodeToUtf8(szFilePath, sfilePath, MAX_PATH * 2);
			strcat(sfilePath, "\\Record.mp4");
			CSdkManager::GetInstance().GetFspEngin()->StartRecord(sfilePath, true);

			pBtnRecord->SetAttribute(L"normalimage", L"img\\main_toolbar_record_selected.png");
			pBtnRecord->SetAttribute(L"hotimage", L"img\\main_toolbar_record_hover.png");
			pBtnRecord->SetAttribute(L"pushedimage", L"img\\main_toolbar_record_normal.png");
		}
	}
	else if (msg.pSender->GetName() == L"btn_speaker")
	{
		//底部扬声器按钮， 弹出音量设置
		COptionUI* pSpeaker = (COptionUI*)msg.pSender;
		POINT point;
		point.x = msg.pSender->GetPos().left;
		point.y = msg.pSender->GetPos().top - 150;

		if (m_SpeakerSettingWnd.GetHWND() == NULL)
		{
			m_SpeakerSettingWnd.Create(this->GetHWND(), _T(""), UI_WNDSTYLE_DIALOG | WS_EX_LAYERED, 0, 0, 0, 0, 0);
		}

		ClientToScreen(m_hWnd, &point);
		::SetWindowPos(m_SpeakerSettingWnd.GetHWND(), HWND_TOP, point.x, point.y, WINDOW_WIDTH, WINDOW_HEIGTH, 0);
		m_SpeakerSettingWnd.ShowWindow(true);
	}
	else if (msg.pSender->GetName() == L"btn_setting")
	{
		//底部设置按钮
		m_pSettingWnd = new CSettingWnd();
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
	else if (msg.pSender->GetName() == L"btn_CallIn")
	{
		//呼叫页面
		CSdkManager::GetInstance().GetUserStateWnd()->ShowGroupJoined();
	}
	else if (msg.pSender->GetName() == L"main_msg_send")
	{
		//显示框添加文字
		CRichEditUI* pInput = (CRichEditUI*)m_PaintManager.FindControl(L"main_msg_input");
		CComboUI* pCombo = (CComboUI*)m_PaintManager.FindControl(L"main_msg_selectuser");
		if(!pInput || pInput->GetText() == L"" || !pCombo ||pCombo->GetCurSel()<0)
			return;

		CControlUI* pSelItem = NULL;
		pSelItem = pCombo->GetItemAt(pCombo->GetCurSel());

		DWORD nlen = pInput->GetText().GetLength();
		if (nlen <= 0)
			return;

		char *pchar = new char[nlen*4];
		ZeroMemory(pchar, nlen * 4 * sizeof(char));
		demo::ConvertUnicodeToUtf8(pInput->GetText().GetData(),pchar,nlen * 4);

		ErrCode errcode = ERR_FAIL;
		if (pSelItem->GetTag() == 0)//所有人
		{
			errcode = CSdkManager::GetInstance().GetFspEngin()->GetFspSignaling()->SendGroupMsg(pchar,nullptr);
		}
		else //发送给指定用户的消息
		{
			char szUserid[MAX_PATH];
			ZeroMemory(szUserid, MAX_PATH*sizeof(char));
			demo::ConvertUnicodeToUtf8(pSelItem->GetText(),szUserid, MAX_PATH);
			errcode = CSdkManager::GetInstance().GetFspEngin()->GetFspSignaling()->SendUserMsg(szUserid,pchar,nullptr);
		}

		if (errcode == ERR_OK)
		{
			AppendMsg(pInput->GetText(), false, L"我", pSelItem->GetText());

			pInput->SetText(L"");
			pInput->SetFocus();
		}

		delete[] pchar;
	}
	else if (msg.pSender->GetName() == L"closebtn") {
	    //退出组，回到在线界面。 清空ui上的状态
		m_pVideoLayout->StopAllVideo();
		m_pScreenShareLayout->StopAllVideo();

	    CSdkManager::GetInstance().LeaveGroup();
		
		m_vecRemoteAudioInfo.clear();
		m_vecRemoteVideoInfo.clear();

		CRichEditUI* pReMsgs = static_cast<CRichEditUI*>(m_PaintManager.FindControl(L"main_re_msgs"));
		pReMsgs->SetText(L"");

		CListUI *pListControl = static_cast<CListUI*>(m_PaintManager.FindControl(L"list_users"));
		pListControl->RemoveAll();

		CComboUI *pUserCombo = (CComboUI*)(m_PaintManager.FindControl(L"main_msg_selectuser"));
		pUserCombo->RemoveAll();

		SetToolbarCamBtnStatus(false);
		SetToolbarMicBtnStatus(false);
		SetToolbarScreenshareBtnStatus(false);
    }
	else
	{
		WindowImplBase::OnClick(msg);
	}
}

void CMeetingMainWnd::OnCamItemClicked(TNotifyUI& msg)
{
	//摄像头选择菜单操作
	DWORD dwCamIndex = _wtoi(msg.pSender->GetName().GetData() + 4);

	char szVideoId[32] = { 0 };
	sprintf_s(szVideoId, "LocalCam_%d", dwCamIndex);

	if (m_setCamOpenIndexes.count(dwCamIndex) > 0)
	{
		//已经打开，关闭
		m_setCamOpenIndexes.erase(dwCamIndex);
		m_pVideoLayout->StopLocalPreview(dwCamIndex);
		CSdkManager::GetInstance().StopPublishVideo(szVideoId);
		UpdateUserItemIcon(CSdkManager::GetInstance().GetLoginUserId(), USER_ITEM_ICON_VIDEO, false);	
	}
	else
	{
		//没有打开，打开摄像头并广播		
		if (m_pVideoLayout->PreviewLocalCamera(dwCamIndex)) {
			m_setCamOpenIndexes.insert(dwCamIndex);
			fsp::ErrCode error = CSdkManager::GetInstance().StartPublishVideo(szVideoId, dwCamIndex);
			if (fsp::ERR_OK != error)
			{
				AppendMsg(_T("广播摄像头失败 错误码%d"), error);
			}
			m_pVideoLayout->SetPublishVideoId(dwCamIndex, szVideoId);
			UpdateUserItemIcon(CSdkManager::GetInstance().GetLoginUserId(), USER_ITEM_ICON_VIDEO, true);
		}
	}

	SetToolbarCamBtnStatus(!m_setCamOpenIndexes.empty());
}

void CMeetingMainWnd::OnSelectChanged(TNotifyUI& msg)
{
	CTabLayoutUI* pTabLayout = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("main_tab")));
	if (msg.pSender->GetName() == L"tabopt_video") {
		pTabLayout->SelectItem(0);
	}
	else if (msg.pSender->GetName() == L"tabopt_screenshare") {
		pTabLayout->SelectItem(1);
	}
}

void CMeetingMainWnd::OnSetFocus(TNotifyUI &msg)
{
	if (msg.pSender->GetName() == L"main_msg_inputtip")
	{
		if (msg.pSender->IsVisible())
		{
			msg.pSender->SetVisible(false);
			CRichEditUI* pInput = (CRichEditUI*)m_PaintManager.FindControl(L"main_msg_input");
			if (pInput)
			{
				pInput->SetVisible(true);
				pInput->SetFocus();
			}

		}
	}
}

void CMeetingMainWnd::OnKillFocus(TNotifyUI &msg)
{
	if (msg.pSender->GetName() == L"main_msg_input")
	{
		CRichEditUI* pInput = (CRichEditUI*)msg.pSender;
		if (pInput && pInput->GetText()==L"")
		{
			CLabelUI* pLableTip = (CLabelUI*)m_PaintManager.FindControl(L"main_msg_inputtip");
			pLableTip->SetVisible(true);
			pInput->SetVisible(false);
		}
	}
}

void CMeetingMainWnd::OnItemSelectChange(TNotifyUI & msg)
{
	if (msg.pSender->GetName() == L"main_msg_selectuser")
	{
		CComboUI* pCombo = (CComboUI*)msg.pSender;
		CHAR string[256] = { 0 };
		demo::ConvertUnicodeToUtf8(pCombo->GetText().GetData(), string,256);
		CSdkManager::GetInstance().SetCurSelectChatUser(string);
	}
}

void CMeetingMainWnd::OnClickCamBtn(TNotifyUI& msg)
{
	//底部视频广播按钮，创建摄像头菜单并显示	
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

		bool isCameraOpened = m_setCamOpenIndexes.end() != m_setCamOpenIndexes.find(dwMenuItemIndex);

		pMenu->AddMenuItem(szMenuItemName,
			CToolbarMenu::MENU_ITEM_TYPE_CHECKBOX,
			szOptionName,
			szCamName,
			L"",
			isCameraOpened);

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

void CMeetingMainWnd::OnBroadcastCamChanged(WPARAM wParam, LPARAM lParam)
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

LRESULT CMeetingMainWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_bVideoWndInitFlag)
		return 0;

	switch (uMsg)
	{
	case WM_TIMER:
	{
		m_pVideoLayout->UpdateVideoInfo(); // 界面线程刷新音视频状态信息
		m_pScreenShareLayout->UpdateVideoInfo();
		break;
	}
	case WM_MOVE:
		m_pVideoLayout->LayoutVideoPanels();
		m_pScreenShareLayout->LayoutVideoPanels();
		break;
	case WM_GETMINMAXINFO:
	{
		RECT rt;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);
		((MINMAXINFO*)lParam)->ptMaxSize.y = (rt.bottom - rt.top);
		break;
	}
	case DUILIB_MSG_ADD_REMOTE_VIDEO:
	{
		//查看远端视频
		RemoteVideoInfo* pInfo = (RemoteVideoInfo*)wParam;

		if (pInfo->strVideoId == fsp::RESERVED_VIDEOID_SCREENSHARE) 
		{
			//屏幕共享，显示位置不一样
			fsp::ErrCode errCode = m_pScreenShareLayout->OpenRemoteVideo(pInfo->strUserId.c_str(), pInfo->strVideoId.c_str());
			if ( errCode != ERR_OK )
			{
				CDuiString strMsg;
				strMsg.Format(L"查看屏幕共享失败:(%s) Errcode:%d", demo::Utf82WStr(pInfo->strUserId.c_str()), errCode);
				AppendMsg(strMsg);
			}
			else
			{
				UpdateUserItemIcon(pInfo->strUserId.c_str(), USER_ITEM_ICON_SCREENSHARE, true);
			}
		}
		else 
		{
			//视频
			fsp::ErrCode errCode = m_pVideoLayout->OpenRemoteVideo(pInfo->strUserId.c_str(), pInfo->strVideoId.c_str());
			if (errCode!=ERR_OK)
			{
				CDuiString strMsg;
				strMsg.Format(L"查看远端视频失败:(%s, %s) Errcode:%d", demo::Utf82WStr(pInfo->strUserId.c_str()), demo::Utf82WStr(pInfo->strVideoId.c_str()), errCode);
				AppendMsg(strMsg);
			}
			else {
				UpdateUserItemIcon(pInfo->strUserId.c_str(), USER_ITEM_ICON_VIDEO, true);
			}
		}
		delete pInfo;
		break;
	}
	case DUILIB_MSG_DEL_REMOTE_VIDEO:
	{
		//关闭远端视频
		RemoteVideoInfo* pInfo = (RemoteVideoInfo*)wParam;

		if (pInfo->strVideoId == fsp::RESERVED_VIDEOID_SCREENSHARE) {
			m_pScreenShareLayout->CloseRemoteVideo(pInfo->strUserId.c_str(), pInfo->strVideoId.c_str());
			UpdateUserItemIcon(pInfo->strUserId.c_str(), USER_ITEM_ICON_SCREENSHARE, false);
		}
		else {
			m_pVideoLayout->CloseRemoteVideo(pInfo->strUserId.c_str(), pInfo->strVideoId.c_str());
			UpdateUserItemIcon(pInfo->strUserId.c_str(), USER_ITEM_ICON_VIDEO, false);
		}
		
		delete pInfo;
		break;
	}
	case DUILIB_MSG_ADD_REMOTE_AUDIO:
	{
		//远端视频打开了
		RemoteAudioInfo* pInfo = (RemoteAudioInfo*)wParam;
		UpdateUserItemIcon(pInfo->strUserId.c_str(), USER_ITEM_ICON_AUDIO, true);
		delete pInfo;
		break;
	}
	case DUILIB_MSG_DEL_REMOTE_AUDIO:
	{
		RemoteAudioInfo* pInfo = (RemoteAudioInfo*)wParam;
		UpdateUserItemIcon(pInfo->strUserId.c_str(), USER_ITEM_ICON_AUDIO, false);
		delete pInfo;
		break;
	}
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

	case DUILIB_MSG_REMOTECONTROL_EVENT: {
		RemoteControlInfo* pInfo = (RemoteControlInfo*)wParam;

		WCHAR wszRemoteUserId[128] = { 0 };
		demo::ConvertUtf8ToUnicode(pInfo->strUserId.c_str(),
			wszRemoteUserId, 128);

		if (pInfo->operationType == fsp::REMOTE_CONTROL_REQUEST) {
			CMessageBox* pMB = new CMessageBox;
			CDuiString strTip;
			strTip.Format(L"%s 请求远程控制", wszRemoteUserId);
			pMB->Create(GetHWND(), L"CDuiLoginWnd", UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
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

		delete pInfo;
	}break;
	case DUILIB_MSG_DEVICECHANGE:
	{
		if (m_pSettingWnd) {
			m_pSettingWnd->OnDeviceChanged();
		}
	}break;
	case DUILIB_MSG_RECIVE_CAHTMSG:
	{
		MsgReciveInfo* pInfo = (MsgReciveInfo*)wParam;
		int nlen = pInfo->srcuserid.length()+1;
		WCHAR* szid = new WCHAR[nlen*2];
		ZeroMemory(szid, nlen * 2 * sizeof(WCHAR));
		demo::ConvertUtf8ToUnicode(pInfo->srcuserid.data(), szid, nlen * 2);

		int nMsglen = pInfo->msg.length() + 1;
		WCHAR* szout = new WCHAR[nMsglen * 2];
		ZeroMemory(szout, nMsglen * 2 * sizeof(WCHAR));
		demo::ConvertUtf8ToUnicode(pInfo->msg.data(), szout, nMsglen * 2);

		AppendMsg(szout, false, szid, pInfo->bGroup ? L"所有人" : L"我");

		delete[] szid;
		delete[] szout;
	}break;
	case DUILIB_MSG_REMOTE_ADD_USER:
	{
		RemoteUserEventInfo* pInfo = (RemoteUserEventInfo*)wParam;
		AddMainUserItem(pInfo->remote_userid);
		AddChatUserItem(pInfo->remote_userid);
	}break;
	case DUILIB_MSG_REMOTE_DEL_USER:
	{
		RemoteUserEventInfo* pInfo = (RemoteUserEventInfo*)wParam;
		DelMainUserItem(pInfo->remote_userid);
		DelChatUserItem(pInfo->remote_userid);
	}break;
	default:
		break;
	}
	return 0;
}

void CMeetingMainWnd::SetToolbarCamBtnStatus(bool isOpen)
{
	CControlUI* pBtnCamera = m_PaintManager.FindControl(L"btn_cam");

	if (isOpen)
	{
		pBtnCamera->SetAttribute(L"normalimage", L"img\\main_toolbar_camera_selected.png");
		pBtnCamera->SetAttribute(L"hotimage", L"img\\main_toolbar_camera_hover.png");
		pBtnCamera->SetAttribute(L"pushedimage", L"img\\main_toolbar_camera_normal.png");
	}
	else
	{
		pBtnCamera->SetAttribute(L"normalimage", L"img\\main_toolbar_camera_normal.png");
		pBtnCamera->SetAttribute(L"hotimage", L"img\\main_toolbar_camera_hover.png");
		pBtnCamera->SetAttribute(L"pushedimage", L"img\\main_toolbar_camera_selected.png");
	}
}

void CMeetingMainWnd::SetToolbarMicBtnStatus(bool isOpen)
{
	CControlUI* pBtnMic = m_PaintManager.FindControl(L"btn_mic");
	if (isOpen)
	{
		pBtnMic->SetAttribute(L"normalimage", L"img\\main_toolbar_mic_selected.png");
		pBtnMic->SetAttribute(L"hotimage", L"img\\main_toolbar_mic_hover.png");
		pBtnMic->SetAttribute(L"pushedimage", L"img\\main_toolbar_mic_normal.png");
	}
	else
	{
		pBtnMic->SetAttribute(L"normalimage", L"img\\main_toolbar_mic_normal.png");
		pBtnMic->SetAttribute(L"hotimage", L"img\\main_toolbar_mic_hover.png");
		pBtnMic->SetAttribute(L"pushedimage", L"img\\main_toolbar_mic_selected.png");
	}
}

void CMeetingMainWnd::SetToolbarScreenshareBtnStatus(bool isOpen)
{
	CControlUI* pBtnScreenShare = m_PaintManager.FindControl(L"btn_screenshare");
	if (isOpen)
	{
		pBtnScreenShare->SetAttribute(L"normalimage", L"img\\main_toolbar_screenshare_selected.png");
		pBtnScreenShare->SetAttribute(L"hotimage", L"img\\main_toolbar_screenshare_hover.png");
		pBtnScreenShare->SetAttribute(L"pushedimage", L"img\\main_toolbar_screenshare_normal.png");
	}
	else
	{
		pBtnScreenShare->SetAttribute(L"normalimage", L"img\\main_toolbar_screenshare_normal.png");
		pBtnScreenShare->SetAttribute(L"hotimage", L"img\\main_toolbar_screenshare_hover.png");
		pBtnScreenShare->SetAttribute(L"pushedimage", L"img\\main_toolbar_screenshare_selected.png");
	}
}

void CMeetingMainWnd::AppendMsg(const CDuiString& strMsg, bool bSysMsg, const CDuiString& dstUser, const CDuiString& srcUser)
{
	CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(m_PaintManager.FindControl(L"main_re_msgs"));
	if (!pRichEdit)
		return;

	PARAFORMAT2 pf2;
	memset(&pf2, 0, sizeof(PARAFORMAT2));
	pf2.cbSize = sizeof(PARAFORMAT2);
	pf2.dwMask = PFM_LINESPACING;// | PFM_ALIGNMENT;
	pf2.bLineSpacingRule = 0;//bSysMsg? 2:1;//0是单倍行距，1是1.5倍行距，2是双倍行距
	pRichEdit->SetParaFormat(pf2);

	long lSelBegin = 0, lSelEnd = 0;

	std::stringstream strStream;
	auto t = chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	strStream<<std::put_time(std::localtime(&t), "%X");

	lSelBegin = pRichEdit->GetTextLength();
	pRichEdit->AppendText(demo::Utf82WStr(strStream.str().c_str()).GetWStr());
	pRichEdit->AppendText(L" ");

	lSelEnd = pRichEdit->GetTextLength();
	pRichEdit->SetSel(lSelBegin, lSelEnd);
	

	CHARFORMAT2 cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT2));
	cf.cbSize = sizeof(cf);
	cf.dwReserved = 0;
	cf.dwMask = CFM_COLOR | CFM_UNDERLINE | CFM_SIZE;
	cf.crTextColor = RGB(0x66, 0x66, 0x66);
	cf.yHeight = 14 * 14;// bSysMsg ? 13 * 13 : 14 * 14;

	pRichEdit->SetSelectionCharFormat(cf);

	if (bSysMsg)
	{
		SetMsgFormat(L" 系统消息：", cf, false);
		pRichEdit->AppendText(L"\n");

		lSelBegin = pRichEdit->GetTextLength();
		pRichEdit->AppendText(strMsg);
		lSelEnd = pRichEdit->GetTextLength();

		pRichEdit->AppendText(L"\n");

		cf.crTextColor = RGB(0xFF, 0x33, 0x66);
		pRichEdit->SetSel(lSelBegin, lSelEnd);
		pRichEdit->SetSelectionCharFormat(cf);
	}
	else
	{
		
		SetMsgFormat(dstUser, cf, true);
		SetMsgFormat(L" 对 ", cf, false);
		SetMsgFormat(srcUser, cf, true);
		SetMsgFormat(L" 说：", cf, false);
		pRichEdit->AppendText(L"\n");

		lSelBegin = pRichEdit->GetTextLength();
		pRichEdit->AppendText(strMsg);
		pRichEdit->AppendText(L"\n");
		lSelEnd = pRichEdit->GetTextLength();

		cf.crTextColor = RGB(0x6A, 0x7D, 0xFE);
		pRichEdit->SetSel(lSelBegin, lSelEnd);
		pRichEdit->SetSelectionCharFormat(cf);
	}
	pRichEdit->EndDown();
}

void CMeetingMainWnd::SetMsgFormat(const CDuiString & msg, CHARFORMAT2& cf, bool bUnderLine)
{
	CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(m_PaintManager.FindControl(L"main_re_msgs"));
	if (!pRichEdit)
		return;

	long begin = 0, end = 0;

	begin = pRichEdit->GetTextLength();
	pRichEdit->AppendText(msg);
	bUnderLine ? cf.dwEffects |= CFE_UNDERLINE: cf.dwEffects &= ~CFE_UNDERLINE;
	end = pRichEdit->GetTextLength();
	pRichEdit->SetSel(begin, end);
	pRichEdit->SetSelectionCharFormat(cf);
}

//更新用户列表中的图标
void CMeetingMainWnd::UpdateUserItemIcon(const std::string& strUserId, int iconType, bool isOpen)
{
	CListUI *pListControl = static_cast<CListUI*>(m_PaintManager.FindControl(L"list_users"));
	CDuiString wstrUserId = demo::Utf82WStr(strUserId.c_str()).GetWStr();

	for (int i = 0; i < pListControl->GetCount(); i++) {
		CContainerUI* pItem = (CContainerUI*)pListControl->GetItemAt(i);

		CLabelUI* pUserIdLabel = (CLabelUI*)pItem->FindSubControl(L"label_userid");
		CDuiString strItemUserId = pUserIdLabel->GetUserData();
		if (strItemUserId != wstrUserId) {
			continue;
		}

		//找到对应useritem
		switch (iconType)
		{
		case USER_ITEM_ICON_AUDIO:
		{
			CControlUI* pIconUiAudio = pItem->FindSubControl(L"icon_user_audio");
			pIconUiAudio->SetBkImage(isOpen ? L"img\\main_user_audio_open.png" : L"img\\main_user_audio_closed.png");
			break;
		}
		case USER_ITEM_ICON_VIDEO:
		{
			CControlUI* pIconUiVideo = pItem->FindSubControl(L"icon_user_video");
			pIconUiVideo->SetBkImage(isOpen ? L"img\\main_user_video_open.png" : L"img\\main_user_video_closed.png");
			break;
		}
		case USER_ITEM_ICON_SCREENSHARE:
		{
			CControlUI* pIconUiScreenshare = pItem->FindSubControl(L"icon_user_screenshare");
			pIconUiScreenshare->SetBkImage(isOpen ? L"img\\main_user_screenshare_open.png" : L"img\\main_user_screenshare_closed.png");
			break;
		}
		default:
			break;
		}

		// 更新了对应的userid 就退出循环
		break;
	}
}

void CMeetingMainWnd::AddMainUserItem(const std::string& strUserId)
{
	CListUI *pListControl = static_cast<CListUI*>(m_PaintManager.FindControl(L"list_users"));
	CDuiString wstrUserId = demo::Utf82WStr(strUserId.c_str()).GetWStr();
	
	for (int i = 0; i < pListControl->GetCount(); i++) {
		CContainerUI* pItem = (CContainerUI*)pListControl->GetItemAt(i);

		CLabelUI* pUserIdLabel = (CLabelUI*)pItem->FindSubControl(L"label_userid");
		CDuiString strItemUserId = pUserIdLabel->GetUserData();
		if (strItemUserId == wstrUserId) {
			//已经存在的不用重复插入
			return;
		}
	}

	CDialogBuilder builder;
	CListContainerElementUI* pUserUi = static_cast<CListContainerElementUI *>(builder.Create(L"meeting_main_useritem.xml",
		(UINT)0, this, &m_PaintManager));

	CLabelUI* pLabelUserId = (CLabelUI*)pUserUi->FindSubControl(L"label_userid");
	
	pLabelUserId->SetText(strUserId == CSdkManager::GetInstance().GetLoginUserId() ? wstrUserId + L"(我)" : wstrUserId);
	pLabelUserId->SetUserData(wstrUserId);

	pListControl->Add(pUserUi);
}

void CMeetingMainWnd::DelMainUserItem(const std::string & strUserId)
{
	CListUI *pListControl = static_cast<CListUI*>(m_PaintManager.FindControl(L"list_users"));
	CDuiString wstrUserId = demo::Utf82WStr(strUserId.c_str()).GetWStr();

	for (int i = 0; i < pListControl->GetCount(); i++)
	{
		CContainerUI* pItem = (CContainerUI*)pListControl->GetItemAt(i);

		CLabelUI* pUserIdLabel = (CLabelUI*)pItem->FindSubControl(L"label_userid");
		CDuiString strItemUserId = pUserIdLabel->GetUserData();
		if (strItemUserId == wstrUserId)
		{
			pListControl->Remove(pItem);
		}
	}
}

void CMeetingMainWnd::AddChatUserItem(const std::string & strUserId)
{
	CComboUI *pUserCombo = (CComboUI*)(m_PaintManager.FindControl(L"main_msg_selectuser"));
	if (!pUserCombo)
		return;

	CDuiString wstrUserId = demo::Utf82WStr(strUserId.c_str()).GetWStr();
	int nIndex = pUserCombo->GetCount();

	CListLabelElementUI* lte = new CListLabelElementUI();
	lte->SetText(wstrUserId);
	lte->SetTag(nIndex);
	pUserCombo->AddAt(lte, nIndex-1);
}

void CMeetingMainWnd::DelChatUserItem(const std::string & strUserId)
{
	CComboUI *pUserCombo = (CComboUI*)(m_PaintManager.FindControl(L"main_msg_selectuser"));
	if (!pUserCombo)
		return;

	CDuiString wstrUserId = demo::Utf82WStr(strUserId.c_str()).GetWStr();

	for (int i = 0; i < pUserCombo->GetCount(); i++)
	{
		CControlUI* pControl = pUserCombo->GetItemAt(i);
		CDuiString userid = pControl->GetText();
		if (wstrUserId == userid)
		{
			pUserCombo->Remove(pControl);
		}
	}
}

void CMeetingMainWnd::OnAppendMsg(const char * szSenderUserId, const char* szMsg, bool bGroup)
{
	MsgReciveInfo *pMsgInfo = new MsgReciveInfo;
	pMsgInfo->srcuserid = szSenderUserId;
	pMsgInfo->msg = szMsg;
	pMsgInfo->bGroup = bGroup;
	::PostMessage(m_hWnd,DUILIB_MSG_RECIVE_CAHTMSG, (WPARAM)pMsgInfo,0);
}


