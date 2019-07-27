/*##############################################################################
 * 文件：ToolbarMenu.cpp
 * 描述：底部工具栏菜单实现
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#include "stdafx.h"
#include "ToolbarMenu.h"
#include "UIMenu.h"

/*------------------------------------------------------------------------------
 * 描  述：构造函数
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
CToolbarMenu::CToolbarMenu(HWND hParent, CPaintManagerUI* pPaintManager, INotifyUI* pNotifyReceiver)
	: m_hParent(hParent)
	, m_pPaintManager(pPaintManager)
	, m_pNotifyReceiver(pNotifyReceiver)
{
}

/*------------------------------------------------------------------------------
 * 描  述：析构函数
 * 参  数：无
 * 返回值：无
------------------------------------------------------------------------------*/
CToolbarMenu::~CToolbarMenu()
{
}

/*------------------------------------------------------------------------------
 * 描  述：初始化菜单
 * 参  数：[in] dwMenuItemHeigh		菜单项高度
 *         [in] dwMenuItemWidth		菜单项宽度
 *         [in] dwMenuLabelWidth	菜单项前面标签宽度
 * 返回值：成功/失败
------------------------------------------------------------------------------*/
bool CToolbarMenu::Init(DWORD dwMenuItemHeigh, DWORD dwMenuItemWidth, DWORD dwMenuLabelWidth)
{
	if (dwMenuLabelWidth > dwMenuItemWidth)
		return false;

	m_dwMenuItemWidth = dwMenuItemWidth;
	m_dwMenuItemHeight = dwMenuItemHeigh;
	m_dwMenuLabelWidth = dwMenuLabelWidth;
	m_dwMenuTextWidth = dwMenuItemWidth - dwMenuLabelWidth;

	return true;
}

/*------------------------------------------------------------------------------
 * 描  述：添加菜单项
 * 参  数：[in] szName		菜单项名称
 *         [in] enMenuType	MenuItemType
 *         [in] szLabelName	标签名称
 *         [in] szText		菜单项显示的文本
 *         [in] szGroup		单选按钮需要设置
 *         [in] bSelected	是否显示为被选中
 * 返回值：无
------------------------------------------------------------------------------*/
void CToolbarMenu::AddMenuItem(LPCTSTR szName, MenuItemType enMenuType, LPCTSTR szLabelName, 
	LPCTSTR szText, LPCTSTR szGroup, bool bSelected)
{
	// 菜单项的名字不能重复
	for (auto iter : m_menuItems)
		if (iter.szName == szName)
			return;

	MenuItem item;
	item.szName = szName;
	item.enType = enMenuType;
	item.bSelected = bSelected;
	item.szLabelGroup = szGroup;
	item.szLabelName = szLabelName;
	item.szText = szText;

	m_menuItems.push_back(item);
}

/*------------------------------------------------------------------------------
 * 描  述：构建菜单项
 * 参  数：[in] item 菜单项数据
 * 返回值：菜单项对象
------------------------------------------------------------------------------*/
CMenuElementUI* CToolbarMenu::ConstructMenuItem(const MenuItem& item)
{
	WCHAR szTmp[32];

	CMenuElementUI* pElement = new CMenuElementUI;
	pElement->SetName(item.szLabelName);
	pElement->SetText(item.szText);
	pElement->SetAttribute(L"width", _itow(m_dwMenuItemWidth, szTmp, 10));
	pElement->SetAttribute(L"height", _itow(m_dwMenuItemHeight, szTmp, 10));

	if (item.bSelected) {
		pElement->SetBkImage(L"file='img\\checkbox_selected.png' dest='4,10,20,26'");
	} else {
		pElement->SetBkImage(L"file='img\\checkbox_normal.png' dest='4,10,20,26'");
	}

	return pElement;
}

/*------------------------------------------------------------------------------
 * 描  述：根据传入的数据构建菜单
 * 参  数：[in] pMenu	菜单对象
 *         [in] items	菜单项数据
 * 返回值：无
------------------------------------------------------------------------------*/
void CToolbarMenu::ConstructMenu(CMenuWnd* pMenu, const std::list<MenuItem>& items)
{
	// FIXME: xml布局文件中额外增加了一个空的MenuElement作为占位符，否则动态添加的
	// 菜单项无法显示，可能是个bug，暂时没有时间去解决，这里首先要移除占位符
	((CMenuUI*)pMenu->m_pm.GetRoot())->Remove(pMenu->m_pm.FindControl(L"placeholder"));

	for (auto iter : items)
	{
		CMenuElementUI* pElement = ConstructMenuItem(iter);
		if (pElement)
			((CMenuUI*)pMenu->m_pm.GetRoot())->Add(pElement);
	}
}

/*------------------------------------------------------------------------------
 * 描  述：显示菜单
 * 参  数：[in] ptAlignPoint	菜单显示的起点
 * 返回值：成功/失败
------------------------------------------------------------------------------*/
void CToolbarMenu::ShowMenu(POINT ptAlignPoint)
{
	CMenuWnd* pMenu = new CMenuWnd(m_hParent);
	if (pMenu == NULL)
		return;

	pMenu->Init(NULL, NULL, L"popup_menu.xml", _T("xml"), ptAlignPoint);

	// 动态添加菜单项
	ConstructMenu(pMenu, m_menuItems);

	// 重新计算菜单大小并显示
	pMenu->ResizeMenu();

	pMenu->m_pm.AddNotifier(m_pNotifyReceiver);
}