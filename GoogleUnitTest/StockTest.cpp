#include"stdafx.h"

#include"pch.h"
#include"globedef.h"

#include"Stock.h"

namespace StockAnalysisTest {
  TEST(StockTest, TestInitialize) {
    ASSERT_FALSE(gl_fNormalMode);
    CStock stock;
    EXPECT_EQ(stock.m_vDayLine.size(), 0);
    EXPECT_FALSE(stock.IsActive());
    EXPECT_STREQ(stock.GetStockCode(), _T(""));
    EXPECT_STREQ(stock.GetStockName(), _T(""));
    EXPECT_EQ(stock.GetOffset(), -1);
    EXPECT_EQ(stock.GetTransactionTime(), 0);
    EXPECT_EQ(stock.GetLastClose(), 0);
    EXPECT_EQ(stock.GetOpen(), 0);
    EXPECT_EQ(stock.GetHigh(), 0);
    EXPECT_EQ(stock.GetLow(), 0);
    EXPECT_EQ(stock.GetNew(), 0);
    EXPECT_EQ(stock.GetVolume(), 0);
    EXPECT_EQ(stock.GetAmount(), 0);
    EXPECT_EQ(stock.GetPBuy(0), 0);
    EXPECT_EQ(stock.GetVBuy(0), 0);
    EXPECT_EQ(stock.GetPBuy(1), 0);
    EXPECT_EQ(stock.GetVBuy(1), 0);
    EXPECT_EQ(stock.GetPBuy(2), 0);
    EXPECT_EQ(stock.GetVBuy(2), 0);
    EXPECT_EQ(stock.GetPBuy(3), 0);
    EXPECT_EQ(stock.GetVBuy(3), 0);
    EXPECT_EQ(stock.GetPBuy(4), 0);
    EXPECT_EQ(stock.GetVBuy(4), 0);
    EXPECT_EQ(stock.GetPSell(0), 0);
    EXPECT_EQ(stock.GetVSell(0), 0);
    EXPECT_EQ(stock.GetPSell(1), 0);
    EXPECT_EQ(stock.GetVSell(1), 0);
    EXPECT_EQ(stock.GetPSell(2), 0);
    EXPECT_EQ(stock.GetVSell(2), 0);
    EXPECT_EQ(stock.GetPSell(3), 0);
    EXPECT_EQ(stock.GetVSell(3), 0);
    EXPECT_EQ(stock.GetPSell(4), 0);
    EXPECT_EQ(stock.GetVSell(4), 0);
    EXPECT_EQ(stock.GetAttackBuyAmount(), 0);
    EXPECT_EQ(stock.GetAttackSellAmount(), 0);
    EXPECT_DOUBLE_EQ(stock.GetRelativeStrong(), 0);
    EXPECT_FALSE(stock.IsDayLineLoaded());
    EXPECT_FALSE(stock.IsActive());
    EXPECT_TRUE(stock.IsDayLineNeedUpdate());
    EXPECT_FALSE(stock.IsInquiringOnce());
    EXPECT_FALSE(stock.IsChoiced());
    EXPECT_FALSE(stock.IsMinLineUpdated());
    EXPECT_FALSE(stock.IsDayLineUpdated());
    EXPECT_FALSE(stock.HaveFirstRTData());
  }
  TEST(StockTest, TestGetMarket) {
    CStock stock;
    EXPECT_EQ(stock.GetMarket(), 0);
    stock.SetMarket(1);
    EXPECT_EQ(stock.GetMarket(), 1);
  }

  TEST(StockTest, TestGetStockCode) {
    CStock stock;
    EXPECT_STREQ(stock.GetStockCode(), _T(""));
    stock.SetStockCode(_T("sh600000"));
    EXPECT_STREQ(stock.GetStockCode(), _T("sh600000"));
  }

  TEST(StockTest, TestGetStockName) {
    CStock stock;
    EXPECT_STREQ(stock.GetStockName(), _T(""));
    stock.SetStockName(_T("浦东银行"));
    EXPECT_STREQ(stock.GetStockName(), _T("浦东银行"));
  }

  TEST(StockTest, TestGetOffset) {
    CStock stock;
    EXPECT_EQ(stock.GetOffset(), -1);
    stock.SetOffset(_T(101010));
    EXPECT_EQ(stock.GetOffset(), 101010);
  }

  TEST(StockTest, TestGetDayLineStartDay) {
    CStock stock;
    EXPECT_EQ(stock.GetDayLineStartDay(), 19900101);
    stock.SetDayLineStartDay(100100100);
    EXPECT_EQ(stock.GetDayLineStartDay(), 100100100);
  }

  TEST(StockTest, TestGetDayLineEndDay) {
    CStock stock;
    EXPECT_EQ(stock.GetDayLineEndDay(), 19900101);
    stock.SetDayLineEndDay(100100100);
    EXPECT_EQ(stock.GetDayLineEndDay(), 100100100);
  }

  TEST(StockTest, TestGetIPOStatus) {
    CStock stock;
    EXPECT_EQ(stock.GetIPOStatus(), __STOCK_NOT_CHECKED__);
    stock.SetIPOStatus(100100100);
    EXPECT_EQ(stock.GetIPOStatus(), 100100100);
  }

  TEST(StockTest, TestGetTransactionTime) {
    CStock stock;
    EXPECT_EQ(stock.GetTransactionTime(), 0);
    stock.SetTransactionTime(100100100100);
    EXPECT_EQ(stock.GetTransactionTime(), 100100100100);
  }

  TEST(StockTest, TestGetLastClose) {
    CStock stock;
    EXPECT_EQ(stock.GetLastClose(), 0);
    stock.SetLastClose(100100100);
    EXPECT_EQ(stock.GetLastClose(), 100100100);
  }

  TEST(StockTest, TestGetOpen) {
    CStock stock;
    EXPECT_EQ(stock.GetOpen(), 0);
    stock.SetOpen(100100100);
    EXPECT_EQ(stock.GetOpen(), 100100100);
  }

  TEST(StockTest, TestGetHigh) {
    CStock stock;
    EXPECT_EQ(stock.GetHigh(), 0);
    stock.SetHigh(100100100);
    EXPECT_EQ(stock.GetHigh(), 100100100);
  }

  TEST(StockTest, TestGetLow) {
    CStock stock;
    EXPECT_EQ(stock.GetLow(), 0);
    stock.SetLow(100100100);
    EXPECT_EQ(stock.GetLow(), 100100100);
  }

  TEST(StockTest, TestGetNew) {
    CStock stock;
    EXPECT_EQ(stock.GetNew(), 0);
    stock.SetNew(100100100);
    EXPECT_EQ(stock.GetNew(), 100100100);
  }

  TEST(StockTest, TestGetAmount) {
    CStock stock;
    EXPECT_EQ(stock.GetAmount(), 0);
    stock.SetAmount(100100100);
    EXPECT_EQ(stock.GetAmount(), 100100100);
  }

  TEST(StockTest, TestGetVolume) {
    CStock stock;
    EXPECT_EQ(stock.GetVolume(), 0);
    stock.SetVolume(100100100);
    EXPECT_EQ(stock.GetVolume(), 100100100);
  }

  TEST(StockTest, TestUpdateCurrentStatus) {
    CStock stock;
    CRTDataPtr pRTData;

    pRTData = make_shared<CRTData>();
    pRTData->SetTransactionTime(100100100100);
    pRTData->SetLastClose(11111);
    pRTData->SetOpen(22222);
    pRTData->SetNew(33333);
    pRTData->SetHigh(66666);
    pRTData->SetLow(10000);
    pRTData->SetVolume(1001001001001);
    pRTData->SetAmount(2002002002002);
    for (int i = 0; i < 5; i++) {
      pRTData->SetPBuy(i, i + 100);
      pRTData->SetVBuy(i, i + 200);
      pRTData->SetPSell(i, i + 300);
      pRTData->SetVSell(i, i + 400);
    }
    stock.UpdateStatus(pRTData);
    EXPECT_EQ(stock.GetTransactionTime(), 100100100100);   // UpdateCurrentStatus函数不更新交易时间
    EXPECT_EQ(stock.GetLastClose(), 11111);
    EXPECT_EQ(stock.GetOpen(), 22222);
    EXPECT_EQ(stock.GetNew(), 33333);
    EXPECT_EQ(stock.GetHigh(), 66666);
    EXPECT_EQ(stock.GetLow(), 10000);
    EXPECT_EQ(stock.GetVolume(), 1001001001001);
    EXPECT_EQ(stock.GetAmount(), 2002002002002);
    for (int i = 0; i < 5; i++) {
      EXPECT_EQ(stock.GetPBuy(i), i + 100);
      EXPECT_EQ(stock.GetVBuy(i), i + 200);
      EXPECT_EQ(stock.GetPSell(i), i + 300);
      EXPECT_EQ(stock.GetVSell(i), i + 400);
    }
  }

  TEST(StockTest, TestGetAttackBuyAmount) {
    CStock id;
    EXPECT_EQ(id.GetAttackBuyAmount(), 0);
    id.SetAttackBuyAmount(10101010);
    EXPECT_EQ(id.GetAttackBuyAmount(), 10101010);
  }

  TEST(StockTest, TestGetAttackSellAmount) {
    CStock id;
    EXPECT_EQ(id.GetAttackSellAmount(), 0);
    id.SetAttackSellAmount(10101010);
    EXPECT_EQ(id.GetAttackSellAmount(), 10101010);
  }

  TEST(StockTest, TestGetOrdinaryBuyVolume) {
    CStock id;
    EXPECT_EQ(id.GetOrdinaryBuyVolume(), 0);
    id.SetOrdinaryBuyVolume(10101010);
    EXPECT_EQ(id.GetOrdinaryBuyVolume(), 10101010);
  }

  TEST(StockTest, TestGetOrdinarySellVolume) {
    CStock id;
    EXPECT_EQ(id.GetOrdinarySellVolume(), 0);
    id.SetOrdinarySellVolume(10101010);
    EXPECT_EQ(id.GetOrdinarySellVolume(), 10101010);
  }

  TEST(StockTest, TestGetAttackBuyVolume) {
    CStock id;
    EXPECT_EQ(id.GetAttackBuyVolume(), 0);
    id.SetAttackBuyVolume(10101010);
    EXPECT_EQ(id.GetAttackBuyVolume(), 10101010);
  }

  TEST(StockTest, TestGetStrongBuyVolume) {
    CStock id;
    EXPECT_EQ(id.GetStrongBuyVolume(), 0);
    id.SetStrongBuyVolume(10101010);
    EXPECT_EQ(id.GetStrongBuyVolume(), 10101010);
  }

  TEST(StockTest, TestGetCurrentAttackBuy) {
    CStock id;
    EXPECT_EQ(id.GetCurrentAttackBuy(), 0);
    id.SetCurrentAttackBuy(10101010);
    EXPECT_EQ(id.GetCurrentAttackBuy(), 10101010);
  }

  TEST(StockTest, TestGetCurrentStrongBuy) {
    CStock id;
    EXPECT_EQ(id.GetCurrentStrongBuy(), 0);
    id.SetCurrentStrongBuy(10101010);
    EXPECT_EQ(id.GetCurrentStrongBuy(), 10101010);
  }

  TEST(StockTest, TestGetAttackSellVolume) {
    CStock id;
    EXPECT_EQ(id.GetAttackSellVolume(), 0);
    id.SetAttackSellVolume(10101010);
    EXPECT_EQ(id.GetAttackSellVolume(), 10101010);
  }

  TEST(StockTest, TestGetStrongSellVolume) {
    CStock id;
    EXPECT_EQ(id.GetStrongSellVolume(), 0);
    id.SetStrongSellVolume(10101010);
    EXPECT_EQ(id.GetStrongSellVolume(), 10101010);
  }

  TEST(StockTest, TestGetCurrentAttackSell) {
    CStock id;
    EXPECT_EQ(id.GetCurrentAttackSell(), 0);
    id.SetCurrentAttackSell(10101010);
    EXPECT_EQ(id.GetCurrentAttackSell(), 10101010);
  }

  TEST(StockTest, TestGetCurrentStrongSell) {
    CStock id;
    EXPECT_EQ(id.GetCurrentStrongSell(), 0);
    id.SetCurrentStrongSell(10101010);
    EXPECT_EQ(id.GetCurrentStrongSell(), 10101010);
  }

  TEST(StockTest, TestGetUnknownVolume) {
    CStock id;
    EXPECT_EQ(id.GetUnknownVolume(), 0);
    id.SetUnknownVolume(10101010);
    EXPECT_EQ(id.GetUnknownVolume(), 10101010);
  }

  TEST(StockTest, TestGetCurrentUnknown) {
    CStock id;
    EXPECT_EQ(id.GetCurrentUnknown(), 0);
    id.SetCurrentUnknown(10101010);
    EXPECT_EQ(id.GetCurrentUnknown(), 10101010);
  }

  TEST(StockTest, TestGetCancelBuyVolume) {
    CStock id;
    EXPECT_EQ(id.GetCancelBuyVolume(), 0);
    id.SetCancelBuyVolume(10101010);
    EXPECT_EQ(id.GetCancelBuyVolume(), 10101010);
  }
  TEST(StockTest, TestGetCancelSellVolume) {
    CStock id;
    EXPECT_EQ(id.GetCancelSellVolume(), 0);
    id.SetCancelSellVolume(10101010);
    EXPECT_EQ(id.GetCancelSellVolume(), 10101010);
  }

  TEST(StockTest, TestReadOnlyFunctions) {
    CStock stock;

    EXPECT_DOUBLE_EQ(stock.GetRelativeStrong(), 0);
  }

  TEST(StockTest, TestIsActive) {
    CStock stock;
    EXPECT_FALSE(stock.IsActive());
    stock.SetActive(true);
    EXPECT_TRUE(stock.IsActive());
    stock.SetActive(false);
    EXPECT_FALSE(stock.IsActive());
  }

  TEST(StockTest, TestIsDayLineNeedUpdate) {
    CStock stock;
    EXPECT_TRUE(stock.IsDayLineNeedUpdate());
    stock.SetDayLineNeedUpdate(false);
    EXPECT_FALSE(stock.IsDayLineNeedUpdate());
    stock.SetDayLineNeedUpdate(true);
    EXPECT_TRUE(stock.IsDayLineNeedUpdate());
  }

  TEST(StockTest, TestIsInquiringOnce) {
    CStock stock;
    EXPECT_FALSE(stock.IsInquiringOnce());
    stock.SetInquiringOnce(true);
    EXPECT_TRUE(stock.IsInquiringOnce());
    stock.SetInquiringOnce(false);
    EXPECT_FALSE(stock.IsInquiringOnce());
  }

  TEST(StockTest, TestIsChoiced) {
    CStock stock;
    EXPECT_FALSE(stock.IsChoiced());
    stock.SetChoiced(true);
    EXPECT_TRUE(stock.IsChoiced());
    stock.SetChoiced(false);
    EXPECT_FALSE(stock.IsChoiced());
  }

  TEST(StockTest, TestIsMinLineUpdated) {
    CStock stock;
    EXPECT_FALSE(stock.IsMinLineUpdated());
    stock.SetMinLineUpdated(true);
    EXPECT_TRUE(stock.IsMinLineUpdated());
    stock.SetMinLineUpdated(false);
    EXPECT_FALSE(stock.IsMinLineUpdated());
  }

  TEST(StockTest, TestIsDayLineUpdated) {
    CStock stock;
    EXPECT_FALSE(stock.IsDayLineUpdated());
    stock.SetDayLineUpdated(true);
    EXPECT_TRUE(stock.IsDayLineUpdated());
    stock.SetDayLineUpdated(false);
    EXPECT_FALSE(stock.IsDayLineUpdated());
  }

  TEST(StockTest, TestIsDayLineLoaded) {
    CStock stock;
    EXPECT_FALSE(stock.IsDayLineLoaded());
    stock.SetDayLineLoaded(true);
    EXPECT_TRUE(stock.IsDayLineLoaded());
    stock.SetDayLineLoaded(false);
    EXPECT_FALSE(stock.IsDayLineLoaded());
  }

  TEST(StockTest, TestHaveFirstRTData) {
    CStock stock;
    EXPECT_FALSE(stock.HaveFirstRTData());
    stock.SetHavingFirstRTData(true);
    EXPECT_TRUE(stock.HaveFirstRTData());
    stock.SetHavingFirstRTData(false);
    EXPECT_TRUE(stock.HaveFirstRTData());
  }

  TEST(StockTest, TestIsStartCalculating) {
    CStock stock;
    EXPECT_FALSE(stock.HaveFirstRTData());
    EXPECT_FALSE(stock.SetHavingFirstRTData(false)); // 不允许外部设置停止计算标识（内部可以）
    EXPECT_FALSE(stock.HaveFirstRTData());
    EXPECT_TRUE(stock.SetHavingFirstRTData(true));
    EXPECT_TRUE(stock.HaveFirstRTData());
    EXPECT_FALSE(stock.SetHavingFirstRTData(true));  // 不允许再次设置开始计算标识
    EXPECT_TRUE(stock.HaveFirstRTData());
  }

  TEST(StockTest, TestIsDayNeededSaving) {    // 此两个函数是具备同步机制的，这里没有进行测试
    CStock stock;
    stock.SetDayLineNeedSavingFlag(true);
    EXPECT_TRUE(stock.IsDayLineNeedSaving());
    stock.SetDayLineNeedSavingFlag(false);
    EXPECT_FALSE(stock.IsDayLineNeedSaving());
  }

  TEST(StockTest, TestTodayDataIsActive) {
    CStock stock;
    EXPECT_FALSE(stock.TodayDataIsActive());
    stock.SetActive(true);
    EXPECT_FALSE(stock.TodayDataIsActive());
    stock.SetHigh(1010);
    EXPECT_TRUE(stock.TodayDataIsActive());
    stock.SetHigh(0);
    stock.SetLow(1);
    EXPECT_TRUE(stock.TodayDataIsActive());
    stock.SetLow(0);
    stock.SetAmount(10000);
    EXPECT_TRUE(stock.TodayDataIsActive());
    stock.SetAmount(0);
    stock.SetVolume(100);
    EXPECT_TRUE(stock.TodayDataIsActive());
    stock.SetActive(false);
    EXPECT_FALSE(stock.TodayDataIsActive());
  }

  TEST(StockTest, TestRTDataDeque) {    // 此三个函数是具备同步机制的，这里没有进行测试
    CRTDataPtr pData = make_shared<CRTData>();
    CStock stock;
    EXPECT_EQ(stock.GetRTDataDequeSize(), 0);
    stock.PushRTData(pData);
    EXPECT_EQ(stock.GetRTDataDequeSize(), 1);
    CRTDataPtr pData2 = stock.PopRTData();
    EXPECT_EQ(stock.GetRTDataDequeSize(), 0);
  }

  TEST(StockTest, TestGetGuaDan) {
    CStock stock;
    EXPECT_FALSE(stock.HaveGuaDan(10000));
    stock.SetGuaDan(10000, 10000);
    EXPECT_TRUE(stock.HaveGuaDan(10000));
    EXPECT_EQ(stock.GetGuaDan(10000), 10000);
  }

  TEST(StockTest, TestClearRTDataDeque) {
    CStock stock;
    CRTDataPtr pRTData;
    for (int i = 0; i < 10; i++) {
      pRTData = make_shared<CRTData>();
      pRTData->SetLastClose(i);
      stock.PushRTData(pRTData);
    }
    EXPECT_EQ(stock.GetRTDataDequeSize(), 10);
    stock.ClearRTDataDeque();
    EXPECT_EQ(stock.GetRTDataDequeSize(), 0);
  }

  TEST(StockTest, TestStoreTempInfo) {
    CSetDayLineToday setDayLineToday;
    CStockPtr pStock = make_shared<CStock>();

    pStock->SetMarket(__SHANGHAI_MARKET__);
    pStock->SetStockCode(_T("sh600000"));
    pStock->SetTransactionTime(20191101); // 此处设置固定的日期，而存储时使用的是当前日期，故而需要与gl_systemTime.GetDay()作比较
    pStock->SetLastClose(101010);
    pStock->SetOpen(202020);
    pStock->SetHigh(303030);
    pStock->SetLow(404040);
    pStock->SetNew(505050);
    pStock->SetVolume(60606060606060);
    pStock->SetAmount(707070707070707);
    pStock->SetUpDown(pStock->GetOpen() - pStock->GetNew());
    pStock->SetUpDownRate((double)pStock->GetUpDown() / pStock->GetLastClose() * 100);
    pStock->SetCurrentValue(808080808080808);
    pStock->SetTotalValue(909090909090909);

    pStock->SetTransactionNumber(123435);
    pStock->SetTransactionNumberBelow5000(45346456);
    pStock->SetTransactionNumberBelow50000(5698);
    pStock->SetTransactionNumberBelow200000(67687);
    pStock->SetTransactionNumberAbove200000(6876);
    pStock->SetOrdinaryBuyVolume(435);
    pStock->SetOrdinarySellVolume(560985);
    pStock->SetAttackBuyBelow50000(54509);
    pStock->SetAttackBuyBelow200000(45049);
    pStock->SetAttackBuyAbove200000(34508);
    pStock->SetAttackSellBelow50000(45896);
    pStock->SetAttackSellBelow200000(56457);
    pStock->SetAttackSellAbove200000(8767);
    pStock->SetAttackBuyAmount(1234566);
    pStock->SetAttackBuyVolume(23423534);
    pStock->SetAttackSellAmount(4353454);
    pStock->SetAttackSellVolume(94589489);
    pStock->SetCancelBuyVolume(435245);
    pStock->SetCancelSellVolume(45648698);
    pStock->SetUnknownVolume(4895747);
    pStock->SetStrongBuyVolume(453456);
    pStock->SetStrongSellVolume(98976);

    ASSERT(!gl_fNormalMode);
    ASSERT(gl_fTestMode);
    setDayLineToday.Open();
    setDayLineToday.m_pDatabase->BeginTrans();
    while (!setDayLineToday.IsEOF()) {
      setDayLineToday.Delete();
      setDayLineToday.MoveNext();
    }
    setDayLineToday.m_pDatabase->CommitTrans();
    setDayLineToday.m_pDatabase->BeginTrans();
    setDayLineToday.AddNew();

    pStock->StoreTempInfo(setDayLineToday);
    setDayLineToday.Update();
    setDayLineToday.m_pDatabase->CommitTrans();
    setDayLineToday.Close();

    setDayLineToday.Open();
    EXPECT_EQ(setDayLineToday.m_Day, gl_systemTime.GetDay()); //存储时使用的是当前日期，故而需要与gl_systemTime.GetDay()作比较
    EXPECT_EQ(setDayLineToday.m_Market, pStock->GetMarket());
    EXPECT_STREQ(setDayLineToday.m_StockCode, pStock->GetStockCode());
    EXPECT_DOUBLE_EQ(atof(setDayLineToday.m_LastClose), (double)pStock->GetLastClose() / 1000);
    EXPECT_DOUBLE_EQ(atof(setDayLineToday.m_Open), (double)pStock->GetOpen() / 1000);
    EXPECT_DOUBLE_EQ(atof(setDayLineToday.m_High), (double)pStock->GetHigh() / 1000);
    EXPECT_DOUBLE_EQ(atof(setDayLineToday.m_Low), (double)pStock->GetLow() / 1000);
    EXPECT_DOUBLE_EQ(atof(setDayLineToday.m_Close), (double)pStock->GetNew() / 1000);
    EXPECT_EQ(atoll(setDayLineToday.m_Volume), pStock->GetVolume());
    EXPECT_EQ(atoll(setDayLineToday.m_Amount), pStock->GetAmount());
    EXPECT_EQ(atof(setDayLineToday.m_UpAndDown), (double)pStock->GetUpDown() / 1000);
    EXPECT_DOUBLE_EQ(atof(setDayLineToday.m_UpDownRate), pStock->GetUpDownRate());
    EXPECT_EQ(atoll(setDayLineToday.m_CurrentValue), pStock->GetCurrentValue());
    EXPECT_EQ(atoll(setDayLineToday.m_TotalValue), pStock->GetTotalValue());

    EXPECT_EQ(atol(setDayLineToday.m_AttackBuyAbove200000), pStock->GetAttackBuyAbove200000());
    EXPECT_EQ(atol(setDayLineToday.m_AttackBuyBelow200000), pStock->GetAttackBuyBelow200000());
    EXPECT_EQ(atol(setDayLineToday.m_AttackBuyBelow50000), pStock->GetAttackBuyBelow50000());
    EXPECT_EQ(atol(setDayLineToday.m_AttackBuyVolume), pStock->GetAttackBuyVolume());
    EXPECT_EQ(atol(setDayLineToday.m_AttackSellAbove200000), pStock->GetAttackSellAbove200000());
    EXPECT_EQ(atol(setDayLineToday.m_AttackSellBelow200000), pStock->GetAttackSellBelow200000());
    EXPECT_EQ(atol(setDayLineToday.m_AttackSellBelow50000), pStock->GetAttackSellBelow50000());
    EXPECT_EQ(atol(setDayLineToday.m_AttackSellVolume), pStock->GetAttackSellVolume());

    EXPECT_EQ(atol(setDayLineToday.m_OrdinaryBuyVolume), pStock->GetOrdinaryBuyVolume());
    EXPECT_EQ(atol(setDayLineToday.m_OrdinarySellVolume), pStock->GetOrdinarySellVolume());
    EXPECT_EQ(atol(setDayLineToday.m_CancelBuyVolume), pStock->GetCancelBuyVolume());
    EXPECT_EQ(atol(setDayLineToday.m_CancelSellVolume), pStock->GetCancelSellVolume());
    EXPECT_EQ(atol(setDayLineToday.m_StrongBuyVolume), pStock->GetStrongBuyVolume());
    EXPECT_EQ(atol(setDayLineToday.m_StrongSellVolume), pStock->GetStrongSellVolume());
    EXPECT_EQ(atol(setDayLineToday.m_UnknownVolume), pStock->GetUnknownVolume());

    EXPECT_EQ(atol(setDayLineToday.m_TransactionNumber), pStock->GetTransactionNumber());
    EXPECT_EQ(atol(setDayLineToday.m_TransactionNumberAbove200000), pStock->GetTransactionNumberAbove200000());
    EXPECT_EQ(atol(setDayLineToday.m_TransactionNumberBelow200000), pStock->GetTransactionNumberBelow200000());
    EXPECT_EQ(atol(setDayLineToday.m_TransactionNumberBelow50000), pStock->GetTransactionNumberBelow50000());
    EXPECT_EQ(atol(setDayLineToday.m_TransactionNumberBelow5000), pStock->GetTransactionNumberBelow5000());
    setDayLineToday.Close();
  }

  TEST(StockTest, TestStoreTodayInfo) {
    CSetDayLine setDayLine;
    CSetDayLineInfo setDayLineInfo;
    CStockPtr pStock = make_shared<CStock>();
    long lDay = 21090101;
    time_t tt = gl_systemTime.ChangeDayToMarketCloseTime(lDay);
    CString strDay = _T("21090101"); // 最好设置此日期为未来，以防止误操作实际数据库

    pStock->SetMarket(__SHANGHAI_MARKET__);
    pStock->SetStockCode(_T("sh600000"));
    pStock->SetTransactionTime(tt); // 此处设置固定的日期，而存储时使用的是当前日期，故而需要与gl_systemTime.GetDay()作比较
    pStock->SetLastClose(101010);
    pStock->SetOpen(202020);
    pStock->SetHigh(303030);
    pStock->SetLow(404040);
    pStock->SetNew(505050);
    pStock->SetVolume(606060606060);
    pStock->SetAmount(707070707070707);
    pStock->SetUpDown(pStock->GetOpen() - pStock->GetNew());
    pStock->SetUpDownRate((double)pStock->GetUpDown() / pStock->GetLastClose() * 100);
    pStock->SetCurrentValue(8080808080808);
    pStock->SetTotalValue(9090909090909);

    pStock->SetTransactionNumber(123435);
    pStock->SetTransactionNumberBelow5000(45346456);
    pStock->SetTransactionNumberBelow50000(5698);
    pStock->SetTransactionNumberBelow200000(67687);
    pStock->SetTransactionNumberAbove200000(6876);
    pStock->SetOrdinaryBuyVolume(435);
    pStock->SetOrdinarySellVolume(560985);
    pStock->SetAttackBuyBelow50000(54509);
    pStock->SetAttackBuyBelow200000(45049);
    pStock->SetAttackBuyAbove200000(34508);
    pStock->SetAttackSellBelow50000(45896);
    pStock->SetAttackSellBelow200000(56457);
    pStock->SetAttackSellAbove200000(8767);
    pStock->SetAttackBuyAmount(1234566);
    pStock->SetAttackBuyVolume(23423534);
    pStock->SetAttackSellAmount(4353454);
    pStock->SetAttackSellVolume(94589489);
    pStock->SetCancelBuyVolume(435245);
    pStock->SetCancelSellVolume(45648698);
    pStock->SetUnknownVolume(4895747);
    pStock->SetStrongBuyVolume(453456);
    pStock->SetStrongSellVolume(98976);

    ASSERT(!gl_fNormalMode);
    ASSERT(gl_fTestMode);
    setDayLine.m_strFilter = _T("[Day] =");
    setDayLine.m_strFilter += strDay;
    setDayLine.Open();
    setDayLine.m_pDatabase->BeginTrans();
    setDayLine.AddNew();
    pStock->StoreBasicInfo(setDayLine);
    setDayLine.Update();
    setDayLine.m_pDatabase->CommitTrans();
    setDayLine.Close();

    setDayLineInfo.m_strFilter = _T("[Day] =");
    setDayLineInfo.m_strFilter += strDay;
    setDayLineInfo.Open();
    setDayLineInfo.m_pDatabase->BeginTrans();
    setDayLineInfo.AddNew();
    setDayLineInfo.m_Day = lDay;
    pStock->StoreCalculatedInfo(setDayLineInfo);
    setDayLineInfo.Update();
    setDayLineInfo.m_pDatabase->CommitTrans();
    setDayLineInfo.Close();

    setDayLine.m_strFilter = _T("[Day] =");
    setDayLine.m_strFilter += strDay;
    setDayLine.Open();
    EXPECT_EQ(setDayLine.m_Day, lDay); //存储时使用的是当前日期，故而需要与gl_systemTime.GetDay()作比较
    EXPECT_EQ(setDayLine.m_Market, pStock->GetMarket());
    EXPECT_STREQ(setDayLine.m_StockCode, pStock->GetStockCode());
    EXPECT_DOUBLE_EQ(atof(setDayLine.m_LastClose), (double)pStock->GetLastClose() / 1000);
    EXPECT_DOUBLE_EQ(atof(setDayLine.m_Open), (double)pStock->GetOpen() / 1000);
    EXPECT_DOUBLE_EQ(atof(setDayLine.m_High), (double)pStock->GetHigh() / 1000);
    EXPECT_DOUBLE_EQ(atof(setDayLine.m_Low), (double)pStock->GetLow() / 1000);
    EXPECT_DOUBLE_EQ(atof(setDayLine.m_Close), (double)pStock->GetNew() / 1000);
    EXPECT_EQ(atoll(setDayLine.m_Volume), pStock->GetVolume());
    EXPECT_EQ(atoll(setDayLine.m_Amount), pStock->GetAmount());
    EXPECT_EQ(atof(setDayLine.m_UpAndDown), (double)pStock->GetUpDown() / 1000);
    EXPECT_DOUBLE_EQ(atof(setDayLine.m_UpDownRate), pStock->GetUpDownRate());
    EXPECT_EQ(atoll(setDayLine.m_CurrentValue), pStock->GetCurrentValue());
    EXPECT_EQ(atoll(setDayLine.m_TotalValue), pStock->GetTotalValue());
    setDayLine.Close();
    setDayLine.m_strFilter = _T("[Day] =");
    setDayLine.m_strFilter += strDay;
    setDayLine.Open();
    setDayLine.m_pDatabase->BeginTrans();
    while (!setDayLine.IsEOF()) {
      setDayLine.Delete();
      setDayLine.MoveNext();
    }
    setDayLine.m_pDatabase->CommitTrans();
    setDayLine.Close();

    setDayLineInfo.m_strFilter = _T("[Day] =");
    setDayLineInfo.m_strFilter += strDay;
    setDayLineInfo.Open();
    EXPECT_EQ(atol(setDayLineInfo.m_AttackBuyAbove200000), pStock->GetAttackBuyAbove200000());
    EXPECT_EQ(atol(setDayLineInfo.m_AttackBuyBelow200000), pStock->GetAttackBuyBelow200000());
    EXPECT_EQ(atol(setDayLineInfo.m_AttackBuyBelow50000), pStock->GetAttackBuyBelow50000());
    EXPECT_EQ(atol(setDayLineInfo.m_AttackBuyVolume), pStock->GetAttackBuyVolume());
    EXPECT_EQ(atol(setDayLineInfo.m_AttackSellAbove200000), pStock->GetAttackSellAbove200000());
    EXPECT_EQ(atol(setDayLineInfo.m_AttackSellBelow200000), pStock->GetAttackSellBelow200000());
    EXPECT_EQ(atol(setDayLineInfo.m_AttackSellBelow50000), pStock->GetAttackSellBelow50000());
    EXPECT_EQ(atol(setDayLineInfo.m_AttackSellVolume), pStock->GetAttackSellVolume());

    EXPECT_EQ(atol(setDayLineInfo.m_OrdinaryBuyVolume), pStock->GetOrdinaryBuyVolume());
    EXPECT_EQ(atol(setDayLineInfo.m_OrdinarySellVolume), pStock->GetOrdinarySellVolume());
    EXPECT_EQ(atol(setDayLineInfo.m_CancelBuyVolume), pStock->GetCancelBuyVolume());
    EXPECT_EQ(atol(setDayLineInfo.m_CancelSellVolume), pStock->GetCancelSellVolume());
    EXPECT_EQ(atol(setDayLineInfo.m_StrongBuyVolume), pStock->GetStrongBuyVolume());
    EXPECT_EQ(atol(setDayLineInfo.m_StrongSellVolume), pStock->GetStrongSellVolume());
    EXPECT_EQ(atol(setDayLineInfo.m_UnknownVolume), pStock->GetUnknownVolume());

    EXPECT_EQ(atol(setDayLineInfo.m_TransactionNumber), pStock->GetTransactionNumber());
    EXPECT_EQ(atol(setDayLineInfo.m_TransactionNumberAbove200000), pStock->GetTransactionNumberAbove200000());
    EXPECT_EQ(atol(setDayLineInfo.m_TransactionNumberBelow200000), pStock->GetTransactionNumberBelow200000());
    EXPECT_EQ(atol(setDayLineInfo.m_TransactionNumberBelow50000), pStock->GetTransactionNumberBelow50000());
    EXPECT_EQ(atol(setDayLineInfo.m_TransactionNumberBelow5000), pStock->GetTransactionNumberBelow5000());
    setDayLineInfo.Close();
    setDayLineInfo.m_strFilter = _T("[Day] =");
    setDayLineInfo.m_strFilter += strDay;
    setDayLineInfo.Open();
    setDayLineInfo.m_pDatabase->BeginTrans();
    while (!setDayLineInfo.IsEOF()) {
      setDayLineInfo.Delete();
      setDayLineInfo.MoveNext();
    }
    setDayLineInfo.m_pDatabase->CommitTrans();
    setDayLineInfo.Close();
  }
}