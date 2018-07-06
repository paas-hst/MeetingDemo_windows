/*##############################################################################
 * 文件：VideoWnd.cpp
 * 描述：视频窗口实现
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#include "stdafx.h"
#include "VideoWnd.h"
#include "util.h"


/*------------------------------------------------------------------------------
 * 描  述：构造函数
 * 参  数：无
 * 返回值：无
 ------------------------------------------------------------------------------*/
CVideoWnd::CVideoWnd(LPCTSTR szLayoutXml, LPCTSTR szWndClassName)
	: CFloatWnd(szLayoutXml, szWndClassName)
	, m_pEventCallback(nullptr)
{
}

/*------------------------------------------------------------------------------
 * 描  述：析构函数
 * 参  数：无
 * 返回值：无
 ------------------------------------------------------------------------------*/
CVideoWnd::~CVideoWnd()
{
}

/*------------------------------------------------------------------------------
 * 描  述：设置窗口显示的矩形区域
 * 参  数：[in] rectWnd 矩形区域
 * 返回值：无
------------------------------------------------------------------------------*/
void CVideoWnd::SetWndRect(const RECT& rectWnd)
{
	DWORD dwWidth = rectWnd.right - rectWnd.left;
	DWORD dwHeight = rectWnd.bottom - rectWnd.top;

	RECT rectPadding;
	rectPadding.left = (dwWidth - 38) / 2;
	rectPadding.right = 0;
	rectPadding.bottom = 0;
	rectPadding.top = (dwHeight - 38) / 2;

	CPaintManagerUI& paintMgr = GetPaintManager();	
	((CHorizontalLayoutUI*)paintMgr.FindControl(L"video_bk"))->SetPadding(rectPadding);

	__super::SetWndRect(rectWnd);
}

/*------------------------------------------------------------------------------
 * 描  述：SDK通知应用层有用户广播了视频的处理
 * 参  数：[in] uMsg		消息类型
 *         [in] wParam		自定义
 *         [in] lParam		自定义
 *         [out] bHandled	消息是否已经被处理
 * 返回值：处理结果
 ------------------------------------------------------------------------------*/
LRESULT CVideoWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (uMsg == WM_LBUTTONDBLCLK && m_pEventCallback)
		m_pEventCallback->OnEvent(uMsg, wParam, lParam);

	return S_OK;
}

/*------------------------------------------------------------------------------
 * 描  述：设置事件处理回调对象
 * 参  数：[in] pCallback 回调对象
 * 返回值：无
------------------------------------------------------------------------------*/
void CVideoWnd::SetEventCallback(IEventCallback* pCallback)
{
	m_pEventCallback = pCallback;
}