﻿// MainFrm.cpp: CMainFrame 类的实现
//

#include "stdafx.h"
#include "StockAnalysis.h"

#include "MainFrm.h"
#include"StockAnalysisDoc.h"
#include"StockAnalysisView.h"

#include"globedef.h"

#include"WebRTData.h"
#include"DayLine.h"
#include"ChinaStock.h"
#include"ChinaMarket.h"

#include"SinaRTWebInquiry.h"

#include"Thread.h"

#include"WebInquirer.h"

using namespace std;
#include<string>
#include<thread>

const int __STOCK_ANALYSIS_TIMER__ = 1;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
  ON_WM_CREATE()
  ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
  ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
  ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
  ON_WM_SETTINGCHANGE()
  ON_WM_TIMER()
  ON_COMMAND(ID_PROCESS_TODAY_STOCK, &CMainFrame::OnProcessTodayStock)
  ON_UPDATE_COMMAND_UI(ID_PROCESS_TODAY_STOCK, &CMainFrame::OnUpdateProcessTodayStock)
  ON_COMMAND(ID_CALCULATE_TODAY_RELATIVE_STRONG, &CMainFrame::OnCalculateTodayRelativeStrong)
  ON_WM_SYSCOMMAND()
  ON_UPDATE_COMMAND_UI(ID_CALCULATE_TODAY_RELATIVE_STRONG, &CMainFrame::OnUpdateCalculateTodayRelativeStrong)
  ON_WM_CHAR()
  ON_WM_KEYUP()
  ON_COMMAND(ID_REBUILD_DAYLINE_RS, &CMainFrame::OnRebuildDaylineRS)
  ON_COMMAND(ID_BUILD_RESET_SYSTEM, &CMainFrame::OnBuildResetMarket)
  ON_UPDATE_COMMAND_UI(ID_REBUILD_DAYLINE_RS, &CMainFrame::OnUpdateRebuildDaylineRS)
  ON_COMMAND(ID_BUILD_ABORT_BUINDING_RS, &CMainFrame::OnAbortBuindingRS)
  ON_UPDATE_COMMAND_UI(ID_BUILD_ABORT_BUINDING_RS, &CMainFrame::OnUpdateAbortBuindingRS)
  ON_COMMAND(ID_RECORD_RT_DATA, &CMainFrame::OnRecordRTData)
  ON_UPDATE_COMMAND_UI(ID_RECORD_RT_DATA, &CMainFrame::OnUpdateRecordRTData)
  ON_COMMAND(ID_CALCULATE_10DAY_RS1, &CMainFrame::OnCalculate10dayRS1)
  ON_COMMAND(ID_CALCULATE_10DAY_RS2, &CMainFrame::OnCalculate10dayRS2)
  ON_COMMAND(ID_CALCULATE_10DAY_RS, &CMainFrame::OnCalculate10dayRS)
  ON_UPDATE_COMMAND_UI(ID_CALCULATE_10DAY_RS1, &CMainFrame::OnUpdateCalculate10dayRS1)
  ON_UPDATE_COMMAND_UI(ID_CALCULATE_10DAY_RS2, &CMainFrame::OnUpdateCalculate10dayRS2)
  ON_UPDATE_COMMAND_UI(ID_CALCULATE_10DAY_RS, &CMainFrame::OnUpdateCalculate10dayRS)
  ON_COMMAND(ID_STOP_UPDATE_DAYLINE, &CMainFrame::OnStopUpdateDayLine)
  ON_COMMAND(ID_USING_NETEASE_REALTIME_DATA_SERVER, &CMainFrame::OnUsingNeteaseRealtimeDataServer)
  ON_COMMAND(ID_USING_SINA_REALTIME_DATA_SERVER, &CMainFrame::OnUsingSinaRealtimeDataServer)
  ON_UPDATE_COMMAND_UI(ID_USING_NETEASE_REALTIME_DATA_SERVER, &CMainFrame::OnUpdateUsingNeteaseRealtimeDataServer)
  ON_UPDATE_COMMAND_UI(ID_USING_SINA_REALTIME_DATA_SERVER, &CMainFrame::OnUpdateUsingSinaRealtimeDataServer)
  ON_COMMAND(ID_BUILD_CREATE_WEEKLINE, &CMainFrame::OnBuildCreateWeekLine)
  ON_UPDATE_COMMAND_UI(ID_BUILD_CREATE_WEEKLINE, &CMainFrame::OnUpdateBuildCreateWeekLine)
  ON_COMMAND(ID_REBUILD_WEEKLINE_RS, &CMainFrame::OnRebuildWeekLineRS)
  ON_UPDATE_COMMAND_UI(ID_REBUILD_WEEKLINE_RS, &CMainFrame::OnUpdateRebuildWeekLineRS)
  ON_COMMAND(ID_BUILD_CURRENT_WEEK_LINE, &CMainFrame::OnBuildCurrentWeekLine)
  ON_UPDATE_COMMAND_UI(ID_BUILD_CURRENT_WEEK_LINE, &CMainFrame::OnUpdateBuildCurrentWeekLine)
END_MESSAGE_MAP()

static UINT indicators[] =
{
  ID_SEPARATOR,           // 状态行指示器
  ID_CURRENT_INPUT,
  ID_CURRENT_SELECT_STOCK,
  ID_CURRENT_SELECT_STOCKNAME,
  ID_CURRENT_SELECT_STOCK_SET,
  ID_CURRENT_SELECT_POSITION,
  ID_CURRENT_RTDATA_READING_TIME,
  ID_CURRENT_ACTIVE_STOCK,
  ID_CURRENT_DAYLINE_READING_TIME,
  ID_CURRENT_RUNNING_THREAD,
  ID_CURRENT_RUNNING_BACKGROUND_THREAD,
  ID_CURRENT_TIME,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame() {
  // TODO: 在此添加成员初始化代码
  m_uIdTimer = 0;
  Reset();
}

void CMainFrame::Reset(void) {
  // 在此之前已经准备好了全局股票池（在CChinaMarket的构造函数中）。
  m_lCurrentPos = 0;
}

CMainFrame::~CMainFrame() {
  if (gl_fTestMode) TRACE("使用了Test驱动\n");

  gl_fExitingSystem = true;

  if (gl_pChinaStockMarket->IsUpdateOptionDB()) {
    gl_pChinaStockMarket->UpdateOptionDB();
  }

  gl_pChinaStockMarket->UpdateOptionChinaStockMarketDB();

  if (gl_pChinaStockMarket->IsUpdateChoicedStockDB()) {
    gl_pChinaStockMarket->UpdateChoicedStockDB(); // 这里直接调用存储函数，不采用工作线程的模式。
  }

  gl_pChinaStockMarket->SaveCalculatingRSOption();

  while (gl_ThreadStatus.IsSavingDayLine()) {
    Sleep(1); // 等待处理日线历史数据的线程结束。
  }

  // 更新股票代码数据库要放在最后，等待存储日线数据的线程（如果唤醒了的话）结束之后再执行。
  // 因为彼线程也在更新股票代码数据库，而此更新只是消除同类项而已。
  if (gl_pChinaStockMarket->IsUpdateStockCodeDB()) {
    gl_pChinaStockMarket->UpdateStockCodeDB(); // 这里直接调用存储函数，不采用工作线程的模式。
  }

  while (gl_WebInquirer.IsReadingWebThreadRunning()) Sleep(1);
  while (gl_ThreadStatus.IsWorkingThreadRunning()) Sleep(1);

  TRACE("finally exited\n");
}

bool CMainFrame::CreateMarketContainer(void) {
  gl_vMarketPtr.push_back(gl_pChinaStockMarket); // 中国股票市场
  gl_vMarketPtr.push_back(gl_pPotenDailyBriefingMarket); // poten.com提供的每日航运指数
  gl_vMarketPtr.push_back(gl_pCrweberIndexMarket); // Crweber.com提供的每日航运指数
  return true;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  gl_systemMessage.PushInformationMessage(_T("系统初始化中....."));

  if (gl_pChinaStockMarket == nullptr) gl_pChinaStockMarket = make_shared<CChinaMarket>();
  if (gl_pCrweberIndexMarket == nullptr) gl_pCrweberIndexMarket = make_shared<CCrweberIndexMarket>();
  if (gl_pPotenDailyBriefingMarket == nullptr) gl_pPotenDailyBriefingMarket = make_shared<CPotenDailyBriefingMarket>();
  gl_WebInquirer.Initialize();

  //生成市场容器Vector
  CreateMarketContainer();

  TRACE(_T("开始重置系统\n"));
  gl_systemMessage.PushInformationMessage(_T("重置系统"));
  ASSERT(gl_pChinaStockMarket != nullptr);
  ASSERT(gl_pPotenDailyBriefingMarket != nullptr);
  ASSERT(gl_pCrweberIndexMarket != nullptr);
  ResetMarket();
  TRACE(_T("重置系统结束\n"));

  if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
    return -1;

  BOOL bNameValid;

  CMDITabInfo mdiTabParams;
  mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // 其他可用样式...
  mdiTabParams.m_bActiveTabCloseButton = TRUE; // 设置为 FALSE 会将关闭按钮放置在选项卡区域的右侧
  mdiTabParams.m_bTabIcons = FALSE; // 设置为 TRUE 将在 MDI 选项卡上启用文档图标
  mdiTabParams.m_bAutoColor = TRUE; // 设置为 FALSE 将禁用 MDI 选项卡的自动着色
  mdiTabParams.m_bDocumentMenu = TRUE; // 在选项卡区域的右边缘启用文档菜单
  EnableMDITabbedGroups(TRUE, mdiTabParams);

  if (!m_wndMenuBar.Create(this))
  {
    TRACE0("未能创建菜单栏\n");
    return -1;      // 未能创建
  }

  m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

  // 防止菜单栏在激活时获得焦点
  CMFCPopupMenu::SetForceMenuFocus(FALSE);

  if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
      !m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
  {
    TRACE0("未能创建工具栏\n");
    return -1;      // 未能创建
  }

  CString strToolBarName;
  bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
  ASSERT(bNameValid);
  m_wndToolBar.SetWindowText(strToolBarName);

  CString strCustomize;
  bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
  ASSERT(bNameValid);
  m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

  // 允许用户定义的工具栏操作:
  InitUserToolbars(nullptr, uiFirstUserToolBarId, uiLastUserToolBarId);

  if (!m_wndStatusBar.Create(this))
  {
    TRACE0("未能创建状态栏\n");
    return -1;      // 未能创建
  }
  m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));

  // TODO: 如果您不希望工具栏和菜单栏可停靠，请删除这五行
  m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
  m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
  EnableDocking(CBRS_ALIGN_ANY);
  DockPane(&m_wndMenuBar);
  DockPane(&m_wndToolBar);

  // 启用 Visual Studio 2005 样式停靠窗口行为
  CDockingManager::SetDockingMode(DT_SMART);
  // 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
  EnableAutoHidePanes(CBRS_ALIGN_ANY);

  // 创建停靠窗口
  if (!CreateDockingWindows())
  {
    TRACE0("未能创建停靠窗口\n");
    return -1;
  }

  m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
  DockPane(&m_wndOutput);

  //m_wndOutput2.EnableDocking(CBRS_ALIGN_ANY);
  //DockPane(&m_wndOutput2);

  // 设置用于绘制所有用户界面元素的视觉管理器
  CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));

  // 启用增强的窗口管理对话框
  EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

  // 启用工具栏和停靠窗口菜单替换
  EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

  // 启用快速(按住 Alt 拖动)工具栏自定义
  CMFCToolBar::EnableQuickCustomization();

  if (CMFCToolBar::GetUserImages() == nullptr)
  {
    // 加载用户定义的工具栏图像
    if (m_UserImages.Load(_T(".\\UserImages.bmp")))
    {
      CMFCToolBar::SetUserImages(&m_UserImages);
    }
  }

  // 启用菜单个性化(最近使用的命令)
  // TODO: 定义您自己的基本命令，确保每个下拉菜单至少有一个基本命令。
  CList<UINT, UINT> lstBasicCommands;

  lstBasicCommands.AddTail(ID_FILE_NEW);
  lstBasicCommands.AddTail(ID_FILE_OPEN);
  lstBasicCommands.AddTail(ID_FILE_SAVE);
  lstBasicCommands.AddTail(ID_FILE_PRINT);
  lstBasicCommands.AddTail(ID_APP_EXIT);
  lstBasicCommands.AddTail(ID_EDIT_CUT);
  lstBasicCommands.AddTail(ID_EDIT_PASTE);
  lstBasicCommands.AddTail(ID_EDIT_UNDO);
  lstBasicCommands.AddTail(ID_APP_ABOUT);
  lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
  lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);

  CMFCToolBar::SetBasicCommands(lstBasicCommands);

  // 将文档名和应用程序名称在窗口标题栏上的顺序进行交换。这
  // 将改进任务栏的可用性，因为显示的文档名带有缩略图。
  ModifyStyle(0, FWS_PREFIXTITLE);

  // 设置100毫秒每次的软调度，用于接受处理实时网络数据。目前新浪股票接口的实时数据更新频率为每三秒一次，故而400毫秒（200X2）读取900个股票就足够了。
  m_uIdTimer = SetTimer(__STOCK_ANALYSIS_TIMER__, 100, nullptr);     // 100毫秒每次调度，用于调度各类定时处理任务。
  if (m_uIdTimer == 0) {
    TRACE(_T("生成100ms时钟时失败\n"));
  }
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//系统更新任务由各CVirtualMarket类中的调度函数完成，
//
/////////////////////////////////////////////////////////////////////////////////////////////
bool CMainFrame::SchedulingTask(void) {
  for (auto pVirtualMarket : gl_vMarketPtr) {
    if (pVirtualMarket->IsReadyToRun()) pVirtualMarket->SchedulingTask();
  }
  return true;
}

bool CMainFrame::ResetMarket(void) {
  for (auto pMarket : gl_vMarketPtr) {
    if (pMarket->IsResetMarket()) {
      pMarket->ResetMarket();
      pMarket->SetResetMarket(false);
    }
  }
  return true;
}

bool CMainFrame::IsNeedResetMarket(void) {
  for (auto pMarket : gl_vMarketPtr) {
    if (pMarket->IsResetMarket()) return true;
  }
  return false;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) {
  if (!CMDIFrameWndEx::PreCreateWindow(cs))
    return FALSE;
  // TODO: 在此处通过修改
  //  CREATESTRUCT cs 来修改窗口类或样式

  return TRUE;
}

BOOL CMainFrame::CreateDockingWindows() {
  BOOL bNameValid;
  // 创建输出窗口
  CString strOutputWnd;
  bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
  ASSERT(bNameValid);
  if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
  {
    TRACE0("未能创建输出窗口\n");
    return FALSE; // 未能创建
  }

  SetDockingWindowIcons(theApp.m_bHiColorIcons);
  return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons) {
  HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
  m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

  UpdateMDITabbedBarsIcons();
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const {
  CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const {
  CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG

// CMainFrame 消息处理程序

void CMainFrame::OnWindowManager() {
  ShowWindowsDialog();
}

void CMainFrame::OnViewCustomize() {
  CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 扫描菜单*/);
  pDlgCust->EnableUserDefinedToolbars();
  pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp, LPARAM lp) {
  LRESULT lres = CMDIFrameWndEx::OnToolbarCreateNew(wp, lp);
  if (lres == 0)
  {
    return 0;
  }

  CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
  ASSERT_VALID(pUserToolbar);

  BOOL bNameValid;
  CString strCustomize;
  bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
  ASSERT(bNameValid);

  pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
  return lres;
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) {
  // 基类将执行真正的工作

  if (!CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
  {
    return FALSE;
  }

  // 为所有用户工具栏启用自定义按钮
  BOOL bNameValid;
  CString strCustomize;
  bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
  ASSERT(bNameValid);

  for (int i = 0; i < iMaxUserToolbars; i++)
  {
    CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
    if (pUserToolbar != nullptr)
    {
      pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
    }
  }

  return TRUE;
}

void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection) {
  CMDIFrameWndEx::OnSettingChange(uFlags, lpszSection);
  m_wndOutput.UpdateFonts();
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// 大部分系统定时操作的发起者，但具体工作由CVirtualMarket类的SchedulingTask()完成，本函数只完成显示实时信息的工作。
//
//
//
///////////////////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnTimer(UINT_PTR nIDEvent) {
  // TODO: 在此添加消息处理程序代码和/或调用默认值
  ASSERT(nIDEvent == __STOCK_ANALYSIS_TIMER__);
  // 重启系统在此处执行，容易调用各重置函数
  if (IsNeedResetMarket()) {
    ResetMarket();
  }

  // 调用主调度函数,由各市场调度函数执行具体任务
  SchedulingTask();

  //CMainFrame只执行更新状态任务
  UpdateStatus();

  if (gl_fTestMode) {
    gl_systemMessage.PushInformationMessage(_T("警告：使用了Test驱动"));
  }

  SysCallOnTimer(nIDEvent);
}

void CMainFrame::UpdateStatus(void) {
  CString str;
  char buffer[30];

  CChinaStockPtr pCurrentStock = gl_pChinaStockMarket->GetCurrentStock();

  //更新状态条
  // 显示股票代码和名称
  if (gl_pChinaStockMarket->IsCurrentStockChanged()) {
    gl_pChinaStockMarket->SetCurrentStockChanged(false);
    SysCallSetPaneText(2, (LPCTSTR)pCurrentStock->GetStockCode());
    SysCallSetPaneText(3, (LPCTSTR)pCurrentStock->GetStockName());
  }

  sprintf_s(buffer, _T("%d"), gl_pChinaStockMarket->GetCurrentSelectedStockSet());
  str = buffer;
  SysCallSetPaneText(4, (LPCTSTR)str);

  sprintf_s(buffer, _T("%d"), gl_pChinaStockMarket->GetCurrentSelectedPosition());
  str = buffer;
  SysCallSetPaneText(5, (LPCTSTR)str);

  if (gl_pChinaStockMarket->IsCurrentEditStockChanged()) {
    str = m_aStockCodeTemp;
    SysCallSetPaneText(1, (LPCTSTR)str);
    gl_pChinaStockMarket->SetCurrentEditStockChanged(false);
  }
  // 显示新浪实时数据读取时间（单位为毫秒）
  SysCallSetPaneText(6, (LPCTSTR)gl_pChinaStockMarket->GetStockCodeForInquiringRTData());

  // 显示活跃股票总数
  sprintf_s(buffer, _T("%d"), gl_pChinaStockMarket->GetTotalActiveStock());
  str = buffer;
  SysCallSetPaneText(7, (LPCTSTR)str);

  // 显示网易日线历史数据读取时间（单位为毫秒）
  SysCallSetPaneText(8, (LPCTSTR)gl_pChinaStockMarket->GetStockCodeForInquiringNeteaseDayLine());

  // 更新当前工作线程数
  sprintf_s(buffer, _T("%02d"), gl_ThreadStatus.GetNumberOfRunningThread());
  str = buffer;
  SysCallSetPaneText(9, (LPCTSTR)str);

  // 更新当前后台工作线程数
  sprintf_s(buffer, _T("%02d"), gl_ThreadStatus.HowManyBackGroundThreadsWorking());
  str = buffer;
  SysCallSetPaneText(10, (LPCTSTR)str);

  //更新当地时间的显示
  SysCallSetPaneText(11, (LPCTSTR)gl_pChinaStockMarket->GetLocalTimeString());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 当系统退出时，需要先退出工作线程。
//
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam) {
  // TODO: 在此添加消息处理程序代码和/或调用默认值
  if ((nID & 0Xfff0) == SC_CLOSE) { // 如果是退出系统
    gl_fExitingSystem = true; // 提示各工作线程中途退出
  }

  SysCallOnSysCommand(nID, lParam);
}

void CMainFrame::OnCalculateTodayRelativeStrong() {
  // TODO: 在此添加命令处理程序代码
  CalculateTodayRelativeStrong();
}

void CMainFrame::CalculateTodayRelativeStrong(void) {
  gl_pChinaStockMarket->RunningThreadBuildDayLineRS(gl_pChinaStockMarket->GetFormatedMarketDay());
}

void CMainFrame::OnProcessTodayStock() {
  // TODO: 在此添加命令处理程序代码
  if (gl_pChinaStockMarket->IsSystemReady()) {
    ProcessTodayStock();
  }
}

void CMainFrame::ProcessTodayStock() {
  gl_pChinaStockMarket->RunningThreadProcessTodayStock();
}

void CMainFrame::OnUpdateProcessTodayStock(CCmdUI* pCmdUI) {
  // TODO: 在此添加命令更新用户界面处理程序代码
  if (gl_pChinaStockMarket->IsSystemReady()) { // 系统自动更新日线数据时，不允许处理当日的实时数据。
    SysCallCmdUIEnable(pCmdUI, true);
  }
  else SysCallCmdUIEnable(pCmdUI, false);
}

void CMainFrame::OnUpdateCalculateTodayRelativeStrong(CCmdUI* pCmdUI) {
  // TODO: 在此添加命令更新用户界面处理程序代码
  if (gl_pChinaStockMarket->IsSystemReady()) {
    if (gl_ThreadStatus.IsCalculatingDayLineRS()) {
      SysCallCmdUIEnable(pCmdUI, false);
    }
    else {
      SysCallCmdUIEnable(pCmdUI, true);
    }
  }
  else {
    SysCallCmdUIEnable(pCmdUI, false);
  }
}

///////////////////////////////////////////////////////////////////
//
// 转发WM_CHAR和WM_KEYUP梁消息至本主框架，由主框架处理之。
//
//
//////////////////////////////////////////////////////////////////
BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) {
  // TODO: 在此添加专用代码和/或调用基类
  if ((pMsg->message == WM_CHAR) || (pMsg->message == WM_KEYUP)) {
    SysCallSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
  }

  return SysCallPreTranslateMessage(pMsg);
}

void CMainFrame::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
  // TODO: 在此添加消息处理程序代码和/或调用默认值
  CChinaStockPtr pStock;
  CString strTemp;

  switch (nChar) {
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
  case 's':
  case 'h':
  case 'z':
  if (m_lCurrentPos < 8) {
    m_aStockCodeTemp[m_lCurrentPos] = nChar;
    m_lCurrentPos++;
    m_aStockCodeTemp[m_lCurrentPos] = 0x000;
  }
  gl_pChinaStockMarket->SetCurrentEditStockChanged(true);
  break;
  case 0x00d: // 回车
  strTemp = m_aStockCodeTemp;
  if (gl_pChinaStockMarket->IsStock(strTemp)) {
    pStock = gl_pChinaStockMarket->GetStock(strTemp);
    gl_pChinaStockMarket->SetCurrentStock(pStock);
    SysCallInvalidate();
  }
  m_aStockCodeTemp[0] = 0x000;
  m_lCurrentPos = 0;
  gl_pChinaStockMarket->SetCurrentEditStockChanged(true);
  break;
  case 0x008: // back space
  if (m_lCurrentPos > 0) {
    m_lCurrentPos--;
    m_aStockCodeTemp[m_lCurrentPos] = 0x000;
    gl_pChinaStockMarket->SetCurrentEditStockChanged(true);
  }
  break;
  default:
  break;
  }
  if (gl_pChinaStockMarket->IsCurrentStockChanged()) {
    CMDIChildWnd* pChild = (CMDIChildWnd*)GetActiveFrame();
    CStockAnalysisView* pView = (CStockAnalysisView*)pChild->GetActiveView();
    if (pView != nullptr) {
      pView->UpdateHistoryDataContainer(gl_pChinaStockMarket->GetCurrentStock());
    }
  }

  SysCallOnChar(nChar, nRepCnt, nFlags);
}

void CMainFrame::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {
  // TODO: 在此添加消息处理程序代码和/或调用默认值
  CChinaStockPtr pStock;
  long lIndex = 0;
  CString strTemp;
  CChinaStockPtr pCurrentStock = gl_pChinaStockMarket->GetCurrentStock();

  if (pCurrentStock != nullptr) {
    switch (nChar) {
    case 118: // F7，选择前一个股票集
    gl_pChinaStockMarket->ChangeToPrevStockSet();
    gl_pChinaStockMarket->SetCurrentSelectedPosition(0);
    gl_pChinaStockMarket->SetCurrentStock(gl_pChinaStockMarket->GetCurrentSelectedStock());
    break;
    case 119: // F8， 选择后一个股票集
    gl_pChinaStockMarket->ChangeToNextStockSet();
    gl_pChinaStockMarket->SetCurrentSelectedPosition(0);
    gl_pChinaStockMarket->SetCurrentStock(gl_pChinaStockMarket->GetCurrentSelectedStock());
    break;
    case 33: // PAGE UP
     // last stock
    gl_pChinaStockMarket->ChangeToPrevStock();
    break;
    case 34: // PAGE DOWN
      // next stock
    gl_pChinaStockMarket->ChangeToNextStock();
    break;
    case 45: // Ins, 加入自选股票
    pCurrentStock->SetChoiced(true);
    if (gl_pChinaStockMarket->AddChoicedStock(pCurrentStock)) {
      gl_pChinaStockMarket->SetUpdateChoicedStockDB(true);
    }
    break;
    case 46: // delete,从自选股票池中删除
    pCurrentStock->SetChoiced(false);
    if (gl_pChinaStockMarket->DeleteChoicedStock(pCurrentStock)) {
      gl_pChinaStockMarket->SetUpdateChoicedStockDB(true);
    }
    break;
    default:
    // 无需处理字符，略过
    break;
    }
  }
  if (gl_pChinaStockMarket->IsCurrentStockChanged()) {
    CMDIChildWnd* pChild = (CMDIChildWnd*)GetActiveFrame();
    CStockAnalysisView* pView = (CStockAnalysisView*)pChild->GetActiveView();
    if (pView != nullptr) {
      pView->UpdateHistoryDataContainer(gl_pChinaStockMarket->GetCurrentStock());
    }
  }
  SysCallOnKeyUp(nChar, nRepCnt, nFlags);
}

void CMainFrame::OnRebuildDaylineRS() {
  // TODO: Add your command handler code here
  gl_pChinaStockMarket->RunningThreadBuildDayLineRS(__CHINA_MARKET_BEGIN_DAY__);
}

void CMainFrame::OnBuildResetMarket() {
  // TODO: Add your command handler code here
  for (auto pMarket : gl_vMarketPtr) {
    pMarket->SetResetMarket(true);
  }
}

void CMainFrame::OnUpdateRebuildDaylineRS(CCmdUI* pCmdUI) {
  // TODO: Add your command update UI handler code here
  // 要避免在八点至半九点半之间执行重算相对强度的工作，因为此时间段时要重置系统，结果导致程序崩溃。
  if ((gl_pChinaStockMarket->GetFormatedMarketTime() > 83000) && (gl_pChinaStockMarket->GetFormatedMarketTime() < 93000)) {
    SysCallCmdUIEnable(pCmdUI, false);
  }
  else if (gl_ThreadStatus.IsCalculatingDayLineRS()) {
    SysCallCmdUIEnable(pCmdUI, false);
  }
  else {
    SysCallCmdUIEnable(pCmdUI, true);
  }
}

void CMainFrame::OnAbortBuindingRS() {
  // TODO: Add your command handler code here
  ASSERT(gl_fExitingCalculatingRS == false);
  gl_fExitingCalculatingRS = true;
}

void CMainFrame::OnUpdateAbortBuindingRS(CCmdUI* pCmdUI) {
  // TODO: Add your command update UI handler code here
  if (gl_ThreadStatus.IsCalculatingDayLineRS()) {
    SysCallCmdUIEnable(pCmdUI, true);
  }
  else {
    SysCallCmdUIEnable(pCmdUI, false);
  }
}

void CMainFrame::OnRecordRTData() {
  // TODO: Add your command handler code here
  if (gl_pChinaStockMarket->IsRecordingRTData()) gl_pChinaStockMarket->SetRecordRTData(false);
  else gl_pChinaStockMarket->SetRecordRTData(true);
}

void CMainFrame::OnUpdateRecordRTData(CCmdUI* pCmdUI) {
  // TODO: Add your command update UI handler code here
  if (gl_pChinaStockMarket->IsRecordingRTData()) SysCallCmdUISetCheck(pCmdUI, true);
  else SysCallCmdUISetCheck(pCmdUI, false);
}

void CMainFrame::OnCalculate10dayRS1() {
  // TODO: Add your command handler code here
  gl_pChinaStockMarket->RunningThreadChoice10RSStrong1StockSet();
  gl_pChinaStockMarket->SetChoiced10RSStrong1StockSet(true);
}

void CMainFrame::OnCalculate10dayRS2() {
  // TODO: Add your command handler code here
  gl_pChinaStockMarket->RunningThreadChoice10RSStrong2StockSet();
  gl_pChinaStockMarket->SetChoiced10RSStrong2StockSet(true);
}

void CMainFrame::OnCalculate10dayRS() {
  // TODO: Add your command handler code here
  gl_pChinaStockMarket->RunningThreadChoice10RSStrongStockSet();
  gl_pChinaStockMarket->SetChoiced10RSStrongStockSet(true);
}

void CMainFrame::OnUpdateCalculate10dayRS1(CCmdUI* pCmdUI) {
  // TODO: Add your command update UI handler code here
  if ((gl_pChinaStockMarket->GetDayLineNeedUpdateNumber() > 0) && (gl_pChinaStockMarket->GetDayLineNeedSaveNumber() > 0)) {
    SysCallCmdUIEnable(pCmdUI, false);
  }
  else SysCallCmdUIEnable(pCmdUI, true);
}

void CMainFrame::OnUpdateCalculate10dayRS2(CCmdUI* pCmdUI) {
  // TODO: Add your command update UI handler code here
  if ((gl_pChinaStockMarket->GetDayLineNeedUpdateNumber() > 0) && (gl_pChinaStockMarket->GetDayLineNeedSaveNumber() > 0)) {
    SysCallCmdUIEnable(pCmdUI, false);
  }
  else SysCallCmdUIEnable(pCmdUI, true);
}

void CMainFrame::OnUpdateCalculate10dayRS(CCmdUI* pCmdUI) {
  // TODO: Add your command update UI handler code here
  if ((gl_pChinaStockMarket->GetDayLineNeedUpdateNumber() > 0) && (gl_pChinaStockMarket->GetDayLineNeedSaveNumber() > 0)) {
    SysCallCmdUIEnable(pCmdUI, false);
  }
  else SysCallCmdUIEnable(pCmdUI, true);
}

void CMainFrame::OnStopUpdateDayLine() {
  // TODO: Add your command handler code here
  gl_pChinaStockMarket->ClearDayLineNeedUpdaeStatus();
}

void CMainFrame::OnUsingNeteaseRealtimeDataServer() {
  // TODO: Add your command handler code here
  gl_pChinaStockMarket->SetUsingNeteaseRTDataServer();
}

void CMainFrame::OnUsingSinaRealtimeDataServer() {
  // TODO: Add your command handler code here
  gl_pChinaStockMarket->SetUsingSinaRTDataServer();
}

void CMainFrame::OnUpdateUsingNeteaseRealtimeDataServer(CCmdUI* pCmdUI) {
  // TODO: Add your command update UI handler code here
  if (gl_pChinaStockMarket->IsUsingNeteaseRTDataServer()) {
    SysCallCmdUISetCheck(pCmdUI, true);
  }
  else {
    SysCallCmdUISetCheck(pCmdUI, false);
  }
}

void CMainFrame::OnUpdateUsingSinaRealtimeDataServer(CCmdUI* pCmdUI) {
  // TODO: Add your command update UI handler code here
  if (gl_pChinaStockMarket->IsUsingSinaRTDataServer()) {
    SysCallCmdUISetCheck(pCmdUI, true);
  }
  else {
    SysCallCmdUISetCheck(pCmdUI, false);
  }
}

void CMainFrame::OnBuildCreateWeekLine() {
  // TODO: Add your command handler code here
  gl_pChinaStockMarket->RunningThreadBuildWeekLine();
}

void CMainFrame::OnUpdateBuildCreateWeekLine(CCmdUI* pCmdUI) {
  // TODO: Add your command update UI handler code here
}

void CMainFrame::OnRebuildWeekLineRS() {
  // TODO: Add your command handler code here
  gl_pChinaStockMarket->RunningThreadBuildWeekLineRS();
}

void CMainFrame::OnUpdateRebuildWeekLineRS(CCmdUI* pCmdUI) {
  // TODO: Add your command update UI handler code here
}

void CMainFrame::OnBuildCurrentWeekLine() {
  // TODO: Add your command handler code here
  gl_pChinaStockMarket->RunningThreadBuildWeekLineOfCurrentWeek();
}

void CMainFrame::OnUpdateBuildCurrentWeekLine(CCmdUI* pCmdUI) {
  // TODO: Add your command update UI handler code here
}