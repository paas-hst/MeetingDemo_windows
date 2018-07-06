/*##############################################################################
 * 文件：util.h
 * 描述：工具函数的声明和定义
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#pragma once

#include "StdAfx.h"
#include "MessageBox.h"

namespace demo
{
/*------------------------------------------------------------------------------
 * 描  述：从Unicode到UTF-8的字符串转换
 * 参  数：[in] wszIn	Unicode字符串
 *         [out] szOut	UTF-8字符串
 *         [in] nSize	缓冲区大小
 * 返回值：转换的字节数
------------------------------------------------------------------------------*/
inline DWORD ConvertUnicodeToUtf8(const WCHAR * wszIn, CHAR *szOut, DWORD nSize)
{
	DWORD nLen = 0;

	nLen = WideCharToMultiByte(CP_UTF8, 0, wszIn, -1, NULL, 0, NULL, NULL);
	if (nSize <= nLen)
		return 0;
	nLen = WideCharToMultiByte(CP_UTF8, 0, wszIn, -1, szOut, nSize, NULL, NULL);
	szOut[nLen] = 0;

	return nLen;
}

/*------------------------------------------------------------------------------
 * 描  述：从UTF-8到Unicode的字符串转换
 * 参  数：[in] wszIn	UTF-8字符串
 *         [out] szOut	Unicode字符串
 *         [in] nSize	缓冲区大小
 * 返回值：转换的字节数
------------------------------------------------------------------------------*/
inline DWORD ConvertUtf8ToUnicode(const CHAR * szIn, WCHAR *wszOut, DWORD nSize)
{
	DWORD nLen = 0;
	nLen = MultiByteToWideChar(CP_UTF8, 0, szIn, -1, NULL, 0);
	if (nSize <= nLen)
		return 0;
	nLen = MultiByteToWideChar(CP_UTF8, 0, szIn, -1, wszOut, nSize);
	wszOut[nLen] = 0;
	return nLen;
}

/*------------------------------------------------------------------------------
 * 描  述：显示提示对话框
 * 参  数：[in] hParent	父窗口
 *         [in] szText	显示的文本
 * 返回值：无
------------------------------------------------------------------------------*/
inline void ShowMessageBox(HWND hParent, LPCTSTR szText)
{
	CMessageBox* pMB = new CMessageBox;

	pMB->Create(hParent, _T("CDuiLoginWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pMB->SetText(szText);
		
	pMB->CenterWindow();
	pMB->ShowModal();
}
}