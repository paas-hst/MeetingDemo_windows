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


CVideoWndMgr::CVideoWndMgr()
{
	ZeroMemory(&m_rectCanvas, sizeof(m_rectCanvas));
	ZeroMemory(m_aVideoWnd, sizeof(m_aVideoWnd));
}


CVideoWndMgr::~CVideoWndMgr()
{
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		if (m_aVideoWnd[i] != nullptr)
			delete m_aVideoWnd[i];
	}
}


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


void CVideoWndMgr::DelBroadcastCam(DWORD dwCamIndex)
{
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		if (m_aVideoWnd[i]->StopPublishCam(dwCamIndex))
			break;
	}
}


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


void CVideoWndMgr::SetWndRect(const RECT& rect)
{
	m_rectCanvas = rect;
	
	if (HasMaximizedWnd())
		MaximizeDisplayWnd();
	else
		CalCFloatWndRect(true);
}


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


void CVideoWndMgr::DelBroadcastMic(DWORD dwMicIndex)
{
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		if (m_aVideoWnd[i]->StopPublishMic())
			return;
	}
}


void CVideoWndMgr::OnTimer()
{
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		m_aVideoWnd[i]->OnTimer();
	}
}

void CVideoWndMgr::OnRemoteControlOperation(const fsp::String& user_id, fsp::RemoteControlOperationType operationType)
{
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		if (m_aVideoWnd[i]->IsHoldRemoteVideo(user_id, fsp::RESERVED_VIDEOID_SCREENSHARE)) {
			m_aVideoWnd[i]->OnRemoteControlOperation(operationType);
			break;
		}
	}
}

void CVideoWndMgr::DelAllAV()
{
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		m_aVideoWnd[i]->DelCurrentAV();
	}
}


void CVideoWndMgr::OnVideoParamChanged()
{
	for (int i = 0; i < VIDEO_WND_COUNT; i++)
	{
		m_aVideoWnd[i]->OnVideoParamChanged();
	}
}