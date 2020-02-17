//////////////////////////////////////////////////////////////////////////////////////
//
// 存储临时实时数据工作线程
//
// 此线程与计算实时数据线程互斥，需要注意之。
//
// 返回值为13
//
///////////////////////////////////////////////////////////////////////////////////////
#include"ChinaMarket.h"

UINT ThreadSaveTempRTData(LPVOID) {
  ASSERT(gl_ChinaStockMarket.IsSystemReady()); // 调用本工作线程时必须设置好市场。
  ASSERT(!gl_ThreadStatus.IsCalculatingRTData()); // 此两个工作线程互斥

  gl_ThreadStatus.SetSavingTempData(true);

  gl_ChinaStockMarket.UpdateTodayTempDB();

  ASSERT(!gl_ThreadStatus.IsCalculatingRTData()); // 再次确认一下
  gl_ThreadStatus.SetSavingTempData(false);

  return 13;
}