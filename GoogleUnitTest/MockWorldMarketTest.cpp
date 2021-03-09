///////////////////////////////////////////////////////////////////////////////////////////
//
// 所有使用Mock类测试ChinaMarket的函数皆位于此文件中。
//
///////////////////////////////////////////////////////////////////////////////////////////
#include"pch.h"

#include"globedef.h"

#include"Thread.h"

#include"WorldMarket.h"
#include"WorldStock.h"

#include"WebInquirer.h"

#include"MockWorldMarket.h"
using namespace std;
using namespace testing;
#include<memory>

namespace StockAnalysisTest {
  CMockWorldMarket* s_pWorldMarket;
  class CMockWorldMarketTest : public ::testing::Test
  {
  protected:
    static void SetUpTestSuite(void) {
      ASSERT_FALSE(gl_fNormalMode);
      //EXPECT_EQ(gl_pChinaStockMarket->GetDayLineNeedUpdateNumber(), gl_pChinaStockMarket->GetTotalStock());
      s_pWorldMarket = new CMockWorldMarket;
      s_pWorldMarket->LoadStockDB();
      s_pWorldMarket->LoadCountryList();
      s_pWorldMarket->LoadWorldExchangeDB();
      s_pWorldMarket->LoadForexExchange();
      s_pWorldMarket->LoadForexSymbol();
      s_pWorldMarket->LoadWorldChoicedStock();
    }
    static void TearDownTestSuite(void) {
      EXPECT_FALSE(gl_fExitingSystem);
      delete s_pWorldMarket;
    }
    virtual void SetUp(void) override {
      //EXPECT_EQ(gl_pChinaStockMarket->GetDayLineNeedUpdateNumber(), gl_pChinaStockMarket->GetTotalStock());
      gl_fExitingSystem = false;

      while (gl_systemMessage.GetInformationDequeSize() > 0) gl_systemMessage.PopInformationMessage();
      while (gl_systemMessage.GetDayLineInfoDequeSize() > 0) gl_systemMessage.PopDayLineInfoMessage();
      while (gl_systemMessage.GetInnerSystemInformationDequeSize() > 0) gl_systemMessage.PopInnerSystemInformationMessage();
    }

    virtual void TearDown(void) override {
      // clearup

      while (gl_systemMessage.GetInformationDequeSize() > 0) gl_systemMessage.PopInformationMessage();
      while (gl_systemMessage.GetDayLineInfoDequeSize() > 0) gl_systemMessage.PopDayLineInfoMessage();
      while (gl_systemMessage.GetInnerSystemInformationDequeSize() > 0) gl_systemMessage.PopInnerSystemInformationMessage();
    }
  };

  TEST_F(CMockWorldMarketTest, TestTaskUpdateDayLineStartEndDate) {
    EXPECT_CALL(*s_pWorldMarket, RunningthreadUpdateDayLneStartEndDate(s_pWorldMarket))
      .Times(1);
    EXPECT_TRUE(s_pWorldMarket->TaskUpdateDayLineStartEndDate());
  }

  TEST_F(CMockWorldMarketTest, TestTaskUpdateDayLineDB) {
    EXPECT_CALL(*s_pWorldMarket, RunningThreadUpdateDayLineDB)
      .Times(1);
    EXPECT_TRUE(s_pWorldMarket->TaskUpdateDayLineDB());
  }

  TEST_F(CMockWorldMarketTest, TestTaskUpdateStockProfileDB) {
    EXPECT_CALL(*s_pWorldMarket, RunningThreadUpdateStockProfileDB)
      .Times(1);
    EXPECT_TRUE(s_pWorldMarket->TaskUpdateStockProfileDB());
  }

  TEST_F(CMockWorldMarketTest, TestTaskUpdateForexSymbolDB) {
    EXPECT_CALL(*s_pWorldMarket, RunningThreadUpdateForexSymbolDB)
      .Times(1);
    EXPECT_TRUE(s_pWorldMarket->TaskUpdateForexSymbolDB());
  }

  TEST_F(CMockWorldMarketTest, TestTaskUpdateCountryListDB) {
    EXPECT_CALL(*s_pWorldMarket, RunningThreadUpdateCountryListDB)
      .Times(1);
    EXPECT_TRUE(s_pWorldMarket->TaskUpdateCountryListDB());
  }

  TEST_F(CMockWorldMarketTest, TestTaskUpdateEPSSurpriseDB) {
    CWorldStockPtr pStock = nullptr;
    for (long l = 0; l < s_pWorldMarket->GetTotalStock(); l++) {
      pStock = s_pWorldMarket->GetStock(l);
      pStock->SetEPSSurpriseNeedSave(false); // 清除所有需更新标识
    }
    s_pWorldMarket->GetStock(0)->SetEPSSurpriseNeedSave(true);
    s_pWorldMarket->GetStock(1)->SetEPSSurpriseNeedSave(true);
    EXPECT_CALL(*s_pWorldMarket, RunningThreadUpdateEPSSurpriseDB(_))
      .Times(2);
    EXPECT_TRUE(s_pWorldMarket->TaskUpdateEPSSurpriseDB());

    gl_fExitingSystem = true;
    pStock = s_pWorldMarket->GetStock(_T("000001.SS"));
    pStock->SetEPSSurpriseNeedSave(true);
    pStock = s_pWorldMarket->GetStock(1);
    pStock->SetEPSSurpriseNeedSave(true);
    EXPECT_CALL(*s_pWorldMarket, RunningThreadUpdateEPSSurpriseDB(_))
      .Times(1);
    EXPECT_TRUE(s_pWorldMarket->TaskUpdateEPSSurpriseDB()) << "由于gl_fExitingSystem设置为真，导致只执行了一次即退出";
    EXPECT_TRUE(s_pWorldMarket->GetStock(1)->IsEPSSurpriseNeedSave());

    gl_fExitingSystem = false;
    s_pWorldMarket->GetStock(1)->SetEPSSurpriseNeedSave(false);
  }

  TEST_F(CMockWorldMarketTest, TestUpdateCountryListDB) {
    EXPECT_CALL(*s_pWorldMarket, UpdateCountryListDB)
      .Times(1);
    EXPECT_EQ(ThreadUpdateCountryListDB(s_pWorldMarket), 40);
  }

  TEST_F(CMockWorldMarketTest, TestUpdateStockProfileDB) {
    EXPECT_CALL(*s_pWorldMarket, UpdateStockProfileDB)
      .Times(1);
    EXPECT_EQ(ThreadUpdateStockProfileDB(s_pWorldMarket), 37);
  }

  TEST_F(CMockWorldMarketTest, TestUpdateForexSymbolDB) {
    EXPECT_CALL(*s_pWorldMarket, UpdateForexSymbolDB)
      .Times(1);
    EXPECT_EQ(ThreadUpdateForexSymbolDB(s_pWorldMarket), 39);
  }

  TEST_F(CMockWorldMarketTest, TestUpdateDayLineStartEndDate) {
    EXPECT_CALL(*s_pWorldMarket, UpdateDayLineStartEndDate)
      .Times(1);
    EXPECT_EQ(ThreadUpdateWorldStockDayLineStartEndDate(s_pWorldMarket), 43);
  }
}