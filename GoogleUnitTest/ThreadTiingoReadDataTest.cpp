#include"pch.h"

#include"globedef.h"
#include"WebInquirer.h"

#include"thread.h"

using namespace testing;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace StockAnalysisTest {
  class CThreadReadTiingoDataTest : public ::testing::Test
  {
  protected:
    static void SetUpTestSuite(void) {
    }

    static void TearDownTestSuite(void) {
    }

    virtual void SetUp(void) override {
    }

    virtual void TearDown(void) override {
    }
    CMockTiingoWebInquiry TiingoWebInquiry;
  };

  TEST_F(CThreadReadTiingoDataTest, TestThreadReadTiingoData) {
    int iCreatingThread = gl_ThreadStatus.GetNumberOfWebInquiringThread();

    gl_pWorldMarket->SetTiingoDataReceived(false);
    EXPECT_CALL(TiingoWebInquiry, ReadWebData())
      .Times(1)
      .WillOnce(Return(false));
    TiingoWebInquiry.__TESTSetBuffer(_T("testData"));
    EXPECT_EQ(ThreadReadTiingoData(&TiingoWebInquiry), (UINT)8);
    EXPECT_EQ(gl_ThreadStatus.GetNumberOfWebInquiringThread(), iCreatingThread);
    EXPECT_EQ(gl_WebInquirer.GetTiingoDataSize(), 0);

    gl_pWorldMarket->SetTiingoDataReceived(false);
    EXPECT_CALL(TiingoWebInquiry, ReadWebData())
      .Times(1)
      .WillOnce(Return(true));
    TiingoWebInquiry.__TESTSetBuffer(_T("testData"));
    EXPECT_EQ(ThreadReadTiingoData(&TiingoWebInquiry), (UINT)8);
    EXPECT_EQ(gl_ThreadStatus.GetNumberOfWebInquiringThread(), iCreatingThread);
    EXPECT_EQ(gl_WebInquirer.GetTiingoDataSize(), 1);
    CWebDataPtr pWebData = gl_WebInquirer.PopTiingoData();
    EXPECT_EQ(pWebData->GetBufferLength(), 8);
    char buffer[30];
    int i = 0;
    while ((i < 30) && (pWebData->GetData(i) != 0x000)) {
      buffer[i] = pWebData->GetData(i);
      i++;
    }
    buffer[i] = 0x000;
    CString str = buffer;
    EXPECT_STREQ(str, _T("testData"));
  }
}