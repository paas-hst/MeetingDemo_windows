/*##############################################################################
 * �ļ���util.h
 * ���������ߺ����������Ͷ���
 * ���ߣ�Teck
 * ʱ�䣺2018��5��24��
 * ��Ȩ��Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#pragma once

#include "StdAfx.h"
#include "MessageBox.h"

namespace demo
{
/*------------------------------------------------------------------------------
 * ��  ������Unicode��UTF-8���ַ���ת��
 * ��  ����[in] wszIn	Unicode�ַ���
 *         [out] szOut	UTF-8�ַ���
 *         [in] nSize	��������С
 * ����ֵ��ת�����ֽ���
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
 * ��  ������UTF-8��Unicode���ַ���ת��
 * ��  ����[in] wszIn	UTF-8�ַ���
 *         [out] szOut	Unicode�ַ���
 *         [in] nSize	��������С
 * ����ֵ��ת�����ֽ���
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
 * ��  ������ʾ��ʾ�Ի���
 * ��  ����[in] hParent	������
 *         [in] szText	��ʾ���ı�
 * ����ֵ����
------------------------------------------------------------------------------*/
inline void ShowMessageBox(HWND hParent, LPCTSTR szText)
{
	CMessageBox* pMB = new CMessageBox;

	pMB->Create(hParent, _T("CDuiLoginWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pMB->SetText(szText);
		
	pMB->CenterWindow();
	pMB->ShowModal();
}

class WStr2Utf8
{
public:
	WStr2Utf8(const WCHAR* wsz_str)
	{
		m_str = NULL;
		if (wsz_str) {
			DWORD nLen = 0;

			nLen = WideCharToMultiByte(CP_UTF8, 0, wsz_str, -1, NULL, 0, NULL, NULL);
			m_str = new char[nLen + 8];
			memset(m_str, 0, nLen + 8);

			nLen = WideCharToMultiByte(CP_UTF8, 0, wsz_str, -1, m_str, nLen + 8, NULL, NULL);
		}
	}

	~WStr2Utf8()
	{
		if (m_str) {
			delete[] m_str;
		}
	}

	const char* GetUtf8Str() const
	{
		return m_str;
	}

private:
	char* m_str;
};


class Utf82WStr
{
public:
	Utf82WStr(const char* szStr)
	{
		Init(szStr);
	}

	Utf82WStr(const std::string& str)
	{
		Init(str.c_str());
	}

	~Utf82WStr()
	{
		if (m_wstr) {
			delete[] m_wstr;
		}
	}

	const WCHAR* GetWStr() const
	{
		return m_wstr;
	}

private:
	void Init(const char* szStr) {
		m_wstr = NULL;
		if (szStr) {
			DWORD nLen = 0;
			nLen = MultiByteToWideChar(CP_UTF8, 0, szStr, -1, NULL, 0);

			m_wstr = new WCHAR[nLen + 8];
			memset(m_wstr, 0, nLen + 8);

			nLen = MultiByteToWideChar(CP_UTF8, 0, szStr, -1, m_wstr, nLen);
		}
	}

	WCHAR* m_wstr;
};

}