/////////////////////////////////////////////////////////////////////////////////////////////////
//
// 从网络文件file中读取网易制式实时数据，返回值是所读数据是否出现格式错误。
// 开始处为第一个{，结束处为倒数第二个}。如果尚有数据需处理，则被处理的字符为','；如果没有数据了，则被处理的字符为' '。
//
// 要获取最新行情，访问数据接口：http://api.money.126.net/data/feed/0601872
//
// _ntes_quote_callback({"0601872":{"code": "0601872", "percent": 0.038251, "high": 5.72, "askvol3": 311970, "askvol2": 257996,
//                      "askvol5": 399200, "askvol4": 201000, "price": 5.7, "open": 5.53, "bid5": 5.65, "bid4": 5.66, "bid3": 5.67,
//                       "bid2": 5.68, "bid1": 5.69, "low": 5.51, "updown": 0.21, "type": "SH", "symbol": "601872", "status": 0,
//                       "ask4": 5.73, "bidvol3": 234700, "bidvol2": 166300, "bidvol1": 641291, "update": "2019/11/04 15:59:54",
//                       "bidvol5": 134500, "bidvol4": 96600, "yestclose": 5.49, "askvol1": 396789, "ask5": 5.74, "volume": 78750304,
//                       "ask1": 5.7, "name": "\u62db\u5546\u8f6e\u8239", "ask3": 5.72, "ask2": 5.71, "arrow": "\u2191",
//                        "time": "2019/11/04 15:59:52", "turnover": 443978974} });
//
// 网易实时数据缺少关键性的成交金额一项，故而无法作为基本数据，只能作为补充用。
//
//////////////////////////////////////////////////////////////////////////////////////////////////
#include"stdafx.h"
#include"globedef.h"
#include"Market.h"

#include"thread.h"
#include "NeteaseRTWebData.h"

CNeteaseRTWebData::CNeteaseRTWebData() : CWebData() {
  m_strWebDataInquirePrefix = _T("http://api.money.126.net/data/feed/");
  m_strWebDataInquireSuffix = _T("");
}

CNeteaseRTWebData::~CNeteaseRTWebData() {
}

bool CNeteaseRTWebData::ReadPrefix(void) {
  static char buffer1[200];
  static CString strHeader = _T("_ntes_quote_callback({");

  strncpy_s(buffer1, m_pCurrentPos, 22); // 读入"_ntes_quote_callback({"
  buffer1[22] = 0x000;
  CString str1;
  str1 = buffer1;
  if (strHeader.Compare(str1) != 0) { // 数据格式出错
    return false;
  }
  IncreaseCurrentPos(22);

  return true;
}

bool CNeteaseRTWebData::IsReadingFinished(void) {
  if ((*m_pCurrentPos == ' ') || (m_lCurrentPos >= (m_lByteRead - 4))) { // 到结尾处了
    return true;
  }
  else {
    return false;
  }
}

bool CNeteaseRTWebData::ReportStatus(long lNumberOfData) {
  TRACE("读入%d个网易实时数据\n", lNumberOfData);
  return true;
}

bool CNeteaseRTWebData::SucceedReadingAndStoringOneWebData(void) {
  CRTDataPtr pRTData = make_shared<CRTData>();
  CString strVolume;
  char buffer[200];

  pRTData->SetDataSource(__NETEASE_RT_WEB_DATA__);
#ifdef DEBUG
  // 测试网易实时数据与新浪实时数据的同一性。
  if (gl_TESTpRTData != nullptr) {
    if (pRTData->GetStockCode().Compare(gl_TESTpRTData->GetStockCode()) == 0) {
      sprintf_s(buffer, "volume: %I64d, askvol1: %d, askvol2: %d, askvol3: %d, askvol4: %d, askvol5: %d",
                pRTData->GetVolume(), pRTData->GetVSell(0), pRTData->GetVSell(1), pRTData->GetVSell(2), pRTData->GetVSell(3), pRTData->GetVSell(4));
      strVolume = _T("2  ");
      strVolume += buffer;
      gl_systemMessage.PushInnerSystemInformationMessage(strVolume);
    }
    if (gl_ChinaStockMarket.IsUsingNeteaseRTDataReceiverAsTester()) {
      CString str;
      if (pRTData->IsActive()) {
        CStockPtr pStock = nullptr;
        if ((pStock = gl_ChinaStockMarket.GetStockPtr(pRTData->GetStockCode())) != nullptr) {
          if (!pStock->IsActive()) {
            str = pStock->GetStockCode();
            str += _T(" 网易实时检测到不处于活跃状态");
            gl_systemMessage.PushInnerSystemInformationMessage(str);
          }
        }
        else {
          str = pRTData->GetStockCode();
          str += _T(" 无效股票代码（网易实时数据）");
          gl_systemMessage.PushInnerSystemInformationMessage(str);
          return false;
        }
      }
    }
#endif // DEBUG
    //gl_QueueSinaRTData.PushRTData(pRTData); // 将此实时数据指针存入实时数据队列.网易实时数据缺少总成交金额一项，只能作为辅助数据，故而暂时不使用。
    return true;
  }
  return false;
}

void CNeteaseRTWebData::ProcessWebDataStored(void) {
  //将下面的函数移入定时调度处，每3秒执行一次即可。本函数无需做任何事情，
  //gl_ChinaStockMarket.DistributeRTDataReceivedFromWebToProperStock();
}

void CNeteaseRTWebData::ReportDataError(void) {
  TRACE("网易实时数据有误,抛掉不用\n");
  CString str;
  str = _T("网易实时数据有误");
  gl_systemMessage.PushInformationMessage(str);
}

void CNeteaseRTWebData::ReportCommunicationError(void) {
  TRACE("Error reading http file ：http://api.money.126.net/data/feed/\n");
  CString str;
  str = _T("Error reading http file ：http://api.money.126.net/data/feed/");
  gl_systemMessage.PushInformationMessage(str);
}

void CNeteaseRTWebData::InquireNextWebData(void) {
  CString strMiddle = _T("");

  // 申请下一批次股票实时数据
  if (!gl_ChinaStockMarket.SystemReady() || gl_ChinaStockMarket.IsUsingNeteaseRTDataReceiverAsTester()) { // 如果系统尚未准备好，则使用全局股票池
    GetInquiringStr(strMiddle, 700, false);
  }
  else { // 开市时使用今日活跃股票池
    GetInquiringStr(strMiddle, 700, true);
  }
  CreateTotalInquiringString(strMiddle);

  SetWebDataReceived(false);
  SetReadingWebData(true);  // 在此先设置一次，以防重入（线程延迟导致）
  StartReadingThread();
}

int CNeteaseRTWebData::GetInquiringStr(CString& strInquire, long lTotalNumber, bool fSkipUnactiveStock) {
  return gl_ChinaStockMarket.GetNeteaseInquiringStockStr(strInquire, lTotalNumber, fSkipUnactiveStock);
}

void CNeteaseRTWebData::StartReadingThread(void) {
  AfxBeginThread(ThreadReadNeteaseRTData, nullptr);
}