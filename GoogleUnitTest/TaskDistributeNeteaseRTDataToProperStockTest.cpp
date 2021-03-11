#include"pch.h"

#include"globedef.h"

#include"ChinaStock.h"
#include"ChinaMarket.h"

using namespace std;

namespace StockAnalysisTest {
  struct NeteaseData {
    NeteaseData(int count, CString Symbol, int iType, bool fActive, time_t tt) {
      m_iCount = count;
      m_strSymbol = Symbol;
      m_iSourceType = iType;
      m_fActive = fActive;
      m_tt = tt;
    }
  public:
    int m_iCount;
    CString m_strSymbol;
    int m_iSourceType;
    bool m_fActive;
    time_t m_tt;
  };

  // 无效实时数据标识
  NeteaseData rtData1(1, _T("600000.SS"), __INVALID_RT_WEB_DATA__, false, -10);
  // 正常实时数据，但时间比较旧（一样）
  NeteaseData rtData2(2, _T("000001.SZ"), __SINA_RT_WEB_DATA__, true, -10);
  // 正常数据，更新的时间
  NeteaseData rtData3(3, _T("600601.SS"), __SINA_RT_WEB_DATA__, true, 0);
  // 非活跃股票，更新的时间
  NeteaseData rtData4(4, _T("600000.SS"), __SINA_RT_WEB_DATA__, true, 0);
  // 在本测试集的开始，故意设置sh600008的状态为非活跃
  NeteaseData rtData5(5, _T("600008.SS"), __SINA_RT_WEB_DATA__, true, -5);
  // 新股票代码
  NeteaseData rtData6(6, _T("000000.NT"), __SINA_RT_WEB_DATA__, true, 0);
  //NeteaseData rtData7(7, _T("140000")__SINA_RT_WEB_DATA__, true, 10101010);
  //NeteaseData rtData8(8, _T("1400000")__SINA_RT_WEB_DATA__, true, 10101010);

  static time_t s_tCurrentMarketTime;

  class TaskDistributeNeteaseRTDataToProperStockTest : public::testing::TestWithParam<NeteaseData*> {
  protected:
    static void SetUpTestSuite(void) {
      CChinaStockPtr pStock = gl_pChinaStockMarket->GetStock(_T("600008.SS"));
      pStock->SetActive(false); // 故意将600008的状态设置为不活跃，这样测试五可以测试。
      pStock->SetIPOStatus(__STOCK_NULL__); // 故意将此股票状态设置为未上市。
      s_tCurrentMarketTime = gl_pChinaStockMarket->GetMarketTime();
    }
    static void TearDownTestSuite(void) {
      CChinaStockPtr pStock = gl_pChinaStockMarket->GetStock(_T("600008.SS"));
      pStock->SetActive(true);
    }
    virtual void SetUp(void) override {
      ASSERT_FALSE(gl_fNormalMode);
      ASSERT_TRUE(gl_fTestMode);
      EXPECT_FALSE(gl_ThreadStatus.IsRTDataNeedCalculate());
      EXPECT_EQ(gl_pChinaStockMarket->GetDayLineNeedProcessNumber(), 0);
      NeteaseData* pData = GetParam();
      m_iCount = pData->m_iCount;
      if (gl_pChinaStockMarket->IsStock(pData->m_strSymbol)) {
        pStock = gl_pChinaStockMarket->GetStock(pData->m_strSymbol);
        pStock->ClearRTDataDeque();
        pStock->SetTransactionTime(s_tCurrentMarketTime - 10);
      }
      gl_pChinaStockMarket->SetNewestTransactionTime(s_tCurrentMarketTime - 10);
      pRTData = make_shared<CWebRTData>();
      pRTData->SetDataSource(pData->m_iSourceType);
      pRTData->SetSymbol(pData->m_strSymbol);
      pRTData->SetActive(pData->m_fActive);
      pRTData->SetTransactionTime(s_tCurrentMarketTime + pData->m_tt);
    }

    virtual void TearDown(void) override {
      // clearup
      EXPECT_EQ(gl_pChinaStockMarket->GetDayLineNeedProcessNumber(), 0);
      gl_ThreadStatus.SetRTDataNeedCalculate(false);
      pStock->ClearRTDataDeque();
    }

  public:
    int m_iCount;
    CChinaStockPtr pStock;
    CWebRTDataPtr pRTData;
  };

  INSTANTIATE_TEST_SUITE_P(TestCheckNeteaseDayLineInquiryData, TaskDistributeNeteaseRTDataToProperStockTest,
                           testing::Values(&rtData1, &rtData2, &rtData3, &rtData4, &rtData5, &rtData6 //&Data7, &Data8
                           ));

  TEST_P(TaskDistributeNeteaseRTDataToProperStockTest, TestCheck) {
    CString strMessage, strRight;
    long lTotalStock = gl_pChinaStockMarket->GetTotalStock();
    CString strSymbol;

    gl_WebRTDataContainer.PushNeteaseData(pRTData);
    EXPECT_EQ(gl_WebRTDataContainer.GetNeteaseDataSize(), 1);
    EXPECT_TRUE(gl_pChinaStockMarket->TaskDistributeNeteaseRTDataToProperStock());
    EXPECT_EQ(gl_WebRTDataContainer.GetNeteaseDataSize(), 0);
    EXPECT_TRUE(gl_ThreadStatus.IsRTDataNeedCalculate());
    switch (m_iCount) {
    case 1:
    EXPECT_GE(gl_systemMessage.GetInnerSystemInformationDequeSize(), 1) << _T("无效实时数据，报错后直接返回");
    break;
    case 2:
    EXPECT_EQ(gl_pChinaStockMarket->GetNewestTransactionTime(), s_tCurrentMarketTime - 10);
    EXPECT_TRUE(pStock->IsActive());
    EXPECT_EQ(pStock->GetTransactionTime(), s_tCurrentMarketTime - 10);
    EXPECT_EQ(pStock->GetRTDataQueueSize(), 0);
    break;
    case 3:
    EXPECT_EQ(gl_pChinaStockMarket->GetNewestTransactionTime(), s_tCurrentMarketTime);
    EXPECT_TRUE(pStock->IsActive());
    EXPECT_EQ(pStock->GetTransactionTime() - s_tCurrentMarketTime, 0);
    EXPECT_EQ(pStock->GetRTDataQueueSize(), 1);
    break;
    case 4:
    EXPECT_EQ(gl_pChinaStockMarket->GetNewestTransactionTime() - s_tCurrentMarketTime, 0);
    //EXPECT_FALSE(pStock->IsActive());
    EXPECT_EQ(pStock->GetTransactionTime() - s_tCurrentMarketTime, 0);
    EXPECT_EQ(pStock->GetRTDataQueueSize(), 1);
    break;
    case 5:
    EXPECT_EQ(gl_pChinaStockMarket->GetNewestTransactionTime() - s_tCurrentMarketTime, -5);
    EXPECT_EQ(pStock->GetTransactionTime() - s_tCurrentMarketTime, -5);
    EXPECT_EQ(pStock->GetRTDataQueueSize(), 1);
    EXPECT_TRUE(pStock->IsActive());
    EXPECT_TRUE(pStock->IsIPOed());
    break;
    case 6: // 新股票代码
    EXPECT_EQ(lTotalStock + 1, gl_pChinaStockMarket->GetTotalStock()) << "发现新的股票，股票总数增加了一个";
    EXPECT_EQ(pStock, nullptr) << "新股票代码，则不预置此指针";
    pStock = gl_pChinaStockMarket->GetStock(pRTData->GetSymbol());
    EXPECT_TRUE(pStock->IsActive());
    EXPECT_EQ(pStock->GetTransactionTime() - s_tCurrentMarketTime, 0);
    EXPECT_EQ(pStock->GetRTDataQueueSize(), 1);
    strSymbol = pStock->GetSymbol();
    gl_pChinaStockMarket->DeleteStock(pStock);
    EXPECT_EQ(lTotalStock, gl_pChinaStockMarket->GetTotalStock()) << "删除了新增加的股票";
    EXPECT_EQ(lTotalStock, gl_pChinaStockMarket->GetTotalStockMapSize()) << "也同时删除了map索引";
    EXPECT_FALSE(gl_pChinaStockMarket->IsStock(strSymbol)) << "刚刚删除了此股票代码";
    break;
    default:
    break;
    }
  }
}