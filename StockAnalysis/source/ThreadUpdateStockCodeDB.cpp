#include"globedef.h"

#include"DayLine.h"
#include"ChinaMarket.h"

#include"Thread.h"

//////////////////////////////////////////////////////////////////////////////////////
//
//
//
//
///////////////////////////////////////////////////////////////////////////////////////
UINT ThreadUpdateStockCodeDB(LPVOID) {
  //
  gl_ChinaStockMarket.UpdateStockCodeDB();
  return 18;
}

UINT ThreadUpdateOptionDB(LPVOID) {
  gl_ChinaStockMarket.UpdateOptionDB();
  return 20;
}