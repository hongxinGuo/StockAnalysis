#include"stdafx.h"
#include"pch.h"
#include"globedef.h"

#include"StakeCode.h"

namespace StockAnalysisTest {
  class CStakeCodeTest : public ::testing::Test
  {
  protected:
    static void SetUpTestSuite(void) { // 本测试类的初始化函数
      ASSERT_FALSE(gl_fNormalMode);
      EXPECT_EQ(gl_pChinaStockMarket->GetCurrentStock(), nullptr) << gl_pChinaStockMarket->GetCurrentStock()->GetStockCode();
    }

    static void TearDownTestSuite() {
      EXPECT_EQ(gl_pChinaStockMarket->GetCurrentStock(), nullptr) << gl_pChinaStockMarket->GetCurrentStock()->GetStockCode();
      EXPECT_EQ(gl_pChinaStockMarket->GetDayLineNeedProcessNumber(), 0);
    }

    virtual void SetUp(void) override {
      ASSERT_FALSE(gl_fNormalMode);
      ASSERT_TRUE(gl_fTestMode);
    }

    virtual void TearDown(void) override {
      // clearup
    }
  };

  TEST_F(CStakeCodeTest, TestGetMarket) {
    CStakeCode sc;
    EXPECT_EQ(sc.GetMarket(), 0);
    sc.SetMarket(1);
    EXPECT_EQ(sc.GetMarket(), 1);
  }

  TEST_F(CStakeCodeTest, TestGetStakeCode) {
    CStakeCode sc;
    EXPECT_STREQ(sc.GetStockCode(), _T(""));
    sc.SetStockCode(_T("sh600000"));
    EXPECT_STREQ(sc.GetStockCode(), _T("sh600000"));
  }

  TEST_F(CStakeCodeTest, TestGetStakeName) {
    CStakeCode sc;
    EXPECT_STREQ(sc.GetStockName(), _T(""));
    sc.SetStockCode(_T("中远海运"));
    EXPECT_STREQ(sc.GetStockCode(), _T("中远海运"));
  }

  TEST_F(CStakeCodeTest, TestGetOffset) {
    CStakeCode sc;
    EXPECT_EQ(sc.GetOffset(), 0);
    sc.SetOffset(10101010);
    EXPECT_EQ(sc.GetOffset(), 10101010);
  }
}