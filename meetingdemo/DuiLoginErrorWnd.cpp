/*##############################################################################
 * 文件：DuiLoginErrorWnd.cpp
 * 描述：登录失败窗口实现
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#include "stdafx.h"
#include "DuiLoginErrorWnd.h"
#include "SdkManager.h"

// 消息映射
DUI_BEGIN_MESSAGE_MAP(CDuiLoginErrorWnd, WindowImplBase)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

/*------------------------------------------------------------------------------
 * 描  述：构造函数
 * 参  数：[in] result 登录结果
 * 返回值：无
------------------------------------------------------------------------------*/
CDuiLoginErrorWnd::CDuiLoginErrorWnd()
{
}

/*------------------------------------------------------------------------------
 * 描  述：析构函数
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
CDuiLoginErrorWnd::~CDuiLoginErrorWnd()
{
}

/*------------------------------------------------------------------------------
 * 描  述：更新错误信息
 * 参  数：[in] strErrInfo 错误信息
 * 返回值：无
------------------------------------------------------------------------------*/
void CDuiLoginErrorWnd::UpdateErrInfo(const CDuiString& strErrInfo)
{
	m_PaintManager.FindControl(L"error_info")->SetText(strErrInfo);
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，DUI调用获取窗口xml布局文件目录
 * 参  数：无
 * 返回值：xml布局文件目录
 ------------------------------------------------------------------------------*/
CDuiString CDuiLoginErrorWnd::GetSkinFolder()
{
	return CDuiString(L"skin");
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，DUI调用获取窗口xml布局文件名称
 * 参  数：无
 * 返回值：xml布局文件名称
 ------------------------------------------------------------------------------*/
CDuiString CDuiLoginErrorWnd::GetSkinFile()
{
	return CDuiString(L"login_error.xml");
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，DUI调用获取窗口类名称
 * 参  数：无
 * 返回值：窗口类名称
 ------------------------------------------------------------------------------*/
LPCTSTR CDuiLoginErrorWnd::GetWindowClassName(void) const
{
	return L"CDuiLoginErrorWnd";
}

/*------------------------------------------------------------------------------
 * 描  述：DUILIB点击消息处理
 * 参  数：[in] msg 通知消息
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiLoginErrorWnd::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == L"relogin_btn")
	{
		CSdkManager::GetInstance().OpenLoginWnd();
	}
	else
	{
		WindowImplBase::OnClick(msg);
	}
}

/*------------------------------------------------------------------------------
 * 描  述：主要用来设置应用程序图标
 * 参  数：无
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiLoginErrorWnd::InitWindow()
{
	SetIcon(IDI_FSPCLIENT);
}

/*------------------------------------------------------------------------------
 * 描  述：窗口销毁处理
 * 参  数：略
 * 返回值：LRESULT
 ------------------------------------------------------------------------------*/
LRESULT CDuiLoginErrorWnd::OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	PostQuitMessage(0);
	return 0;
}