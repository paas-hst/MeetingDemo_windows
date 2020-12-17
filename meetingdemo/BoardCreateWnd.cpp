#include "stdafx.h"
#include <string>
#include <sstream>
#include "util.h"
#include "CommDlg.h"
#include "BoardCreateWnd.h"
#include "SdkManager.h"
#include "define.h"

CBoardCreateWnd::CBoardCreateWnd()
{
}

CBoardCreateWnd::~CBoardCreateWnd()
{
}

CDuiString CBoardCreateWnd::GetSkinFolder()
{
	return CDuiString(L"skin");
}

CDuiString CBoardCreateWnd::GetSkinFile()
{
	return CDuiString(L"board_create_wnd.xml");
}

LPCTSTR CBoardCreateWnd::GetWindowClassName(void) const
{
	return L"CBoardCreateWnd";
}

void CBoardCreateWnd::OnClick(TNotifyUI & msg)
{
	if (msg.pSender->GetName() == L"btn_board_blank")
	{
		static int count = 0;
		count++;
		string whiteboardname = CSdkManager::GetInstance().GetLoginUserId() + ":Blank";
		ostringstream oss;
		oss << whiteboardname << count;

		fsp::WhiteBoardProfile whiteboard_profile;
		whiteboard_profile.boardtype = WHITE_BOARD_BLANK;
		whiteboard_profile.boardname = oss.str().c_str();
		whiteboard_profile.width = 1280;
		whiteboard_profile.height = 720;
		whiteboard_profile.page = 1;
		ErrCode hr = CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->Create(whiteboard_profile);
		if (hr == ERR_OK) {

		}
	}
	if (msg.pSender->GetName() == L"btn_board_document")
	{
		TCHAR szBuffer[MAX_PATH] = { 0 };
		OPENFILENAME ofn = { 0 };
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = m_hWnd;
		//ofn.lpstrFilter = _T("jpg文件(*.jpg)\0*.jpg\0所有文件(*.*)\0*.*\0");//要选择的文件后缀
		ofn.lpstrFilter = _T("文件:*.ppt *.pptx *.xls *.xlsx *.xlsm *.doc *.docx *.pdf *.jpg *.jpeg *.png *.bmp\
			\0*.ppt;*.pptx;*.xls;*.xlsx;*.xlsm;*.doc;*.docx;*.pdf;*.jpg;*.jpeg;*.png;*.bmp\0所有文件(*.*)\0*.*\0");
		ofn.lpstrInitialDir = _T("D:\\Program Files");//默认的文件路径
		ofn.lpstrFile = szBuffer;//存放文件的缓冲区
		ofn.nMaxFile = sizeof(szBuffer) / sizeof(*szBuffer);
		ofn.nFilterIndex = 0;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR;
		BOOL bSel = GetOpenFileName(&ofn);

		if (bSel)
		{
			static int count = 0;
			count++;
			string whiteboardname = CSdkManager::GetInstance().GetLoginUserId() + ":Document";
			ostringstream oss;
			oss << whiteboardname << count;

			char *pchar = new char[MAX_PATH * 4];
			ZeroMemory(pchar, MAX_PATH * 4 * sizeof(char));
			demo::ConvertUnicodeToUtf8(szBuffer, pchar, MAX_PATH * 4);

			string boardfilePath = pchar;

			size_t a = boardfilePath.find_last_of('\\');
			size_t b = boardfilePath.find_last_of('.');

			string boardfilename = '/'+ CSdkManager::GetInstance().GetLoginGroupId() +
				'/'+ boardfilePath.substr(a + 1, b - a - 1);

			WhiteBoardProfile whiteboard_profile;
			whiteboard_profile.boardtype = WHITE_BOARD_DOCUMENT;
			whiteboard_profile.boardname = boardfilePath.substr(a + 1, b - a - 1).c_str();//oss.str().c_str();
			whiteboard_profile.filePath = boardfilePath.c_str();

			ErrCode hr = CSdkManager::GetInstance().GetFspEngin()->GetFspWhiteBoard()->Create(whiteboard_profile);
			if (hr == ERR_OK) {

			}
			delete[] pchar;
		}
	}
	this->ShowWindow(false);
}

LRESULT CBoardCreateWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KILLFOCUS:
	{
		this->ShowWindow(false);
	}break;
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}