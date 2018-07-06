#ifndef WIN_IMPL_BASE_HPP
#define WIN_IMPL_BASE_HPP

namespace DuiLib
{

	enum UILIB_RESOURCETYPE
	{
		UILIB_FILE=1,		// 来自磁盘文件
		UILIB_ZIP,			// 来自磁盘zip压缩包
		UILIB_RESOURCE,		// 来自资源
		UILIB_ZIPRESOURCE,	// 来自资源的zip压缩包
	};

	class DUILIB_API WindowImplBase
		: public CWindowWnd
		, public CNotifyPump
		, public INotifyUI
		, public IMessageFilterUI
		, public IDialogBuilderCallback
	{
	public:
		WindowImplBase(){};
		virtual ~WindowImplBase(){};

		// OnCreate处理中会调用
		virtual void InitWindow(){};

		// CWindowWnd的虚函数，WM_DESTROY消息通知处理
		virtual void OnFinalMessage( HWND hWnd );

		// INotifyUI方法
		virtual void Notify(TNotifyUI& msg);

		// 消息映射
		DUI_DECLARE_MESSAGE_MAP()
		//	private:                                                                  
		//	static const DUI_MSGMAP_ENTRY _messageEntries[];                      
		//	protected:                                                                
		//	static const DUI_MSGMAP messageMap;                                   
		//	static const DUI_MSGMAP* PASCAL _GetBaseMessageMap();                 
		// virtual const DUI_MSGMAP* GetMessageMap() const;                      

	protected:
		// 每个窗口都有一个xml布局文件，实现以下方法指定xml布局文件
		virtual CDuiString GetSkinFolder() = 0;
		virtual CDuiString GetSkinFile() = 0;

		// CWindowWnd的虚函数
		virtual LPCTSTR GetWindowClassName(void) const = 0 ;

		// TODO:
		virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);

		// 负责绘制的Manager
		CPaintManagerUI m_PaintManager;

		// zip形式资源文件缓冲区
		static LPBYTE m_lpResourceZIPBuffer;

	public:
		// CWindowWnd的虚函数
		virtual UINT GetClassStyle() const;

		// 指定资源类型，具体查看UILIB_RESOURCETYPE定义
		virtual UILIB_RESOURCETYPE GetResourceType() const;

		// 如果资源是zip文件，则指定zip文件名
		virtual CDuiString GetZIPFileName() const;

		// 如果是以windows资源方式，则需要指定资源ID
		virtual LPCTSTR GetResourceID() const;

		// IDialogBuilderCallback接口方法，DialogBuilder会回调此方法创建控件
		virtual CControlUI* CreateControl(LPCTSTR pstrClass);

		// IMessageFilterUI接口方法
		virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& /*bHandled*/);

		// WinImplBase已经处理的方法
		virtual void OnClick(TNotifyUI& msg);
		virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		// WinImplBase没有处理，子类可以重写的方法
		virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnNcActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnNcPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
		virtual LRESULT OnMouseWheel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

		// 重写了CWindowWnd的HandleMessage方法，自定义窗口消息处理
		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

		// 子类可以重写此方法以处理自定义消息
		virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		virtual LONG GetStyle();
	};
}

#endif // WIN_IMPL_BASE_HPP
