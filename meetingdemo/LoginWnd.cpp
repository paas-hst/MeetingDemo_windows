#include "stdafx.h"
#include "LoginWnd.h"
#include "SdkManager.h"
#include "AppConfigWnd.h"
#include "util.h"
#include <atlconv.h>

 // 消息映射
DUI_BEGIN_MESSAGE_MAP(CLoginWnd, WindowImplBase)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

CLoginWnd::CLoginWnd()
{
}

CLoginWnd::~CLoginWnd()
{
}

CDuiString CLoginWnd::GetSkinFolder()
{
	return CDuiString(L"skin");
}

CDuiString CLoginWnd::GetSkinFile()
{
	return CDuiString(L"login_wnd.xml");
}

LPCTSTR CLoginWnd::GetWindowClassName(void) const
{
	return L"CDuiLoginWnd";
}

void CLoginWnd::ShowLoading()
{
	CControlUI* pBtnLogin = (CControlUI*)m_PaintManager.FindControl(L"login_btn");
	CControlUI* pBtnLoading = (CControlUI*)m_PaintManager.FindControl(L"login_waiting");

	pBtnLogin->SetVisible(false);
	pBtnLoading->SetVisible(true);

	CLabelUI* pLabelErrorInfo = (CLabelUI*)m_PaintManager.FindControl(L"login_errorinfo");
	pLabelErrorInfo->SetVisible(false);
}

void CLoginWnd::ShowError(const CDuiString& strErrInfo)
{
	CControlUI* pBtnLogin = (CControlUI*)m_PaintManager.FindControl(L"login_btn");
	CControlUI* pBtnLoading = (CControlUI*)m_PaintManager.FindControl(L"login_waiting");

	pBtnLogin->SetVisible(true);
	pBtnLoading->SetVisible(false);

	CLabelUI* pLabelErrorInfo = (CLabelUI*)m_PaintManager.FindControl(L"login_errorinfo");
	pLabelErrorInfo->SetVisible(true);
	pLabelErrorInfo->SetText(strErrInfo);
}

void CLoginWnd::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == L"login_btn")
	{
		CEditUI* pUserEdit = (CEditUI*)m_PaintManager.FindControl(L"user_name_edit");		
		CDuiString strUserId = pUserEdit->GetText();

		CEditUI* pUserCustomName = (CEditUI*)m_PaintManager.FindControl(L"custom_name_edit");
		CDuiString strCustomName = pUserCustomName->GetText();

		if (strUserId.IsEmpty() || strUserId.GetLength() >= 32) {
			demo::ShowMessageBox(m_hWnd, L"UserId不能为空且长度不超过32");
		}
		else
		{
			fsp::ErrCode errCode = CSdkManager::GetInstance().Login(strUserId, strCustomName);
			if (errCode == fsp::ERR_OK) {
				ShowLoading();
			}
			else {
				ShowError(CSdkManager::GetInstance().BuildErrorInfo(errCode));
			}
		}
	}
	else if (msg.pSender->GetName() == L"app_config_btn")
	{
		CAppConfigWnd wndAppConfig;
		wndAppConfig.Create(m_hWnd, _T("CDuiAppConfigWnd"), UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
		wndAppConfig.CenterWindow();
		wndAppConfig.ShowModal();
	}
	else
	{
		WindowImplBase::OnClick(msg);
	}
}

void CLoginWnd::InitWindow()
{
	SetIcon(IDI_FSPCLIENT);

	CLabelUI* pLabel = (CLabelUI*)m_PaintManager.FindControl(L"login_sdkver");

	WCHAR wszVersion[128] = { 0 };
	demo::ConvertUtf8ToUnicode(CSdkManager::GetInstance().GetFspEngin()->GetVersion().c_str(),
		wszVersion, 128);
	CDuiString strVerInfo;
	strVerInfo.Format(L"SdkVersion:  %s", wszVersion);
	pLabel->SetText(strVerInfo);

	m_wndShadow.Create(m_hWnd);
	RECT rcCorner = { 6,6,6,6 };
	RECT rcHoleOffset = { 7,7,7,7 };
	m_wndShadow.SetImage(_T("img\\wnd_bg.png"), rcCorner, rcHoleOffset);
}

LRESULT CLoginWnd::OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	PostQuitMessage(0);
	return 0;
}