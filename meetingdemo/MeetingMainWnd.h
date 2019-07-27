
#pragma once

#include <set>
#include "UIlib.h"
#include "UIVideoPanelLayout.h"
#include "SpeakerSettingWnd.h"

using namespace DuiLib;

class CSdkManager;
class CSettingWnd;

class CMeetingMainWnd : public WindowImplBase 
{
public:
	CMeetingMainWnd();
	~CMeetingMainWnd();

	//加入组后，显示并做些ui初始化
	void ShowGroupJoined();
	void ShowInviteResponse(const InviteResponseInfo& info);

	void ResetWindowStatus();

	DUI_DECLARE_MESSAGE_MAP()

public:
	void OnAppendMsg(const char * szSenderUserId, const char* szMsg,bool bGroup = false);

	void InitUserList(const fsp::Vector<fsp::String>& user_ids);

private:
	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;
	virtual void InitWindow() override;
	virtual CControlUI* CreateControl(LPCTSTR pstrClass) override;

	void OnClickCamBtn(TNotifyUI& msg);

	void OnCamItemClicked(TNotifyUI& msg);
	
	void OnBroadcastCamChanged(WPARAM wParam, LPARAM lParam);
	
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled) override;
	
private:
	void OnClick(TNotifyUI& msg);
	void OnSelectChanged(TNotifyUI& msg);
	void OnSetFocus(TNotifyUI& msg);
	void OnKillFocus(TNotifyUI& msg);
	void OnItemSelectChange(TNotifyUI& msg);

	void SetToolbarCamBtnStatus(bool isOpen);
	void SetToolbarMicBtnStatus(bool isOpen);
	void SetToolbarScreenshareBtnStatus(bool isOpen);
	void AppendMsg(const CDuiString& strMsg,bool bSysMsg = true, 
		           const CDuiString& dstUser = L"",const CDuiString& srcUser=L"");
	void SetMsgFormat(const CDuiString& msg, CHARFORMAT2& cf, bool bUnderLine);

	void UpdateUserItemIcon(const std::string& strUserId, int iconType, bool isOpen);
	void AddMainUserItem(const std::string& strUserId);
	void DelMainUserItem(const std::string& strUserId);

	void AddChatUserItem(const std::string& strUserId);
	void DelChatUserItem(const std::string& strUserId);

	void InitMainUserList(const fsp::Vector<fsp::String>& user_ids);
	void InitChatUserList(const fsp::Vector<fsp::String>& user_ids);

private:

	CWndShadow m_wndShadow;

	CSettingWnd* m_pSettingWnd = nullptr;

	bool m_bVideoWndInitFlag;

	// 同时可以开启2个摄像头
	std::set<DWORD> m_setCamOpenIndexes;

	bool m_bBroadcastMic;
	bool m_isScreenSharing;
	bool m_isRecording;

	CVideoPanelLayoutUI* m_pScreenShareLayout;
	CVideoPanelLayoutUI* m_pVideoLayout;

	std::vector<RemoteAudioInfo> m_vecRemoteAudioInfo;
	std::vector<RemoteVideoInfo> m_vecRemoteVideoInfo;

	CSpeakerSettingWnd m_SpeakerSettingWnd;
};

