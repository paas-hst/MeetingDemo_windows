/*##############################################################################
 * 文件：ToolbarMenu.h
 * 描述：底部工具栏菜单类的声明与定义
 * 作者：Teck
 * 时间：2018年5月24日
 * 版权：Copyright(C) 2018 Fsmeeting.com. All rights reserved.
 ##############################################################################*/
#pragma once

#include <list>
#include <memory>
#include "UIlib.h"
#include "UIMenu.h"

using namespace DuiLib;

////////////////////////////////////////////////////////////////////////////////
// Menu Item Format:
// | Select Label | Menu Text | Sub-Menu Label
////////////////////////////////////////////////////////////////////////////////
class CToolbarMenu
{
public:
	enum MenuItemType
	{
		MENU_ITEM_TYPE_NONE,	// 正常
		MENU_ITEM_TYPE_RADIO,	// 单选框
		MENU_ITEM_TYPE_CHECKBOX	// 多选框
	};

public:
	CToolbarMenu(HWND hParent, 
		CPaintManagerUI* pPaintManager, 
		INotifyUI* pNotifyReceiver);

	~CToolbarMenu();

	bool Init(DWORD dwMenuItemHeigh, 
		DWORD dwMenuItemWidth, 
		DWORD dwMenuLabelWidth);

	void AddMenuItem(LPCTSTR szName, 
		MenuItemType enMenuType,
		LPCTSTR szLabelName,
		LPCTSTR szText,
		LPCTSTR szGroup,
		bool bSelected);

	void ShowMenu(POINT ptAlignPoint);

private:
	struct Menu;
	typedef std::shared_ptr<Menu> MenuSP;

	struct MenuItem
	{
		CDuiString		szName;
		CDuiString		szLabelName;
		CDuiString		szLabelGroup;
		CDuiString		szText;
		bool			bSelected;
		MenuItemType	enType;
	};

private:
	void ConstructMenu(CMenuWnd* pMenu, const std::list<MenuItem>& items);
	CMenuElementUI* ConstructMenuItem(const MenuItem& item);

private:
	HWND m_hParent;
	INotifyUI* m_pNotifyReceiver;
	CPaintManagerUI* m_pPaintManager;

	DWORD m_dwMenuItemWidth;
	DWORD m_dwMenuItemHeight;
	DWORD m_dwMenuLabelWidth;
	DWORD m_dwMenuTextWidth;
	
	std::list<MenuItem> m_menuItems;
};

