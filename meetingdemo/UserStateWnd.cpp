#include "stdafx.h"

#include <algorithm>

#include "UserStateWnd.h"

#include "core/UIDefine.h"
#include "UIMenu.h"
#include "ToolbarMenu.h"
#include "fsp_engine.h"
#include "SdkManager.h"
#include "util.h"
#include "define.h"

static CRITICAL_SECTION g_CriticalSection;


DUI_BEGIN_MESSAGE_MAP(CUserStateWnd, WindowImplBase)
    DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

CUserStateWnd::CUserStateWnd()
{
	InitializeCriticalSection(&g_CriticalSection);
}

CUserStateWnd::~CUserStateWnd()
{
	DeleteCriticalSection(&g_CriticalSection);
}

void CUserStateWnd::ShowGroupJoined()
{
	CMeetingMainWnd* pMainWnd = CSdkManager::GetInstance().GetMeetingMainWnd();
	
	this->CenterWindow();

	CVerticalLayoutUI* pJoinGroup = (CVerticalLayoutUI*)m_PaintManager.FindControl(L"User_stat_jornGroup");
	pJoinGroup->SetVisible(false);

	CVerticalLayoutUI* pCallInLayout = (CVerticalLayoutUI*)m_PaintManager.FindControl(L"user_stat_CallIn");
	pCallInLayout->SetVisible(true);

	this->ShowWindow();
}

void CUserStateWnd::ShowGroupLeaved()
{
	UpdateJoinGroupBtn();
	ShowWindow();
}

void CUserStateWnd::ShowJoinGroupError(const CDuiString& strErrorInfo)
{
	UpdateJoinGroupBtn();
	demo::ShowMessageBox(m_hWnd, CDuiString(L"加入组失败：") + strErrorInfo);
}

void CUserStateWnd::UpdateUserList()
{
	EnterCriticalSection(&g_CriticalSection);

	CListUI *pListControl = static_cast<CListUI*>(m_PaintManager.FindControl(_T("list_users")));
	fsp::Vector<fsp::UserInfo> vecUsers = CSdkManager::GetInstance().GetUserList();

	CEditUI* pFindUser = (CEditUI*)m_PaintManager.FindControl(_T("edit_finduser"));
	if (!pListControl || !pFindUser)
	{
		LeaveCriticalSection(&g_CriticalSection);
		return;
	}

	//自己排在第一个行
	std::sort(vecUsers.begin(), vecUsers.end(), [](const fsp::UserInfo& l, const fsp::UserInfo& r)
	{
		if (l.user_id == CSdkManager::GetInstance().GetLoginUserId().c_str())
		{
			return true;
		}
		if (r.user_id == CSdkManager::GetInstance().GetLoginUserId().c_str())
		{
			return false;
		}
		return std::string(l.user_id.c_str()) < std::string(r.user_id.c_str());		
	});

	fsp::Vector<fsp::String> selectedUserIds = GetSelectedUserIds();

	pListControl->RemoveAll();

	for (fsp::UserInfo user : vecUsers) {
		CDialogBuilder builder;
		CListContainerElementUI* pUserUi = static_cast<CListContainerElementUI *>(builder.Create(_T("user_state_listitem.xml"),
			(UINT)0, this, &m_PaintManager));

		CLabelUI* pLabelUserId = (CLabelUI*)pUserUi->FindSubControl(L"label_userid");
		CCheckBoxUI* pSelectCb = (CCheckBoxUI*)(pUserUi->FindSubControl(L"check_user_select"));

		CDuiString wstrUsrId = demo::Utf82WStr(user.user_id.c_str()).GetWStr();
		pLabelUserId->SetText(CSdkManager::GetInstance().GetLoginUserId() == user.user_id.c_str() ? wstrUsrId+_T("(我)") : wstrUsrId);
		pLabelUserId->SetUserData(wstrUsrId);

		if (!pFindUser->GetText().IsEmpty())
		{
			pUserUi->SetVisible(wstrUsrId.Find(pFindUser->GetText()) >= 0);
		} 

		if (CSdkManager::GetInstance().GetLoginUserId() == user.user_id.c_str()) {
			pUserUi->SetEnabled(false);
			pSelectCb->SetVisible(false);
			pSelectCb->SetEnabled(false);
			pSelectCb->Selected(false);
		}

		//选中的重新设置为选中状态
		for (fsp::String selectedUserId : selectedUserIds) {
			if (user.user_id == selectedUserId) {
				
				pSelectCb->Selected(true);
				break;
			}
		}

		pListControl->Add(pUserUi);
	}

	LeaveCriticalSection(&g_CriticalSection);
}

void CUserStateWnd::ShowInviteCome(const InviteReqInfo& inviteInfo)
{
	//收到邀请，显示相关对话框
	CMessageBox* pMB = new CMessageBox;
	CDuiString strTip;
	strTip.Format(L"用户 %s 邀请你加入分组", demo::Utf82WStr(inviteInfo.strInviterUserId.c_str()).GetWStr());
	pMB->Create(GetHWND(), _T("CDuiLoginWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pMB->SetText(strTip);
	pMB->SetBtnText(L"拒绝", L"接受");
	pMB->CenterWindow();
	if (IDOK == pMB->ShowModal()) {
		CSdkManager::GetInstance().GetFspEngin()->GetFspSignaling()->AcceptInvite(
			inviteInfo.strInviterUserId.c_str(),
			inviteInfo.nInviteId);

		//同意后加入组
		CSdkManager::GetInstance().JoinGroup(demo::Utf82WStr(inviteInfo.strGroupId.c_str()).GetWStr());
	}
	else {
		CSdkManager::GetInstance().GetFspEngin()->GetFspSignaling()->RejectInvite(
			inviteInfo.strInviterUserId.c_str(),
			inviteInfo.nInviteId);
	}
}

CDuiString CUserStateWnd::GetSkinFolder()
{
	return CDuiString(L"skin");
}

CDuiString CUserStateWnd::GetSkinFile()
{
	return CDuiString(L"user_state_wnd.xml");
}

LPCTSTR CUserStateWnd::GetWindowClassName(void) const
{
	return L"CUserStateWnd";
}

void CUserStateWnd::InitWindow()
{
	SetIcon(IDI_FSPCLIENT);

	m_wndShadow.Create(m_hWnd);
	RECT rcCorner = { 6,6,6,6 };
	RECT rcHoleOffset = { 7,7,7,7 };
	m_wndShadow.SetImage(_T("img\\wnd_bg.png"), rcCorner, rcHoleOffset);
}

LRESULT CUserStateWnd::OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	PostQuitMessage(0); 
	return 0;
}

void CUserStateWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == _T("itemclick")) {
		CContainerUI* pItemLayout = (CContainerUI*)msg.pSender;
		CCheckBoxUI* pSelectCb = (CCheckBoxUI*)(pItemLayout->FindSubControl(L"check_user_select"));
		pSelectCb->Selected(!pSelectCb->IsSelected());
	}
	else if (msg.sType == _T("selectchanged"))
	{
		if (msg.pSender->GetName() == L"btn_selectAll")
		{
			COptionUI *pSelectAll = (COptionUI*)msg.pSender;
			bool bSelectAll = pSelectAll->IsSelected();

			CListUI *pListControl = static_cast<CListUI*>(m_PaintManager.FindControl(_T("list_users")));
			fsp::Vector<fsp::String> vecUserIds;
			for (int i = 0; i < pListControl->GetCount(); i++)
			{
				CContainerUI* pItem = (CContainerUI*)pListControl->GetItemAt(i);
				CCheckBoxUI* pSelectCb = (CCheckBoxUI*)(pItem->FindSubControl(L"check_user_select"));
				pSelectCb->Selected(bSelectAll);
			}
		}
		 
		UpdateJoinGroupBtn();
	}
	else if (msg.pSender->GetName() == _T("check_user_select"))
	{
		UpdateJoinGroupBtn();
	}
	else if (msg.sType == _T("textchanged") && msg.pSender->GetName() == L"edit_finduser")
	{
		CDuiString strtext = msg.pSender->GetText();
		UpdateCurUserList(strtext);
	}
	else if (msg.sType == _T("killfocus")  && msg.pSender->GetName() == L"edit_finduser")
	{
		CEditUI* pEdit = (CEditUI*)msg.pSender;
		if (pEdit)
		{
			pEdit->SetNormalImage( pEdit->GetText().IsEmpty()?_T("img\\user_find_KillFocus.png"):_T("img\\user_find_SetFocus.png"));
		}
	
	}

	WindowImplBase::Notify(msg);
}

void CUserStateWnd::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == L"btn_joingroup")
	{
		//邀请并加入组的按钮
		CEditUI* pUserEdit = (CEditUI*)m_PaintManager.FindControl(L"edit_groupid");
		CDuiString strGropuId = pUserEdit->GetText();

		if (strGropuId.IsEmpty() || strGropuId.GetLength() >= 32) {
			demo::ShowMessageBox(m_hWnd, L"GroupId不能为空且长度不超过32");
		}
		else
		{
			InviteSelectedUser(strGropuId);

			fsp::ErrCode errCode = CSdkManager::GetInstance().JoinGroup(strGropuId);
			if (errCode == fsp::ERR_OK) {
				CButtonUI* pBtnJoinGroup = (CButtonUI*)m_PaintManager.FindControl(L"btn_joingroup");
				pBtnJoinGroup->SetEnabled(false);
				pBtnJoinGroup->SetText(L"正在加入组...");
			}
			else {
				ShowJoinGroupError(CSdkManager::GetInstance().BuildErrorInfo(errCode));
			}
		}
	}
	else if (msg.pSender->GetName() == L"btn_bottom_invite" || msg.pSender->GetName() == L"btn_online_callIn") {
		InviteSelectedUser(demo::Utf82WStr(CSdkManager::GetInstance().GetLoginGroupId().c_str()).GetWStr());
	}
	else if (msg.pSender->GetName() == L"closebtn")
	{
		CMeetingMainWnd* pWnd = CSdkManager::GetInstance().GetMeetingMainWnd();
		if (pWnd&&pWnd->GetHWND() && ::IsWindowVisible(pWnd->GetHWND()))
		{
			this->ShowWindow(false);
		}
		else
		{
			CSdkManager::GetInstance().GetFspEngin()->Logout();
			::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
		}
	}
	else
	{
		WindowImplBase::OnClick(msg);
	}
}

LRESULT CUserStateWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch (uMsg) {
	case WM_ACTIVATE:
	{
		if (CSdkManager::GetInstance().IsLogined()) {
			//请求在线列表
			fsp::Vector<fsp::String> vecUserIds;
			unsigned int nRequestId;
			CSdkManager::GetInstance().GetFspEngin()->GetFspSignaling()->UserStatusRefresh(vecUserIds, &nRequestId);
		}
		break;
	}
	}
	return 0;
}

void CUserStateWnd::UpdateJoinGroupBtn()
{
	fsp::Vector<fsp::String> selectedUserIds = GetSelectedUserIds();
	CButtonUI* pBtnJoinGroup = (CButtonUI*)m_PaintManager.FindControl(_T("btn_joingroup"));
	CButtonUI* pBtnCallIn = (CButtonUI*)m_PaintManager.FindControl(_T("btn_online_callIn"));

	if (pBtnJoinGroup && pBtnJoinGroup->IsVisible())
	{
		if (selectedUserIds.empty()) 
		{
			pBtnJoinGroup->SetText(_T("加入组"));
		}
		else 
		{
			pBtnJoinGroup->SetText(_T("邀请并加入组"));
		}

		pBtnJoinGroup->SetEnabled(true);
	}
	else if (pBtnCallIn && pBtnCallIn->IsVisible())
	{
		pBtnCallIn->SetEnabled(!selectedUserIds.empty());
	}
}

fsp::Vector<fsp::String> CUserStateWnd::GetSelectedUserIds()
{
	CListUI *pListControl = static_cast<CListUI*>(m_PaintManager.FindControl(_T("list_users")));
	fsp::Vector<fsp::String> vecUserIds;

	for (int i = 0; i < pListControl->GetCount(); i++) {
		CContainerUI* pItem = (CContainerUI*)pListControl->GetItemAt(i);

		CLabelUI* pUserIdLabel = (CLabelUI*)pItem->FindSubControl(L"label_userid");
		CCheckBoxUI* pSelectCb = (CCheckBoxUI*)(pItem->FindSubControl(L"check_user_select"));
		CDuiString strUserId = pUserIdLabel->GetUserData();
		if (pSelectCb->IsSelected()) {
			vecUserIds.push_back(demo::WStr2Utf8(strUserId).GetUtf8Str());
		}
	}

	return vecUserIds;
}

void CUserStateWnd::InviteSelectedUser(const CDuiString& strGroupId)
{
	fsp::Vector<fsp::String> vecUserIds = GetSelectedUserIds();

	// 不用向自己发起呼叫
	fsp::Vector<fsp::String> vecUserIdsWithoutMe;
	std::string strMe = CSdkManager::GetInstance().GetLoginUserId();
	for (auto iter = vecUserIds.begin(); iter != vecUserIds.end(); iter++)
	{
		if ((*iter).c_str() != strMe)
			vecUserIdsWithoutMe.push_back(*iter);
	}
	
	if (!vecUserIdsWithoutMe.empty()) {
		unsigned int nInviteId;
		//@todo 
		CSdkManager::GetInstance().GetFspEngin()->GetFspSignaling()->Invite(vecUserIdsWithoutMe,
			demo::WStr2Utf8(strGroupId).GetUtf8Str(), "", &nInviteId);
	}
	else {
	}
}

void CUserStateWnd::UpdateCurUserList(const CDuiString & strUserID)
{
	EnterCriticalSection(&g_CriticalSection);

	CListUI *pListControl = static_cast<CListUI*>(m_PaintManager.FindControl(_T("list_users")));

	for (int i = 0; i < pListControl->GetCount(); i++)
	{
		CContainerUI* pItem = (CContainerUI*)pListControl->GetItemAt(i);

		CLabelUI* pUserIdLabel = (CLabelUI*)pItem->FindSubControl(L"label_userid");
		CDuiString strItemUserId = pUserIdLabel->GetUserData();
		if (strUserID.IsEmpty())
		{
			pItem->SetVisible(true);
		}
		else
		{
			int n = strItemUserId.Find(strUserID);
			pItem->SetVisible(strItemUserId.Find(strUserID) >= 0);
		}

	}

	LeaveCriticalSection(&g_CriticalSection);
}
