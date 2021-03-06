#include"pch.h"
#include"globedef.h"

UINT ThreadUpdateEPSSurpriseDB(not_null<CWorldStock*> pStock) {
  gl_ThreadStatus.IncreaseSavingThread();
  gl_UpdateWorldMarketDB.Wait();
  pStock->UpdateEPSSurpriseDB();
  gl_UpdateWorldMarketDB.Signal();
  gl_ThreadStatus.DecreaseSavingThread();

  return 41;
}