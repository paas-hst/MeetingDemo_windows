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
CDuiLoginErrorWnd::CDuiLoginErrorWnd(fsp::ErrCode result)
	: m_eLoginResult(result)
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
		CSdkManager::GetInstance().SetRestart();
		Close();
	}
	else
	{
		WindowImplBase::OnClick(msg);
	}
}

/*------------------------------------------------------------------------------
 * 描  述：窗口销毁处理
 * 参  数：略
 * 返回值：LRESULT
 ------------------------------------------------------------------------------*/
LRESULT CDuiLoginErrorWnd::OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	::SendMessage(GetHWND(), WM_QUIT, 0, 0);
	return 0;
}

/*------------------------------------------------------------------------------
 * 描  述：窗口销毁前，删除自身
 * 参  数：[in] hWnd 窗口句柄
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiLoginErrorWnd::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

/*------------------------------------------------------------------------------
 * 描  述：主要用来设置应用程序图标
 * 参  数：无
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiLoginErrorWnd::InitWindow()
{
	SetIcon(IDI_FSPCLIENT);

	CDuiString strErrInfo = L"未知错误";
	if (m_eLoginResult == ErrCode::ERR_TOKEN_INVALID)
		strErrInfo = L"认证失败！";
	else if (m_eLoginResult == ErrCode::ERR_CONNECT_FAIL)
		strErrInfo = L"连接服务器失败！";
	else if (m_eLoginResult == ErrCode::ERR_APP_NOT_EXIST)
		strErrInfo = L"应用不存在！";
	else if (m_eLoginResult == ErrCode::ERR_USERID_CONFLICT)
		strErrInfo = L"用户已登录！";
	else if (m_eLoginResult == ErrCode::ERR_NO_BALANCE)
		strErrInfo = L"账户余额不足！";
	else
	{
		WCHAR szTmp[8];
		_snwprintf(szTmp, 8, L"%d", m_eLoginResult);
		strErrInfo.Append(szTmp);
	}
		
	m_PaintManager.FindControl(L"error_info")->SetText(strErrInfo);
}