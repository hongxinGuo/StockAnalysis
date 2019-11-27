//////////////////////////////////////////////////////////////////////////////////////////////////

//#include"stdafx.h"

#include"globedef.h"
#include"Thread.h"

#include"accessory.h"
#include"TransferSharedPtr.h"

#include"stock.h"
#include"Market.h"

#include"SetDayLineInfo.h"
#include"SetDayLineToday.h"
#include"SetOption.h"
#include"SetCrweberIndex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMarket::CMarket(void) : CObject() {
  static int siInstance = 0;
  if (++siInstance > 1) {
    TRACE("市场变量只允许存在一个实例\n");
    ASSERT(0);
  }

  m_fPermitResetSystem = true; // 允许系统被重置标识，唯独此标识不允许系统重置。初始时设置为真：允许重置系统。

  Reset();
}

CMarket::~CMarket() {
}

void CMarket::Reset(void) {
  m_lTotalActiveStock = 0; // 初始时股票池中的股票数量为零

  m_fLoadedSelectedStock = false;
  m_fSystemReady = false;    // 市场初始状态为未设置好。
  m_fCurrentStockChanged = false;
  m_fCurrentEditStockChanged = false;

  m_pCurrentStock = nullptr;

  m_lTotalMarketBuy = m_lTotalMarketSell = 0;

  m_ttNewestTransactionTime = 0;

  time_t ttime;
  time(&ttime);
  tm tm_;
  localtime_s(&tm_, &ttime);
  if (tm_.tm_hour >= 15) { // 中国股票市场已经闭市
    m_fTodayStockCompiled = true; // 闭市后才执行本系统，则认为已经处理过今日股票数据了。
  }
  else m_fTodayStockCompiled = false;

  m_lRelativeStrongEndDay = m_lRelativeStrongStartDay = m_lLastLoginDay = 19900101;

  m_fSaveDayLine = false;

  m_fTodayTempDataLoaded = false;

  m_fCheckTodayActiveStock = true;  //检查当日活跃股票，必须为真。

  m_fUpdatedStockCodeDataBase = false;

  m_fCalculatingRS = false;

  m_fGetRTStockData = true;
  m_fReadingTengxunRTData = true; // 默认状态下不读取腾讯实时行情
  m_iCountDownDayLine = 3;    // 400ms延时（100ms每次）
  m_iCountDownSlowReadingRTData = 3; // 400毫秒每次

  m_fUsingSinaRTDataReceiver = true; // 使用新浪实时数据提取器
  m_fUsingNeteaseRTDataReceiver = false; // 使用网易实时数据提取器
  m_fUsingNeteaseRTDataReceiverAsTester = false;
  m_fUsingTengxunRTDataReceiverAsTester = true;

  m_iDayLineNeedProcess = 0;
  m_iDayLineNeedSave = 0;
  m_iDayLineNeedUpdate = 0;

  // 生成股票代码池
  CreateTotalStockContainer();

  // 重置此全局变量
  gl_CrweberIndex.Reset();
  ResetIT(); // 重置实时股票代码查询迭代器
}

#ifdef _DEBUG
void CMarket::AssertValid() const {
  CObject::AssertValid();
}

void CMarket::Dump(CDumpContext& dc) const {
  CObject::Dump(dc);
}
#endif //_DEBUG

void CMarket::TaskGetNeteaseDayLineFromWeb(void) {
  ASSERT(SystemReady());
  if (m_iDayLineNeedUpdate > 0) {
    GetNeteaseDayLineWebData();
  }
}

void CMarket::TaskProcessDayLineGetFromNeeteaseServer() {
  if (m_iDayLineNeedProcess > 0) {
    ProcessDayLineGetFromNeeteaseServer();
  }
}

void CMarket::TaskLoadSavedTempData(void) {
  ASSERT(SystemReady());
  // 装入之前存储的系统今日数据（如果有的话）
  if (!m_fTodayTempDataLoaded) { // 此工作仅进行一次。
    LoadTodayTempDB();
    m_fTodayTempDataLoaded = true;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// 初始化所有可能的股票代码池，只被CMarket的初始函数调用一次。
// 这个函数需要其他全局变量初始化的支持，故而gl_ChinaStockMarket实例需要放在所有全局变量的最后。
//
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CMarket::CreateTotalStockContainer(void) {
  char buffer[10]{};

  CStockPtr pStock = nullptr;
  int iCount = 0;

  // 清空之前的数据（如果有的话。在Reset时，这两个容器中就存有数据）。
  m_vChinaMarketAStock.clear();
  m_mapChinaMarketAStock.clear();

  // 生成上海股票代码
  for (int i = 600000; i < 602000; i++) {
    CString str = _T("sh");
    _itoa_s(i, buffer, 10);
    pStock = make_shared<CStock>();
    str += buffer;
    pStock->SetStockCode(str);
    pStock->SetMarket(__SHANGHAI_MARKET__); // 上海主板
    pStock->SetOffset(iCount);
    m_vChinaMarketAStock.push_back(pStock);
    m_mapChinaMarketAStock[pStock->GetStockCode()] = iCount++; // 使用下标生成新的映射
    ASSERT(pStock->IsDayLineNeedUpdate());
    m_iDayLineNeedUpdate++;
  }

  // 生成三版股票代码
  for (int i = 603000; i < 604000; i++) {
    CString str = _T("sh");
    _itoa_s(i, buffer, 10);
    pStock = make_shared<CStock>();
    str += buffer;
    pStock->SetStockCode(str);
    pStock->SetMarket(__SHANGHAI_MARKET__); // 上海三板
    pStock->SetOffset(iCount);
    m_vChinaMarketAStock.push_back(pStock);
    m_mapChinaMarketAStock[pStock->GetStockCode()] = iCount++; // 使用下标生成新的映射
    m_iDayLineNeedUpdate++;
  }

  // 生成科创版股票代码
  for (int i = 688000; i < 689000; i++) {
    CString str = _T("sh");
    _itoa_s(i, buffer, 10);
    pStock = make_shared<CStock>();
    str += buffer;
    pStock->SetStockCode(str);
    pStock->SetMarket(__SHANGHAI_MARKET__); // 上海科创板
    pStock->SetOffset(iCount);
    m_vChinaMarketAStock.push_back(pStock);
    m_mapChinaMarketAStock[pStock->GetStockCode()] = iCount++; // 使用下标生成新的映射
    m_iDayLineNeedUpdate++;
  }

  // 生成B股股票代码
  for (int i = 900000; i < 901000; i++) {
    CString str = _T("sh");
    _itoa_s(i, buffer, 10);
    pStock = make_shared<CStock>();
    str += buffer;
    pStock->SetStockCode(str);
    pStock->SetMarket(__SHANGHAI_MARKET__); // 上海B股
    pStock->SetOffset(iCount);
    m_vChinaMarketAStock.push_back(pStock);
    m_mapChinaMarketAStock[pStock->GetStockCode()] = iCount++; // 使用下标生成新的映射
    m_iDayLineNeedUpdate++;
  }

  // 生成上海指数代码
  for (int i = 0; i < 1000; i++) {
    CString str = _T("sh");
    sprintf_s(buffer, 10, "%06d", i);
    pStock = make_shared<CStock>();
    str += buffer;
    pStock->SetStockCode(str);
    pStock->SetMarket(__SHANGHAI_MARKET__); // 上海指数
    pStock->SetOffset(iCount);
    m_vChinaMarketAStock.push_back(pStock);
    m_mapChinaMarketAStock[pStock->GetStockCode()] = iCount++; // 使用下标生成新的映射
    m_iDayLineNeedUpdate++;
  }

  ///////////////////////////////////////////////
  // 生成深圳主板股票代码
  for (int i = 0; i < 2000; i++) {
    CString str = _T("sz");
    sprintf_s(buffer, 10, "%06d", i);
    pStock = make_shared<CStock>();
    pStock->SetOffset(iCount);
    str += buffer;
    pStock->SetStockCode(str);
    pStock->SetMarket(__SHENZHEN_MARKET__); // 深圳主板
    m_vChinaMarketAStock.push_back(pStock);
    m_mapChinaMarketAStock[pStock->GetStockCode()] = iCount++;// 使用下标生成新的映射
    m_iDayLineNeedUpdate++;
  }

  // 生成深圳中小板股票代码
  for (int i = 2000; i < 3000; i++) {
    CString str = _T("sz");
    sprintf_s(buffer, 10, "%06d", i);
    pStock = make_shared<CStock>();
    pStock->SetOffset(iCount);
    str += buffer;
    pStock->SetStockCode(str);
    pStock->SetMarket(__SHENZHEN_MARKET__); // 深圳中小板
    m_vChinaMarketAStock.push_back(pStock);
    m_mapChinaMarketAStock[pStock->GetStockCode()] = iCount++;// 使用下标生成新的映射
    m_iDayLineNeedUpdate++;
  }

  // 生成B股股票代码
  for (int i = 200000; i < 201000; i++) {
    CString str = _T("sz");
    sprintf_s(buffer, 10, "%06d", i);
    pStock = make_shared<CStock>();
    pStock->SetOffset(iCount);
    str += buffer;
    pStock->SetStockCode(str);
    pStock->SetMarket(__SHENZHEN_MARKET__); // 深圳B股
    m_vChinaMarketAStock.push_back(pStock);
    m_mapChinaMarketAStock[pStock->GetStockCode()] = iCount++;// 使用下标生成新的映射
    m_iDayLineNeedUpdate++;
  }

  // 生成创业板股票代码
  for (int i = 300000; i < 301000; i++) {
    CString str = _T("sz");
    sprintf_s(buffer, 10, "%06d", i);
    pStock = make_shared<CStock>();
    pStock->SetOffset(iCount);
    str += buffer;
    pStock->SetStockCode(str);
    pStock->SetMarket(__SHENZHEN_MARKET__); // 深圳创业板
    m_vChinaMarketAStock.push_back(pStock);
    m_mapChinaMarketAStock[pStock->GetStockCode()] = iCount++;// 使用下标生成新的映射
    m_iDayLineNeedUpdate++;
  }

  // 生成深圳指数
  for (int i = 399000; i < 400000; i++) {
    CString str = _T("sz");
    sprintf_s(buffer, 10, "%06d", i);
    pStock = make_shared<CStock>();
    pStock->SetOffset(iCount);
    str += buffer;
    pStock->SetStockCode(str);
    pStock->SetMarket(__SHENZHEN_MARKET__); // 深圳指数
    m_vChinaMarketAStock.push_back(pStock);
    m_mapChinaMarketAStock[pStock->GetStockCode()] = iCount++;// 使用下标生成新的映射
    m_iDayLineNeedUpdate++;
  }
  ASSERT(m_iDayLineNeedUpdate == 12000); // 目前总查询股票数为12000个。
  return true;
}

void CMarket::ResetIT(void) {
  m_itSinaStock = m_vChinaMarketAStock.begin();
  m_itTengxunStock = m_vChinaMarketAStock.begin();
  m_itNeteaseStock = m_vChinaMarketAStock.begin();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 生成网易日线股票代码的字符串，用于查询此股票在当前市场是否处于活跃状态（或者是否存在此股票号码）
//
//  此函数是检查m_vChinaMarketAStock股票池
//
//
//
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CMarket::CreateNeteaseDayLineInquiringStr(CString& str) {
  static int iStarted = 0;
  static int siCounter = 0;

  long lTotalStock;

  lTotalStock = m_vChinaMarketAStock.size();

  bool fFound = false;
  int iCount = 0;
  CString strTemp;
  while (!fFound && (iCount < 1000)) {
    if (!m_vChinaMarketAStock.at(siCounter)->IsDayLineNeedUpdate()) { // 日线数据不需要更新。在系统初始时，设置此m_fDayLineNeedUpdate标识
      // TRACE("%S 日线数据无需更新\n", static_cast<LPCWSTR>(m_vChinaMarketAStock.at(siCounter)->m_strStockCode));
      siCounter++;
      iCount++;
      if (siCounter == lTotalStock) {
        siCounter = 0;
      }
    }
    else if (m_vChinaMarketAStock.at(siCounter)->GetIPOStatus() == __STOCK_NULL__) {	// 尚未使用过的股票代码无需查询日线数据
      m_vChinaMarketAStock.at(siCounter)->SetDayLineNeedUpdate(false); // 此股票日线资料不需要更新了。
      // TRACE("无效股票代码：%S, 无需查询日线数据\n", static_cast<LPCWSTR>(m_vChinaMarketAStock.at(siCounter)->m_strStockCode));
      iCount++;
      siCounter++;
      if (siCounter == lTotalStock) {
        siCounter = 0;
      }
    }
    else if (m_vChinaMarketAStock.at(siCounter)->GetDayLineEndDay() >= gl_systemTime.GetLastTradeDay()) { // 上一交易日的日线数据已经存储？此时已经处理过一次日线数据了，无需再次处理。
      m_vChinaMarketAStock.at(siCounter)->SetDayLineNeedUpdate(false); // 此股票日线资料不需要更新了。
      // TRACE("%S 日线数据本日已更新\n", static_cast<LPCWSTR>(m_vChinaMarketAStock.at(siCounter)->m_strStockCode));
      siCounter++;
      iCount++;
      if (siCounter == lTotalStock) {
        siCounter = 0;
      }
    }
    else if (m_vChinaMarketAStock.at(siCounter)->IsDayLineNeedProcess()) { // 日线数据已下载但尚未处理（一般此情况不会出现）
      siCounter++;
      iCount++;
      if (siCounter == lTotalStock) {
        siCounter = 0;
      }
    }
    else {
      fFound = true;
    }
  }

  if (iCount >= 1000) { //  没有找到需要申请日线的股票
    TRACE("未找到需更新日线历史数据的股票\n");
    return false;
  }

  // 找到了需申请日线历史数据的股票（siCounter为索引）
  CStockPtr pStock = m_vChinaMarketAStock.at(siCounter);
  ASSERT(!pStock->IsDayLineNeedSaving());
  ASSERT(!pStock->IsDayLineNeedProcess());
  pStock->SetDayLineNeedUpdate(false);
  switch (pStock->GetMarket()) { // 转换成网易日线数据申请制式（上海为‘0’，深圳为‘1’）
  case __SHANGHAI_MARKET__: // 上海市场？
  case __SHANGHAI_MAIN__: // 上海主板？
  case __SHANGHAI_INDEX__: // 上海指数
  case __SHANGHAI_3BAN__: // 上海3板
  case __SHANGHAI_KECHUANG__: // 上海科创板
  case __SHANGHAI_B_SHARE__: // 上海B股
  str += '0'; // 上海市场标识
  break;
  case __SHENZHEN_MARKET__: // 深圳市场？
  case __SHENZHEN_INDEX__: // 深圳指数
  case __SHENZHEN_3BAN__: // 山镇中小板
  case __SHENZHEN_B_SHARE__: // 深圳B股
  case __SHENZHEN_CHUANGYE__: // 深圳创业板
  case __SHENZHEN_MAIN__: // 深圳主板
  str += '1'; // 深圳市场标识
  break;
  default: // 越界
  ASSERT(0);
  }
  char buffer[30];
  str += pStock->GetStockCode().Right(6); // 取股票代码的右边六位数字。
  siCounter++;
  if (siCounter == lTotalStock) {
    siCounter = 0;
  }
  return true;
}

/////////////////////////////////////////////////////////////////////////
//
//	得到分时线偏移量。09:30为0，15:00为240,步长为1分钟
//
//
////////////////////////////////////////////////////////////////////////
long CMarket::GetMinLineOffset(CStock sID, time_t Time) {
  ASSERT(Time >= 0);
  tm tmTemp{};
  time_t t = 0;
  long lIndex = 0;

  localtime_s(&tmTemp, &Time);
  tmTemp.tm_hour = (9 - 8);			// time_t, tm使用的是国际标准时(UTC),故北京时间09：30即UTC的01：30。要减去8小时
  tmTemp.tm_min = 30;
  tmTemp.tm_sec = 0;
  t = mktime(&tmTemp);
  lIndex = (Time - t) / 60;
  if (lIndex < 0) lIndex = 0;
  if ((lIndex >= 120) && (lIndex < 209)) lIndex = 119;
  if (lIndex >= 210) lIndex -= 90;
  if (lIndex >= 240) lIndex = 239;

  ASSERT((lIndex >= 0) && (lIndex < 240));
  return(lIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////
//
//		判断pStock是否为沪深A股。
//		沪市A股代码以6开头，深市A股代码以00开头。
//
//
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool CMarket::IsAStock(CStockPtr pStock) {
  ASSERT(pStock != nullptr);

  return(IsAStock(pStock->GetStockCode()));
}

////////////////////////////////////////////////////////////////////////////////////////////
//
//		判断strStockCode是否为沪深A股主板的股票代码。
//		沪市A股代码以600或601开头，深市A股代码以000或001开头。
//
//
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool CMarket::IsAStock(CString strStockCode) {
  if ((strStockCode[0] == 's') && (strStockCode[1] == 'h') && (strStockCode[2] == '6') && (strStockCode[3] == '0')) {
    if ((strStockCode[4] == '0') || (strStockCode[4] == '1')) {
      return true;
    }
  }
  else {
    if ((strStockCode[0] == 's') && (strStockCode[1] == 'z') && (strStockCode[2] == '0') && (strStockCode[3] == '0')) {
      if ((strStockCode[4] == '0') || (strStockCode[4] == '2')) {
        return true;
      }
    }
  }
  return(false);
}

////////////////////////////////////////////////////////////////////////////////////////////
//
//		判断代码是否为沪深股票。
//
//
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool CMarket::IsStock(CString strStockCode, CStockPtr& pStock) {
  if ((pStock = GetStockPtr(strStockCode)) != nullptr) {
    return(true);
  }
  else {
    pStock = NULL;
    return(false);
  }
}

//////////////////////////////////////////////////////////////////////////
//
//
//
//
/////////////////////////////////////////////////////////////////////////
INT64 CMarket::GetTotalAttackBuyAmount(void) {
  INT64 lAmount = 0;
  for (auto pStock : m_vChinaMarketAStock) {
    if (pStock->IsActive() && IsAStock(pStock)) {
      lAmount += pStock->GetAttackBuyAmount();
    }
  }
  return(lAmount);
}

//////////////////////////////////////////////////////////////////////////////
//
//
//
//
/////////////////////////////////////////////////////////////////////////////
INT64 CMarket::GetTotalAttackSellAmount(void) {
  INT64 lAmount = 0;
  for (auto pStock : m_vChinaMarketAStock) {
    if (pStock->IsActive() && IsAStock(pStock)) {
      lAmount += pStock->GetAttackSellAmount();
    }
  }
  return(lAmount);
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// 处理实时数据等，由SchedulingTaskPerSecond函数调用,每三秒执行一次。
// 将实时数据暂存队列中的数据分别存放到各自股票的实时队列中。
// 分发数据时，只分发新的（交易时间晚于之前数据的）实时数据。
//
// 此函数用到大量的全局变量，还是放在主线程为好。工作线程目前还是只做计算个股的挂单情况。
//
///////////////////////////////////////////////////////////////////////////////////////////
bool CMarket::TaskDistributeSinaRTDataToProperStock(void) {
  CSingleLock sl(&gl_ProcessSinaRTDataQueue);
  sl.Lock();
  if (sl.IsLocked()) {
    CStockPtr pStock;
    const long lTotalNumber = gl_QueueSinaRTData.GetRTDataSize();
    CString strVolume;

    for (int iCount = 0; iCount < lTotalNumber; iCount++) {
      CRTDataPtr pRTData = gl_QueueSinaRTData.PopRTData();
      if (pRTData->GetDataSource() == __INVALID_RT_WEB_DATA__) {
        gl_systemMessage.PushInnerSystemInformationMessage(_T("实时数据源设置有误"));
      }
      if (pRTData->IsActive()) { // 此实时数据有效？
        if (m_ttNewestTransactionTime < pRTData->GetTransactionTime()) {
          m_ttNewestTransactionTime = pRTData->GetTransactionTime();
        }
        long lIndex = m_mapChinaMarketAStock.at(pRTData->GetStockCode());
        pStock = m_vChinaMarketAStock.at(lIndex);
        if (!pStock->IsActive()) {
          if (pRTData->IsDataHavingValidTime()) {
            pStock->SetActive(true);
            pStock->SetStockName(pRTData->GetStockName());
            pStock->SetStockCode(pRTData->GetStockCode());
            pStock->UpdateStatus(pRTData);
            pStock->SetTransactionTime(pRTData->GetTransactionTime());
            pStock->SetIPOStatus(__STOCK_IPOED__);
            m_lTotalActiveStock++;
          }
        }
        if (pRTData->GetTransactionTime() > pStock->GetTransactionTime()) { // 新的数据？
          pStock->PushRTData(pRTData); // 存储新的数据至数据池
          pStock->SetTransactionTime(pRTData->GetTransactionTime());   // 设置最新接受到实时数据的时间
        }
      }
    }
    gl_ThreadStatus.SetRTDataNeedCalculate(true); // 设置接收到实时数据标识
  }
  ASSERT(gl_QueueSinaRTData.GetRTDataSize() == 0); // 必须一次处理全体数据。
  sl.Unlock();

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// 生成每次查询新浪实时股票数据的字符串
//
//////////////////////////////////////////////////////////////////////////////////////////
int CMarket::GetSinaInquiringStockStr(CString& str, long lTotalNumber, bool fSkipUnactiveStock) {
  //ASSERT(SystemReady());
  return GetInquiringStr(str, m_itSinaStock, _T(","), lTotalNumber, fSkipUnactiveStock);
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// 生成每次查询腾讯实时股票数据的字符串
//
//////////////////////////////////////////////////////////////////////////////////////////
int CMarket::GetTengxunInquiringStockStr(CString& str, long lTotalNumber, bool fSkipUnactiveStock) {
  ASSERT(SystemReady());
  return GetInquiringStr(str, m_itTengxunStock, _T(","), lTotalNumber, fSkipUnactiveStock);
}

int CMarket::GetNeteaseInquiringStockStr(CString& str, long lTotalNumber, bool fSkipUnactiveStock) {
  CString strStockCode, strRight6, strLeft2, strPrefix;
  if (fSkipUnactiveStock) StepToNextActiveStockIT(m_itNeteaseStock);
  if (m_itNeteaseStock == m_vChinaMarketAStock.end()) {
    m_itNeteaseStock = m_vChinaMarketAStock.begin();
  }
  strStockCode = (*m_itNeteaseStock++)->GetStockCode();
  strRight6 = strStockCode.Right(6);
  strLeft2 = strStockCode.Left(2);
  if (strLeft2.Compare(_T("sh")) == 0) {
    strPrefix = _T("0");
  }
  else strPrefix = _T("1");
  str += strPrefix + strRight6;  // 得到第一个股票代码
  int iCount = 1; // 从1开始计数，因为第一个数据前不需要添加postfix。
  while ((m_itNeteaseStock != m_vChinaMarketAStock.end()) && (iCount < lTotalNumber)) { // 每次最大查询量为lTotalNumber个股票
    if (fSkipUnactiveStock) StepToNextActiveStockIT(m_itNeteaseStock);
    if (m_itNeteaseStock != m_vChinaMarketAStock.end()) {
      iCount++;
      str += _T(",");
      strStockCode = (*m_itNeteaseStock++)->GetStockCode();
      strRight6 = strStockCode.Right(6);
      strLeft2 = strStockCode.Left(2);
      if (strLeft2.Compare(_T("sh")) == 0) {
        strPrefix = _T("0");
      }
      else strPrefix = _T("1");
      str += strPrefix + strRight6;  // 得到第一个股票代码
    }
  }
  if (m_itNeteaseStock == m_vChinaMarketAStock.end()) {
    m_itNeteaseStock = m_vChinaMarketAStock.begin();
  }
  else {
    m_itNeteaseStock--;    // 退一格，这样能够覆盖住边缘
  }
  return iCount;
}

int CMarket::GetInquiringStr(CString& str, vector<CStockPtr>::iterator& itStock, CString strPostfix, long lTotalNumber, bool fSkipUnactiveStock) {
  if (fSkipUnactiveStock) StepToNextActiveStockIT(itStock);
  if (itStock == m_vChinaMarketAStock.end()) {
    itStock = m_vChinaMarketAStock.begin();
  }
  str += (*itStock++)->GetStockCode();  // 得到第一个股票代码
  int iCount = 1; // 从1开始计数，因为第一个数据前不需要添加postfix。
  while ((itStock != m_vChinaMarketAStock.end()) && (iCount < lTotalNumber)) { // 每次最大查询量为lTotalNumber个股票
    if (fSkipUnactiveStock) StepToNextActiveStockIT(itStock);
    if (itStock != m_vChinaMarketAStock.end()) {
      iCount++;
      str += strPostfix;
      str += (*itStock++)->GetStockCode();
    }
  }
  if (itStock == m_vChinaMarketAStock.end()) {
    itStock = m_vChinaMarketAStock.begin();
  }
  else {
    itStock--;    // 退一格，这样能够覆盖住边缘
  }
  return iCount;
}

bool CMarket::StepToNextActiveStockIT(vector<CStockPtr>::iterator& itStock) {
  while (!(*itStock)->IsActive()) {
    itStock++;
    if (itStock == m_vChinaMarketAStock.end()) {
      return true;
    }
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////////////
//
// 由工作线程ThreadCalculatingRTDataProc调用，注意全局变量的使用
//
//
/////////////////////////////////////////////////////////////////////////////////////////
bool CMarket::ProcessRTData(void) {
  for (auto pStock : m_vChinaMarketAStock) {
    if (pStock->IsActive()) {
      pStock->ProcessRTData();
    }
  }
  return true;
}

bool CMarket::ProcessTengxunRTData(void) {
  return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
//
//
// 大约每100毫秒调度一次
//
//
///////////////////////////////////////////////////////////////////////////////////////////
bool CMarket::SchedulingTask(void) {
  static time_t s_timeLast = 0;

  gl_systemTime.CalculateTime();      // 计算系统各种时间

  // 抓取实时数据(新浪、腾讯和网易）。每400毫秒申请一次，即可保证在3秒中内遍历一遍全体活跃股票。
  if (!gl_ExitingSystem && m_fGetRTStockData && (m_iCountDownSlowReadingRTData <= 0)) {
    TaskGetRTDataFromWeb();

    // 如果要求慢速读取实时数据，则设置读取速率为每分钟一次
    if (!m_fMarketOpened && SystemReady()) m_iCountDownSlowReadingRTData = 10; // 完全轮询一遍后，非交易时段一分钟左右更新一次即可
    else m_iCountDownSlowReadingRTData = 3;  // 计数4次,即每400毫秒申请一次实时数据
  }
  m_iCountDownSlowReadingRTData--;

  //根据时间，调度各项定时任务.每秒调度一次
  if (gl_systemTime.Gett_time() > s_timeLast) {
    SchedulingTaskPerSecond(gl_systemTime.Gett_time() - s_timeLast);
    s_timeLast = gl_systemTime.Gett_time();
  }

  // 系统准备好了之后需要完成的各项工作
  if (SystemReady()) {
    TaskLoadSavedTempData();
    TaskGetNeteaseDayLineFromWeb();
  }
  return true;
}

/////////////////////////////////////////////////////////////////////////////////
//
// 从新浪、网易或者腾讯实时行情数据服务器读取实时数据。使用其中之一即可。
//
/////////////////////////////////////////////////////////////////////////////////
bool CMarket::TaskGetRTDataFromWeb(void) {
  static int siCountDownTengxunNumber = 3;
  static int siCountDownNeteaseNumber = 300;

  if (m_fUsingSinaRTDataReceiver) {
    gl_SinaRTWebData.GetWebData(); // 每400毫秒(100X4)申请一次实时数据。新浪的实时行情服务器响应时间不超过100毫秒（30-70之间），且没有出现过数据错误。
  }

  if (SystemReady()) {
    // 网易实时数据有大量的缺失字段，且前缀后缀也有时缺失，暂时停止使用。
    // 网易实时数据有时还发送没有要求过的股票，不知为何。
    ASSERT(m_fUsingNeteaseRTDataReceiver == false);
    if (m_fUsingNeteaseRTDataReceiver) {
      if (siCountDownNeteaseNumber <= 0) {
        // 读取网易实时行情数据。估计网易实时行情与新浪的数据源相同，故而两者可互换，使用其一即可。
        gl_NeteaseRTWebData.GetWebData(); // 目前不使用此功能。
        siCountDownNeteaseNumber = 3;
      }
      else siCountDownNeteaseNumber--;
    }
    // 读取腾讯实时行情数据。 由于腾讯实时行情的股数精度为手，没有零股信息，导致无法与新浪实时行情数据对接（新浪精度为股），故而暂时不用
    if (m_fReadingTengxunRTData) {
      if (siCountDownTengxunNumber <= 0) {
        gl_TengxunRTWebData.GetWebData();// 只有当系统准备完毕后，方可执行读取腾讯实时行情数据的工作。目前不使用此功能
        siCountDownTengxunNumber = 3;
      }
      else siCountDownTengxunNumber--; // 新浪实时数据读取三次，腾讯才读取一次。因为腾讯的挂单股数采用的是每手标准，精度不够
    }
  }
  return true;
}

bool CMarket::GetNeteaseDayLineWebData(void) {
  // 抓取日线数据.
  // 最多使用四个引擎，否则容易被网易服务器拒绝服务。一般还是用两个为好。
  switch (gl_cMaxSavingOneDayLineThreads) {
  case 8: case 7: case 6:
  gl_NeteaseDayLineWebDataSix.GetWebData(); // 网易日线历史数据
  case 5:
  gl_NeteaseDayLineWebDataFive.GetWebData();
  case 4:
  gl_NeteaseDayLineWebDataFourth.GetWebData();
  case 3:
  gl_NeteaseDayLineWebDataThird.GetWebData();
  case 2:
  gl_NeteaseDayLineWebDataSecond.GetWebData();
  case 1: case 0:
  gl_NeteaseDayLineWebData.GetWebData();
  break;
  default:
  gl_NeteaseDayLineWebData.GetWebData();
  TRACE(_T("Out of range in Get Newease DayLine Web Data\n"));
  break;
  }
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
// 定时调度函数，每秒一次。
//
// 各种任务之间有可能出现互斥的现象，如存储临时实时数据的工作线程与计算实时数据的工作线程之间就不允许同时运行，
// 故而所有的定时任务，要按照时间间隔从长到短排列，即先执行每分钟一次的任务，再执行每秒钟一次的任务，这样能够保证长间隔的任务优先执行。
//
//
/////////////////////////////////////////////////////////////////////////////////////////////
bool CMarket::SchedulingTaskPerSecond(long lSecondNumber) {
  static int s_iCountDownProcessRTWebData = 0;
  const long lCurrentTime = gl_systemTime.GetTime();

  // 各调度程序按间隔时间大小顺序排列，间隔时间长的必须位于间隔时间短的之前。
  SchedulingTaskPerHour(lSecondNumber, lCurrentTime);
  SchedulingTaskPer5Minutes(lSecondNumber, lCurrentTime);
  SchedulingTaskPer1Minute(lSecondNumber, lCurrentTime);
  SchedulingTaskPer10Seconds(lSecondNumber, lCurrentTime);

  if (s_iCountDownProcessRTWebData <= 0) {
    // 将接收到的实时数据分发至各相关股票的实时数据队列中。
    // 由于有多个数据源，故而需要等待各数据源都执行一次后，方可以分发至相关股票处，故而需要每三秒执行一次，以保证各数据源至少都能提供一次数据。
    TaskDistributeSinaRTDataToProperStock();
    s_iCountDownProcessRTWebData = 0;
  }
  else s_iCountDownProcessRTWebData--;

  // 计算实时数据，每秒钟一次。目前个股实时数据为每3秒钟一次更新，故而无需再快了。
  // 此计算任务要在DistributeRTDataReceivedFromWebToProperStock之后执行，以防止出现同步问题。
  if (SystemReady() && !gl_ThreadStatus.IsSavingTempData() && IsTodayTempRTDataLoaded()) { // 在系统存储临时数据时不能同时计算实时数据，否则容易出现同步问题。
    if (gl_ThreadStatus.IsRTDataNeedCalculate()) {
      gl_ThreadStatus.SetCalculatingRTData(true);
      AfxBeginThread(ThreadCalculateRTData, nullptr);
    }
  }

  // 将处理日线历史数据的函数改为定时查询，读取和存储采用工作进程。
  TaskProcessDayLineGetFromNeeteaseServer();

  return true;
}

bool CMarket::SchedulingTaskPerHour(long lSecondNumber, long lCurrentTime) {
  static int i1HourCounter = 3599; // 一小时一次的计数器

                                   // 计算每一小时一次的任务
  if (i1HourCounter <= 0) {
    i1HourCounter = 3599;
  }
  else i1HourCounter -= lSecondNumber;

  return true;
}

bool CMarket::SchedulingTaskPer5Minutes(long lSecondNumber, long lCurrentTime) {
  static int i5MinuteCounter = 299; // 五分钟一次的计数器

  // 计算每五分钟一次的任务。
  if (i5MinuteCounter <= 0) {
    i5MinuteCounter = 299;

    // 自动查询crweber.com
    gl_CrweberIndexWebData.GetWebData();

    ResetSystemFlagAtMidnight(lCurrentTime);
    SaveTempDataIntoDB(lCurrentTime);
  } // 每五分钟一次的任务
  else i5MinuteCounter -= lSecondNumber;

  return true;
}

void CMarket::ResetSystemFlagAtMidnight(long lCurrentTime) {
  // 午夜过后重置各种标识
  if (lCurrentTime <= 1500 && !m_fPermitResetSystem) {  // 在零点到零点十五分，重置系统标识
    m_fPermitResetSystem = true;
    CString str;
    str = _T(" 重置系统重置标识");
    gl_systemMessage.PushInformationMessage(str);
  }
}

void CMarket::SaveTempDataIntoDB(long lCurrentTime) {
  // 开市时每五分钟存储一次当前状态。这是一个备用措施，防止退出系统后就丢掉了所有的数据，不必太频繁。
  if (m_fSystemReady) {
    if (m_fMarketOpened && !gl_ThreadStatus.IsCalculatingRTData()) {
      if (((lCurrentTime > 93000) && (lCurrentTime < 113600)) || ((lCurrentTime > 130000) && (lCurrentTime < 150600))) { // 存储临时数据严格按照交易时间来确定(中间休市期间和闭市后各要存储一次，故而到11:36和15:06才中止）
        CString str;
        str = _T(" 存储临时数据");
        gl_systemMessage.PushInformationMessage(str);
        UpdateTempRTData();
      }
    }
  }
}

bool CMarket::SchedulingTaskPer1Minute(long lSecondNumber, long lCurrentTime) {
  static int i1MinuteCounter = 59;  // 一分钟一次的计数器

  // 计算每分钟一次的任务。所有的定时任务，要按照时间间隔从长到短排列，即现执行每分钟一次的任务，再执行每秒钟一次的任务，这样能够保证长间隔的任务优先执行。
  if (i1MinuteCounter <= 0) {
    i1MinuteCounter = 59; // 重置计数器

    // 测试用。每小时自动查询crweber.com
    //gl_CrweberIndexWebData.GetWebData();

    TaskResetSystem(lCurrentTime);
    TaskResetSystemAgain(lCurrentTime);

    // 判断中国股票市场开市状态
    if ((lCurrentTime < 91500) || (lCurrentTime > 150630) || ((lCurrentTime > 113500) && (lCurrentTime < 125500))) { //下午三点六分三十秒市场交易结束（为了保证最后一个临时数据的存储）
      m_fMarketOpened = false;
    }
    else if ((gl_systemTime.GetDayOfWeek() == 0) || (gl_systemTime.GetDayOfWeek() == 6)) { //周六或者周日闭市。结构tm用0--6表示星期日至星期六
      m_fMarketOpened = false;
    }
    else m_fMarketOpened = true;

    // 在开市前和中午暂停时查询所有股票池，找到当天活跃股票。
    if (((lCurrentTime >= 91500) && (lCurrentTime < 92900)) || ((lCurrentTime >= 113100) && (lCurrentTime < 125900))) {
      m_fCheckTodayActiveStock = true;
    }
    else m_fCheckTodayActiveStock = false;

    // 下午三点一分开始处理当日实时数据。
    if ((lCurrentTime >= 150100) && !IsTodayStockCompiled()) {
      if (SystemReady()) {
        AfxBeginThread(ThreadCompileCurrentTradeDayStock, nullptr);
        SetTodayStockCompiledFlag(true);
      }
    }

    if (m_fSaveDayLine && (m_iDayLineNeedSave <= 0)) {
      m_fSaveDayLine = false;
      m_fUpdatedStockCodeDataBase = true;
      TRACE("日线历史数据更新完毕\n");
      CString str;
      str = _T("日线历史数据更新完毕");
      gl_systemMessage.PushInformationMessage(str);
      UpdateStockCodeDB();  // 更新股票池数据库
    }
  } // 每一分钟一次的任务
  else i1MinuteCounter -= lSecondNumber;

  return true;
}

bool CMarket::TaskResetSystem(long lCurrentTime) {
  // 九点十三分重启系统
// 必须在此时间段内重启，如果更早的话容易出现数据不全的问题。
  if (m_fPermitResetSystem) { // 如果允许重置系统
    if ((lCurrentTime >= 91300) && (lCurrentTime <= 91400) && gl_systemTime.IsWorkingDay()) { // 交易日九点十五分重启系统
      gl_fResetSystem = true;     // 只是设置重启标识，实际重启工作由CMainFrame的OnTimer函数完成。
      m_fSystemReady = false;
    }
  }
  return true;
}

bool CMarket::TaskResetSystemAgain(long lCurrentTime) {
  // 九点二十五分再次重启系统
  if (m_fPermitResetSystem) { // 如果允许重置系统
    if ((lCurrentTime >= 92500) && (lCurrentTime <= 93000) && gl_systemTime.IsWorkingDay()) { // 交易日九点十五分重启系统
      gl_fResetSystem = true;     // 只是设置重启标识，实际重启工作由CMainFrame的OnTimer函数完成。
      m_fSystemReady = false;
      m_fPermitResetSystem = false; // 今天不再允许重启系统。
    }
  }
  return true;
}

bool CMarket::SchedulingTaskPer10Seconds(long lSecondNumber, long lCurrentTime) {
  static int i10SecondsCounter = 9; // 十秒一次的计数器

  // 计算每十秒钟一次的任务
  if (i10SecondsCounter <= 0) {
    i10SecondsCounter = 9;
    // do something

    // 判断是否存储日线库和股票代码库
    if ((m_iDayLineNeedSave > 0)) {
      m_fSaveDayLine = true;
      gl_ChinaStockMarket.SaveDayLineData();
    }
  } // 每十秒钟一次的任务
  else i10SecondsCounter -= lSecondNumber;

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
// 得到股票的简称
//
//////////////////////////////////////////////////////////////////////////////////////////////
CString CMarket::GetStockName(CString strStockCode) {
  try {
    return (m_vChinaMarketAStock.at(m_mapChinaMarketAStock.at(strStockCode))->GetStockName());
  }
  catch (exception & e) {
    TRACE("GetStockName函数异常\n");
    return _T("");
  }
}

////////////////////////////////////////////////////////////////////////////////
//
//	通过股票代码得到股票的索引。
//	如果没找到的话返回值为假。
//
//
////////////////////////////////////////////////////////////////////////////////
bool CMarket::GetStockIndex(CString strStockCode, long& lIndex) {
  try {
    lIndex = m_mapChinaMarketAStock.at(strStockCode);
    return true;
  }
  catch (exception & e) {
    TRACE("GetStockIndex函数异常\n");
    lIndex = -1;
    return false;
  }
}

////////////////////////////////////////////////////////////////////////////////
//
//	通过股票代码得到股票的指针。
//	如果没找到的话返回空指针
//
//
////////////////////////////////////////////////////////////////////////////////
CStockPtr CMarket::GetStockPtr(CString strStockCode) {
  try {
    return (m_vChinaMarketAStock.at(m_mapChinaMarketAStock.at(strStockCode)));
  }
  catch (exception & e) {
    TRACE("GetStockPtr函数异常\n");
    return nullptr;
  }
}

CStockPtr CMarket::GetStockPtr(long lIndex) {
  try {
    return m_vChinaMarketAStock.at(lIndex);
  }
  catch (exception & e) {
    TRACE("GetStockPtr函数异常\n");
    return nullptr;
  }
}

void CMarket::IncreaseActiveStockNumber(void) {
  m_lTotalActiveStock++;
}

//////////////////////////////////////////////////////////////////////////
//
// 设置当前操作的股票
//
// 设置相应的股票指针，装载其日线数据。
//
/////////////////////////////////////////////////////////////////////////
void CMarket::SetShowStock(CStockPtr pStock) {
  if (m_pCurrentStock != pStock) {
    m_pCurrentStock = pStock;
    m_fCurrentStockChanged = true;
    m_pCurrentStock->SetDayLineLoaded(false);
    AfxBeginThread(ThreadLoadDayLine, 0);
  }
}

bool CMarket::IsCurrentStockChanged(void) {
  if (m_fCurrentStockChanged) {
    m_fCurrentStockChanged = false;
    return true;
  }
  else return false;
}

//////////////////////////////////////////////////////////////////////////////////////
//
//	通过股票代码和市场代码设置当前选择股票
//
//////////////////////////////////////////////////////////////////////////////////////
void CMarket::SetShowStock(CString strStockCode) {
  m_pCurrentStock = GetStockPtr(strStockCode);
  ASSERT(m_pCurrentStock != NULL);
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//	将日线数据存入数据库．
//  此函数由工作线程ThreadDayLineSaveProc调用，尽量不要使用全局变量。
//
//////////////////////////////////////////////////////////////////////////////////////////

bool CMarket::SaveDayLineData(void) {
  CString str;
  strTransferSharedPtr* pTransfer = nullptr;

  for (auto pStock : m_vChinaMarketAStock) {
    if (pStock->IsDayLineNeedSavingAndClearFlag()) { // 清除标识需要与检测标识处于同一原子过程中，防止同步问题出现
      m_iDayLineNeedSave--;
      if (pStock->m_vDayLine.size() > 0) { // 新股第一天上市时，由于只存储早于今天的日线数据，导致其容器是空的，故而需要判断一下
        pTransfer = new strTransferSharedPtr; // 此处生成，由线程负责delete
        pTransfer->m_pStock = pStock;
        AfxBeginThread(ThreadSaveDayLineOfOneStock, (LPVOID)pTransfer, THREAD_PRIORITY_LOWEST);
      }
      else {
        CString str1 = pStock->GetStockCode();
        str1 += _T(" 新股上市,没有日线资料");
        gl_systemMessage.PushDayLineInfoMessage(str1);
      }
      str = pStock->GetStockCode();
      str += _T("日线资料存储完成");
      gl_systemMessage.PushDayLineInfoMessage(str);
    }
    if (gl_ExitingSystem) {
      break; // 如果程序正在退出，则停止存储。
    }
  }

  return(true);
}

bool CMarket::ClearAllDayLineVector(void) {
  for (auto pStock : m_vChinaMarketAStock) {
    pStock->m_vDayLine.clear();
  }

  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//	将实时数据存入数据库．默认数据库为空。
//
//
//////////////////////////////////////////////////////////////////////////////////////////
bool CMarket::SaveRTData(void) {
  CSetRealTimeData setRTData;
  setRTData.m_strFilter = _T("[ID] = 1");

  setRTData.Open();
  setRTData.m_pDatabase->BeginTrans();
  for (auto pStock : m_vChinaMarketAStock) {
    if (pStock->IsActive()) {
      pStock->SaveRealTimeData(setRTData);
    }
  }
  setRTData.m_pDatabase->CommitTrans();
  setRTData.Close();
  return(true);
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//	将crweber.com油运指数数据存入数据库。
//
//
//////////////////////////////////////////////////////////////////////////////////////////
bool CMarket::SaveCrweberIndexData(void) {
  CSetCrweberIndex setIndex;
  setIndex.m_strFilter = _T("[ID] = 1");

  // 存储今日生成的数据于CrweberIndex表中。
  setIndex.Open();
  setIndex.m_pDatabase->BeginTrans();
  gl_CrweberIndex.AppendData(setIndex);
  setIndex.m_pDatabase->CommitTrans();
  setIndex.Close();
  return(true);
}

bool CMarket::IsDayLineNeedSaving(void) {
  for (auto pStock : m_vChinaMarketAStock) {
    if (pStock->IsDayLineNeedSaving()) {
      ASSERT(pStock->IsActive());
      return true;
    }
  }
  return false;
}

bool CMarket::IsDayLineNeedUpdate(void) {
  for (auto pStock : m_vChinaMarketAStock) {
    if (pStock->IsDayLineNeedUpdate()) return true;
  }
  return false;
}

bool CMarket::ProcessDayLineGetFromNeeteaseServer(void) {
  for (auto pStock : m_vChinaMarketAStock) {
    if (pStock->IsDayLineNeedProcess()) {
      pStock->ProcessNeteaseDayLineData();
      pStock->SetDayLineNeedProcess(false);
      m_iDayLineNeedProcess--;
    }
  }
  return true;
}

//////////////////////////////////////////////////////////////////////////////////
//
// 处理当前交易日的实时数据，生成日线各基本数据（相对强度、进攻性买卖盘）。
//
// 只有下载完日线历史数据后，方可执行处理实时数据，否则可能误判股票代码存在与否。
//
// long lCurrentTradeDay 当前交易日。由于存在周六和周日，故而此日期并不一定就是当前日期，而可能时周五
//
//////////////////////////////////////////////////////////////////////////////////
long CMarket::CompileCurrentTradeDayStock(void) {
  char buffer[20];
  CString strDay;
  CSetDayLine setDayLine;
  CSetDayLineInfo setDayLineInfo;
  long iCount = 0;
  long lCurrentTradeDay = gl_systemTime.FormatToDay(m_ttNewestTransactionTime);

  CString str;
  str = _T("开始处理最新交易日的实时数据");
  gl_systemMessage.PushInformationMessage(str);

  // 存储当前交易日的数据
  _ltoa_s(lCurrentTradeDay, buffer, 10);
  strDay = buffer;
  setDayLine.m_strFilter = _T("[Day] =");
  setDayLine.m_strFilter += strDay;
  setDayLine.Open();
  setDayLine.m_pDatabase->BeginTrans();
  while (!setDayLine.IsEOF()) {
    setDayLine.Delete();
    setDayLine.MoveNext();
  }
  setDayLine.m_pDatabase->CommitTrans();

  setDayLine.m_pDatabase->BeginTrans();
  for (auto pStock : m_vChinaMarketAStock) {
    if (!pStock->TodayDataIsActive()) {  // 此股票今天停牌,所有的数据皆为零,不需要存储.
      continue;
    }
    iCount++;
    pStock->SetDayLineEndDay(lCurrentTradeDay);
    pStock->SetIPOStatus(__STOCK_IPOED__); // 再设置一次。防止新股股票代码由于没有历史数据而被误判为不存在。
    setDayLine.AddNew();
    pStock->SaveBasicInfo(setDayLine);
    setDayLine.Update();
  }
  setDayLine.m_pDatabase->CommitTrans();
  setDayLine.Close();

  // 存储今日生成的数据于DayLineInfo表中。
  setDayLineInfo.m_strFilter = _T("[Day] =");
  setDayLineInfo.m_strFilter += strDay;
  setDayLineInfo.Open();
  setDayLineInfo.m_pDatabase->BeginTrans();
  while (!setDayLineInfo.IsEOF()) {
    setDayLineInfo.Delete();
    setDayLineInfo.MoveNext();
  }
  setDayLineInfo.m_pDatabase->CommitTrans();

  setDayLineInfo.m_pDatabase->BeginTrans();
  for (auto pStock : m_vChinaMarketAStock) {
    if (!pStock->TodayDataIsActive()) {  // 此股票今天停牌,所有的数据皆为零,不需要存储.
      continue;
    }
    setDayLineInfo.AddNew();
    pStock->SaveCalculatedInfo(setDayLineInfo);
    setDayLineInfo.Update();
  }
  setDayLineInfo.m_pDatabase->CommitTrans();
  setDayLineInfo.Close();

  gl_systemMessage.PushInformationMessage(_T("最新交易日实时数据处理完毕"));
  return iCount;
}

//////////////////////////////////////////////////////////////////////////////////
//
// 将当日处理好的数据储存于数据库中，以备万一系统崩溃时重新装入。
// 似乎应该以一个定时工作线程的形式存在。
//
// 研究之。
//
//////////////////////////////////////////////////////////////////////////////////
bool CMarket::UpdateTodayTempDB(void) {
  CSetDayLineToday setDayLineToday;

  // 存储今日生成的数据于DayLineToday表中。
  setDayLineToday.Open();
  setDayLineToday.m_pDatabase->BeginTrans();
  while (!setDayLineToday.IsEOF()) {
    setDayLineToday.Delete();
    setDayLineToday.MoveNext();
  }
  setDayLineToday.m_pDatabase->CommitTrans();
  setDayLineToday.m_pDatabase->BeginTrans();
  for (auto pStock : m_vChinaMarketAStock) {
    if (!pStock->TodayDataIsActive()) {  // 此股票今天停牌,所有的数据皆为零,不需要存储.
      continue;
    }
    ASSERT(pStock->GetVolume() == pStock->GetOrdinaryBuyVolume() + pStock->GetOrdinarySellVolume() + pStock->GetAttackBuyVolume()
           + pStock->GetAttackSellVolume() + pStock->GetStrongBuyVolume() + pStock->GetStrongSellVolume() + pStock->GetUnknownVolume());
    setDayLineToday.AddNew();
    pStock->SaveTempInfo(setDayLineToday);
    setDayLineToday.Update();
  }
  setDayLineToday.m_pDatabase->CommitTrans();
  setDayLineToday.Close();

  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
//
// 读入暂存的今日当前数据状态，重置分析的初始态。这样当在开市时系统退出时，不至于损失掉所有已分析的数据
//
// 在设置m_lUnknownVolume为记录集中的m_UnknownVolume - m_Volume，这是因为第一次计算时只是初始化系统。
// 需要设置m_lUnknownVolume = pRTData->m_lVolume - setDayLineToday.m_Volume + setDayLineToday.m_UnknownVolume
// 而第一次执行计算实时数据时，只是初始化系统环境，其中设置m_lUnknownVolume += pRTData->GetVolume
// 故而此处这样计算。
/////////////////////////////////////////////////////////////////////////////////////////////
bool CMarket::LoadTodayTempDB(void) {
  CStockPtr pStock = nullptr;
  CSetDayLineToday setDayLineToday;
  CRTDataPtr pRTData;

  ASSERT(!m_fTodayTempDataLoaded);
  ASSERT(!gl_ThreadStatus.IsCalculatingRTData());    // 执行此初始化工作时，计算实时数据的工作线程必须没有运行。
  // 读取今日生成的数据于DayLineToday表中。
  setDayLineToday.Open();
  if (!setDayLineToday.IsEOF()) {
    if (setDayLineToday.m_Day == gl_systemTime.GetDay()) { // 如果是当天的行情，则载入，否则放弃
      while (!setDayLineToday.IsEOF()) {
        if ((pStock = GetStockPtr(setDayLineToday.m_StockCode)) != nullptr) {
          ASSERT(!pStock->HaveFirstRTData()); // 确保没有开始计算实时数据
          // 需要设置m_lUnknownVolume = pRTData->m_lVolume - setDayLineToday.m_Volume + setDayLineToday.m_UnknownVolume
          // 而第一次执行计算实时数据时，只是初始化系统环境，其中设置m_lUnknownVolume += pRTData->GetVolume
          // 故而LoadAndCalculateTempInfo需要特别处理。
          pStock->LoadAndCalculateTempInfo(setDayLineToday);
        }
        setDayLineToday.MoveNext();
      }
    }
  }
  setDayLineToday.Close();

  return true;
}

///////////////////////////////////////////////////////////////////////////////////
//
// 计算lDay的日线相对强度, lDay的格式为：YYYYMMDD,如 19990605.
// 将日线按涨跌排列后,其相对强弱即其在队列中的位置
//
//////////////////////////////////////////////////////////////////////////////////
bool CMarket::CalculateOneDayRelativeStrong(long lDay) {
  vector<CStockPtr> vStock;
  vector<int> vIndex;
  vector<double> vRelativeStrong;
  int iTotalAShare = 0;
  CString strSQL;
  CString strDay;
  char  pch[30];
  int iStockNumber = 0, j = 0;
  CTime ctTime;
  CSetDayLine setDayLine;
  const long lYear = lDay / 10000;
  const long lMonth = lDay / 100 - lYear * 100;
  const long lDayOfMonth = lDay - lYear * 10000 - lMonth * 100;
  char buffer[100];

  for (j = 0; j < 30; j++) pch[j] = 0x000;

  _ltoa_s(lDay, pch, 10);
  strDay = pch;
  setDayLine.m_strSort = _T("[UpDownRate]");
  setDayLine.m_strFilter = _T("[Day] =");
  setDayLine.m_strFilter += strDay;
  setDayLine.Open();
  if (setDayLine.IsEOF()) { // 数据集为空，表明此日没有交易
    setDayLine.Close();
    return false;
  }
  setDayLine.m_pDatabase->BeginTrans();
  iStockNumber = 0;
  while (!setDayLine.IsEOF()) {
    if (gl_ChinaStockMarket.IsAStock(setDayLine.m_StockCode)) {
      long lIndex = m_mapChinaMarketAStock.at(setDayLine.m_StockCode);
      vStock.push_back(m_vChinaMarketAStock.at(lIndex));
      vIndex.push_back(iStockNumber); // 将A股的索引记录在容器中。
      iTotalAShare++;
    }
    iStockNumber++;
    setDayLine.MoveNext();
  }

  setDayLine.MoveFirst();
  int iCount = 0;
  int iBefore = 0;
  while (iCount < vIndex.size()) { // 只计算活跃股票的相对强度
    for (int i = 0; i < vIndex.at(iCount) - iBefore; i++) { // 根据索引去更改数据库,跨过不是A股的股票
      setDayLine.MoveNext();
    }
    setDayLine.Edit();
    double dLastClose = atof(setDayLine.m_LastClose);
    double dLow = atof(setDayLine.m_Low);
    double dHigh = atof(setDayLine.m_High);
    double dClose = atof(setDayLine.m_Close);
    if (((dLow / dLastClose) < 0.88)
        || ((dHigh / dLastClose) > 1.12)) { // 除权、新股上市等
      setDayLine.m_RelativeStrong = ConvertValueToString(50); // 新股上市或者除权除息，不计算此股
    }
    else if ((fabs(dHigh - dClose) < 0.0001)
             && (((dClose / dLastClose)) > 1.095)) { // 涨停板
      setDayLine.m_RelativeStrong = ConvertValueToString(100);
    }
    else if ((fabs(dClose - dLow) < 0.0001)
             && ((dClose / dLastClose) < 0.905)) { // 跌停板
      setDayLine.m_RelativeStrong = ConvertValueToString(0);
    }
    else {
      setDayLine.m_RelativeStrong = ConvertValueToString((static_cast<double>(iCount) * 100) / iTotalAShare);
    }
    setDayLine.Update();
    iBefore = vIndex.at(iCount++);
    setDayLine.MoveNext(); // 移到下一个数据。
    iBefore++; // 计数器也同时加一。
  }
  setDayLine.m_pDatabase->CommitTrans();
  setDayLine.Close();

  vStock.clear();
  vIndex.clear();
  vRelativeStrong.clear();

  sprintf_s(buffer, "%4d年%2d月%2d日的股票相对强度计算完成", lYear, lMonth, lDayOfMonth);
  CString strTemp;
  strTemp = buffer;
  gl_systemMessage.PushDayLineInfoMessage(strTemp);    // 采用同步机制报告信息

  return(true);
}

bool CMarket::UpdateStockCodeDB(void) {
  CSetStockCode setStockCode;

  setStockCode.Open();
  setStockCode.m_pDatabase->BeginTrans();
  while (!setStockCode.IsEOF()) {
    setStockCode.Delete();
    setStockCode.MoveNext();
  }
  setStockCode.m_pDatabase->CommitTrans();
  setStockCode.m_pDatabase->BeginTrans();
  for (auto pStock : m_vChinaMarketAStock) {
    setStockCode.AddNew();
    CString str;
    setStockCode.m_Counter = pStock->GetOffset();
    setStockCode.m_StockType = pStock->GetMarket();
    setStockCode.m_StockCode = pStock->GetStockCode();
    if (pStock->GetStockName() != _T("")) {   // 如果此股票ID有了新的名字，
      setStockCode.m_StockName = pStock->GetStockName(); // 则存储新的名字
    }
    if (pStock->GetIPOStatus() == __STOCK_IPOED__) { // 如果此股票是活跃股票
      if (pStock->GetDayLineEndDay() < (gl_systemTime.GetDay() - 100)) { // 如果此股票的日线历史数据已经早于一个月了，则设置此股票状态为已退市
        setStockCode.m_IPOed = __STOCK_DELISTED__;
      }
      else {
        setStockCode.m_IPOed = pStock->GetIPOStatus();
      }
    }
    else {
      setStockCode.m_IPOed = pStock->GetIPOStatus();
    }
    setStockCode.m_DayLineStartDay = pStock->GetDayLineStartDay();
    setStockCode.m_DayLineEndDay = pStock->GetDayLineEndDay();
    setStockCode.Update();
  }
  setStockCode.m_pDatabase->CommitTrans();
  setStockCode.Close();

  return true;
}

void CMarket::LoadStockCodeDB(void) {
  CSetStockCode setStockCode;

  setStockCode.Open();
  // 装入股票代码数据库
  while (!setStockCode.IsEOF()) {
    long lIndex = 1;
    lIndex = m_mapChinaMarketAStock.at(setStockCode.m_StockCode);
    CStockPtr pStock = m_vChinaMarketAStock.at(lIndex);
    if (setStockCode.m_StockCode != _T("")) {
      pStock->SetStockCode(setStockCode.m_StockCode);
    }
    if (setStockCode.m_StockName != _T("")) {
      CString str = setStockCode.m_StockName; // 用str中间过渡一下，就可以读取UniCode制式的m_StockName了。
      pStock->SetStockName(str);
    }
    if (setStockCode.m_IPOed != __STOCK_NOT_CHECKED__) { // 如果此股票代码已经被检查过，则设置股票目前状态。否则不设置。
      pStock->SetIPOStatus(setStockCode.m_IPOed);
    }
    pStock->SetDayLineStartDay(setStockCode.m_DayLineStartDay);
    if (pStock->GetDayLineEndDay() < setStockCode.m_DayLineEndDay) { // 有时一个股票会有多个记录，以最后的日期为准。
      pStock->SetDayLineEndDay(setStockCode.m_DayLineEndDay);
    }
    // 不再更新日线数据比上个交易日要新的股票。其他所有的股票都查询一遍，以防止出现新股票或者老的股票重新活跃起来。
    if (gl_systemTime.GetLastTradeDay() <= pStock->GetDayLineEndDay()) { // 最新日线数据为今日或者上一个交易日的数据。
      if (pStock->IsDayLineNeedUpdate()) pStock->SetDayLineNeedUpdate(false); // 日线数据不需要更新
    }
    else if (setStockCode.m_IPOed == __STOCK_NULL__) { // 无效代码不需更新日线数据
      if (pStock->IsDayLineNeedUpdate()) pStock->SetDayLineNeedUpdate(false);
    }
    else if (setStockCode.m_IPOed == __STOCK_DELISTED__) { // 退市股票如果已下载过日线数据，则不需要再更新日线数据
      if (pStock->IsDayLineNeedUpdate() && (pStock->GetDayLineEndDay() != 19900101)) pStock->SetDayLineNeedUpdate(false);
    }
    else {
      int i = 0;
    }
    setStockCode.MoveNext();
  }
  if (gl_ChinaStockMarket.m_iDayLineNeedUpdate > 0) {
    int i = gl_ChinaStockMarket.m_iDayLineNeedUpdate;
    TRACE("尚余%d个股票需要更新日线数据\n", i);
  }
  setStockCode.Close();
}

///////////////////////////////////////////////////////////////////////////////////
//
// 更新选项数据库
//
//
//
//////////////////////////////////////////////////////////////////////////////////
bool CMarket::UpdateOptionDB(void) {
  CSetOption setOption;
  setOption.Open();
  setOption.m_pDatabase->BeginTrans();
  if (setOption.IsEOF()) {
    setOption.AddNew();
    setOption.m_RelativeStrongEndDay = gl_ChinaStockMarket.GetRelativeStrongEndDay();
    setOption.m_RalativeStrongStartDay = gl_ChinaStockMarket.GetRelativeStrongStartDay();
    setOption.m_LastLoginDay = gl_systemTime.GetDay();
    setOption.Update();
  }
  else {
    setOption.Edit();
    setOption.m_RelativeStrongEndDay = gl_ChinaStockMarket.GetRelativeStrongEndDay();
    setOption.m_RalativeStrongStartDay = gl_ChinaStockMarket.GetRelativeStrongStartDay();
    setOption.m_LastLoginDay = gl_systemTime.GetDay();
    setOption.Update();
  }
  setOption.m_pDatabase->CommitTrans();
  setOption.Close();
  return false;
}

void CMarket::LoadOptionDB(void) {
  CSetOption setOption;
  setOption.Open();
  if (setOption.IsEOF()) {
    gl_ChinaStockMarket.SetRelativeStrongEndDay(19900101);
    gl_ChinaStockMarket.SetRelativeStrongEndDay(19900101);
    gl_ChinaStockMarket.SetLastLoginDay(19900101);
  }
  else {
    if (setOption.m_RelativeStrongEndDay == 0) {
      gl_ChinaStockMarket.SetRelativeStrongEndDay(19900101);
    }
    else {
      gl_ChinaStockMarket.SetRelativeStrongEndDay(setOption.m_RelativeStrongEndDay);
    }
    if (setOption.m_RalativeStrongStartDay == 0) {
      gl_ChinaStockMarket.SetRelativeStrongStartDay(19900101);
    }
    else {
      gl_ChinaStockMarket.SetRelativeStrongStartDay(setOption.m_RalativeStrongStartDay);
    }
    if (setOption.m_LastLoginDay == 0) {
      gl_ChinaStockMarket.SetLastLoginDay(19900101);
    }
    else {
      gl_ChinaStockMarket.SetLastLoginDay(setOption.m_LastLoginDay);
    }
  }

  setOption.Close();
}

bool CMarket::UpdateTempRTData(void) {
  if (!gl_ThreadStatus.IsSavingTempData()) {
    gl_ThreadStatus.SetSavingTempData(true);
    AfxBeginThread(ThreadSaveTempRTData, nullptr);
  }

  return false;
}