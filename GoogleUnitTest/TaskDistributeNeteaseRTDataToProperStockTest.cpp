#include"pch.h"

#include"globedef.h"

#include"ChinaStock.h"
#include"ChinaMarket.h"

namespace StockAnalysisTest {
  struct NeteaseData {
    NeteaseData(int count, CString StockCode, int iType, bool fActive, time_t tt) {
      m_iCount = count;
      m_strStockCode = StockCode;
      m_iSourceType = iType;
      m_fActive = fActive;
      m_tt = tt;
    }
  public:
    int m_iCount;
    CString m_strStockCode;
    int m_iSourceType;
    bool m_fActive;
    time_t m_tt;
  };

  // ��Чʵʱ���ݱ�ʶ
  NeteaseData rtData1(1, _T("sh600000"), __INVALID_RT_WEB_DATA__, false, -10);
  // ����ʵʱ���ݣ���ʱ��ȽϾɣ�һ����
  NeteaseData rtData2(2, _T("sz000001"), __SINA_RT_WEB_DATA__, true, -10);
  // �������ݣ����µ�ʱ��
  NeteaseData rtData3(3, _T("sh600601"), __SINA_RT_WEB_DATA__, true, 0);
  // �ǻ�Ծ��Ʊ�����µ�ʱ��
  NeteaseData rtData4(4, _T("sh000834"), __SINA_RT_WEB_DATA__, true, 0);
  // �ڱ����Լ��Ŀ�ʼ����������sh600008��״̬Ϊ�ǻ�Ծ
  NeteaseData rtData5(5, _T("sh600008"), __SINA_RT_WEB_DATA__, true, -5);
  // ��Ч���ڹ�Ʊ����
  //NeteaseData rtData6(6, _T("1400000")__SINA_RT_WEB_DATA__, true, 10101010);
  //NeteaseData rtData7(7, _T("140000")__SINA_RT_WEB_DATA__, true, 10101010);
  //NeteaseData rtData8(8, _T("1400000")__SINA_RT_WEB_DATA__, true, 10101010);

  static time_t s_tCurrentMarketTime;

  class TaskDistributeNeteaseRTDataToProperStockTest : public::testing::TestWithParam<NeteaseData*> {
  protected:
    static void SetUpTestSuite(void) {
      CChinaStockPtr pStock = gl_pChinaStockMarket->GetStock(_T("sh600008"));
      pStock->SetActive(false); // ���⽫600008��״̬����Ϊ����Ծ��������������Բ��ԡ�
      pStock->SetIPOStatus(__STOCK_NULL__); // ���⽫�˹�Ʊ״̬����Ϊδ���С�
      s_tCurrentMarketTime = gl_pChinaStockMarket->GetMarketTime();
    }
    static void TearDownTestSuite(void) {
      CChinaStockPtr pStock = gl_pChinaStockMarket->GetStock(_T("sh600008"));
      pStock->SetActive(true);
    }
    virtual void SetUp(void) override {
      ASSERT_FALSE(gl_fNormalMode);
      ASSERT_TRUE(gl_fTestMode);
      EXPECT_FALSE(gl_ThreadStatus.IsRTDataNeedCalculate());
      EXPECT_EQ(gl_pChinaStockMarket->GetDayLineNeedProcessNumber(), 0);
      NeteaseData* pData = GetParam();
      m_iCount = pData->m_iCount;
      pStock = gl_pChinaStockMarket->GetStock(pData->m_strStockCode);
      pStock->ClearRTDataDeque();
      pStock->SetTransactionTime(s_tCurrentMarketTime - 10);
      gl_pChinaStockMarket->SetNewestTransactionTime(s_tCurrentMarketTime - 10);
      pRTData = make_shared<CRTData>();
      pRTData->SetDataSource(pData->m_iSourceType);
      pRTData->SetStockCode(pData->m_strStockCode);
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
    CRTDataPtr pRTData;
  };

  INSTANTIATE_TEST_SUITE_P(TestCheckNeteaseDayLineInquiryData, TaskDistributeNeteaseRTDataToProperStockTest,
                           testing::Values(&rtData1, &rtData2, &rtData3, &rtData4, &rtData5//, &Data6, &Data7, &Data8
                           ));

  TEST_P(TaskDistributeNeteaseRTDataToProperStockTest, TestCheck) {
    CString strMessage, strRight;
    gl_RTDataContainer.PushNeteaseRTData(pRTData);
    EXPECT_EQ(gl_RTDataContainer.GetNeteaseRTDataSize(), 1);
    EXPECT_TRUE(gl_pChinaStockMarket->TaskDistributeNeteaseRTDataToProperStock());
    EXPECT_EQ(gl_RTDataContainer.GetNeteaseRTDataSize(), 0);
    EXPECT_TRUE(gl_ThreadStatus.IsRTDataNeedCalculate());
    switch (m_iCount) {
    case 1:
    EXPECT_GE(gl_systemMessage.GetInnerSystemInformationDequeSize(), 1) << _T("��Чʵʱ���ݣ�������ֱ�ӷ���");
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
    EXPECT_EQ(pStock->GetTransactionTime(), s_tCurrentMarketTime);
    EXPECT_EQ(pStock->GetRTDataQueueSize(), 1);
    break;
    case 4:
    EXPECT_EQ(gl_pChinaStockMarket->GetNewestTransactionTime(), s_tCurrentMarketTime);
    //EXPECT_FALSE(pStock->IsActive());
    EXPECT_EQ(pStock->GetTransactionTime(), s_tCurrentMarketTime);
    EXPECT_EQ(pStock->GetRTDataQueueSize(), 1);
    break;
    case 5:
    EXPECT_EQ(gl_pChinaStockMarket->GetNewestTransactionTime(), s_tCurrentMarketTime - 5);
    EXPECT_EQ(pStock->GetTransactionTime(), s_tCurrentMarketTime - 5);
    EXPECT_EQ(pStock->GetRTDataQueueSize(), 1);
    EXPECT_TRUE(pStock->IsActive());
    EXPECT_EQ(pStock->GetIPOStatus(), __STOCK_IPOED__);
    break;
    default:
    break;
    }
  }
}