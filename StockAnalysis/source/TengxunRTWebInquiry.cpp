#include"stdafx.h"
#include"globedef.h"
#include"ChinaMarket.h"

#include"Thread.h"
#include "TengxunRTWebInquiry.h"

CTengxunRTWebInquiry::CTengxunRTWebInquiry() : CVirtualWebInquiry() {
  m_strWebDataInquirePrefix = _T("http://qt.gtimg.cn/q=");
  m_strWebDataInquireSuffix = _T("");
  m_strConnection = _T("TengxunRT");
}

CTengxunRTWebInquiry::~CTengxunRTWebInquiry() {
}

bool CTengxunRTWebInquiry::PrepareNextInquiringStr(void) {
  CString strMiddle = _T("");
  ASSERT(gl_ChinaStockMarket.SystemReady());
  // 申请下一批次股票实时数据。
  // 申请腾讯实时数据时，如果遇到不存在的股票代码，服务器会返回v_pv_none_match="1";，导致系统故障，
  // 故而现在只使用有效股票代码。
  strMiddle = GetNextInquiringMiddleStr(900, false); // 目前暂时还是使用全部股票池
  CreateTotalInquiringString(strMiddle);
  return true;
}

CString CTengxunRTWebInquiry::GetNextInquiringMiddleStr(long lTotalNumber, bool fSkipUnactiveStock) {
  return gl_ChinaStockMarket.GetTengxunInquiringStockStr(lTotalNumber, fSkipUnactiveStock);
}

void CTengxunRTWebInquiry::StartReadingThread(void) {
  SetReadingWebData(true);  // 在此先设置一次，以防重入（线程延迟导致）
  AfxBeginThread(ThreadReadTengxunRTData, this);
}

bool CTengxunRTWebInquiry::ReportStatus(long lNumberOfData) {
  TRACE("读入%d个腾讯实时数据\n", lNumberOfData);
  return true;
}