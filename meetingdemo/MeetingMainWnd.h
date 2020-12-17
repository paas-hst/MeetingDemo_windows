
#pragma once

#include <set>
#include "UIlib.h"
#include "UIVideoPanelLayout.h"
#include "SpeakerSettingWnd.h"
#include "BoardCreateWnd.h"

using namespace DuiLib;

class CSdkManager;
class CSettingWnd;

struct WhiteboardLayoutInfo
{
	std::string strBoardId;
	DuiLib::CControlUI* pLayout;
	DuiLib::CControlUI* pOption;
};

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
	void AppendMsgMainThread(const char * szSenderUserId, const char* szMsg,bool bGroup = false);
	void AppendCommonInfoMainThread(const CDuiString& strMsg);

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
	void SetToolbarBoardBtnStatus(bool isOpen);

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

	CControlUI* EnsureBoardLayout(const std::string&  strboardId, const std::string& strboardname);
	CControlUI* FindBoardLayout(const std::string&  strboardId);
	void ReleaseBoardLayout(const std::string& strboardId);
	void ClearAllBoardLayout();
	void SetBoardEditStatus(CControlUI* pPrient, bool enable);
	void SetBoardPageBtnStatus(CControlUI* pPrient, bool isFrist, bool isEnd);
	void OnDocumentEvent(fsp_wb::DocStatusType doc_event_type, fsp::ErrCode err_code);


private:

	CWndShadow m_wndShadow;

	CSettingWnd* m_pSettingWnd = nullptr;

	std::string m_strCurBoardId;

	// 同时可以开启2个摄像头
	std::set<DWORD> m_setCamOpenIndexes;

	bool m_bVideoWndInitFlag;
	bool m_bBroadcastMic;
	bool m_isScreenSharing;
	bool m_isRecording;
	bool m_isBoarding;

	CVideoPanelLayoutUI* m_pScreenShareLayout;
	CVideoPanelLayoutUI* m_pVideoLayout;

	std::vector<RemoteAudioInfo> m_vecRemoteAudioInfo;
	std::vector<RemoteVideoInfo> m_vecRemoteVideoInfo;

	std::vector<WhiteboardLayoutInfo> m_vecBoards;

	CSpeakerSettingWnd m_SpeakerSettingWnd;
	CBoardCreateWnd m_BoardCreateWnd;
};

