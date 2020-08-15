#pragma once

#include"stdafx.h"
#include"pch.h"

#include"ChinaStock.h"
#include"ChinaMarket.h"

namespace testing {
  class CMockChinaMarket : public CChinaMarket {
  public:
    MOCK_METHOD(bool, RunningThreadSaveChoicedRTData, (), (override));
    MOCK_METHOD(bool, RunningThreadProcessTodayStock, (), (override));
    MOCK_METHOD(bool, RunningThreadBuildDayLineRS, (long lStartCalculatingDay), (override));
    MOCK_METHOD(bool, RunningThreadBuildDayLineRSOfDay, (long lThisDay), (override));
    MOCK_METHOD(bool, RunningThreadSaveTempRTData, (), (override));
    MOCK_METHOD(bool, RunningThreadSaveDayLineBasicInfoOfStock, (CChinaStockPtr pStock), (override));
    MOCK_METHOD(bool, RunningThreadLoadDayLine, (CChinaStockPtr pCurrentStock), (override));
    MOCK_METHOD(bool, RunningThreadUpdateStockCodeDB, (), (override));
    MOCK_METHOD(bool, RunningThreadUpdateOptionDB, (), (override));
    MOCK_METHOD(bool, RunningThreadAppendChoicedStockDB, (), (override));
    MOCK_METHOD(bool, RunningThreadChoice10RSStrong2StockSet, (), (override));
    MOCK_METHOD(bool, RunningThreadChoice10RSStrong1StockSet, (), (override));
    MOCK_METHOD(bool, RunningThreadChoice10RSStrongStockSet, (), (override));
    MOCK_METHOD(bool, RunningThreadCalculate10RSStrongStock, (vector<CChinaStockPtr>* pv10RSStrongStock, CRSReference* pRef, CChinaStockPtr pStock), (override));
    MOCK_METHOD(bool, RunningThreadCalculate10RSStrong1Stock, (vector<CChinaStockPtr>* pv10RSStrongStock, CChinaStockPtr pStock), (override));
    MOCK_METHOD(bool, RunningThreadCalculate10RSStrong2Stock, (vector<CChinaStockPtr>* pv10RSStrongStock, CChinaStockPtr pStock), (override));
    MOCK_METHOD(bool, RunningThreadBuildWeekLine, (long lStartDay), (override));
    MOCK_METHOD(bool, RunningThreadBuildWeekLineOfStock, (CChinaStockPtr pStock, long lStartDay), (override));
    MOCK_METHOD(bool, RunningThreadBuildWeekLineRS, (), (override));
    MOCK_METHOD(bool, RunningThreadBuildWeekLineRSOfDay, (long lThisDay), (override));
    MOCK_METHOD(bool, RunningThreadBuildWeekLineOfCurrentWeek, (), (override));

    MOCK_METHOD(bool, UpdateOptionDB, (), (override));
    MOCK_METHOD(bool, UpdateStockCodeDB, (), (override));
    MOCK_METHOD(bool, AppendChoicedStockDB, (), (override));
    MOCK_METHOD(bool, UpdateTodayTempDB, (), (override));
    MOCK_METHOD(bool, SaveRTData, (), (override));
    MOCK_METHOD(long, ProcessCurrentTradeDayStock, (long lCurrentTradeDay), (override));
    MOCK_METHOD(bool, Choice10RSStrong2StockSet, (), (override));
    MOCK_METHOD(bool, Choice10RSStrong1StockSet, (), (override));
    MOCK_METHOD(bool, Choice10RSStrongStockSet, (CRSReference* pRef, int iIndex), (override));
    MOCK_METHOD(bool, BuildDayLineRSOfDay, (long lDay), (override));
    MOCK_METHOD(bool, BuildWeekLineOfCurrentWeek, (), (override));
  };

  typedef shared_ptr<CMockChinaMarket> CMockChinaMarketPtr;
}
