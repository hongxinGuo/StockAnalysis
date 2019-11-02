#include "pch.h"

#include"globedef.h"
#include"StockBasicInfo.h"

using namespace testing;

namespace StockAnalysisTest {
  TEST(StockBasicInfoTest, TestInitialize) {
    ASSERT_FALSE(gl_fNormalMode);
    CStockBasicInfo id;
    EXPECT_EQ(id.GetMarket(), 0);
    EXPECT_STREQ(id.GetStockCode(), _T(""));
    EXPECT_STREQ(id.GetStockName(), _T(""));
    EXPECT_EQ(id.GetOffset(), -1);
    EXPECT_EQ(id.GetDayLineEndDay(), 19900101);
    EXPECT_EQ(id.GetIPOStatus(), __STOCK_NOT_CHECKED__);
    EXPECT_FALSE(id.IsActive());
    EXPECT_TRUE(id.IsDayLineNeedUpdate());
    EXPECT_FALSE(id.IsInquiringOnce());
    id.SetMarket(1);
    id.SetStockCode(_T("abcde"));
    id.SetStockName(_T("dcba"));
    id.SetOffset(1);
    id.SetDayLineEndDay(20020202);
    id.SetIPOStatus(0);
    id.SetActive(true);
    id.SetDayLineNeedUpdate(false);
    id.SetInquiringOnce(true);
    EXPECT_EQ(id.GetMarket(), 1);
    EXPECT_STREQ(id.GetStockCode(), _T("abcde"));
    EXPECT_STREQ(id.GetStockName(), _T("dcba"));
    EXPECT_EQ(id.GetOffset(), 1);
    EXPECT_EQ(id.GetDayLineEndDay(), 20020202);
    EXPECT_EQ(id.GetIPOStatus(), 0);
    EXPECT_TRUE(id.IsActive());
    EXPECT_FALSE(id.IsDayLineNeedUpdate());
    EXPECT_TRUE(id.IsInquiringOnce());
    id.Reset();
    EXPECT_EQ(id.GetMarket(), 0);
    EXPECT_STREQ(id.GetStockCode(), _T(""));
    EXPECT_STREQ(id.GetStockName(), _T(""));
    EXPECT_EQ(id.GetOffset(), -1);
    EXPECT_EQ(id.GetDayLineEndDay(), 19900101);
    EXPECT_EQ(id.GetIPOStatus(), __STOCK_NOT_CHECKED__);
    EXPECT_FALSE(id.IsActive());
    EXPECT_TRUE(id.IsDayLineNeedUpdate());
    EXPECT_FALSE(id.IsInquiringOnce());
  }

  TEST(StockBasicInfoTest, TestGetMarket) {
    CStockBasicInfo id;
    EXPECT_EQ(id.GetMarket(), 0);
    id.SetMarket(__SHANGHAI_MARKET__);
    EXPECT_EQ(id.GetMarket(), __SHANGHAI_MARKET__);
    id.SetMarket(__SHENZHEN_MARKET__);
    EXPECT_EQ(id.GetMarket(), __SHENZHEN_MARKET__);
  }

  TEST(StockBasicInfoTest, TestGetStockCode) {
    CStockBasicInfo id;
    EXPECT_STREQ(id.GetStockCode(), _T(""));
    id.SetStockCode(_T("sh600000"));
    EXPECT_STREQ(id.GetStockCode(), _T("sh600000"));
  }

  TEST(StockBasicInfoTest, TestGetStockName) {
    CStockBasicInfo id;
    EXPECT_STREQ(id.GetStockName(), _T(""));
    id.SetStockName(_T("sh600000"));
    EXPECT_STREQ(id.GetStockName(), _T("sh600000"));
  }
  TEST(StockBasicInfoTest, TestGetOffset) {
    CStockBasicInfo id;
    EXPECT_EQ(id.GetOffset(), -1);
    id.SetOffset(10101);
    EXPECT_EQ(id.GetOffset(), 10101);
  }

  TEST(StockBasicInfoTest, TestGetDayLineEndDay) {
    CStockBasicInfo id;
    EXPECT_EQ(id.GetDayLineEndDay(), 19900101);
    id.SetDayLineEndDay(19980101);
    EXPECT_EQ(id.GetDayLineEndDay(), 19980101);
  }

  TEST(StockBasicInfoTest, TestGetDayLineStartDay) {
    CStockBasicInfo id;
    EXPECT_EQ(id.GetDayLineStartDay(), 19900101);
    id.SetDayLineStartDay(19980101);
    EXPECT_EQ(id.GetDayLineStartDay(), 19980101);
  }

  TEST(StockBasicInfoTest, TestGetIPOStatus) {
    CStockBasicInfo id;
    EXPECT_EQ(id.GetIPOStatus(), 128);
    id.SetIPOStatus(255);
    EXPECT_EQ(id.GetIPOStatus(), 255);
  }

  TEST(StockBasicInfoTest, TestIsActive) {
    CStockBasicInfo id;
    EXPECT_FALSE(id.IsActive());
    id.SetActive(true);
    EXPECT_TRUE(id.IsActive());
    id.SetActive(false);
    EXPECT_FALSE(id.IsActive());
  }

  TEST(StockBasicInfoTest, TestIsDayLineNeedUpdate) {
    CStockBasicInfo id;
    EXPECT_TRUE(id.IsDayLineNeedUpdate());
    id.SetDayLineNeedUpdate(false);
    EXPECT_FALSE(id.IsDayLineNeedUpdate());
    id.SetDayLineNeedUpdate(true);
    EXPECT_TRUE(id.IsDayLineNeedUpdate());
  }

  TEST(StockBasicInfoTest, TestIsInquiringOnce) {
    CStockBasicInfo id;
    EXPECT_FALSE(id.IsInquiringOnce());
    id.SetInquiringOnce(true);
    EXPECT_TRUE(id.IsInquiringOnce());
    id.SetInquiringOnce(false);
    EXPECT_FALSE(id.IsInquiringOnce());
  }

  TEST(StockBasicInfoTest, TestIsNeedUpdate) {
    CStockBasicInfo id;
    EXPECT_TRUE(id.IsNeedUpdate());
    id.SetNeedUpdate(false);
    EXPECT_FALSE(id.IsNeedUpdate());
    id.SetNeedUpdate(true);
    EXPECT_TRUE(id.IsNeedUpdate());
  }

  TEST(StockBasicInfoTest, TestGetTransactionTime) {
    CStockBasicInfo id;
    EXPECT_EQ(id.GetTransactionTime(), 0);
    id.SetTransactionTime(1010101010);
    EXPECT_EQ(id.GetTransactionTime(), 1010101010);
  }

  TEST(StockBasicInfoTest, TestGetLastClose) {
    CStockBasicInfo id;
    EXPECT_EQ(id.GetLastClose(), 0);
    id.SetLastClose(10101010);
    EXPECT_EQ(id.GetLastClose(), 10101010);
  }
  TEST(StockBasicInfoTest, TestGetOpen) {
    CStockBasicInfo id;
    EXPECT_EQ(id.GetOpen(), 0);
    id.SetOpen(10101);
    EXPECT_EQ(id.GetOpen(), 10101);
  }

  TEST(StockBasicInfoTest, TestGetHigh) {
    CStockBasicInfo id;
    EXPECT_EQ(id.GetHigh(), 0);
    id.SetHigh(19980101);
    EXPECT_EQ(id.GetHigh(), 19980101);
  }

  TEST(StockBasicInfoTest, TestGetLow) {
    CStockBasicInfo id;
    EXPECT_EQ(id.GetLow(), 0);
    id.SetLow(19980101);
    EXPECT_EQ(id.GetLow(), 19980101);
  }

  TEST(StockBasicInfoTest, TestGetNew) {
    CStockBasicInfo id;
    EXPECT_EQ(id.GetNew(), 0);
    id.SetNew(10101010);
    EXPECT_EQ(id.GetNew(), 10101010);
  }
  TEST(StockBasicInfoTest, TestGetAmount) {
    CStockBasicInfo id;
    EXPECT_EQ(id.GetAmount(), 0);
    id.SetAmount(1010101010101010);
    EXPECT_EQ(id.GetAmount(), 1010101010101010);
  }
  TEST(StockBasicInfoTest, TestGetVolume) {
    CStockBasicInfo id;
    EXPECT_EQ(id.GetVolume(), 0);
    id.SetVolume(10101010);
    EXPECT_EQ(id.GetVolume(), 10101010);
  }
  TEST(StockBasicInfoTest, TestGetTotalValue) {
    CStockBasicInfo id;
    EXPECT_EQ(id.GetTotalValue(), 0);
    id.SetTotalValue(10101010);
    EXPECT_EQ(id.GetTotalValue(), 10101010);
  }
  TEST(StockBasicInfoTest, TestGetCurrentValue) {
    CStockBasicInfo id;
    EXPECT_EQ(id.GetCurrentValue(), 0);
    id.SetCurrentValue(10101010);
    EXPECT_EQ(id.GetCurrentValue(), 10101010);
  }
  TEST(StockBasicInfoTest, TestGetPBuy) {
    CStockBasicInfo id;
    for (int i = 0; i < 5; i++) {
      EXPECT_EQ(id.GetPBuy(i), 0);
      id.SetPBuy(i, 10101010);
      EXPECT_EQ(id.GetPBuy(i), 10101010);
    }
  }

  TEST(StockBasicInfoTest, TestGetVBuy) {
    CStockBasicInfo id;
    for (int i = 0; i < 5; i++) {
      EXPECT_EQ(id.GetVBuy(i), 0);
      id.SetVBuy(i, 10101010);
      EXPECT_EQ(id.GetVBuy(i), 10101010);
    }
  }

  TEST(StockBasicInfoTest, TestGetPSell) {
    CStockBasicInfo id;
    for (int i = 0; i < 5; i++) {
      EXPECT_EQ(id.GetPSell(i), 0);
      id.SetPSell(i, 10101010);
      EXPECT_EQ(id.GetPSell(i), 10101010);
    }
  }

  TEST(StockBasicInfoTest, TestGetVSell) {
    CStockBasicInfo id;
    for (int i = 0; i < 5; i++) {
      EXPECT_EQ(id.GetVSell(i), 0);
      id.SetVSell(i, 10101010);
      EXPECT_EQ(id.GetVSell(i), 10101010);
    }
  }

  TEST(StockBasicInfoTest, TestGetRelativeStrong) {
    CStockBasicInfo id;
    EXPECT_DOUBLE_EQ(id.GetRelativeStrong(), 0.0);
    id.SetRelativeStrong(10101010.0);
    EXPECT_DOUBLE_EQ(id.GetRelativeStrong(), 10101010.0);
  }
}