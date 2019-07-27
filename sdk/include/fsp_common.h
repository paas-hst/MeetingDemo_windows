#pragma once

/**
 * @file fsp_common.h
 * @brief fsp sdk公共基础定义
 */

namespace fsp {

/**
 * @brief 错误码集合
 */
enum ErrCode {
	ERR_OK = 0, ///<成功

	ERR_INVALID_ARG = 1,      ///<非法参数
	ERR_INVALID_STATE = 2,    ///<非法状态
	ERR_OUTOF_MEMORY = 3,     ///<内存不足
	ERR_DEVICE_FAIL = 4,      ///<访问设备失败

	ERR_CONNECT_FAIL = 30,     ///<网络连接失败
	ERR_NO_GROUP = 31,         ///<没加入组
	ERR_TOKEN_INVALID = 32,    ///<认证失败
	ERR_APP_NOT_EXIST = 33,    ///<app不存在，或者app被删除
	ERR_USERID_CONFLICT = 34,  ///<相同userid已经加入同一个组，无法再加入

	ERR_NO_BALANCE = 70,         ///<账户余额不足
	ERR_NO_VIDEO_PRIVILEGE = 71, ///<没有视频权限
	ERR_NO_AUDIO_PRIVILEGE = 72, ///<没有音频权限

	ERR_NO_SCREEN_SHARE = 73,    ///<当前没有屏幕共享

	ERR_RECVING_SCREEN_SHARE = 74,   ///<当前正在接收屏幕共享

	ERR_SERVER_ERROR = 301,	    ///服务内部错误
	ERR_FAIL = 302              ///<操作失败
};

}//namespace fsp

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define FSP_CALL __cdecl
#if defined(FSP_EXPORT)
#define FSP_API extern "C" __declspec(dllexport)
#else
#define FSP_API extern "C" __declspec(dllimport)
#endif
#elif defined(__APPLE__)
#define FSP_API __attribute__((visibility("default"))) extern "C"
#define FSP_CALL
#elif defined(__ANDROID__) || defined(__linux__)
#define FSP_API extern "C" __attribute__((visibility("default")))
#define FSP_CALL
#else
#define FSP_API extern "C"
#define FSP_CALL
#endif

#include <assert.h>

#define	FSP_MININUM_STRING_CAPACITY	32

FSP_API void* FSP_CALL	FspMalloc(size_t size);
FSP_API void  FSP_CALL	FspFree(void* block);
FSP_API void  FSP_CALL	FspMemCpy(void* dst, const void* src, int n);
FSP_API void  FSP_CALL	FspMemMov(void* dst, const void* src, int n);
FSP_API int	  FSP_CALL	FspStrLen(const char* s);
FSP_API int	  FSP_CALL	FspStrCmp(const char* lhs, const char* rhs);