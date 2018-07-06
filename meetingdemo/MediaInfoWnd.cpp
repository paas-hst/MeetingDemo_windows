/*##############################################################################
 * 文件：MediaInfoWnd.cpp
 * 描述：媒体信息显示窗口实现
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#include "stdafx.h"
#include "MediaInfoWnd.h"

/*------------------------------------------------------------------------------
 * 描  述：构造函数
 * 参  数：无
 * 返回值：无
 ------------------------------------------------------------------------------*/
CMediaInfoWnd::CMediaInfoWnd(LPCTSTR szLayoutXml, LPCTSTR szWndClassName)
	: CFloatWnd(szLayoutXml, szWndClassName)
{
}

/*------------------------------------------------------------------------------
 * 描  述：析构函数
 * 参  数：无
 * 返回值：无
 ------------------------------------------------------------------------------*/
CMediaInfoWnd::~CMediaInfoWnd()
{
}

/*------------------------------------------------------------------------------
 * 描  述：设置窗口显示的矩形区域
 * 参  数：[in] rectWnd 矩形区域
 * 返回值：无
------------------------------------------------------------------------------*/
void CMediaInfoWnd::SetWndRect(const RECT& rectWnd)
{
	DWORD dwWidth = rectWnd.right - rectWnd.left;

	RECT rectPadding;
	ZeroMemory(&rectPadding, sizeof(rectPadding));

	// normal size的宽度是400
	if (dwWidth > 400) // 最大化
	{
		// 76(用户名) + 110(音量大小) + 120(视频信息) + 30(菜单按钮)		
		rectPadding.left = (dwWidth - 336) / 3;
	}
	else // 恢复初始大小
	{
		rectPadding.left = 10;
	}

	// 重新设置元素之间的间距
	CPaintManagerUI& paintMgr = GetPaintManager();
	((CHorizontalLayoutUI*)paintMgr.FindControl(L"audio_info"))->SetPadding(rectPadding);
	((CHorizontalLayoutUI*)paintMgr.FindControl(L"video_info"))->SetPadding(rectPadding);

	__super::SetWndRect(rectWnd);
}