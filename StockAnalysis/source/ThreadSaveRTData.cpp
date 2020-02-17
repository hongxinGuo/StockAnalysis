//////////////////////////////////////////////////////////////////////////////////////
//
// 存储当前所选股票的实时数据的工作线程
//
//
// 返回值为19
//
///////////////////////////////////////////////////////////////////////////////////////
#include"ChinaMarket.h"

UINT ThreadSaveRTData(LPVOID) {
  ASSERT(gl_ChinaStockMarket.IsSystemReady()); // 调用本工作线程时必须设置好市场。
  gl_ChinaStockMarket.SaveRTData();
  return 19;
}