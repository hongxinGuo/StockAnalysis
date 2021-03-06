//////////////////////////////////////////////////////////////////////////////////////////////////
#include"pch.h"
#include"globedef.h"
#include"accessory.h"
#include"Thread.h"

#include"WebInquirer.h"

#include"ChinaStock.h"
#include"ChinaMarket.h"

#include"SetDayLineExtendInfo.h"
#include"SetDayLineToday.h"
#include"SetOption.h"
#include"SetChinaMarketOption.h"
#include"SetCrweberIndex.h"
#include"SetChoicedStock.h"
#include"SetRSStrong2Stock.h"
#include"SetRSStrong1Stock.h"
#include"SetRSStrongStock.h"
#include"SetRSOption.h"
#include"SetStockSection.h"

#include"SetWeekLineInfo.h"

using namespace std;
#include<thread>
#include<algorithm>
#include<set>

#include<gsl/gsl>
using namespace gsl;

// 信号量必须声明为全局变量（为了初始化）
Semaphore gl_SaveOneStockDayLine(4);  // 此信号量用于生成日线历史数据库
Semaphore gl_ProcessSinaRTDataQueue(1);   // 新浪实时数据处理同时只允许一个线程存在
Semaphore gl_ProcessTengxunRTDataQueue(1);
Semaphore gl_ProcessNeteaseRTDataQueue(1);
Semaphore gl_SemaphoreBackGroundTaskThreads(cMaxBackGroundTaskThreads); // 后台工作线程数。最大为8

CWebRTDataContainer gl_WebRTDataContainer;
CWebInquirer gl_WebInquirer;

bool CompareChinaMarketStock(CChinaStockPtr p1, CChinaStockPtr p2) { return (p1->GetSymbol().Compare(p2->GetSymbol()) < 0); }

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CChinaMarket::CChinaMarket(void) : CVirtualMarket() {
	static int siInstance = 0;

	if (++siInstance > 1) {
		TRACE(_T("ChinaMarket市场变量只允许存在一个实例\n"));
	}
	m_strMarketId = _T("中国股票市场");
	m_lMarketTimeZone = -8 * 3600; // 北京标准时间位于东八区，超前GMT8小时
	m_fSaveRTData = false; // 此存储实时数据标识，用于存储供测试函数用的实时数据。目前任务已经完成。

	m_avChoicedStock.resize(30);
	m_aRSStrongOption.resize(10);
	m_vStockSection.resize(2000); // 沪深各1000个段。
	CStockSectionPtr pStockSection;
	for (int i = 0; i < 2000; i++) {
		pStockSection = make_shared<CStockSection>();
		pStockSection->SetIndexNumber(i);
		if (i < 1000) pStockSection->SetMarket(__SHANGHAI_MARKET__);
		else pStockSection->SetMarket(__SHENZHEN_MARKET__);
		m_vStockSection.at(i) = pStockSection;
	}

	Reset();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 全局变量的解析位于程序退出的最后，要晚于CMainFrame的解析。故而如果要想将系统退出的过程放在这里，需要研究。
// 目前不允许此析构函数完成任何功能。
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
CChinaMarket::~CChinaMarket() {
	m_vChinaMarketStock.resize(0);
}

void CChinaMarket::ResetMarket(void) {
	TRACE(_T("重置中国股市\n"));
	CString str = _T("重置中国股市于北京标准时间：");
	str += GetStringOfMarketTime();
	gl_systemMessage.PushInformationMessage(str);
	while (gl_ThreadStatus.IsBackGroundthreadsWorking() || gl_ThreadStatus.IsCalculatingRTData() || gl_ThreadStatus.IsSavingTempData()
		|| gl_ThreadStatus.IsSavingThreadRunning()) {
		Sleep(1);
	}
	Reset();

	ASSERT(m_mapChinaMarketStock.size() == GetTotalStock()); // 读入数据库前，要保证已经装载了预先设置的股票代码
	LoadStockSection(); // 装入各段证券代码空间是否已被使用的标识（六位代码，以1000为单位增加，沪深各有1000000个可用代码）
	LoadStockCodeDB();
	LoadOptionDB();
	LoadOptionChinaStockMarketDB();
	LoadChoicedStockDB();
	Load10DaysRSStrong1StockSet();
	Load10DaysRSStrong2StockSet();
	LoadCalculatingRSOption();
	Load10DaysRSStrongStockDB();
}

void CChinaMarket::Reset(void) {
	CalculateTime(); // 初始化市场时间
	m_lLoadedStock = 0;

	m_llRTDataReceived = 0;
	m_lStockDayLineNeedUpdate = 0;

	m_fLoadedSelectedStock = false;
	SetSystemReady(false); // 市场初始状态为未设置好。
	m_fCurrentStockChanged = false;
	m_fChoiced10RSStrong1StockSet = false;
	m_fChoiced10RSStrong2StockSet = false;
	m_fChoiced10RSStrongStockSet = false;
	m_fCurrentEditStockChanged = false;
	m_fCalculateChoiced10RS = false;

	m_lTotalMarketBuy = m_lTotalMarketSell = 0;

	m_ttNewestTransactionTime = 0;

	if (GetFormatedMarketTime() >= 150400) { // 中国股票市场已经闭市
		SetTodayStockProcessed(true); // 闭市后才执行本系统，则认为已经处理过今日股票数据了。
	}
	else SetTodayStockProcessed(false);

	m_lRSEndDate = m_lRSStartDate = m_lLastLoginDay = __CHINA_MARKET_BEGIN_DATE__;
	m_lUpdatedDateFor10DaysRS2 = m_lUpdatedDateFor10DaysRS1 = m_lUpdatedDateFor10DaysRS = __CHINA_MARKET_BEGIN_DATE__;

	m_fSaveDayLine = false;
	m_fMarketOpened = false;
	m_fSaveTempData = true;

	m_fTodayTempDataLoaded = false;

	m_fUpdateStockSection = false;

	m_lCurrentRSStrongIndex = 0;
	m_lCurrentSelectedStockSet = -1; // 选择使用全体股票集、
	m_lCurrentSelectedPosition = 0;

	m_fRTDataSetCleared = false;

	m_fCheckActiveStock = true;  //检查当日活跃股票，必须为真。

	m_fGetRTData = true;
	m_iCountDownSlowReadingRTData = 3; // 400毫秒每次

	m_iRTDataServer = 0; // 使用新浪实时数据服务器
	m_iRTDataInquiryTickNumber = 3;// 默认400毫秒查询一次

	m_fUsingSinaRTDataReceiver = true; // 使用新浪实时数据提取器
	m_fUsingTengxunRTDataReceiver = true; // 默认状态下读取腾讯实时行情
	m_fUsingNeteaseRTDataReceiver = true; // 使用网易实时数据提取器
	m_iCountDownTengxunNumber = 5;

	m_fUpdateChoicedStockDB = false;

	m_lSinaStockRTDataInquiringIndexFromTotalStockSet = 0;
	m_lSinaStockRTDataInquiringIndex = 0;
	m_lTengxunRTDataInquiringIndex = 0;
	m_lNeteaseRTDataInquiringIndex = 0;
	m_lNeteaseDayLineDataInquiringIndex = 0;

	m_pCurrentStock = nullptr;

	m_vChinaMarketStock.clear();
	m_mapChinaMarketStock.clear();

	m_vTotalStockSet.resize(0);
	m_mapTotalStockSet.clear();

	m_vCurrentSectionStockCode.clear();
	// 预设股票代码集如下
	m_vCurrentSectionStockCode.push_back(_T("000000.SS")); // 上海指数
	m_vCurrentSectionStockCode.push_back(_T("600000.SS")); // 上海主板
	m_vCurrentSectionStockCode.push_back(_T("601000.SS")); // 上海主板
	m_vCurrentSectionStockCode.push_back(_T("603000.SS")); // 上海三板
	m_vCurrentSectionStockCode.push_back(_T("688000.SS")); // 上海科创板
	m_vCurrentSectionStockCode.push_back(_T("900000.SS")); // 上海B股
	m_vCurrentSectionStockCode.push_back(_T("000000.SZ")); // 深圳主板
	m_vCurrentSectionStockCode.push_back(_T("001000.SZ")); // 深圳主板
	m_vCurrentSectionStockCode.push_back(_T("002000.SZ")); // 深圳中小板
	m_vCurrentSectionStockCode.push_back(_T("200000.SZ")); // 深圳B股
	m_vCurrentSectionStockCode.push_back(_T("300000.SZ")); // 深圳创业板
	m_vCurrentSectionStockCode.push_back(_T("399000.SZ")); // 深圳指数
	// 从股票代码集数据库中读入其他股票集

	//重置StockSection
	for (int i = 0; i < m_vStockSection.size(); i++) {
		m_vStockSection.at(i)->SetBuildStockPtr(false);
	}
	// 生成股票代码池
	CreateTotalStockContainer();
}

#ifdef _DEBUG
void CChinaMarket::AssertValid() const {
	CVirtualMarket::AssertValid();
}

void CChinaMarket::Dump(CDumpContext& dc) const {
	CVirtualMarket::Dump(dc);
}
#endif //_DEBUG

bool CChinaMarket::CheckMarketReady(void) {
	if (!IsSystemReady()) {
		long lMax = GetTotalStock() > 12000 ? GetTotalStock() * 2 : 24000;
		if (m_llRTDataReceived > lMax) {
			SetSystemReady(true);
			gl_systemMessage.PushInformationMessage(_T("中国股票市场初始化完毕"));
		}
	}
	return IsSystemReady();
}

bool CChinaMarket::ChangeToNextStock(void) {
	ASSERT(m_pCurrentStock != nullptr);
	const long lIndex = m_pCurrentStock->GetOffset();
	CChinaStockPtr pStock = m_pCurrentStock;

	if (IsTotalStockSetSelected()) {
		bool fFound = false;
		int i = 1;
		while (!fFound) {
			if ((lIndex + i) < GetTotalStock()) {
				pStock = GetStock(lIndex + i);
			}
			else {
				pStock = GetStock(lIndex + i - GetTotalStock());
			}
			if (!pStock->IsNullStock()) fFound = true;
			i++;
		}
	}
	else {
		ASSERT(m_avChoicedStock.at(m_lCurrentSelectedStockSet).size() > 0); //
		if (m_lCurrentSelectedPosition >= (m_avChoicedStock.at(m_lCurrentSelectedStockSet).size() - 1)) {
			m_lCurrentSelectedPosition = 0;
			pStock = m_avChoicedStock.at(m_lCurrentSelectedStockSet).at(m_lCurrentSelectedPosition);
		}
		else {
			m_lCurrentSelectedPosition++;
			pStock = m_avChoicedStock.at(m_lCurrentSelectedStockSet).at(m_lCurrentSelectedPosition);
		}
	}

	SetCurrentStock(pStock);
	return true;
}

bool CChinaMarket::ChangeToPrevStock(void) {
	ASSERT(m_pCurrentStock != nullptr);
	const long lIndex = m_pCurrentStock->GetOffset();
	CChinaStockPtr pStock = m_pCurrentStock;

	if (IsTotalStockSetSelected()) {
		bool fFound = false;
		int i = 1;
		while (!fFound) {
			if ((lIndex - i) >= 0) {
				pStock = GetStock(lIndex - i);
			}
			else {
				pStock = GetStock(lIndex + GetTotalStock() - i);
			}
			if (!pStock->IsNullStock()) fFound = true;
			i++;
		}
	}
	else {
		ASSERT(m_avChoicedStock.at(m_lCurrentSelectedStockSet).size() > 0); //
		if (m_lCurrentSelectedPosition == 0) {
			m_lCurrentSelectedPosition = m_avChoicedStock.at(m_lCurrentSelectedStockSet).size() - 1;
			pStock = m_avChoicedStock.at(m_lCurrentSelectedStockSet).at(m_lCurrentSelectedPosition);
		}
		else {
			m_lCurrentSelectedPosition--;
			pStock = m_avChoicedStock.at(m_lCurrentSelectedStockSet).at(m_lCurrentSelectedPosition);
		}
	}
	SetCurrentStock(pStock);
	return true;
}

bool CChinaMarket::ChangeToPrevStockSet(void) {
	do {
		if (m_lCurrentSelectedStockSet > -1) m_lCurrentSelectedStockSet--;
		else {
			m_lCurrentSelectedStockSet = c_10DaysRSStockSetStartPosition + 9;
		}
		ASSERT(m_lCurrentSelectedStockSet < 20);
	} while ((m_lCurrentSelectedStockSet != -1) && (m_avChoicedStock.at(m_lCurrentSelectedStockSet).size() == 0));

	return true;
}

bool CChinaMarket::ChangeToNextStockSet(void) {
	do {
		if (m_lCurrentSelectedStockSet == (c_10DaysRSStockSetStartPosition + 9)) m_lCurrentSelectedStockSet = -1;
		else {
			m_lCurrentSelectedStockSet++;
		}
		ASSERT(m_lCurrentSelectedStockSet < 20);
	} while ((m_lCurrentSelectedStockSet != -1) && (m_avChoicedStock.at(m_lCurrentSelectedStockSet).size() == 0));

	return true;
}

size_t CChinaMarket::GetCurrentStockSetSize(void) {
	if (IsTotalStockSetSelected()) return GetTotalStock();
	else return m_avChoicedStock.at(m_lCurrentSelectedStockSet).size();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// 初始化所有可能的股票代码池，只被CChinaMarket的初始函数调用一次。
// 这个函数需要其他全局变量初始化的支持，故而gl_ChinaStockMarket实例需要放在所有全局变量的最后。
//
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CChinaMarket::CreateTotalStockContainer(void) {
	char buffer[10]{};

	CChinaStockPtr pStock = nullptr;

	// 清空之前的数据（如果有的话。在Reset时，这两个容器中就存有数据）。
	ASSERT(GetTotalStock() == 0);

	for (int i = 0; i < m_vCurrentSectionStockCode.size(); i++) {
		CreateStockSection(m_vCurrentSectionStockCode.at(i));
	}
	ASSERT(m_mapChinaMarketStock.size() == m_vChinaMarketStock.size());
	return true;
}

void CChinaMarket::CreateStockSection(CString strFirstStockCode) {
	CString strCode = GetStockSymbol(strFirstStockCode);
	CString strStockCode, strStockSymbol, strExchange;
	CString str = _T("");
	int iCode = atoi(strCode.GetBuffer());
	int iMarket = 0;
	char buffer[10];
	CChinaStockPtr pStock = nullptr;

	if (IsShanghaiExchange(strFirstStockCode)) { // 上海市场
		iMarket = 0;
	}
	else if (IsShenzhenExchange(strFirstStockCode)) { // 深圳市场
		iMarket = 1000;
	}
	if (m_vStockSection.at((iCode / 1000) + iMarket)->IsBuildStockPtr()) return; // 已经在证券池中建立了
	// 生成上海股票代码
	for (int i = iCode; i < (iCode + 1000); i++) {
		strExchange = GetStockExchange(strFirstStockCode);
		sprintf_s(buffer, _T("%06d"), i);
		strStockSymbol = buffer;
		strStockCode = CreateStockCode(strExchange, strStockSymbol);
		m_vTotalStockSet.push_back(strStockCode); //
		m_mapTotalStockSet[strStockCode] = m_vTotalStockSet.size();
	}
	if (UpdateStockSection(iCode / 1000 + iMarket)) {
		SetUpdateStockSection(true);
	}
	m_vStockSection.at(iCode / 1000 + iMarket)->SetBuildStockPtr(true); // 已经在证券池中建立了
}

bool CChinaMarket::CreateStock(CString strStockCode, CString strStockName, bool fProcessRTData) {
	CChinaStockPtr pStock;
	CString str;

	ASSERT(m_vChinaMarketStock.size() == m_mapChinaMarketStock.size());
	pStock = make_shared<CChinaStock>();
	pStock->SetActive(false);
	pStock->SetTodayNewStock(true);
	pStock->SetSymbol(strStockCode);
	pStock->SetStockName(strStockName);
	pStock->SetIPOStatus(__STOCK_NOT_CHECKED__);
	pStock->SetOffset(GetTotalStock());
	pStock->SetDayLineEndDate(19900101);
	pStock->SetDayLineStartDate(19900101);
	pStock->SetUpdateProfileDB(true);
	pStock->SetNeedProcessRTData(fProcessRTData);
	AddStock(pStock);
	ASSERT(pStock->IsDayLineNeedUpdate());
	str = _T("china Market生成新代码") + pStock->GetSymbol();
	gl_systemMessage.PushInnerSystemInformationMessage(str);
	ASSERT(m_vChinaMarketStock.size() == m_mapChinaMarketStock.size());
	return true;
}

bool CChinaMarket::AddStock(CChinaStockPtr pStock) {
	if (pStock == nullptr) return false;
	if (IsStock(pStock->GetSymbol())) return false;

	m_mapChinaMarketStock[pStock->GetSymbol()] = m_vChinaMarketStock.size(); // 使用下标生成新的映射
	m_vChinaMarketStock.push_back(pStock);

	return true;
}

bool CChinaMarket::DeleteStock(CChinaStockPtr pStock) {
	if (pStock == nullptr) return false;
	if (!IsStock(pStock->GetSymbol())) return false;

	m_mapChinaMarketStock.erase(pStock->GetSymbol());

	auto it = find(m_vChinaMarketStock.begin(), m_vChinaMarketStock.end(), pStock);
	m_vChinaMarketStock.erase(it);

	return true;
}

bool CChinaMarket::UpdateStockSection(CString strStockCode) {
	if (!IsStock(strStockCode)) return false;

	CString strCode = GetStockSymbol(strStockCode);
	int iCode = atoi(strCode.GetBuffer());
	int iMarket = 0;

	if (IsShanghaiExchange(strStockCode)) { // 上海市场
		iMarket = 0;
	}
	else if (IsShenzhenExchange(strStockCode)) { // 深圳市场
		iMarket = 1000;
	}
	return UpdateStockSection(iCode / 1000 + iMarket);
}

bool CChinaMarket::UpdateStockSection(long lIndex) {
	if (!m_vStockSection.at(lIndex)->IsActive()) {
		m_vStockSection.at(lIndex)->SetActive(true);
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 生成网易日线股票代码的字符串，用于查询此股票在当前市场是否处于活跃状态（或者是否存在此股票号码）
//
//  此函数是检查m_vChinaMarketStock股票池
//
//
//
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CChinaMarket::CreateNeteaseDayLineInquiringStr(CString& strReturn, long lEndPosition) {
	bool fFound = false;
	int iCount = 0;
	CString strTemp;
	strReturn = _T("");

	while (!fFound && (iCount++ < GetTotalStock())) {
		CChinaStockPtr pStock = m_vChinaMarketStock.at(m_lNeteaseDayLineDataInquiringIndex);
		if (!pStock->IsDayLineNeedUpdate()) { // 日线数据不需要更新。在系统初始时，设置此m_fDayLineNeedUpdate标识
			// TRACE("%S 日线数据无需更新\n", static_cast<LPCWSTR>(pStock->m_strSymbol));
			IncreaseStockInquiringIndex(m_lNeteaseDayLineDataInquiringIndex, lEndPosition);
		}
		else if (pStock->GetDayLineEndDate() >= GetLastTradeDate()) { //上一交易日的日线数据已经存储？此时已经处理过一次日线数据了，无需再次处理。
			pStock->SetDayLineNeedUpdate(false); // 此股票日线资料不需要更新了。
			// TRACE("%S 日线数据本日已更新\n", static_cast<LPCWSTR>(pStock->m_strSymbol));
			IncreaseStockInquiringIndex(m_lNeteaseDayLineDataInquiringIndex, lEndPosition);
		}
		else if (pStock->IsDayLineNeedProcess()) { // 日线数据已下载但尚未处理（一般此情况不会出现）
			IncreaseStockInquiringIndex(m_lNeteaseDayLineDataInquiringIndex, lEndPosition);
		}
		else {
			fFound = true;
		}
	}

	if (iCount >= GetTotalStock()) { //  没有找到需要申请日线的证券
		TRACE("未找到需更新日线历史数据的股票\n");
		return false;
	}

	// 找到了需申请日线历史数据的股票（siCounter为索引）
	CChinaStockPtr pStock = m_vChinaMarketStock.at(m_lNeteaseDayLineDataInquiringIndex);
	ASSERT(!pStock->IsDayLineNeedSaving());
	ASSERT(!pStock->IsDayLineNeedProcess());
	ASSERT(pStock->IsDayLineNeedUpdate());
	pStock->SetDayLineNeedUpdate(false);
	strReturn += XferStandredToNetease(pStock->GetSymbol());
	IncreaseStockInquiringIndex(m_lNeteaseDayLineDataInquiringIndex, lEndPosition);
	return true;
}

long CChinaMarket::IncreaseStockInquiringIndex(long& lIndex, long lEndPosition) {
	if (++lIndex >= lEndPosition) {
		lIndex = 0;
	}
	return lIndex;
}

/////////////////////////////////////////////////////////////////////////
//
//	得到分时线偏移量。09:30为0，15:00为240,步长为1分钟
//
//
////////////////////////////////////////////////////////////////////////
long CChinaMarket::GetMinLineOffset(time_t Time) {
	ASSERT(Time >= 0);
	tm tmTemp{};
	time_t t = 0;
	long lIndex = 0;

	gmtime_s(&tmTemp, &Time);
	tmTemp.tm_hour = 9;
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
bool CChinaMarket::IsAStock(not_null<CChinaStockPtr> pStock) const {
	return(IsAStock(pStock->GetSymbol()));
}

////////////////////////////////////////////////////////////////////////////////////////////
//
//		判断strStockCode是否为沪深A股主板的股票代码。
//		沪市A股代码以600或601开头，深市A股代码以000或001开头。
//
//
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool CChinaMarket::IsAStock(CString strStockCode) const {
	CString strSymbol = GetStockSymbol(strStockCode);
	if (IsShanghaiExchange(strStockCode)) {
		if ((strSymbol[0] == '6') && (strSymbol[1] == '0')) {
			if ((strSymbol[2] == '0') || (strSymbol[2] == '1')) {
				return true;
			}
		}
	}
	else if (IsShenzhenExchange(strStockCode)) {
		if ((strSymbol[0] == '0') && (strSymbol[1] == '0')) {
			if ((strSymbol[2] == '0') || (strSymbol[2] == '2')) {
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
bool CChinaMarket::IsStock(CString strStockCode) const {
	if (m_mapChinaMarketStock.find(strStockCode) == m_mapChinaMarketStock.end()) {
		return(false);
	}
	else {
		return(true);
	}
}

bool CChinaMarket::IsDayLineDBUpdated(void) noexcept {
	for (auto& pStock : m_vChinaMarketStock) {
		if (pStock->IsDayLineDBUpdated()) return true;
	}
	return false;
}

void CChinaMarket::ClearDayLineDBUpdatedFlag(void) noexcept {
	for (auto& pStock : m_vChinaMarketStock) {
		pStock->SetDayLineDBUpdated(false);
	}
}

//////////////////////////////////////////////////////////////////////////
//
//
//
//
/////////////////////////////////////////////////////////////////////////
INT64 CChinaMarket::GetTotalAttackBuyAmount(void) {
	INT64 lAmount = 0;
	for (auto& pStock : m_vChinaMarketStock) {
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
INT64 CChinaMarket::GetTotalAttackSellAmount(void) {
	INT64 lAmount = 0;
	for (auto& pStock : m_vChinaMarketStock) {
		if (pStock->IsActive() && IsAStock(pStock)) {
			lAmount += pStock->GetAttackSellAmount();
		}
	}
	return(lAmount);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 抓取网易历史日线数据
// 由于可能会抓取全部5000个左右日线数据，所需时间超过10分钟，故而9:15:00第一次重置系统时不去更新，而在9:25:00第二次重置系统后才开始。
// 为了防止与重启系统发生冲突，实际执行时间延后至9:26:00。
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CChinaMarket::TaskGetNeteaseDayLineFromWeb(void) {
	ASSERT(IsSystemReady());
	if ((GetFormatedMarketTime() >= 92600) && IsDayLineNeedUpdate()) {
		// 抓取日线数据.开始于09:26:00
		// 最多使用四个引擎，否则容易被网易服务器拒绝服务。一般还是用两个为好。
		return(gl_WebInquirer.GetNeteaseDayLineData());
	}
	else return false;
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
bool CChinaMarket::TaskDistributeSinaRTDataToProperStock(void) {
	gl_ProcessSinaRTDataQueue.Wait();
	const size_t lTotalNumber = gl_WebRTDataContainer.GetSinaDataSize();
	CString strVolume;
	bool fFoundNewStock = false;
	CString strStandredStockCode;
	CWebRTDataPtr pRTData = nullptr;
	CChinaStockPtr pStock = nullptr;

	for (int iCount = 0; iCount < lTotalNumber; iCount++) {
		pRTData = gl_WebRTDataContainer.PopSinaData();
		if (pRTData->GetDataSource() == __INVALID_RT_WEB_DATA__) {
			gl_systemMessage.PushInnerSystemInformationMessage(_T("新浪实时数据源设置有误"));
			continue;
		}
		if (pRTData->IsActive()) { // 此实时数据有效？
			if (m_ttNewestTransactionTime < pRTData->GetTransactionTime()) {
				m_ttNewestTransactionTime = pRTData->GetTransactionTime();
			}
			if (IsCheckActiveStock()) {
				if (!IsStock(pRTData->GetSymbol())) {
					CreateStock(pRTData->GetSymbol(), pRTData->GetStockName(), true);
					fFoundNewStock = true;
				}
			}
			pStock = GetStock(pRTData->GetSymbol());
			ASSERT(pStock != nullptr);
			if (!pStock->IsActive()) { // 这里在发行版运行时出现错误，原因待查。
				if (pRTData->IsValidTime(14)) {
					pStock->SetTodayActive(pRTData->GetSymbol(), pRTData->GetStockName());
					pStock->SetIPOStatus(__STOCK_IPOED__);
				}
			}
			if (pRTData->GetTransactionTime() > pStock->GetTransactionTime()) { // 新的数据？
				pStock->PushRTData(pRTData); // 存储新的数据至数据池
				if (pStock->IsRecordRTData()) {
					StoreChoiceRTData(pRTData);
				}
				pStock->SetTransactionTime(pRTData->GetTransactionTime());   // 设置最新接受到实时数据的时间
			}
		}
	}
	gl_ThreadStatus.SetRTDataNeedCalculate(true); // 设置接收到实时数据标识
	ASSERT(gl_WebRTDataContainer.GetSinaDataSize() == 0); // 必须一次处理全体数据。
	gl_ProcessSinaRTDataQueue.Signal();

	return true;
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
bool CChinaMarket::TaskDistributeNeteaseRTDataToProperStock(void) {
	gl_ProcessNeteaseRTDataQueue.Wait();
	CChinaStockPtr pStock;
	const size_t lTotalNumber = gl_WebRTDataContainer.GetNeteaseDataSize();
	CString strVolume;
	bool fFoundNewStock = false;

	for (int iCount = 0; iCount < lTotalNumber; iCount++) {
		CWebRTDataPtr pRTData = gl_WebRTDataContainer.PopNeteaseData();
		if (pRTData->GetDataSource() == __INVALID_RT_WEB_DATA__) {
			gl_systemMessage.PushInnerSystemInformationMessage(_T("网易实时数据源设置有误"));
			continue;
		}
		if (pRTData->IsActive()) { // 此实时数据有效？
			if (m_ttNewestTransactionTime < pRTData->GetTransactionTime()) {
				m_ttNewestTransactionTime = pRTData->GetTransactionTime();
			}
			if (IsCheckActiveStock()) {
				if (!IsStock(pRTData->GetSymbol())) {
					CreateStock(pRTData->GetSymbol(), pRTData->GetStockName(), true);
					fFoundNewStock = true;
				}
			}
			pStock = GetStock(pRTData->GetSymbol());
			ASSERT(pStock != nullptr);
			if (!pStock->IsActive()) {
				if (pRTData->IsValidTime(14)) {
					pStock->SetTodayActive(pRTData->GetSymbol(), pRTData->GetStockName());
					pStock->SetIPOStatus(__STOCK_IPOED__);
				}
			}
			if (pRTData->GetTransactionTime() > pStock->GetTransactionTime()) { // 新的数据？
				pStock->PushRTData(pRTData); // 存储新的数据至数据池
				if (pStock->IsRecordRTData()) {
					StoreChoiceRTData(pRTData);
				}
				pStock->SetTransactionTime(pRTData->GetTransactionTime());   // 设置最新接受到实时数据的时间
			}
		}
	}
	gl_ThreadStatus.SetRTDataNeedCalculate(true); // 设置接收到实时数据标识
	ASSERT(gl_WebRTDataContainer.GetNeteaseDataSize() == 0); // 必须一次处理全体数据。
	gl_ProcessNeteaseRTDataQueue.Signal();

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// 生成每次查询新浪实时股票数据的字符串
//
//////////////////////////////////////////////////////////////////////////////////////////
CString CChinaMarket::GetSinaStockInquiringStr(long lTotalNumber, bool fCheckActiveStock) {
	if (fCheckActiveStock) {
		return GetNextSinaStockInquiringMiddleStrFromTotalStockSet(m_lSinaStockRTDataInquiringIndexFromTotalStockSet, _T(","), lTotalNumber);
	}
	else {
		return GetNextStockInquiringMiddleStr(m_lSinaStockRTDataInquiringIndex, _T(","), lTotalNumber, GetTotalStock());
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// 生成每次查询腾讯实时股票数据的字符串
//
//////////////////////////////////////////////////////////////////////////////////////////
CString CChinaMarket::GetTengxunInquiringStockStr(long lTotalNumber, long lEndPosition) {
	ASSERT(IsSystemReady());
	return GetNextStockInquiringMiddleStr(m_lTengxunRTDataInquiringIndex, _T(","), lTotalNumber, lEndPosition);
}

CString CChinaMarket::GetNeteaseStockInquiringMiddleStr(long lTotalNumber, long lEndPosition, bool fCheckActiveStock) {
	if (fCheckActiveStock) {
		return GetNextNeteaseStockInquiringMiddleStrFromTotalStockSet(_T(","), lTotalNumber);
	}
	else {
		return GetNextNeteaseStockInquiringStr(lTotalNumber, lEndPosition);
	}
}

CString CChinaMarket::GetNextNeteaseStockInquiringStr(long lTotalNumber, long lEndPosition) {
	CString strStockCode, strRight6, strLeft2, strPrefix;

	m_strNeteaseRTDataInquiringStr = _T("");
	m_strNeteaseRTDataInquiringStr = XferStandredToNetease(m_vChinaMarketStock.at(m_lNeteaseRTDataInquiringIndex)->GetSymbol());
	IncreaseStockInquiringIndex(m_lNeteaseRTDataInquiringIndex, lEndPosition);
	int iCount = 1; // 从1开始计数，因为第一个数据前不需要添加postfix。
	while ((m_lNeteaseRTDataInquiringIndex < lEndPosition) && (iCount < lTotalNumber)) { // 每次最大查询量为lTotalNumber个股票
		iCount++;
		m_strNeteaseRTDataInquiringStr += _T(",");
		m_strNeteaseRTDataInquiringStr += XferStandredToNetease(m_vChinaMarketStock.at(m_lNeteaseRTDataInquiringIndex)->GetSymbol());
		if (m_lNeteaseRTDataInquiringIndex == 0) break;
		IncreaseStockInquiringIndex(m_lNeteaseRTDataInquiringIndex, lEndPosition);
	}
	if (m_lNeteaseRTDataInquiringIndex > 0) m_lNeteaseRTDataInquiringIndex--;// 退后一步，防止最后一个股票查询错误（其实不必要了）

	return m_strNeteaseRTDataInquiringStr;
}

bool CChinaMarket::CheckValidOfNeteaseDayLineInquiringStr(CString str) {
	CString strStockCode, strNetease;

	strNetease = str.Left(7);
	strStockCode = XferNeteaseToStandred(strNetease);
	CChinaStockPtr pStock = GetStock(strStockCode);
	if (pStock == nullptr) {
		CString strReport = _T("网易日线查询股票代码错误：");
		TRACE(_T("网易日线查询股票代码错误：%s\n"), strStockCode.GetBuffer());
		strReport += strStockCode;
		gl_systemMessage.PushInnerSystemInformationMessage(strReport);
		return false;
	}
	return true;
}

CString CChinaMarket::GetNextStockInquiringMiddleStr(long& iStockIndex, CString strPostfix, long lTotalNumber, long lEndPosition) {
	CString strReturn = _T("");
	CString strStockCode, strStockExchange, strStockSymbol;

	if (0 == lEndPosition) return _T("sh600000"); // 当没有证券可查询时，返回一个有效字符串
	strReturn = XferStandredToSina(m_vChinaMarketStock.at(iStockIndex)->GetSymbol());  // 得到第一个股票代码
	IncreaseStockInquiringIndex(iStockIndex, lEndPosition);
	int iCount = 1; // 从1开始计数，因为第一个数据前不需要添加postfix。
	while ((iStockIndex < lEndPosition) && (iCount < lTotalNumber)) { // 每次最大查询量为lTotalNumber个股票
		iCount++;
		strReturn += strPostfix;
		strReturn += XferStandredToSina(m_vChinaMarketStock.at(iStockIndex)->GetSymbol());  // 得到第一个股票代码
			// 每次查到最后时暂停一下。目前不使用之，已加快查询速度
		// if (iStockIndex == lStartPosition) break;
		IncreaseStockInquiringIndex(iStockIndex, lEndPosition);
	}
	if (iStockIndex > 0) iStockIndex--; // 退后一步，防止最后一个股票查询错误（其实不必要了）

	return strReturn;
}

CString CChinaMarket::GetNextSinaStockInquiringMiddleStrFromTotalStockSet(long& lStockIndex, CString strPostfix, long lTotalNumber) {
	CString strReturn = _T("");
	long lEndPosition = m_vTotalStockSet.size();

	if (0 == lEndPosition) return _T("sh600000"); // 当没有证券可查询时，返回一个有效字符串
	strReturn = XferStandredToSina(m_vTotalStockSet.at(lStockIndex));  // 得到第一个股票代码
	IncreaseStockInquiringIndex(lStockIndex, lEndPosition);
	int iCount = 1; // 从1开始计数，因为第一个数据前不需要添加postfix。
	while ((lStockIndex < lEndPosition) && (iCount < lTotalNumber)) { // 每次最大查询量为lTotalNumber个股票
		iCount++;
		strReturn += strPostfix;
		strReturn += XferStandredToSina(m_vTotalStockSet.at(lStockIndex));
		IncreaseStockInquiringIndex(lStockIndex, lEndPosition);
	}
	if (lStockIndex > 0) lStockIndex--; // 退后一步，防止最后一个股票查询错误（其实不必要了）

	return strReturn;
}

CString CChinaMarket::GetNextNeteaseStockInquiringMiddleStrFromTotalStockSet(CString strPostfix, long lTotalNumber) {
	CString strReturn;
	long lEndPosition = m_vTotalStockSet.size();
	static long s_lIndex = 0;

	if (0 == lEndPosition) return _T("0600000"); // 当没有证券可查询时，返回一个有效字符串
	strReturn = XferStandredToNetease(m_vTotalStockSet.at(s_lIndex));  // 得到第一个股票代码
	IncreaseStockInquiringIndex(s_lIndex, lEndPosition);
	int iCount = 1; // 从1开始计数，因为第一个数据前不需要添加postfix。
	while ((s_lIndex < lEndPosition) && (iCount < lTotalNumber)) { // 每次最大查询量为lTotalNumber个股票
		iCount++;
		strReturn += strPostfix;
		strReturn += XferStandredToNetease(m_vTotalStockSet.at(s_lIndex));  // 得到第一个股票代码
		IncreaseStockInquiringIndex(s_lIndex, lEndPosition);
	}
	if (s_lIndex > 0) s_lIndex--; // 退后一步，防止最后一个股票查询错误（其实不必要了）

	return strReturn;
}

////////////////////////////////////////////////////////////////////////////////////////
//
// 由工作线程ThreadCalculatingRTDataProc调用，注意全局变量的使用
//
// 目前调用的方式是直接加在主线程中，不采用工作线程模式调用。
//
/////////////////////////////////////////////////////////////////////////////////////////
bool CChinaMarket::TaskProcessRTData(void) {
	ASSERT(gl_ThreadStatus.IsRTDataNeedCalculate() && !gl_ThreadStatus.IsSavingTempData());// 此两个线程互斥
	for (auto& pStock : m_vChinaMarketStock) {
		if (pStock->IsActive()) {
			pStock->ProcessRTData();
		}
	}
	return true;
}

bool CChinaMarket::TaskProcessWebRTDataGetFromSinaServer(void) {
	CWebDataPtr pWebDataReceived = nullptr;
	const size_t lTotalData = gl_WebInquirer.GetSinaRTDataSize();
	for (int i = 0; i < lTotalData; i++) {
		pWebDataReceived = gl_WebInquirer.PopSinaRTData();
		pWebDataReceived->ResetCurrentPos();
		while (!pWebDataReceived->IsProcessedAllTheData()) {
			CWebRTDataPtr pRTData = make_shared<CWebRTData>();
			if (pRTData->ReadSinaData(pWebDataReceived)) {
				m_llRTDataReceived++;
				gl_WebRTDataContainer.PushSinaData(pRTData); // 将此实时数据指针存入实时数据队列
			}
			else return false;  // 后面的数据出问题，抛掉不用。
		}
	}
	return true;
}

void CChinaMarket::StoreChoiceRTData(CWebRTDataPtr pRTData) {
	m_qRTData.push(pRTData);
}

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
// 网易实时数据缺少关键性的成交金额一项，故而无法作为基本数据，只能作为补充用.（turnover即为成交金额，可以使用之。05/12/2020）
//
//////////////////////////////////////////////////////////////////////////////////////////////////
bool CChinaMarket::TaskProcessWebRTDataGetFromNeteaseServer(void) {
	CWebDataPtr pWebDataReceived = nullptr;
	int iCount = 0;

	const size_t lTotalData = gl_WebInquirer.GetNeteaseRTDataSize();
	for (int i = 0; i < lTotalData; i++) {
		pWebDataReceived = gl_WebInquirer.PopNeteaseRTData();
		pWebDataReceived->ResetCurrentPos();
		if (!IsInvalidNeteaseRTData(*pWebDataReceived)) {
			if (!IsValidNeteaseRTDataPrefix(*pWebDataReceived)) return false;
			iCount = 0;
			while (!((pWebDataReceived->GetCurrentPosData() == ' ') || (pWebDataReceived->GetCurrentPos() >= (pWebDataReceived->GetBufferLength() - 4)))) {
				CWebRTDataPtr pRTData = make_shared<CWebRTData>();
				if (pRTData->ReadNeteaseData(pWebDataReceived)) {// 检测一下
					CheckNeteaseRTDataValidation(*pRTData);
					iCount++;
					m_llRTDataReceived++;
					gl_WebRTDataContainer.PushNeteaseData(pRTData); // 将此实时数据指针存入实时数据队列
				}
				else return false;  // 后面的数据出问题，抛掉不用。
			}
			//TRACE(_T("ReadNetease正常结束,共接收了%d个数据\n"), iCount);
		}
	}

	return true;
}

bool CChinaMarket::IsInvalidNeteaseRTData(CWebData& WebDataReceived) {
	char buffer[50];
	char* pBuffer = buffer;
	CString strInvalidStock = _T("_ntes_quote_callback({ });"); // 此为无效股票查询到的数据格式，共26个字符
	WebDataReceived.GetData(pBuffer, 26, WebDataReceived.GetCurrentPos());
	buffer[26] = 0x000;
	CString str1 = buffer;

	if (str1.Compare(strInvalidStock) == 0) {
		ASSERT(WebDataReceived.GetBufferLength() == 26);
		return true;
	}
	else return false;
}

bool CChinaMarket::IsValidNeteaseRTDataPrefix(CWebData& WebDataReceived) {
	char buffer[50];
	CString strInvalidStock = _T("_ntes_quote_callback("); // 此为无效股票查询到的数据格式，共22个字符

	WebDataReceived.GetData(buffer, 21, WebDataReceived.GetCurrentPos()); // 读入"_ntes_quote_callback("
	buffer[21] = 0x000;
	CString str1;
	str1 = buffer;
	if (strInvalidStock.Compare(str1) != 0) { // 数据格式出错
		return false;
	}
	else {
		WebDataReceived.IncreaseCurrentPos(21);
		return true;
	}
}

bool CChinaMarket::CheckNeteaseRTDataValidation(CWebRTData& RTData) {
	// 检测一下
	CString str;

	ASSERT(RTData.GetDataSource() == __NETEASE_RT_WEB_DATA__);
	if (RTData.IsActive()) {
		CChinaStockPtr pStock = nullptr;
		if ((pStock = GetStock(RTData.GetSymbol())) != nullptr) {
			if (!pStock->IsActive()) {
				str = pStock->GetSymbol();
				str += _T(" 网易实时检测到不处于活跃状态");
				gl_systemMessage.PushInnerSystemInformationMessage(str);
				return false;
			}
		}
		else {
			str = RTData.GetSymbol();
			str += _T(" 无效股票代码（网易实时数据）");
			TRACE("\n无效股票代码%s\n", RTData.GetSymbol().GetBuffer());
			TRACE("申请的股票集为： %s\n\n", m_strNeteaseRTDataInquiringStr.GetBuffer());
			gl_systemMessage.PushInnerSystemInformationMessage(str);
			return false;
		}
		return true;
	}
	return false;
}

bool CChinaMarket::TaskDiscardNeteaseRTData(void) {
	CWebRTDataPtr pRTData = nullptr;
	const size_t lTotalData = gl_WebRTDataContainer.GetNeteaseDataSize();

	for (int i = 0; i < lTotalData; i++) {
		// 目前不使用网易实时数据，这里只是简单地取出后扔掉。
		pRTData = gl_WebRTDataContainer.PopNeteaseData();
		pRTData = nullptr;
	}

	return true;
}

bool CChinaMarket::TaskDiscardSinaRTData(void) {
	CWebRTDataPtr pRTData = nullptr;
	const size_t lTotalData = gl_WebRTDataContainer.GetSinaDataSize();

	for (int i = 0; i < lTotalData; i++) {
		// 目前不使用网易实时数据，这里只是简单地取出后扔掉。
		pRTData = gl_WebRTDataContainer.PopSinaData();
		pRTData = nullptr;
	}

	return true;
}

bool CChinaMarket::TaskDiscardTengxunRTData(void) {
	CWebRTDataPtr pRTData = nullptr;
	const size_t lTotalData = gl_WebRTDataContainer.GetTengxunDataSize();

	for (int i = 0; i < lTotalData; i++) {
		// 目前不使用网易实时数据，这里只是简单地取出后扔掉。
		pRTData = gl_WebRTDataContainer.PopTengxunData();
		pRTData = nullptr;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// 腾讯实时数据，如果遇到被查询股票代码为非上市时，只是简单略过，不返回数据。故而查询900个股票，返回的数据量要小于900.
// 只有当所有的查询股票皆为非上市时，才返回一个21个字符串：v_pv_none_match=\"1\";\n
//
//
//
/////////////////////////////////////////////////////////////////////////////////////////
bool CChinaMarket::TaskProcessWebRTDataGetFromTengxunServer(void) {
	CWebDataPtr pWebDataReceived = nullptr;
	int j = 0;

	const size_t lTotalData = gl_WebInquirer.GetTengxunRTDataSize();
	for (int i = 0; i < lTotalData; i++) {
		pWebDataReceived = gl_WebInquirer.PopTengxunRTData();
		pWebDataReceived->ResetCurrentPos();
		if (!IsInvalidTengxunRTData(*pWebDataReceived)) { // 处理这21个字符串的函数可以放在这里，也可以放在最前面。
			j = 0;
			while (!pWebDataReceived->IsProcessedAllTheData()) {
				CWebRTDataPtr pRTData = make_shared<CWebRTData>();
				if (pRTData->ReadTengxunData(pWebDataReceived)) {
					CheckTengxunRTDataValidation(*pRTData); // 检测一下
					j++;
					gl_WebRTDataContainer.PushTengxunData(pRTData); // 将此实时数据指针存入实时数据队列
				}
				else return false;  // 后面的数据出问题，抛掉不用。
			}
			//TRACE(_T("接收到%d个腾讯实时数据\n"), j);
		}
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 当所有被查询的股票皆为非上市股票时，腾讯实时股票服务器会返回一个21个字符长的字符串：v_pv_none_match=\"1\";\n
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CChinaMarket::IsInvalidTengxunRTData(CWebData& WebDataReceived) {
	char buffer[50];
	char* pBuffer = buffer;
	CString strInvalidStock = _T("v_pv_none_match=\"1\";\n"); // 此为无效股票查询到的数据格式，共21个字符

	WebDataReceived.GetData(pBuffer, 21, WebDataReceived.GetCurrentPos());
	buffer[21] = 0x000;
	CString str1 = buffer;

	if (str1.Compare(strInvalidStock) == 0) {
		ASSERT(WebDataReceived.GetBufferLength() == 21);
		return true;
	}
	else return false;
}

bool CChinaMarket::CheckTengxunRTDataValidation(CWebRTData& RTData) {
	CString str;
	ASSERT(RTData.GetDataSource() == __TENGXUN_RT_WEB_DATA__);
	if (RTData.IsActive()) {
		CChinaStockPtr pStock = nullptr;
		if ((pStock = GetStock(RTData.GetSymbol())) != nullptr) {
			if (!pStock->IsActive()) {
				str = pStock->GetSymbol();
				str += _T("腾讯实时检测到不处于活跃状态");
				gl_systemMessage.PushInnerSystemInformationMessage(str);
				return false;
			}
		}
		else {
			str = RTData.GetSymbol();
			str += _T("无效股票代码（腾讯实时数据）");
			gl_systemMessage.PushInnerSystemInformationMessage(str);
			return false;
		}
		return true;
	}
	return false;
}

bool CChinaMarket::TaskProcessTengxunRTData(void) {
	CWebRTDataPtr pRTData = nullptr;
	const size_t lTotalData = gl_WebRTDataContainer.GetTengxunDataSize();

	for (int i = 0; i < lTotalData; i++) {
		pRTData = gl_WebRTDataContainer.PopTengxunData();
		if (pRTData->IsActive()) {
			auto pStock = GetStock(pRTData->GetSymbol());
			pStock->SetTotalValue(pRTData->GetTotalValue());
			pStock->SetCurrentValue(pRTData->GetCurrentValue());
			pStock->SetHighLimit(pRTData->GetHighLimit());
			pStock->SetLowLimit(pRTData->GetLowLimit());
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
//
//
// 大约每100毫秒调度一次
//
//
///////////////////////////////////////////////////////////////////////////////////////////
bool CChinaMarket::SchedulingTask(void) {
	CVirtualMarket::SchedulingTask();

	static time_t s_timeLast = 0;
	const long lCurrentTime = GetFormatedMarketTime();

	// 抓取实时数据(新浪、腾讯和网易）。每400毫秒申请一次，即可保证在3秒中内遍历一遍全体活跃股票。
	if (m_fGetRTData && (m_iCountDownSlowReadingRTData <= 0)) {
		TaskGetRTDataFromWeb();
		TaskProcessWebRTDataGetFromSinaServer();
		TaskProcessWebRTDataGetFromNeteaseServer();
		// 如果要求慢速读取实时数据，则设置读取速率为每分钟一次
		if (!m_fFastReceivingRTData && IsSystemReady()) m_iCountDownSlowReadingRTData = 100 * m_iRTDataInquiryTickNumber; // 完全轮询一遍后，非交易时段一分钟左右更新一次即可
		else m_iCountDownSlowReadingRTData = m_iRTDataInquiryTickNumber;  // 默认计数4次,即每400毫秒申请一次实时数据
	}
	m_iCountDownSlowReadingRTData--;

	//根据时间，调度各项定时任务.每秒调度一次
	if (GetMarketTime() > s_timeLast) {
		SchedulingTaskPerSecond(GetMarketTime() - s_timeLast, lCurrentTime);
		s_timeLast = GetMarketTime();
	}

	// 系统准备好了之后需要完成的各项工作
	if (IsSystemReady()) {
		if (!m_fTodayTempDataLoaded) { // 此工作仅进行一次。
			LoadTodayTempDB();
			m_fTodayTempDataLoaded = true;
		}
		TaskProcessWebRTDataGetFromTengxunServer();
		TaskGetNeteaseDayLineFromWeb();
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
//
// 从新浪、网易或者腾讯实时行情数据服务器读取实时数据。使用其中之一即可。
//
/////////////////////////////////////////////////////////////////////////////////
bool CChinaMarket::TaskGetRTDataFromWeb(void) {
	switch (m_iRTDataServer) {
	case 0: // 使用新浪实时数据服务器
		if (IsUsingSinaRTDataReceiver()) {
			gl_WebInquirer.GetSinaRTData(); // 每400毫秒(100X4)申请一次实时数据。新浪的实时行情服务器响应时间不超过100毫秒（30-70之间），且没有出现过数据错误。
		}
		break;
	case 1: // 使用网易实时数据服务器
		// 网易实时数据有大量的缺失字段，且前缀后缀也有时缺失。
		// 网易实时数据有时还发送没有要求过的股票，不知为何。
		if (IsUsingNeteaseRTDataReceiver()) {
			// 读取网易实时行情数据。估计网易实时行情与新浪的数据源相同，故而两者可互换，使用其一即可。
			gl_WebInquirer.GetNeteaseRTData();
		}
		break;
	default: // 错误
		break;
	}

	if (IsSystemReady()) {
		// 读取腾讯实时行情数据。 由于腾讯实时行情的股数精度为手，没有零股信息，导致无法与新浪实时行情数据对接（新浪精度为股），故而暂时不用
		if (IsUsingTengxunRTDataReceiver()) {
			if (m_iCountDownTengxunNumber <= 0) {
				gl_WebInquirer.GetTengxunRTData();// 只有当系统准备完毕后，方可执行读取腾讯实时行情数据的工作。目前不使用此功能
				m_iCountDownTengxunNumber = 5;
			}
			else m_iCountDownTengxunNumber--; // 新浪实时数据读取五次，腾讯才读取一次。因为腾讯的挂单股数采用的是每手标准，精度不够
		}
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
bool CChinaMarket::SchedulingTaskPerSecond(long lSecond, long lCurrentTime) {
	static int s_iCountDownProcessWebRTData = 0;
	static int s_iCount1Hour = 3599;
	static int s_iCount5Minute = 299;
	static int s_iCount1Minute = 59;
	static int s_iCount10Second = 9;

	s_iCount10Second -= lSecond;
	s_iCount1Minute -= lSecond;
	s_iCount5Minute -= lSecond;
	s_iCount1Hour -= lSecond;
	if (s_iCount1Hour < 0) {
		s_iCount1Hour = 3599;
		SchedulingTaskPerHour(lCurrentTime);
	}
	if (s_iCount5Minute < 0) {
		s_iCount5Minute = 299;
		SchedulingTaskPer5Minutes(lCurrentTime);
	}
	if (s_iCount1Minute < 0) {
		s_iCount1Minute = 59;
		SchedulingTaskPerMinute(lCurrentTime);
	}
	if (s_iCount10Second < 0) {
		s_iCount10Second = 9;
		SchedulingTaskPer10Seconds(lCurrentTime);
	}

	CheckMarketReady(); // 检查市场是否完成初始化

	if ((GetDayLineNeedUpdateNumber() <= 0) && (GetDayLineNeedSaveNumber() <= 0) && m_fCalculateChoiced10RS) {
		TaskChoice10RSStrongStockSet(lCurrentTime);
		TaskChoice10RSStrong1StockSet(lCurrentTime);
		TaskChoice10RSStrong2StockSet(lCurrentTime);
	}

	// 判断是否开始正常收集数据
	TaskCheckStartReceivingData(lCurrentTime);
	// 判断中国股票市场开市状态
	TaskCheckMarketOpen(lCurrentTime);

	if (s_iCountDownProcessWebRTData <= 0) {
		// 将接收到的实时数据分发至各相关股票的实时数据队列中。
		// 由于有多个数据源，故而需要等待各数据源都执行一次后，方可以分发至相关股票处，故而需要每三秒执行一次，以保证各数据源至少都能提供一次数据。
		TaskDistributeSinaRTDataToProperStock();
		// 分发网易实时数据至各相关股票中。
		TaskDistributeNeteaseRTDataToProperStock();

		TaskProcessTengxunRTData();

		TaskDiscardNeteaseRTData();
		TaskDiscardSinaRTData();
		TaskDiscardTengxunRTData();
		s_iCountDownProcessWebRTData = 0;
	}
	else s_iCountDownProcessWebRTData--;

	// 计算实时数据，每秒钟一次。目前个股实时数据为每3秒钟一次更新，故而无需再快了。
	// 此计算任务要在DistributeRTDataReceivedFromWebToProperStock之后执行，以防止出现同步问题。
	// 在系统存储临时数据时不能同时计算实时数据，否则容易出现同步问题。如果系统正在存储临时实时数据，则等待一秒后的下一次轮询时再计算实时数据
	if (IsSystemReady() && !gl_ThreadStatus.IsSavingTempData() && IsTodayTempRTDataLoaded()) {
		if (gl_ThreadStatus.IsRTDataNeedCalculate()) {
			gl_ThreadStatus.SetCalculatingRTData(true);
			TaskProcessRTData();
			gl_ThreadStatus.SetRTDataNeedCalculate(false);
			gl_ThreadStatus.SetCalculatingRTData(false);
		}
	}

	TaskShowCurrentTransaction();

	// 装载当前股票日线数据
	TaskLoadCurrentStockHistoryData();

	return true;
}

bool CChinaMarket::SchedulingTaskPerHour(long lCurrentTime) {
	// 计算每一小时一次的任务
	return true;
}

bool CChinaMarket::SchedulingTaskPer5Minutes(long lCurrentTime) {
	// 计算每五分钟一次的任务。

	TaskUpdateStockCodeDB();

	if (IsSavingTempData()) {
		TaskSaveTempDataIntoDB(lCurrentTime);
	}

	return true;
}

void CChinaMarket::TaskSaveTempDataIntoDB(long lCurrentTime) {
	// 开市时每五分钟存储一次当前状态。这是一个备用措施，防止退出系统后就丢掉了所有的数据，不必太频繁。
	if (IsSystemReady() && m_fMarketOpened && !gl_ThreadStatus.IsCalculatingRTData()) {
		if (((lCurrentTime > 93000) && (lCurrentTime < 113600)) || ((lCurrentTime > 130000) && (lCurrentTime < 150600))) { // 存储临时数据严格按照交易时间来确定(中间休市期间和闭市后各要存储一次，故而到11:36和15:06才中止）
			CString str;
			str = _T("存储临时数据");
			gl_systemMessage.PushDayLineInfoMessage(str);
			UpdateTempRTData();
		}
	}
}

bool CChinaMarket::AddChoicedStock(CChinaStockPtr pStock) {
	auto it = find(m_avChoicedStock.at(0).cbegin(), m_avChoicedStock.at(0).cend(), pStock);
	if (it == m_avChoicedStock.at(0).end()) {
		m_avChoicedStock.at(0).push_back(pStock);
		ASSERT(!pStock->IsSaveToChoicedStockDB());
		return true;
	}
	return false;
}

bool CChinaMarket::DeleteChoicedStock(CChinaStockPtr pStock) {
	auto it = find(m_avChoicedStock.at(0).cbegin(), m_avChoicedStock.at(0).cend(), pStock);
	if (it == m_avChoicedStock.at(0).end()) {
		return false;
	}
	else {
		(*it)->SetChoiced(false);
		(*it)->SetSaveToChoicedStockDB(false);
		m_avChoicedStock.at(0).erase(it);
		return true;
	}
}

void CChinaMarket::ClearDayLineNeedUpdateStatus(void) {
	for (auto& pStock : m_vChinaMarketStock) {
		pStock->SetDayLineNeedUpdate(false);
	}
}

bool CChinaMarket::IsUpdateStockCodeDB(void) {
	for (auto& pStock : m_vChinaMarketStock) {
		if (pStock->IsUpdateProfileDB()) return true;
	}
	return false;
}

bool CChinaMarket::SchedulingTaskPerMinute(long lCurrentTime) {
	// 计算每分钟一次的任务。所有的定时任务，要按照时间间隔从长到短排列，即先执行每分钟一次的任务，再执行每秒钟一次的任务，这样能够保证长间隔的任务优先执行。
	TaskResetMarket(lCurrentTime);
	TaskResetMarketAgain(lCurrentTime);

	// 在开市前和中午暂停时查询所有股票池，找到当天活跃股票。
	TaskSetCheckActiveStockFlag(lCurrentTime);

	// 下午三点三分开始处理当日实时数据。
	TaskProcessTodayStock(lCurrentTime);

	TaskSaveChoicedRTData();

	TaskClearChoicedRTDataSet(lCurrentTime);

	TaskUpdateOptionDB();
	TaskUpdateChoicedStockDB();

	TaskCheckDayLineDB();

	if (m_fUpdateStockSection) {
		TaskSaveStockSection();
		m_fUpdateStockSection = false;
	}

	return true;
}

bool CChinaMarket::TaskSetCheckActiveStockFlag(long lCurrentTime) {
	if (!IsSystemReady()) {
		m_fCheckActiveStock = true;
		return true;
	}
	if (((lCurrentTime >= 91500) && (lCurrentTime < 92700))
		|| ((lCurrentTime >= 113300) && (lCurrentTime < 125900))
		|| (lCurrentTime > 150300)) {
		m_fCheckActiveStock = true;
		return true;
	}
	else {
		m_fCheckActiveStock = false;
		return false;
	}
}

bool CChinaMarket::TaskChoice10RSStrong1StockSet(long lCurrentTime) {
	if (IsSystemReady() && !m_fChoiced10RSStrong1StockSet && (lCurrentTime > 151100) && IsWorkingDay()) {
		CreatingThreadChoice10RSStrong1StockSet();
		m_fChoiced10RSStrong1StockSet = true;
		return true;
	}
	return false;
}

bool CChinaMarket::TaskChoice10RSStrong2StockSet(long lCurrentTime) {
	if (IsSystemReady() && !m_fChoiced10RSStrong2StockSet && (lCurrentTime > 151200) && IsWorkingDay()) {
		CreatingThreadChoice10RSStrong2StockSet();
		m_fChoiced10RSStrong2StockSet = true;
		return true;
	}
	return false;
}

bool CChinaMarket::TaskChoice10RSStrongStockSet(long lCurrentTime) {
	if (IsSystemReady() && !m_fChoiced10RSStrongStockSet && (lCurrentTime > 151000) && IsWorkingDay()) {
		CreatingThreadChoice10RSStrongStockSet();
		m_fChoiced10RSStrongStockSet = true;
		return true;
	}
	return false;
}

bool CChinaMarket::TaskProcessTodayStock(long lCurrentTime) {
	if (IsSystemReady() && (lCurrentTime >= 150400) && IsTodayStockNotProcessed() && IsWorkingDay()) {
		CreatingThreadProcessTodayStock();
		return true;
	}
	return false;
}

bool CChinaMarket::TaskCheckDayLineDB(void) {
	if (m_fSaveDayLine && (!IsDayLineNeedSaving()) && (!IsDayLineNeedUpdate()) && (!IsDayLineNeedProcess())) {
		m_fSaveDayLine = false;
		TRACE("日线历史数据更新完毕\n");
		CString str;
		str = _T("中国市场日线历史数据更新完毕");
		gl_systemMessage.PushInformationMessage(str);
		if (IsDayLineDBUpdated()) {
			// 更新股票池数据库
			ClearDayLineDBUpdatedFlag();
		}
		return true;
	}
	return false;
}

bool CChinaMarket::TaskCheckStartReceivingData(long lCurrentTime) {
	if (!IsWorkingDay()) { //周六或者周日闭市。结构tm用0--6表示星期日至星期六
		m_fFastReceivingRTData = false;
		return(m_fFastReceivingRTData);
	}
	else if ((lCurrentTime < 91200) || (lCurrentTime > 150630) || ((lCurrentTime > 114500) && (lCurrentTime < 124500))) { //下午三点六分三十秒市场交易结束（为了保证最后一个临时数据的存储）
		m_fFastReceivingRTData = false;

		return(m_fFastReceivingRTData);
	}
	else m_fFastReceivingRTData = true;
	return m_fFastReceivingRTData;
}

bool CChinaMarket::TaskCheckMarketOpen(long lCurrentTime) {
	if (!IsWorkingDay()) { //周六或者周日闭市。结构tm用0--6表示星期日至星期六
		m_fMarketOpened = false;
	}
	else if ((lCurrentTime > 92800) && (lCurrentTime < 150600)) { // 市场结束接收数据的时间，皆定为150600（与停止存储临时数据的时间一样）
		m_fMarketOpened = true;
	}
	else m_fMarketOpened = false;

	return m_fMarketOpened;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//
// 此任务必须每分钟调度一次，因其实现机制采用了
//
//////////////////////////////////////////////////////////////////////////////////////////////////
bool CChinaMarket::TaskResetMarket(long lCurrentTime) {
	// 九点十三分重启系统
// 必须在此时间段内重启，如果更早的话容易出现数据不全的问题。
	if (IsPermitResetMarket()) { // 如果允许重置系统
		if ((lCurrentTime >= 91300) && (lCurrentTime < 91400) && IsWorkingDay()) { // 交易日九点十五分重启系统
			if (!TooManyStockDayLineNeedUpdate()) { // 当有工作日作为休假日后，所有的日线数据都需要检查一遍，此时不在0915时重置系统以避免更新日线函数尚在执行。
				SetResetMarket(true);// 只是设置重启标识，实际重启工作由CMainFrame的OnTimer函数完成。
				SetSystemReady(false);
			}
		}
	}
	return true;
}

bool CChinaMarket::TaskResetMarketAgain(long lCurrentTime) {
	// 九点二十五分再次重启系统
	if (IsPermitResetMarket()) { // 如果允许重置系统
		if ((lCurrentTime >= 92500)) {
			if ((lCurrentTime <= 93000) && IsWorkingDay()) { // 交易日九点二十五分再次重启系统
				SetResetMarket(true);// 只是设置重启标识，实际重启工作由CMainFrame的OnTimer函数完成。
				SetSystemReady(false);
			}
			SetPermitResetMarket(false); // 今天不再允许重启系统。
		}
	}
	return true;
}

bool CChinaMarket::TaskUpdateStockCodeDB(void) {
	if (IsUpdateStockCodeDB()) {
		CreatingThreadUpdateStockCodeDB();
		return true;
	}
	return false;
}

bool CChinaMarket::TaskUpdateOptionDB(void) {
	if (IsUpdateOptionDB()) {
		CreatingThreadUpdateOptionDB();
		SetUpdateOptionDB(false);
		return true;
	}
	return false;
}

bool CChinaMarket::TaskUpdateChoicedStockDB(void) {
	if (IsUpdateChoicedStockDB()) {
		CreatingThreadAppendChoicedStockDB();
		return true;
	}
	return false;
}

bool CChinaMarket::TaskShowCurrentTransaction(void) {
	// 显示当前交易情况
	CChinaStockPtr pCurrentStock = GetCurrentStock();

	if (pCurrentStock != nullptr) {
		if (pCurrentStock->IsRTDataCalculated()) {
			if (pCurrentStock->GetCurrentTransationVolume() > 0) pCurrentStock->ReportGuadanTransaction();
			pCurrentStock->ReportGuadan();
			pCurrentStock->SetRTDataCalculated(false);
		}
	}

	return true;
}

bool CChinaMarket::TaskSaveChoicedRTData(void) {
	if (IsSystemReady() && m_fSaveRTData) {
		CreatingThreadSaveChoicedRTData();
		return true;
	}
	else return false;
}

/////////////////////////////////////////////////////////////////////
//
// 九点二十五分至九点三十分内清除昨日的实时数据。
//
/////////////////////////////////////////////////////////////////////
bool CChinaMarket::TaskClearChoicedRTDataSet(long lCurrentTime) {
	if (lCurrentTime > 93100) {
		m_fRTDataSetCleared = true;
	}

	if (!m_fRTDataSetCleared) {
		if ((lCurrentTime > 92900) && (lCurrentTime < 93100)) {
			CSetRealTimeData setRTData;
			setRTData.Open();
			setRTData.m_pDatabase->BeginTrans();
			while (!setRTData.IsEOF()) {
				setRTData.Delete();
				setRTData.MoveNext();
			}
			setRTData.m_pDatabase->CommitTrans();
			setRTData.Close();

			m_fRTDataSetCleared = true;
		}
	}
	return true;
}

bool CChinaMarket::TaskSaveStockSection(void) {
	CreatingThreadSaveStockSection();
	return true;
}

bool CChinaMarket::SaveStockSection(void) {
	CSetStockSection setStockSection;

	setStockSection.Open();
	setStockSection.m_pDatabase->BeginTrans();
	while (!setStockSection.IsEOF()) {
		setStockSection.Delete();
		setStockSection.MoveNext();
	}
	setStockSection.m_pDatabase->CommitTrans();
	setStockSection.Close();

	CStockSectionPtr pStockSection = nullptr;

	setStockSection.Open();
	setStockSection.m_pDatabase->BeginTrans();
	for (int i = 0; i < 2000; i++) {
		pStockSection = m_vStockSection.at(i);
		setStockSection.AddNew();
		setStockSection.m_ID = i;
		setStockSection.m_Active = pStockSection->IsActive();
		setStockSection.m_Market = pStockSection->GetMarket();
		setStockSection.m_IndexNumber = pStockSection->GetIndexNumber();
		setStockSection.m_Comment = pStockSection->GetComment();
		setStockSection.Update();
	}
	setStockSection.m_pDatabase->CommitTrans();
	setStockSection.Close();

	m_fUpdateStockSection = false;
	return true;
}

bool CChinaMarket::ChangeDayLineStockCodeToStandred(void) {
	CSetDayLineExtendInfo setDayLineExtendInfo;

	setDayLineExtendInfo.Open(AFX_DB_USE_DEFAULT_TYPE, NULL, CRecordset::executeDirect);
	setDayLineExtendInfo.m_pDatabase->BeginTrans();
	while (!setDayLineExtendInfo.IsEOF()) {
		setDayLineExtendInfo.Edit();
		setDayLineExtendInfo.m_Symbol = XferSinaToStandred(setDayLineExtendInfo.m_Symbol);
		setDayLineExtendInfo.Update();
		setDayLineExtendInfo.MoveNext();
	}
	setDayLineExtendInfo.m_pDatabase->CommitTrans();
	setDayLineExtendInfo.Close();
	gl_systemMessage.PushInformationMessage("Updated dayline extend info");
	return false;
}

bool CChinaMarket::SchedulingTaskPer10Seconds(long lCurrentTime) {
	// 计算每十秒钟一次的任务
		// 将处理日线历史数据的函数改为定时查询，读取和存储采用工作进程。
	if (IsDayLineNeedProcess()) {
		TaskProcessDayLineGetFromNeeteaseServer();
	}

	// 判断是否存储日线库和股票代码库
	if (IsDayLineNeedSaving()) {
		m_fSaveDayLine = true;
		TaskSaveDayLineData();
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
// 得到股票的简称
//
//////////////////////////////////////////////////////////////////////////////////////////////
CString CChinaMarket::GetStockName(CString strStockCode) {
	try {
		return (m_vChinaMarketStock.at(m_mapChinaMarketStock.at(strStockCode))->GetStockName());
	}
	catch (exception&) {
		TRACE("GetStockName函数异常\n");
		return _T("");
	}
}

////////////////////////////////////////////////////////////////////////////////
//
//	通过股票代码得到股票的指针。
//	如果没找到的话返回空指针
//
//
////////////////////////////////////////////////////////////////////////////////
CChinaStockPtr CChinaMarket::GetStock(CString strStockCode) {
	CChinaStockPtr pStock;
	if (!IsStock(strStockCode)) {
		return nullptr;
	}
	else {
		return (m_vChinaMarketStock.at(m_mapChinaMarketStock.at(strStockCode)));
	}
}

CChinaStockPtr CChinaMarket::GetStock(long lIndex) {
	try {
		return m_vChinaMarketStock.at(lIndex);
	}
	catch (exception&) {
		TRACE("GetStock越界\n");
		return nullptr;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//
//	通过股票代码和市场代码设置当前选择股票
//
//////////////////////////////////////////////////////////////////////////////////////
void CChinaMarket::SetCurrentStock(CString strStockCode) {
	CChinaStockPtr pStock = GetStock(strStockCode);
	SetCurrentStock(pStock);
	ASSERT(m_pCurrentStock != nullptr);
}

//////////////////////////////////////////////////////////////////////////
//
// 设置当前操作的股票
//
// 设置相应的股票指针，装载其日线数据。
//
/////////////////////////////////////////////////////////////////////////
void CChinaMarket::SetCurrentStock(CChinaStockPtr pStock) {
	bool fSet = false;
	if (m_pCurrentStock != nullptr) {
		if (!m_pCurrentStock->IsSameStock(pStock)) {
			m_pCurrentStock->SetRecordRTData(false);
			fSet = true;
		}
	}
	else {
		fSet = true;
	}
	if (fSet) {
		pStock->SetRecordRTData(true);
		m_pCurrentStock = pStock;
		SetCurrentStockChanged(true);
		m_pCurrentStock->SetDayLineLoaded(false); // 这里只是设置标识，实际装载日线由调度程序执行。
	}
}

void CChinaMarket::ResetCurrentStock(void) {
	if (m_pCurrentStock != nullptr) {
		m_pCurrentStock->SetRecordRTData(false);
		m_pCurrentStock = nullptr;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//	将日线数据存入数据库．
//  此函数由工作线程ThreadDayLineSaveProc调用，尽量不要使用全局变量。(目前使用主线程调用之，以消除同步问题的出现）
//
// 无论是否执行了存储函数，都需要将下载的日线历史数据删除，这样能够节省内存的占用。由于实际存储功能使用线程模式实现，
// 故而其执行时间可能晚于主线程，导致主线程删除日线数据时出现同步问题。解决的方法是让工作线程独立删除存储后的日线数据，
// 主线程的删除函数只在不调用工作线程（无需存储日线数据）的情况下方才执行。
//////////////////////////////////////////////////////////////////////////////////////////
bool CChinaMarket::TaskSaveDayLineData(void) {
	CString str;
	bool fSave = false;

	for (auto& pStock : m_vChinaMarketStock) {
		if (pStock->IsDayLineNeedSavingAndClearFlag()) { // 清除标识需要与检测标识处于同一原子过程中，防止同步问题出现
			if (pStock->GetDayLineSize() > 0) {
				if (pStock->HaveNewDayLineData()) {
					CreatingThreadSaveDayLineBasicInfoOfStock(pStock.get());
					fSave = true;
				}
				else pStock->UnloadDayLine(); // 当无需执行存储函数时，这里还要单独卸载日线数据。因存储日线数据线程稍后才执行，故而不能在此统一执行删除函数。
			}
			else { // 此种情况为有股票代码，但此代码尚未上市
				CString str1 = pStock->GetSymbol();
				str1 += _T(" 为未上市股票代码");
				gl_systemMessage.PushDayLineInfoMessage(str1);
			}
		}
		if (gl_fExitingSystem) {
			break; // 如果程序正在退出，则停止存储。
		}
	}

	return fSave;
}

bool CChinaMarket::UnloadDayLine(void) noexcept {
	for (auto& pStock : m_vChinaMarketStock) {
		pStock->UnloadDayLine();
	}

	return true;
}

bool CChinaMarket::BuildWeekLine(long lStartDate) {
	gl_systemMessage.PushInformationMessage(_T("重新生成周线历史数据"));
	for (auto& pStock : m_vChinaMarketStock) {
		CreatingThreadBuildWeekLineOfStock(pStock.get(), lStartDate);
	}
	while (gl_ThreadStatus.GetNumberOfBackGroundWorkingThread() > 0) {
		Sleep(1000);
	}
	gl_systemMessage.PushInformationMessage(_T("周线历史数据生成完毕"));

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 使用当前日期的日线数据生成本周的周线数据。
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CChinaMarket::BuildWeekLineOfCurrentWeek(void) {
	CDayLineContainer dayLineContainer;
	CWeekLineContainer weekLineContainer;
	set<CString> setDayLineStockCode;
	set<CString> setWeekLineStockCode;
	const long lCurrentMonday = GetCurrentMonday(GetFormatedMarketDate());

	if (!LoadDayLine(dayLineContainer, GetFormatedMarketDate())) {
		return true; // 加载本日日线数据失败，周线数据无需处理。
	}
	auto pDayLineData = dayLineContainer.GetContainer();

	gl_systemMessage.PushDayLineInfoMessage(_T("开始生成今日周线"));

	CreateStockCodeSet(setDayLineStockCode, dayLineContainer.GetContainer());

	DeleteCurrentWeekWeekLineBeforeTheDate(lCurrentMonday); // 从当前周周线表中清除掉本星期一之前的数据
	LoadCurrentWeekLine(weekLineContainer);
	CreateStockCodeSet(setWeekLineStockCode, weekLineContainer.GetContainer());

	CWeekLinePtr pWeekLine;
	for (auto& pData : *pDayLineData) {
		if (setWeekLineStockCode.find(pData->GetStockSymbol()) == setWeekLineStockCode.end()) { //周线数据容器中无此日线数据
			 // 存储此日线数据至周线数据容器
			pWeekLine = make_shared<CWeekLine>();
			pWeekLine->UpdateWeekLine(dynamic_pointer_cast<CDayLine>(pData));
			weekLineContainer.StoreData(pWeekLine);
		}
		else {
			// 更新周线数据容器
			weekLineContainer.UpdateData(dynamic_pointer_cast<CDayLine>(pData));
		}
	}

	// 清除之前的周线数据
	DeleteWeekLine(lCurrentMonday);
	// 存储周线数据值周线数据表
	SaveWeekLine(weekLineContainer);
	// 清除当前周的数据
	DeleteCurrentWeekWeekLine();
	// 存储当前周数据于当前周数据表
	SaveCurrentWeekLine(weekLineContainer);

	gl_systemMessage.PushDayLineInfoMessage(_T("生成今日周线任务完成"));

	return true;
}

bool CChinaMarket::CreateStockCodeSet(set<CString>& setStockCode, not_null<vector<CChinaStockHistoryDataPtr>*> pvData) {
	CString strStockSymbol;
	vector<CString> vectorStockCode;

	for (auto& pData : *pvData) {
		strStockSymbol = pData->GetStockSymbol();
		vectorStockCode.push_back(strStockSymbol);
	}
	setStockCode.insert(vectorStockCode.begin(), vectorStockCode.end());

	return true;
}

bool CChinaMarket::BuildCurrentWeekWeekLineTable(void) {
	const long lCurrentMonday = GetCurrentMonday(GetFormatedMarketDate());
	CSetWeekLineBasicInfo setWeekLineBasicInfo;
	CSetWeekLineExtendInfo setWeekLineExtendInfo;
	CString strDate;
	char buffer[10];
	CWeekLinePtr pWeekLine = nullptr;
	CWeekLineContainer weekLineContainer;

	DeleteCurrentWeekWeekLine();

	sprintf_s(buffer, _T("%08d"), lCurrentMonday);
	strDate = buffer;
	setWeekLineBasicInfo.m_strFilter = _T("[Date] = ");
	setWeekLineBasicInfo.m_strFilter += strDate;
	setWeekLineBasicInfo.m_strSort = _T("[Symbol]");
	setWeekLineBasicInfo.Open();

	setWeekLineExtendInfo.m_strFilter = _T("[Date] = ");
	setWeekLineExtendInfo.m_strFilter += strDate;
	setWeekLineExtendInfo.m_strSort = _T("[Symbol]");
	setWeekLineExtendInfo.Open();

	while (!setWeekLineBasicInfo.IsEOF()) {
		pWeekLine = make_shared<CWeekLine>();
		pWeekLine->LoadBasicData(&setWeekLineBasicInfo);
		while (!setWeekLineExtendInfo.IsEOF() && (setWeekLineBasicInfo.m_Symbol > setWeekLineExtendInfo.m_Symbol)) {
			setWeekLineExtendInfo.MoveNext();
		}
		if (setWeekLineExtendInfo.IsEOF()) {
			setWeekLineExtendInfo.MoveFirst();
		}
		else if (setWeekLineBasicInfo.m_Symbol == setWeekLineExtendInfo.m_Symbol) { // 由于存在事后补数据的缘故，此两个表的股票可能不是一一对应
			pWeekLine->LoadExtendData(&setWeekLineExtendInfo);
			weekLineContainer.StoreData(pWeekLine);
			setWeekLineExtendInfo.MoveNext();
		}
		else {
			setWeekLineExtendInfo.MoveFirst();
		}
		setWeekLineBasicInfo.MoveNext();
	}

	SaveCurrentWeekLine(weekLineContainer);

	return true;
}

bool CChinaMarket::SortStockVector(void) {
	sort(m_vChinaMarketStock.begin(), m_vChinaMarketStock.end(), CompareChinaMarketStock);
	m_mapChinaMarketStock.clear();
	int j = 0;
	for (auto& pStock : m_vChinaMarketStock) {
		pStock->SetOffset(j);
		ASSERT(!IsStock(pStock->GetSymbol()));
		m_mapChinaMarketStock[pStock->GetSymbol()] = j++;
	}
	ASSERT(m_vChinaMarketStock.size() == m_mapChinaMarketStock.size());
	return true;
}

bool CChinaMarket::LoadDayLine(CDayLineContainer& dayLineContainer, long lDate) {
	CString strSQL;
	CString strDate;
	char  pch[30];
	//CTime ctTime;
	CSetDayLineBasicInfo setDayLineBasicInfo;
	CSetDayLineExtendInfo setDayLineExtendInfo;

	sprintf_s(pch, _T("%08d"), lDate);
	strDate = pch;
	setDayLineBasicInfo.m_strSort = _T("[Symbol]");
	setDayLineBasicInfo.m_strFilter = _T("[Date] =");
	setDayLineBasicInfo.m_strFilter += strDate;
	setDayLineBasicInfo.Open();
	if (setDayLineBasicInfo.IsEOF()) { // 数据集为空，表明此日没有交易
		setDayLineBasicInfo.Close();
		CString str = strDate;
		str += _T("日数据集为空，无需处理周线数据");
		gl_systemMessage.PushDayLineInfoMessage(str);    // 采用同步机制报告信息
		return false;
	}
	setDayLineExtendInfo.m_strSort = _T("[Symbol]");
	setDayLineExtendInfo.m_strFilter = _T("[Date] =");
	setDayLineExtendInfo.m_strFilter += strDate;
	setDayLineExtendInfo.Open();
	setDayLineExtendInfo.m_pDatabase->BeginTrans();
	setDayLineBasicInfo.m_pDatabase->BeginTrans();
	while (!setDayLineBasicInfo.IsEOF()) {
		CDayLinePtr pDayLine = make_shared<CDayLine>();
		pDayLine->LoadChinaMarketBasicData(&setDayLineBasicInfo);
		while (!setDayLineExtendInfo.IsEOF() && (strcmp(setDayLineExtendInfo.m_Symbol, setDayLineBasicInfo.m_Symbol) < 0)) {
			setDayLineExtendInfo.MoveNext();
		}
		if (!setDayLineExtendInfo.IsEOF() && (strcmp(setDayLineExtendInfo.m_Symbol, setDayLineBasicInfo.m_Symbol) == 0)) {
			pDayLine->LoadChinaMarketExtendData(&setDayLineExtendInfo);
		}
		dayLineContainer.StoreData(pDayLine);
		setDayLineBasicInfo.MoveNext();
	}
	setDayLineBasicInfo.m_pDatabase->CommitTrans();
	setDayLineExtendInfo.m_pDatabase->CommitTrans();
	setDayLineBasicInfo.Close();
	setDayLineExtendInfo.Close();

	return true;
}

bool CChinaMarket::LoadWeekLineBasicInfo(CWeekLineContainer& weekLineContainer, long lMondayOfWeek) {
	CString strSQL;
	CString strDate;
	char  pch[30];
	//CTime ctTime;
	CSetWeekLineBasicInfo setWeekLineBasicInfo;

	sprintf_s(pch, _T("%08d"), lMondayOfWeek);
	strDate = pch;
	setWeekLineBasicInfo.m_strSort = _T("[Symbol]");
	setWeekLineBasicInfo.m_strFilter = _T("[Date] =");
	setWeekLineBasicInfo.m_strFilter += strDate;
	setWeekLineBasicInfo.Open();
	setWeekLineBasicInfo.m_pDatabase->BeginTrans();
	while (!setWeekLineBasicInfo.IsEOF()) {
		CWeekLinePtr pWeekLine = make_shared<CWeekLine>();
		pWeekLine->LoadBasicData(&setWeekLineBasicInfo);
		weekLineContainer.StoreData(pWeekLine);
	}
	setWeekLineBasicInfo.m_pDatabase->CommitTrans();
	setWeekLineBasicInfo.Close();

	return true;
}

bool CChinaMarket::DeleteWeekLine(long lMonday) {
	DeleteWeekLineBasicInfo(lMonday);
	DeleteWeekLineExtendInfo(lMonday);

	return true;
}

bool CChinaMarket::DeleteWeekLine(void) {
	if (gl_fTestMode) {
		ASSERT(0); // 由于处理实际数据库，故不允许测试此函数
		exit(1);
	}
	DeleteWeekLineBasicInfo();
	DeleteWeekLineExtendInfo();
	return true;
}

bool CChinaMarket::DeleteWeekLineBasicInfo(void) {
	CDatabase database;

	if (gl_fTestMode) {
		ASSERT(0); // 由于处理实际数据库，故不允许测试此函数
		exit(1);
	}

	database.Open(_T("ChinaMarket"), FALSE, FALSE, _T("ODBC;UID=hxguo;PASSWORD=hxguo;charset=utf8mb4"));
	database.BeginTrans();
	database.ExecuteSQL(_T("TRUNCATE `chinamarket`.`weekline`;"));
	database.CommitTrans();
	database.Close();

	return true;
}

bool CChinaMarket::DeleteWeekLineExtendInfo(void) {
	CDatabase database;

	if (gl_fTestMode) {
		ASSERT(0); // 由于处理实际数据库，故不允许测试此函数
		exit(1);
	}

	database.Open(_T("ChinaMarket"), FALSE, FALSE, _T("ODBC;UID=hxguo;PASSWORD=hxguo;charset=utf8mb4"));
	database.BeginTrans();
	database.ExecuteSQL(_T("TRUNCATE `chinamarket`.`weeklineinfo`;"));
	database.CommitTrans();
	database.Close();

	return true;
}

bool CChinaMarket::DeleteWeekLineBasicInfo(long lMonday) {
	CString strSQL;
	CString strDate;
	char  pch[30];
	//CTime ctTime;
	CSetWeekLineBasicInfo setWeekLineBasicInfo;

	sprintf_s(pch, _T("%08d"), lMonday);
	strDate = pch;
	setWeekLineBasicInfo.m_strFilter = _T("[Date] =");
	setWeekLineBasicInfo.m_strFilter += strDate;
	setWeekLineBasicInfo.Open();
	setWeekLineBasicInfo.m_pDatabase->BeginTrans();
	while (!setWeekLineBasicInfo.IsEOF()) {
		setWeekLineBasicInfo.Delete();
		setWeekLineBasicInfo.MoveNext();
	}
	setWeekLineBasicInfo.m_pDatabase->CommitTrans();
	setWeekLineBasicInfo.Close();

	return true;
}

bool CChinaMarket::DeleteWeekLineExtendInfo(long lMonday) {
	CString strSQL;
	CString strDate;
	char  pch[30];
	CSetWeekLineExtendInfo setWeekLineExtendInfo;

	sprintf_s(pch, _T("%08d"), lMonday);
	strDate = pch;
	setWeekLineExtendInfo.m_strFilter = _T("[Date] =");
	setWeekLineExtendInfo.m_strFilter += strDate;
	setWeekLineExtendInfo.Open();
	setWeekLineExtendInfo.m_pDatabase->BeginTrans();
	while (!setWeekLineExtendInfo.IsEOF()) {
		setWeekLineExtendInfo.Delete();
		setWeekLineExtendInfo.MoveNext();
	}
	setWeekLineExtendInfo.m_pDatabase->CommitTrans();
	setWeekLineExtendInfo.Close();

	return true;
}

bool CChinaMarket::SaveWeekLine(CWeekLineContainer& weekLineContainer) {
	weekLineContainer.SaveWeekLine();// 此容器中为各股票的当周周线数据

	return true;
}

bool CChinaMarket::SaveCurrentWeekLine(CWeekLineContainer& weekLineContainer) {
	weekLineContainer.SaveCurrentWeekLine();

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////
//
// 装载当前周周线表中的所有数据（使用CCurrentWeekLineInfo表）。
//
///////////////////////////////////////////////////////////////////////////////////////
bool CChinaMarket::LoadCurrentWeekLine(CWeekLineContainer& weekLineContainer) {
	CCurrentWeekLineInfo setCurrentWeekLineInfo;

	setCurrentWeekLineInfo.Open();
	setCurrentWeekLineInfo.m_pDatabase->BeginTrans();
	while (!setCurrentWeekLineInfo.IsEOF()) {
		CWeekLinePtr pWeekLine = make_shared<CWeekLine>();
		pWeekLine->LoadCurrentWeekData(&setCurrentWeekLineInfo);
		weekLineContainer.StoreData(pWeekLine);
		setCurrentWeekLineInfo.MoveNext();
	}
	setCurrentWeekLineInfo.m_pDatabase->CommitTrans();
	setCurrentWeekLineInfo.Close();

	return true;
}

bool CChinaMarket::DeleteCurrentWeekWeekLine(void) {
	CCurrentWeekLineInfo setCurrentWeekLineInfo;

	setCurrentWeekLineInfo.Open();
	setCurrentWeekLineInfo.m_pDatabase->BeginTrans();
	while (!setCurrentWeekLineInfo.IsEOF()) {
		setCurrentWeekLineInfo.Delete();
		setCurrentWeekLineInfo.MoveNext();
	}
	setCurrentWeekLineInfo.m_pDatabase->CommitTrans();
	setCurrentWeekLineInfo.Close();

	return true;
}

bool CChinaMarket::DeleteCurrentWeekWeekLineBeforeTheDate(long lCutOffDate) {
	CCurrentWeekLineInfo setCurrentWeekLineInfo;

	setCurrentWeekLineInfo.Open();
	setCurrentWeekLineInfo.m_pDatabase->BeginTrans();
	while (!setCurrentWeekLineInfo.IsEOF()) {
		if (setCurrentWeekLineInfo.m_Date < lCutOffDate) {
			setCurrentWeekLineInfo.Delete();
		}
		setCurrentWeekLineInfo.MoveNext();
	}
	setCurrentWeekLineInfo.m_pDatabase->CommitTrans();
	setCurrentWeekLineInfo.Close();

	return true;
}

CChinaStockPtr CChinaMarket::GetCurrentSelectedStock(void) {
	if (m_lCurrentSelectedStockSet >= 0) {
		return m_avChoicedStock.at(m_lCurrentSelectedStockSet).at(0);
	}
	else {
		return GetStock(0);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//	将实时数据存入数据库．默认数据库为空。
//
//
//////////////////////////////////////////////////////////////////////////////////////////
bool CChinaMarket::SaveRTData(void) {
	CSetRealTimeData setRTData;
	CWebRTDataPtr pRTData = nullptr;
	const size_t lTotal = m_qRTData.size();

	if (lTotal > 0) {
		setRTData.m_strFilter = _T("[ID] = 1");
		setRTData.Open();
		setRTData.m_pDatabase->BeginTrans();
		for (size_t i = 0; i < lTotal; i++) {
			pRTData = m_qRTData.front();
			m_qRTData.pop(); // 抛掉最前面这个数据
			pRTData->AppendData(setRTData);
		}
		setRTData.m_pDatabase->CommitTrans();
		setRTData.Close();
	}
	return(true);
}

bool CChinaMarket::Choice10RSStrong2StockSet(void) {
	vector<CChinaStockPtr> v10RSStrongStock;

	for (auto& pStock : m_vChinaMarketStock) {
		CreatingThreadCalculate10RSStrong2Stock(&v10RSStrongStock, pStock);
	}
	while (gl_ThreadStatus.GetNumberOfBackGroundWorkingThread() > 0) {
		if (gl_fExitingSystem) return false;
		Sleep(100); // 等待工作线程完成所有任务
	}

	CSetRSStrong2Stock setRSStrong2;

	setRSStrong2.Open();
	setRSStrong2.m_pDatabase->BeginTrans();
	while (!setRSStrong2.IsEOF()) {
		setRSStrong2.Delete();
		setRSStrong2.MoveNext();
	}
	setRSStrong2.m_pDatabase->CommitTrans();
	setRSStrong2.m_pDatabase->BeginTrans();
	for (auto& pStock : v10RSStrongStock) {
		setRSStrong2.AddNew();
		setRSStrong2.m_Symbol = pStock->GetSymbol();
		setRSStrong2.Update();
	}
	setRSStrong2.m_pDatabase->CommitTrans();
	setRSStrong2.Close();

	return true;
}

bool CChinaMarket::Choice10RSStrong1StockSet(void) {
	vector<CChinaStockPtr> v10RSStrongStock;

	for (auto& pStock : m_vChinaMarketStock) {
		CreatingThreadCalculate10RSStrong1Stock(&v10RSStrongStock, pStock);
	}
	while (gl_ThreadStatus.GetNumberOfBackGroundWorkingThread() > 0) {
		if (gl_fExitingSystem) return false;
		Sleep(100); // 等待工作线程完成所有任务
	}

	CSetRSStrong1Stock setRSStrong1;

	setRSStrong1.Open();
	setRSStrong1.m_pDatabase->BeginTrans();
	while (!setRSStrong1.IsEOF()) {
		setRSStrong1.Delete();
		setRSStrong1.MoveNext();
	}
	setRSStrong1.m_pDatabase->CommitTrans();
	setRSStrong1.m_pDatabase->BeginTrans();
	for (auto& pStock : v10RSStrongStock) {
		setRSStrong1.AddNew();
		setRSStrong1.m_Symbol = pStock->GetSymbol();
		setRSStrong1.Update();
	}
	setRSStrong1.m_pDatabase->CommitTrans();
	setRSStrong1.Close();

	return true;
}

bool CChinaMarket::Choice10RSStrongStockSet(CRSReference* pRef, int iIndex) {
	vector<CChinaStockPtr> v10RSStrongStock;

	for (auto& pStock : m_vChinaMarketStock) {
		CreatingThreadCalculate10RSStrongStock(&v10RSStrongStock, pRef, pStock);
	}

	while (gl_ThreadStatus.GetNumberOfBackGroundWorkingThread() > 0) {
		if (gl_fExitingSystem) return false;
		Sleep(100); // 等待工作线程完成所有任务
	}

	m_lCurrentRSStrongIndex = iIndex; // CSetRSStrongStock需要此m_lCurrentRSStrongIndex来选择正确的数据表。
	CSetRSStrongStock setRSStrong(iIndex);

	setRSStrong.Open();
	setRSStrong.m_pDatabase->BeginTrans();
	while (!setRSStrong.IsEOF()) {
		setRSStrong.Delete();
		setRSStrong.MoveNext();
	}
	setRSStrong.m_pDatabase->CommitTrans();
	setRSStrong.m_pDatabase->BeginTrans();
	for (auto& pStock : v10RSStrongStock) {
		setRSStrong.AddNew();
		setRSStrong.m_Symbol = pStock->GetSymbol();
		setRSStrong.Update();
	}
	setRSStrong.m_pDatabase->CommitTrans();
	setRSStrong.Close();

	return true;
}

bool CChinaMarket::IsDayLineNeedUpdate(void) const noexcept {
	for (auto& pStock : m_vChinaMarketStock) {
		if (pStock->IsDayLineNeedUpdate()) return true;
	}
	return false;
}

bool CChinaMarket::IsDayLineNeedProcess(void) const noexcept {
	for (auto& pStock : m_vChinaMarketStock) {
		if (pStock->IsDayLineNeedProcess()) return true;
	}
	return false;
}

bool CChinaMarket::IsDayLineNeedSaving(void) const {
	for (auto& pStock : m_vChinaMarketStock) {
		if (pStock->IsDayLineNeedSaving()) {
			return true;
		}
	}
	return false;
}

long CChinaMarket::GetDayLineNeedUpdateNumber(void) {
	long l = 0;
	for (auto& pStock : m_vChinaMarketStock) {
		if (pStock->IsDayLineNeedUpdate()) l++;
	}
	return l;
}

long CChinaMarket::GetDayLineNeedProcessNumber(void) {
	long l = 0;
	for (auto& pStock : m_vChinaMarketStock) {
		if (pStock->IsDayLineNeedProcess()) l++;
	}
	return l;
}
long CChinaMarket::GetDayLineNeedSaveNumber(void) {
	long l = 0;
	for (auto& pStock : m_vChinaMarketStock) {
		if (pStock->IsDayLineNeedSaving()) l++;
	}
	return l;
}

bool CChinaMarket::TaskProcessDayLineGetFromNeeteaseServer(void) {
	for (auto& pStock : m_vChinaMarketStock) {
		if (pStock->IsDayLineNeedProcess()) {
			pStock->ProcessNeteaseDayLineData();
			pStock->ResetTempDayLineDataBuffer();
		}
	}
	return true;
}

bool CChinaMarket::TaskLoadCurrentStockHistoryData(void) {
	if (m_pCurrentStock != nullptr) {
		if (!m_pCurrentStock->IsDayLineLoaded()) {
			CreatingThreadLoadDayLine(m_pCurrentStock.get());
			m_pCurrentStock->SetDayLineLoaded(true);
		}
		if (!m_pCurrentStock->IsWeekLineLoaded()) {
			CreatingThreadLoadWeekLine(m_pCurrentStock.get());
			m_pCurrentStock->SetWeekLineLoaded(true);
		}
	}
	return true;
}

bool CChinaMarket::CreatingThreadSaveChoicedRTData(void) {
	thread thread1(ThreadSaveRTData, this);
	thread1.detach();// 必须分离之，以实现并行操作，并保证由系统回收资源。
	return true;
}

bool CChinaMarket::CreatingThreadProcessTodayStock(void) {
	thread thread1(ThreadProcessTodayStock, this);
	thread1.detach();// 必须分离之，以实现并行操作，并保证由系统回收资源。
	return true;
}

bool CChinaMarket::CreatingThreadBuildDayLineRS(long lStartCalculatingDay) {
	thread thread1(ThreadBuildDayLineRS, this, lStartCalculatingDay);
	thread1.detach();// 必须分离之，以实现并行操作，并保证由系统回收资源。
	return true;
}

bool CChinaMarket::CreatingThreadBuildDayLineRSOfDate(long lThisDay) {
	thread thread1(ThreadBuildDayLineRSOfDate, this, lThisDay);
	thread1.detach(); // 必须分离之，以实现并行操作，并保证由系统回收资源。
	return true;
}

bool CChinaMarket::CreatingThreadBuildWeekLineRSOfDate(long lThisDay) {
	thread thread1(ThreadBuildWeekLineRSOfDate, this, lThisDay);
	thread1.detach(); // 必须分离之，以实现并行操作，并保证由系统回收资源。
	return true;
}

bool CChinaMarket::CreatingThreadSaveTempRTData(void) {
	thread thread1(ThreadSaveTempRTData, this);
	thread1.detach();// 必须分离之，以实现并行操作，并保证由系统回收资源。
	return true;
}

bool CChinaMarket::CreatingThreadSaveDayLineBasicInfoOfStock(CChinaStock* pStock) {
	thread thread1(ThreadSaveDayLineBasicInfoOfStock, pStock);
	thread1.detach();// 必须分离之，以实现并行操作，并保证由系统回收资源。
	return true;
}

bool CChinaMarket::CreatingThreadLoadDayLine(CChinaStock* pCurrentStock) {
	thread thread1(ThreadLoadDayLine, pCurrentStock);
	thread1.detach();// 必须分离之，以实现并行操作，并保证由系统回收资源。
	return true;
}

bool CChinaMarket::CreatingThreadLoadWeekLine(CChinaStock* pCurrentStock) {
	thread thread1(ThreadLoadWeekLine, pCurrentStock);
	thread1.detach();// 必须分离之，以实现并行操作，并保证由系统回收资源。
	return true;
}

bool CChinaMarket::CreatingThreadUpdateStockCodeDB(void) {
	thread thread1(ThreadUpdateStockCodeDB, this);
	thread1.detach();// 必须分离之，以实现并行操作，并保证由系统回收资源。
	return true;
}

bool CChinaMarket::CreatingThreadUpdateOptionDB(void) {
	thread thread1(ThreadUpdateOptionDB, this);
	thread1.detach();// 必须分离之，以实现并行操作，并保证由系统回收资源。
	return true;
}

bool CChinaMarket::CreatingThreadAppendChoicedStockDB(void) {
	thread thread1(ThreadAppendChoicedStockDB, this);
	thread1.detach();// 必须分离之，以实现并行操作，并保证由系统回收资源。
	return true;
}

bool CChinaMarket::CreatingThreadChoice10RSStrong2StockSet(void) {
	thread thread1(ThreadChoice10RSStrong2StockSet, this);
	thread1.detach();// 必须分离之，以实现并行操作，并保证由系统回收资源。
	return true;
}

bool CChinaMarket::CreatingThreadChoice10RSStrong1StockSet(void) {
	thread thread1(ThreadChoice10RSStrong1StockSet, this);
	thread1.detach();// 必须分离之，以实现并行操作，并保证由系统回收资源。
	return true;
}

bool CChinaMarket::CreatingThreadChoice10RSStrongStockSet(void) {
	for (int i = 0; i < 10; i++) {
		if (m_aRSStrongOption.at(i).m_fActive) {
			thread thread1(ThreadChoice10RSStrongStockSet, this, &(m_aRSStrongOption.at(i)), i);
			thread1.detach();// 必须分离之，以实现并行操作，并保证由系统回收资源。
		}
	}
	SetUpdatedDateFor10DaysRS(GetFormatedMarketDate());
	SetUpdateOptionDB(true); // 更新选项数据库.此时计算工作线程只是刚刚启动，需要时间去完成。

	return true;
}

bool CChinaMarket::CreatingThreadCalculate10RSStrong1Stock(vector<CChinaStockPtr>* pv10RSStrongStock, CChinaStockPtr pStock) {
	thread thread1(ThreadCalculate10RSStrong1Stock, pv10RSStrongStock, pStock);
	thread1.detach();

	return true;
}

bool CChinaMarket::CreatingThreadCalculate10RSStrong2Stock(vector<CChinaStockPtr>* pv10RSStrongStock, CChinaStockPtr pStock) {
	thread thread1(ThreadCalculate10RSStrong2Stock, pv10RSStrongStock, pStock);
	thread1.detach();

	return true;
}

bool CChinaMarket::CreatingThreadBuildWeekLine(long lStartDate) {
	thread thread1(ThreadBuildWeekLine, this, lStartDate);
	thread1.detach();
	return true;
}

bool CChinaMarket::CreatingThreadBuildWeekLineOfStock(CChinaStock* pStock, long lStartDate) {
	thread thread1(ThreadBuildWeekLineOfStock, pStock, lStartDate);
	thread1.detach();

	return true;
}

bool CChinaMarket::CreatingThreadBuildWeekLineRS(void) {
	thread thread1(ThreadBuildWeekLineRS, this, __CHINA_MARKET_BEGIN_DATE__);
	thread1.detach();

	return true;
}

bool CChinaMarket::CreatingThreadCalculate10RSStrongStock(vector<CChinaStockPtr>* pv10RSStrongStock, CRSReference* pRef, CChinaStockPtr pStock) {
	thread thread1(ThreadCalculate10RSStrongStock, pv10RSStrongStock, pRef, pStock);
	thread1.detach();

	return true;
}

bool CChinaMarket::CreatingThreadBuildWeekLineOfCurrentWeek(void) {
	thread thread1(ThreadBuildWeekLineOfCurrentWeek, this);
	thread1.detach();

	return true;
}

bool CChinaMarket::CreatingThreadBuildCurrentWeekWeekLineTable(void) {
	thread thread1(ThreadBuildCurrentWeekWeekLineTable, this);
	thread1.detach();

	return true;
}

bool CChinaMarket::CreatingThreadSaveStockSection(void) {
	thread thread1(ThreadSaveStockSection, this);
	thread1.detach();

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
long CChinaMarket::BuildDayLine(long lCurrentTradeDay) {
	char buffer[20]{ 0 };
	CString strDate;
	CSetDayLineBasicInfo setDayLineBasicInfo;
	CSetDayLineExtendInfo setDayLineExtendInfo;
	long iCount = 0;

	CString str;
	str = _T("开始处理");
	str += GetStringOfDate(lCurrentTradeDay);
	str += _T("的实时数据");
	gl_systemMessage.PushInformationMessage(str);

	DeleteDayLineBasicInfo(lCurrentTradeDay);
	DeleteDayLineExtendInfo(lCurrentTradeDay);

	// 存储当前交易日的数据
	_ltoa_s(lCurrentTradeDay, buffer, 10);
	strDate = buffer;
	setDayLineBasicInfo.m_strFilter = _T("[ID] = 1");
	setDayLineBasicInfo.Open();
	setDayLineBasicInfo.m_pDatabase->BeginTrans();
	for (auto& pStock : m_vChinaMarketStock) {
		if (!pStock->IsTodayDataActive()) {  // 此股票今天停牌,所有的数据皆为零,不需要存储.
			continue;
		}
		iCount++;
		pStock->SetDayLineEndDate(lCurrentTradeDay);
		pStock->SetIPOStatus(__STOCK_IPOED__); // 再设置一次。防止新股股票代码由于没有历史数据而被误判为不存在。
		pStock->SetUpdateProfileDB(true);
		setDayLineBasicInfo.AddNew();
		pStock->SaveTodayBasicInfo(&setDayLineBasicInfo);
		setDayLineBasicInfo.Update();
	}
	setDayLineBasicInfo.m_pDatabase->CommitTrans();
	setDayLineBasicInfo.Close();

	// 存储今日生成的数据于DayLineExtendInfo表中。
	setDayLineExtendInfo.m_strFilter = _T("[ID] = 1");
	setDayLineExtendInfo.Open();
	setDayLineExtendInfo.m_pDatabase->BeginTrans();
	for (auto& pStock : m_vChinaMarketStock) {
		if (!pStock->IsTodayDataActive()) {  // 此股票今天停牌,所有的数据皆为零,不需要存储.
			continue;
		}
		setDayLineExtendInfo.AddNew();
		pStock->SaveTodayExtendInfo(&setDayLineExtendInfo);
		setDayLineExtendInfo.Update();
	}
	setDayLineExtendInfo.m_pDatabase->CommitTrans();
	setDayLineExtendInfo.Close();

	str = GetStringOfDate(lCurrentTradeDay);
	str += _T("的实时数据处理完毕");
	gl_systemMessage.PushInformationMessage(str);

	sprintf_s(buffer, _T("%d"), iCount);
	str = _T("今日处理了");
	str += buffer;
	str += _T("个股票");
	gl_systemMessage.PushInformationMessage(str);

	return iCount;
}

bool CChinaMarket::DeleteDayLine(long lDate) {
	DeleteDayLineBasicInfo(lDate);
	DeleteDayLineExtendInfo(lDate);

	return true;
}

bool CChinaMarket::DeleteDayLineBasicInfo(long lDate) {
	char buffer[20]{ 0x000 };
	CString strDate;
	CSetDayLineBasicInfo setDayLineBasicInfo;

	_ltoa_s(lDate, buffer, 10);
	strDate = buffer;
	setDayLineBasicInfo.m_strFilter = _T("[Date] =");
	setDayLineBasicInfo.m_strFilter += strDate;
	setDayLineBasicInfo.Open();
	setDayLineBasicInfo.m_pDatabase->BeginTrans();
	while (!setDayLineBasicInfo.IsEOF()) {
		setDayLineBasicInfo.Delete();
		setDayLineBasicInfo.MoveNext();
	}
	setDayLineBasicInfo.m_pDatabase->CommitTrans();
	setDayLineBasicInfo.Close();

	return true;
}

bool CChinaMarket::DeleteDayLineExtendInfo(long lDate) {
	char buffer[20]{ 0x000 };
	CString strDate;
	CSetDayLineExtendInfo setDayLineExtendInfo;

	_ltoa_s(lDate, buffer, 10);
	strDate = buffer;
	setDayLineExtendInfo.m_strFilter = _T("[Date] =");
	setDayLineExtendInfo.m_strFilter += strDate;
	setDayLineExtendInfo.Open();
	setDayLineExtendInfo.m_pDatabase->BeginTrans();
	while (!setDayLineExtendInfo.IsEOF()) {
		setDayLineExtendInfo.Delete();
		setDayLineExtendInfo.MoveNext();
	}
	setDayLineExtendInfo.m_pDatabase->CommitTrans();
	setDayLineExtendInfo.Close();

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
//
// 将当日处理好的数据储存于数据库中，以备万一系统崩溃时重新装入。
// 似乎应该以一个定时工作线程的形式存在。
//
// 研究之。
//
//////////////////////////////////////////////////////////////////////////////////
bool CChinaMarket::UpdateTodayTempDB(void) {
	CSetDayLineToday setDayLineToday;
	CString str;

	DeleteTodayTempDB();

	// 存储今日生成的数据于DayLineToday表中。
	setDayLineToday.m_strFilter = _T("[ID] = 1");
	setDayLineToday.Open();
	setDayLineToday.m_pDatabase->BeginTrans();
	for (auto& pStock : m_vChinaMarketStock) {
		if (!pStock->IsTodayDataActive()) {  // 此股票今天停牌,所有的数据皆为零,不需要存储.
			continue;
		}
		if (pStock->IsNeedProcessRTData() && (!pStock->IsVolumeConsistence())) {
			str = pStock->GetSymbol();
			str += _T(" 股数不正确");
			gl_systemMessage.PushInnerSystemInformationMessage(str);
		}
		setDayLineToday.AddNew();
		pStock->SaveTempInfo(setDayLineToday);
		setDayLineToday.Update();
	}
	setDayLineToday.m_pDatabase->CommitTrans();
	setDayLineToday.Close();

	return true;
}

bool CChinaMarket::DeleteTodayTempDB(void) {
	CDatabase database;

	if (gl_fTestMode) {
		ASSERT(0); // 由于处理实际数据库，故不允许测试此函数
		exit(1); //退出系统
	}

	database.Open(_T("ChinaMarket"), FALSE, FALSE, _T("ODBC;UID=hxguo;PASSWORD=hxguo;charset=utf8mb4"));
	database.BeginTrans();
	database.ExecuteSQL(_T("TRUNCATE `chinamarket`.`today`;"));
	database.CommitTrans();
	database.Close();

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
bool CChinaMarket::LoadTodayTempDB(void) {
	CChinaStockPtr pStock = nullptr;
	CSetDayLineToday setDayLineToday;
	CWebRTDataPtr pRTData;

	ASSERT(!m_fTodayTempDataLoaded);
	ASSERT(!gl_ThreadStatus.IsCalculatingRTData());    // 执行此初始化工作时，计算实时数据的工作线程必须没有运行。
	// 读取今日生成的数据于DayLineToday表中。
	setDayLineToday.Open();
	if (!setDayLineToday.IsEOF()) {
		if (setDayLineToday.m_Date == GetFormatedMarketDate()) { // 如果是当天的行情，则载入，否则放弃（默认所有的数据日期皆为同一个时间）
			while (!setDayLineToday.IsEOF()) {
				if ((pStock = GetStock(setDayLineToday.m_Symbol)) != nullptr) {
					ASSERT(!pStock->HaveFirstRTData()); // 确保没有开始计算实时数据
					pStock->LoadTempInfo(setDayLineToday);
				}
				setDayLineToday.MoveNext();
			}
		}
	}
	setDayLineToday.Close();

	return true;
}

bool CChinaMarket::Load10DaysRSStrong1StockSet(void) {
	CSetRSStrong1Stock setRSStrong1;
	CChinaStockPtr pStock = nullptr;

	m_v10RSStrong1Stock.clear();
	setRSStrong1.Open();
	while (!setRSStrong1.IsEOF()) {
		pStock = gl_pChinaMarket->GetStock(setRSStrong1.m_Symbol);
		if (pStock != nullptr) {
			m_v10RSStrong1Stock.push_back(pStock);
		}
		setRSStrong1.MoveNext();
	}
	setRSStrong1.Close();

	return true;
}

bool CChinaMarket::Load10DaysRSStrong2StockSet(void) {
	CSetRSStrong2Stock setRSStrong2;
	CChinaStockPtr pStock = nullptr;

	m_v10RSStrong2Stock.clear();
	setRSStrong2.Open();
	while (!setRSStrong2.IsEOF()) {
		pStock = gl_pChinaMarket->GetStock(setRSStrong2.m_Symbol);
		if (pStock != nullptr) {
			m_v10RSStrong2Stock.push_back(pStock);
		}
		setRSStrong2.MoveNext();
	}
	setRSStrong2.Close();

	return true;
}

bool CChinaMarket::LoadCalculatingRSOption(void) {
	CSetRSOption setRSOption;

	setRSOption.Open();
	while (!setRSOption.IsEOF()) {
		m_aRSStrongOption.at(setRSOption.m_Index).m_fActive = setRSOption.m_Active;
		m_aRSStrongOption.at(setRSOption.m_Index).m_lDayLength[0] = setRSOption.m_DayLengthFirst;
		m_aRSStrongOption.at(setRSOption.m_Index).m_lDayLength[1] = setRSOption.m_DayLengthSecond;
		m_aRSStrongOption.at(setRSOption.m_Index).m_lDayLength[2] = setRSOption.m_DayLengthThird;
		m_aRSStrongOption.at(setRSOption.m_Index).m_lDayLength[3] = setRSOption.m_DayLengthFourth;
		m_aRSStrongOption.at(setRSOption.m_Index).m_lStrongDayLength[0] = setRSOption.m_StrongDayLengthFirst;
		m_aRSStrongOption.at(setRSOption.m_Index).m_lStrongDayLength[1] = setRSOption.m_StrongDayLengthSecond;
		m_aRSStrongOption.at(setRSOption.m_Index).m_lStrongDayLength[2] = setRSOption.m_StrongDayLengthThird;
		m_aRSStrongOption.at(setRSOption.m_Index).m_lStrongDayLength[3] = setRSOption.m_StrongDayLengthFourth;
		m_aRSStrongOption.at(setRSOption.m_Index).m_dRSStrong[0] = atof(setRSOption.m_RSStrongFirst);
		m_aRSStrongOption.at(setRSOption.m_Index).m_dRSStrong[1] = atof(setRSOption.m_RSStrongSecond);
		m_aRSStrongOption.at(setRSOption.m_Index).m_dRSStrong[2] = atof(setRSOption.m_RSStrongThird);
		m_aRSStrongOption.at(setRSOption.m_Index).m_dRSStrong[3] = atof(setRSOption.m_RSStrongFourth);
		setRSOption.MoveNext();
	}
	setRSOption.Close();
	return true;
}

void CChinaMarket::SaveCalculatingRSOption(void) {
	CSetRSOption setRSOption;

	setRSOption.Open();
	setRSOption.m_pDatabase->BeginTrans();
	while (!setRSOption.IsEOF()) {
		setRSOption.Delete();
		setRSOption.MoveNext();
	}
	setRSOption.m_pDatabase->CommitTrans();
	setRSOption.Close();

	setRSOption.m_strFilter = _T("[ID] = 1");
	setRSOption.Open();
	setRSOption.m_pDatabase->BeginTrans();
	for (int i = 0; i < 10; i++) {
		setRSOption.AddNew();
		setRSOption.m_Index = i;
		setRSOption.m_Active = m_aRSStrongOption.at(i).m_fActive;
		setRSOption.m_DayLengthFirst = m_aRSStrongOption.at(i).m_lDayLength[0];
		setRSOption.m_DayLengthSecond = m_aRSStrongOption.at(i).m_lDayLength[1];
		setRSOption.m_DayLengthThird = m_aRSStrongOption.at(i).m_lDayLength[2];
		setRSOption.m_DayLengthFourth = m_aRSStrongOption.at(i).m_lDayLength[3];
		setRSOption.m_StrongDayLengthFirst = m_aRSStrongOption.at(i).m_lStrongDayLength[0];
		setRSOption.m_StrongDayLengthSecond = m_aRSStrongOption.at(i).m_lStrongDayLength[1];
		setRSOption.m_StrongDayLengthThird = m_aRSStrongOption.at(i).m_lStrongDayLength[2];
		setRSOption.m_StrongDayLengthFourth = m_aRSStrongOption.at(i).m_lStrongDayLength[3];
		setRSOption.m_RSStrongFirst = ConvertValueToString(m_aRSStrongOption.at(i).m_dRSStrong[0]);
		setRSOption.m_RSStrongSecond = ConvertValueToString(m_aRSStrongOption.at(i).m_dRSStrong[1]);
		setRSOption.m_RSStrongThird = ConvertValueToString(m_aRSStrongOption.at(i).m_dRSStrong[2]);
		setRSOption.m_RSStrongFourth = ConvertValueToString(m_aRSStrongOption.at(i).m_dRSStrong[3]);
		setRSOption.Update();
	}
	setRSOption.m_pDatabase->CommitTrans();
	setRSOption.Close();
}

bool CChinaMarket::Load10DaysRSStrongStockDB(void) {
	for (int i = 0; i < 10; i++) {
		LoadOne10DaysRSStrongStockDB(i);
	}
	return true;
}

bool CChinaMarket::LoadOne10DaysRSStrongStockDB(long lIndex) {
	m_lCurrentRSStrongIndex = lIndex;
	CSetRSStrongStock setRSStrongStock(lIndex);

	setRSStrongStock.Open();
	while (!setRSStrongStock.IsEOF()) {
		CChinaStockPtr pStock = gl_pChinaMarket->GetStock(setRSStrongStock.m_Symbol);
		if (pStock != nullptr) m_avChoicedStock.at(m_lCurrentRSStrongIndex + c_10DaysRSStockSetStartPosition).push_back(pStock); // 10日RS股票集起始位置为第10个。
		setRSStrongStock.MoveNext();
	}
	setRSStrongStock.Close();

	return true;
}

///////////////////////////////////////////////////////////////////////////////////
//
// 计算lDate的日线相对强度, lDate的格式为：YYYYMMDD,如 19990605.
// 将日线按涨跌排列后,其相对强弱即其在队列中的位置.
// m_dRSIndex则是计算相对指数的涨跌强度。
//
//////////////////////////////////////////////////////////////////////////////////
bool CChinaMarket::BuildDayLineRS(long lDate) {
	vector<CChinaStockPtr> vStock;
	vector<int> vIndex;
	vector<double> vRS;
	int iTotalAShare = 0;
	CString strSQL;
	CString strDate;
	char  pch[30];
	int iStockNumber = 0;
	CSetDayLineBasicInfo setDayLineBasicInfo;
	double dShanghaiIndexUpDownRate = 0;
	double dShenzhenIndexUpDownRate = 0;
	double dIndexUpDownRate{ 0.0 };
	double dRSIndex{ 0.0 };

	sprintf_s(pch, _T("%08d"), lDate);
	strDate = pch;
	setDayLineBasicInfo.m_strSort = _T("[UpDownRate]");
	setDayLineBasicInfo.m_strFilter = _T("[Date] =");
	setDayLineBasicInfo.m_strFilter += strDate;
	setDayLineBasicInfo.Open();
	if (setDayLineBasicInfo.IsEOF()) { // 数据集为空，表明此日没有交易
		setDayLineBasicInfo.Close();
		CString str = strDate;
		str += _T("日数据集为空，没有计算相对强度");
		gl_systemMessage.PushDayLineInfoMessage(str);    // 采用同步机制报告信息
		return false;
	}
	setDayLineBasicInfo.m_pDatabase->BeginTrans();
	iStockNumber = 0;
	while (!setDayLineBasicInfo.IsEOF()) {
		if (strcmp(setDayLineBasicInfo.m_Symbol, _T("sh000001")) == 0) { // 上海综指
			dShanghaiIndexUpDownRate = GetUpDownRate(setDayLineBasicInfo.m_Close, setDayLineBasicInfo.m_LastClose);
		}
		else if (strcmp(setDayLineBasicInfo.m_Symbol, _T("sz399001")) == 0) { // 深圳成指
			dShenzhenIndexUpDownRate = GetUpDownRate(setDayLineBasicInfo.m_Close, setDayLineBasicInfo.m_LastClose);
		}
		if (IsAStock(setDayLineBasicInfo.m_Symbol)) {
			const long lIndex = m_mapChinaMarketStock.at(setDayLineBasicInfo.m_Symbol);
			vStock.push_back(m_vChinaMarketStock.at(lIndex));
			vIndex.push_back(iStockNumber); // 将A股的索引记录在容器中。
			iTotalAShare++;
		}
		iStockNumber++;
		setDayLineBasicInfo.MoveNext();
	}
	dIndexUpDownRate = (dShanghaiIndexUpDownRate + dShenzhenIndexUpDownRate) / 2;

	setDayLineBasicInfo.MoveFirst();
	int iCount = 0;
	int iBefore = 0;
	while (iCount < vIndex.size()) { // 只计算活跃股票的相对强度
		for (int i = 0; i < vIndex.at(iCount) - iBefore; i++) { // 根据索引去更改数据库,跨过不是A股的股票
			setDayLineBasicInfo.MoveNext();
		}
		setDayLineBasicInfo.Edit();
		const double dLastClose = atof(setDayLineBasicInfo.m_LastClose);
		const double dLow = atof(setDayLineBasicInfo.m_Low);
		const double dHigh = atof(setDayLineBasicInfo.m_High);
		const double dClose = atof(setDayLineBasicInfo.m_Close);
		double dUpDownRate = 0;
		// 计算指数相对强度
		if (dLastClose < 0.001) { // 新股上市等，昨日收盘价格为零
			dRSIndex = 50;
		}
		else {
			dUpDownRate = (dClose - dLastClose) / dLastClose;
			if ((dUpDownRate > 0.11) || (dUpDownRate < -0.11)) { // 除权等导致价格突变
				dRSIndex = 50;
			}
			else {
				dRSIndex = (dUpDownRate - dIndexUpDownRate) * 500 + 50; // 以大盘涨跌为基准（50）。
			}
		}
		setDayLineBasicInfo.m_RSIndex = ConvertValueToString(dRSIndex);

		// 计算涨跌排名相对强度
		if (dLastClose < 0.001) {
			setDayLineBasicInfo.m_RS = ConvertValueToString(50); // 新股上市或者除权除息，不计算此股
		}
		else if (((dLow / dLastClose) < 0.88) || ((dHigh / dLastClose) > 1.12)) { // 除权、新股上市等
			setDayLineBasicInfo.m_RS = ConvertValueToString(50); // 新股上市或者除权除息，不计算此股
		}
		else if ((fabs(dHigh - dClose) < 0.0001) && (((dClose / dLastClose)) > 1.095)) { // 涨停板
			setDayLineBasicInfo.m_RS = ConvertValueToString(100);
		}
		else if ((fabs(dClose - dLow) < 0.0001) && ((dClose / dLastClose) < 0.905)) { // 跌停板
			setDayLineBasicInfo.m_RS = ConvertValueToString(0);
		}
		else {
			setDayLineBasicInfo.m_RS = ConvertValueToString((static_cast<double>(iCount) * 100) / iTotalAShare);
		}
		setDayLineBasicInfo.Update();
		iBefore = vIndex.at(iCount++);
		setDayLineBasicInfo.MoveNext(); // 移到下一个数据。
		iBefore++; // 计数器也同时加一。
	}
	setDayLineBasicInfo.m_pDatabase->CommitTrans();
	setDayLineBasicInfo.Close();

	vStock.clear();
	vIndex.clear();
	vRS.clear();

	CString strDate2 = GetStringOfDate(lDate);
	CString strTemp;
	strTemp = strDate2 + _T("的股票日线相对强度计算完成");
	gl_systemMessage.PushDayLineInfoMessage(strTemp);    // 采用同步机制报告信息

	return(true);
}

///////////////////////////////////////////////////////////////////////////////////
//
// 计算lDate的周线相对强度, lDate的格式为：YYYYMMDD,如 19990605.
// 将周线按涨跌排列后,其相对强弱即其在队列中的位置.
// m_dRSIndex则是计算相对指数的涨跌强度。
//
//////////////////////////////////////////////////////////////////////////////////
bool CChinaMarket::BuildWeekLineRS(long lDate) {
	vector<CChinaStockPtr> vStock;
	vector<int> vIndex;
	vector<double> vRS;
	int iTotalAShare = 0;
	CString strSQL;
	CString strDate;
	char  pch[30];
	int iStockNumber = 0;
	//CTime ctTime;
	CSetWeekLineBasicInfo setWeekLineBasicInfo;
	double dShanghaiIndexUpDownRate = 0;
	double dShenzhenIndexUpDownRate = 0;
	double dIndexUpDownRate{ 0 };
	double dRSIndex{ 0 };

	ASSERT(GetCurrentMonday(lDate) == lDate); // 确保此日期为星期一

	sprintf_s(pch, _T("%08d"), lDate);
	strDate = pch;
	setWeekLineBasicInfo.m_strSort = _T("[UpDownRate]");
	setWeekLineBasicInfo.m_strFilter = _T("[Date] =");
	setWeekLineBasicInfo.m_strFilter += strDate;
	setWeekLineBasicInfo.Open();
	if (setWeekLineBasicInfo.IsEOF()) { // 数据集为空，表明此日没有交易
		setWeekLineBasicInfo.Close();
		CString str = strDate;
		str += _T("日数据集为空，没有计算相对强度");
		gl_systemMessage.PushDayLineInfoMessage(str);    // 采用同步机制报告信息
		return false;
	}
	setWeekLineBasicInfo.m_pDatabase->BeginTrans();
	iStockNumber = 0;
	while (!setWeekLineBasicInfo.IsEOF()) {
		if (strcmp(setWeekLineBasicInfo.m_Symbol, _T("sh000001")) == 0) { // 上海综指
			dShanghaiIndexUpDownRate = GetUpDownRate(setWeekLineBasicInfo.m_Close, setWeekLineBasicInfo.m_LastClose);
		}
		else if (strcmp(setWeekLineBasicInfo.m_Symbol, _T("sz399001")) == 0) { // 深圳成指
			dShenzhenIndexUpDownRate = GetUpDownRate(setWeekLineBasicInfo.m_Close, setWeekLineBasicInfo.m_LastClose);
		}
		if (IsAStock(setWeekLineBasicInfo.m_Symbol)) {
			const long lIndex = m_mapChinaMarketStock.at(setWeekLineBasicInfo.m_Symbol);
			vStock.push_back(m_vChinaMarketStock.at(lIndex));
			vIndex.push_back(iStockNumber); // 将A股的索引记录在容器中。
			iTotalAShare++;
		}
		iStockNumber++;
		setWeekLineBasicInfo.MoveNext();
	}
	dIndexUpDownRate = (dShanghaiIndexUpDownRate + dShenzhenIndexUpDownRate) / 2;

	setWeekLineBasicInfo.MoveFirst();
	int iCount = 0;
	int iBefore = 0;
	while (iCount < vIndex.size()) { // 只计算活跃股票的相对强度
		for (int i = 0; i < vIndex.at(iCount) - iBefore; i++) { // 根据索引去更改数据库,跨过不是A股的股票
			setWeekLineBasicInfo.MoveNext();
		}
		setWeekLineBasicInfo.Edit();
		const double dLastClose = atof(setWeekLineBasicInfo.m_LastClose);
		const double dClose = atof(setWeekLineBasicInfo.m_Close);
		double dUpDownRate = 0;
		// 计算指数相对强度
		if (dLastClose < 0.001) { // 新股上市等，昨日收盘价格为零
			dRSIndex = 50;
		}
		else {
			dUpDownRate = (dClose - dLastClose) / dLastClose;
			dRSIndex = (dUpDownRate - dIndexUpDownRate) * 500 + 50; // 以大盘涨跌为基准（50）。
		}
		setWeekLineBasicInfo.m_RSIndex = ConvertValueToString(dRSIndex);

		// 计算涨跌排名相对强度
		if (dLastClose < 0.001) {
			setWeekLineBasicInfo.m_RS = ConvertValueToString(50); // 新股上市或者除权除息，不计算此股
		}
		else {
			setWeekLineBasicInfo.m_RS = ConvertValueToString((static_cast<double>(iCount) * 100) / iTotalAShare);
		}
		setWeekLineBasicInfo.Update();
		iBefore = vIndex.at(iCount++);
		setWeekLineBasicInfo.MoveNext(); // 移到下一个数据。
		iBefore++; // 计数器也同时加一。
	}
	setWeekLineBasicInfo.m_pDatabase->CommitTrans();
	setWeekLineBasicInfo.Close();

	vStock.clear();
	vIndex.clear();
	vRS.clear();

	CString strDate2 = GetStringOfDate(lDate);
	CString strTemp;
	strTemp = strDate2 + _T("的股票周线相对强度计算完成");
	gl_systemMessage.PushDayLineInfoMessage(strTemp);    // 采用同步机制报告信息

	return(true);
}

double CChinaMarket::GetUpDownRate(CString strClose, CString strLastClose) noexcept {
	const double lastClose = atof(strLastClose);
	if (lastClose < 0.001) return 0;
	double result = (atof(strClose) - lastClose) / lastClose;
	if ((result > 0.11) || (result < -0.11)) result = 0;
	return result;
}

bool CChinaMarket::UpdateStockCodeDB(void) {
	CChinaStockPtr pStock = nullptr;
	CSetStockCode setStockCode;
	int iUpdatedStock = 0;
	int iCount = 0;

	//更新原有的代码集状态
	if (IsUpdateStockCodeDB()) {
		for (auto& pStock2 : m_vChinaMarketStock) {
			if (pStock2->IsUpdateProfileDB()) iUpdatedStock++;
		}
		setStockCode.m_strSort = _T("[Symbol]");
		setStockCode.Open();
		setStockCode.m_pDatabase->BeginTrans();
		while (iCount < iUpdatedStock) {
			if (setStockCode.IsEOF()) break;
			pStock = m_vChinaMarketStock.at(m_mapChinaMarketStock.at(setStockCode.m_Symbol));
			if (pStock->IsUpdateProfileDBAndClearFlag()) {
				//ASSERT(!pStock3->IsTodayNewStock());
				iCount++;
				pStock->UpdateStockCodeDB(setStockCode);
			}
			setStockCode.MoveNext();
		}
		if (iCount < iUpdatedStock) {
			for (auto& pStock3 : m_vChinaMarketStock) {
				if (pStock3->IsUpdateProfileDBAndClearFlag()) {
					ASSERT(pStock3->IsTodayNewStock());
					iCount++;
					pStock3->AppendStockCodeDB(setStockCode);
					pStock3->SetTodayNewStock(false);
				}
				if (iCount >= iUpdatedStock) break;
			}
		}
		setStockCode.m_pDatabase->CommitTrans();
		setStockCode.Close();
		m_lLoadedStock = m_vChinaMarketStock.size();
	}
	ASSERT(iCount == iUpdatedStock);
	return true;
}

void CChinaMarket::LoadStockSection(void) {
	CSetStockSection setStockSection;

	setStockSection.Open();
	while (!setStockSection.IsEOF()) {
		if (!m_vStockSection.at(setStockSection.m_IndexNumber)->IsActive()) {
			m_vStockSection.at(setStockSection.m_IndexNumber)->SetActive(setStockSection.m_Active);
			m_vStockSection.at(setStockSection.m_IndexNumber)->SetMarket(setStockSection.m_Market);
			m_vStockSection.at(setStockSection.m_IndexNumber)->SetIndexNumber(setStockSection.m_IndexNumber);
			m_vStockSection.at(setStockSection.m_IndexNumber)->SetComment(setStockSection.m_Comment);
		}
		setStockSection.MoveNext();
	}
	setStockSection.Close();
}

void CChinaMarket::LoadStockCodeDB(void) {
	CSetStockCode setStockCode;
	char buffer[30]{ 0, 0, 0 };
	CString str;

	setStockCode.m_strSort = _T("[Symbol]");
	setStockCode.Open();
	// 装入股票代码数据库
	while (!setStockCode.IsEOF()) {
		CChinaStockPtr pStock = make_shared<CChinaStock>();
		if (!IsStock(setStockCode.m_Symbol)) {
			pStock->LoadStockCodeDB(setStockCode);
			m_vChinaMarketStock.push_back(pStock);
		}
		else {
			str = _T("发现重复代码：");
			str += setStockCode.m_Symbol;
			gl_systemMessage.PushInnerSystemInformationMessage(str);
			setStockCode.Delete(); // 删除此重复代码
		}
		setStockCode.MoveNext();
	}
	if (IsDayLineNeedUpdate()) {
		const int i = GetDayLineNeedUpdateNumber();
		m_lStockDayLineNeedUpdate = i;
		if (GetDayOfWeek() == 1) gl_systemMessage.PushInformationMessage(_T("每星期一复查退市股票日线"));
		TRACE("尚余%d个股票需要检查日线数据\n", i);
		_itoa_s(i, buffer, 10);
		str = buffer;
		str += _T("个股票需要检查日线数据");
		gl_systemMessage.PushInformationMessage(str);
	}
	setStockCode.Close();
	m_lLoadedStock = m_vChinaMarketStock.size();
	SortStockVector();
}

///////////////////////////////////////////////////////////////////////////////////
//
// 更新选项数据库
//
//
//
//////////////////////////////////////////////////////////////////////////////////
bool CChinaMarket::UpdateOptionDB(void) {
	CSetOption setOption;

	setOption.Open();
	setOption.m_pDatabase->BeginTrans();
	if (setOption.IsEOF()) {
		setOption.AddNew();
		setOption.m_RSEndDate = GetRSEndDate();
		setOption.m_RSStartDate = GetRSStartDate();
		setOption.m_LastLoginDate = GetFormatedMarketDate();
		setOption.m_UpdatedDateFor10DaysRS1 = GetUpdatedDateFor10DaysRS1();
		setOption.m_UpdatedDateFor10DaysRS2 = GetUpdatedDateFor10DaysRS2();
		setOption.m_UpdatedDateFor10DaysRS = GetUpdatedDateFor10DaysRS();
		setOption.Update();
	}
	else {
		setOption.Edit();
		setOption.m_RSEndDate = GetRSEndDate();
		setOption.m_RSStartDate = GetRSStartDate();
		setOption.m_LastLoginDate = GetFormatedMarketDate();
		setOption.m_UpdatedDateFor10DaysRS1 = GetUpdatedDateFor10DaysRS1();
		setOption.m_UpdatedDateFor10DaysRS2 = GetUpdatedDateFor10DaysRS2();
		setOption.m_UpdatedDateFor10DaysRS = GetUpdatedDateFor10DaysRS();
		setOption.Update();
	}
	setOption.m_pDatabase->CommitTrans();
	setOption.Close();
	return true;
}

void CChinaMarket::LoadOptionDB(void) {
	CSetOption setOption;
	setOption.Open();
	if (setOption.IsEOF()) {
		SetRSStartDate(__CHINA_MARKET_BEGIN_DATE__);
		SetRSEndDate(__CHINA_MARKET_BEGIN_DATE__);
		SetLastLoginDate(__CHINA_MARKET_BEGIN_DATE__);
		SetUpdatedDateFor10DaysRS1(__CHINA_MARKET_BEGIN_DATE__);
		SetUpdatedDateFor10DaysRS2(__CHINA_MARKET_BEGIN_DATE__);
	}
	else {
		if (setOption.m_RSEndDate == 0) {
			SetRSEndDate(__CHINA_MARKET_BEGIN_DATE__);
		}
		else {
			SetRSEndDate(setOption.m_RSEndDate);
			if (GetRSEndDate() > __CHINA_MARKET_BEGIN_DATE__) {
				// 当日线历史数据库中存在旧数据时，采用单线程模式存储新数据。使用多线程模式时，MySQL会出现互斥区Exception，估计是数据库重入时发生同步问题）。
				// 故而修补数据时同时只运行一个存储线程，其他都处于休眠状态。此种问题不会出现于生成所有日线数据时，故而新建日线数据时可以使用多线程（目前为4个）。
				gl_SaveOneStockDayLine.SetMaxCount(1);
			}
		}
		if (setOption.m_RSStartDate == 0) {
			SetRSStartDate(__CHINA_MARKET_BEGIN_DATE__);
		}
		else {
			SetRSStartDate(setOption.m_RSStartDate);
		}
		if (setOption.m_LastLoginDate == 0) {
			SetLastLoginDate(__CHINA_MARKET_BEGIN_DATE__);
		}
		else {
			SetLastLoginDate(setOption.m_LastLoginDate);
		}
		SetUpdatedDateFor10DaysRS1(setOption.m_UpdatedDateFor10DaysRS1);
		SetUpdatedDateFor10DaysRS2(setOption.m_UpdatedDateFor10DaysRS2);
		SetUpdatedDateFor10DaysRS(setOption.m_UpdatedDateFor10DaysRS);
		if (setOption.m_UpdatedDateFor10DaysRS1 < GetFormatedMarketDate())  m_fChoiced10RSStrong1StockSet = false;
		else m_fChoiced10RSStrong1StockSet = true;
		if (setOption.m_UpdatedDateFor10DaysRS2 < GetFormatedMarketDate())  m_fChoiced10RSStrong2StockSet = false;
		else m_fChoiced10RSStrong2StockSet = true;
		if (setOption.m_UpdatedDateFor10DaysRS < GetFormatedMarketDate())  m_fChoiced10RSStrongStockSet = false;
		else m_fChoiced10RSStrongStockSet = true;
	}

	setOption.Close();
}

void CChinaMarket::LoadOptionChinaStockMarketDB(void) {
	SetChinaMarketOption setOptionChinaStockMarket;

	setOptionChinaStockMarket.Open();
	if (!setOptionChinaStockMarket.IsEOF()) {
		m_iRTDataServer = setOptionChinaStockMarket.m_RTDataServerIndex;
		m_iRTDataInquiryTickNumber = setOptionChinaStockMarket.m_RTDataInquiryTime / 100 - 1;
	}
	else {
		m_iRTDataServer = 0; // 默认使用新浪实时数据服务器
		m_iRTDataInquiryTickNumber = 3; // 默认400毫秒查询一次
	}
}

bool CChinaMarket::UpdateOptionChinaMarketDB(void) {
	SetChinaMarketOption setOptionChinaStockMarket;

	setOptionChinaStockMarket.Open();
	setOptionChinaStockMarket.m_pDatabase->BeginTrans();
	if (setOptionChinaStockMarket.IsEOF()) {
		setOptionChinaStockMarket.AddNew();
		setOptionChinaStockMarket.m_RTDataServerIndex = m_iRTDataServer;
		setOptionChinaStockMarket.m_RTDataInquiryTime = (m_iRTDataInquiryTickNumber + 1) * 100;
		setOptionChinaStockMarket.Update();
	}
	else {
		setOptionChinaStockMarket.Edit();
		setOptionChinaStockMarket.m_RTDataServerIndex = m_iRTDataServer;
		setOptionChinaStockMarket.m_RTDataInquiryTime = (m_iRTDataInquiryTickNumber + 1) * 100;
		setOptionChinaStockMarket.Update();
	}
	setOptionChinaStockMarket.m_pDatabase->CommitTrans();
	setOptionChinaStockMarket.Close();

	return true;
}

bool CChinaMarket::UpdateChoicedStockDB(void) {
	CSetChoicedStock setChoicedStock;

	setChoicedStock.Open();
	setChoicedStock.m_pDatabase->BeginTrans();
	while (!setChoicedStock.IsEOF()) {
		setChoicedStock.Delete();
		setChoicedStock.MoveNext();
	}
	setChoicedStock.m_pDatabase->CommitTrans();
	setChoicedStock.m_pDatabase->BeginTrans();
	for (auto& pStock : m_avChoicedStock.at(0)) {
		ASSERT(pStock->IsChoiced());
		setChoicedStock.AddNew();
		setChoicedStock.m_Symbol = pStock->GetSymbol();
		setChoicedStock.Update();
		pStock->SetSaveToChoicedStockDB(true);
	}
	setChoicedStock.m_pDatabase->CommitTrans();
	setChoicedStock.Close();

	return true;
}

bool CChinaMarket::AppendChoicedStockDB(void) {
	CSetChoicedStock setChoicedStock;

	setChoicedStock.Open();
	setChoicedStock.m_pDatabase->BeginTrans();
	for (auto& pStock : m_avChoicedStock.at(0)) {
		ASSERT(pStock->IsChoiced());
		if (!pStock->IsSaveToChoicedStockDB()) {
			setChoicedStock.AddNew();
			setChoicedStock.m_Symbol = pStock->GetSymbol();
			setChoicedStock.Update();
			pStock->SetSaveToChoicedStockDB(true);
		}
	}
	setChoicedStock.m_pDatabase->CommitTrans();
	setChoicedStock.Close();

	return true;
}

void CChinaMarket::LoadChoicedStockDB(void) {
	CSetChoicedStock setChoicedStock;

	setChoicedStock.Open();
	// 装入股票代码数据库
	while (!setChoicedStock.IsEOF()) {
		CChinaStockPtr pStock = GetStock(setChoicedStock.m_Symbol);
		if (pStock != nullptr) {
			auto it = find(m_avChoicedStock.at(0).cbegin(), m_avChoicedStock.at(0).cend(), pStock);
			if (it == m_avChoicedStock.at(0).end()) {
				m_avChoicedStock.at(0).push_back(pStock);
			}
			pStock->SetChoiced(true);
			pStock->SetSaveToChoicedStockDB(true);
		}
		setChoicedStock.MoveNext();
	}
	setChoicedStock.Close();
}

bool CChinaMarket::UpdateTempRTData(void) {
	if (!gl_ThreadStatus.IsSavingTempData()) {
		gl_ThreadStatus.SetSavingTempData(true);
		CreatingThreadSaveTempRTData();
		return true;
	}
	return false;
}