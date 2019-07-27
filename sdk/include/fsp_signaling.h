#pragma once

/**
 * @file fsp_signaling.h
 * @brief fsp sdk信令相关接口和类型定义
 */

#include "fsp_common.h"
#include "fsp_string.h"
#include "fsp_vector.h"

namespace fsp {


/**
 * @brief 用户在线状态类型
 */
enum UserStateType
{
	USER_STATE_OFFLINE = 0,
	USER_STATE_ONLINE = 1
};

//前置声明
class IFspEngineEventHandler;


struct UserInfo
{
	String user_id;
	UserStateType state;

	UserInfo()
	{
		state = USER_STATE_OFFLINE;
	}
};


/**
 * @brief sdk回调事件和异步结果的接口
 *
 *上层实现该接口并通过IFspEngine::SetEngineListener设置给sdk
 */
class IFspSignalingEventHandler {
public:

	/**
	 * @brief IFspSignaling::UserStatusRefresh 的结束回调
	 */
	virtual void OnUsersStateRefreshed(ErrCode err_code,
		unsigned int nRequestId, const Vector<UserInfo> users) = 0;

	/**
	 * @brief 收到邀请
	 */
	virtual void OnInviteCome(const char* inviter_usr_id,
		unsigned int invite_id, const char* group_id, const char* msg) = 0;

	/**
	 * @brief 邀请被接受通知
	 */
	virtual void OnInviteAccepted(const char* remote_user_id, unsigned int invite_id) = 0;

	/**
	 * @brief 邀请被拒绝通知
	 */
	virtual void OnInviteRejected(const char* remote_user_id, unsigned int invite_id) = 0;

	/**
	 * @brief 收到组外用户发的消息
	 */
	virtual void OnUserMsgCome(const char* sender_user_id, unsigned int msg_id, const char* msg) = 0;

	/**
	 * @brief 收到组内用户发的消息
	 */
	virtual void OnGroupMsgCome(const char* sender_user_id, unsigned int msg_id, const char* msg) = 0;
};


/**
 * @brief sdk对外的信令接口. 包含各种信令业务。在线邀请，透明消息等。通过 IFspEngine::GetFspSignaling 获取到
 * 
 */
class IFspSignaling {
public:

	/**
	 * @brief 添加信令业务回调对象，一般初始化时调用添加
	 */
	virtual void AddEventHandler(IFspSignalingEventHandler* event_handler) = 0;

	/**
	 * @brief 移除信令业务回调对象，一般destory SDK时调用
	 */
	virtual void RemoveEventHandler(IFspSignalingEventHandler* event_handler) = 0;


	/**
	 * @brief 请求刷新用户列表
	 */
	virtual ErrCode UserStatusRefresh(const Vector<String>& user_ids, unsigned int* out_request_id) = 0;

	/**
	 * @brief 邀请用户加入组，对方将收到 OnInviteCome 回调
	 * @param user_ids 需要邀请的userid列表
	 * @param group_id 邀请加入哪个组
	 * @param msg 邀请时顺带的一个消息
	 * @param out_invite_id 邀请id, 输出参数，指向的值将被赋值为一个唯一标识本地发出的邀请。后续的接收或拒绝事件将包含这个邀请id
	 */
	virtual ErrCode Invite(const Vector<String>& user_ids, const char* group_id, const char* msg, unsigned int* out_invite_id) = 0;

	/**
	 * @brief 接受一个邀请
	 *
	 * 接受后，对方将收到 OnInviteAccepted 回调。 接受邀请，一般会马上调用 IFspEngine::JoinGroup 去加入组
	 * @param inviter_user_id 邀请方的userid
	 * @param invite_id 邀请方对应的邀请id, 来自 IFspSignalingEventHandler::OnInviteCome
	 */
	virtual ErrCode AcceptInvite(const char* inviter_user_id, unsigned int invite_id) = 0;

	/**
	 * @brief 接受一个邀请，对方将收到 OnInviteRejected 回调
	 * @param inviter_user_id 邀请方的userid
	 * @param invite_id 邀请方对应的邀请id, 来自 IFspSignalingEventHandler::OnInviteCome
	 */
	virtual ErrCode RejectInvite(const char* inviter_user_id, unsigned int invite_id) = 0;

	/**
	 * @brief 发送消息给指定用户，对方将收到 OnUserMsgCome 回调
	 */
	virtual ErrCode SendUserMsg(const char* user_id, const char* msg, unsigned int* out_msg_id) = 0;

	/**
	 * @brief 发送消息给组内用户，对方将收到 OnGroupMsgCome 回调
	 */
	virtual ErrCode SendGroupMsg(const char* msg, unsigned int* out_msg_id) = 0;

	/**
	 * @brief 发送消息给组内用户，对方将收到 OnGroupMsgCome 回调
	 */
	virtual ErrCode SendGroupMsgWithWhiteList(const Vector<String>& white_user_ids, const char* msg, unsigned int* out_msg_id) = 0;

	/**
	 * @brief 发送消息给组内用户，对方将收到 OnGroupMsgCome 回调
	 */
	virtual ErrCode SendGroupMsgWithBlackList(const Vector<String>& black_user_ids, const char* msg, unsigned int* out_msg_id) = 0;
};

} // namespace fsp

