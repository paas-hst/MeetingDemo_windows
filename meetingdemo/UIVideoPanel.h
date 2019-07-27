#pragma once

#include "define.h"
#include "UIlib.h"


class CVideoRenderControlUI;

class CVideoPanelUI : public DuiLib::CControlUI
{
public:
	static const TCHAR* K_UI_INTERFACE_NAME;

	CVideoPanelUI();
	~CVideoPanelUI();

	bool HaveVideo();

	bool ShowLocalPreview(int nDeviceId);
	void StopLocalPreview();
	void SetPublishVideoId(int nDeviceId, const char* szVideoId);

	fsp::ErrCode OpenRemoteVideo(const char* szUserId, const char* szVideoId);
	void CloseRemoteVideo();

	void UpdateVideoInfo();

	int GetLocalPreviewDeviceId() const;

	std::string GetUserId() const { return m_strUserId; }
	std::string GetVideoId() const { return m_strVideoId; }

	void ChangeRenderMode(fsp::RenderMode mode);

	fsp::RenderMode GetRenderMode() const {
		return m_eRenderMode;
	}

	HWND GetRenderHwnd() const;
	void UpdateVideoPos();
	
	virtual void SetPos(RECT rc, bool bNeedInvalidate = true) override;

protected:

	virtual LPCTSTR GetClass() const override;
	virtual LPVOID GetInterface(LPCTSTR pstrName) override;
	virtual void SetInternVisible(bool bVisible = true) override;
	
	virtual void DoInit() override;

private:
	void UpdateVideoRenderVisible();
	void CreateControlWnd(bool isBkMode);

	void DoClearVideo();

private:
	int m_nLocalPreviewDeviceId = fsp::INVALID_DEVICE_ID;

	std::string m_strUserId;
	std::string m_strVideoId;

	RECT m_lastPosRect;
	
	fsp::RenderMode m_eRenderMode;

	HWND m_hVideoWnd;
	HWND m_hControlBgWnd;
	HWND m_hControlFWnd;

	CVideoRenderControlUI* m_pControlBgUi;
	CVideoRenderControlUI* m_pControlFUi;
};

