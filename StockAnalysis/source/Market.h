#ifndef __MARKET_H__
#define __MERKET_H__

#include"stdafx.h"
#include"globedef.h"

#include "Stock.h"
#include"SetStockCode.h"

#include"CrweberIndex.h"

using namespace std;
#include<vector>
#include<map>
#include<atomic>

class CMarket final : public CObject
{
public:
  // 只能有一个实例
  CMarket(void);
  ~CMarket(void);
  void Reset(void);

#ifdef _DEBUG
  virtual	void AssertValid() const;
  virtual	void Dump(CDumpContext& dc) const;
#endif
public:
  // 定时更新，完成具体调度任务。由主线程的OnTimer函数调用。其后跟随各被调度函数
  bool SchedulingTask(void); // 由程序的定时器调度，大约每100毫秒一次
  bool SchedulingTaskPerSecond(long lSecondNumber); // 每秒调度一次
  bool SchedulingTaskPer10Seconds(long lSecondNumber, long lCurrentTime); // 每十秒调度一次
  bool SchedulingTaskPer1Minute(long lSecondNumber, long lCurrentTime); // 每一分钟调度一次
  bool SchedulingTaskPer5Minutes(long lSecondNumber, long lCurrentTime); // 每五分钟调度一次
  bool SchedulingTaskPerHour(long lSecondNumber, long lCurrentTime); // 每小时调度一次

  // 各种任务
  bool TaskGetRTDataFromWeb(void);
  void TaskGetNeteaseDayLineFromWeb(void);
  void TaskProcessDayLineGetFromNeeteaseServer();

  bool GetNeteaseDayLineWebData(void);
  void TaskLoadSavedTempData(void);

  bool TaskCheckMarketOpen(long lCurrentTime);
  bool TaskResetSystem(long lCurrentTime);
  bool TaskResetSystemAgain(long lCurrentTime);

  // interface function
public:
  // 系统状态区
  bool IsPermitResetSystem(void) noexcept { return m_fPermitResetSystem; }
  void SetPermitResetSystem(bool fFlag) noexcept { m_fPermitResetSystem = fFlag; }

  // 初始化市场

  // 实时数据读取
  int GetSinaInquiringStockStr(CString& str, long lTotalNumber, bool fSkipUnactiveStock = true);
  int GetTengxunInquiringStockStr(CString& str, long lTotalNumber, bool fSkipUnactiveStock = true);
  int	GetNeteaseInquiringStockStr(CString& str, long lTotalNumber = 900, bool fSkipUnactiveStock = true);
  int GetInquiringStr(CString& str, vector<CStockPtr>::iterator& itStock, CString strPostfix, long lTotalNumber = 900, bool fSkipUnactiveStock = true);
  bool StepToNextActiveStockIT(vector<CStockPtr>::iterator& itStock);
  void ResetIT(void); //重置各迭代器

  //日线历史数据读取
  bool CreateNeteaseDayLineInquiringStr(CString& str);

  bool IsAStock(CStockPtr pStock); // 是否为沪深A股
  bool IsAStock(CString strStockCode); // 是否为沪深A股
  bool IsStock(CString  strStockCode, CStockPtr& pStock);	// 是否为正确的股票代码

  CString GetStockName(CString strStockCode);

  // 得到股票索引
  bool GetStockIndex(CString strStockCode, long& lIndex);
  // 得到股票指针
  CStockPtr GetStockPtr(CString strStockCode);
  CStockPtr GetStockPtr(long lIndex);

  void IncreaseActiveStockNumber(void);

  // 得到当前显示股票
  CStockPtr GetShowStock(void) noexcept { return m_pCurrentStock; }
  void SetShowStock(CString strStockCode);
  void SetShowStock(CStockPtr pStock);
  bool IsCurrentStockChanged(void);

  long GetTotalActiveStock(void) noexcept { return m_lTotalActiveStock; }
  void SetTotalActiveStock(long lValue) noexcept { m_lTotalActiveStock = lValue; }

  long GetMinLineOffset(CStock sID, time_t Time);

  bool SystemReady(void) noexcept { return m_fSystemReady; }
  void SetSystemReady(bool fFlag) noexcept { m_fSystemReady = fFlag; }

  bool IsTodayStockCompiled(void) noexcept { return m_fTodayStockCompiled; }
  void SetTodayStockCompiledFlag(bool fFlag) noexcept { m_fTodayStockCompiled = fFlag; }

  // 数据库读取存储操作
  bool SaveRTData(void);  // 实时数据处理函数，将读取到的实时数据存入数据库中
  bool SaveDayLineData(void);  // 日线数据处理函数，将读取到的日线数据存入数据库中
  bool SaveCrweberIndexData(void);  // crweber.com油运指数存储函数
  bool UpdateStockCodeDB(void);
  void LoadStockCodeDB(void);
  bool UpdateOptionDB(void);
  void LoadOptionDB(void);
  bool UpdateTempRTData(void);
  bool UpdateTodayTempDB(void);
  bool LoadTodayTempDB(void);

  bool ClearAllDayLineVector(void);

  // 股票历史数据处理
  bool IsDayLineNeedUpdate(void);
  bool IsDayLineNeedSaving(void);
  // 是否所有股票的历史日线数据都查询过一遍了
  bool ProcessDayLineGetFromNeeteaseServer(void);

  long CompileCurrentTradeDayStock(void);
  bool CalculateOneDayRelativeStrong(long lDay);

  bool IsLoadSelectedStock(void) noexcept { return m_fLoadedSelectedStock; }
  void SetLoadSelectedStock(bool fLoad) noexcept { m_fLoadedSelectedStock = fLoad; }

  bool IsCheckTodayActiveStock(void) noexcept { return m_fCheckTodayActiveStock; }

  bool IsTodayTempRTDataLoaded(void) noexcept { return m_fTodayTempDataLoaded; }
  void SetTodayTempRTDataLoaded(bool fFlag) noexcept { m_fTodayTempDataLoaded = fFlag; }

  bool IsCalculatingRS(void) noexcept { return m_fCalculatingRS; }
  void SetCalculatingRS(bool fFlag) noexcept { m_fCalculatingRS = fFlag; }

  bool IsUpdateStockCodeDB(void) noexcept { return m_fUpdateStockCodeDB; }
  void SetUpdateStockCodeDB(bool fFlag) noexcept { m_fUpdateStockCodeDB = fFlag; }

  bool IsUsingNeteaseRTDataReceiverAsTester(void) { return m_fUsingNeteaseRTDataReceiverAsTester; }
  bool IsUsingTengxunRTDataReceiverAsTester(void) { return m_fUsingTengxunRTDataReceiverAsTester; }

  long GetRelativeStrongStartDay(void) noexcept { return m_lRelativeStrongStartDay; }
  void SetRelativeStrongStartDay(long lDay) noexcept { m_lRelativeStrongStartDay = lDay; }
  long GetRelativeStrongEndDay(void) noexcept { return m_lRelativeStrongEndDay; }
  void SetRelativeStrongEndDay(long lDay) noexcept { m_lRelativeStrongEndDay = lDay; }
  long GetLastLoginDay(void) noexcept { return m_lLastLoginDay; }
  void SetLastLoginDay(long lDay) noexcept { m_lLastLoginDay = lDay; }

  INT64 GetTotalAttackBuyAmount(void);
  INT64 GetTotalAttackSellAmount(void);

  size_t GetTotalStockMapIndexSize(void) noexcept { return m_mapChinaMarketAStock.size(); }
  long GetTotalStockIndex(CString str) { return m_mapChinaMarketAStock.at(str); }

  void SetReadingSinaRTDataTime(clock_t tt) { m_ReadingSinaRTDataTime = tt; }
  clock_t GetReadingSinaRTDataTime(void) { return m_ReadingSinaRTDataTime; }
  void SetReadingTengxunRTDataTime(clock_t tt) { m_ReadingTengxunRTDataTime = tt; }
  clock_t GetReadingTengxunRTDataTime(void) { return m_ReadingTengxunRTDataTime; }
  void SetReadingNeteaseDayDataTime(clock_t tt) { m_ReadingNeteaseDayDataTime = tt; }
  clock_t GetReadingNeteaseDayDataTime(void) { return m_ReadingNeteaseDayDataTime; }

  //处理个股票的实时数据，计算挂单变化等。由工作线程ThreadCalculatingRTDataProc调用。
  bool ProcessRTData(void);

  // 处理腾讯实时数据
  bool ProcessTengxunRTData(void);

  //处理实时股票变化等
  bool TaskDistributeSinaRTDataToProperStock(void);

  void ResetSystemFlagAtMidnight(long lCurrentTime);
  void SaveTempDataIntoDB(long lCurrentTime);

  // 测试用函数
  bool __Test_IsPermitResetSystem(void) { return m_fPermitResetSystem; }
  void __Test_SetResetSystem(bool fFlag) { gl_fResetSystem = fFlag; }

private:
  // 初始化
  bool CreateTotalStockContainer(void); // 此函数是构造函数的一部分，不允许单独调用。

public:

  CStockPtr m_pCurrentStock; // 当前显示的股票
  char m_aStockCodeTemp[30];
  bool m_fCurrentEditStockChanged;
  bool m_fMarketOpened; // 是否开市
  bool m_fGetRTStockData; // 读取实时数据标识
  bool m_fReadingTengxunRTData; // 读取腾讯实时行情
  bool m_fSaveDayLine; // 将读取的日线存入数据库标识
  int m_iCountDownDayLine; // 日线数据读取延时计数。
  int m_iCountDownSlowReadingRTData; // 慢速读取实时数据计数器

  bool m_fUsingSinaRTDataReceiver; // 使用新浪实时数据提取
  bool m_fUsingNeteaseRTDataReceiver; // 使用网易实时数据提取器
  bool m_fUsingTengxunRTDataReceiver; // 使用腾讯实时数据提取器
  bool m_fUsingNeteaseRTDataReceiverAsTester;
  bool m_fUsingTengxunRTDataReceiverAsTester;

  vector<CStockPtr> m_vChinaMarketAStock; // 本系统允许的所有股票池（无论代码是否存在）

  vector<CStockPtr> gl_vStockChoice; // 自选股票池

  vector<CrweberIndexPtr> gl_vCrweberIndex; // crweber.com网站上的油运指数

  time_t m_ttNewestTransactionTime;

protected:

  map<CString, long> m_mapChinaMarketAStock; // 将所有被查询的股票代码映射为偏移量（目前只接受A股信息）

  // Option各选项
  long m_lRelativeStrongStartDay;
  long m_lRelativeStrongEndDay;
  long m_lLastLoginDay; // 上次登录日期。如果此日期为昨日的话，则无需下载日线历史数据

  long m_lTotalActiveStock;	// 当天股票总数

  vector<CStockPtr>::iterator m_itSinaStock; // 新浪实时股票代码查询迭代器
  vector<CStockPtr>::iterator m_itTengxunStock; // 腾讯实时股票代码迭代器
  vector<CStockPtr>::iterator m_itNeteaseStock; // 网易实时股票代码迭代器

  vector<CStockPtr> m_vpSelectedStock; // 当前选择的股票
  bool m_fLoadedSelectedStock;

  bool m_fCurrentStockChanged; // 当前选择的股票改变了
  INT64 m_lTotalMarketBuy; // 沪深市场中的A股向上买入金额
  INT64 m_lTotalMarketSell; // 沪深市场中的A股向下卖出金额

  bool m_fCalculatingRS;

  // 系统状态区
  bool m_fPermitResetSystem; // 允许重置系统（如果不断机多日运行的话，需要每日重置系统
  bool m_fSystemReady; // 市场初始态已经设置好
  bool m_fTodayStockCompiled; // 今日是否执行了股票收盘
  bool m_fUpdatedStockCodeDataBase; //是否更新了日线历史数据库标识
  bool m_fCheckTodayActiveStock; // 是否查询今日活跃股票代码
  bool m_fTodayTempDataLoaded; //今日暂存的临时数据是否加载标识。

  // 多线程读取之变量
  atomic<clock_t> m_ReadingSinaRTDataTime; // 每次读取新浪实时数据的时间
  atomic<clock_t> m_ReadingTengxunRTDataTime; // 每次读取腾讯实时数据的时间
  atomic<clock_t> m_ReadingNeteaseDayDataTime;    // 每次读取网易日线历史数据的时间

  bool m_fUpdateStockCodeDB;

public:
  // 网易日线历史数据读取处理和存储计数器。
  atomic_int m_iDayLineNeedUpdate; // 日线需要更新的股票数量
  atomic_int m_iDayLineNeedProcess; // 日线需要处理的股票数量
  atomic_int m_iDayLineNeedSave; // 日线需要存储的股票数量

private:
};

#endif
