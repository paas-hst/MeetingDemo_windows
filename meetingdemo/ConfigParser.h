/*##############################################################################
 * 文件：ConfigParser.h
 * 描述：配置文件解析器声明
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#pragma once

#include <string>


namespace demo
{

struct ClientConfig
{
	bool bAppUserDefine;

	std::string strAppId;
	std::string strAppSecret;
	std::string strUserAppId;
	std::string strUserAppSecret;

	bool bServerUserDefine;

	std::string strServerAddr;
	std::string strUserServerAddr;

	bool bForceLogin;

	std::string strRecvVoiceVariant;
};

class CConfigParser
{
public:
	static CConfigParser& GetInstance();

	bool Init();
	void Serialize();

	ClientConfig& GetClientConfig() { return m_ClientConfig; }

private:
	// 不允许拷贝和赋值
	CConfigParser() {}
	CConfigParser(const CConfigParser&) {}
	CConfigParser& operator=(const CConfigParser&) {}

private:
	ClientConfig m_ClientConfig;
	std::string m_ConfigFile;
};

}