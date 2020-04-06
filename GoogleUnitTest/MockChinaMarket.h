#pragma once

#include"stdafx.h"
#include"pch.h"

#include"ChinaStock.h"
#include"ChinaMarket.h"

namespace Testing {
  class CMockChinaMarket : public CChinaMarket {
  public:
    MOCK_METHOD(bool, RunningThreadSaveChoicedRTData, (), (override));
    MOCK_METHOD(bool, RunningThreadProcessTodayStock, (), (override));
    MOCK_METHOD(bool, RunningThreadCalculateRelativeStrong, (long lStartCalculatingDay), (override));
    MOCK_METHOD(bool, RunningThreadCalculateThisDayRS, (long lThisDay), (override));
    MOCK_METHOD(bool, RunningThreadSaveTempRTData, (), (override));
    MOCK_METHOD(bool, RunningThreadSaveDayLineOfOneStock, (CChinaStockPtr pStock), (override));
    MOCK_METHOD(bool, RunningThreadLoadDayLine, (CChinaStockPtr pCurrentStock), (override));
    MOCK_METHOD(bool, RunningThreadUpdateStockCodeDB, (), (override));
    MOCK_METHOD(bool, RunningThreadUpdateOptionDB, (), (override));
    MOCK_METHOD(bool, UpdateOptionDB, (), (override));
    MOCK_METHOD(bool, UpdateStockCodeDB, (), (override));
    MOCK_METHOD(bool, UpdateTodayTempDB, (), (override));
    MOCK_METHOD(bool, SaveRTData, (), (override));
  };

  typedef shared_ptr<CMockChinaMarket> CMockChinaMarketPtr;
}
