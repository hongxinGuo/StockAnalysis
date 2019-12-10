#include"stdafx.h"
#include"globedef.h"
#include"thread.h"
#include"market.h"

#include "SinaWebRTData.h"

CSinaWebRTData::CSinaWebRTData() : CWebData() {
  m_strWebDataInquirePrefix = _T("http://hq.sinajs.cn/list=");
  m_strWebDataInquireSuffix = _T("");
}

CSinaWebRTData::~CSinaWebRTData() {
}

bool CSinaWebRTData::GetWebData(void) {
  if (!IsReadingWebData()) {
    InquireNextWebData();
  }
  return true;
}

void CSinaWebRTData::InquireNextWebData(void) {
  static int iCountUp = 0;
  CRTDataPtr pRTData = nullptr;
  static int iTotalInquiringStocks = 0;

  CString strMiddle = _T("");

  // 申请下一批次股票实时数据
  if (gl_ChinaStockMarket.IsCheckTodayActiveStock() || !gl_ChinaStockMarket.SystemReady()) { // 如果处于寻找今日活跃股票期间（9:10--9:29, 11:31--12:59),则使用全局股票池
    iTotalInquiringStocks += GetInquiringStr(strMiddle, 900, false);
    if (iTotalInquiringStocks > 36000) {
      if (!gl_ChinaStockMarket.SystemReady()) { // 如果系统尚未设置好，则显示系统准备
        gl_systemMessage.PushInformationMessage(_T("完成系统初始化"));
      }
      gl_ChinaStockMarket.SetSystemReady(true); // 所有的股票实时数据都轮询三遍，当日活跃股票集已经建立，故而可以接受日线数据了。
    }
  }
  else { // 开市时使用今日活跃股票池
    GetInquiringStr(strMiddle, 900, true);
  }
  CreateTotalInquiringString(strMiddle);

  SetReadingWebData(true);  // 在此先设置一次，以防重入（线程延迟导致）
  StartReadingThread();
}

int CSinaWebRTData::GetInquiringStr(CString& strInquire, long lTotalNumber, bool fSkipUnactiveStock) {
  return gl_ChinaStockMarket.GetSinaInquiringStockStr(strInquire, lTotalNumber, fSkipUnactiveStock);
}

void CSinaWebRTData::StartReadingThread(void) {
  AfxBeginThread(ThreadReadSinaRTData, this);
}

bool CSinaWebRTData::ReportStatus(long lNumberOfData) {
  TRACE("读入%d个新浪实时数据\n", lNumberOfData);
  return true;
}