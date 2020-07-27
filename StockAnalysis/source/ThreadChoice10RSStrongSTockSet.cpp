///////////////////////////////////////////////////////////////////////////////////
//
// 从活跃股票集中挑选强势股票。
//
//
///////////////////////////////////////////////////////////////////////////////////
#include"globedef.h"
#include"Thread.h"

#include"ChinaMarket.h"

Semaphore gl_ChoiceRSStrong(1); //由于各种选择股票集的工作线程都操作日线历史数据，故而同一时间只允许一个线程执行，这样能够减少同步问题。

UINT ThreadChoice10RSStrong2StockSet(CChinaMarket* pMarket) {
  gl_ThreadStatus.IncreaseRunningThread();
  gl_ChoiceRSStrong.Wait();
  gl_systemMessage.PushInformationMessage(_T("开始计算10日RS2\n"));

  // 添加一个注释
  if (pMarket->Choice10RSStrong2StockSet()) {
    gl_systemMessage.PushInformationMessage(_T("10日RS2计算完毕\n"));
    pMarket->SetUpdatedDayFor10DayRS2(pMarket->GetFormatedMarketDay());
    pMarket->SetUpdateOptionDB(true); // 更新选项数据库
  }
  gl_ChoiceRSStrong.Signal();
  gl_ThreadStatus.DecreaseRunningThread();

  return 102;
}

UINT ThreadChoice10RSStrong1StockSet(CChinaMarket* pMarket) {
  gl_ThreadStatus.IncreaseRunningThread();
  gl_ChoiceRSStrong.Wait();
  gl_systemMessage.PushInformationMessage(_T("开始计算10日RS1\n"));

  // 添加一个注释
  if (pMarket->Choice10RSStrong1StockSet()) {
    gl_systemMessage.PushInformationMessage(_T("10日RS1计算完毕\n"));
    pMarket->SetUpdatedDayFor10DayRS1(pMarket->GetFormatedMarketDay());
    pMarket->SetUpdateOptionDB(true); // 更新选项数据库
  }
  gl_ChoiceRSStrong.Signal();
  gl_ThreadStatus.DecreaseRunningThread();

  return 101;
}

UINT ThreadChoice10RSStrongStockSet(CChinaMarket* pMarket, CRSReference* pRef, int iIndex) {
  gl_ThreadStatus.IncreaseRunningThread();
  gl_ChoiceRSStrong.Wait();
  CString str = _T("开始计算10日RS ");
  char buffer[30];
  sprintf_s(buffer, _T("%d"), iIndex);
  str += buffer;
  str += _T("\n");
  gl_systemMessage.PushInformationMessage(str);

  // 添加一个注释
  if (pMarket->Choice10RSStrongStockSet(pRef, iIndex)) {
    str = _T("10日RS ");
    str += buffer;
    str += _T("计算完毕\n");
    gl_systemMessage.PushInformationMessage(str);
    //pMarket->SetUpdatedDayFor10DayRS1(pMarket->GetFormatedMarketDay());
    //pMarket->SetUpdateOptionDB(true); // 更新选项数据库
  }
  gl_ChoiceRSStrong.Signal();
  gl_ThreadStatus.DecreaseRunningThread();

  return 103;
}

UINT ThreadCalculate10RSStrongStock(vector<CChinaStockPtr>* pv10RSStrongStock, CRSReference* pRef, CChinaStockPtr pStock) {
  gl_ThreadStatus.IncreaseRunningThread();
  gl_SemaphoreBackGroundTaskThreads.Wait();
  gl_ThreadStatus.IncreaseBackGroundWorkingthreads();     // 正在工作的线程数加一
  if (gl_fExitingSystem) return 34;

  if (gl_pChinaStockMarket->IsAStock(pStock) && pStock->IsActive()) {
    if (!pStock->IsDayLineLoaded()) {
      pStock->LoadDayLine();
      pStock->SetDayLineLoaded(true);
    }
    if (pStock->Calculate10RSStrongStockSet(pRef)) {
      pv10RSStrongStock->push_back(pStock);
    }
    if (!pStock->IsSameStock(gl_pChinaStockMarket->GetCurrentStock())) {
      pStock->UnloadDayLine();
    }
  }
  gl_ThreadStatus.DecreaseBackGroundWorkingthreads(); // 正在工作的线程数减一
  gl_SemaphoreBackGroundTaskThreads.Signal();
  gl_ThreadStatus.DecreaseRunningThread();
  return 104;
}

UINT ThreadCalculate10RSStrong1Stock(vector<CChinaStockPtr>* pv10RSStrongStock, CChinaStockPtr pStock) {
  gl_ThreadStatus.IncreaseRunningThread();
  gl_SemaphoreBackGroundTaskThreads.Wait();
  gl_ThreadStatus.IncreaseBackGroundWorkingthreads();     // 正在工作的线程数加一
  if (gl_fExitingSystem) return 35;

  if (gl_pChinaStockMarket->IsAStock(pStock) && pStock->IsActive()) {
    if (!pStock->IsDayLineLoaded()) {
      pStock->LoadDayLine();
      pStock->SetDayLineLoaded(true);
    }
    if (pStock->Calculate10RSStrong1StockSet()) {
      pv10RSStrongStock->push_back(pStock);
    }
    if (!pStock->IsSameStock(gl_pChinaStockMarket->GetCurrentStock())) {
      pStock->UnloadDayLine();
    }
  }
  gl_ThreadStatus.DecreaseBackGroundWorkingthreads(); // 正在工作的线程数减一
  gl_SemaphoreBackGroundTaskThreads.Signal();
  gl_ThreadStatus.DecreaseRunningThread();
  return 105;
}

UINT ThreadCalculate10RSStrong2Stock(vector<CChinaStockPtr>* pv10RSStrongStock, CChinaStockPtr pStock) {
  gl_ThreadStatus.IncreaseRunningThread();
  gl_SemaphoreBackGroundTaskThreads.Wait();
  gl_ThreadStatus.IncreaseBackGroundWorkingthreads();     // 正在工作的线程数加一
  if (gl_fExitingSystem) return 36;

  if (gl_pChinaStockMarket->IsAStock(pStock) && pStock->IsActive()) {
    if (!pStock->IsDayLineLoaded()) {
      pStock->LoadDayLine();
      pStock->SetDayLineLoaded(true);
    }
    if (pStock->Calculate10RSStrong2StockSet()) {
      pv10RSStrongStock->push_back(pStock);
    }
    if (!pStock->IsSameStock(gl_pChinaStockMarket->GetCurrentStock())) {
      pStock->UnloadDayLine();
    }
  }
  gl_ThreadStatus.DecreaseBackGroundWorkingthreads(); // 正在工作的线程数减一
  gl_SemaphoreBackGroundTaskThreads.Signal();
  gl_ThreadStatus.DecreaseRunningThread();
  return 106;
}