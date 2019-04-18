/*##############################################################################
 * 文件：DuiFrameWnd.h
 * 描述：主窗口声明与定义
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#pragma once

#include <set>
#include "UIlib.h"
#include "VideoWndMgr.h"

using namespace DuiLib;

class CSdkManager;
class CDuiSettingWnd;

class CDuiFrameWnd : public WindowImplBase 
{
public:
	CDuiFrameWnd();
	~CDuiFrameWnd();

	void ResetWindowStatus();

	DUI_DECLARE_MESSAGE_MAP()

private:
	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;
	virtual void InitWindow() override;
	void OnClick(TNotifyUI& msg);
	void OnSelectChanged(TNotifyUI& msg);

	void OnClickMicBtn(TNotifyUI& msg);
	void OnClickCamBtn(TNotifyUI& msg);
	void OnClickScreenShareBtn(TNotifyUI& msg);
	void OnClickSettingBtn(TNotifyUI& msg);

	bool IsCamOpened(DWORD dwCamIndex);

	void OnCamSelectChanged(TNotifyUI& msg);
	
	void OnAddRemoteAudio(WPARAM wParam, LPARAM lParam);
	void OnDelRemoteAudio(WPARAM wParam, LPARAM lParam);
	void OnAddRemoteVideo(WPARAM wParam, LPARAM lParam);
	void OnDelRemoteVideo(WPARAM wParam, LPARAM lParam);

	void OnBroadcastCamChanged(WPARAM wParam, LPARAM lParam);

	RECT GetDisplayRect();
	void AdjustTitleGroupUser();

	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled) override;

	void OnTimer();

	void SetToolbarCamBtnStatus(bool isOpen);
	void SetToolbarMicBtnStatus(bool isOpen);
	void SetToolbarScreenshareBtnStatus(bool isOpen);

private:
	// 6个视频窗口管理器
	CVideoWndMgr m_VideoWndMgr;

	CDuiSettingWnd* m_pSettingWnd = nullptr;

	bool m_bVideoWndInitFlag;

	// 同时可以开启2个摄像头
	std::set<DWORD> m_setCamOpenIndexes;

	bool m_bBroadcastMic;
	bool m_isScreenSharing;

	std::vector<RemoteAudioInfo> m_vecRemoteAudioInfo;
	std::vector<RemoteVideoInfo> m_vecRemoteVideoInfo;
};

