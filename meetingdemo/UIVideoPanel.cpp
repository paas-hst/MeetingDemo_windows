#include "stdafx.h"

#include "UIMenu.h"
#include "util.h"
#include "SdkManager.h"
#include "UIVideoPanel.h"

#define CAMERA_MENU_NAME_PREFIX L"menu_cam_"

/////////////////////////////////////////////////////////////////////
class CVideoRenderControlUI :public DuiLib::CWindowWnd, DuiLib::INotifyUI
{
public:

	CVideoRenderControlUI(bool isBkMode, CVideoPanelUI* pVideoPanel) {
		m_isBkMode = isBkMode;
		m_pVideoPanel = pVideoPanel;

		ResetVideo();
	}

	~CVideoRenderControlUI() {};

	LPCTSTR GetWindowClassName() const { return _T("UIVideoRenderControl"); };

	UINT GetClassStyle() const { return CS_DBLCLKS; };

	void LoadSkin();

	void UpdateVideoInfo(const WCHAR* szInfo);
	void SetUserId(const WCHAR* szUserId);

	void ResetVideo();

	void Notify(DuiLib::TNotifyUI& msg);
	

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
public:

	DuiLib::CPaintManagerUI m_PaintManager;

private:
	CMenuElementUI* CVideoRenderControlUI::ConstructMenuItem(LPCTSTR szName, LPCTSTR szText, bool bSelected, bool bHasSubMenu);
	void CVideoRenderControlUI::ConstructMenu(CMenuWnd* pMenu);

	enum RemoteControlState
	{
		RemoteControl_No,
		RemoteControl_Waiting,
		RemoteControl_Ing
	};

	RemoteControlState m_RemoteControlState;

	bool m_isBkMode;
	CVideoPanelUI* m_pVideoPanel;

	CLabelUI* m_pLabelUserId = nullptr;
	CLabelUI* m_pLabelVideoInfo = nullptr;
};

void CVideoRenderControlUI::UpdateVideoInfo(const WCHAR* szInfo)
{
	if (!m_isBkMode && m_pLabelVideoInfo) {
		m_pLabelVideoInfo->SetText(szInfo);
	}
}

void CVideoRenderControlUI::SetUserId(const WCHAR* szUserId)
{
	if (!m_isBkMode && m_pLabelUserId) {
		m_pLabelUserId->SetText(szUserId); 
	}
}

void CVideoRenderControlUI::ResetVideo()
{
	m_RemoteControlState = RemoteControl_No;
}

void CVideoRenderControlUI::LoadSkin()
{
	m_PaintManager.Init(m_hWnd);
	DuiLib::CDialogBuilder builder;
	DuiLib::CControlUI* pRoot = builder.Create(_T("video_controler.xml"), (UINT)0, nullptr, &m_PaintManager);

	m_PaintManager.AttachDialog(pRoot);
	m_PaintManager.AddNotifier(this);

	CDuiString strControlNames[3] = { L"label_user_id", L"btn_video_more", L"label_video_info" };
	for (auto iter : strControlNames) {
		CControlUI* pLabelUserId = m_PaintManager.FindControl(iter);
		if (m_isBkMode) {
			pLabelUserId->SetBkColor(0xff7a7a7a);
			pLabelUserId->SetBkImage(L"img\\video_info.png");
		}
		else {
			pLabelUserId->SetBkColor(0xff000000);
		}
	}

	m_pLabelVideoInfo = (CLabelUI*)m_PaintManager.FindControl(_T("label_video_info"));
	m_pLabelUserId = (CLabelUI*)m_PaintManager.FindControl(L"label_user_id");
}

void CVideoRenderControlUI::Notify(DuiLib::TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == L"btn_video_more")
		{
			CMenuWnd* pMenu = new CMenuWnd(GetHWND());

			CDuiPoint point = msg.ptMouse;
			ClientToScreen(GetHWND(), &point);

			pMenu->Init(NULL,
				this,
				L"popup_menu.xml",
				_T("xml"),
				point,
				eMenuAlignment_Right | eMenuAlignment_Bottom);

			ConstructMenu(pMenu);
		}
	}
	else if (msg.sType == DUI_MSGTYPE_ITEMCLICK) {
		CDuiString strSenderName = msg.pSender->GetName();
		fsp::IFspEngine* pEngine = CSdkManager::GetInstance().GetFspEngin();

		if (strSenderName == L"video_disp_sfpp" && m_pVideoPanel)
		{
			m_pVideoPanel->ChangeRenderMode(RENDERMODE_SCALE_FILL);
		}
		else if (strSenderName == L"video_disp_dbcj" && m_pVideoPanel)
		{
			m_pVideoPanel->ChangeRenderMode(RENDERMODE_CROP_FILL);
		}
		else if (strSenderName == L"video_disp_dbwz" && m_pVideoPanel)
		{
			m_pVideoPanel->ChangeRenderMode(RENDERMODE_FIT_CENTER);
		}
		else if (strSenderName == L"video_operate_remotecontrol" && m_pVideoPanel)
		{
			if (m_RemoteControlState == RemoteControl_No)
			{
				if (pEngine->RemoteControlOperation(m_pVideoPanel->GetUserId().c_str(), REMOTE_CONTROL_REQUEST) == ERR_OK)
				{
					m_RemoteControlState = RemoteControl_Waiting;
				}

			}
			else if (m_RemoteControlState == RemoteControl_Waiting)
			{
				m_RemoteControlState = RemoteControl_Ing;
			}
			else if (m_RemoteControlState == RemoteControl_Ing)
			{
				if (pEngine->RemoteControlOperation(m_pVideoPanel->GetUserId().c_str(), REMOTE_CONTROL_CANCEL) == ERR_OK)
				{
					m_RemoteControlState = RemoteControl_No;
				}
			}
		}
		else if (strSenderName.Find(CAMERA_MENU_NAME_PREFIX) == 0 && m_pVideoPanel) {
			CDuiString strCameraIndex = strSenderName.Right(strSenderName.GetLength() - CDuiString(CAMERA_MENU_NAME_PREFIX).GetLength());
			int nCameraIndex = std::stoi(strCameraIndex.GetData());
			if (nCameraIndex != m_pVideoPanel->GetLocalPreviewDeviceId()) {
				//对当前videoid 切换使用另外一个设备
				std::string strVideoId = m_pVideoPanel->GetVideoId();
				CSdkManager::GetInstance().StartPublishVideo(strVideoId.c_str(), nCameraIndex);
				m_pVideoPanel->StopLocalPreview();
				m_pVideoPanel->ShowLocalPreview(nCameraIndex);
				m_pVideoPanel->SetPublishVideoId(nCameraIndex, strVideoId.c_str());
			}
		}
	}
}

LRESULT CVideoRenderControlUI::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	if (m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

CMenuElementUI* CVideoRenderControlUI::ConstructMenuItem(LPCTSTR szName, LPCTSTR szText, bool bSelected, bool bHasSubMenu)
{
	CMenuElementUI* pElement = new CMenuElementUI;
	pElement->SetAttribute(L"name", szName);
	pElement->SetAttribute(L"width", L"180");
	pElement->SetAttribute(L"height", L"36");
	pElement->SetText(szText);
	if (bSelected) {
		pElement->SetBkImage(L"file='img\\video\\radio_sel.png' dest='4,10,20,26'");
	}
	if (bHasSubMenu) {
		pElement->SetBkImage(L"file='img\\video\\more_menu_arrow.png' dest='156,14,163,22'");
	}

	return pElement;
}

void CVideoRenderControlUI::ConstructMenu(CMenuWnd* pMenu)
{
	// FIXME: xml布局文件中额外增加了一个空的MenuElement作为占位符，否则动态添加的
	// 菜单项无法显示，可能是个bug，暂时没有时间去解决，这里首先要移除占位符
	CMenuUI* pMenuUi = (CMenuUI*)pMenu->m_pm.GetRoot();
	pMenuUi->Remove(pMenu->m_pm.FindControl(L"placeholder"));

	if (m_pVideoPanel->GetVideoId() == fsp::RESERVED_VIDEOID_SCREENSHARE) {
		CDuiString strControlCaption = L"远程控制桌面";
		if (m_RemoteControlState == RemoteControl_Ing) {
			strControlCaption = L"取消控制";
		}
		else if (m_RemoteControlState == RemoteControl_Waiting) {
			strControlCaption = L"等待共享端接受";
		}
		CMenuElementUI* pRemoteControlElement = ConstructMenuItem(L"video_operate_remotecontrol",
			strControlCaption,			
			false,
			false);
		pMenuUi->Add(pRemoteControlElement);
	}
	else {
		// 先添加视频显示模式菜单项
		CMenuElementUI* pSfppElement = ConstructMenuItem(L"video_disp_sfpp",
			L"视频缩放平铺显示",			
			m_pVideoPanel->GetRenderMode() == fsp::RENDERMODE_SCALE_FILL,
			false);
		CMenuElementUI* pDbcjElement = ConstructMenuItem(L"video_disp_dbcj",
			L"视频等比裁剪显示",			
			m_pVideoPanel->GetRenderMode() == fsp::RENDERMODE_CROP_FILL,
			false);
		CMenuElementUI* pDbwzElement = ConstructMenuItem(L"video_disp_dbwz",
			L"视频等比完整显示",			
			m_pVideoPanel->GetRenderMode() == fsp::RENDERMODE_FIT_CENTER,
			false);

		pMenuUi->Add(pSfppElement);
		pMenuUi->Add(pDbcjElement);
		pMenuUi->Add(pDbwzElement);

		fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();
		fsp::Vector<fsp::VideoDeviceInfo> info = pEngin->GetDeviceManager()->GetCameraDevices();

		// 再添加切换摄像头菜单项
		if (info.size() > 1 && m_pVideoPanel->HaveVideo()
			&& m_pVideoPanel->GetLocalPreviewDeviceId() != fsp::INVALID_DEVICE_ID)
		{
			CMenuElementUI* pSwitchElement = ConstructMenuItem(L"cam_switch", L"切换摄像头", false, true);
			
			DWORD dwCamIndex = 0;
			for (auto iter : info)
			{
				WCHAR szOptionName[16];
				_snwprintf_s(szOptionName, 16, L"%s%d", CAMERA_MENU_NAME_PREFIX, dwCamIndex);

				WCHAR szCamName[32];
				demo::ConvertUtf8ToUnicode(iter.device_name.c_str(), szCamName, 32);

				CMenuElementUI* pElement = ConstructMenuItem(szOptionName,
					szCamName,					
					dwCamIndex == m_pVideoPanel->GetLocalPreviewDeviceId(),
					false);

				pSwitchElement->Add(pElement);

				dwCamIndex++;
			}
			pMenuUi->Add(pSwitchElement);
		}
	}


	pMenu->ResizeMenu();
}




///////////////////////////// CVideoRenderUI //////////////////////////////////////////
const TCHAR* CVideoPanelUI::K_UI_INTERFACE_NAME = L"VideoPanel";

CVideoPanelUI::CVideoPanelUI()
{
	m_hVideoWnd = nullptr;
	m_hControlBgWnd = nullptr;
	m_hControlFWnd = nullptr;

	m_pControlBgUi = new CVideoRenderControlUI(true, this);
	m_pControlFUi = new CVideoRenderControlUI(false, this);

	m_eRenderMode = fsp::RENDERMODE_FIT_CENTER;

	memset(&m_lastPosRect, 0, sizeof(m_lastPosRect));

}

CVideoPanelUI::~CVideoPanelUI()
{
	if (m_hVideoWnd) {
		::DestroyWindow(m_hVideoWnd);
	}

	delete m_pControlBgUi;
}

HWND CVideoPanelUI::GetRenderHwnd() const
{
	return m_hVideoWnd;
}

bool CVideoPanelUI::HaveVideo()
{
	if (m_nLocalPreviewDeviceId != fsp::INVALID_DEVICE_ID) {
		return true;
	}

	if (!m_strUserId.empty() && !m_strVideoId.empty()) {
		return true;
	}

	return false;
}

bool CVideoPanelUI::ShowLocalPreview(int nDeviceId)
{
	if (HaveVideo()) {
		return false;
	}

	fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();	

	// 显示本地视频
	fsp::ErrCode errCode = pEngin->AddVideoPreview(nDeviceId, m_hVideoWnd, m_eRenderMode);

	if (errCode != fsp::ERR_OK) {
		return false;
	}

	m_nLocalPreviewDeviceId = nDeviceId;
	m_strUserId = CSdkManager::GetInstance().GetLoginUserId().c_str();

	UpdateVideoPos();
	UpdateVideoRenderVisible();

	m_pControlFUi->SetUserId(demo::Utf82WStr(m_strUserId.c_str()).GetWStr());

	return true;
}

void CVideoPanelUI::StopLocalPreview()
{
	fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();
	pEngin->RemoveVideoPreview(m_nLocalPreviewDeviceId, m_hVideoWnd);
	m_nLocalPreviewDeviceId = fsp::INVALID_DEVICE_ID;

	DoClearVideo();
}

void CVideoPanelUI::SetPublishVideoId(int nDeviceId, const char* szVideoId)
{
	if (nDeviceId == m_nLocalPreviewDeviceId) {
		m_strVideoId = szVideoId;
	}
}

fsp::ErrCode CVideoPanelUI::OpenRemoteVideo(const char* szUserId, const char* szVideoId)
{
	if (HaveVideo()) {
		return ERR_INVALID_ARG;
	}		

	fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();
	fsp::ErrCode errCode = pEngin->SetRemoteVideoRender(szUserId, szVideoId, m_hVideoWnd, m_eRenderMode);

	if (errCode != fsp::ERR_OK) {
		return errCode;
	}

	m_strUserId = szUserId;
	m_strVideoId = szVideoId;

	UpdateVideoPos();
	UpdateVideoRenderVisible();	

	// 显示用户ID
	fsp::VideoStatsInfo statsInfo;
	if (GetLocalPreviewDeviceId() == fsp::INVALID_DEVICE_ID){
		m_pControlFUi->SetUserId(demo::Utf82WStr(m_strUserId.c_str()).GetWStr());
	}

	return ERR_OK;
}

void CVideoPanelUI::CloseRemoteVideo()
{
	fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();
	fsp::ErrCode errCode = pEngin->SetRemoteVideoRender(m_strUserId.c_str(), m_strVideoId.c_str(),
		nullptr, m_eRenderMode);

	DoClearVideo();
}

void CVideoPanelUI::UpdateVideoInfo()
{
	fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();

	CSdkManager::GetInstance().GetLoginUserId();

	fsp::VideoStatsInfo statsInfo;
	if (fsp::ERR_OK == pEngin->GetVideoStats(m_strUserId.c_str(), m_strVideoId.c_str(), &statsInfo))
	{
		WCHAR szVideoStats[32];
		_snwprintf_s(szVideoStats, 32,
			L"%dK %dF %d*%d",
			statsInfo.bitrate / 1024,
			statsInfo.framerate,
			statsInfo.width,
			statsInfo.height);

		m_pControlFUi->UpdateVideoInfo(szVideoStats);
	}
}

int CVideoPanelUI::GetLocalPreviewDeviceId() const
{
	return m_nLocalPreviewDeviceId;
}

void CVideoPanelUI::ChangeRenderMode(fsp::RenderMode mode)
{
	m_eRenderMode = mode;
	if (!HaveVideo()) {
		return;
	}

	fsp::IFspEngine* pEngin = CSdkManager::GetInstance().GetFspEngin();
	if (GetLocalPreviewDeviceId() == fsp::INVALID_DEVICE_ID) {
		pEngin->AddVideoPreview(GetLocalPreviewDeviceId(), GetRenderHwnd(), m_eRenderMode);
	}
	else {
		pEngin->SetRemoteVideoRender(m_strUserId.c_str(), m_strVideoId.c_str(),
			GetRenderHwnd(), m_eRenderMode);
	}
}

void CVideoPanelUI::UpdateVideoPos()
{
	RECT rc = GetClientPos();

	::MoveWindow(m_hVideoWnd, rc.left, rc.top,
		rc.right - rc.left,
		rc.bottom - rc.top, 0);

	POINT pt;
	pt.x = rc.left;
	pt.y = rc.top;
	ClientToScreen(m_pManager->GetPaintWindow(), &pt);

	::MoveWindow(m_hControlBgWnd, pt.x, pt.y, rc.right - rc.left, rc.bottom - rc.top, FALSE);
	::MoveWindow(m_hControlFWnd, pt.x, pt.y, rc.right - rc.left, rc.bottom - rc.top, FALSE);

	m_pControlBgUi->m_PaintManager.SetInitSize(rc.right - rc.left, rc.bottom - rc.top);
	m_pControlFUi->m_PaintManager.SetInitSize(rc.right - rc.left, rc.bottom - rc.top);

	int nControlWidth = rc.right - rc.left;
	int nControlHeight = rc.bottom - rc.top;

	CDuiString strBkImg;
	int nBkImgWidth = 96;
	int nBkImgHeight = 96;
	strBkImg.Format(L"file='img\\main_video_bk_icon.png' dest='%d,%d,%d,%d'",
		nControlWidth/2- nBkImgWidth/2, nControlHeight / 2 - nBkImgHeight / 2,
		nControlWidth / 2 + nBkImgWidth / 2, nControlHeight / 2 + nBkImgHeight / 2);
	SetBkImage(strBkImg);

	m_pControlBgUi->m_PaintManager.Invalidate();
	m_pControlFUi->m_PaintManager.Invalidate();
}

LPCTSTR CVideoPanelUI::GetClass() const
{
	return _T("VideoPanelUI");
}

LPVOID CVideoPanelUI::GetInterface(LPCTSTR pstrName)
{
	if (_tcsicmp(pstrName, K_UI_INTERFACE_NAME) == 0)
	{
		return static_cast<CVideoPanelUI*>(this);
	}
	return CControlUI::GetInterface(pstrName);
}

void CVideoPanelUI::SetInternVisible(bool bVisible)
{
	CControlUI::SetInternVisible(bVisible);
	UpdateVideoRenderVisible();
}

void CVideoPanelUI::SetPos(RECT rc, bool bNeedInvalidate /* = true */)
{
	CControlUI::SetPos(rc, bNeedInvalidate);
	UpdateVideoPos();
}

void CVideoPanelUI::DoInit()
{
	m_hVideoWnd = ::CreateWindow(_T("VideoRenderWnd"), _T("VideoRender"), WS_CHILD ,
		0, 0, 0, 0, m_pManager->GetPaintWindow(), (HMENU)0, NULL, NULL);

	CDuiString strShowVideoInfoVal = GetCustomAttribute(L"show_videoinfo");

	if (strShowVideoInfoVal.CompareNoCase(L"false") != 0) {
		CreateControlWnd(true);
		CreateControlWnd(false);
	}

	SetBkColor(0xFFF0F3F6);
}

void CVideoPanelUI::UpdateVideoRenderVisible()
{
	bool bVisible = IsVisible() && HaveVideo();
	
	if (m_hVideoWnd) {
		::ShowWindow(m_hVideoWnd, bVisible);
	}

	bool isControlVisible = bVisible;

	if (m_hControlBgWnd) {
		::ShowWindow(m_hControlBgWnd, bVisible);
	}
	if (m_hControlFWnd) {
		::ShowWindow(m_hControlFWnd, bVisible);
	}

	this->NeedParentUpdate();
}

void CVideoPanelUI::CreateControlWnd(bool isBkMode)
{
	HWND hNewWnd;
	CVideoRenderControlUI* pControlUI;

	if (isBkMode) {
		pControlUI = m_pControlBgUi;
	}
	else {
		pControlUI = m_pControlFUi;
	}

	hNewWnd = ::CreateWindow(_T("#32770"), _T("VideoRenderControl"), WS_CLIPCHILDREN,
		0, 0, 0, 0, isBkMode ? m_hVideoWnd : m_pControlBgUi->GetHWND(), (HMENU)0, NULL, NULL);
	long dwStyle = GetWindowLong(hNewWnd, GWL_STYLE);
	dwStyle &= ~(WS_CAPTION);
	SetWindowLong(hNewWnd, GWL_STYLE, dwStyle);
	long dwExStyle = GetWindowLong(hNewWnd, GWL_EXSTYLE);
	dwExStyle |= (WS_EX_LAYERED);
	dwExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE | WS_EX_APPWINDOW);
	SetWindowLong(hNewWnd, GWL_EXSTYLE, dwExStyle);

	if (isBkMode) { 
		SetLayeredWindowAttributes(hNewWnd, RGB(0x00, 0x00, 0x00), 125, LWA_COLORKEY | LWA_ALPHA);
	}
	else {
		SetLayeredWindowAttributes(hNewWnd, RGB(0x00, 0x00, 0x00), 0, LWA_COLORKEY);
	}
	
	::SetWindowLongPtr(hNewWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pControlUI));
	pControlUI->Subclass(hNewWnd);
	pControlUI->LoadSkin();

	if (isBkMode) {
		m_hControlBgWnd = hNewWnd;
	}
	else {
		m_hControlFWnd = hNewWnd;
	}
}

void CVideoPanelUI::DoClearVideo()
{
	m_strUserId.clear();
	m_strVideoId.clear();

	m_pControlFUi->ResetVideo();

	UpdateVideoRenderVisible();
}