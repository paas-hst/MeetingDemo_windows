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
#include "FloatWnd.h"

using namespace DuiLib;

class CSdkManager;

class CDuiSettingWnd : public WindowImplBase
{
public:
	CDuiSettingWnd();
	~CDuiSettingWnd();

	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;

	DUI_DECLARE_MESSAGE_MAP()

private:
	void OnClick(TNotifyUI& msg);
	void OnComboItemSelected(TNotifyUI& msg);
	void OnValueChanged(TNotifyUI& msg);

	void OnSelectMic(TNotifyUI& msg);
	void OnSelectAud(TNotifyUI& msg);
	void OnSelectCam(TNotifyUI& msg);
	void OnSelectRes(TNotifyUI& msg);
	void OnSetWndFocus(TNotifyUI& msg);

	virtual void InitWindow() override;
	virtual LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled) override;
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
private:
	CFloatWnd* m_pVideoWnd;
	DWORD m_dwCurRenderCamId;
};

