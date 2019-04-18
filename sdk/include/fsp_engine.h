#pragma once

/**
 * @file fsp_engine.h
 * @brief fsp sdk主要头文件
 */

#include "fsp_common.h"
#include "fsp_string.h"
#include "fsp_vector.h"

namespace fsp {

static const int INVALID_DEVICE_ID = -1; ///<非法的设备id
///音频参数值
static const int AUDIOPARAM_VALUE_ENABLE = 1;  ///<某个音频参数开启
static const int AUDIOPARAM_VALUE_DISABLE = 0; ///<某个音频参数关闭


//官方保留的videoid, 代表特定类型的广播，广播时不能取这些值。
static const char* RESERVED_VIDEOID_SCREENSHARE = "reserved_videoid_screenshare";

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

/**
 * @brief 音频参数key
 */
enum AudioParamKey {
  AUDIOPARAM_SPEAKER_VOLUME = 1,    ///<扬声器音量 取值 0-100
  AUDIOPARAM_MICROPHONE_VOLUME = 2, ///<麦克风音量 取值 0-100
  AUDIOPARAM_SPEAKER_MUTE =
      3, ///<扬声器静音 取值 AUDIOPARAM_VALUE_ENABLE 或 AUDIOPARAM_VALUE_DISABLE
  AUDIOPARAM_MICROPHONE_MUTE =
      4, ///<麦克风静音 取值 AUDIOPARAM_VALUE_ENABLE 或 AUDIOPARAM_VALUE_DISABLE
  AUDIOPARAM_SPEAKER_ENERGY = 5,   ///<扬声器能量值，只读 取0-100
  AUDIOPARAM_MICROPHONE_ENERGY = 6 ///<麦克风能量值, 只读 取0-100
};

/**
 * @brief IFspEngineEventHandler.OnEvent回调的事件类型
 */
enum EventType {
  EVENT_JOINGROUP_RESULT = 0, ///<加入组结果
  EVENT_CONNECT_LOST = 1,     ///<与fsp服务的连接断开，应用层需要去重新加入组
  EVENT_RECONNECT_START = 2   ///<网络断开过，开始重连
};

/**
 * @brief IFspEngineEventHandler.OnDeviceChange回调的事件类型
 */
enum DeviceEventType {
  DEVICEEVENT_CAMERA_ADDED = 1,      ///<新插入了摄像头设备
  DEVICEEVENT_CAMERA_REMOVED = 2,    ///<拔掉了摄像头设备
  DEVICEEVENT_SPEAKER_ADDED = 3,     ///<新插入了扬声器设备
  DEVICEEVENT_SPEAKER_REMOVED = 4,   ///<拔掉了扬声器设备
  DEVICEEVENT_MICROPHONE_ADDED = 5,  ///<新插入了麦克风设备
  DEVICEEVENT_MICROPHONE_REMOVED = 6 ///<拔掉了麦克风设备
};

/**
 * @brief IFspEngineEventHandler.OnRemoteAudioEvent回调的事件类型
 */
enum RemoteAudioEventType {
  REMOTE_AUDIO_EVENT_PUBLISHE_STARTED = 0, ///<远端广播了音频
  REMTOE_AUDIO_EVENT_PUBLISHE_STOPED = 1   ///<远端停止广播音频
};

/**
 * @brief 视频显示缩放模式
 */
enum RenderMode {
  RENDERMODE_SCALE_FILL = 1, ///<缩放平铺
  RENDERMODE_CROP_FILL = 2,  ///<等比裁剪显示
  RENDERMODE_FIT_CENTER = 3  ///<等比居中显示
};

/**
 * @brief IFspEngineEventHandler.OnRemoteVideoEvent回调的事件类型
 */
enum RemoteVideoEventType {
  REMOTE_VIDEO_EVENT_PUBLISHE_STARTED = 0, ///<远端广播了一路视频
  REMOTE_VIDEO_EVENT_PUBLISHE_STOPED = 1,  ///<远端视频停止广播
  REMOTE_VIDEO_EVENT_FIRST_RENDERED = 2 ///<远端视频第一次显示,加载完成的事件
};


//前置声明
class IFspEngineEventHandler;

/**
 * @brief sdk初始化时需要的信息
 */
struct FspEngineContext {
  IFspEngineEventHandler *event_handler; ///<外部实现的事件回调对象
  String app_id;						///< appid 由fsp平台分配的应用id
  String log_path;						///<日志目录，如果不填，默认程序所在目录
  String server_addr;					///<服务地址，ip或域名加端口， 格式： "127.0.0.1:50002"，
										///<如果不填，默认使用好视通的 paas
  bool auto_recv_audio;					///<是否自动接收远端音频
  bool auto_play_audio;					///<是否自动播放远端音频

  FspEngineContext() 
	  : event_handler(NULL)
	  , app_id("")
	  , log_path("./")
	  , server_addr("")
	  , auto_recv_audio(true)
	  , auto_play_audio(true)
  {}
};

/**
 * @brief 音频设备信息
 */
struct AudioDeviceInfo {
  int device_id;      ///<设备唯一id，支持热拔插
  String device_name; ///<设备名
};

/**
 * @brief 视频设备信息
 */
struct VideoDeviceInfo {
  int camera_id;      ///<设备唯一id，支持热拔插
  String device_name; ///<设备名
};

/**
 * @brief 远端视频统计信息
 */
struct VideoStatsInfo {
  int width;     ///<图像宽度，像素
  int height;    ///<图像高度，像素
  int framerate; ///<帧率
  int bitrate;   ///<码率

  VideoStatsInfo() {
    width = 0;
    height = 0;
    framerate = 0;
    bitrate = 0;
  }
};

/**
 * @brief 视频profile, 当设备或网络不支持时，会取最接近最大值的那个值
 * 
 * 常见宽高：320x240, 640x360, 640x480, 1280x720, 1920x1080；
 * 常见帧率： 1, 7, 10, 15, 24, 30
 */
struct VideoProfile
{
	int width;     ///<视频宽
	int height;    ///<视频高
	int framerate; ///<帧率

	VideoProfile() {
		width = 0;
		height = 0;
		framerate = 0;
	}
};

/**
 * @brief 屏幕共享质量偏好设置
 */
enum ScreenShareQualityBias
{
	SCREEN_SHARE_BIAS_QUALITY = 0, ///<偏重画面质量，适合共享文档等静态桌面
	SCREEN_SHARE_BIAS_SPEED = 1    ///<偏重帧率速度，适合视频等动态桌面
};

/**
 * 远程控制操作
 */
enum RemoteControlOperationType
{
	REMOTE_CONTROL_REQUEST = 0, ///<查看端请求远程控制桌面
	REMOTE_CONTROL_CANCEL = 1,  ///<查看端取消远程控制桌面
	REMOTE_CONTROL_ACCEPT = 2,  ///<广播端同意远程控制请求
	REMOTE_CONTROL_REJECT = 3   ///<广播端拒绝远程控制请求
};

/**
 * @brief 本地设备管理，包括音视频设备
 *
 * 通过 IFspEngine.GetDeviceManager
 * 获取，对象属于IFspEngine，不需要去关注对象声明周期
 */
class IDeviceManager {
public:
  /**
   * @brief 获取音频麦克风设备列表
   * @return 麦克风设备信息的集合
   */
  virtual Vector<AudioDeviceInfo> GetMicrophoneDevices() = 0;

  /**
   * @brief 获取音频扬声器设备列表
   * @return 扬声器设备信息的集合
   */
  virtual Vector<AudioDeviceInfo> GetSpeakerDevices() = 0;

  /**
   * @brief 获取摄像头设备列表
   * @return 摄像头设备信息集合
   */
  virtual Vector<VideoDeviceInfo> GetCameraDevices() = 0;
};

/**
 * @brief 音频引擎，全局的音频系统参数控制
 *
 * 通过 IFspEngine.GetAudioEngine
 * 获取，对象属于IFspEngine，不需要去关注对象声明周期
 */
class IAudioEngine {
public:
  /**
   * @brief 设置音频系统参数
   * @param param_key 设置哪个音频参数，具体参考AudioParamKey
   * @param value param_key所指参数的值，如何取值参考AudioParamKey的每个定义
   */
  virtual ErrCode SetAudioParam(AudioParamKey param_key, int value) = 0;

  /**
   * @brief 获取音频系统参数
   * @param param_key 获取哪个音频参数
   */
  virtual int GetAudioParam(AudioParamKey param_key) = 0;


  /**
   * @brief 设置当前扬声器设备
   */
  virtual ErrCode SetSpeakerDevice(int device_id) = 0;

  /**
   * @brief 设置当前麦克风设备
   */
  virtual ErrCode SetMicrophoneDevice(int device_id) = 0;

  /**
   * @brief 当前使用的扬声器设备
   * @return 扬声器设备id， 如果设备不存在, 返回 INVALID_DEVICE_ID
   */
  virtual int GetSpeakerDevice() = 0;

  /**
   * @brief 当前使用的麦克风设备
   * @return 麦克风设备id， 如果设备不存在, 返回 INVALID_DEVICE_ID
   */
  virtual int GetMicrophoneDevice() = 0;
};

/**
 * @brief sdk回调事件和异步结果的接口
 *
 *上层实现该接口并通过IFspEngine::SetEngineListener设置给sdk
 */
class IFspEngineEventHandler {
public:
  /**
   * @brief 设备变化通知
   * @param device_event 事件类型
   */
  virtual void OnDeviceChange(DeviceEventType device_event) = 0;

  /**
   * @brief 事件通知
   * @param event_type 事件的类型
   * @param err_code 事件结果
   */
  virtual void OnEvent(EventType event_type, ErrCode err_code) = 0;

  /**
   * @brief 远端视频的事件
   * @param user_id 远端视频所属的user id
   * @param video_id 远端视频所属的video id
   * @param remote_video_event 事件类型
   */
  virtual void OnRemoteVideoEvent(const String &user_id, const String &video_id,
                                  RemoteVideoEventType remote_video_event) = 0;

  /**
   * @brief 远端音频的事件
   * @param user_id 远端视频所属的user id
   * @param remote_audio_event 事件类型
   */
  virtual void OnRemoteAudioEvent(const String &user_id,
                                  RemoteAudioEventType remote_audio_event) = 0;
  
  /**
  * @brief 屏幕共享远程控制的通知事件
  * @param user_id 对方userid
  * @param operation_type 参见RemoteControlOperation枚举注释
  */
  virtual void OnRemoteControlOperationEvent(const String& user_id,
	  RemoteControlOperationType operation_type) = 0;

};


/**
 * @brief sdk对外核心接口
 */
class IFspEngine {
public:
  /**
   * @brief 初始化
   * @param context 初始化配置参数
   * @return 结果错误码
   */
  virtual ErrCode Init(const FspEngineContext &context) = 0;

  /**
   * sdk版本信息
   * @return 版本字符串
   */
  virtual String GetVersion() = 0;

  /**
   * @brief 加入组
   * @param fsp_token 访问fsp的令牌,令牌的获取参考fsp鉴权
   * @param group_id 登录组的id
   * @return 结果错误码
   * @param user_id 自身的userid
   */
  virtual ErrCode JoinGroup(const String &fsp_token, const String &group_id,
                            const String &user_id) = 0;

  /**
   * @brief 退出组
   * @return 结果错误码
   */
  virtual ErrCode LeaveGroup() = 0;

  /**
   * @brief 本地视频增加预览渲染
   * @param camera_id 哪个摄像头
   * @param render_wnd 渲染窗口, 标识一个渲染
   * @param mode 拉伸模式
   * @return 结果错误码
   */
  virtual ErrCode AddVideoPreview(int camera_id, HWND render_wnd, RenderMode mode) = 0;

  /**
   * @brief 本地视频删除预览渲染
   * @param camera_id 哪个摄像头
   * @param render_wnd 渲染窗口，标识一个渲染
   * @return 结果错误码
   */
  virtual ErrCode RemoveVideoPreview(int camera_id, HWND render_wnd) = 0;

  /**
   * @brief 开始广播视频
   * @param video_id 视频id, 支持同时广播多路视频时， video_id标识每路视频
   * @param camera_id 这路视频对应用哪个摄像头, camera_id 为
   * VideoDeviceInfo.camera_id
   * @return 结果错误码
   */
  virtual ErrCode StartPublishVideo(const String &video_id, int camera_id) = 0;

  /**
   * @brief 停止广播视频
   * @param video_id 哪路视频
   */
  virtual ErrCode StopPublishVideo(const String &video_id) = 0;

  /**
  * @breif 设置本地视频profile, 如果设置的profile参数有不合理，SDK会做适当调整
  * @param video_id 设置哪路视频的profile
  * @param profile profile参数
  */
  virtual ErrCode SetVideoProfile(const String &video_id, const VideoProfile& profile) = 0;
  
  /**
   * @brief 设置远端用户视频的渲染窗口
   * @param user_id 哪个用户
   * @param video_id 哪路视频的video id
   * @param render_wnd 渲染窗口, 传NULL停止接收视频
   * @param mode 拉伸模式
   */
  virtual ErrCode SetRemoteVideoRender(const String &user_id,
	  const String &video_id,
	  HWND render_wnd,
	  RenderMode mode) = 0;

  /**
   * @brief 获取视频的统计数据, 一般定时调用获取。 支持获取远端和本端的统计，
   * 根据user_id区分
   * @param user_id 用户id, 如果是本端userid， 获取的是本地广播出去视频的统计
   * @param video_id 对应的videoid
   * @param [out] stats 用户输出的VideoStatsInfo对象指针
   */
  virtual ErrCode GetVideoStats(const String &user_id, const String &video_id,
	  VideoStatsInfo *stats) = 0;

  /**
   * @brief 开始广播音频
   */
  virtual ErrCode StartPublishAudio() = 0;

  /**
   * @brief 停止广播音频
   */
  virtual ErrCode StopPublishAudio() = 0;

  /*
   * @brief 开始播放音频，如果在FspEngineContext中设置了自动播放音频，则不需要手动调用此接口
   */
  virtual ErrCode StartPlayAudio() = 0;

  /*
   * @brief 停止播放音频
   */
  virtual ErrCode StopPlayAudio() = 0;

  /**
   * @brief 获取远端用户的音频能量
   * @param user_id 对应的远端用户id
   * @return 能量值 0 - 100
   */
  virtual int GetRemoteAudioEnergy(const String &user_id) = 0;
  
  /**
   * @brief 开关远端音频
   * @param user_id 远端用户id
   * @param is_mute true关闭远端音频，false打开
   */
  virtual ErrCode MuteRemoteAudio(const String &user_id, bool is_mute) = 0;

  
  /**
   * @brief 获取engine 的IDeviceManager
   * @return IDeviceManager 对象指针
   */
  virtual IDeviceManager *GetDeviceManager() = 0;

  /**
   * @brief 获取engine的 IAudioEngine
   * @return IAudioEngine 对象指针
   */
  virtual IAudioEngine *GetAudioEngine() = 0;

  /**
  * @brief 开始屏幕共享, 如果已经在屏幕共享，可以继续调用更新共享参数
  * @param left 共享区域的左边x坐标
  * @param top 共享区域的顶边y坐标
  * @param right 共享区域的右边x坐标
  * @param bottom 共享区域的底边y坐标,
  * @param quality_bias 共享的质量偏好模式
  * @note 如果四个坐标值全0， 共享整个桌面
  * @return 结果错误码
  */
  virtual ErrCode StartPublishScreenShare(int left, int top, int right, int bottom, ScreenShareQualityBias quality_bias) = 0;

  /**
  * @brief  停止共享
  * @return 结果错误码
  */
  virtual ErrCode StopPublishScreenShare() = 0;

  /**
   * @brief 开始本地录制
   * @param file_path 录制文件的全路径,包含文件后缀.mp4，比如d:/yourname.mp4
   * @param is_record_audio 是否录制音频
   * @param 结果错误码
   */
  virtual ErrCode StartRecord(const String& file_path, bool is_record_audio) = 0;

  /**
   * @brief 结束本地录制
   */
  virtual ErrCode StopRecord() = 0;

  /**
  * @brief  远程控制操作
  * @param  user_id 对方userid
  * @param  operation_type 操作类型
  * @return 结果错误码
  */
  virtual ErrCode RemoteControlOperation(const String &user_id, RemoteControlOperationType operation_type) = 0;

};

} // namespace fsp



/**
 * @brief 获取IFspEngine 指针
 * @return IFspEngine单例指针，每次获取的都一样
 */
FSP_API fsp::IFspEngine *FSP_CALL FspGetEngine();

/**
 * @brief 释放IFspEngine相关资源及对象
 */
FSP_API void FSP_CALL FspReleaseEngine();