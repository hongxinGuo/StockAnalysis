﻿#include "stdafx.h"
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

COutputWnd::COutputWnd() {
  m_uIdTimer = 0;
}

COutputWnd::~COutputWnd() {
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_TIMER()
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) {
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

  if (!m_wndOutputInformation.Create(dwStyle, rectDummy, &m_wndTabs, 2) ||
      !m_wndOutputDayLineInfo.Create(dwStyle, rectDummy, &m_wndTabs, 3) ||
      !m_wndOutputTransaction.Create(dwStyle, rectDummy, &m_wndTabs, 4) ||
      !m_wndOutputCancelSell.Create(dwStyle, rectDummy, &m_wndTabs, 5) ||
      !m_wndOutputCancelBuy.Create(dwStyle, rectDummy, &m_wndTabs, 6) ||
      !m_wndOutputTrace2.Create(dwStyle, rectDummy, &m_wndTabs, 7) ||
      !m_wndOutputInnerSystemInformation.Create(dwStyle, rectDummy, &m_wndTabs, 8))
  {
    TRACE0("未能创建输出窗口\n");
    return -1;      // 未能创建
  }

  UpdateFonts();

  CString strTabName;
  BOOL bNameValid;

  // 将列表窗口附加到选项卡:
  bNameValid = strTabName.LoadString(IDS_INFORMATION_TAB);
  ASSERT(bNameValid);
  m_wndTabs.AddTab(&m_wndOutputInformation, strTabName, (UINT)0);
  bNameValid = strTabName.LoadString(IDS_DAYLINE_INFO_TAB);
  ASSERT(bNameValid);
  m_wndTabs.AddTab(&m_wndOutputDayLineInfo, strTabName, (UINT)1);
  bNameValid = strTabName.LoadString(IDS_TRANSACTION_TAB);
  ASSERT(bNameValid);
  m_wndTabs.AddTab(&m_wndOutputTransaction, strTabName, (UINT)2);
  bNameValid = strTabName.LoadString(IDS_CANCEL_SELL_TAB);
  ASSERT(bNameValid);
  m_wndTabs.AddTab(&m_wndOutputCancelSell, strTabName, (UINT)3);
  bNameValid = strTabName.LoadString(IDS_CANCEL_BUY_TAB);
  ASSERT(bNameValid);
  m_wndTabs.AddTab(&m_wndOutputCancelBuy, strTabName, (UINT)4);
  bNameValid = strTabName.LoadString(IDS_TRACE2_TAB);
  ASSERT(bNameValid);
  m_wndTabs.AddTab(&m_wndOutputTrace2, strTabName, (UINT)5);
  bNameValid = strTabName.LoadString(IDS_INNER_SYSTEM_INFORMATION_TAB2);
  ASSERT(bNameValid);
  m_wndTabs.AddTab(&m_wndOutputInnerSystemInformation, strTabName, (UINT)6);

  // 设置1000毫秒每次的软调度，用于接受处理实时网络数据
  m_uIdTimer = SetTimer(3, 1000, nullptr);     // 500毫秒每次调度，用于从股票数据提供网站读取数据。
  if (m_uIdTimer == 0) {
    CString str;
  }

  return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy) {
  CDockablePane::OnSize(nType, cx, cy);

  // 选项卡控件应覆盖整个工作区:
  m_wndTabs.SetWindowPos(nullptr, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox) {
  CClientDC dc(this);
  CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

  int cxExtentMax = 0;

  for (int i = 0; i < wndListBox.GetCount(); i++)
  {
    CString strItem;
    wndListBox.GetText(i, strItem);

    cxExtentMax = max(cxExtentMax, (int)dc.GetTextExtent(strItem).cx);
  }

  wndListBox.SetHorizontalExtent(cxExtentMax);
  dc.SelectObject(pOldFont);
}

void COutputWnd::UpdateFonts() {
  m_wndOutputInformation.SetFont(&afxGlobalData.fontRegular);
  m_wndOutputDayLineInfo.SetFont(&afxGlobalData.fontRegular);
  m_wndOutputTransaction.SetFont(&afxGlobalData.fontRegular);
  m_wndOutputCancelSell.SetFont(&afxGlobalData.fontRegular);
  m_wndOutputCancelBuy.SetFont(&afxGlobalData.fontRegular);
  m_wndOutputTrace2.SetFont(&afxGlobalData.fontRegular);
  m_wndOutputInnerSystemInformation.SetFont(&afxGlobalData.fontRegular);
}

/////////////////////////////////////////////////////////////////////////////
// COutputList1

COutputList::COutputList() {
}

COutputList::~COutputList() {
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

void COutputList::OnContextMenu(CWnd* /*pWnd*/, CPoint point) {
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

void COutputList::OnEditCopy() {
  MessageBox(_T("复制输出"));
}

void COutputList::OnEditClear() {
  MessageBox(_T("清除输出"));
}

void COutputList::OnViewOutput() {
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
void COutputWnd::OnTimer(UINT_PTR nIDEvent) {
  // TODO: 在此添加消息处理程序代码和/或调用默认值

  CString str, str2;
  long lTotal = 0;
  long lCurrentPos = 0;
  bool fUpdate = false;
  CString strTime = gl_systemTime.GetTimeString();

  // 如果显示列表超过10000个，则删除前面的1000个。
  if (m_wndOutputInformation.GetCount() > 10000) {
    for (int i = 0; i < 1000; i++) {
      m_wndOutputInformation.DeleteString(0);
    }
  }
  fUpdate = false;
  // 将输出信息拷贝到消息队列中。
  if ((lTotal = gl_systemMessage.GetInformationDequeSize()) > 0) {
    lCurrentPos = m_wndOutputInformation.GetCurSel();
    if (m_wndOutputInformation.GetCount() <= (lCurrentPos + 4)) fUpdate = true;
    for (int i = 0; i < lTotal; i++) {
      str = gl_systemMessage.PopInformationMessage();
      str2 = strTime + _T(": ") + str;
      m_wndOutputInformation.AddString(str2);
    }
    lCurrentPos = m_wndOutputInformation.GetTopIndex();
    if (fUpdate) {
      m_wndOutputInformation.SetCurSel(m_wndOutputInformation.GetCount() - 1);
      m_wndOutputInformation.SetTopIndex(m_wndOutputInformation.GetCount() - 1);
    }
  }

  if (m_wndOutputDayLineInfo.GetCount() > 10000) {// 如果显示列表超过10000个，则删除前面的1000个。
    for (int i = 0; i < 1000; i++) {
      m_wndOutputDayLineInfo.DeleteString(0);
    }
  }
  fUpdate = false;
  if ((lTotal = gl_systemMessage.GetDayLineInfoDequeSize()) > 0) {
    lCurrentPos = m_wndOutputDayLineInfo.GetCurSel();
    if (m_wndOutputDayLineInfo.GetCount() <= (lCurrentPos + 4)) {
      fUpdate = true;
    }
    for (int i = 0; i < lTotal; i++) {
      str = gl_systemMessage.PopDayLineInfoMessage();
      str2 = strTime + _T(": ") + str;
      m_wndOutputDayLineInfo.AddString(str2);
    }
    if (fUpdate) {
      m_wndOutputDayLineInfo.SetCurSel(m_wndOutputDayLineInfo.GetCount() - 1);
      m_wndOutputDayLineInfo.SetTopIndex(m_wndOutputDayLineInfo.GetCount() - 1);
    }
  }

  if (m_wndOutputTransaction.GetCount() > 10000) {// 如果显示列表超过2000个，则删除前面的1000个。
    for (int i = 0; i < 1000; i++) {
      m_wndOutputTransaction.DeleteString(0);
    }
  }
  if ((lTotal = gl_systemMessage.GetTransactionDequeSize()) > 0) {
    for (int i = 0; i < lTotal; i++) {
      str = gl_systemMessage.PopTransactionMessage();
      str2 = strTime + _T(": ") + str;
      m_wndOutputTransaction.AddString(str2);
    }
    m_wndOutputTransaction.SetTopIndex(m_wndOutputTransaction.GetCount() - 1);
  }

  if (m_wndOutputCancelSell.GetCount() > 10000) {// 如果显示列表超过2000个，则删除前面的1000个。
    for (int i = 0; i < 1000; i++) {
      m_wndOutputCancelSell.DeleteString(0);
    }
  }
  if ((lTotal = gl_systemMessage.GetCancelSellDequeSize()) > 0) {
    for (int i = 0; i < lTotal; i++) {
      str = gl_systemMessage.PopCancelSellMessage();
      str2 = strTime + _T(": ") + str;
      m_wndOutputCancelSell.AddString(str2);
    }
    m_wndOutputCancelSell.SetTopIndex(m_wndOutputCancelSell.GetCount() - 1);
  }

  if (m_wndOutputCancelBuy.GetCount() > 10000) {// 如果显示列表超过2000个，则删除前面的1000个。
    for (int i = 0; i < 1000; i++) {
      m_wndOutputCancelBuy.DeleteString(0);
    }
  }
  if ((lTotal = gl_systemMessage.GetCancelBuyDequeSize()) > 0) {
    for (int i = 0; i < lTotal; i++) {
      str = gl_systemMessage.PopCancelBuyMessage();
      str2 = strTime + _T(": ") + str;
      m_wndOutputCancelBuy.AddString(str2);
    }
    m_wndOutputCancelBuy.SetTopIndex(m_wndOutputCancelBuy.GetCount() - 1);
  }

  if (m_wndOutputTrace2.GetCount() > 10000) {// 如果显示列表超过2000个，则删除前面的1000个。
    for (int i = 0; i < 1000; i++) {
      m_wndOutputTrace2.DeleteString(0);
    }
  }
  if ((lTotal = gl_systemMessage.GetTrace2DequeSize()) > 0) {
    for (int i = 0; i < lTotal; i++) {
      str = gl_systemMessage.PopTrace2Message();
      str2 = strTime + _T(": ") + str;
      m_wndOutputTrace2.AddString(str2);
    }
    m_wndOutputTrace2.SetTopIndex(m_wndOutputTrace2.GetCount() - 1);
  }

  if (m_wndOutputInnerSystemInformation.GetCount() > 10000) {// 如果显示列表超过2000个，则删除前面的1000个。
    for (int i = 0; i < 1000; i++) {
      m_wndOutputInnerSystemInformation.DeleteString(0);
    }
  }
  fUpdate = false;
  if ((lTotal = gl_systemMessage.GetInnerSystemInformationDequeSize()) > 0) {
    lCurrentPos = m_wndOutputInnerSystemInformation.GetCurSel();
    if (m_wndOutputInnerSystemInformation.GetCount() <= (lCurrentPos + 4)) fUpdate = true;
    for (int i = 0; i < lTotal; i++) {
      str = gl_systemMessage.PopInnerSystemInformationMessage();
      str2 = strTime + _T(": ") + str;
      m_wndOutputInnerSystemInformation.AddString(str2);
    }
    if (fUpdate) {
      m_wndOutputInnerSystemInformation.SetCurSel(m_wndOutputInnerSystemInformation.GetCount() - 1);
      m_wndOutputInnerSystemInformation.SetTopIndex(m_wndOutputInnerSystemInformation.GetCount() - 1);
    }
  }

  // 调用基类的OnTimer函数
  CDockablePane::OnTimer(nIDEvent);
}