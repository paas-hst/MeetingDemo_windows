/*##############################################################################
 * 文件：SdkManager.cpp
 * 描述：SDK管理器实现，主要实现登录逻辑，管理SDK
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#include "stdafx.h"
#include "SdkManager.h"
#include "define.h"
#include "util.h"
#include "ConfigParser.h"
#include "fsp_token.h"
#include <fstream>
#include <sstream>

/*------------------------------------------------------------------------------
 * 描  述：构造函数
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
CSdkManager::CSdkManager()
	: m_pFspEngin(nullptr)
	, m_dwAudOpenIndex(INVALID_AUD_INDEX)
	, m_dwMicOpenIndex(INVALID_MIC_INDEX)
	, m_dwAudSetVoluem(50)
	, m_dwMicSetVolume(50)
	, m_dwResolutionIndex(0)	// 默认分辨率：320*240
	, m_dwFrameRate(15)			// 默认帧率：15帧/秒
	, m_pDuiLoginWnd(nullptr)
	, m_pDuiLoginWaitWnd(nullptr)
	, m_pDuiLoginErrorWnd(nullptr)
	, m_pDuiFrameWnd(nullptr)
	, m_bRestart(false)
{
	Create(NULL, _T("CSdkManagerMsgWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	ShowWindow(false);
}

/*------------------------------------------------------------------------------
 * 描  述：析构函数
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
CSdkManager::~CSdkManager()
{
	Destroy();
}

/*------------------------------------------------------------------------------
 * 描  述：静态函数，获取静态实例
 * 参  数：无
 * 返回值：静态实例
------------------------------------------------------------------------------*/
CSdkManager& CSdkManager::GetInstance()
{
	static CSdkManager mgr;
	return mgr;
}

/*------------------------------------------------------------------------------
 * 描  述：初始化
 * 参  数：无
 * 返回值：成功/失败
------------------------------------------------------------------------------*/
bool CSdkManager::Init()
{
	m_pFspEngin = FspGetEngine();
	if (m_pFspEngin == nullptr)
		return false;

	demo::ClientConfig& config = demo::CConfigParser::GetInstance().GetClientConfig();

	m_FspEnginContext.app_id = config.bUserDefine ? config.strUserAppId.c_str() : config.strAppId.c_str();
	m_FspEnginContext.log_path = "./";
	m_FspEnginContext.event_handler = this;
	m_FspEnginContext.server_addr = config.strServerAddr.c_str();
	
	fsp::ErrCode result = m_pFspEngin->Init(m_FspEnginContext);
	if (result != ERR_OK)
		return false;

	fsp::IAudioEngine *pAudioEngin = m_pFspEngin->GetAudioEngine();

	// 获取默认麦克风和扬声器并设置音量
	int nMicIndex = pAudioEngin->GetMicrophoneDevice();
	if (nMicIndex != -1 && nMicIndex >= 0)
	{
		m_dwMicOpenIndex = nMicIndex;
		pAudioEngin->SetAudioParam(AUDIOPARAM_MICROPHONE_VOLUME, m_dwMicSetVolume);
	}

	int nAudIndex = pAudioEngin->GetSpeakerDevice();
	if (nAudIndex != -1 && nAudIndex >= 0)
	{
		m_dwAudOpenIndex = nAudIndex;
		pAudioEngin->SetAudioParam(AUDIOPARAM_SPEAKER_VOLUME, m_dwAudSetVoluem);
	}

	return true;
}

void CSdkManager::Destroy()
{
	Close();
	if (m_pDuiLoginErrorWnd) {
		if (IsWindow(m_pDuiLoginErrorWnd->GetHWND()))
			m_pDuiLoginErrorWnd->Close();
		delete m_pDuiLoginErrorWnd;
		m_pDuiLoginErrorWnd = nullptr;
	}

	if (m_pDuiLoginWnd) {
		if (IsWindow(m_pDuiLoginWnd->GetHWND()))
			m_pDuiLoginWnd->Close();
		delete m_pDuiLoginWnd;
		m_pDuiLoginWnd = nullptr;
	}

	if (m_pDuiFrameWnd) {
		if (IsWindow(m_pDuiFrameWnd->GetHWND()))
			m_pDuiFrameWnd->Close();
		delete m_pDuiFrameWnd;
		m_pDuiFrameWnd = nullptr;
	}

	if (m_pDuiLoginWaitWnd) {
		if (IsWindow(m_pDuiLoginWaitWnd->GetHWND()))
			m_pDuiLoginWaitWnd->Close();
		delete m_pDuiLoginWaitWnd;
		m_pDuiLoginWaitWnd = nullptr;
	}

	FspReleaseEngine();
}

/*------------------------------------------------------------------------------
 * 描  述：启动
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::OpenLoginWnd()
{
	if (m_pDuiLoginErrorWnd == nullptr) {
		m_pDuiLoginErrorWnd = new CDuiLoginErrorWnd();
		m_pDuiLoginErrorWnd->Create(NULL, _T("LoginError"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);

	}

	if (m_pDuiLoginWnd == nullptr) {
		m_pDuiLoginWnd = new CDuiLoginWnd;
		m_pDuiLoginWnd->Create(NULL, _T("Login"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	}

	if (m_pDuiFrameWnd == nullptr) {
		m_pDuiFrameWnd = new CDuiFrameWnd;
		m_pDuiFrameWnd->Create(NULL, _T("Main"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	}

	if (m_pDuiLoginWaitWnd == nullptr) {
		m_pDuiLoginWaitWnd = new CDuiLoginWaitWnd();
		m_pDuiLoginWaitWnd->Create(NULL, _T("Logining"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	}

	m_pDuiLoginErrorWnd->ShowWindow(false);
	m_pDuiFrameWnd->ShowWindow(false);
	m_pDuiLoginWaitWnd->ShowWindow(false);

	m_pDuiLoginWnd->CenterWindow();
	m_pDuiLoginWnd->ShowWindow(true);
}

/*------------------------------------------------------------------------------
 * 描  述：设置使用的麦克风设备
 * 参  数：[in] dwMicIndex 麦克风索引
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::SetOpenMic(DWORD dwMicIndex)
{
	if (dwMicIndex != INVALID_MIC_INDEX)
	{
		m_dwMicOpenIndex = dwMicIndex;
		m_pFspEngin->GetAudioEngine()->SetMicrophoneDevice(dwMicIndex);
	}
}

/*------------------------------------------------------------------------------
 * 描  述：获取使用的麦克风设备索引
 * 参  数：无
 * 返回值：麦克风索引
------------------------------------------------------------------------------*/
DWORD CSdkManager::GetOpenMic()
{
	return m_dwMicOpenIndex;
}

/*------------------------------------------------------------------------------
 * 描  述：设置使用的扬声器设备
 * 参  数：[in] dwAudIndex 扬声器索引
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::SetOpenAud(DWORD dwAudIndex)
{
	if (dwAudIndex != INVALID_AUD_INDEX)
	{
		m_dwAudOpenIndex = dwAudIndex;
		m_pFspEngin->GetAudioEngine()->SetSpeakerDevice(dwAudIndex);
	}
}

/*------------------------------------------------------------------------------
 * 描  述：获取扬声器设备索引
 * 参  数：无
 * 返回值：扬声器索引
------------------------------------------------------------------------------*/
DWORD CSdkManager::GetOpenAud()
{
	return m_dwAudOpenIndex;
}

/*------------------------------------------------------------------------------
 * 描  述：设置麦克风音量
 * 参  数：[in] dwMicVol 音量
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::SetMicVol(DWORD dwMicVol)
{
	assert(dwMicVol >= 0 && dwMicVol <= 100);

	m_pFspEngin->GetAudioEngine()->SetAudioParam(AUDIOPARAM_MICROPHONE_VOLUME, dwMicVol);

	m_dwMicSetVolume = dwMicVol;
}

/*------------------------------------------------------------------------------
 * 描  述：获取麦克风音量
 * 参  数：无
 * 返回值：音量
------------------------------------------------------------------------------*/
DWORD CSdkManager::GetMicVol()
{
	return m_dwMicSetVolume;
}

/*------------------------------------------------------------------------------
 * 描  述：设置扬声器音量
 * 参  数：[in] dwAudVol 音量
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::SetAudVol(DWORD dwAudVol)
{
	assert(dwAudVol >= 0 && dwAudVol <= 100);

	m_pFspEngin->GetAudioEngine()->SetAudioParam(AUDIOPARAM_SPEAKER_VOLUME, dwAudVol);

	m_dwAudSetVoluem = dwAudVol;
}

/*------------------------------------------------------------------------------
 * 描  述：获取扬声器音量
 * 参  数：无
 * 返回值：音量
------------------------------------------------------------------------------*/
DWORD CSdkManager::GetAudVol()
{
	return m_dwAudSetVoluem;
}

/*------------------------------------------------------------------------------
 * 描  述：设置本地全局摄像头帧率
 * 参  数：[in] dwFrameRate 帧率
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::SetFrameRate(DWORD dwFrameRate)
{
	if (dwFrameRate != m_dwFrameRate)
	{
		m_dwFrameRate = dwFrameRate;
		m_pDuiFrameWnd->PostMessageW(DUILIB_MSG_VIDEO_PARAM_CHANGED, 0, 0);
	}
}

void CSdkManager::SetScreenShareConfig(const ScreenShareConfig& config)
{
	m_screenShareConfig = config;
}

ScreenShareConfig CSdkManager::GetScreenShareConfig() const
{
	return m_screenShareConfig;
}

/*------------------------------------------------------------------------------
 * 描  述：获取本地全局摄像头帧率
 * 参  数：无
 * 返回值：帧率
------------------------------------------------------------------------------*/
DWORD CSdkManager::GetFrameRate()
{
	return m_dwFrameRate;
}

/*------------------------------------------------------------------------------
 * 描  述：设置本地全局摄像头分辨率
 * 参  数：[in] dwResolutionIndex 分辨率索引，具体分辨率见VideoResolution
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::SetResolution(DWORD dwResolutionIndex)
{
	if (dwResolutionIndex != m_dwResolutionIndex)
	{
		m_dwResolutionIndex = dwResolutionIndex;
		m_pDuiFrameWnd->PostMessageW(DUILIB_MSG_VIDEO_PARAM_CHANGED, 0, 0);
	}
}

/*------------------------------------------------------------------------------
 * 描  述：获取本地全局摄像头分辨率索引，具体分辨率见VideoResolution
 * 参  数：无
 * 返回值：音量
------------------------------------------------------------------------------*/
DWORD CSdkManager::GetResolution()
{
	return m_dwResolutionIndex;
}

/*------------------------------------------------------------------------------
 * 描  述：设备变更事件，暂未处理
 * 参  数：[in] device_event 事件类型
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::OnDeviceChange(DeviceEventType device_event)
{
	m_pDuiFrameWnd->PostMessageW(DUILIB_MSG_DEVICECHANGE, 0, 0);
}

/*------------------------------------------------------------------------------
 * 描  述：SDK事件回调处理
 * 参  数：[in] event_type	事件类型
 *         [in] err_code	错误码
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::OnEvent(EventType event_type, ErrCode err_code)
{
	PostMessage(DUILIB_MSG_FSP_EVENT, event_type, err_code);
}

LRESULT CSdkManager::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (uMsg == DUILIB_MSG_FSP_EVENT) {
		EventType event_type = (EventType)wParam;
		ErrCode err_code = (ErrCode)lParam;

		OnFspEvent(event_type, err_code);

		bHandled = TRUE;
	}

	return S_OK;
}

void CSdkManager::OnFspEvent(fsp::EventType eventType, fsp::ErrCode result)
{
	if (eventType == fsp::EVENT_JOINGROUP_RESULT) {
		assert(m_pDuiLoginWnd);

		// 隐藏登录等待窗口
		m_pDuiLoginWaitWnd->ShowWindow(false);

		if (result == ErrCode::ERR_OK) // 登录成功，显示主窗口
		{
			m_pDuiLoginWnd->ShowWindow(false);

			m_pDuiFrameWnd->CenterWindow();
			m_pDuiFrameWnd->ShowWindow(true);
		}
		else // 登录失败，跳转到失败页面
		{
			CDuiString strErrInfo = L"未知错误:";
			if (result == ErrCode::ERR_TOKEN_INVALID)
				strErrInfo = L"认证失败！";
			else if (result == ErrCode::ERR_CONNECT_FAIL)
				strErrInfo = L"连接服务器失败！";
			else if (result == ErrCode::ERR_APP_NOT_EXIST)
				strErrInfo = L"应用不存在！";
			else if (result == ErrCode::ERR_USERID_CONFLICT)
				strErrInfo = L"用户已登录！";
			else if (result == ErrCode::ERR_NO_BALANCE)
				strErrInfo = L"账户余额不足！";
			else
			{
				WCHAR szTmp[8];
				_snwprintf_s(szTmp, 8, L"%d", result);
				strErrInfo.Append(szTmp);
			}

			ShowErrorWnd(strErrInfo);
		}
	}
	else if (eventType == fsp::EVENT_CONNECT_LOST){
		m_pDuiFrameWnd->ResetWindowStatus();
		//连接断开，显示失败窗口
		ShowErrorWnd(L"连接断开");
	}
}

/*------------------------------------------------------------------------------
 * 描  述：SDK回调远端视频相关事件，应该发消息到UI线程处理
 * 参  数：[in] user_id 用户ID
 *         [in] video_id 视频ID
 *         [in] remote_video_event 事件类型
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::OnRemoteVideoEvent(const String& user_id, const String& video_id, RemoteVideoEventType remote_video_event)
{
	RemoteVideoInfo* pInfo = new RemoteVideoInfo;
	pInfo->strUserId = user_id;
	pInfo->strVideoId = video_id;

	if (REMOTE_VIDEO_EVENT_PUBLISHE_STARTED == remote_video_event)
	{
		m_pDuiFrameWnd->PostMessageW(DUILIB_MSG_ADD_REMOTE_VIDEO, (WPARAM)pInfo, 0);
	}
	else if (REMOTE_VIDEO_EVENT_PUBLISHE_STOPED == remote_video_event)
	{
		m_pDuiFrameWnd->PostMessageW(DUILIB_MSG_DEL_REMOTE_VIDEO, (WPARAM)pInfo, 0);
	}
}

/*------------------------------------------------------------------------------
 * 描  述：SDK回调远端音频相关事件，应该发消息到UI线程处理
 * 参  数：[in] user_id 用户ID
 *         [in] remote_video_event 事件类型
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::OnRemoteAudioEvent(const String& user_id, RemoteAudioEventType remote_audio_event)
{
	if (m_pDuiFrameWnd && IsWindow(m_pDuiFrameWnd->GetHWND())) {
		RemoteAudioInfo* pInfo = new RemoteAudioInfo;
		pInfo->strUserId = user_id;

		if (REMOTE_AUDIO_EVENT_PUBLISHE_STARTED == remote_audio_event)
		{
			m_pDuiFrameWnd->PostMessageW(DUILIB_MSG_ADD_REMOTE_AUDIO, (WPARAM)pInfo, 0);
		}
		else if (REMTOE_AUDIO_EVENT_PUBLISHE_STOPED == remote_audio_event)
		{
			m_pDuiFrameWnd->PostMessageW(DUILIB_MSG_DEL_REMOTE_AUDIO, (WPARAM)pInfo, 0);
		}
	}
}

/*------------------------------------------------------------------------------
 * 描  述：远程控制事件
 * 参  数：[in] user_id  	    用户ID
 *         [in] operation_type	类型
 * 返回值：Token
------------------------------------------------------------------------------*/
void CSdkManager::OnRemoteControlOperationEvent(const String& user_id,
	fsp::RemoteControlOperationType operation_type)
{
	RemoteControlInfo* pInfo = new RemoteControlInfo;
	pInfo->strUserId = user_id;
	pInfo->operationType = operation_type;
	m_pDuiFrameWnd->PostMessage(DUILIB_MSG_REMOTECONTROL_EVENT, (WPARAM)pInfo, 0);
}

/*------------------------------------------------------------------------------
 * 描  述：生成Token，考虑到安全性，建议将Token生成代码集成到服务器
 * 参  数：[in] szGroupId	分组ID
 *         [in] szUserId	用户ID
 * 返回值：Token
------------------------------------------------------------------------------*/
std::string CSdkManager::BuildToken(char* szGroupId, char* szUserId)
{
	demo::ClientConfig& config = demo::CConfigParser::GetInstance().GetClientConfig();
	
	fsp::tools::AccessToken token(config.bUserDefine ? config.strUserAppSecret : config.strAppSecret);
	token.app_id		= config.bUserDefine ? config.strUserAppId : config.strAppId;
	token.group_id		= szGroupId;
	token.user_id		= szUserId;
	token.expire_time	= 0;

	return token.Build();
}

/*------------------------------------------------------------------------------
 * 描  述：加入分组处理
 * 参  数：[in] szGroup	分组ID
 *         [in] szUser	用户ID
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::JoinGroup(LPCTSTR szGroup, LPCTSTR szUser)
{
	// 保存起来，主窗口标题栏需要显示此信息
	m_strGroup	= szGroup;
	m_strUser	= szUser;

	char szGroupId[32];
	char szUserId[32];
	demo::ConvertUnicodeToUtf8(szGroup, szGroupId, 32);
	demo::ConvertUnicodeToUtf8(szUser, szUserId, 32);

	// 生成Token
	fsp::String strToken = BuildToken(szGroupId, szUserId).c_str();

	// 加入分组
	if (fsp::ERR_OK != m_pFspEngin->JoinGroup(strToken, szGroupId, szUserId))
		return;

	// 隐藏登录窗口
	m_pDuiLoginWnd->ShowWindow(false);

	// 显示登录等待窗口
	m_pDuiLoginWaitWnd->CenterWindow();
	m_pDuiLoginWaitWnd->ShowWindow(true);
}

/*------------------------------------------------------------------------------
 * 描  述：设置分组ID
 * 参  数：[in] szGroup
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::SetLoginGroup(LPCTSTR szGroup)
{
	m_strGroup = szGroup;
}

/*------------------------------------------------------------------------------
 * 描  述：获取分组ID
 * 参  数：无
 * 返回值：分组ID
------------------------------------------------------------------------------*/
const CDuiString& CSdkManager::GetLoginGroup()
{
	return m_strGroup;
}

/*------------------------------------------------------------------------------
 * 描  述：设置用户ID
 * 参  数：[in] szUser 用户ID
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::SetLoginUser(LPCTSTR szUser)
{
	m_strUser = szUser; 
}

/*------------------------------------------------------------------------------
 * 描  述：获取用户ID
 * 参  数：无
 * 返回值：用户ID
------------------------------------------------------------------------------*/
const CDuiString& CSdkManager::GetLoginUser()
{
	return m_strUser;
}

void CSdkManager::ShowErrorWnd(const CDuiString& strErrInfo)
{
	//先隐藏主窗口
	if (m_pDuiFrameWnd) {
		m_pDuiFrameWnd->ShowWindow(false);
	}

	m_pDuiLoginErrorWnd->UpdateErrInfo(strErrInfo);
	m_pDuiLoginErrorWnd->CenterWindow();
	m_pDuiLoginErrorWnd->ShowWindow(true);
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