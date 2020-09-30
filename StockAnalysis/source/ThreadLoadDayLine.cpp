#include"globedef.h"

#include"ChinaMarket.h"
#include"Thread.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 日线装载工作线程
//
// 从数据库中装入相应股票的日线数据，然后计算各相对强度
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
UINT ThreadLoadDL(CChinaStockPtr pStock) {
  ASSERT(pStock != nullptr);

  gl_ThreadStatus.IncreaseRunningThread();
  pStock->UnloadDL();
  // 装入日线数据
  pStock->LoadDL(pStock->GetStockCode());
  // 计算各相对强度（以指数相对强度为默认值）
  pStock->CalculateDLRSIndex();
  pStock->SetDLLoaded(true);
  gl_ThreadStatus.DecreaseRunningThread();

  return 16;
}