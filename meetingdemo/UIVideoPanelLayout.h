#pragma once

#include "define.h"
#include "UIlib.h"
#include "UIVideoPanel.h"

class CVideoPanelLayoutUI : public DuiLib::CContainerUI
{
public:
	static const TCHAR* K_UI_INTERFACE_NAME;

	CVideoPanelLayoutUI();
	~CVideoPanelLayoutUI();

	bool PreviewLocalCamera(int nDeviceId);
	void StopLocalPreview(int nDeviceId);
	void SetPublishVideoId(int nDeviceId, const char* szVideoId);

	fsp::ErrCode OpenRemoteVideo(const char* szUserId, const char* szVideoId);
	void CloseRemoteVideo(const char* szUserId, const char* szVideoId);

	void LayoutVideoPanels();
	void StopAllVideo();

	void UpdateVideoInfo();
	void InitVideoPanles();

	virtual LPVOID GetInterface(LPCTSTR pstrName) override;
	virtual void SetPos(RECT rc, bool bNeedInvalidate = true) override;

private:
	int GetVisiblePanels();
	
	static const int MAX_VIDEO_PANEL_COUNT = 6;
	bool m_isScreenShare = false;
	CVideoPanelUI* m_arrVideoPanels[MAX_VIDEO_PANEL_COUNT];
};

