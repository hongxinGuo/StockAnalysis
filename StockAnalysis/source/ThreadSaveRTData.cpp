//////////////////////////////////////////////////////////////////////////////////////
//
// 存储当前所选股票的实时数据的工作线程
//
//
// 返回值为19
//
///////////////////////////////////////////////////////////////////////////////////////
#include"globedef.h"

#include"ChinaMarket.h"

UINT ThreadSaveRTData(void) {
  ASSERT(gl_pChinaStockMarket->IsSystemReady()); // 调用本工作线程时必须设置好市场。
  gl_pChinaStockMarket->SaveRTData();
  return 19;
}