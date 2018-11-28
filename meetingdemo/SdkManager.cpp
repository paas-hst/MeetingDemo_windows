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
	, m_bMainFrameCreated(false)
	, m_pDuiLoginWnd(nullptr)
	, m_pDuiLoginWaitWnd(nullptr)
	, m_pDuiLoginErrorWnd(nullptr)
	, m_pDuiFrameWnd(nullptr)
	, m_bRestart(false)
{
	m_bSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
}

/*------------------------------------------------------------------------------
 * 描  述：析构函数
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
CSdkManager::~CSdkManager()
{
	FspReleaseEngine();
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
	m_FspEnginContext.data_handler = this;
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

/*------------------------------------------------------------------------------
 * 描  述：启动
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::Start()
{
	m_pDuiLoginWnd = new CDuiLoginWnd;
	m_pDuiLoginWnd->Create(NULL, _T("CDuiLoginWnd"), UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
	m_pDuiLoginWnd->CenterWindow();
	m_pDuiLoginWnd->ShowModal();

	if (m_bRestart)
	{
		m_bRestart = false;
		Start();
	}
}

void CSdkManager::SetRestart()
{
	m_bRestart = true;
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
	//MessageBox(NULL, L"OnDeviceChange", L"Info", 0);
}

/*------------------------------------------------------------------------------
 * 描  述：SDK事件回调处理
 * 参  数：[in] event_type	事件类型
 *         [in] err_code	错误码
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::OnEvent(EventType event_type, ErrCode err_code)
{
	if (event_type == EVENT_JOINGROUP_RESULT)
	{
		PostMessage(m_pDuiLoginWaitWnd->GetHWND(), DUILIB_MSG_LOGIN_RESULT, err_code, 0);
	}
}

/*------------------------------------------------------------------------------
 * 描  述：登录结果处理，从LoginWaitWnd线程回调过来
 * 参  数：[in] result 登录结果
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::OnLoginResult(fsp::ErrCode result)
{
	assert(m_pDuiLoginWnd);
	
	// 隐藏登录等待窗口
	m_pDuiLoginWaitWnd->ShowWindow(false);

	if (result == ErrCode::ERR_OK) // 登录成功，显示主窗口
	{
		if (!m_bMainFrameCreated)
		{
			m_pDuiFrameWnd = new CDuiFrameWnd;
			m_pDuiFrameWnd->Create(NULL, _T("CDuiFrameWnd"), UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
			m_pDuiFrameWnd->CenterWindow();

			m_bMainFrameCreated = true;
			ReleaseSemaphore(m_bSemaphore, 1, 0);

			m_pDuiFrameWnd->ShowModal();
		}
	}
	else // 登录失败，跳转到失败页面
	{		
		m_pDuiLoginErrorWnd = new CDuiLoginErrorWnd(result);
		m_pDuiLoginErrorWnd->Create(NULL, _T("CDuiLoginErrorWnd"), UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
		m_pDuiLoginErrorWnd->CenterWindow();
		m_pDuiLoginErrorWnd->ShowModal();
	}

	m_pDuiLoginWaitWnd->Close();
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
	if (!m_bMainFrameCreated)
		WaitForSingleObject(m_bSemaphore, INFINITE);

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
	if (!m_bMainFrameCreated)
		WaitForSingleObject(m_bSemaphore, INFINITE);

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

/*------------------------------------------------------------------------------
 * 描  述：生成Token
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
	m_pDuiLoginWaitWnd = new CDuiLoginWaitWnd();
	m_pDuiLoginWaitWnd->Create(NULL, _T("CDuiLoginWaitWnd"), UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
	m_pDuiLoginWaitWnd->CenterWindow();
	m_pDuiLoginWaitWnd->ShowModal();

	m_pDuiLoginWnd->Close();
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

/*------------------------------------------------------------------------------
 * 描  述：本地音频数据回调处理，此处直接写文件
 * 参  数：[in] data 数据
 *         [in] data_len 数据长度
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::OnLocalAudioStreamRawData(const char* data, int data_len)
{
	string strRecorFile = "LocalAudio.data";
	std::ofstream ofs(strRecorFile, std::ios_base::binary | std::ios_base::app);
	ofs.write(data, data_len);
}

/*------------------------------------------------------------------------------
 * 描  述：远端音频数据回调处理，此处直接写文件
 * 参  数：[in] user_id 远端用户标识
 *         [in] data 数据
 *         [in] data_len 数据长度
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::OnRemoteAudioStreamRawData(const String& user_id, const char* data, int data_len)
{
	string strRecorFile = "RemoteAudio_" + string(user_id.c_str()) + ".data";
	std::ofstream ofs(strRecorFile, std::ios_base::binary | std::ios_base::app);
	ofs.write(data, data_len);
}

/*------------------------------------------------------------------------------
 * 描  述：本地麦克风/远端用户音频数据回调处理，此处直接写文件
 * 参  数：[in] data 数据
 *         [in] data_len 数据长度
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::OnMixAudioStreamRawData(const char* data, int data_len)
{
	MessageBox(NULL, L"OnAudioMixedStreamRawData", L"Callback", 0);
}

/*------------------------------------------------------------------------------
 * 描  述：本地视频数据回调处理，此处直接写文件
 * 参  数：[in] camera_id 本地摄像头索引
 *         [in] header 视频头信息，具体见BitmapInfoHeader的注释
 *         [in] data 数据
 *         [in] data_len 数据长度
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::OnLocalVideoStreamRawData(int camera_id, BitmapInfoHeader* header, const char* data, int data_len)
{
	std::ostringstream convert;
	convert << camera_id;
	string strRecorFile = "LocalVideo" + convert.str() + ".data";
	std::ofstream ofs(strRecorFile, std::ios_base::binary | std::ios_base::app);
	ofs.write(data, data_len);
}

/*------------------------------------------------------------------------------
 * 描  述：远端视频数据回调处理，此处直接写文件
 * 参  数：[in] user_id 远端用户标识
 *         [in] video_id 远端视频标识
 *         [in] header 视频头信息，具体见BitmapInfoHeader的注释
 *         [in] data 数据
 *         [in] data_len 数据长度
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::OnRemoteVideoStreamRawData(const String& user_id, const String& video_id, BitmapInfoHeader* header, const char* data, int data_len)
{
	string strRecorFile = "RemoteVideo_" + string(user_id.c_str()) + "_" + string(video_id.c_str()) +".data";
	std::ofstream ofs(strRecorFile, std::ios_base::binary | std::ios_base::app);
	ofs.write(data, data_len);
}

/*------------------------------------------------------------------------------
 * 描  述：启动本地视频录制
 * 参  数：[in] nDevId 本地摄像头索引
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::StartRecordLocalVideo(int nDevId)
{
	fsp::CallbackDataDesc data_desc;
	data_desc.data_type = CALLBACK_DATA_LOCAL_VIDEO_RAW;
	data_desc.camera_id = nDevId;
	m_pFspEngin->SetCallbackDataState(data_desc, true);
}

/*------------------------------------------------------------------------------
 * 描  述：停止本地视频录制
 * 参  数：[in] nDevId 本地摄像头索引
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::StopRecordLocalVideo(int nDevId)
{
	fsp::CallbackDataDesc data_desc;
	data_desc.data_type = CALLBACK_DATA_LOCAL_VIDEO_RAW;
	data_desc.camera_id = nDevId;
	m_pFspEngin->SetCallbackDataState(data_desc, false);
}

/*------------------------------------------------------------------------------
 * 描  述：启动本地音频录制
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::StartRecordLocalAudio()
{
	fsp::CallbackDataDesc data_desc;
	data_desc.data_type = CALLBACK_DATA_LOCAL_AUDIO_RAW;
	m_pFspEngin->SetCallbackDataState(data_desc, true);
}

/*------------------------------------------------------------------------------
 * 描  述：停止本地音频录制
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::StopRecordLocalAudio()
{
	fsp::CallbackDataDesc data_desc;
	data_desc.data_type = CALLBACK_DATA_LOCAL_AUDIO_RAW;
	m_pFspEngin->SetCallbackDataState(data_desc, false);
}

/*------------------------------------------------------------------------------
 * 描  述：启动远端视频录制
 * 参  数：[in] user_id 远端用户标识
 *         [in] video_id 远端视频标识
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::StartRecordRemoteVideo(const std::string& user_id, const std::string& video_id)
{
	fsp::CallbackDataDesc data_desc;
	data_desc.data_type = CALLBACK_DATA_REMOTE_VIDEO_RAW;
	data_desc.user_id = user_id.c_str();
	data_desc.video_id = video_id.c_str();
	
	m_pFspEngin->SetCallbackDataState(data_desc, true);
}

/*------------------------------------------------------------------------------
 * 描  述：停止远端视频录制
 * 参  数：[in] user_id 远端用户标识
 *         [in] video_id 远端视频标识
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::StopRecordRemoteVideo(const std::string& user_id, const std::string& video_id)
{
	fsp::CallbackDataDesc data_desc;
	data_desc.data_type = CALLBACK_DATA_REMOTE_VIDEO_RAW;
	data_desc.user_id = user_id.c_str();
	data_desc.video_id = video_id.c_str();

	m_pFspEngin->SetCallbackDataState(data_desc, false);
}

/*------------------------------------------------------------------------------
 * 描  述：启动远端音频录制
 * 参  数：[in] user_id 远端用户标识
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::StartRecordRemoteAudio(const std::string& user_id)
{
	fsp::CallbackDataDesc data_desc;
	data_desc.data_type = CALLBACK_DATA_REMOTE_AUDIO_RAW;
	data_desc.user_id = user_id.c_str();

	m_pFspEngin->SetCallbackDataState(data_desc, true);
}

/*------------------------------------------------------------------------------
 * 描  述：停止远端音频录制
 * 参  数：[in] user_id 远端用户标识
 * 返回值：无
------------------------------------------------------------------------------*/
void CSdkManager::StopRecordRemoteAudio(const std::string& user_id)
{
	fsp::CallbackDataDesc data_desc;
	data_desc.data_type = CALLBACK_DATA_REMOTE_AUDIO_RAW;
	data_desc.user_id = user_id.c_str();

	m_pFspEngin->SetCallbackDataState(data_desc, false);
}
