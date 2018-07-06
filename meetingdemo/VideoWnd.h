/*##############################################################################
 * 文件：VideoWnd.h
 * 描述：视频窗口声明与定义
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#pragma once
#include "FloatWnd.h"
#include "define.h"

class CVideoWnd : public CFloatWnd
{
public:
	CVideoWnd(LPCTSTR szLayoutXml, LPCTSTR szWndClassName);
	virtual ~CVideoWnd();

	void SetEventCallback(IEventCallback* pCallback);

private:
	virtual void SetWndRect(const RECT& rectWnd) override;
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

private:
	IEventCallback* m_pEventCallback;
};

