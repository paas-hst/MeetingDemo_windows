// fspclient.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "meetingdemo.h"
#include "SdkManager.h"
#include "UIlib.h"


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	DuiLib::CPaintManagerUI::SetInstance(hInstance);

	if (CSdkManager::GetInstance().Init())
		CSdkManager::GetInstance().Start();

	CoUninitialize();

	return 0;
}
