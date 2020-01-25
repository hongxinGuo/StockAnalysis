#include"stdafx.h"
#include"globedef.h"

#include "Stock.h"

#include"WebDataInquirer.h"

bool CWebInquirer::GetNeteaseDayLineData(void) {
  // 抓取日线数据.
  // 最多使用四个引擎，否则容易被网易服务器拒绝服务。一般还是用两个为好。
  switch (gl_cMaxSavingOneDayLineThreads) {
  case 8: case 7: case 6:
  m_NeteaseDayLineWebDataSixth.GetWebData(); // 网易日线历史数据
  case 5:
  m_NeteaseDayLineWebDataFifth.GetWebData();
  case 4:
  m_NeteaseDayLineWebDataFourth.GetWebData();
  case 3:
  m_NeteaseDayLineWebDataThird.GetWebData();
  case 2:
  m_NeteaseDayLineWebDataSecond.GetWebData();
  case 1: case 0:
  m_NeteaseDayLineWebData.GetWebData();
  break;
  default:
  m_NeteaseDayLineWebData.GetWebData();
  TRACE(_T("Out of range in Get Newease DayLine Web Data\n"));
  break;
  }
  return true;
}