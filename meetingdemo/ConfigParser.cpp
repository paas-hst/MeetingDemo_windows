/*##############################################################################
 * 文件：ConfigParser.cpp
 * 描述：配置文件解析器实现
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#include "stdafx.h"
#include "ConfigParser.h"
#include <atlconv.h>
#include "tinyxml2.h"

using namespace demo;

#define MAX_CHAR_SIZE 128

namespace
{

TCHAR* GetModulePath(HMODULE hModule)
{
	TCHAR  szPath[MAX_PATH];
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];

	GetModuleFileName(hModule, szPath, MAX_PATH);

	_tsplitpath(szPath, szDrive, szDir, NULL, NULL);

	_stprintf(szPath, _T("%s%s"), szDrive, szDir);

	static TCHAR  szModuelPath[MAX_PATH];

	return _tcscpy(szModuelPath, szPath);
}

}

/*------------------------------------------------------------------------------
 * 描  述：静态方法，获取静态实例
 * 参  数：无
 * 返回值：配置解析器实例引用
 ------------------------------------------------------------------------------*/
CConfigParser& CConfigParser::GetInstance()
{
	static CConfigParser configer;
	return configer;
}

/*------------------------------------------------------------------------------
 * 描  述：初始化
 * 参  数：无
 * 返回值：成功/失败
 ------------------------------------------------------------------------------*/
bool CConfigParser::Init()
{
	WCHAR* module_path = GetModulePath(NULL);

	USES_CONVERSION;
	std::string strConfigFile = T2A(module_path) + std::string("appinfo.xml");

	tinyxml2::XMLDocument doc;
	doc.LoadFile(strConfigFile.c_str());
	if (doc.ErrorID() != 0)
		return false;

	const char* szAppId = doc.FirstChildElement("AppId")->GetText();
	if (!szAppId) 
		return false;
	else
		m_ClientConfig.strAppId = szAppId;

	tinyxml2::XMLElement* serverAddrElement = doc.FirstChildElement("ServerAddr");
	const char* szServerAddr = serverAddrElement ? serverAddrElement->GetText() : "";
	if (!szServerAddr) // 允许不配置ServerAddr
		m_ClientConfig.strServerAddr = "";
	else
		m_ClientConfig.strServerAddr = szServerAddr;
	
	const char* szAppSecret = doc.FirstChildElement("AppSecret")->GetText();
	if (!szAppSecret)
		return false;
	else
		m_ClientConfig.strAppSecret = szAppSecret;

	return true;
}

