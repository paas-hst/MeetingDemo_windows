/*##############################################################################
 * 文件：DuiLoginWnd.cpp
 * 描述：登录窗口实现
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#include "stdafx.h"
#include "DuiLoginWnd.h"
#include "DuiLoginWaitWnd.h"
#include "SdkManager.h"
#include "DuiAppConfigWnd.h"
#include "util.h"
#include <atlconv.h>

 // 消息映射
DUI_BEGIN_MESSAGE_MAP(CDuiLoginWnd, WindowImplBase)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

/*------------------------------------------------------------------------------
 * 描  述：构造函数
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
CDuiLoginWnd::CDuiLoginWnd()
{
}

/*------------------------------------------------------------------------------
 * 描  述：析构函数
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
CDuiLoginWnd::~CDuiLoginWnd()
{
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，DUI调用获取窗口xml布局文件目录
 * 参  数：无
 * 返回值：xml布局文件目录
 ------------------------------------------------------------------------------*/
CDuiString CDuiLoginWnd::GetSkinFolder()
{
	return CDuiString(L"skin");
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，DUI调用获取窗口xml布局文件名称
 * 参  数：无
 * 返回值：xml布局文件名称
 ------------------------------------------------------------------------------*/
CDuiString CDuiLoginWnd::GetSkinFile()
{
	return CDuiString(L"login.xml");
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，DUI调用获取窗口类名称
 * 参  数：无
 * 返回值：窗口类名称
 ------------------------------------------------------------------------------*/
LPCTSTR CDuiLoginWnd::GetWindowClassName(void) const
{
	return L"CDuiLoginWnd";
}

/*------------------------------------------------------------------------------
 * 描  述：DUILIB点击消息处理
 * 参  数：[in] msg 通知消息
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiLoginWnd::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == L"login_btn")
	{
		CEditUI* pGroupEdit = (CEditUI*)m_PaintManager.FindControl(L"group_name_edit");
		CEditUI* pUserEdit = (CEditUI*)m_PaintManager.FindControl(L"user_name_edit");

		assert(pGroupEdit && pUserEdit);

		LPCTSTR szGroup = pGroupEdit->GetText();
		LPCTSTR szUser = pUserEdit->GetText();

		if (wcscmp(szGroup, L"") == 0 || wcscmp(szUser, L"") == 0)
		{
			demo::ShowMessageBox(m_hWnd, L"请输入分组ID和用户ID");
		}
		else if (wcslen(szGroup) >= 32 || wcslen(szUser) >= 32)
		{
			demo::ShowMessageBox(m_hWnd, L"分组ID或者用户ID太长了！");
		}
		else
		{
			CSdkManager::GetInstance().JoinGroup(szGroup, szUser);
		}
	}
	else if (msg.pSender->GetName() == L"app_config_btn")
	{
		CDuiAppConfigWnd wndAppConfig;
		wndAppConfig.Create(m_hWnd, _T("CDuiAppConfigWnd"), UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
		wndAppConfig.CenterWindow();
		wndAppConfig.ShowModal();
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
void CDuiLoginWnd::InitWindow()
{
	SetIcon(IDI_FSPCLIENT);

	CLabelUI* pLabel = (CLabelUI*)m_PaintManager.FindControl(L"login_sdkver");

	WCHAR wszVersion[128] = { 0 };
	demo::ConvertUtf8ToUnicode(CSdkManager::GetInstance().GetFspEngin()->GetVersion().c_str(),
		wszVersion, 128);
	CDuiString strVerInfo;
	strVerInfo.Format(L"SdkVersion:  %s", wszVersion);
	pLabel->SetText(strVerInfo);
}

/*------------------------------------------------------------------------------
 * 描  述：窗口销毁处理
 * 参  数：略
 * 返回值：LRESULT
 ------------------------------------------------------------------------------*/
LRESULT CDuiLoginWnd::OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	PostQuitMessage(0);
	return 0;
}