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



/**
 * @brief 错误码集合
 */
enum ErrCode {
  ERR_OK = 0, ///<成功

  ERR_INVALID_ARG = 1,      ///<非法参数
  ERR_NOT_INITED = 2,       ///<未初始化
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
  EVENT_JOINGROUP_RESULT = 0 ///<加入组结果
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

enum ScreenShareEventType
{
	SCREEN_SHARE_EVENT_PUBLISHED = 0,		 ///<远端开启了屏幕共享
	SCREEN_SHARE_EVENT_PUBLISH_STOPED = 1	 ///<远端停止了屏幕共享
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

/**
 * @brief 远端视频操作类型
 */
enum RemtoeVideoOperate {
  REMOTE_VIDEO_OPEN = 0, ///<打开远端视频
  REMOTE_VIDEO_CLOSE = 1 ///<关闭远端视频
};

/**
 * @brief 回调数据类型
 */
enum CallbackDataType
{
	CALLBACK_DATA_LOCAL_AUDIO_RAW = 0,		///<本地麦克风PCM数据
	CALLBACK_DATA_REMOTE_AUDIO_RAW = 1,		///<远端广播麦克风PCM数据
	CALLBACK_DATA_LOCAL_AUDIO_MIXED_RAW = 2,///<本端麦克风和所有远端广播麦克风合成的PCM数据
	CALLBACK_DATA_LOCAL_VIDEO_RAW = 3,		///<本地摄像头裸流数据
	CALLBACK_DATA_REMOTE_VIDEO_RAW = 4		///<远端广播摄像头裸流数据
};

//前置声明
class IFspEngineEventHandler;
class IFspEngineDataHandler;

/**
 * @brief sdk初始化时需要的信息
 */
struct FspEngineContext {
  IFspEngineEventHandler *event_handler; ///<外部实现的事件回调对象
  IFspEngineDataHandler *data_handler;	///<外部实现的数据回调对象
  String app_id;						///< appid 由fsp平台分配的应用id
  String log_path;						///<日志目录，如果不填，默认程序所在目录
  String server_addr;					///<服务地址，ip或域名加端口， 格式： "127.0.0.1:50002"，
										///<如果不填，默认使用好视通的 paas
  bool auto_recv_audio;					///<是否自动接收远端音频
  bool auto_play_audio;					///<是否自动播放远端音频

  FspEngineContext() 
	  : event_handler(NULL)
	  , data_handler(NULL)
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
 * @brief 回调数据描述
 *
 * 回调不同的数据（设置StreamDataType）需要填充不同的附加字段，填充规则如下：
 * CALLBACK_DATA_LOCAL_AUDIO_RAW: 无需填充其他附加字段 		
 * CALLBACK_DATA_REMOTE_AUDIO_RAW: 需要填充user_id字段
 * CALLBACK_DATA_LOCAL_AUDIO_MIXED_RAW: 无需填充其他字段
 * CALLBACK_DATA_LOCAL_VIDEO_RAW: 需要填充camera_id字段
 * CALLBACK_DATA_REMOTE_VIDEO_RAW: 需要填充user_id和video_id字段
 */
struct CallbackDataDesc
{
	CallbackDataType data_type;
	String user_id;
	String video_id;
	int camera_id;
};

/**
 * @brief 视频头信息，描述回调视频的属性信息
 */
struct BitmapInfoHeader {
	DWORD      biSize;				///<结构体大小
	LONG       biWidth;				///<图片宽度
	LONG       biHeight;			///<图片高度
	WORD       biPlanes;			///<目标绘图设备层数
	WORD       biBitCount;			///<色深
	DWORD      biCompression;		///<压缩方式
	DWORD      biSizeImage;			///<图像大小
	LONG       biXPelsPerMeter;		///<水平分辨率
	LONG       biYPelsPerMeter;		///<垂直分辨率
	DWORD      biClrUsed;			///<图像使用的颜色
	DWORD      biClrImportant;		///<重要的颜色数
};

/**
 * @brief 流统计数据，单位是Byte
 */
struct StreamStats {
	int recv_data_size;	///<接收数据大小
	int send_data_size;	///<发送数据大小
};

/*
 * @brief 发起共享的参数信息
 */
struct ScreenShareHostConfig{
	bool enable_telecontrol;	///<是否允许远程控制
	bool remove_wallpaper;		///<是否移除壁纸
	int  quality_bias;			///<0 为偏重清晰度（文档模式)， 1 为偏重帧率（视频模式）
	RECT rect_share;			///<所共享的区域大小，share_mode为3时必填

	ScreenShareHostConfig()
	{
		rect_share.left = 0;
		rect_share.top = 0;
		rect_share.right = 0;
		rect_share.bottom = 0;
		enable_telecontrol = false;
		remove_wallpaper = false;
		quality_bias = false;
	}
};

struct ScreenShareViewConfig {
	HWND		wnd_parent;			 ///<父窗体的句柄
	bool		enable_telecontrol;	 ///<是否允许远程控制
	RECT		rect_share;			///<接收屏幕共享数据时渲染的区域

	ScreenShareViewConfig()
	{
		wnd_parent = NULL;
		rect_share.left = 0;
		rect_share.top = 0;
		rect_share.right = 0;
		rect_share.bottom = 0;
		enable_telecontrol = false;
	}
};

enum ScreenShareRemoteControlEventType
{
	EVENT_APPLY_REMOTE_CONTROL = 0,   ///<请求远程控制
	EVENT_CANCEL_REMOTE_CONTROL = 1,  ///<取消远程控制
	EVENT_ACCEPT_REMOTE_CONTROL = 2,  ///<同意远程控制
	EVENT_REFUSE_REMOTE_CONTROL = 3   ///<拒绝远程控制
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
   * @brief 开始扬声器回声延迟检测
   */
  virtual void StartAecDelayDetect() = 0;

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
  * @brief 屏幕共享的事件
  * @param user_id 远端屏幕共享所属的user id
  * @param screen_share_event 参见ScreenShareEventType枚举注释
  */
  virtual void OnScreenShareEvent(const String &user_id, ScreenShareEventType screen_share_event) = 0;

  /**
  * @brief 屏幕共享远程控制的通知事件
  * @param user_id 远程控制操作的用户id
  * @param event_type 参见ScreenShareRemoteControlEventType枚举注释
  */
  virtual void OnScreenShareRemoteControlEvent(const String& user_id, 
	  const String& src_user_name, ScreenShareRemoteControlEventType event_type) = 0;

};

/**
 * @brief 音视频数据回调，应用层继承此接口并实现
 * 当前音频格式固定：采样率16K，采样位深16bit，单声道
 */
class IFspEngineDataHandler {
public:
	/**
	 * @brief 本地音频数据回调（麦克风）
	 * @param data 数据指针
	 * @param data_len 数据长度
	 */
	virtual void OnLocalAudioStreamRawData(const char* data, int data_len) = 0;

	/**
	 * @brief 远端音频数据回调
	 * @param user_id 远端用户标识
	 * @param data 数据指针
	 * @param data_len 数据长度
	 */
	virtual void OnRemoteAudioStreamRawData(const String& user_id, const char* data, int data_len) = 0;

	/**
	 * @brief 混音数据回调，包含本地和远端所有音频，采样率16K，采样位深16bit，单声道
	 * @param data 数据指针
	 * @param data_len 数据长度
	 */
	virtual void OnMixAudioStreamRawData(const char* data, int data_len) = 0;

	/**
	 * @brief 本地视频数据回调
	 * @param camera_id 本地摄像头标识
	 * @param header 视频头信息
	 * @param data 数据指针
	 * @param data_len 数据长度
	 */
	virtual void OnLocalVideoStreamRawData(int camera_id, BitmapInfoHeader* header, const char* data, int data_len) = 0;

	/**
	 * @brief 远端视频数据回调
	 * @param user_id 远端用户标识
	 * @param video_id 远端视频标识
	 * @param header 视频头信息
	 * @param data 数据指针
	 * @param data_len 数据长度
	 */
	virtual void OnRemoteVideoStreamRawData(const String& user_id, const String& video_id, BitmapInfoHeader* header, const char* data, int data_len) = 0;
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
   * @return 结果错误码
   */
  virtual ErrCode AddVideoPreview(int camera_id, HWND render_wnd) = 0;

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
   * @brief 操作远程视频
   * @param user_id 远端用户id
   * @param video_id 远端的video id
   * @param operation 操作动作类型
   */
  virtual ErrCode HandleRemoteVideo(const String &user_id,
                                    const String &video_id,
                                    RemtoeVideoOperate operation) = 0;

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
   * @brief 开关远端音频
   * @param user_id 远端用户id
   * @param is_mute true关闭远端音频，false打开
   */
  virtual ErrCode MuteRemoteAudio(const String &user_id, bool is_mute) = 0;

  /**
   * @brief 设置远端用户视频的渲染窗口
   * @param user_id 哪个用户
   * @param video_id 哪路视频的video id
   * @param render_wnd 渲染窗口, 标识一个渲染
   */
  virtual ErrCode SetRemoteVideoRender(const String &user_id,
                                       const String &video_id,
                                       HWND render_wnd) = 0;

  /**
   * @breif 设置视频渲染的拉伸模式
   * @param redner_wnd 渲染窗口标识的渲染
   * @param mode 拉伸模式
   */
  virtual ErrCode SetRenderMode(HWND render_wnd, RenderMode mode) = 0;

  /**
   * @breif 设置数据回调状态
   * @param desc 回调数据描述，具体见CallbackDataDesc描述
   * @param state true表示开启，false表示关闭
   */
  virtual ErrCode SetCallbackDataState(CallbackDataDesc desc, bool state) = 0;

  /**
   * @brief 获取流统计数据
   * @param reset true: 返回统计数据后数据清零; false: 返回统计数据后，数据不清零，继续累加
   * @return 统计数据
   */
  virtual StreamStats GetStreamStats(bool reset) = 0;

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
  * @brief 开始共享
  * @param screen_share_config 共享的参数，参数信息参考数据结构ScreenShareHostConfig
  * @return 结果错误码
  */
  virtual ErrCode StartPublishScreenShare(const ScreenShareHostConfig& screen_share_config) = 0;

  /**
  * @brief 设置共享的参数，发起方调用
  * @param screen_share_config 共享的参数，参数信息参考数据结构ScreenShareHostConfig
  * @return 结果错误码
  */
  virtual ErrCode SetScreenShareHostConfig(const ScreenShareHostConfig& screen_share_config) = 0;

  /**
  * @brief 获取共享的参数，发起方调用
  * @param screen_share_config 共享的参数，参数信息参考数据结构ScreenShareHostConfig
  * @return 结果错误码
  */
  virtual ErrCode GetScreenShareHostConfig(ScreenShareHostConfig& screen_share_config) = 0;

  /**
  * @brief  停止共享
  * @return 结果错误码
  */
  virtual ErrCode StopPublishScreenShare() = 0;

  /**
  * @brief 接收共享数据
  * @param user_id 被接收方的user id
  * @param screen_share_config 共享的参数，参数信息参考数据结构ScreenShareViewConfig
  * @return 结果错误码
  */
  virtual ErrCode StartReceiveScreenShare(const String& user_id, HWND render_wnd,
										const ScreenShareViewConfig& screen_share_config) = 0;

  /**
  * @brief 设置共享的参数，接收方调用
  * @param user_id 需要设置参数的用户id
  * @param screen_share_config 共享的参数，参数信息参考数据结构ScreenShareViewConfig
  * @return 结果错误码
  */
  virtual ErrCode SetScreenShareViewConfig(const String& user_id, const ScreenShareViewConfig& screen_share_config) = 0;

  /**
  * @brief 获取共享的参数，接收方调用
  * @param user_id 需要获取参数的用户id
  * @param screen_share_config 共享的参数，参数信息参考数据结构ScreenShareViewConfig
  * @return 结果错误码
  */
  virtual ErrCode GetScreenShareViewConfig(const String& user_id, ScreenShareViewConfig& screen_share_config) = 0;

  /**
  * @brief  停止接收共享
  * @user_id被停止接收的用户的id
  * @return 结果错误码
  */
  virtual ErrCode StopReceiveScreenShare(const String &user_id) = 0;

  /**
  * @brief  远程控制操作
  * @param  user_id 申请发起方的user id
  * @param  operation_type 事件类型，0 申请远程控制，1 取消远程控制， 2 同意，3 拒绝
  * @return 结果错误码
  */
  virtual ErrCode RemoteControlOperation(const String &user_id, ScreenShareRemoteControlEventType operation_type) = 0;

  /**
  * @brief  改变所接收的屏幕共享显示区域（显示区域大小变化时调用）
  * @param  user_id 需要改变显示区域的用户id（即所接收的某个桌面共享者的用户id）
  * @param  display_rect 新的显示区域
  * @param  rsp_code 响应的信息，2 是同意，3是拒绝
  * @return 结果错误码
  */
  virtual ErrCode ChangeScreenShareDisplayRect(const String& user_id, const RECT& display_rect) = 0;

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