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
	m_config_file = T2A(module_path) + std::string("appinfo.xml");

	tinyxml2::XMLDocument doc;
	doc.LoadFile(m_config_file.c_str());
	if (doc.ErrorID() != 0)
		return false;

	// AppId
	const char* szAppId = doc.FirstChildElement("AppId")->GetText();
	if (!szAppId) 
		return false;
	else
		m_ClientConfig.strAppId = szAppId;
	
	// AppSecret
	const char* szAppSecret = doc.FirstChildElement("AppSecret")->GetText();
	if (!szAppSecret)
		return false;
	else
		m_ClientConfig.strAppSecret = szAppSecret;

	// ServerAddr，可以为空
	tinyxml2::XMLElement* serverAddrElement = doc.FirstChildElement("ServerAddr");
	const char* szServerAddr = serverAddrElement ? serverAddrElement->GetText() : "";
	if (!szServerAddr)
		m_ClientConfig.strServerAddr = "";
	else
		m_ClientConfig.strServerAddr = szServerAddr;

	// UserDefine
	m_ClientConfig.bUserDefine = doc.FirstChildElement("UserDefine")->BoolText();

	// UserAppId
	const char* szUserAppId = doc.FirstChildElement("UserAppId")->GetText();
	if (!szUserAppId)
		m_ClientConfig.strUserAppId = "";
	else
		m_ClientConfig.strUserAppId = szUserAppId;

	// UserAppSecret
	const char* szUserAppSecret = doc.FirstChildElement("UserAppSecret")->GetText();
	if (!szUserAppSecret)
		m_ClientConfig.strUserAppSecret = "";
	else
		m_ClientConfig.strUserAppSecret = szUserAppSecret;

	return true;
}

void CConfigParser::Serialize()
{
	tinyxml2::XMLDocument doc;
	doc.Parse("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>");

	tinyxml2::XMLComment* pAppIdComment = doc.NewComment("Default app ID");
	doc.LinkEndChild(pAppIdComment);

	tinyxml2::XMLElement* pAppId = doc.NewElement("AppId");
	pAppId->SetText(m_ClientConfig.strAppId.c_str());
	doc.LinkEndChild(pAppId);

	tinyxml2::XMLComment* pAppSecretComment = doc.NewComment("Default app secret");
	doc.LinkEndChild(pAppSecretComment);

	tinyxml2::XMLElement* pAppSecret = doc.NewElement("AppSecret");
	pAppSecret->SetText(m_ClientConfig.strAppSecret.c_str());
	doc.LinkEndChild(pAppSecret);

	tinyxml2::XMLComment* pUserAppComment = doc.NewComment("Whether use user-defined app info");
	doc.LinkEndChild(pUserAppComment);

	tinyxml2::XMLElement* pUserDefine = doc.NewElement("UserDefine");
	pUserDefine->SetText(m_ClientConfig.bUserDefine ? "true" : "false");
	doc.LinkEndChild(pUserDefine);
	
	tinyxml2::XMLComment* pUserAppIdComment = doc.NewComment("User-defined app ID");
	doc.LinkEndChild(pUserAppIdComment);

	tinyxml2::XMLElement* pUserAppId = doc.NewElement("UserAppId");
	pUserAppId->SetText(m_ClientConfig.strUserAppId.c_str());
	doc.LinkEndChild(pUserAppId);

	tinyxml2::XMLComment* pUserAppSecretComment = doc.NewComment("User-defined app secret");
	doc.LinkEndChild(pUserAppSecretComment);

	tinyxml2::XMLElement* pUserAppSecret = doc.NewElement("UserAppSecret");
	pUserAppSecret->SetText(m_ClientConfig.strUserAppSecret.c_str());
	doc.LinkEndChild(pUserAppSecret);

	if (!m_ClientConfig.strServerAddr.empty()) 
	{
		tinyxml2::XMLComment* pServerAddrComment = doc.NewComment("Access service address");
		doc.LinkEndChild(pServerAddrComment);

		tinyxml2::XMLElement* pServerAddr = doc.NewElement("ServerAddr");
		pServerAddr->SetText(m_ClientConfig.strServerAddr.c_str());
		doc.LinkEndChild(pServerAddr);
	}

	doc.SaveFile(m_config_file.c_str());
}
