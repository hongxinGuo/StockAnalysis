///////////////////////////////////////////////////////////////////////////////////
//
// 从活跃股票集中挑选强势股票。
//
//
///////////////////////////////////////////////////////////////////////////////////
#include"globedef.h"
#include"Thread.h"

#include"ChinaMarket.h"

UINT ThreadChoice10RSStrongStockSet(CChinaMarket* pMarket) {
  gl_ThreadStatus.IncreaseRunningThread();

  // 添加一个注释
  pMarket->Choice10RSStrongStockSet();

  gl_ThreadStatus.DecreaseRunningThread();

  return 23;
}