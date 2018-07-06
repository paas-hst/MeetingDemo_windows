#ifndef MAINFRAME_HPP
#define MAINFRAME_HPP

#include "skin_change_event.hpp"
#include "UIFriends.hpp"

class MainFrame : public WindowImplBase
{
public:

	MainFrame();
	~MainFrame();

public:

	// 窗口类的名称，窗口在WindowWnd中注册
	LPCTSTR GetWindowClassName() const;	

	// 窗口销毁前的最后一个消息通知（收到窗口Destroy消息）
	virtual void OnFinalMessage(HWND hWnd);

	// WindowImplBase的虚函数，OnCreate时调用
	virtual void InitWindow();

	virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);

	// WindowImplBase的虚函数，当通过文件加载布局时使用
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual UILIB_RESOURCETYPE GetResourceType() const;

	// WindowImplBase继承自IDialogBuilderCallback接口的方法
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);


	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LPCTSTR GetResourceID() const;

    DWORD GetBkColor();
    void SetBkColor(DWORD dwBackColor);

protected:	

	void Notify(TNotifyUI& msg);
	void OnPrepare(TNotifyUI& msg);
	void OnExit(TNotifyUI& msg);
	void OnTimer(TNotifyUI& msg);

private:

	void UpdateFriendsList();

	void UpdateGroupsList();

	void UpdateMicroBlogList();

private:
	int bk_image_index_;

	FriendListItemInfo myself_info_;
	std::vector<FriendListItemInfo> friends_;

	SkinChangedObserver skin_changed_observer_;
};

#endif // MAINFRAME_HPP