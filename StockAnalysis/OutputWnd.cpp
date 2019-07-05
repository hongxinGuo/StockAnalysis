﻿
#include "stdafx.h"
#include"globedef.h"

#include "OutputWnd.h"
#include "Resource.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

COutputWnd::COutputWnd() noexcept
{
  m_uIdTimer = 0;

}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
  ON_WM_TIMER()
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CDockablePane::OnCreate(lpCreateStruct) == -1)
    return -1;

  CRect rectDummy;
  rectDummy.SetRectEmpty();

  // 创建选项卡窗口: 
  if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
  {
    TRACE0("未能创建输出选项卡窗口\n");
    return -1;      // 未能创建
  }

  // 创建输出窗格: 
  const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

  if (!m_wndOutputBuild.Create(dwStyle, rectDummy, &m_wndTabs, 2) ||
    !m_wndOutputDebug.Create(dwStyle, rectDummy, &m_wndTabs, 3) ||
    !m_wndOutputFind.Create(dwStyle, rectDummy, &m_wndTabs, 4) ||
    !m_wndOutputWaring.Create(dwStyle, rectDummy, &m_wndTabs, 5) ||
    !m_wndOutputTrace1.Create(dwStyle, rectDummy, &m_wndTabs, 6) )
{
		TRACE0("未能创建输出窗口\n");
		return -1;      // 未能创建
	}

	UpdateFonts();

	CString strTabName;
	BOOL bNameValid;

	// 将列表窗口附加到选项卡: 
	bNameValid = strTabName.LoadString(IDS_BUILD_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputBuild, strTabName, (UINT)0);
	bNameValid = strTabName.LoadString(IDS_DEBUG_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputDebug, strTabName, (UINT)1);
	bNameValid = strTabName.LoadString(IDS_FIND_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputFind, strTabName, (UINT)2);
  bNameValid = strTabName.LoadString(IDS_FIND2_TAB);
  ASSERT(bNameValid);
  m_wndTabs.AddTab(&m_wndOutputWaring, strTabName, (UINT)3);
  bNameValid = strTabName.LoadString(IDS_TRACE1_TAB);
  ASSERT(bNameValid);
  m_wndTabs.AddTab(&m_wndOutputTrace1, strTabName, (UINT)4);


  // 设置1000毫秒每次的软调度，用于接受处理实时网络数据
  m_uIdTimer = SetTimer(3, 1000, nullptr);     // 500毫秒每次调度，用于从股票数据提供网站读取数据。
  if (m_uIdTimer == 0) {
    CString str;
  }

	// 使用一些虚拟文本填写输出选项卡(无需复杂数据)
	FillBuildWindow();
	FillDebugWindow();
	FillFindWindow();

	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// 选项卡控件应覆盖整个工作区: 
	m_wndTabs.SetWindowPos (nullptr, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

	int cxExtentMax = 0;

	for (int i = 0; i < wndListBox.GetCount(); i ++)
	{
		CString strItem;
		wndListBox.GetText(i, strItem);

		cxExtentMax = max(cxExtentMax, (int)dc.GetTextExtent(strItem).cx);
	}

	wndListBox.SetHorizontalExtent(cxExtentMax);
	dc.SelectObject(pOldFont);
}

void COutputWnd::FillBuildWindow()
{
	m_wndOutputBuild.AddString(_T("生成输出正显示在此处。"));
	m_wndOutputBuild.AddString(_T("输出正显示在列表视图的行中"));
	m_wndOutputBuild.AddString(_T("但您可以根据需要更改其显示方式..."));
}

void COutputWnd::FillDebugWindow()
{
	m_wndOutputDebug.AddString(_T("调试输出正显示在此处。"));
	m_wndOutputDebug.AddString(_T("输出正显示在列表视图的行中"));
	m_wndOutputDebug.AddString(_T("但您可以根据需要更改其显示方式..."));
}

void COutputWnd::FillFindWindow()
{
	m_wndOutputFind.AddString(_T("查找输出正显示在此处。"));
	m_wndOutputFind.AddString(_T("输出正显示在列表视图的行中"));
	m_wndOutputFind.AddString(_T("但您可以根据需要更改其显示方式..."));
}

void COutputWnd::UpdateFonts()
{
	m_wndOutputBuild.SetFont(&afxGlobalData.fontRegular);
	m_wndOutputDebug.SetFont(&afxGlobalData.fontRegular);
	m_wndOutputFind.SetFont(&afxGlobalData.fontRegular);
  m_wndOutputWaring.SetFont(&afxGlobalData.fontRegular);
  m_wndOutputTrace1.SetFont(&afxGlobalData.fontRegular);
}

/////////////////////////////////////////////////////////////////////////////
// COutputList1

COutputList::COutputList() noexcept
{
}

COutputList::~COutputList()
{
}

BEGIN_MESSAGE_MAP(COutputList, CListBox)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_VIEW_OUTPUTWND, OnViewOutput)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COutputList 消息处理程序

void COutputList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_OUTPUT_POPUP);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}

	SetFocus();
}

void COutputList::OnEditCopy()
{
	MessageBox(_T("复制输出"));
}

void COutputList::OnEditClear()
{
	MessageBox(_T("清除输出"));
}

void COutputList::OnViewOutput()
{
	CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner());
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());

	if (pMainFrame != nullptr && pParentBar != nullptr)
	{
		pMainFrame->SetFocus();
		pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
		pMainFrame->RecalcLayout();

	}
}


/////////////////////////////////////////////////////////////////////////////////////////
//
// 显示各项系统消息和警告等。
//
//
//
/////////////////////////////////////////////////////////////////////////////////////////
void COutputWnd::OnTimer(UINT_PTR nIDEvent)
{
  // TODO: 在此添加消息处理程序代码和/或调用默认值

  CString str;
  long lTotal = 0;

  if (m_wndOutputBuild.GetCount() > 2000) { // 如果显示列表超过2000个，则删除前面的1000个。
    for (int i = 0; i < 1000; i++) {
      m_wndOutputBuild.DeleteString(0);
    }
  }
  // 将输出信息拷贝到消息队列中。
  if ((lTotal = gl_systemMessage.GetOutputDequeSize()) > 0) {
    for (int i = 0; i < lTotal; i++) {
      str = gl_systemMessage.PopOutputMessage();
      m_wndOutputBuild.AddString(str);
    }
    m_wndOutputBuild.SetTopIndex(m_wndOutputBuild.GetCount() - 1);
  }

  if (m_wndOutputDebug.GetCount() > 2000) {// 如果显示列表超过2000个，则删除前面的1000个。
    for (int i = 0; i < 1000; i++) {
      m_wndOutputDebug.DeleteString(0);
    }
  }

  if ((lTotal = gl_systemMessage.GetDataBaseDequeSize()) > 0) {
    for (int i = 0; i < lTotal; i++) {
      str = gl_systemMessage.PopDataBaseMessage();
      m_wndOutputDebug.AddString(str);
    }
    m_wndOutputDebug.SetTopIndex(m_wndOutputDebug.GetCount() - 1);
  }

  if (m_wndOutputFind.GetCount() > 2000) {// 如果显示列表超过2000个，则删除前面的1000个。
    for (int i = 0; i < 1000; i++) {
      m_wndOutputFind.DeleteString(0);
    }
  }
  if ((lTotal = gl_systemMessage.GetFindDequeSize()) > 0) {
    for (int i = 0; i < lTotal; i++) {
      str = gl_systemMessage.PopFindMessage();
      m_wndOutputFind.AddString(str);
    }
    m_wndOutputFind.SetTopIndex(m_wndOutputFind.GetCount() - 1);
  }

  if (m_wndOutputWaring.GetCount() > 2000) {// 如果显示列表超过2000个，则删除前面的1000个。
    for (int i = 0; i < 1000; i++) {
      m_wndOutputWaring.DeleteString(0);
    }
  }
  if ((lTotal = gl_systemMessage.GetWarningDequeSize()) > 0) {
    for (int i = 0; i < lTotal; i++) {
      str = gl_systemMessage.PopWarningMessage();
      m_wndOutputWaring.AddString(str);
    }
    m_wndOutputWaring.SetTopIndex(m_wndOutputWaring.GetCount() - 1);
  }

  if (m_wndOutputTrace1.GetCount() > 2000) {// 如果显示列表超过2000个，则删除前面的1000个。
    for (int i = 0; i < 1000; i++) {
      m_wndOutputTrace1.DeleteString(0);
    }
  }
  if ((lTotal = gl_systemMessage.GetTrace1DequeSize()) > 0) {
    for (int i = 0; i < lTotal; i++) {
      str = gl_systemMessage.PopTrace1Message();
      m_wndOutputTrace1.AddString(str);
    }
    m_wndOutputTrace1.SetTopIndex(m_wndOutputTrace1.GetCount() - 1);
  }

  CDockablePane::OnTimer(nIDEvent);
}

