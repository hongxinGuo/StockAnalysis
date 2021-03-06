#include"pch.h"
#include"globedef.h"

UINT ThreadUpdateCountryListDB(not_null<CWorldMarket*> pMarket) {
  gl_ThreadStatus.IncreaseSavingThread();
  pMarket->UpdateCountryListDB();
  gl_ThreadStatus.DecreaseSavingThread();
  return 40;
}