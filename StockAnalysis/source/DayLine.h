#pragma once

#include"Accessory.h"

#include"SetDayLineBasicInfo.h"
#include"SetDayLineExtendInfo.h"
#include"SetWorldStockDayLine.h"
#include"SetForexDayLine.h"

#include"ChinaStockHistoryData.h"

#include<gsl/gsl>
using namespace gsl;

using namespace std;
#include<memory>

class CDayLine;
typedef shared_ptr<CDayLine> CDayLinePtr;

class CDayLine : public CChinaStockHistoryData {
public:
  CDayLine();
  void Reset(void);       // 这些实现类需要采用这种方法重置内部状态，因为系统会一直运行，每天都需要重置状态。

  bool SaveChinaMarketData(not_null<CSetDayLineBasicInfo*> psetDayLineBasicInfo);
  bool AppendChinaMarketData(not_null<CSetDayLineBasicInfo*> psetDayLineBasicInfo);
  bool LoadChinaMarketBasicData(not_null<CSetDayLineBasicInfo*> psetDayLineBasicInfo);
  bool LoadChinaMarketExtendData(not_null<CSetDayLineExtendInfo*> psetDayLineExtendInfo);
  bool SaveWorldMarketData(not_null<CSetWorldStockDayLine*> pSetWorldStockDayLine);
  bool AppendWorldMarketData(not_null<CSetWorldStockDayLine*> pSetWorldStockDayLine);
  bool LoadWorldMarketData(not_null<CSetWorldStockDayLine*> pSetWorldStockDayLine);
  bool SaveForexDayLine(not_null<CSetForexDayLine*> pSetForexDayLine);
  bool AppendForexDayLine(not_null<CSetForexDayLine*> pSetForexDayLine);
  bool LoadForexDayLine(not_null<CSetForexDayLine*> pSetForexDayLine);

  bool ProcessNeteaseData(CString strStockCode, vector<char>& pBuffer, INT64& lCurrentPos);
  bool IsActive(void) const;

public:
  void Set3DaysRS(double dValue) noexcept { Set3RS(dValue); }
  double Get3DaysRS(void) const noexcept { return Get3RS(); }
  void Set5DaysRS(double dValue) noexcept { Set5RS(dValue); }
  double Get5DaysRS(void) const noexcept { return Get5RS(); }
  void Set10DaysRS(double dValue) noexcept { Set10RS(dValue); }
  double Get10DaysRS(void) const noexcept { return Get10RS(); }
  void Set30DaysRS(double dValue) noexcept { Set30RS(dValue); }
  double Get30DaysRS(void) const noexcept { return Get30RS(); }
  void Set60DaysRS(double dValue) noexcept { Set60RS(dValue); }
  double Get60DaysRS(void) const noexcept { return Get60RS(); }
  void Set120DaysRS(double dValue) noexcept { Set120RS(dValue); }
  double Get120DaysRS(void) const noexcept { return Get120RS(); }

private:
};
