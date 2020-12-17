#include "stdafx.h"

#include "UIMenu.h"
#include "util.h"
#include "UIBoardPanel.h"
#include "SdkManager.h"

#define WHITEBOARD_NAME_TEST "whiteboard1"   //test by August 2019.08.12

const TCHAR* CBoardPanelUI::K_UI_INTERFACE_NAME = L"BoardPanel";

CBoardPanelUI::CBoardPanelUI()
{
	m_pFspBoard = CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard();
	memset(&m_lastPosRect, 0, sizeof(m_lastPosRect));
	m_isOpen = false;
}

CBoardPanelUI::~CBoardPanelUI()
{

}


LPCTSTR CBoardPanelUI::GetClass() const
{
	return _T("BoardPanelUI");
}

LPVOID CBoardPanelUI::GetInterface(LPCTSTR pstrName)
{
	if (_tcsicmp(pstrName, K_UI_INTERFACE_NAME) == 0)
	{
		return static_cast<CBoardPanelUI*>(this);
	}
	return CControlUI::GetInterface(pstrName);
}

void CBoardPanelUI::SetInternVisible(bool bVisible)
{
	CControlUI::SetInternVisible(bVisible);

	HWND hwnd = m_pFspBoard->GetRenderWnd(m_strRemoteBoardId.c_str());
	if (hwnd)
	{
		if (bVisible) {
			::ShowWindow(hwnd, SW_SHOW);
		}
		else {
			::ShowWindow(hwnd, SW_HIDE);
		}
	}
}

void CBoardPanelUI::SetPos(RECT arc, bool bNeedInvalidate /* = true */)
{
	CControlUI::SetPos(arc, bNeedInvalidate);

	RECT rc = GetClientPos();
	POINT pt;
	pt.x = rc.left;
	pt.y = rc.top;
	ClientToScreen(m_pManager->GetPaintWindow(), &pt);
	
	HWND hwnd = m_pFspBoard->GetRenderWnd(m_strRemoteBoardId.c_str());
	MoveWindow(hwnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
}

void CBoardPanelUI::BoardOpen(const std::string& nboardname, const std::string& nboardid)
{
	//@todo, test august
	if (m_isOpen == false)
	{
		m_strRemoteBoardName = nboardname;
		m_strRemoteBoardId = nboardid;

		m_pFspBoard->Open(m_strRemoteBoardId.c_str(), m_pManager->GetPaintWindow());
		m_pFspBoard->SetLineWidth(m_strRemoteBoardId.c_str(), 4);
		m_pFspBoard->SetLineColor(m_strRemoteBoardId.c_str(), 0xff00FF52);
		m_pFspBoard->SetPageChangeSynMode(m_strRemoteBoardId.c_str(), true);

		m_isOpen = true;

		RECT rc = GetClientPos();

		HWND hwnd = m_pFspBoard->GetRenderWnd(m_strRemoteBoardId.c_str());
		MoveWindow(hwnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
	}
}

void CBoardPanelUI::BoardClose(const std::string& nboardid)
{
	//@todo, test august
	if (m_isOpen == true && m_strRemoteBoardId == nboardid)
	{
		m_pFspBoard->Close(m_strRemoteBoardId.c_str());
		m_isOpen = false;
	}
}

void CBoardPanelUI::DoInit()
{
	SetBkColor(0xFFD8D8D8);
}
