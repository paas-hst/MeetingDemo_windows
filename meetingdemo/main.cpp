// fspclient.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "SdkManager.h"
#include "UIlib.h"
#include "ConfigParser.h"

#include <atlbase.h>
CComModule _Module;
#include <atlwin.h>

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	HINSTANCE hInstRich = ::LoadLibrary(_T("Riched20.dll"));

	::CoInitialize(NULL);
	::OleInitialize(NULL);

	_Module.Init(0, hInstance);

#if defined(WIN32) && !defined(UNDER_CE)
	HRESULT Hr = ::CoInitialize(NULL);
#else
	HRESULT Hr = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
#endif
	if (FAILED(Hr)) return 0;

	TCHAR szAppName[] = L"VideoRenderWnd";
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = DefWindowProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = 0;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		return 0;
	}

	DuiLib::CPaintManagerUI::SetInstance(hInstance);
	CWndShadow::Initialize(hInstance);

	if (!demo::CConfigParser::GetInstance().Init())
		return -1;

	if (CSdkManager::GetInstance().Init())
		CSdkManager::GetInstance().OpenLoginWnd();
	else
		return -1;

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	CSdkManager::GetInstance().Destroy();

	_Module.Term();

	::OleUninitialize();
	::CoUninitialize();

	::FreeLibrary(hInstRich);

	return 0;
}
