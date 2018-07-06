/*##############################################################################
 * 文件：DisplayVideoWnd.h
 * 描述：视频小窗口声明和定义
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#pragma once

#include "FloatWnd.h"
#include "fsp_engine.h"
#include "UIMenu.h"
#include "define.h"

class CSdkManager;

class CDisplayVideoWnd : public INotifyUI, public IEventCallback
{
public:
	CDisplayVideoWnd();
	~CDisplayVideoWnd();

	void Init(HWND hParentWnd);
	void SetWndRect(const RECT& rect, bool bShow);

	bool IsIdle();
	bool IsVideoMaximized();

	bool IsHoldRemoteVideo(const fsp::String& strUserId, const fsp::String& strVideoId);
	bool IsHoldRemoteVideo(const fsp::String& strUserId);
	bool IsHoldRemoteAudio(const fsp::String& strUserId);

	bool IsHoldLocalVideo(DWORD dwCamIndex);
	bool IsHoldLocalVideo();
	bool IsHoldLocalAudio(DWORD dwMicIndex);
	bool IsHoldLocalAudio();

	bool StartPublishCam(DWORD dwCamIndex);
	bool StopPublishCam(DWORD dwCamIndex);

	void StartPublishMic(DWORD dwMicIndex);
	bool StopPublishMic(DWORD dwMicIndex);

	void AddRemoteVideo(const fsp::String& strUserId, const fsp::String& strVideoId);
	void DelRemoteVideo(const fsp::String& strUserId, const fsp::String& strVideoId);

	void AddRemoteAudio(const fsp::String& strUserId);
	void DelRemoteAudio(const fsp::String& strUserId);

	void OnTimer();
	void OnVideoParamChanged();

	void SetEventCallback(IEventCallback* pCallback);
	void ShowWindow(bool bShow);

private:
	virtual void Notify(TNotifyUI& msg) override;
	virtual void OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	HWND GetVideoWnd() { return m_pVideoWnd->GetHWND(); }
	void OpenMic();
	void CloseMic();
	void OnMicEnergyChange(DWORD dwEnergy);
	void OnVideoStatsChange(LPCTSTR szVideoStats);
	void ConstructMenu(CMenuWnd* pMenu);
	CMenuElementUI* ConstructMenuItem(LPCTSTR szName, 
		LPCTSTR szText, 
		bool bRadio, 
		bool bSelected, 
		bool bHasSubMenu);
	void SetVideoRenderMode(fsp::RenderMode mode);
	void ChangeCamIndex(DWORD dwCamIndex);
	void RefreshUserName();
	void SetVideoParam();

private:
	CFloatWnd* m_pVideoWnd;
	CFloatWnd* m_pInfoWnd;

	fsp::RenderMode m_eRenderMode;

	HWND m_hParentWnd;

	bool m_bHasAudio;
	bool m_bHasVideo;

	bool m_bIsLocal;

	DWORD m_dwCamIndex;
	DWORD m_dwMicIndex;

	fsp::String m_strUserId;
	fsp::String m_strVideoId;

	bool m_bIsVideoMaximized;
	RECT m_rectDisplay;

	IEventCallback* m_pEventCallback;
};

