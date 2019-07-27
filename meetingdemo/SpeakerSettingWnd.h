#pragma once
#include "UIlib.h"

using namespace DuiLib;

#define WINDOW_HEIGTH  160
#define WINDOW_WIDTH   80

class CSpeakerSettingWnd: public WindowImplBase
{
public:
	CSpeakerSettingWnd();
	~CSpeakerSettingWnd();

public:
	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;

public:
	virtual void Notify(TNotifyUI& msg);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};