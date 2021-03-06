#include"pch.h"
#include"globedef.h"

UINT ThreadUpdateForexExchangeDB(not_null<CWorldMarket*> pMarket) {
  gl_ThreadStatus.IncreaseSavingThread();
  gl_UpdateWorldMarketDB.Wait();
  pMarket->UpdateForexExchangeDB();
  gl_UpdateWorldMarketDB.Signal();
  gl_ThreadStatus.DecreaseSavingThread();

  return 49;
}