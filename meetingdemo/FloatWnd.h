/*##############################################################################
 * 文件：FloatWnd.h
 * 描述：浮动窗口类的声明与定义
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#pragma once

#include "UIlib.h"

using namespace DuiLib;

class CFloatWnd : public WindowImplBase
{
public:
	CFloatWnd(LPCTSTR szLayoutXml, LPCTSTR szWndClassName);
	virtual ~CFloatWnd();

	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;

	bool CreateWnd(HWND hParentWnd, LPCTSTR szWndName, DWORD dwWndStyle, bool bShowAfterCreate);

	virtual void SetWndRect(const RECT& rectWnd);

	CPaintManagerUI& GetPaintManager() { return m_PaintManager; }

private:
	static DWORD WINAPI WndThreadProc(LPVOID lpParam);


protected:
	CDuiString	m_szLayoutXml;
	CDuiString	m_szWndClassName;
	HWND		m_hParentWnd;
	DWORD		m_dwThreadId;
	bool		m_bShowAfterCreate;
};

