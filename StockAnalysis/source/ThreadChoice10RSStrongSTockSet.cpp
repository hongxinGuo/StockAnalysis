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
  pMarket->Choice10RSStrong2StockSet();

  gl_systemMessage.PushInformationMessage(_T("10日RS2计算完毕\n"));
  gl_ChoiceRSStrong.Signal();
  gl_ThreadStatus.DecreaseRunningThread();

  return 23;
}

UINT ThreadChoice10RSStrong1StockSet(CChinaMarket* pMarket) {
  gl_ThreadStatus.IncreaseRunningThread();
  gl_ChoiceRSStrong.Wait();
  gl_systemMessage.PushInformationMessage(_T("开始计算10日RS1\n"));

  // 添加一个注释
  pMarket->Choice10RSStrong1StockSet();

  gl_systemMessage.PushInformationMessage(_T("10日RS1计算完毕\n"));
  gl_ChoiceRSStrong.Signal();
  gl_ThreadStatus.DecreaseRunningThread();

  return 24;
}