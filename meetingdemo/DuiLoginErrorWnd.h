/*##############################################################################
 * 文件：DuiLoginErrorWnd.h
 * 描述：登录失败窗口类的声明与定义
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#pragma once

#include "UIlib.h"
#include "fsp_engine.h"

using namespace DuiLib;

class CSdkManager;

class CDuiLoginErrorWnd : public WindowImplBase
{
public:
	CDuiLoginErrorWnd();
	~CDuiLoginErrorWnd();
	
	void UpdateErrInfo(const CDuiString& strErrInfo);

	DUI_DECLARE_MESSAGE_MAP()

private:
	void OnClick(TNotifyUI& msg);
	
	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;
	virtual void InitWindow() override;
	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled) override;

};

