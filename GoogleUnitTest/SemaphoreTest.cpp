#include"pch.h"

#include"Semaphore.h"

using namespace MyLib;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace StockAnalysisTest {
  Semaphore s_TestSemaphore(10);

  class SemaphoreTest : public ::testing::Test
  {
  protected:
    virtual void SetUp(void) override {
    }
    virtual void TearDown(void) override {
      s_TestSemaphore.SetMaxCount(10);
    }
  };

  TEST_F(SemaphoreTest, TestSetMaxCount) {
    EXPECT_EQ(s_TestSemaphore.GetMaxCount(), 10);
    s_TestSemaphore.SetMaxCount(5);
    EXPECT_EQ(s_TestSemaphore.GetMaxCount(), 5);
    s_TestSemaphore.SetMaxCount();
    EXPECT_EQ(s_TestSemaphore.GetMaxCount(), 1);
  }
}