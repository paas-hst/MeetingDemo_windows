/*##############################################################################
 * 文件：SdkManager.h
 * 描述：SDK管理器的声明与定义
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#pragma once

#include "fsp_engine.h"
#include "DuiLoginWnd.h"
#include "DuiLoginWaitWnd.h"
#include "DuiLoginErrorWnd.h"
#include "DuiFrameWnd.h"

using namespace fsp;

class CSdkManager : public IFspEngineEventHandler
{
public:
	static CSdkManager& GetInstance();

	bool Init();
	void Start();
	void SetRestart();

	void OnLoginResult(fsp::ErrCode result);
	void JoinGroup(LPCTSTR szGroup, LPCTSTR szUser);

	void SetOpenMic(DWORD dwMicIndex);
	DWORD GetOpenMic();

	void SetOpenAud(DWORD dwAudIndex);
	DWORD GetOpenAud();

	void SetMicVol(DWORD dwMicVol);
	DWORD GetMicVol();

	void SetAudVol(DWORD dwAudVol);
	DWORD GetAudVol();

	void SetLoginGroup(LPCTSTR szGroup);
	const CDuiString& GetLoginGroup();

	void SetLoginUser(LPCTSTR szUser);
	const CDuiString& GetLoginUser();

	void SetResolution(DWORD dwResolutionIndex);
	DWORD GetResolution();

	void SetFrameRate(DWORD dwFrameRate);
	DWORD GetFrameRate();

	IFspEngine* GetFspEngin() { return m_pFspEngin; }

	CDuiFrameWnd* GetFrameWnd() { return m_pDuiFrameWnd; }

private:
	CSdkManager();
	~CSdkManager();

	virtual void OnDeviceChange(DeviceEventType device_event) override;
	virtual void OnEvent(EventType event_type, ErrCode err_code) override;
	virtual void OnRemoteVideoEvent(const String& user_id, 
		const String& video_id,
		RemoteVideoEventType remote_video_event) override;
	virtual void OnRemoteAudioEvent(const String& user_id, 
		RemoteAudioEventType remote_audio_event) override;

	std::string BuildToken(char* szGroupId, char* szUserId);

private:
	CDuiLoginWnd*		m_pDuiLoginWnd;
	CDuiLoginWaitWnd*	m_pDuiLoginWaitWnd;
	CDuiLoginErrorWnd*	m_pDuiLoginErrorWnd;
	CDuiFrameWnd*		m_pDuiFrameWnd;

	IFspEngine*			m_pFspEngin;
	FspEngineContext	m_FspEnginContext;

	CDuiString m_strGroup;
	CDuiString m_strUser;

	DWORD m_dwMicSetVolume;	// 麦克风设置音量值
	DWORD m_dwMicOpenIndex;	// 选择使用的麦克风

	DWORD m_dwAudSetVoluem; // 扬声器设置音量值
	DWORD m_dwAudOpenIndex; // 选择使用的扬声器

	DWORD m_dwResolutionIndex;	// 分辨率
	DWORD m_dwFrameRate;		// 帧率

	// 确保主窗口创建后再处理SDK回调消息，否则会崩溃
	bool	m_bMainFrameCreated;
	HANDLE	m_bSemaphore;

	bool m_bRestart;
};

