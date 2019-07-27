/*##############################################################################
 * 文件：DuiAppConfigWnd.cpp
 * 描述：应用配置窗口类的实现
 * 作者：Teck
 * 时间：2018年8月2日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#include "stdafx.h"
#include "AppConfigWnd.h"
#include "ConfigParser.h"
#include "atlconv.h"

// 消息映射
DUI_BEGIN_MESSAGE_MAP(CAppConfigWnd, WindowImplBase)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_END_MESSAGE_MAP()

/*------------------------------------------------------------------------------
 * 描  述：构造函数
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
CAppConfigWnd::CAppConfigWnd() : m_bUseDefaultApp(true), m_bUseDefaultServer(true)
{
}

/*------------------------------------------------------------------------------
 * 描  述：析构函数
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
CAppConfigWnd::~CAppConfigWnd()
{
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，DUI调用获取窗口xml布局文件目录
 * 参  数：无
 * 返回值：xml布局文件目录
 ------------------------------------------------------------------------------*/
CDuiString CAppConfigWnd::GetSkinFolder()
{
	return CDuiString(L"skin");
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，DUI调用获取窗口xml布局文件名称
 * 参  数：无
 * 返回值：xml布局文件名称
 ------------------------------------------------------------------------------*/
CDuiString CAppConfigWnd::GetSkinFile()
{
	return CDuiString(L"app_config.xml");
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，DUI调用获取窗口类名称
 * 参  数：无
 * 返回值：窗口类名称
 ------------------------------------------------------------------------------*/
LPCTSTR CAppConfigWnd::GetWindowClassName(void) const
{
	return L"CDuiAppConfigWnd";
}

/*------------------------------------------------------------------------------
 * 描  述：使用默认配置的界面
 * 参  数：无
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CAppConfigWnd::SetUseDefaultApp()
{
	demo::ClientConfig& config = demo::CConfigParser::GetInstance().GetClientConfig();

	COptionUI* pDefaultAppOption = (COptionUI*)m_PaintManager.FindControl(L"default_app_option");
	CLabelUI* pAppIdLabel = (CLabelUI*)m_PaintManager.FindControl(L"app_id_label");
	CLabelUI* pAppSecretLabel = (CLabelUI*)m_PaintManager.FindControl(L"app_secret_label");
	CRichEditUI *pAppIdEdit = (CRichEditUI*)m_PaintManager.FindControl(L"app_id_edit");
	CRichEditUI *pAppSecretEdit = (CRichEditUI*)m_PaintManager.FindControl(L"app_secret_edit");
	assert(pAppIdLabel && pAppSecretLabel && pAppIdEdit && pAppSecretEdit);

	USES_CONVERSION;
	pDefaultAppOption->Selected(true);
	pAppIdEdit->SetText(A2T(config.strAppId.c_str()));
	pAppSecretEdit->SetText(A2T(config.strAppSecret.c_str()));

	// 文本显示为灰色
	pAppIdLabel->SetAttribute(L"textcolor", L"0xFFD2D2D2");
	pAppSecretLabel->SetAttribute(L"textcolor", L"0xFFD2D2D2");
	pAppIdEdit->SetAttribute(L"textcolor", L"0xFFD2D2D2");
	pAppSecretEdit->SetAttribute(L"textcolor", L"0xFFD2D2D2");

	// 文本框不可编辑
	pAppIdEdit->SetEnabled(false);
	pAppSecretEdit->SetEnabled(false);
}

/*------------------------------------------------------------------------------
 * 描  述：使用用户自定义配置的界面
 * 参  数：无
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CAppConfigWnd::SetUseUserDefinedApp()
{
	demo::ClientConfig& config = demo::CConfigParser::GetInstance().GetClientConfig();

	COptionUI* pUserDefinedAppOption = (COptionUI*)m_PaintManager.FindControl(L"user_app_option");
	CLabelUI* pAppIdLabel = (CLabelUI*)m_PaintManager.FindControl(L"app_id_label");
	CLabelUI* pAppSecretLabel = (CLabelUI*)m_PaintManager.FindControl(L"app_secret_label");
	CRichEditUI *pAppIdEdit = (CRichEditUI*)m_PaintManager.FindControl(L"app_id_edit");
	CRichEditUI *pAppSecretEdit = (CRichEditUI*)m_PaintManager.FindControl(L"app_secret_edit");
	assert(pAppIdLabel && pAppSecretLabel && pAppIdEdit && pAppSecretEdit);

	USES_CONVERSION;
	pUserDefinedAppOption->Selected(true);
	pAppIdEdit->SetText(A2T(config.strUserAppId.c_str()));
	pAppSecretEdit->SetText(A2T(config.strUserAppSecret.c_str()));

	// 文本显示为黑色
	pAppIdLabel->SetAttribute(L"textcolor", L"0xFF000000");
	pAppIdEdit->SetAttribute(L"textcolor", L"0xFF000000");
	pAppSecretLabel->SetAttribute(L"textcolor", L"0xFF000000");
	pAppSecretEdit->SetAttribute(L"textcolor", L"0xFF000000");

	// 文本框可编辑
	pAppIdEdit->SetEnabled(true);
	pAppSecretEdit->SetEnabled(true);
}

void CAppConfigWnd::SetUseDefaultServer()
{
	demo::ClientConfig& config = demo::CConfigParser::GetInstance().GetClientConfig();

	COptionUI* pDefaultServerOption = (COptionUI*)m_PaintManager.FindControl(L"default_server_option");
	CLabelUI* pServerAddressLabel = (CLabelUI*)m_PaintManager.FindControl(L"server_address_label");
	CRichEditUI *pServerAddressEdit = (CRichEditUI*)m_PaintManager.FindControl(L"server_address_edit");
	assert(pDefaultServerOption && pServerAddressLabel && pServerAddressEdit);

	USES_CONVERSION;
	pDefaultServerOption->Selected(true);
	pServerAddressEdit->SetText(A2T(config.strServerAddr.c_str()));

	// 文本显示为黑色
	pServerAddressLabel->SetAttribute(L"textcolor", L"0xFFD2D2D2");
	pServerAddressEdit->SetAttribute(L"textcolor", L"0xFFD2D2D2");

	// 文本框可编辑
	pServerAddressEdit->SetEnabled(false);
}

void CAppConfigWnd::SetUseUserDefinedServer()
{
	demo::ClientConfig& config = demo::CConfigParser::GetInstance().GetClientConfig();

	COptionUI* pUserDefinedServerOption = (COptionUI*)m_PaintManager.FindControl(L"user_server_option");
	CLabelUI* pServerAddressLabel = (CLabelUI*)m_PaintManager.FindControl(L"server_address_label");
	CRichEditUI *pServerAddressEdit = (CRichEditUI*)m_PaintManager.FindControl(L"server_address_edit");
	assert(pUserDefinedServerOption && pServerAddressLabel && pServerAddressEdit);

	USES_CONVERSION;
	pUserDefinedServerOption->Selected(true);
	pServerAddressEdit->SetText(A2T(config.strUserServerAddr.c_str()));

	// 文本显示为黑色
	pServerAddressLabel->SetAttribute(L"textcolor", L"0xFF000000");
	pServerAddressEdit->SetAttribute(L"textcolor", L"0xFF000000");

	// 文本框可编辑
	pServerAddressEdit->SetEnabled(true);
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，初始化
 * 参  数：无
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CAppConfigWnd::InitWindow()
{
	demo::ClientConfig& config = demo::CConfigParser::GetInstance().GetClientConfig();

	(m_bUseDefaultApp = !config.bAppUserDefine) ? SetUseDefaultApp() : SetUseUserDefinedApp();

	(m_bUseDefaultServer = !config.bServerUserDefine) ? SetUseDefaultServer() : SetUseUserDefinedServer();
}

/*------------------------------------------------------------------------------
 * 描  述：点击按钮处理
 * 参  数：msg 消息类型
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CAppConfigWnd::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == L"cancel_btn")
	{
		Close();
	}
	else if (msg.pSender->GetName() == L"ok_btn")
	{
		demo::ClientConfig& config = demo::CConfigParser::GetInstance().GetClientConfig();
		
		config.bAppUserDefine = !m_bUseDefaultApp;
		if (!m_bUseDefaultApp)
		{
			CRichEditUI* pAppIdEdit = (CRichEditUI*)m_PaintManager.FindControl(L"app_id_edit");
			CRichEditUI* pAppSecretEdit = (CRichEditUI*)m_PaintManager.FindControl(L"app_secret_edit");

			USES_CONVERSION;
			config.strUserAppId = T2A(pAppIdEdit->GetText());
			config.strUserAppSecret = T2A(pAppSecretEdit->GetText());
		}

		config.bServerUserDefine = !m_bUseDefaultServer;
		if (!m_bUseDefaultServer)
		{
			CRichEditUI* pServerAddressEdit = (CRichEditUI*)m_PaintManager.FindControl(L"server_address_edit");

			USES_CONVERSION;
			config.strUserServerAddr = T2A(pServerAddressEdit->GetText());
		}

		Close(); // 关闭对话框

		RestartApp(); // 重启程序使得配置生效
	}
	else
	{
		WindowImplBase::OnClick(msg);
	}
}

void CAppConfigWnd::OnSelectChanged(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == L"default_app_option")
	{
		if (!m_bUseDefaultApp)
		{
			m_bUseDefaultApp = true;
			SetUseDefaultApp();
		}
	}
	else if (msg.pSender->GetName() == L"user_app_option")
	{
		if (m_bUseDefaultApp)
		{
			m_bUseDefaultApp = false;
			SetUseUserDefinedApp();
		}
	}
	else if (msg.pSender->GetName() == L"default_server_option")
	{
		if (!m_bUseDefaultServer)
		{
			m_bUseDefaultServer = true;
			SetUseDefaultServer();
		}
	}
	else if (msg.pSender->GetName() == L"user_server_option")
	{
		if (m_bUseDefaultServer)
		{
			m_bUseDefaultServer = false;
			SetUseUserDefinedServer();
		}
	}
}


/*------------------------------------------------------------------------------
 * 描  述：重启程序
 * 参  数：无
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CAppConfigWnd::RestartApp()
{
	//获取exe程序当前路径  
	TCHAR szAppName[MAX_PATH];
	::GetModuleFileName(DuiLib::CPaintManagerUI::GetInstance(), szAppName, MAX_PATH);

	TCHAR szAppFullName[MAX_PATH];
	_snwprintf(szAppFullName, MAX_PATH, L"%s", szAppName);

	//重启程序  
	STARTUPINFO StartInfo;
	PROCESS_INFORMATION procStruct;
	memset(&StartInfo, 0, sizeof(STARTUPINFO));
	StartInfo.cb = sizeof(STARTUPINFO);
	::CreateProcess(
		(LPCTSTR)szAppFullName,
		NULL,
		NULL,
		NULL,
		FALSE,
		NORMAL_PRIORITY_CLASS,
		NULL,
		NULL,
		&StartInfo,
		&procStruct);

	// 当前进程退出之前先保存配置
	demo::CConfigParser::GetInstance().Serialize();

	::ExitProcess(0);
}