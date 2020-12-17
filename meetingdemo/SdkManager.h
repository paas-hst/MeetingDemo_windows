﻿/*##############################################################################
 * 文件：SdkManager.h
 * 描述：SDK管理器的声明与定义
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#pragma once

#include "fsp_engine.h"
#include <map>
#include "LoginWnd.h"
#include "MeetingMainWnd.h"
#include "UserStateWnd.h"
#include "fsp_white_board.h"
#include "doc_define.h"

using namespace fsp;

//为了处理fsp事件转到主线程的消息，所以从WindowImplBase继承
class CSdkManager : 
	public IFspEngineEventHandler, 
	public IFspSignalingEventHandler, 
	public WindowImplBase, 
	public IFspWhiteBoardEventListener
{
public:
	static CSdkManager& GetInstance();
	
	bool Init();
	void Destroy();

	void OpenLoginWnd();

	bool IsLogined();

	fsp::ErrCode Login(LPCTSTR szUser, LPCTSTR szCustomName);
	fsp::ErrCode JoinGroup(LPCTSTR szGroup);
	fsp::ErrCode LeaveGroup();

	void SetOpenMic(DWORD dwMicIndex);
	DWORD GetOpenMic();

	void SetOpenAud(DWORD dwAudIndex);
	DWORD GetOpenAud();

	void SetMicVol(DWORD dwMicVol);
	DWORD GetMicVol();

	void SetVoiceVariant(INT nVoiceVariant);
	INT GetVoiceVariant();

	void SetAudVol(DWORD dwAudVol);
	DWORD GetAudVol();

	const std::string& GetLoginGroupId();
	const std::string GetLoginUserId();

	void SetResolution(DWORD dwResolutionIndex);
	DWORD GetResolution();

	void SetFrameRate(DWORD dwFrameRate);
	DWORD GetFrameRate();

	fsp::Vector<fsp::UserInfo> GetUserList() { return m_vecUsers; }
	std::string GetCustomName(const char* szUserId);

	void SetScreenShareConfig(const ScreenShareConfig& config);
	ScreenShareConfig GetScreenShareConfig() const;

	IFspEngine* GetFspEngin() { return m_pFspEngine; }

	CMeetingMainWnd* GetMeetingMainWnd() { return m_pMeetingMainWnd; }

	CUserStateWnd* GetUserStateWnd() { return m_pUserStateWnd; }

	CDuiString BuildErrorInfo(fsp::ErrCode errCode);

	void SetCurSelectChatUser(fsp::String userid) { m_CurSelectChatUser = userid; }
	const fsp::String GetCurSelectChatUser() { return m_CurSelectChatUser; }

	fsp::ErrCode StartPublishVideo(const char* szVideoId, int nCameraId);
	fsp::ErrCode StopPublishVideo(const char* szVideoId);
	
private:
	CSdkManager();
	~CSdkManager();
	
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;
	
	/////////  begin IFspEngineEventHandler
	void OnFspEvent(fsp::EventType eventType, fsp::ErrCode result);
	virtual void OnDeviceChange(DeviceEventType device_event) override;
	virtual void OnEvent(EventType event_type, ErrCode err_code) override;
	virtual void OnRemoteVideoEvent(const String& user_id, 
		const String& video_id,
		RemoteVideoEventType remote_video_event) override;
	virtual void OnRemoteAudioEvent(const String& user_id, const String& audio_id,
		RemoteAudioEventType remote_audio_event) override;
	virtual void OnRemoteControlOperationEvent(const String& user_id,
		fsp::RemoteControlOperationType operation_type) override;
	virtual void OnGroupUsersRefreshed(const fsp::Vector<fsp::String>& user_ids) override;
	virtual void OnRemoteUserEvent(const char* szRemoteUserId, fsp::RemoteUserEventType remote_user_eventType) override;
	/////////   end IFspEngineEventHandler


	////////////// begin IFspSignalingEventHandler
	void OnUsersStateRefreshed(fsp::ErrCode errCode, unsigned int nRequestId, const Vector<fsp::UserInfo> users);
	void OnUserStateChange(const fsp::UserInfo& changedUserInfo);
	void OnInviteCome(const char* szInviterUserId,
		unsigned int nInviteId, const char* szGroupId, const char* szMsg);
	void OnInviteAccepted(const char* szRemoteUserId, unsigned int nInviteId);
	void OnInviteRejected(const char* szRemoteUserId, unsigned int nInviteId);
	void OnUserMsgCome(const char* szSenderUserId, unsigned int nMsgId, const char* szMsg);
	void OnGroupMsgCome(const char* szSenderUserId, unsigned int nMsgId, const char* szMsg);
	/////////////  end IFspSignalingEventHandler

	////////////// begin IFspBoardEventListener
	void OnWhiteBoardCreateResult(const String& strBoardId, const String& strBoardName, ErrCode result);
	void OnWhiteBoardPublishStart(const String& strBoardId, const String& strBoardName);
	void OnWhiteBoardPublishStop(const String& nBoardId);
	void OnBoardSynUpdate(const String& nBoardId,
		const WhiteBoardProfile& whiteboard_profile, int nCurrentPageID);
	void OnRemoteChangePage(const String& nBoardId, int nCurrentPageID);
	void OnDocumentEvent(fsp_wb::DocStatusType doc_event_type, ErrCode err_code);
	/////////////  end IFspBoardEventListener

	std::string BuildToken(const std::string& struserId);
	void OnUserForceLogin();
	
private:
	CLoginWnd*		    m_pLoginWnd = nullptr;
	CMeetingMainWnd*	m_pMeetingMainWnd = nullptr;
	CUserStateWnd*      m_pUserStateWnd = nullptr;

	bool m_isLogined = false;

	IFspEngine*			m_pFspEngine;
	fsp::IFspSignaling* m_pFspSignaling;

	FspEngineContext	m_FspEnginContext;

	std::string m_strMyGroupId;
	std::string m_strMyUserId;

	DWORD m_dwMicSetVolume = 0;	// 麦克风设置音量值
	INT	  m_nVoiceVariant = 0; //麦克风设置变声
	DWORD m_dwMicOpenIndex = 0;	// 选择使用的麦克风

	DWORD m_dwAudSetVoluem; // 扬声器设置音量值
	DWORD m_dwAudOpenIndex; // 选择使用的扬声器

	DWORD m_dwResolutionIndex;	// 分辨率
	DWORD m_dwFrameRate;		// 帧率

	std::map<std::string, int> m_mapMyPublishedVideoIds;
	
	ScreenShareConfig m_screenShareConfig;

	fsp::Vector<fsp::UserInfo> m_vecUsers;

	fsp::String m_CurSelectChatUser="所有人";
};

