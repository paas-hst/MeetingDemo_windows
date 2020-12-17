#include "stdafx.h"
#include "SdkManager.h"
#include "define.h"
#include "util.h"
#include "ConfigParser.h"
#include "fsp_token.h"
#include <fstream>
#include <sstream>


////////////////////////////////////////////////////////////////////////////////

#define DESTROY_WND(WND_PTR)				\
	if (WND_PTR)							\
	{										\
		if (IsWindow(WND_PTR->GetHWND()))	\
			WND_PTR->Close();				\
		delete WND_PTR;						\
		WND_PTR = nullptr;					\
	}

////////////////////////////////////////////////////////////////////////////////

CSdkManager::CSdkManager()
	: m_pFspEngine(nullptr)
	, m_dwAudOpenIndex(0)
	, m_dwMicOpenIndex(0)
	, m_dwAudSetVoluem(50)
	, m_dwMicSetVolume(50)
	, m_nVoiceVariant(0)
	, m_dwResolutionIndex(0)	// 默认分辨率：320*240
	, m_dwFrameRate(15)			// 默认帧率：15帧/秒
	, m_pLoginWnd(nullptr)
	, m_pMeetingMainWnd(nullptr)
	, m_pUserStateWnd(nullptr)
{
	Create(NULL, _T("CSdkManagerMsgWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	ShowWindow(false);
}

CSdkManager::~CSdkManager()
{
	Destroy();
}

CSdkManager& CSdkManager::GetInstance()
{
	static CSdkManager mgr;
	return mgr;
}

bool CSdkManager::Init()
{
	m_pFspEngine = FspGetEngine();
	if (m_pFspEngine == nullptr)
		return false;

	/// 初始化引擎 

	demo::ClientConfig& config = demo::CConfigParser::GetInstance().GetClientConfig();

	m_FspEnginContext.app_id = config.bAppUserDefine ? config.strUserAppId.c_str() : config.strAppId.c_str();
	m_FspEnginContext.log_path = "./";
	m_FspEnginContext.event_handler = this;
	m_FspEnginContext.server_addr = config.bServerUserDefine ? config.strUserServerAddr.c_str() : config.strServerAddr.c_str();
	m_FspEnginContext.recv_voice_variant = atoi(config.strRecvVoiceVariant.c_str());
	 
	fsp::ErrCode result = m_pFspEngine->Init(m_FspEnginContext);
	if (result != ERR_OK)
		return false;

	/// 获取默认麦克风和扬声器并设置音量

	fsp::IAudioEngine *pAudioEngin = m_pFspEngine->GetAudioEngine();

	int nMicIndex = pAudioEngin->GetMicrophoneDevice();
	if (nMicIndex != -1 && nMicIndex >= 0)
	{
		m_dwMicOpenIndex = nMicIndex;
		pAudioEngin->SetAudioParam(AUDIOPARAM_MICROPHONE_VOLUME, m_dwMicSetVolume);
		pAudioEngin->SetAudioParam(AUDIOPARAM_VOICE_VARIANT, m_nVoiceVariant);
	}

	int nAudIndex = pAudioEngin->GetSpeakerDevice();
	if (nAudIndex != -1 && nAudIndex >= 0)
	{
		m_dwAudOpenIndex = nAudIndex;
		pAudioEngin->SetAudioParam(AUDIOPARAM_SPEAKER_VOLUME, m_dwAudSetVoluem);
	}

	/// 设置信令处理回调

	m_pFspSignaling = m_pFspEngine->GetFspSignaling();
	m_pFspSignaling->AddEventHandler(this);

	m_pFspEngine->GetFspWhiteBoard()->SetEventListener(this);

	return true;
}

void CSdkManager::Destroy()
{
	DESTROY_WND(m_pLoginWnd);
	DESTROY_WND(m_pUserStateWnd);
	DESTROY_WND(m_pMeetingMainWnd);

	Close();

	FspReleaseEngine();
}

void CSdkManager::OpenLoginWnd()
{
	if (m_pLoginWnd == nullptr) {
		m_pLoginWnd = new CLoginWnd;
		m_pLoginWnd->Create(NULL, _T("Login"), UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
	}

	if (m_pMeetingMainWnd == nullptr) {
		m_pMeetingMainWnd = new CMeetingMainWnd;
		m_pMeetingMainWnd->Create(NULL, _T("Main"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	}
	if (m_pUserStateWnd == nullptr) {
		m_pUserStateWnd = new CUserStateWnd();
		m_pUserStateWnd->Create(NULL, _T("UserState"), UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
	}

	m_pMeetingMainWnd->ShowWindow(false);
	m_pUserStateWnd->ShowWindow(false);

	m_pLoginWnd->CenterWindow();
	m_pLoginWnd->ShowWindow(true);
}

bool CSdkManager::IsLogined()
{
	return m_isLogined;
}

fsp::ErrCode CSdkManager::Login(LPCTSTR szUser, LPCTSTR szCustomName)
{
	m_strMyUserId = demo::WStr2Utf8(szUser).GetUtf8Str();

	// 生成Token
	fsp::String strToken = BuildToken(m_strMyUserId).c_str();

	demo::ClientConfig& config = demo::CConfigParser::GetInstance().GetClientConfig();

	return m_pFspEngine->Login(strToken.c_str(), m_strMyUserId.c_str(), config.bForceLogin, demo::WStr2Utf8(szCustomName).GetUtf8Str());
}

fsp::ErrCode CSdkManager::JoinGroup(LPCTSTR szGroup)
{
	m_strMyGroupId = demo::WStr2Utf8(szGroup).GetUtf8Str();
	m_mapMyPublishedVideoIds.clear();
	return m_pFspEngine->JoinGroup(m_strMyGroupId.c_str());
}

fsp::ErrCode CSdkManager::LeaveGroup()
{
	m_strMyGroupId.clear();
	m_pMeetingMainWnd->ShowWindow(false);
	m_pUserStateWnd->ShowGroupLeaved();
	return m_pFspEngine->LeaveGroup();
}

void CSdkManager::SetOpenMic(DWORD dwMicIndex)
{
	if (dwMicIndex != INVALID_MIC_INDEX)
	{
		m_dwMicOpenIndex = dwMicIndex;
		m_pFspEngine->GetAudioEngine()->SetMicrophoneDevice(dwMicIndex);
	}
}

DWORD CSdkManager::GetOpenMic()
{
	return m_dwMicOpenIndex;
}

void CSdkManager::SetOpenAud(DWORD dwAudIndex)
{
	if (dwAudIndex != INVALID_AUD_INDEX)
	{
		m_dwAudOpenIndex = dwAudIndex;
		m_pFspEngine->GetAudioEngine()->SetSpeakerDevice(dwAudIndex);
	}
}

DWORD CSdkManager::GetOpenAud()
{
	return m_dwAudOpenIndex;
}

void CSdkManager::SetMicVol(DWORD dwMicVol)
{
	assert(dwMicVol >= 0 && dwMicVol <= 100);

	m_pFspEngine->GetAudioEngine()->SetAudioParam(AUDIOPARAM_MICROPHONE_VOLUME, dwMicVol);

	m_dwMicSetVolume = dwMicVol;
}

DWORD CSdkManager::GetMicVol()
{
	return m_dwMicSetVolume;
}

void CSdkManager::SetVoiceVariant(INT nVoiceVariant)
{
	assert(nVoiceVariant >= -12 && nVoiceVariant <= 12);

	m_pFspEngine->GetAudioEngine()->SetAudioParam(AUDIOPARAM_VOICE_VARIANT, nVoiceVariant);

	m_nVoiceVariant = nVoiceVariant;
}

INT CSdkManager::GetVoiceVariant()
{
	return m_nVoiceVariant;
}

void CSdkManager::SetAudVol(DWORD dwAudVol)
{
	assert(dwAudVol >= 0 && dwAudVol <= 100);

	m_pFspEngine->GetAudioEngine()->SetAudioParam(AUDIOPARAM_SPEAKER_VOLUME, dwAudVol);

	m_dwAudSetVoluem = dwAudVol;
}

DWORD CSdkManager::GetAudVol()
{
	return m_dwAudSetVoluem;
}

void CSdkManager::SetFrameRate(DWORD dwFrameRate)
{
	if (dwFrameRate != m_dwFrameRate)
	{
		m_dwFrameRate = dwFrameRate;
		
		fsp::VideoProfile profile;
		profile.width = VideoResolutions[m_dwResolutionIndex].dwWidth;
		profile.height = VideoResolutions[m_dwResolutionIndex].dwHeight;
		profile.framerate = m_dwFrameRate;
		for (const auto& iter : m_mapMyPublishedVideoIds) {
			m_pFspEngine->SetVideoProfile(iter.first.c_str(), profile);
		}
	}
}

void CSdkManager::SetScreenShareConfig(const ScreenShareConfig& config)
{
	m_screenShareConfig = config;
}

std::string CSdkManager::GetCustomName(const char* szUserId)
{
	for (auto user : m_vecUsers) {
		if (user.user_id == szUserId && !user.custom_info.empty()) {
			return user.custom_info.c_str();
		}
	}

	//如果没有custominfo 则用userid作为昵称
	return szUserId;
}

ScreenShareConfig CSdkManager::GetScreenShareConfig() const
{
	return m_screenShareConfig;
}

DWORD CSdkManager::GetFrameRate()
{
	return m_dwFrameRate;
}

void CSdkManager::SetResolution(DWORD dwResolutionIndex)
{
	if (dwResolutionIndex != m_dwResolutionIndex)
	{
		m_dwResolutionIndex = dwResolutionIndex;
		fsp::VideoProfile profile;
		profile.width = VideoResolutions[m_dwResolutionIndex].dwWidth;
		profile.height = VideoResolutions[m_dwResolutionIndex].dwHeight;
		profile.framerate = m_dwFrameRate;
		for (const auto& iter : m_mapMyPublishedVideoIds) {
			m_pFspEngine->SetVideoProfile(iter.first.c_str(), profile);
		}
	}
}

DWORD CSdkManager::GetResolution()
{
	return m_dwResolutionIndex;
}

fsp::ErrCode CSdkManager::StartPublishVideo(const char* szVideoId, int nCameraId)
{
	m_mapMyPublishedVideoIds[szVideoId] = nCameraId;
	return m_pFspEngine->StartPublishVideo(szVideoId, nCameraId);
}

fsp::ErrCode CSdkManager::StopPublishVideo(const char* szVideoId)
{
	m_mapMyPublishedVideoIds.erase(szVideoId);
	return m_pFspEngine->StopPublishVideo(szVideoId);
}

void CSdkManager::OnDeviceChange(DeviceEventType device_event)
{
	m_pMeetingMainWnd->PostMessageW(DUILIB_MSG_DEVICECHANGE, 0, 0);
}

void CSdkManager::OnEvent(EventType event_type, ErrCode err_code)
{
	PostMessage(DUILIB_MSG_FSP_EVENT, event_type, err_code);
}

LRESULT CSdkManager::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	if (uMsg == DUILIB_MSG_FSP_EVENT) {
		EventType event_type = (EventType)wParam;
		ErrCode err_code = (ErrCode)lParam;

		OnFspEvent(event_type, err_code);
	}else if (uMsg == DUILIB_MSG_USER_REFRESH_FINISH) {
		//刷新在线列表消息结果
		m_pUserStateWnd->UpdateUserList();
	}
	else if (uMsg == DUILIB_MSG_INVITE_COME) {
		InviteReqInfo* pInviteInfo = (InviteReqInfo*)wParam;
		m_pUserStateWnd->ShowInviteCome(*pInviteInfo);
		delete pInviteInfo;
	}
	else if (uMsg == DUILIB_MSG_INVITE_RESPONSE) {
		InviteResponseInfo* pInviteInfo = (InviteResponseInfo*)wParam;
		m_pMeetingMainWnd->ShowInviteResponse(*pInviteInfo);
		delete pInviteInfo;
	}
	else {
		bHandled = FALSE;
	}

	return S_OK;
}

void CSdkManager::OnFspEvent(fsp::EventType eventType, fsp::ErrCode result)
{
	if (eventType == fsp::EVENT_LOGIN_RESULT) {
		if (result == ErrCode::ERR_OK) // 登录成功，显示主窗口
		{
			m_isLogined = true;

			m_pLoginWnd->ShowWindow(false);

			m_pUserStateWnd->CenterWindow();
			m_pUserStateWnd->ShowWindow(true);

			//请求在线列表
			fsp::Vector<fsp::String> vecUserIds;
			unsigned int nRequestId;
			m_pFspSignaling->UserStatusRefresh(vecUserIds, &nRequestId);
		}
		else {
			m_isLogined = false;

			m_pLoginWnd->ShowError(BuildErrorInfo(result));
		}
	}
	else if (eventType == fsp::EVENT_JOINGROUP_RESULT) {

		assert(m_pLoginWnd);

		if (result == ErrCode::ERR_OK)
		{
			// 登录成功，显示主窗口, 调整在线列表窗口显示
			m_pMeetingMainWnd->ShowGroupJoined();
			m_pUserStateWnd->ShowWindow(false);
		}
		else // 加入组失败
		{
			m_pUserStateWnd->ShowJoinGroupError(BuildErrorInfo(result));
		}
	}
	else if (eventType == fsp::EVENT_CONNECT_LOST){
		if (result == fsp::ErrCode::ERR_USERID_CONFLICT)
		{
			OnUserForceLogin();
		}
	}
}

void CSdkManager::OnRemoteVideoEvent(const String& user_id, const String& video_id, RemoteVideoEventType remote_video_event)
{
	RemoteVideoInfo* pInfo = new RemoteVideoInfo;
	pInfo->strUserId = user_id;
	pInfo->strVideoId = video_id;

	if (REMOTE_VIDEO_EVENT_PUBLISHE_STARTED == remote_video_event)
	{
		m_pMeetingMainWnd->PostMessageW(DUILIB_MSG_ADD_REMOTE_VIDEO, (WPARAM)pInfo, 0);
	}
	else if (REMOTE_VIDEO_EVENT_PUBLISHE_STOPED == remote_video_event)
	{
		m_pMeetingMainWnd->PostMessageW(DUILIB_MSG_DEL_REMOTE_VIDEO, (WPARAM)pInfo, 0);
	}
}

void CSdkManager::OnRemoteAudioEvent(const String& user_id, const String& audio_id, RemoteAudioEventType remote_audio_event)
{
	if (m_pMeetingMainWnd && IsWindow(m_pMeetingMainWnd->GetHWND())) {
		RemoteAudioInfo* pInfo = new RemoteAudioInfo;
		pInfo->strUserId = user_id;

		if (REMOTE_AUDIO_EVENT_PUBLISHE_STARTED == remote_audio_event)
		{
			m_pMeetingMainWnd->PostMessageW(DUILIB_MSG_ADD_REMOTE_AUDIO, (WPARAM)pInfo, 0);
		}
		else if (REMTOE_AUDIO_EVENT_PUBLISHE_STOPED == remote_audio_event)
		{
			m_pMeetingMainWnd->PostMessageW(DUILIB_MSG_DEL_REMOTE_AUDIO, (WPARAM)pInfo, 0);
		}
	}
}

void CSdkManager::OnGroupUsersRefreshed(const fsp::Vector<fsp::String>& user_ids)
{
	m_pMeetingMainWnd->InitUserList(user_ids);
}

void CSdkManager::OnRemoteUserEvent(const char* szRemoteUserId, fsp::RemoteUserEventType remote_user_eventType)
{
	if (m_pMeetingMainWnd && IsWindow(m_pMeetingMainWnd->GetHWND())) {
		RemoteUserEventInfo* pInfo = new RemoteUserEventInfo;
		pInfo->remote_userid = szRemoteUserId;

		if (REMOTE_USER_EVENT_GROUP_JOINED == remote_user_eventType)
		{
			m_pMeetingMainWnd->PostMessageW(DUILIB_MSG_REMOTE_ADD_USER, (WPARAM)pInfo, 0);
		}
		else if (REMOTE_USER_EVENT_GROUP_LEAVED == remote_user_eventType)
		{
			m_pMeetingMainWnd->PostMessageW(DUILIB_MSG_REMOTE_DEL_USER, (WPARAM)pInfo, 0);
		}
	}
}

void CSdkManager::OnRemoteControlOperationEvent(const String& user_id,
	fsp::RemoteControlOperationType operation_type)
{
	RemoteControlInfo* pInfo = new RemoteControlInfo;
	pInfo->strUserId = user_id;
	pInfo->operationType = operation_type;
	m_pMeetingMainWnd->PostMessage(DUILIB_MSG_REMOTECONTROL_EVENT, (WPARAM)pInfo, 0);
}

void CSdkManager::OnUsersStateRefreshed(fsp::ErrCode errCode, unsigned int nRequestId, const fsp::Vector<fsp::UserInfo> users)
{
	m_vecUsers = users;
	PostMessage(DUILIB_MSG_USER_REFRESH_FINISH);
}

void CSdkManager::OnUserStateChange(const fsp::UserInfo& changedUserInfo)
{
	fsp::Vector<fsp::UserInfo>::iterator iter;
	for (iter = m_vecUsers.begin(); iter != m_vecUsers.end(); iter++) {
		if (iter->user_id == changedUserInfo.user_id) {
			break;
		}
	}

	if (iter == m_vecUsers.end() && changedUserInfo.state == fsp::USER_STATE_ONLINE) {
		m_vecUsers.push_back(changedUserInfo);
	}
	else if (iter != m_vecUsers.end() && changedUserInfo.state == fsp::USER_STATE_OFFLINE) {
		m_vecUsers.erase(iter);
	}
	else if (iter != m_vecUsers.end()) {
		iter->state = changedUserInfo.state;
	}

	PostMessage(DUILIB_MSG_USER_REFRESH_FINISH);
}

void CSdkManager::OnInviteCome(const char* szInviterUserId,
	unsigned int nInviteId, const char* szGroupId, const char* szMsg)
{
	InviteReqInfo* pInfo = new InviteReqInfo();
	pInfo->strInviterUserId = szInviterUserId;
	pInfo->strGroupId = szGroupId;
	pInfo->strMsg = szMsg;
	pInfo->nInviteId = nInviteId;

	PostMessage(DUILIB_MSG_INVITE_COME, (WPARAM)pInfo, 0);
}

void CSdkManager::OnInviteAccepted(const char* szRemoteUserId, unsigned int nInviteId)
{
	InviteResponseInfo* pInfo = new InviteResponseInfo();
	pInfo->strRemoteUserId = szRemoteUserId;
	pInfo->nInviteId = nInviteId;
	pInfo->isAccept = true;

	PostMessage(DUILIB_MSG_INVITE_RESPONSE, (WPARAM)pInfo, 0);
}

void CSdkManager::OnInviteRejected(const char* szRemoteUserId, unsigned int nInviteId)
{
	InviteResponseInfo* pInfo = new InviteResponseInfo();
	pInfo->strRemoteUserId = szRemoteUserId;
	pInfo->nInviteId = nInviteId;
	pInfo->isAccept = false;

	PostMessage(DUILIB_MSG_INVITE_RESPONSE, (WPARAM)pInfo, 0);
}

void CSdkManager::OnUserMsgCome(const char * szSenderUserId, unsigned int nMsgId, const char * szMsg)
{
	m_pMeetingMainWnd->AppendMsgMainThread(szSenderUserId,szMsg);
}

void CSdkManager::OnGroupMsgCome(const char * szSenderUserId, unsigned int nMsgId, const char * szMsg)
{
	m_pMeetingMainWnd->AppendMsgMainThread(szSenderUserId, szMsg, true);
}

void CSdkManager::OnWhiteBoardCreateResult(const String& strBoardId, const String& strBoardName, ErrCode result)
{
	CDuiString strInfo;
	strInfo.Format(L"白板：%s, id: %s, 创建结果:%d",
		demo::Utf82WStr(strBoardName.c_str()), demo::Utf82WStr(strBoardId.c_str()), result);
	m_pMeetingMainWnd->AppendCommonInfoMainThread(strInfo);
}

void CSdkManager::OnWhiteBoardPublishStart(const String& strBoardId, const String& strBoardName)
{
	WhiteBoardPulishInfo* pInfo = new WhiteBoardPulishInfo();
	pInfo->strWhiteBoardName = strBoardName.c_str();
	pInfo->strWhiteBoardId = strBoardId.c_str();
	pInfo->isPublish = true;

	m_pMeetingMainWnd->PostMessage(DUILIB_MSG_WHITEBOARD_PUBLISH, (WPARAM)pInfo, 0);
}

void CSdkManager::OnWhiteBoardPublishStop(const String& strBoardId)
{
	WhiteBoardPulishInfo* pInfo = new WhiteBoardPulishInfo();
	pInfo->strWhiteBoardId = strBoardId.c_str();
	pInfo->isPublish = false;

	m_pMeetingMainWnd->PostMessage(DUILIB_MSG_WHITEBOARD_PUBLISH, (WPARAM)pInfo, 0);
}

void CSdkManager::OnBoardSynUpdate(const String& nBoardId,
	const WhiteBoardProfile& whiteboard_profile, int nCurrentPageID)
{
	RemoteWhiteboardInfo* pInfo = new RemoteWhiteboardInfo;
	pInfo->nPageId = nCurrentPageID;
	pInfo->strMediaId = nBoardId;
	pInfo->whiteboard_profile = whiteboard_profile;
	m_pMeetingMainWnd->PostMessageW(DUILIB_MSG_WHITEBOARD_SYN_DATA, (WPARAM)pInfo, 0);
}

void CSdkManager::OnRemoteChangePage(const String& nBoardId, int nCurrentPageID)
{
	RemoteWhiteboardInfo* pInfo = new RemoteWhiteboardInfo;
	pInfo->nPageId = nCurrentPageID;
	pInfo->strMediaId = nBoardId;

	m_pMeetingMainWnd->PostMessageW(DUILIB_MSG_WHITEBOARD_PAGE_CHANGE, (WPARAM)pInfo, 0);
}

void CSdkManager::OnDocumentEvent(fsp_wb::DocStatusType doc_status_type, ErrCode err_code)
{
	WhiteboardDocumentInfo* pInfo = new WhiteboardDocumentInfo;
	pInfo->doc_status_type = doc_status_type;
	pInfo->err_code = err_code;
	m_pMeetingMainWnd->PostMessageW(DUILIB_MSG_DOCUMENT_EVENT, (WPARAM)pInfo, 0);
}

CDuiString CSdkManager::BuildErrorInfo(fsp::ErrCode errCode)
{
	CDuiString strErrInfo = L"未知错误:";
	if (errCode == ErrCode::ERR_TOKEN_INVALID)
		strErrInfo = L"认证失败！";
	else if (errCode == ErrCode::ERR_CONNECT_FAIL)
		strErrInfo = L"连接服务器失败！";
	else if (errCode == ErrCode::ERR_APP_NOT_EXIST)
		strErrInfo = L"应用不存在！";
	else if (errCode == ErrCode::ERR_USERID_CONFLICT)
		strErrInfo = L"用户已登录！";
	else if (errCode == ErrCode::ERR_NO_BALANCE)
		strErrInfo = L"账户余额不足！";
	else
	{
		WCHAR szTmp[8];
		_snwprintf_s(szTmp, 8, L"%d", errCode);
		strErrInfo.Append(szTmp);
	}

	return strErrInfo;
}

std::string CSdkManager::BuildToken(const std::string& struserId)
{
	demo::ClientConfig& config = demo::CConfigParser::GetInstance().GetClientConfig();
	
	fsp::tools::AccessToken token(config.bAppUserDefine ? config.strUserAppSecret : config.strAppSecret);
	token.app_id		= config.bAppUserDefine ? config.strUserAppId : config.strAppId;
	token.user_id		= struserId;
	token.expire_time	= 0;

	return token.Build();
}

const std::string& CSdkManager::GetLoginGroupId()
{
	return m_strMyGroupId;
}

const std::string CSdkManager::GetLoginUserId()
{
	return m_strMyUserId;
}

void CSdkManager::OnUserForceLogin()
{
	demo::ShowMessageBox(NULL, CDuiString(L"用户强制登录，连接断开！"));

	if (m_pMeetingMainWnd) {
		m_pMeetingMainWnd->ShowWindow(false);
		m_pMeetingMainWnd->Close();
	}

	if (m_pUserStateWnd) {
		m_pUserStateWnd->ShowWindow(false);
		m_pUserStateWnd->Close();
	}

	if (m_pLoginWnd) {
		m_pLoginWnd->ShowWindow(true);
	}
}

CDuiString CSdkManager::GetSkinFolder()
{
	return CDuiString(L"skin");
}

CDuiString CSdkManager::GetSkinFile()
{
	return CDuiString(L"emptywnd.xml");
}

LPCTSTR CSdkManager::GetWindowClassName(void) const
{
	return L"CSdkManagerMsgWnd";
}