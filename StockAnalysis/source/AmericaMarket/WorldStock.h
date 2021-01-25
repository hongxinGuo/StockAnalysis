#pragma once

#include"stdafx.h"
#include"globeSymbolDef.h"

#include"DayLine.h"
#include"EPSSurprise.h"

#include"SetWorldStock.h"

using namespace std;
#include<memory>
#include<atomic>
#include<vector>

class CWorldStock : public CObject {
public:
  CWorldStock();
  void Reset(void);
public:
  void Load(CSetWorldStock& setWorldStock);
  bool CheckDayLineUpdateStatus(long lTodayDate, long lLastTradeDate, long lTime, long lDayOfWeek);
  void Save(CSetWorldStock& setWorldStock);
  void Update(CSetWorldStock& setWorldStock);
  void Append(CSetWorldStock& setWorldStock);
  void SaveDayLine(void);
  bool UpdateEPSSurpriseDB(void);

  void UpdateDayLine(vector<CDayLinePtr>& vDayLine);
  void UpdateEPSSurprise(vector<CEPSSurprisePtr>& vEPSSurprise);

  void UpdateSymbolForSort(void);

  long GetDayLineStartDate(void) noexcept { return m_lDayLineStartDate; }
  void SetDayLineStartDate(long lDate) noexcept { m_lDayLineStartDate = lDate; }
  long GetDayLineEndDate(void) noexcept { return m_lDayLineEndDate; }
  void SetDayLineEndDate(long lDate) noexcept { m_lDayLineEndDate = lDate; }
  long GetLastRTDataUpdateDate(void) noexcept { return m_lLastRTDataUpdateDate; }
  void SetLastRTDataUpdateDate(long lDate) noexcept { m_lLastRTDataUpdateDate = lDate; }
  long GetPeerUpdateDate(void) noexcept { return m_lPeerUpdateDate; }
  void SetPeerUpdateDate(long lDate) noexcept { m_lPeerUpdateDate = lDate; }
  void SetLastEPSSurpriseUpdateDate(long lDate) noexcept { m_lLastEPSSurpriseUpdateDate = lDate; }
  void UpdateDayLineStartEndDate(void);
  long GetDayLineSize(void) noexcept { return m_vDayLine.size(); }
  CDayLinePtr GetDayLine(long lIndex) { return m_vDayLine.at(lIndex); }
  void UnloadDayLine(void) { m_vDayLine.resize(0); }

  bool IsDayLineNeedUpdate(void) noexcept { return m_fDayLineNeedUpdate; }
  void SetDayLineNeedUpdate(bool fFlag) noexcept { m_fDayLineNeedUpdate = fFlag; }
  bool IsDayLineNeedSaving(void) noexcept { return m_fDayLineNeedSaving; }
  void SetDayLineNeedSaving(bool fFlag) noexcept { m_fDayLineNeedSaving = fFlag; }
  bool IsDayLineNeedSavingAndClearFlag(void);
  bool HaveNewDayLineData(void);

  long GetIPOStatus(void) noexcept { return m_lIPOStatus; }
  void SetIPOStatus(long lValue) noexcept { m_lIPOStatus = lValue; }
  bool IsDelisted(void) noexcept { return (m_lIPOStatus == __STAKE_DELISTED__); }
  bool IsNullStock(void) noexcept { return (m_lIPOStatus == __STAKE_NULL__); }
  bool IsIPOed(void) noexcept { return(m_lIPOStatus == __STAKE_IPOED__); }
  bool IsNotChecked(void) noexcept { return(m_lIPOStatus == __STAKE_NOT_CHECKED__); }

  bool IsEPSSurpriseNeedUpdate(void) noexcept { return m_fEPSSurpriseNeedUpdate; }
  void SetEPSSurpriseNeedUpdate(bool fFlag) noexcept { m_fEPSSurpriseNeedUpdate = fFlag; }
  bool CheckEPSSurpriseStatus(long lCurrentDate);
  bool IsEPSSurpriseNeedSave(void) noexcept { return m_fEPSSurpriseNeedSave; }
  void SetEPSSurpriseNeedSave(bool fFlag) noexcept { m_fEPSSurpriseNeedSave = fFlag; }
  bool IsEPSSurpriseNeedSaveAndClearFlag(void);

  bool CheckPeerStatus(long lCurrentDate);

  CString GetSymbol(void) { return m_strSymbol; }
  CString GetTicker(void) { return m_strTicker; }

  CString GetFinnhubDayLineInquiryString(time_t tCurrentTime);
  CString GetTiingoDayLineInquiryString(long lCurrentDate);

  // 实时数据区
  time_t GetTransactionTime(void) noexcept { return m_TransactionTime; }
  void SetTransactionTime(time_t time) noexcept { m_TransactionTime = time; }
  long GetLastClose(void) noexcept { return m_lLastClose; }
  void SetLastClose(long lValue) noexcept { m_lLastClose = lValue; }
  long GetOpen(void) noexcept { return m_lOpen; }
  void SetOpen(long lValue) noexcept { m_lOpen = lValue; }
  long GetHigh(void) noexcept { return m_lHigh; }
  void SetHigh(long lValue) noexcept { m_lHigh = lValue; }
  long GetLow(void) noexcept { return m_lLow; }
  void SetLow(long lValue) noexcept { m_lLow = lValue; }
  long GetNew(void) noexcept { return m_lNew; }
  void SetNew(long lValue) noexcept { m_lNew = lValue; }
  INT64 GetAmount(void) noexcept { return m_llAmount; }
  void SetAmount(INT64 llValue) noexcept { m_llAmount = llValue; }
  INT64 GetVolume(void) noexcept { return m_llVolume; }
  void SetVolume(INT64 llValue) noexcept { m_llVolume = llValue; }
  long GetUpDown(void) noexcept { return m_lUpDown; }
  void SetUpDown(long lValue) noexcept { m_lUpDown = lValue; }
  double GetUpDownRate(void) noexcept { return m_dUpDownRate; }
  void SetUpDownRate(double dValue) noexcept { m_dUpDownRate = dValue; }
  double GetChangeHandRate(void) noexcept { return m_dChangeHandRate; }
  void SetChangeHandRate(double dValue) noexcept { m_dChangeHandRate = dValue; }
  void SetTotalValue(INT64 llValue) noexcept { m_llTotalValue = llValue; }
  INT64 GetTotalValue(void) noexcept { return m_llTotalValue; }
  void SetCurrentValue(INT64 llValue) noexcept { m_llCurrentValue = llValue; }
  INT64 GetCurrentValue(void) noexcept { return m_llCurrentValue; }

  bool IsActive(void) noexcept { return m_fActive; }
  void SetActive(bool fFlag) noexcept { m_fActive = fFlag; }

public:
  // Finnhub symbol信息
  CString m_strDescription;
  CString m_strDisplaySymbol;
  CString m_strSymbol;
  CString m_strExchangeCode;
  CString m_strType;
  CString m_strMic;
  CString m_strFigi;
  CString m_strCurrency;

  // Finnhub profile信息
  CString m_strAddress;
  CString m_strCity;
  CString m_strCountry;
  CString m_strCusip;
  CString m_strSedol;
  long m_lEmployeeTotal;
  CString m_strListedExchange;
  CString m_strGgroup;
  CString m_strGind;
  CString m_strGsector;
  CString m_strGsubind;
  CString m_strIPODate;
  CString m_strIsin;
  double m_dMarketCapitalization;
  CString m_strNaics;
  CString m_strNaicsNationalIndustry;
  CString m_strNaicsSector;
  CString m_strNaicsSubsector;
  CString m_strName;
  CString m_strPhone;
  double m_dShareOutstanding;
  CString m_strState;
  CString m_strTicker;
  CString m_strWebURL;
  CString m_strLogo;
  CString m_strFinnhubIndustry;
  CString m_strPeer;
  long m_lProfileUpdateDate; // 最新简介更新日期
  long m_lDayLineStartDate;
  long m_lDayLineEndDate;
  long m_lLastRTDataUpdateDate; // 最新实时数据更新日期
  long m_lPeerUpdateDate; // 最新实时数据更新日期
  long m_lLastEPSSurpriseUpdateDate; // 最新EPS Surprise更新日期
  long m_lIPOStatus;

  // Tiingo Symbol信息
  CString m_strTiingoPermaTicker; // Tiingo永久代码标识
  bool m_fIsActive; //
  bool m_fIsADR;
  INT32 m_iSICCode;
  CString m_strSICIndustry;
  CString m_strSICSector;
  CString m_strTiingoIndustry;
  CString m_strTiingoSector;
  CString m_strCompanyWebSite;
  CString m_strSECFilingWebSite;
  long m_lStatementUpdateDate;
  long m_lDailyDataUpdateDate;

  // 系统生成信息
  CString m_strSymbolForSort; // 用于排序的符号。前面时交易所信息，后面是股票代码。

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

  bool m_fActive;	// 是否本日内有数据读入。由新浪实时行情处理函数和网易日线历史数据处理函数来设置。

  vector<CDayLinePtr> m_vDayLine;
  vector<CEPSSurprisePtr> m_vEPSSurprise;
  bool m_fEPSSurpriseNeedUpdate;
  atomic_bool m_fEPSSurpriseNeedSave;
  // 无需存储数据区
  bool m_fUpdateDatabase; // 要求更新此数据
  bool m_fInquiryStockProfile; // 要求更新公司简介
  bool m_fFinnhubPeerUpdated; // 同业公司数据已更新
  bool m_fDayLineNeedUpdate; // 日线需要更新。默认为真
  atomic_bool m_fDayLineNeedSaving; // 日线历史数据已处理，等待存储。

protected:
  // 无需存储数据区
};

typedef shared_ptr<CWorldStock> CWorldStockPtr;