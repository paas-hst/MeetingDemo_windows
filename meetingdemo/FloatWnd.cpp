/*##############################################################################
 * 文件：FloatWnd.cpp
 * 描述：浮动窗口实现，抽象了一层窗口实现，减少冗余代码
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#include "stdafx.h"
#include "FloatWnd.h"
#include "UIMenu.h"

/*------------------------------------------------------------------------------
 * 描  述：构造函数
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
CFloatWnd::CFloatWnd(LPCTSTR szLayoutXml, LPCTSTR szWndClassName)
	: m_szLayoutXml(szLayoutXml)
	, m_szWndClassName(szWndClassName)
	, m_dwThreadId(0)
{
}

/*------------------------------------------------------------------------------
 * 描  述：析构函数
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
CFloatWnd::~CFloatWnd()
{
	if (m_dwThreadId != 0)
		::PostThreadMessage(m_dwThreadId, WM_QUIT, 0, 0);
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，DUI调用获取窗口xml布局文件目录
 * 参  数：无
 * 返回值：xml布局文件目录
 ------------------------------------------------------------------------------*/
CDuiString CFloatWnd::GetSkinFolder()
{
	return CDuiString(L"skin");
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，DUI调用获取窗口xml布局文件名称
 * 参  数：无
 * 返回值：xml布局文件名称
 ------------------------------------------------------------------------------*/
CDuiString CFloatWnd::GetSkinFile()
{
	return m_szLayoutXml;
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，DUI调用获取窗口类名称
 * 参  数：无
 * 返回值：窗口类名称
 ------------------------------------------------------------------------------*/
LPCTSTR CFloatWnd::GetWindowClassName(void) const
{
	return m_szWndClassName;
}

/*------------------------------------------------------------------------------
 * 描  述：窗口拥有自己的线程
 * 参  数：[in] lpParam 自定义参数
 * 返回值：DWORD
 ------------------------------------------------------------------------------*/
DWORD CFloatWnd::WndThreadProc(LPVOID lpParam)
{
	CFloatWnd* pThis = (CFloatWnd*)lpParam;

	if (pThis->m_bShowAfterCreate)
		::ShowWindow(pThis->m_hWnd, SW_SHOW);
	else
		::ShowWindow(pThis->m_hWnd, SW_HIDE);

	pThis->m_PaintManager.MessageLoop();

	return 0;
}

/*------------------------------------------------------------------------------
 * 描  述：创建窗口
 * 参  数：[in] hParentWnd	父窗口
 *         [in] szWndName	窗口名称
 *         [in] dwWndStyle	窗口风格
 *         [in] bShowAfterCreate 创建后是否立即显示
 * 返回值：成功/失败
 ------------------------------------------------------------------------------*/
bool CFloatWnd::CreateWnd(HWND hParentWnd, LPCTSTR szWndName, DWORD dwWndStyle, bool bShowAfterCreate)
{
	m_hParentWnd = hParentWnd;
	m_bShowAfterCreate = bShowAfterCreate;

	WindowImplBase::Create(hParentWnd, szWndName, dwWndStyle, WS_EX_WINDOWEDGE);

	if (NULL == ::CreateThread(NULL, 0, WndThreadProc, this, 0, &m_dwThreadId))
		return false;

	return true;
}

/*------------------------------------------------------------------------------
 * 描  述：设置窗口矩形区域
 * 参  数：[in] rectWnd 窗口矩形区域
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CFloatWnd::SetWndRect(const RECT& rectWnd)
{
	::MoveWindow(m_hWnd, 
		rectWnd.left,
		rectWnd.top,
		rectWnd.right - rectWnd.left,
		rectWnd.bottom - rectWnd.top,
		TRUE);

	::ShowWindow(m_hWnd, SW_SHOW);
}

