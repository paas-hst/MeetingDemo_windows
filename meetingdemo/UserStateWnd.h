#pragma once

#include <set>
#include "UIlib.h"
#include "define.h"

using namespace DuiLib;

class CSdkManager;
class CDuiSettingWnd;

class CUserStateWnd : public WindowImplBase 
{
public:
	CUserStateWnd();
	~CUserStateWnd();
	
	void ShowGroupJoined();
	void ShowGroupLeaved();
	void ShowJoinGroupError(const CDuiString& strErrorInfo);
	void UpdateUserList();

	void ShowInviteCome(const InviteReqInfo& inviteInfo);

	DUI_DECLARE_MESSAGE_MAP()

private:
	virtual void Notify(TNotifyUI& msg) override;
	virtual void OnClick(TNotifyUI& msg) override;

	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;
	virtual void InitWindow() override;
	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled) override;
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

	void UpdateJoinGroupBtn();
	fsp::Vector<fsp::String> GetSelectedUserIds();
	void InviteSelectedUser(const CDuiString& strGroupId);

	void UpdateCurUserList(const CDuiString& strUserID);

	CWndShadow m_wndShadow;
	bool bInFind = false;
};

