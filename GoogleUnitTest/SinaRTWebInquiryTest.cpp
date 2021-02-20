#include"stdafx.h"
#include"pch.h"

#include"globedef.h"

#include"MockSinaRTWebInquiry.h"

using namespace std;
#include<atomic>

using namespace testing;

namespace StockAnalysisTest {
  class CSinaRTWebInquiryTest : public ::testing::Test {
  protected:
    static void SetUpTestSuite(void) {
      gl_pChinaStockMarket->ResetSinaStockRTDataInquiringIndex();
    }

    static void TearDownTestSuite(void) {
      while (gl_ThreadStatus.IsSavingThreadRunning()) Sleep(1);
    }

    virtual void SetUp(void) override {
      ASSERT_FALSE(gl_fNormalMode);
      ASSERT_TRUE(gl_fTestMode);
      gl_pChinaStockMarket->ResetSinaStockRTDataInquiringIndex();
    }

    virtual void TearDown(void) override {
      // clearup
      gl_pChinaStockMarket->ResetSinaStockRTDataInquiringIndex();
    }
  public:
    CMockSinaRTWebInquiry m_SinaRTWebInquiry; // 新浪实时数据采集
  };

  TEST_F(CSinaRTWebInquiryTest, TestInitialize) {
    EXPECT_STREQ(m_SinaRTWebInquiry.GetInquiringStringPrefix(), _T("http://hq.sinajs.cn/list="));
    EXPECT_STREQ(m_SinaRTWebInquiry.GetInquiringStringSuffix(), _T(""));
    EXPECT_FALSE(m_SinaRTWebInquiry.IsReportStatus());
    EXPECT_EQ(m_SinaRTWebInquiry.GetInquiringNumber(), 850) << _T("新浪默认值");
  }

  TEST_F(CSinaRTWebInquiryTest, TestStartReadingThread) {
    EXPECT_FALSE(m_SinaRTWebInquiry.IsReadingWebData());
    EXPECT_EQ(m_SinaRTWebInquiry.GetByteReaded(), 0);
  }

  TEST_F(CSinaRTWebInquiryTest, TestGetWebData) {
    m_SinaRTWebInquiry.SetReadingWebData(true);
    EXPECT_FALSE(m_SinaRTWebInquiry.GetWebData());
    m_SinaRTWebInquiry.SetReadingWebData(false);
    gl_pChinaStockMarket->SetSystemReady(true);
    EXPECT_CALL(m_SinaRTWebInquiry, StartReadingThread)
      .Times(1);
    m_SinaRTWebInquiry.GetWebData();
    EXPECT_TRUE(m_SinaRTWebInquiry.IsReadingWebData()) << _T("此标志由工作线程负责重置。此处调用的是Mock类，故而此标识没有重置");
  }

  TEST_F(CSinaRTWebInquiryTest, TestPrepareInquiringStr) {
    EXPECT_EQ(gl_pChinaStockMarket->GetSinaStockRTDataInquiringIndex(), 0);
    gl_pChinaStockMarket->SetSystemReady(false);
    EXPECT_TRUE(m_SinaRTWebInquiry.PrepareNextInquiringStr());
    CString str = m_SinaRTWebInquiry.GetInquiringString();
    EXPECT_STREQ(str.Left(25), _T("http://hq.sinajs.cn/list="));
  }

  TEST_F(CSinaRTWebInquiryTest, TestReportStatus) {
    EXPECT_TRUE(m_SinaRTWebInquiry.ReportStatus(1));
  }
}