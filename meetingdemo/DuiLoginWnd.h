/*##############################################################################
 * 文件：DuiLoginWnd.h
 * 描述：登录窗口类的声明与定义
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#pragma once

#include "UIlib.h"
#include "Resource.h"

using namespace DuiLib;

class CSdkManager;

class CDuiLoginWnd : public WindowImplBase
{
public:
	CDuiLoginWnd();
	~CDuiLoginWnd();

	DUI_DECLARE_MESSAGE_MAP()

private:
	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;
	virtual void InitWindow() override;
	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled) override;

	void OnClick(TNotifyUI& msg);
};

