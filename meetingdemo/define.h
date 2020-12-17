﻿/*##############################################################################
 * 文件：define.h
 * 描述：公共声明和定义
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#pragma once

#include <string>

#include "fsp_engine.h"
#include "doc_define.h"

// 无效值定义
#define INVALID_MIC_INDEX					0xFFFFFFFF
#define INVALID_AUD_INDEX					0xFFFFFFFF
#define INVALID_CAM_INDEX					0xFFFFFFFF
#define INVALID_VOLUME						0xFFFFFFFF

// 内部自定义消息
#define DUILIB_MSG_CMMONINFO                WM_USER + 100
#define DUILIB_MSG_SET_MIC_DEV				WM_USER + 101
#define DUILIB_MSG_SET_AUD_DEV				WM_USER + 102
#define DUILIB_MSG_SET_MIC_VOL				WM_USER + 103
#define DUILIB_MSG_SET_AUD_VOL				WM_USER + 104
#define	DUILIB_MSG_ADD_REMOTE_VIDEO			WM_USER + 105
#define DUILIB_MSG_DEL_REMOTE_VIDEO			WM_USER + 106
#define DUILIB_MSG_ADD_REMOTE_AUDIO			WM_USER + 107
#define DUILIB_MSG_DEL_REMOTE_AUDIO			WM_USER + 108
#define DUILIB_MSG_TOO_MANY_CAM				WM_USER + 109
#define DUILIB_MSG_BROADCAST_CAM_CHANGED	WM_USER + 110
#define DUILIB_MSG_CHANGE_CAM_FAILED		WM_USER + 111
#define	DUILIB_MSG_BROADCAST_CAM_FAILED		WM_USER + 112
#define DUILIB_MSG_CONNECT_LOST             WM_USER + 114
#define DUILIB_MSG_FSP_EVENT				WM_USER + 115
#define DUILIB_MSG_REMOTECONTROL_EVENT      WM_USER + 116
#define DUILIB_MSG_DEVICECHANGE             WM_USER + 117
#define DUILIB_MSG_USER_REFRESH_FINISH      WM_USER + 118
#define DUILIB_MSG_INVITE_COME              WM_USER + 119
#define DUILIB_MSG_INVITE_RESPONSE          WM_USER + 120
#define DUILIB_MSG_RECIVE_CAHTMSG           WM_USER + 121
#define DUILIB_MSG_REMOTE_ADD_USER          WM_USER + 123
#define DUILIB_MSG_REMOTE_DEL_USER          WM_USER + 124
#define DUILIB_MSG_WHITEBOARD_PUBLISH       WM_USER + 125
#define DUILIB_MSG_DOCUMENT_EVENT			WM_USER + 126
#define DUILIB_MSG_WHITEBOARD_PAGE_CHANGE	WM_USER + 127
#define	DUILIB_MSG_WHITEBOARD_SYN_DATA	    WM_USER + 128

// 登录结果
#define LOGIN_SUCCESS		1
#define LOGIN_FAILED		0

// 定时器
#define UPDATE_VIDEO_INFO_TIMER_ID			99
#define UPDATE_VIDEO_INFO_INTERVAL			1000	// ms

#define UPDATE_SETTING_INFO_TIMER_ID			100
#define UPDATE_SETTING_INFO_INTERVAL			200	// ms

// UI
#define UI_VIDEO_INFO_HEIGHT				30
#define UI_VIDEO_WND_HEIGHT					300
#define UI_VIDEO_WND_WIDTH					400
#define UI_MAIN_WND_WIDTH					1200
#define UI_VIDEO_WND_ROW_SIZE				2
#define UI_VIDEO_WND_COLUMN_SIZE			3
#define UI_MAIN_FRAME_TITLE_HEIGHT			30
#define UI_MAIN_FRAME_TOOLBAR_HEIGHT		90

// 字符串
#define STRING_TOO_MANY_CAM					L"最多允许广播2个视频设备！"
#define STRING_CHANGE_CAM_FAILED			L"切换摄像头失败，可能发生了系统错误或者待切换的摄像头已经在使用中！"
#define STRING_BROADCAST_CAM_FAILED			L"广播摄像头失败！"

struct VideoResolution
{
	DWORD dwWidth;
	DWORD dwHeight;
};

static VideoResolution VideoResolutions[] =
{
	{320, 240},
	{480, 360},
	{640, 480},
	{1280, 720},
	{1920, 1080}
};

// 消息结构体
struct RemoteVideoInfo
{
	fsp::String strUserId;
	fsp::String strVideoId;
};

struct RemoteAudioInfo
{
	fsp::String strUserId;
};

struct RemoteControlInfo
{
	fsp::String strUserId;
	fsp::RemoteControlOperationType operationType;
};

struct ScreenShareConfig
{
	fsp::ScreenShareQualityBias qualityBias;
	int left;
	int top;
	int right;
	int bottom;

	ScreenShareConfig()
	{
		qualityBias = fsp::SCREEN_SHARE_BIAS_QUALITY;
		left = 0;
		top = 0;
		right = 0;
		bottom = 0;
	}
};

struct InviteReqInfo
{
	std::string strInviterUserId;
	std::string strGroupId;
	std::string strMsg;
	unsigned int nInviteId;
};

struct InviteResponseInfo
{
	std::string strRemoteUserId;
	unsigned int nInviteId;
	bool isAccept;
};

struct MsgReciveInfo
{
	std::string srcuserid;
	std::string msg;
	bool bGroup;
};

struct RemoteUserEventInfo
{
	std::string remote_userid;
	std::string nick_name;
};

struct RemoteWhiteboardInfo
{
	fsp::String strMediaId;
	fsp::WhiteBoardProfile whiteboard_profile;
	int nPageId;
};

struct WhiteBoardPulishInfo
{
	std::string strWhiteBoardId;
	std::string strWhiteBoardName;
	bool isPublish;
};

struct WhiteboardDocumentInfo
{
	fsp_wb::DocStatusType doc_status_type;
	fsp::ErrCode err_code;
};

struct IEventCallback
{
	virtual void OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
};