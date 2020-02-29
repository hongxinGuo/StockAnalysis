#include"globedef.h"

#include"DayLine.h"
#include"ChinaMarket.h"

#include"Thread.h"

using namespace std;
#include<memory>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 此线程由系统在收市后于15:05自动唤醒，每日只执行一次
//
//
//由于存在周六和周日，故而当前交易日并不一定就是当前日期，而可能是周五
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT ThreadProcessCurrentTradeDayStock(LPVOID) {
  ASSERT(gl_ChinaStockMarket.IsSystemReady()); // 调用本工作线程时必须设置好市场。

  char buffer[30];
  CString str;
  int i;
  long lDay = FormatToDay(gl_ChinaStockMarket.GetNewestTransactionTime());
  if (lDay == gl_ChinaStockMarket.GetDay()) {
    i = gl_ChinaStockMarket.ProcessCurrentTradeDayStock(lDay);
    TRACE("处理今日股票\n");
    sprintf_s(buffer, "%d", i);
    str = _T("今日处理了");
    str += buffer;
    str += _T("个股票");
    gl_systemMessage.PushInformationMessage(str);
    AfxBeginThread(ThreadCalculateThisDayRS, (LPVOID)lDay, THREAD_PRIORITY_LOWEST);
    if (gl_ChinaStockMarket.GetTime() > 150400) {   // 如果中国股市闭市了
      gl_ChinaStockMarket.SetRelativeStrongEndDay(gl_ChinaStockMarket.GetDay());
      gl_ChinaStockMarket.SetUpdateStockCodeDB(true);  // 更新代码。
      gl_ChinaStockMarket.SetUpdateOptionDB(true);   // 更新状态
      gl_ChinaStockMarket.SetTodayStockProcessed(true);  // 设置今日已处理标识
    }
  }
  return 14;
}