/*##############################################################################
 * 文件：MediaInfoWnd.h
 * 描述：媒体信息显示窗口声明与定义
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#pragma once
#include "FloatWnd.h"

class CMediaInfoWnd : public CFloatWnd
{
public:
	CMediaInfoWnd(LPCTSTR szLayoutXml, LPCTSTR szWndClassName);
	virtual ~CMediaInfoWnd();

private:
	virtual void SetWndRect(const RECT& rectWnd) override;
};

