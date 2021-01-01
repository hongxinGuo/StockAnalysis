#include"stdafx.h"
#include"pch.h"

#include"globedef.h"

#include"MockFinnhubWebInquiry.h"

using namespace std;
using namespace testing;

namespace StockAnalysisTest {
  class CFinnhubWebInquiryTest : public ::testing::Test
  {
  protected:
    static void SetUpTestSuite(void) {
    }

    static void TearDownTestSuite(void) {
    }

    virtual void SetUp(void) override {
      ASSERT_FALSE(gl_fNormalMode);
      gl_pAmericaMarket->CalculateTime();
    }

    virtual void TearDown(void) override {
      // clearup
      gl_pAmericaMarket->SetResetMarket(true);
      while (gl_systemMessage.GetInformationDequeSize() > 0) gl_systemMessage.PopInformationMessage();
      gl_pAmericaMarket->SetSystemReady(false);
    }
    CMockFinnhubWebInquiry m_FinnhubWebInquiry; // ����������ʷ����
  };

  TEST_F(CFinnhubWebInquiryTest, TestInitialize) {
    EXPECT_STREQ(m_FinnhubWebInquiry.GetInquiringStringPrefix(), _T(""));
#ifdef  DEBUG
    EXPECT_STREQ(m_FinnhubWebInquiry.GetInquiringStringSuffix(), _T("&token=bv4ac1n48v6tcp17l5cg"));
#else
    EXPECT_STREQ(m_FinnhubWebInquiry.GetInquiringStringSuffix(), _T("&token=bv985d748v6ujthqfke0"));
#endif //  DEBUG
    EXPECT_STREQ(m_FinnhubWebInquiry.GetConnection(), _T("Finnhub"));
  }

  TEST_F(CFinnhubWebInquiryTest, TestGetWebData) {
    m_FinnhubWebInquiry.SetReadingWebData(true);
    EXPECT_FALSE(m_FinnhubWebInquiry.GetWebData());
    m_FinnhubWebInquiry.SetReadingWebData(false);
    gl_pAmericaMarket->SetSystemReady(true);
    EXPECT_CALL(m_FinnhubWebInquiry, StartReadingThread)
      .Times(1);
    m_FinnhubWebInquiry.GetWebData();
    EXPECT_TRUE(m_FinnhubWebInquiry.IsReadingWebData()) << _T("�˱�־�ɹ����̸߳������á��˴����õ���Mock�࣬�ʶ��˱�ʶû������");
  }

  TEST_F(CFinnhubWebInquiryTest, TestReportStatus) {
    EXPECT_TRUE(m_FinnhubWebInquiry.ReportStatus(1));
  }

  TEST_F(CFinnhubWebInquiryTest, TestPrepareNextInquiringStr) {
    CString str;
    gl_pAmericaMarket->SetSystemReady(true);
    for (int i = 0; i < 4; i++) {
      if (m_FinnhubWebInquiry.PrepareNextInquiringStr()) {
        str = m_FinnhubWebInquiry.GetInquiringString();
#ifdef  DEBUG
        EXPECT_STREQ(str.Right(27), _T("&token=bv4ac1n48v6tcp17l5cg"));
#else
        EXPECT_STREQ(str.Right(27), _T("&token=bv985d748v6ujthqfke0"));
#endif //  DEBUG
      }
      else EXPECT_EQ(str.GetLength(), 0);
    }
    gl_pAmericaMarket->SetSystemReady(false);
  }
}