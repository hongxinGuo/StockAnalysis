#pragma once

#include"stdafx.h"
#include"pch.h"

#include"Market.h"
#include"Stock.h"

#include"SetStockCode.h"

using namespace testing;

namespace StockAnalysisTest {
  class TestEnvironment : public::testing::Environment {  // 全局初始化，由main()函数调用。
  public:
    virtual ~TestEnvironment() {
    }

    virtual void SetUp(void) {
      // 重置股票池状态（因已装入实际状态）
      for (auto pStock : gl_ChinaStockMarket.m_vChinaMarketAStock) {
        pStock->SetDayLineEndDay(-1);
        pStock->SetDayLineNeedUpdate(true);
      }
      // 初始话活跃股票标识
      EXPECT_TRUE(gl_fTestMode);
      EXPECT_FALSE(gl_fNormalMode);
      CSetStockCode setStockCode;
      setStockCode.Open();
      while (!setStockCode.IsEOF()) {
        CStockPtr pStock = gl_ChinaStockMarket.GetStockPtr(setStockCode.m_StockCode);
        EXPECT_FALSE(pStock->IsActive());
        if (setStockCode.m_IPOed == __STOCK_IPOED__) {
          pStock->SetActive(true);
          pStock->SetStockCode(setStockCode.m_StockCode);
          //pStock->SetStockName(setStockCode.m_StockName); // 字符串制式不相符
          pStock->SetMarket(setStockCode.m_StockType);
          gl_ChinaStockMarket.IncreaseActiveStockNumber();
        }
        setStockCode.MoveNext();
      }
      setStockCode.Close();
      EXPECT_GT(gl_ChinaStockMarket.GetTotalActiveStock(), 0);
    }

    virtual void TearDown(void) {
    }
  };
}