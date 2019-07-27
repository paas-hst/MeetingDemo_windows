/*##############################################################################
 * 文件：DuiSettingWnd.h
 * 描述：设置窗口类的声明与定义
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#pragma once

#include "UIlib.h"
#include "fsp_engine.h"
#include "UIVideoPanel.h"

using namespace DuiLib;

class CSdkManager;

class CSettingWnd : public WindowImplBase
{
public:
	CSettingWnd();
	~CSettingWnd();

	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;
	virtual CControlUI* CreateControl(LPCTSTR pstrClass) override;

	void OnDeviceChanged();

	DUI_DECLARE_MESSAGE_MAP()

private:
	void OnClick(TNotifyUI& msg);
	void OnComboItemSelected(TNotifyUI& msg);
	void OnValueChanged(TNotifyUI& msg);

	void OnSelectMic(TNotifyUI& msg);
	void OnSelectAud(TNotifyUI& msg);
	void OnSelectCam(TNotifyUI& msg);
	void OnSelectRes(TNotifyUI& msg);
	void OnSelectedChange(TNotifyUI& msg);

	void UpdateAudioTab();

	void UpdateDeviceList();

	virtual void InitWindow() override;
	virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) override;
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
private:
	CWndShadow m_wndShadow;
	//CFloatWnd* m_pVideoWnd;
	CTabLayoutUI* m_pTabLayout = nullptr;
	CVideoPanelUI* m_pVideoPanel;

	DWORD m_dwCurRenderCamId;
};

