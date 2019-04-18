/*##############################################################################
 * 文件：DuiLoginWaitWnd.h
 * 描述：登录等待窗口类的声明与定义
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#pragma once

#include "UIlib.h"

using namespace DuiLib;

class CSdkManager;

class CDuiLoginWaitWnd : public WindowImplBase
{
public:
	CDuiLoginWaitWnd();
	~CDuiLoginWaitWnd();

private:
	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;

	virtual void InitWindow() override;
	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled) override;
};

