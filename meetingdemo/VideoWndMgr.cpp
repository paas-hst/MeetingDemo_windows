/*##############################################################################
 * 文件：VideoWndMgr.cpp
 * 描述：负责管理所有视频小窗口
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#include "stdafx.h"
#include "VideoWndMgr.h"
#include "DisplayVideoWnd.h"
#include "fsp_engine.h"
#include "SdkManager.h"
#include "define.h"

/*------------------------------------------------------------------------------
 * 描  述：构造函数
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
CVideoWndMgr::CVideoWndMgr()
{
	ZeroMemory(&m_rectCanvas, sizeof(m_rectCanvas));
	ZeroMemory(m_aVideoWnd, sizeof(m_aVideoWnd));
}

/*------------------------------------------------------------------------------
 * 描  述：析构函数
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
CVideoWndMgr::~CVideoWndMgr()
{
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		if (m_aVideoWnd[i] != nullptr)
			delete m_aVideoWnd[i];
	}
}

/*------------------------------------------------------------------------------
 * 描  述：添加广播摄像头
 * 参  数：[in] dwCamIndex 摄像头索引
 * 返回值：成功/失败
------------------------------------------------------------------------------*/
bool CVideoWndMgr::AddBroadcastCam(DWORD dwCamIndex)
{
	// 是否重复广播视频
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		if (m_aVideoWnd[i]->IsHoldLocalVideo(dwCamIndex))
			return true;
	}

	// 看下是否曾经广播过音频
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		if (m_aVideoWnd[i]->IsHoldLocalAudio()
			&& !m_aVideoWnd[i]->IsHoldLocalVideo())
		{
			return m_aVideoWnd[i]->StartPublishCam(dwCamIndex);
		}
	}

	// 否则寻找一个空闲窗口
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		if (m_aVideoWnd[i]->IsIdle())
		{
			return m_aVideoWnd[i]->StartPublishCam(dwCamIndex);
		}
	}

	return false;
}

/*------------------------------------------------------------------------------
 * 描  述：删除广播的摄像头
 * 参  数：[in] dwCamIndex 摄像头索引
 * 返回值：无
------------------------------------------------------------------------------*/
void CVideoWndMgr::DelBroadcastCam(DWORD dwCamIndex)
{
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		if (m_aVideoWnd[i]->StopPublishCam(dwCamIndex))
			break;
	}
}

/*------------------------------------------------------------------------------
 * 描  述：计算视频小窗口的矩形区域
 * 参  数：[in] bShow 是否显示
 * 返回值：无
------------------------------------------------------------------------------*/
void CVideoWndMgr::CalCFloatWndRect(bool bShow)
{
	// 计算视频窗口显示位置和大小（两行三列）
	DWORD dwWndWidth = (m_rectCanvas.right - m_rectCanvas.left) / UI_VIDEO_WND_COLUMN_SIZE;
	DWORD dwWndHeight = (m_rectCanvas.bottom - m_rectCanvas.top) / UI_VIDEO_WND_ROW_SIZE;

	RECT rect;
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		rect.left	= m_rectCanvas.left + i % UI_VIDEO_WND_COLUMN_SIZE * dwWndWidth;
		rect.top	= m_rectCanvas.top + i / UI_VIDEO_WND_COLUMN_SIZE * dwWndHeight;
		rect.right	= rect.left + dwWndWidth;
		rect.bottom = rect.top + dwWndHeight;

		m_aVideoWnd[i]->SetWndRect(rect, bShow);
	}
}

/*------------------------------------------------------------------------------
 * 描  述：初始化
 * 参  数：[in] hParentWnd 父窗口
 *         [in] rectCanvas 所有视频子窗口的背景窗口矩形区域
 * 返回值：成功/失败
------------------------------------------------------------------------------*/
void CVideoWndMgr::SetWndRect(const RECT& rect)
{
	m_rectCanvas = rect;
	CalCFloatWndRect(true);
}

/*------------------------------------------------------------------------------
 * 描  述：初始化
 * 参  数：[in] hParentWnd 父窗口
 * 返回值：成功/失败
------------------------------------------------------------------------------*/
void CVideoWndMgr::Init(HWND hParentWnd)
{
	m_hParenWnd = hParentWnd;

	// 创建视频窗口
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		m_aVideoWnd[i] = new CDisplayVideoWnd();
		m_aVideoWnd[i]->Init(m_hParenWnd);
		m_aVideoWnd[i]->SetEventCallback(this);
	}
}

/*------------------------------------------------------------------------------
 * 描  述：是否有被双击放大的视频窗口
 * 参  数：无
 * 返回值：是/否
------------------------------------------------------------------------------*/
bool CVideoWndMgr::HasMaximizedWnd()
{
	bool bHasMaxWnd = false;
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		if (m_aVideoWnd[i]->IsVideoMaximized())
		{
			bHasMaxWnd = true;
			break;
		}
	}

	return bHasMaxWnd;
}

/*------------------------------------------------------------------------------
 * 描  述：处理窗口最大化
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
void CVideoWndMgr::MaximizeDisplayWnd()
{
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		if (m_aVideoWnd[i]->IsVideoMaximized())
		{
			RECT rect;
			::GetClientRect(m_hParenWnd, &rect);
			rect.top += UI_MAIN_FRAME_TITLE_HEIGHT;
			rect.bottom -= UI_MAIN_FRAME_TOOLBAR_HEIGHT;

			m_aVideoWnd[i]->SetWndRect(rect, true);
		}
		else
		{
			m_aVideoWnd[i]->ShowWindow(false);
		}
	}
}

/*------------------------------------------------------------------------------
 * 描  述：处理鼠标双击事件
 * 参  数：[in] uMsg	消息类型
 *         [in] wParam	参数
 *         [in] lParam	参数
 * 返回值：无
------------------------------------------------------------------------------*/
void CVideoWndMgr::OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_LBUTTONDBLCLK)
	{
		if (HasMaximizedWnd())
			MaximizeDisplayWnd();
		else
			CalCFloatWndRect(true);
	}
}

/*------------------------------------------------------------------------------
 * 描  述：添加远端用户的某一路视频
 * 参  数：[in] strUserId	用户ID
 *         [in] strVideoId	视频ID
 * 返回值：无
------------------------------------------------------------------------------*/
void CVideoWndMgr::AddRemoteVideo(const fsp::String& strUserId, const fsp::String& strVideoId)
{
	// 重复广播视频
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		// FIXME: 健壮性考虑，比如远端掉线
		if (m_aVideoWnd[i]->IsHoldRemoteVideo(strUserId, strVideoId))
		{
			m_aVideoWnd[i]->AddRemoteVideo(strUserId, strVideoId);
			return;
		}
	}

	// 看下此用户先前有没有广播音频
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		if (m_aVideoWnd[i]->IsHoldRemoteAudio(strUserId)
			&& !m_aVideoWnd[i]->IsHoldRemoteVideo(strUserId))
		{
			m_aVideoWnd[i]->AddRemoteVideo(strUserId, strVideoId);
			return;
		}
	}

	// 否则找一个空闲的窗口
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		if (m_aVideoWnd[i]->IsIdle())
		{
			m_aVideoWnd[i]->AddRemoteVideo(strUserId, strVideoId);
			return;
		}
	}
}

/*------------------------------------------------------------------------------
 * 描  述：删除远端用户的某一路视频
 * 参  数：[in] strUserId	用户ID
 *         [in] strVideoId	视频ID
 * 返回值：无
------------------------------------------------------------------------------*/
void CVideoWndMgr::DelRemoteVideo(const fsp::String& strUserId, const fsp::String& strVideoId)
{
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		if (m_aVideoWnd[i]->IsHoldRemoteVideo(strUserId, strVideoId))
		{
			m_aVideoWnd[i]->DelRemoteVideo(strUserId, strVideoId);
			return;
		}
	}
}

/*------------------------------------------------------------------------------
 * 描  述：添加远端某用户的音频
 * 参  数：[in] strUserId	用户ID
 * 返回值：无
------------------------------------------------------------------------------*/
void CVideoWndMgr::AddRemoteAudio(const fsp::String& strUserId)
{
	// 重复广播音频
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		// FIXME: 健壮性考虑，比如远端掉线
		if (m_aVideoWnd[i]->IsHoldRemoteAudio(strUserId))
		{
			m_aVideoWnd[i]->AddRemoteAudio(strUserId);
			return;
		}		
	}

	// 看下之前是否曾经广播过此用户的视频
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		if (m_aVideoWnd[i]->IsHoldRemoteVideo(strUserId)
			&& !m_aVideoWnd[i]->IsHoldRemoteAudio(strUserId))
		{
			m_aVideoWnd[i]->AddRemoteAudio(strUserId);
			return;
		}
	}

	// 否则找一个空闲的窗口
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		if (m_aVideoWnd[i]->IsIdle())
		{
			m_aVideoWnd[i]->AddRemoteAudio(strUserId);
			return;
		}
	}
}

/*------------------------------------------------------------------------------
 * 描  述：删除远端某用户的音频
 * 参  数：[in] strUserId	用户ID
 * 返回值：无
------------------------------------------------------------------------------*/
void CVideoWndMgr::DelRemoteAudio(const fsp::String& strUserId)
{
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		if (m_aVideoWnd[i]->IsHoldRemoteAudio(strUserId))
		{
			m_aVideoWnd[i]->DelRemoteAudio(strUserId);
			return;
		}
	}
}

/*------------------------------------------------------------------------------
 * 描  述：添加本地广播的麦克风
 * 参  数：[in] dwMicIndex 麦克风索引
 * 返回值：无
------------------------------------------------------------------------------*/
void CVideoWndMgr::AddBroadcastMic(DWORD dwMicIndex)
{
	// 重复广播音频
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		if (m_aVideoWnd[i]->IsHoldLocalAudio(dwMicIndex))
			return;
	}

	// 看下是否曾经广播过视频
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		if (m_aVideoWnd[i]->IsHoldLocalVideo()
			&& !m_aVideoWnd[i]->IsHoldLocalAudio())
		{
			m_aVideoWnd[i]->StartPublishMic(dwMicIndex);
			return;
		}
	}

	// 否则找一个空闲窗口
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		if (m_aVideoWnd[i]->IsIdle())
		{
			m_aVideoWnd[i]->StartPublishMic(dwMicIndex);
			return;
		}
	}
}

/*------------------------------------------------------------------------------
 * 描  述：删除本地广播的麦克风
 * 参  数：[in] dwMicIndex 麦克风索引
 * 返回值：无
------------------------------------------------------------------------------*/
void CVideoWndMgr::DelBroadcastMic(DWORD dwMicIndex)
{
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		if (m_aVideoWnd[i]->StopPublishMic(dwMicIndex))
			return;
	}
}

/*------------------------------------------------------------------------------
 * 描  述：定时器处理
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
void CVideoWndMgr::OnTimer()
{
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		m_aVideoWnd[i]->OnTimer();
	}
}

/*------------------------------------------------------------------------------
 * 描  述：视频参数改变，要重新设置
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
void CVideoWndMgr::OnVideoParamChanged()
{
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		m_aVideoWnd[i]->OnVideoParamChanged();
	}
}