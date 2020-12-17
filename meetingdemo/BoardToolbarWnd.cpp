#include "stdafx.h"

#include <algorithm>

#include "BoardToolbarWnd.h"

#include "core/UIDefine.h"
#include "UIMenu.h"
#include "ToolbarMenu.h"
#include "fsp_engine.h"
#include "SdkManager.h"
#include "util.h"
#include "define.h"

DUI_BEGIN_MESSAGE_MAP(CBoardToolbarWnd, WindowImplBase)
    DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

#define WHITEBOARD_NAME_TEST "whiteboard1"   //test by August 2019.08.12

CBoardToolbarWnd::CBoardToolbarWnd()
{
}

CBoardToolbarWnd::~CBoardToolbarWnd()
{
}

CDuiString CBoardToolbarWnd::GetSkinFolder()
{
	return CDuiString(L"skin");
}


CDuiString CBoardToolbarWnd::GetSkinFile()
{
	return CDuiString(L"board_toolbar_wnd.xml");
}

LPCTSTR CBoardToolbarWnd::GetWindowClassName(void) const
{
	return L"CBoardToolbarWnd";
}

void CBoardToolbarWnd::InitWindow()
{
	m_wndShadow.Create(m_hWnd);
	RECT rcCorner = { 6,6,6,6 };
	RECT rcHoleOffset = { 6,6,6,6 };
	m_wndShadow.SetImage(_T("img\\wnd_bg.png"), rcCorner, rcHoleOffset);
}

void CBoardToolbarWnd::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == L"btn_board_line")
	{
		CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->SetCurOperateType(WHITEBOARD_NAME_TEST, fsp::BOARD_OPERATE_PRODUCE);
		CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->SetCurProduceGraphType(WHITEBOARD_NAME_TEST, fsp::GRAPH_TYPE_LINE);
	}
	else if (msg.pSender->GetName() == L"btn_board_path") {
		CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->SetCurOperateType(WHITEBOARD_NAME_TEST, fsp::BOARD_OPERATE_PRODUCE);
		CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->SetCurProduceGraphType(WHITEBOARD_NAME_TEST, fsp::GRAPH_TYPE_PATH);
	}
	else if (msg.pSender->GetName() == L"btn_board_text") {
		CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->SetCurOperateType(WHITEBOARD_NAME_TEST, fsp::BOARD_OPERATE_PRODUCE);
		CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->SetCurProduceGraphType(WHITEBOARD_NAME_TEST, fsp::GRAPH_TYPE_TEXT);
	}
	else if (msg.pSender->GetName() == L"btn_board_clear") {
		CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->ClearCurrentPage(WHITEBOARD_NAME_TEST);
	}
	else if (msg.pSender->GetName() == L"btn_board_select") {
		CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->SetCurOperateType(WHITEBOARD_NAME_TEST, fsp::BOARD_OPERATE_SELECT);
	}
	else if (msg.pSender->GetName() == L"btn_board_undo") {
		CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->Undo(WHITEBOARD_NAME_TEST);
	}
	else if (msg.pSender->GetName() == L"btn_board_redo") {
		CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->Redo(WHITEBOARD_NAME_TEST);
	}
	else
	{
		WindowImplBase::OnClick(msg);
	}
}
