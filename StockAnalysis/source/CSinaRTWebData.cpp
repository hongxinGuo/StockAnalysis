#include"stdafx.h"
#include"globedef.h"
#include"market.h"

#include "CSinaRTWebData.h"

bool CSinaRTWebData::ReadAndSaveWebData(char*& pCurrentPos, long& iCount)
{
  CRTDataPtr pRTData = make_shared<CRTData>();
  if (pRTData->ReadSinaData(pCurrentPos, iCount)) {
    gl_QueueRTData.PushRTData(pRTData); // 将此实时数据指针存入实时数据队列
    return true;
  }
  return false;
}

void CSinaRTWebData::ReportDataError(void)
{
  TRACE("数据有误,抛掉不用\n");
  CString str;
  str = _T("数据有误");
  gl_systemMessage.PushInformationMessage(str);
}

void CSinaRTWebData::ReportCommunicationError(void)
{
  TRACE("Error reading http file ：hq.sinajs.cn\n");
  CString str;
  str = _T("Error reading http file ：hq.sinajs.cn");
  gl_systemMessage.PushInformationMessage(str);
}

void CSinaRTWebData::InquireNextWebData(void)
{
  static int iCountUp = 0;
  CRTDataPtr pRTData = nullptr;
  long i = 0;

  CString strTemp = _T("");

  // 申请下一批次股票实时数据
  if (gl_ChinaStockMarket.IsCheckTodayActiveStock() || !gl_ChinaStockMarket.SystemReady()) { // 如果处于寻找今日活跃股票期间（9:10--9:29, 11:31--12:59),则使用全局股票池
    gl_stSinaRTDataInquire.strInquire = m_strSinaRTDataInquire; // 设置查询新浪实时数据的字符串头
    if (gl_ChinaStockMarket.CreateSinaRTDataInquiringStr(strTemp)) {
      if (gl_ChinaStockMarket.CountLoopRTDataInquiring()) {  // 遍历三遍全体股票池
        if (!gl_ChinaStockMarket.SystemReady()) { // 如果系统尚未设置好，则显示系统准备
          gl_systemMessage.PushInformationMessage(_T("完成系统初始化"));
        }
        gl_ChinaStockMarket.SetSystemReady(true); // 所有的股票实时数据都轮询三遍，当日活跃股票集已经建立，故而可以接受日线数据了。
        gl_ChinaStockMarket.ResetIT();
      }
    }
    gl_stSinaRTDataInquire.strInquire += strTemp;
  }
  else { // 开市时使用今日活跃股票池
    gl_stSinaRTDataInquire.strInquire = m_strSinaRTDataInquire;
    GetInquiringStockStr(gl_stSinaRTDataInquire.strInquire);
  }
  SetWebDataReceived(false);
  SetReadingWebData(true);  // 在此先设置一次，以防重入（线程延迟导致）
  StartReadingThread();
}