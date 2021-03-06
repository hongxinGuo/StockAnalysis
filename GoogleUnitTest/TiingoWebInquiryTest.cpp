#include"pch.h"

#include"globedef.h"

#include"MockTiingoWebInquiry.h"

using namespace std;
using namespace testing;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace StockAnalysisTest {
  static bool m_fSystemStatus;
  class CTiingoWebInquiryTest : public ::testing::Test
  {
  protected:
    static void SetUpTestSuite(void) {
      m_fSystemStatus = gl_pWorldMarket->IsSystemReady();
      gl_pWorldMarket->SetSystemReady(true); // Tiingo引擎必须等待系统初始化后才可使用。
    }

    static void TearDownTestSuite(void) {
      gl_pWorldMarket->SetSystemReady(m_fSystemStatus);
    }

    virtual void SetUp(void) override {
      ASSERT_FALSE(gl_fNormalMode);
      gl_pWorldMarket->CalculateTime();
    }

    virtual void TearDown(void) override {
      // clearup
      gl_pWorldMarket->SetResetMarket(true);
      while (gl_systemMessage.GetInformationDequeSize() > 0) gl_systemMessage.PopInformationMessage();
    }
    CMockTiingoWebInquiry m_TiingoWebInquiry; // 网易日线历史数据
  };

  TEST_F(CTiingoWebInquiryTest, TestInitialize) {
    EXPECT_STREQ(m_TiingoWebInquiry.GetInquiringStringPrefix(), _T(""));
    EXPECT_STREQ(m_TiingoWebInquiry.GetInquiringStringSuffix(), _T("&token=fad87279362b9e580e4fb364a263cda3c67336c8"));
    EXPECT_STREQ(m_TiingoWebInquiry.GetConnectionName(), _T("Tiingo"));
  }

  TEST_F(CTiingoWebInquiryTest, TestGetWebData) {
    m_TiingoWebInquiry.SetReadingWebData(true);
    EXPECT_FALSE(m_TiingoWebInquiry.GetWebData());
    m_TiingoWebInquiry.SetReadingWebData(false);
    gl_pWorldMarket->SetSystemReady(true);
    EXPECT_CALL(m_TiingoWebInquiry, StartReadingThread)
      .Times(1);
    m_TiingoWebInquiry.GetWebData();
    EXPECT_TRUE(m_TiingoWebInquiry.IsReadingWebData()) << _T("此标志由工作线程负责重置。此处调用的是Mock类，故而此标识没有重置");
  }

  TEST_F(CTiingoWebInquiryTest, TestReportStatus) {
    EXPECT_TRUE(m_TiingoWebInquiry.ReportStatus(1));
  }

  TEST_F(CTiingoWebInquiryTest, TestPrepareNextInquiringStr) {
    CString str;
    gl_pWorldMarket->SetSystemReady(true);
    for (int i = 0; i < 4; i++) {
      if (m_TiingoWebInquiry.PrepareNextInquiringStr()) {
        str = m_TiingoWebInquiry.GetInquiringString();
        EXPECT_STREQ(str.Right(47), _T("&token=fad87279362b9e580e4fb364a263cda3c67336c8"));
      }
      else EXPECT_EQ(str.GetLength(), 0);
    }
    gl_pWorldMarket->SetSystemReady(false);
  }

  TEST_F(CTiingoWebInquiryTest, TestGetNextInquiringMiddleStr) {
    EXPECT_STREQ(m_TiingoWebInquiry.GetNextInquiringMiddleStr(), _T("")) << "此函数不执行任何动作";
  }
}