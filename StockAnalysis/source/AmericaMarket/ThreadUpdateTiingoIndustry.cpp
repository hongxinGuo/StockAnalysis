#include"pch.h"
#include"globedef.h"

UINT ThreadUpdateTiingoIndustry(not_null<CWorldMarket*> pMarket) {
  gl_ThreadStatus.IncreaseSavingThread();
  gl_UpdateWorldMarketDB.Wait();
  pMarket->UpdateTiingoIndustry();
  gl_UpdateWorldMarketDB.Signal();
  gl_ThreadStatus.DecreaseSavingThread();

  return 45;
}