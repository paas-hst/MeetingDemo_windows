
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
#include "UIBoardPanel.h"

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
	m_isBoarding = false;

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
	if (_tcsicmp(pstrClass, CBoardPanelUI::K_UI_INTERFACE_NAME) == 0)
	{
		return new CBoardPanelUI();
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
			if (fsp::ErrCode::ERR_OK == pEngin->StopPublishAudio(fsp::RESERVED_AUDIOID_MICROPHONE))
			{
				m_bBroadcastMic = false;
			}
		}
		else
		{
			if (fsp::ErrCode::ERR_OK == pEngin->StartPublishAudio(fsp::RESERVED_AUDIOID_MICROPHONE))
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
			CSdkManager::GetInstance().GetFspEngin()->StartRecord(sfilePath, 0, 0, 0, 0, true);

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
	else if (msg.pSender->GetName() == L"btn_board")
	{
	    POINT point;
	    point.x = msg.pSender->GetPos().left - 20;
	    point.y = msg.pSender->GetPos().top - 65;
	    if (m_BoardCreateWnd.GetHWND() == NULL)
	    {
		    m_BoardCreateWnd.Create(this->GetHWND(), _T(""), UI_WNDSTYLE_DIALOG | WS_EX_LAYERED, 0, 0, 0, 0, 0);
	    }
	    ClientToScreen(m_hWnd, &point);
	    ::SetWindowPos(m_BoardCreateWnd.GetHWND(), HWND_TOP, point.x, point.y, 120, 60, 0);
	    m_BoardCreateWnd.ShowWindow(true);
		SetToolbarBoardBtnStatus(m_isBoarding);
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

		ClearAllBoardLayout();

		SetToolbarCamBtnStatus(false);
		SetToolbarMicBtnStatus(false);
		SetToolbarScreenshareBtnStatus(false);
		SetToolbarBoardBtnStatus(false);
    }
	else if (msg.pSender->GetName() == L"btn_board_edit")
	{
		fsp::BoardOperateType  Operatemode = CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->GetCurOperateType(m_strCurBoardId.c_str());
		if (Operatemode == fsp::BOARD_OPERATE_UNKOWN) {
			CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->SetCurOperateType(m_strCurBoardId.c_str(), fsp::BOARD_OPERATE_PRODUCE);
			CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->SetCurProduceGraphType(m_strCurBoardId.c_str(), fsp::GRAPH_TYPE_PATH);
			SetBoardEditStatus(msg.pSender->GetParent(), true);
			msg.pSender->SetBkColor(0xFFFF0000);
			msg.pSender->SetText(L"退出标注");
		}
		else {
			CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->SetCurOperateType(m_strCurBoardId.c_str(), fsp::BOARD_OPERATE_UNKOWN);
			SetBoardEditStatus(msg.pSender->GetParent(), false);
			msg.pSender->SetBkColor(0xFF00FF52);
			msg.pSender->SetText(L"标注");
		}
	}
	else if (msg.pSender->GetName() == L"btn_board_line")
	{
	    CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->SetCurOperateType(m_strCurBoardId.c_str(), fsp::BOARD_OPERATE_PRODUCE);
	    CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->SetCurProduceGraphType(m_strCurBoardId.c_str(), fsp::GRAPH_TYPE_LINE);
	}
	else if (msg.pSender->GetName() == L"btn_board_path") {
	    CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->SetCurOperateType(m_strCurBoardId.c_str(), fsp::BOARD_OPERATE_PRODUCE);
	    CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->SetCurProduceGraphType(m_strCurBoardId.c_str(), fsp::GRAPH_TYPE_PATH);
	}
	else if (msg.pSender->GetName() == L"btn_board_text") {
	    CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->SetCurOperateType(m_strCurBoardId.c_str(), fsp::BOARD_OPERATE_PRODUCE);
	    CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->SetCurProduceGraphType(m_strCurBoardId.c_str(), fsp::GRAPH_TYPE_TEXT);
	}
	else if (msg.pSender->GetName() == L"btn_board_clear") {
	    CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->ClearCurrentPage(m_strCurBoardId.c_str());
	}
	else if (msg.pSender->GetName() == L"btn_board_select") {
	    CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->SetCurOperateType(m_strCurBoardId.c_str(), fsp::BOARD_OPERATE_SELECT);
	}
	else if (msg.pSender->GetName() == L"btn_board_delete") {
	    CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->SetCurOperateType(m_strCurBoardId.c_str(), fsp::BOARD_OPERATE_DELETE);
	}
	else if (msg.pSender->GetName() == L"btn_board_undo") {
	    CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->Undo(m_strCurBoardId.c_str());
	}
	else if (msg.pSender->GetName() == L"btn_board_redo") {
	    CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->Redo(m_strCurBoardId.c_str());
	}
	else if (msg.pSender->GetName() == L"btn_board_thin") {
	    CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->SetLineWidth(m_strCurBoardId.c_str(), 4);
	}
	else if (msg.pSender->GetName() == L"btn_board_mid") {
	    CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->SetLineWidth(m_strCurBoardId.c_str(), 8);
	}
	else if (msg.pSender->GetName() == L"btn_board_thick") {
	    CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->SetLineWidth(m_strCurBoardId.c_str(), 12);
	}
	else if (msg.pSender->GetName() == L"btn_board_red") {
	    CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->SetLineColor(m_strCurBoardId.c_str(), 0xffff0000);
	}
	else if (msg.pSender->GetName() == L"btn_board_blue") {
	    CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->SetLineColor(m_strCurBoardId.c_str(), 0xff0098FF);
	}
	else if (msg.pSender->GetName() == L"btn_board_green") {
	    CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->SetLineColor(m_strCurBoardId.c_str(), 0xff00FF52);
	}
	else if (msg.pSender->GetName() == L"btn_board_prePage") {
		int nCurrentPageID = CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->GetCurrentPageId(m_strCurBoardId.c_str());
		int nPageConut = CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->GetPageCount(m_strCurBoardId.c_str());
		if (nCurrentPageID > 0) {
			nCurrentPageID--;
			CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->ChangeCurrentPage(m_strCurBoardId.c_str(), nCurrentPageID);
		}
		SetBoardPageBtnStatus(msg.pSender->GetParent(), (nCurrentPageID == 0), ((nCurrentPageID + 1) >= nPageConut));

		CControlUI* pBtnBoardPageId = m_PaintManager.FindSubControlByName(msg.pSender->GetParent(), L"btn_board_curPage");
		TCHAR swzConfigFile[50];
		_swprintf(swzConfigFile, L"%d", nCurrentPageID + 1);
		pBtnBoardPageId->SetAttribute(L"text", swzConfigFile);
	}
	else if (msg.pSender->GetName() == L"btn_board_nexPage") {
		int nCurrentPageID = CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->GetCurrentPageId(m_strCurBoardId.c_str());
		int nPageConut = CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->GetPageCount(m_strCurBoardId.c_str());
		if (nCurrentPageID < (nPageConut - 1)) {
			nCurrentPageID++;
			CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->ChangeCurrentPage(m_strCurBoardId.c_str(), nCurrentPageID);
		}
		SetBoardPageBtnStatus(msg.pSender->GetParent(), (nCurrentPageID == 0), (nCurrentPageID + 1) >= nPageConut);

		CControlUI* pBtnBoardPageId = m_PaintManager.FindSubControlByName(msg.pSender->GetParent(), L"btn_board_curPage");
		TCHAR swzConfigFile[20];
		_swprintf(swzConfigFile, L"%d", nCurrentPageID + 1);
		pBtnBoardPageId->SetAttribute(L"text", swzConfigFile);
	}
	else if (msg.pSender->GetName() == L"btn_board_syn") {
		bool synmode = CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->GetPageChangeSynMode(m_strCurBoardId.c_str());
		CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->SetPageChangeSynMode(m_strCurBoardId.c_str(), !synmode);
		msg.pSender->SetText((!synmode) ? L"同步" : L"自由");
		if (!synmode) {
			int nPageConut = CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->GetPageCount(m_strCurBoardId.c_str());
			int nCurrentPageID = CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->GetCurrentPageId(m_strCurBoardId.c_str());
			CControlUI* pBtnBoardPageId = m_PaintManager.FindSubControlByName(msg.pSender->GetParent(), L"btn_board_curPage");
			SetBoardPageBtnStatus(msg.pSender->GetParent(), (nCurrentPageID == 0), (nCurrentPageID + 1) >= nPageConut);
			pBtnBoardPageId->SetAttribute(L"text", std::to_wstring(nCurrentPageID + 1).c_str());
		}
	}
	else if (msg.pSender->GetName() == L"btn_board_close") {
		//销毁白板
		ErrCode hr = CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->Destory(m_strCurBoardId.c_str());
		if (hr == ERR_OK) {
			CDuiString strMsg;
			strMsg.Format(L"关闭白板:(ID: %s)", demo::Utf82WStr(m_strCurBoardId.c_str()));
			AppendMsg(strMsg);
			if (m_isBoarding) {
				m_isBoarding = false;
			}
		}
		//本地销毁界面
		ReleaseBoardLayout(m_strCurBoardId);
		CTabLayoutUI* pTabLayout = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("main_tab")));
		pTabLayout->SelectItem(0);
		auto opts = dynamic_cast<CControlUI*>(m_PaintManager.FindControl(_T("tabopt_video")));
		opts->SetAttribute(_T("selected"), _T("true"));
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
	else if (msg.pSender->GetUserData().IsEmpty() == false) {
		std::string strBoardId = demo::WStr2Utf8(msg.pSender->GetUserData().GetData()).GetUtf8Str();
		CControlUI* nBoardLayout = FindBoardLayout(strBoardId);
		if (nBoardLayout) {
			pTabLayout->SelectItem(nBoardLayout);
			m_strCurBoardId = strBoardId;
		}
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
	case DUILIB_MSG_CMMONINFO:
	{
		CDuiString* pStrInfo = (CDuiString*)wParam;
		AppendMsg(*pStrInfo);
		delete pStrInfo;
	}
	break;
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
					pInfo->strUserId.c_str(), fsp::REMOTE_CONTROL_ACCEPT);
			}
			else {
				CSdkManager::GetInstance().GetFspEngin()->RemoteControlOperation(
					pInfo->strUserId.c_str(), fsp::REMOTE_CONTROL_REJECT);
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
	case DUILIB_MSG_WHITEBOARD_PUBLISH:
	{
		WhiteBoardPulishInfo* pInfo = (WhiteBoardPulishInfo*)wParam;
		if (pInfo->isPublish) {
			EnsureBoardLayout(pInfo->strWhiteBoardId, pInfo->strWhiteBoardName);
			CDuiString strMsg;
			strMsg.Format(L"收到发布白板:(%s,%s)", demo::Utf82WStr(pInfo->strWhiteBoardName.c_str()),
				demo::Utf82WStr(pInfo->strWhiteBoardId.c_str()));
			AppendMsg(strMsg);
		}
		else {
			CDuiString strMsg;
			strMsg.Format(L"停止发布白板:(%s,%s)", demo::Utf82WStr(pInfo->strWhiteBoardName.c_str()),
				demo::Utf82WStr(pInfo->strWhiteBoardId.c_str()));
			AppendMsg(strMsg);
			ReleaseBoardLayout(pInfo->strWhiteBoardId);
			m_isBoarding = false;
			SetToolbarBoardBtnStatus(m_isBoarding);
		}
		delete pInfo;
	}
	break;
	case DUILIB_MSG_WHITEBOARD_SYN_DATA:
	{
		RemoteWhiteboardInfo* pInfo = (RemoteWhiteboardInfo*)wParam;

		int nCurrentPageID = pInfo->nPageId;
		int nPageConut = CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->GetPageCount(pInfo->strMediaId.c_str());
		CControlUI* nBoardLayout = FindBoardLayout(pInfo->strMediaId.c_str());
		SetBoardPageBtnStatus(nBoardLayout, (nCurrentPageID == 0), (nCurrentPageID + 1) >= nPageConut);

		CControlUI* pBtnBoardPageId = m_PaintManager.FindSubControlByName(nBoardLayout, L"btn_board_curPage");
		TCHAR swzText[80];
		_swprintf(swzText, L"%d", nCurrentPageID + 1);
		pBtnBoardPageId->SetAttribute(L"text", swzText);

		//CControlUI* pBtnBoardtab = m_PaintManager.FindControl(L"tabopt_board1");
		//_swprintf(swzText, L"%s", demo::Utf82WStr(pInfo->whiteboard_profile.boardname.c_str()));
		//pBtnBoardtab->SetAttribute(L"text", swzText);

		CDuiString strMsg;
		strMsg.Format(L"白板同步:(name:%s)(%s),%dx%d,[%d-%d]", demo::Utf82WStr(pInfo->whiteboard_profile.boardname.c_str()),
			demo::Utf82WStr(pInfo->strMediaId.c_str()), pInfo->whiteboard_profile.width, pInfo->whiteboard_profile.height,
			pInfo->whiteboard_profile.page, nCurrentPageID + 1);
		AppendMsg(strMsg);

		delete pInfo;
	}
		break;
	case DUILIB_MSG_WHITEBOARD_PAGE_CHANGE:
	{
		RemoteWhiteboardInfo* pInfo = (RemoteWhiteboardInfo*)wParam;
		int nCurrentPageID = pInfo->nPageId;
		int nPageConut = CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->GetPageCount(pInfo->strMediaId.c_str());
		CControlUI* nBoardLayout = FindBoardLayout(pInfo->strMediaId.c_str());
		SetBoardPageBtnStatus(nBoardLayout, (nCurrentPageID == 0), (nCurrentPageID + 1) >= nPageConut);

		CControlUI* pBtnBoardPageId = m_PaintManager.FindSubControlByName(nBoardLayout, L"btn_board_curPage");
		TCHAR swzText[20];
		_swprintf(swzText, L"%d", nCurrentPageID + 1);
		pBtnBoardPageId->SetAttribute(L"text", swzText);

		//CDuiString strMsg;
		//strMsg.Format(L"切换白板页:(%s:%d)", demo::Utf82WStr(pInfo->strMediaId.c_str()), nCurrentPageID+1);
		//AppendMsg(strMsg);

		delete pInfo;
	}
		break;
	case DUILIB_MSG_DOCUMENT_EVENT:
	{
		WhiteboardDocumentInfo* pInfo = (WhiteboardDocumentInfo*)wParam;

		OnDocumentEvent(pInfo->doc_status_type, pInfo->err_code);

		delete pInfo;
		break;
	}
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

void CMeetingMainWnd::SetToolbarBoardBtnStatus(bool isOpen)
{
	CControlUI* pBtnBoard = m_PaintManager.FindControl(L"btn_board");
	if (isOpen)
	{
		pBtnBoard->SetAttribute(L"normalimage", L"img\\main_toolbar_board_selected.png");
		pBtnBoard->SetAttribute(L"hotimage", L"img\\main_toolbar_board_hover.png");
		pBtnBoard->SetAttribute(L"pushedimage", L"img\\main_toolbar_board_normal.png");
	}
	else
	{
		pBtnBoard->SetAttribute(L"normalimage", L"img\\main_toolbar_board_normal.png");
		pBtnBoard->SetAttribute(L"hotimage", L"img\\main_toolbar_board_hover.png");
		pBtnBoard->SetAttribute(L"pushedimage", L"img\\main_toolbar_board_selected.png");
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
	
	CDuiString wstrUserName = demo::Utf82WStr(CSdkManager::GetInstance().GetCustomName(strUserId.c_str())).GetWStr();

	pLabelUserId->SetText(strUserId == CSdkManager::GetInstance().GetLoginUserId() ? wstrUserName + L"(我)" : wstrUserName);
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

	int nIndex = pUserCombo->GetCount();
	CDuiString wstrUserName = demo::Utf82WStr(CSdkManager::GetInstance().GetCustomName(strUserId.c_str())).GetWStr();

	CListLabelElementUI* lte = new CListLabelElementUI();	

	lte->SetText(wstrUserName);
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

void CMeetingMainWnd::AppendMsgMainThread(const char * szSenderUserId, const char* szMsg, bool bGroup)
{
	MsgReciveInfo *pMsgInfo = new MsgReciveInfo;
	pMsgInfo->srcuserid = szSenderUserId;
	pMsgInfo->msg = szMsg;
	pMsgInfo->bGroup = bGroup;
	::PostMessage(m_hWnd,DUILIB_MSG_RECIVE_CAHTMSG, (WPARAM)pMsgInfo,0);
}

void CMeetingMainWnd::AppendCommonInfoMainThread(const CDuiString& strMsg)
{
	CDuiString* pMsgStr = new CDuiString(strMsg);
	::PostMessage(m_hWnd, DUILIB_MSG_CMMONINFO, (WPARAM)pMsgStr, 0);
}

CControlUI* CMeetingMainWnd::EnsureBoardLayout(const std::string& strboardId, const std::string& strboardname)
{
	CControlUI* ret_val = NULL;
	for (std::vector<WhiteboardLayoutInfo>::size_type i = 0; i < m_vecBoards.size(); i++)
	{
		if (m_vecBoards[i].strBoardId.compare(strboardId) == 0) {
			ret_val = m_vecBoards[i].pLayout;
			break;
		}
	}

	if (NULL == ret_val) {
		CTabLayoutUI* pTabLayout = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("main_tab")));
		CDialogBuilder builder;
		auto ctrl = builder.Create(_T("board_tab.xml"), nullptr, this, &m_PaintManager);
		pTabLayout->Add(ctrl);
		ctrl->Activate();

		auto opt = new DuiLib::COptionUI();
		opt->SetGroup(_T("main_tab"));
		opt->SetText(demo::Utf82WStr(strboardname.c_str()).GetWStr());
		opt->SetToolTip(demo::Utf82WStr(strboardname.c_str()).GetWStr());
		opt->SetFixedWidth(90);
		opt->SetUserData(demo::Utf82WStr(strboardId.c_str()).GetWStr());
		opt->SetAttribute(_T("name"), _T("tabopt_board1"));
		opt->SetAttribute(_T("selectedimage"), _T("img\\main_tonal_bg.png"));
		opt->SetHotBkColor(0xFFD8D8D8);
		opt->SetTextColor(0xff454545);
		opt->SetHotTextColor(0xff6a7dfe);
		opt->SetSelectedTextColor(0xffffffff);

		auto opts = dynamic_cast<CContainerUI*>(m_PaintManager.FindControl(_T("main_opt")));
		opts->AddAt(opt, opts->GetCount() - 2);

		//打开白板
		CBoardPanelUI* pBoardPanelWnd = static_cast<CBoardPanelUI*>(m_PaintManager.FindSubControlByName(ctrl, _T("main_board_panel1")));
		pBoardPanelWnd->BoardOpen(strboardname.c_str(), strboardId.c_str());

		WhiteboardLayoutInfo tmp;
		tmp.strBoardId = strboardId;
		tmp.pLayout = ctrl;
		tmp.pOption = opt;
		m_vecBoards.push_back(tmp);
		ret_val = ctrl;
	}
	return ret_val;
}

CControlUI* CMeetingMainWnd::FindBoardLayout(const std::string& strboardId)
{
	CControlUI* ret_val = NULL;

	for (std::vector<WhiteboardLayoutInfo>::size_type i = 0; i < m_vecBoards.size(); i++)
	{
		if (m_vecBoards[i].strBoardId.compare(strboardId) == 0) {
			ret_val = m_vecBoards[i].pLayout;
			break;
		}
	}
	return ret_val;
}

void CMeetingMainWnd::ReleaseBoardLayout(const std::string&  strboardId)
{
	for (std::vector<WhiteboardLayoutInfo>::iterator iter = m_vecBoards.begin();
		iter != m_vecBoards.end(); iter++)
	{
		if ((*iter).strBoardId.compare(strboardId) == 0) {
			auto opts = dynamic_cast<CContainerUI*>(m_PaintManager.FindControl(_T("main_opt")));
			opts->Remove((*iter).pOption);
			(*iter).pOption->SetVisible(false);

			CTabLayoutUI* pTabLayout = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("main_tab")));
			pTabLayout->Remove((*iter).pLayout);
			CBoardPanelUI* pBoardPanelWnd = (CBoardPanelUI*)m_PaintManager.FindSubControlByName((*iter).pLayout, L"main_board_panel1");
			(*iter).pLayout->SetVisible(false);
			pBoardPanelWnd->BoardClose(strboardId.c_str());

			m_vecBoards.erase(iter);
			break;
		}
	}
}

void CMeetingMainWnd::ClearAllBoardLayout()
{
	for (auto iter = m_vecBoards.begin(); iter != m_vecBoards.end();)
	{
		auto opts = dynamic_cast<CContainerUI*>(m_PaintManager.FindControl(_T("main_opt")));
		opts->Remove(iter->pOption);
		iter->pOption->SetVisible(false);

		CTabLayoutUI* pTabLayout = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("main_tab")));
		pTabLayout->Remove(iter->pLayout);
		CBoardPanelUI* pBoardPanelWnd = (CBoardPanelUI*)m_PaintManager.FindSubControlByName(iter->pLayout, L"main_board_panel1");
		iter->pLayout->SetVisible(false);
		pBoardPanelWnd->BoardClose(iter->strBoardId.c_str());

		iter = m_vecBoards.erase(iter);
	}
}

void CMeetingMainWnd::SetBoardEditStatus(CControlUI* pPrient, bool enable)
{
	CControlUI* pBtnSelect = m_PaintManager.FindSubControlByName(pPrient, L"btn_board_select");
	CControlUI* pBtnDelete = m_PaintManager.FindSubControlByName(pPrient, L"btn_board_delete");
	CControlUI* pBtnClear = m_PaintManager.FindSubControlByName(pPrient, L"btn_board_clear");
	CControlUI* pBtnLine = m_PaintManager.FindSubControlByName(pPrient, L"btn_board_line");
	CControlUI* pBtnPath = m_PaintManager.FindSubControlByName(pPrient, L"btn_board_path");
	CControlUI* pBtnText = m_PaintManager.FindSubControlByName(pPrient, L"btn_board_text");
	CControlUI* pBtnUndo = m_PaintManager.FindSubControlByName(pPrient, L"btn_board_undo");
	CControlUI* pBtnRedo = m_PaintManager.FindSubControlByName(pPrient, L"btn_board_redo");
	CControlUI* pBtnThin = m_PaintManager.FindSubControlByName(pPrient, L"btn_board_thin");
	CControlUI* pBtnMid = m_PaintManager.FindSubControlByName(pPrient, L"btn_board_mid");
	CControlUI* pBtnThick = m_PaintManager.FindSubControlByName(pPrient, L"btn_board_thick");
	CControlUI* pBtnRed = m_PaintManager.FindSubControlByName(pPrient, L"btn_board_red");
	CControlUI* pBtnBlue = m_PaintManager.FindSubControlByName(pPrient, L"btn_board_blue");
	CControlUI* pBtnGreen = m_PaintManager.FindSubControlByName(pPrient, L"btn_board_green");

	if (!pBtnSelect || !pBtnDelete || !pBtnClear || !pBtnLine || !pBtnPath || !pBtnText || !pBtnUndo ||
		!pBtnRedo || !pBtnThin || !pBtnMid || !pBtnThick || !pBtnRed || !pBtnBlue || !pBtnGreen) {
		return;
	}
	
	pBtnSelect->SetVisible(enable);
	pBtnDelete->SetVisible(enable);
	pBtnClear->SetVisible(enable);
	pBtnLine->SetVisible(enable);
	pBtnPath->SetVisible(enable);
	pBtnText->SetVisible(enable);
	pBtnUndo->SetVisible(enable);
	pBtnRedo->SetVisible(enable);
	pBtnThin->SetVisible(enable);
	pBtnMid->SetVisible(enable);
	pBtnThick->SetVisible(enable);
	pBtnRed->SetVisible(enable);
	pBtnBlue->SetVisible(enable);
	pBtnGreen->SetVisible(enable);
	
}

void CMeetingMainWnd::OnDocumentEvent(fsp_wb::DocStatusType doc_event_type, fsp::ErrCode err_code)
{
	switch (doc_event_type)
	{
	case fsp_wb::DOC_STATUS_UPLOAD_RESULT:
	{
		CDuiString strMsg;
		strMsg.Format(L"文件上传:(%s)", (err_code == ERR_OK) ? L"成功" : L"失败");
		AppendMsg(strMsg);
	}break;
	case fsp_wb::DOC_STATUS_TRANSCODEING:
	{
		CDuiString strMsg;
		strMsg.Format(L"文件转码中,请耐心等待...");
		AppendMsg(strMsg);
	}break;
	case fsp_wb::DOC_STATUS_TRANSCODE_RESULT:
	{
		CDuiString strMsg;
		strMsg.Format(L"文件转码:(%s)", (err_code == ERR_OK) ? L"成功" : L"失败");
		AppendMsg(strMsg);
	}break;
	case fsp_wb::DOC_STATUS_TRANSCODE_CANCLE_RESULT:
	{
		CDuiString strMsg;
		strMsg.Format(L"文件转码取消结果:(%s)", (err_code == ERR_OK) ? L"成功" : L"失败");
		AppendMsg(strMsg);
	}break;
	case fsp_wb::DOC_STATUS_GET_DOWNLOAD_PATH_RESULT:
	{
		CDuiString strMsg;
		strMsg.Format(L"获取下载路径结果:(%s)", (err_code == ERR_OK) ? L"成功" : L"失败");
		AppendMsg(strMsg);
	}break;
	case fsp_wb::DOC_STATUS_DOWNLOAD_RESULT:
	{
		CDuiString strMsg;
		strMsg.Format(L"图片下载结果:(%s)", (err_code == ERR_OK) ? L"成功" : L"失败");
		AppendMsg(strMsg);
	}break;
	default:
		break;
	}
}

void CMeetingMainWnd::SetBoardPageBtnStatus(CControlUI* pPrient, bool isFrist, bool isEnd)
{
	CControlUI* pBtnBoardPer = m_PaintManager.FindSubControlByName(pPrient, L"btn_board_prePage");
	CControlUI* pBtnBoardNex = m_PaintManager.FindSubControlByName(pPrient, L"btn_board_nexPage");
	if (!pBtnBoardPer || !pBtnBoardNex) {
		return;
	}
	if (isFrist) {
		pBtnBoardPer->SetAttribute(L"normalimage", L"img\\board_perPage_disable.png");
		pBtnBoardPer->SetAttribute(L"pushedimage", L"img\\board_perPage_disable.png");
	}
	else {
		pBtnBoardPer->SetAttribute(L"normalimage", L"img\\board_perPage_normal.png");
		pBtnBoardPer->SetAttribute(L"pushedimage", L"img\\board_perPage_selected.png");
	}
	if (isEnd) {
		pBtnBoardNex->SetAttribute(L"normalimage", L"img\\board_nexPage_disable.png");
		pBtnBoardNex->SetAttribute(L"pushedimage", L"img\\board_nexPage_disable.png");
	}
	else {
		pBtnBoardNex->SetAttribute(L"normalimage", L"img\\board_nexPage_normal.png");
		pBtnBoardNex->SetAttribute(L"pushedimage", L"img\\board_nexPage_selected.png");
	}
}