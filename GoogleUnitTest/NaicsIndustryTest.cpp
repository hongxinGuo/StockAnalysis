#include"pch.h"
#include"globedef.h"

#include"NaicsIndustry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace StockAnalysisTest {
  class CNaicsIndustryTest : public ::testing::Test
  {
  protected:
    static void SetUpTestSuite(void) { // 本测试类的初始化函数
      ASSERT_FALSE(gl_fNormalMode);
    }

    static void TearDownTestSuite() {
    }

    virtual void SetUp(void) override {
      ASSERT_FALSE(gl_fNormalMode);
      ASSERT_TRUE(gl_fTestMode);
    }

    virtual void TearDown(void) override {
      // clearup
    }
  };

  TEST_F(CNaicsIndustryTest, TestInitialize) {
    CNaicsIndustry NaicsIndustry;
    EXPECT_STREQ(NaicsIndustry.m_strNaics, _T(" "));
    EXPECT_STREQ(NaicsIndustry.m_strNationalIndustry, _T(" "));
    EXPECT_STREQ(NaicsIndustry.m_strSector, _T(" "));
    EXPECT_STREQ(NaicsIndustry.m_strSubSector, _T(" "));
    EXPECT_FALSE(NaicsIndustry.m_fUpdated);
  }

  TEST_F(CNaicsIndustryTest, TestIsUpdated) {
    CNaicsIndustry finnhubExchange;

    EXPECT_FALSE(finnhubExchange.IsUpdated());
    finnhubExchange.SetUpdated(true);
    EXPECT_TRUE(finnhubExchange.IsUpdated());
  }

  TEST_F(CNaicsIndustryTest, TestAppend) {
    CSetNaicsIndustry setNaicsIndustry, setNaicsIndustry2;
    CNaicsIndustry NaicsIndustry, NaicsIndustry2;

    NaicsIndustry.m_strNaics = _T("AA");
    NaicsIndustry.m_strNationalIndustry = _T("aaa");
    NaicsIndustry.m_strSector = _T("abdc");
    NaicsIndustry.m_strSubSector = _T("Beijing");
    NaicsIndustry.m_fUpdated = true;

    ASSERT(!gl_fNormalMode);
    setNaicsIndustry.Open();
    setNaicsIndustry.m_pDatabase->BeginTrans();
    NaicsIndustry.Append(setNaicsIndustry);
    setNaicsIndustry.m_pDatabase->CommitTrans();
    setNaicsIndustry.Close();

    setNaicsIndustry2.m_strFilter = _T("[Naics] = 'AA'");
    setNaicsIndustry2.Open();
    setNaicsIndustry2.m_pDatabase->BeginTrans();
    EXPECT_TRUE(!setNaicsIndustry2.IsEOF()) << "此时已经存入了AA";
    NaicsIndustry2.Load(setNaicsIndustry2);
    EXPECT_STREQ(NaicsIndustry.m_strNaics, _T("AA"));
    EXPECT_STREQ(NaicsIndustry.m_strNationalIndustry, _T("aaa"));
    EXPECT_STREQ(NaicsIndustry.m_strSector, _T("abdc"));
    EXPECT_STREQ(NaicsIndustry.m_strSubSector, _T("Beijing"));
    EXPECT_TRUE(NaicsIndustry.m_fUpdated);
    setNaicsIndustry2.Delete();
    setNaicsIndustry2.m_pDatabase->CommitTrans();
    setNaicsIndustry2.Close();
  }
}