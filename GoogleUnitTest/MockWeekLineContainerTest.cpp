#include"stdafx.h"
#include"pch.h"
#include"globedef.h"

#include"MockWeekLineContainer.h"

using namespace testing;
using namespace testing;

namespace StockAnalysisTest {
  class CMockWeekLineContainerTest : public ::testing::Test
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
      pWeekLine = make_shared<CMockWeekLineContainer>();
    }

    virtual void TearDown(void) override {
      // clearup
    }

  protected:
    CMockWeekLineContainerPtr pWeekLine;
  };

  TEST_F(CMockWeekLineContainerTest, TestCalculateRS) {
    InSequence Seq;
    EXPECT_CALL(*pWeekLine, CalculateRS1(3))
      .Times(1);
    EXPECT_CALL(*pWeekLine, CalculateRS1(5))
      .Times(1);
    EXPECT_CALL(*pWeekLine, CalculateRS1(10))
      .Times(1);
    EXPECT_CALL(*pWeekLine, CalculateRS1(30))
      .Times(1);
    EXPECT_CALL(*pWeekLine, CalculateRS1(60))
      .Times(1);
    EXPECT_CALL(*pWeekLine, CalculateRS1(120))
      .Times(1);
    pWeekLine->CalculateRelativeStrong();
  }

  TEST_F(CMockWeekLineContainerTest, TestCalculateRSLogarithm) {
    InSequence Seq;
    EXPECT_CALL(*pWeekLine, CalculateRSLogarithm1(3))
      .Times(1);
    EXPECT_CALL(*pWeekLine, CalculateRSLogarithm1(5))
      .Times(1);
    EXPECT_CALL(*pWeekLine, CalculateRSLogarithm1(10))
      .Times(1);
    EXPECT_CALL(*pWeekLine, CalculateRSLogarithm1(30))
      .Times(1);
    EXPECT_CALL(*pWeekLine, CalculateRSLogarithm1(60))
      .Times(1)
      .WillOnce(Return(true));
    EXPECT_CALL(*pWeekLine, CalculateRSLogarithm1(120))
      .Times(1);
    pWeekLine->CalculateRelativeStrongLogarithm();
  }

  TEST_F(CMockWeekLineContainerTest, TestCalculateRSIndex) {
    InSequence Seq;
    EXPECT_CALL(*pWeekLine, CalculateRSIndex1(3))
      .Times(1);
    EXPECT_CALL(*pWeekLine, CalculateRSIndex1(5))
      .Times(1);
    EXPECT_CALL(*pWeekLine, CalculateRSIndex1(10))
      .Times(1);
    EXPECT_CALL(*pWeekLine, CalculateRSIndex1(30))
      .Times(1);
    EXPECT_CALL(*pWeekLine, CalculateRSIndex1(60))
      .Times(1);
    EXPECT_CALL(*pWeekLine, CalculateRSIndex1(120))
      .Times(1);
    pWeekLine->CalculateRelativeStrongIndex();
  }
}