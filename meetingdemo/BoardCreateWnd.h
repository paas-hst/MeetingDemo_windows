#pragma once
#include "UIlib.h"

using namespace DuiLib;

#define WINDOW_HEIGTH  160
#define WINDOW_WIDTH   80

class CBoardCreateWnd : public WindowImplBase
{
public:
	CBoardCreateWnd();
	~CBoardCreateWnd();

public:
	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;

public:
	//virtual void Notify(TNotifyUI& msg);
	void OnClick(TNotifyUI& msg);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};
