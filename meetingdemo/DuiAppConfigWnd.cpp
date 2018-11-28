/*##############################################################################
 * 文件：DuiAppConfigWnd.cpp
 * 描述：应用配置窗口类的实现
 * 作者：Teck
 * 时间：2018年8月2日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#include "stdafx.h"
#include "DuiAppConfigWnd.h"
#include "ConfigParser.h"
#include "atlconv.h"

// 消息映射
DUI_BEGIN_MESSAGE_MAP(CDuiAppConfigWnd, WindowImplBase)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

/*------------------------------------------------------------------------------
 * 描  述：构造函数
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
CDuiAppConfigWnd::CDuiAppConfigWnd() : m_bUseDefault(true)
{
}

/*------------------------------------------------------------------------------
 * 描  述：析构函数
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
CDuiAppConfigWnd::~CDuiAppConfigWnd()
{
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，DUI调用获取窗口xml布局文件目录
 * 参  数：无
 * 返回值：xml布局文件目录
 ------------------------------------------------------------------------------*/
CDuiString CDuiAppConfigWnd::GetSkinFolder()
{
	return CDuiString(L"skin");
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，DUI调用获取窗口xml布局文件名称
 * 参  数：无
 * 返回值：xml布局文件名称
 ------------------------------------------------------------------------------*/
CDuiString CDuiAppConfigWnd::GetSkinFile()
{
	return CDuiString(L"app_config.xml");
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，DUI调用获取窗口类名称
 * 参  数：无
 * 返回值：窗口类名称
 ------------------------------------------------------------------------------*/
LPCTSTR CDuiAppConfigWnd::GetWindowClassName(void) const
{
	return L"CDuiAppConfigWnd";
}

/*------------------------------------------------------------------------------
 * 描  述：使用默认配置的界面
 * 参  数：无
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiAppConfigWnd::SetUseDefaultConfig()
{
	demo::ClientConfig& config = demo::CConfigParser::GetInstance().GetClientConfig();

	CButtonUI* pSwitchBtn = (CButtonUI*)m_PaintManager.FindControl(L"switch_btn");
	CLabelUI* pSwitchLabel = (CLabelUI*)m_PaintManager.FindControl(L"switch_label");
	CLabelUI* pAppIdLabel = (CLabelUI*)m_PaintManager.FindControl(L"app_id_label");
	CLabelUI* pAppSecretLabel = (CLabelUI*)m_PaintManager.FindControl(L"app_secret_label");
	CRichEditUI *pAppIdEdit = (CRichEditUI*)m_PaintManager.FindControl(L"app_id_edit");
	CRichEditUI *pAppSecretEdit = (CRichEditUI*)m_PaintManager.FindControl(L"app_secret_edit");

	assert(pSwitchBtn && pSwitchLabel && pAppIdLabel && pAppSecretLabel && pAppIdEdit && pAppSecretEdit);

	pSwitchLabel->SetAttribute(L"text", L"使用默认配置");

	pSwitchBtn->SetAttribute(L"normalimage", L"img\\login\\switch_off.png");
	pSwitchBtn->SetAttribute(L"hotimage", L"img\\login\\switch_off_hot.png");
	pSwitchBtn->SetAttribute(L"pushedimage", L"img\\login\\switch_off_press.png");

	USES_CONVERSION;
	pAppIdEdit->SetText(A2T(config.strAppId.c_str()));
	pAppSecretEdit->SetText(A2T(config.strAppSecret.c_str()));

	pAppIdLabel->SetAttribute(L"textcolor", L"0xFFD2D2D2");
	pAppSecretLabel->SetAttribute(L"textcolor", L"0xFFD2D2D2");
	pAppIdEdit->SetAttribute(L"textcolor", L"0xFFD2D2D2");
	pAppSecretEdit->SetAttribute(L"textcolor", L"0xFFD2D2D2");
}

/*------------------------------------------------------------------------------
 * 描  述：使用用户自定义配置的界面
 * 参  数：无
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiAppConfigWnd::SetUseUserDefineConfig()
{
	demo::ClientConfig& config = demo::CConfigParser::GetInstance().GetClientConfig();

	CButtonUI* pSwitchBtn = (CButtonUI*)m_PaintManager.FindControl(L"switch_btn");
	CLabelUI* pSwitchLabel = (CLabelUI*)m_PaintManager.FindControl(L"switch_label");
	CLabelUI* pAppIdLabel = (CLabelUI*)m_PaintManager.FindControl(L"app_id_label");
	CLabelUI* pAppSecretLabel = (CLabelUI*)m_PaintManager.FindControl(L"app_secret_label");
	CRichEditUI *pAppIdEdit = (CRichEditUI*)m_PaintManager.FindControl(L"app_id_edit");
	CRichEditUI *pAppSecretEdit = (CRichEditUI*)m_PaintManager.FindControl(L"app_secret_edit");

	assert(pSwitchBtn && pSwitchLabel && pAppIdLabel && pAppSecretLabel && pAppIdEdit && pAppSecretEdit);

	pSwitchLabel->SetAttribute(L"text", L"使用用户配置");

	pSwitchBtn->SetAttribute(L"normalimage", L"img\\login\\switch_on.png");
	pSwitchBtn->SetAttribute(L"hotimage", L"img\\login\\switch_on_hover.png");
	pSwitchBtn->SetAttribute(L"pushedimage", L"img\\login\\switch_on_press.png");

	USES_CONVERSION;
	pAppIdEdit->SetText(A2T(config.strUserAppId.c_str()));
	pAppSecretEdit->SetText(A2T(config.strUserAppSecret.c_str()));

	pAppIdLabel->SetAttribute(L"textcolor", L"0xFF000000");
	pAppSecretLabel->SetAttribute(L"textcolor", L"0xFF000000");
	pAppIdEdit->SetAttribute(L"textcolor", L"0xFF000000");
	pAppSecretEdit->SetAttribute(L"textcolor", L"0xFF000000");
}

/*------------------------------------------------------------------------------
 * 描  述：虚拟函数，初始化
 * 参  数：无
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiAppConfigWnd::InitWindow()
{
	demo::ClientConfig& config = demo::CConfigParser::GetInstance().GetClientConfig();
	(m_bUseDefault = !config.bUserDefine) ? SetUseDefaultConfig() : SetUseUserDefineConfig();
}

/*------------------------------------------------------------------------------
 * 描  述：点击按钮处理
 * 参  数：msg 消息类型
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiAppConfigWnd::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == L"cancel_btn")
	{
		Close();
	}
	else if (msg.pSender->GetName() == L"ok_btn")
	{
		demo::ClientConfig& config = demo::CConfigParser::GetInstance().GetClientConfig();
		config.bUserDefine = !m_bUseDefault;
		if (!m_bUseDefault)
		{
			CRichEditUI* pAppIdEdit = (CRichEditUI*)m_PaintManager.FindControl(L"app_id_edit");
			CRichEditUI* pAppSecretEdit = (CRichEditUI*)m_PaintManager.FindControl(L"app_secret_edit");

			USES_CONVERSION;
			config.strUserAppId = T2A(pAppIdEdit->GetText());
			config.strUserAppSecret = T2A(pAppSecretEdit->GetText());
		}

		Close(); // 关闭对话框

		RestartApp(); // 重启程序使得配置生效
	}
	else if (msg.pSender->GetName() == L"switch_btn")
	{
		(m_bUseDefault = !m_bUseDefault) ? SetUseDefaultConfig() : SetUseUserDefineConfig();
	}
	else
	{
		WindowImplBase::OnClick(msg);
	}
}

/*------------------------------------------------------------------------------
 * 描  述：重启程序
 * 参  数：无
 * 返回值：无
 ------------------------------------------------------------------------------*/
void CDuiAppConfigWnd::RestartApp()
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