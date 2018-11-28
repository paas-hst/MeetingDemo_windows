/*##############################################################################
 * 文件：DuiAppConfigWnd.h
 * 描述：应用配置窗口类的声明与定义
 * 作者：Teck
 * 时间：2018年8月2日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#pragma once

#include "UIlib.h"
#include "FloatWnd.h"

using namespace DuiLib;
class CDuiAppConfigWnd : public WindowImplBase
{
public:
	CDuiAppConfigWnd();
	~CDuiAppConfigWnd();

	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;

	DUI_DECLARE_MESSAGE_MAP()

private:
	void OnClick(TNotifyUI& msg);
	virtual void InitWindow() override;

	void SetUseDefaultConfig();
	void SetUseUserDefineConfig();
	void RestartApp();

private:
	bool m_bUseDefault;
};

