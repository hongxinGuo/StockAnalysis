#pragma once

#include"globeSymbolDef.h"

using namespace std;
#include<memory>
#include<atomic>
#include<vector>

class CVirtualStock : public CObject {
public:
  CVirtualStock();
  virtual ~CVirtualStock();
  virtual void Reset(void);
  virtual int GetRatio(void)  const = 0;

public:
  CString GetExchangeCode(void) const { return m_strExchangeCode; }
  void SetExchangeCode(CString strExchangeCode) { m_strExchangeCode = strExchangeCode; }
  CString GetSymbol(void) const { return m_strSymbol; }
  void SetSymbol(CString str) { m_strSymbol = str; }

  // 基本实时数据，需要更新
  time_t GetTransactionTime(void) const noexcept { return m_TransactionTime; }
  void SetTransactionTime(time_t time) noexcept { m_TransactionTime = time; }
  long GetLastClose(void) const noexcept { return m_lLastClose; }
  void SetLastClose(long lValue) noexcept { m_lLastClose = lValue; }
  long GetOpen(void) const noexcept { return m_lOpen; }
  void SetOpen(long lValue) noexcept { m_lOpen = lValue; }
  long GetHigh(void) const noexcept { return m_lHigh; }
  void SetHigh(long lValue) noexcept { m_lHigh = lValue; }
  long GetLow(void) const noexcept { return m_lLow; }
  void SetLow(long lValue) noexcept { m_lLow = lValue; }
  long GetNew(void) const noexcept { return m_lNew; }
  void SetNew(long lValue) noexcept { m_lNew = lValue; }
  INT64 GetAmount(void) const  noexcept { return m_llAmount; }
  void SetAmount(INT64 llValue) noexcept { m_llAmount = llValue; }
  INT64 GetVolume(void) const noexcept { return m_llVolume; }
  void SetVolume(INT64 llValue) noexcept { m_llVolume = llValue; }
  long GetUpDown(void) const noexcept { return m_lUpDown; }
  void SetUpDown(long lValue) noexcept { m_lUpDown = lValue; }
  double GetUpDownRate(void) const noexcept { return m_dUpDownRate; }
  void SetUpDownRate(double dValue) noexcept { m_dUpDownRate = dValue; }
  double GetChangeHandRate(void) const noexcept { return m_dChangeHandRate; }
  void SetChangeHandRate(double dValue) noexcept { m_dChangeHandRate = dValue; }
  void SetTotalValue(INT64 llValue) noexcept { m_llTotalValue = llValue; }
  INT64 GetTotalValue(void) const noexcept { return m_llTotalValue; }
  void SetCurrentValue(INT64 llValue) noexcept { m_llCurrentValue = llValue; }
  INT64 GetCurrentValue(void) const noexcept { return m_llCurrentValue; }

  //
  bool IsTodayNewStock(void) const noexcept { return m_fTodayNewStock; }
  void SetTodayNewStock(bool fFlag) noexcept { m_fTodayNewStock = fFlag; }
  bool IsUpdateProfileDB(void) const noexcept { return m_fUpdateProfileDB; }
  bool IsUpdateProfileDBAndClearFlag(void) noexcept { const bool fNeedSaving = m_fUpdateProfileDB.exchange(false); return fNeedSaving; }
  void SetUpdateProfileDB(bool fFlag) noexcept { m_fUpdateProfileDB = fFlag; }
  bool IsClearDayLineVector(void) const noexcept { return m_fClearDayLineVector; }
  bool IsClearDayLineVectorAndClearFlag(void) noexcept { const bool fNeedSaving = m_fClearDayLineVector.exchange(false); return fNeedSaving; }
  void SetClearDayLineVector(bool fFlag) noexcept { m_fClearDayLineVector = fFlag; }
  bool IsActive(void) const noexcept { return m_fActive; }
  void SetActive(bool fFlag) noexcept { m_fActive = fFlag; }

  long GetDayLineStartDate(void) const noexcept { return m_lDayLineStartDate; }
  void SetDayLineStartDate(long lDate) noexcept { m_lDayLineStartDate = lDate; }
  long GetDayLineEndDate(void) const noexcept { return m_lDayLineEndDate; }
  void SetDayLineEndDate(long lDate) noexcept { m_lDayLineEndDate = lDate; }

  long GetIPOStatus(void) const noexcept { return m_lIPOStatus; }
  void SetIPOStatus(long lValue) noexcept { m_lIPOStatus = lValue; }
  bool IsDelisted(void) const noexcept { return (m_lIPOStatus == __STOCK_DELISTED__); }
  bool IsNullStock(void) const noexcept { return (m_lIPOStatus == __STOCK_NULL__); }
  bool IsIPOed(void) const noexcept { return(m_lIPOStatus == __STOCK_IPOED__); }
  bool IsNotChecked(void) const noexcept { return(m_lIPOStatus == __STOCK_NOT_CHECKED__); }

  // 由于处理日线历史数据的函数位于不同的线程中，故而需要同步机制设置标识
  bool IsDayLineNeedUpdate(void) const noexcept { return m_fDayLineNeedUpdate; }
  void SetDayLineNeedUpdate(bool fFlag) noexcept { m_fDayLineNeedUpdate = fFlag; }
  bool IsDayLineNeedProcess(void) const  noexcept { return m_fDayLineNeedProcess; }
  void SetDayLineNeedProcess(bool fFlag) noexcept { m_fDayLineNeedProcess = fFlag; }
  bool IsDayLineNeedSaving(void) const noexcept { return m_fDayLineNeedSaving; }
  void SetDayLineNeedSaving(bool fFlag) noexcept { m_fDayLineNeedSaving = fFlag; }
  bool IsDayLineNeedSavingAndClearFlag(void) noexcept { const bool fNeedSaving = m_fDayLineNeedSaving.exchange(false); return fNeedSaving; }

protected:
  CString m_strExchangeCode; // 证券所属交易所。美国为US，上海为SS，深圳为SZ；外汇为forex等。
  CString m_strSymbol; // 股票代码。十位以内，前六位为数字代码，后两位为市场前缀。如600601.SS，000001.SZ

  // 实时数据区
  time_t m_TransactionTime; // 实时数据交易时间
  long m_lLastClose; // 以0.001元计的收盘价
  long m_lOpen; // 以0.001元计的开盘价
  long m_lHigh; // 以0.001元计的最高价
  long m_lLow; // 以0.001元计的最低价
  long m_lNew; // 以0.001元计的最新价
  long m_lUpDown; // 涨跌值
  double m_dUpDownRate; // 涨跌率
  INT64 m_llVolume;	// 以1股计的成交量
  INT64 m_llAmount; // 以元计的成交金额
  double m_dChangeHandRate; // 换手率
  INT64 m_llTotalValue;	// 总市值。单位：万元
  INT64 m_llCurrentValue;	// 流通市值。单位：万元

  bool m_fTodayNewStock; // 本日新发现的股票
  atomic_bool m_fUpdateProfileDB;
  atomic_bool m_fClearDayLineVector;
  bool m_fActive;	// 是否本日内有数据读入。由新浪实时行情处理函数和网易日线历史数据处理函数来设置。
  long m_lDayLineStartDate;	// 日线数据起始日。这个是处理日线历史数据时得到的起始交易日，
  long m_lDayLineEndDate;	// 日线数据更新日。这个是处理日线历史数据时得到的最新日，
  long m_lIPOStatus; // 通过网易历史日线查询，如果只有前缀信息而没有实际内容，可以确认没有实际交易。在这种情况下，新浪实时行情有数据，只是为零而已。默认情况下为已上市
               // 未上市（无效股票代码）为__STOCK_NULL__；正常为__STOCK_IPOED__；已通过IPO但尚未上市或退市为__STOCK_DELISTED；其他情况尚未出现，留待以后处理。

  atomic_bool m_fDayLineNeedUpdate; // 日线需要更新。默认为真
  atomic_bool m_fDayLineNeedProcess; // 已从网络上读取了日线历史数据，等待处理
  atomic_bool m_fDayLineNeedSaving; // 日线历史数据已处理，等待存储。
};

typedef shared_ptr<CVirtualStock> CVirtualStockPtr;
