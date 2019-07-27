#include "stdafx.h"
#include "UIVideoPanelLayout.h"
#include "util.h"

const TCHAR* CVideoPanelLayoutUI::K_UI_INTERFACE_NAME = L"VideoPanelLayout";

CVideoPanelLayoutUI::CVideoPanelLayoutUI()
{
	for (int i = 0; i < MAX_VIDEO_PANEL_COUNT; i++) {
		m_arrVideoPanels[i] = nullptr;
	}
}

CVideoPanelLayoutUI::~CVideoPanelLayoutUI()
{

}

LPVOID CVideoPanelLayoutUI::GetInterface(LPCTSTR pstrName)
{
	if (_tcsicmp(pstrName, K_UI_INTERFACE_NAME) == 0)
	{
		return static_cast<CVideoPanelLayoutUI*>(this);
	}
	return CContainerUI::GetInterface(pstrName);
}

void CVideoPanelLayoutUI::SetPos(RECT rc, bool bNeedInvalidate /* = true */)
{
	CContainerUI::SetPos(rc, bNeedInvalidate);

	LayoutVideoPanels();
}

void CVideoPanelLayoutUI::InitVideoPanles()
{
	CDuiString attrContent = GetCustomAttribute(L"content");
	CDuiString strPanelNameFormat = L"main_video_panel%d";
	if (attrContent == L"screenshare") {
		strPanelNameFormat = L"main_screenshare_panel%d";
	}

	if (m_arrVideoPanels[0] == nullptr) {
		for (int i = 0; i < MAX_VIDEO_PANEL_COUNT; i++) {
			CDuiString strPanelName;
			strPanelName.Format(strPanelNameFormat, i + 1);
			m_arrVideoPanels[i] = (CVideoPanelUI*)m_pManager->FindControl(strPanelName);
		}
	}
}

bool CVideoPanelLayoutUI::PreviewLocalCamera(int nDeviceId)
{
	for (CVideoPanelUI* pVideoPanel : m_arrVideoPanels) {
		if (!pVideoPanel->HaveVideo()) {
			pVideoPanel->SetVisible(true);
			return pVideoPanel->ShowLocalPreview(nDeviceId);
		}
	}

	return false;
}

void CVideoPanelLayoutUI::SetPublishVideoId(int nDeviceId, const char* szVideoId)
{
	for (CVideoPanelUI* pVideoPanel : m_arrVideoPanels) {
		if (pVideoPanel->GetLocalPreviewDeviceId() == nDeviceId) {
			pVideoPanel->SetPublishVideoId(nDeviceId, szVideoId);
			break;
		}
	}
}

void CVideoPanelLayoutUI::StopLocalPreview(int nDeviceId)
{
	for (CVideoPanelUI* pVideoPanel : m_arrVideoPanels) {
		if (pVideoPanel->GetLocalPreviewDeviceId() == nDeviceId) {
			pVideoPanel->SetVisible(false);
			return pVideoPanel->StopLocalPreview();
		}
	}
	LayoutVideoPanels();
}

fsp::ErrCode CVideoPanelLayoutUI::OpenRemoteVideo(const char* szUserId, const char* szVideoId)
{
	fsp::ErrCode errCode = fsp::ERR_OK;
	for (CVideoPanelUI* pVideoPanel : m_arrVideoPanels) {
		if (!pVideoPanel->HaveVideo()) {
			pVideoPanel->SetVisible(true);
			errCode = pVideoPanel->OpenRemoteVideo(szUserId, szVideoId);
			break;
		}
	}

	LayoutVideoPanels();

	return errCode;
}

void CVideoPanelLayoutUI::CloseRemoteVideo(const char* szUserId, const char* szVideoId)
{
	for (CVideoPanelUI* pVideoPanel : m_arrVideoPanels) {
		if (pVideoPanel->GetUserId() == szUserId && pVideoPanel->GetVideoId() == szVideoId) {
			pVideoPanel->SetVisible(false);
			pVideoPanel->CloseRemoteVideo();
		}
	}
	LayoutVideoPanels();
}

//根据有视频的数量来布局
void CVideoPanelLayoutUI::LayoutVideoPanels()
{
	int width = GetWidth();
	int height = GetHeight();

	int nVisibleCount = GetVisiblePanels();
	int nColumn = 1;
	int nRow = 1;
	
	if (nVisibleCount == 2) {
		nColumn = 2;
	}
	else if (nVisibleCount == 3 || nVisibleCount == 4) {
		nRow = 2;
		nColumn = 2;
	}
	else if (nVisibleCount == 5 || nVisibleCount == 6) {
		nRow = 2;
		nColumn = 3;
	}

	int dwWndWidth = width / nColumn;
	int dwWndHeight = height / nRow;

	int dwVisibleIndex = 0;
	for (int i = 0; i < MAX_VIDEO_PANEL_COUNT; i++)
	{
		//一路视频没有，确保低1个显示
		if ((dwVisibleIndex == 0 && nVisibleCount == 0) || m_arrVideoPanels[i]->HaveVideo()) {
			RECT rect;
			rect.left = dwVisibleIndex % nColumn * dwWndWidth;
			rect.top = dwVisibleIndex / nColumn * dwWndHeight;
			rect.left += (dwVisibleIndex % nColumn);
			rect.top += (dwVisibleIndex / nColumn);

			rect.right = rect.left + dwWndWidth;
			rect.bottom = rect.top + dwWndHeight;

			m_arrVideoPanels[i]->SetVisible(true);
			m_arrVideoPanels[i]->SetPos(rect);

			dwVisibleIndex++;
		}
		else {
			m_arrVideoPanels[i]->SetVisible(false);
		}
	}
}

void CVideoPanelLayoutUI::StopAllVideo()
{
	for (CVideoPanelUI* pVideoPanel : m_arrVideoPanels) {
		pVideoPanel->CloseRemoteVideo();
		pVideoPanel->StopLocalPreview();
	}
}

void CVideoPanelLayoutUI::UpdateVideoInfo()
{
	for (CVideoPanelUI* pVideoPanel : m_arrVideoPanels) {
		if (pVideoPanel->HaveVideo()) {
			pVideoPanel->UpdateVideoInfo();
		}
	}
}

int CVideoPanelLayoutUI::GetVisiblePanels()
{
	int nVisibleCount = 0;
	for (int i = 0; i < MAX_VIDEO_PANEL_COUNT; i++)
	{
		if (m_arrVideoPanels[i]->HaveVideo()) {
			nVisibleCount++;
		}
	}

	return nVisibleCount;
}