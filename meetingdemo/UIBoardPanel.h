#pragma once

#include "define.h"
#include "UIlib.h"

#include "fsp_engine.h"
#include "fsp_white_board.h"
#include "BoardToolbarWnd.h"

class CVideoRenderControlUI;

class CBoardPanelUI : public DuiLib::CControlUI
{
public:
	static const TCHAR* K_UI_INTERFACE_NAME;

	CBoardPanelUI();
	~CBoardPanelUI();

	virtual void SetPos(RECT rc, bool bNeedInvalidate = true) override;
	void BoardOpen(const std::string& nboardname, const std::string& nboardid);
	void BoardClose(const std::string& nboardid);

protected:

	virtual LPCTSTR GetClass() const override;
	virtual LPVOID GetInterface(LPCTSTR pstrName) override;
	virtual void SetInternVisible(bool bVisible = true) override;
	
	virtual void DoInit() override;

private:
	void UpdateVideoRenderVisible();
	void CreateControlWnd(bool isBkMode);

private:
	int m_nLocalPreviewDeviceId = fsp::INVALID_DEVICE_ID;

	std::string m_strRemoteBoardName;
	std::string m_strRemoteBoardId;
	bool m_isOpen;

	RECT m_lastPosRect;

	fsp::IFspWhiteBoard* m_pFspBoard;
};

