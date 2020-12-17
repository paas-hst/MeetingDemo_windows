#pragma once

#include <set>
#include "UIlib.h"
#include "define.h"

using namespace DuiLib;


class CBoardToolbarWnd : public WindowImplBase 
{
public:
	CBoardToolbarWnd();
	~CBoardToolbarWnd();

	DUI_DECLARE_MESSAGE_MAP()

private:
	virtual void OnClick(TNotifyUI& msg) override;

	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;
	virtual void InitWindow() override;
	

	CWndShadow m_wndShadow;
};

