#include"stdafx.h"
#include"pch.h"

#include"globedef.h"

#include"MockSystemMessage.h"
using namespace Testing;
using namespace testing;

namespace StockAnalysisTest {
  class SystemMessageMockTest : public ::testing::Test
  {
  protected:
    virtual void SetUp(void) override {
    }

    virtual void TearDown(void) override {
      // clearup
    }
    CMockSystemDeque systemDeque;
  };

  TEST_F(SystemMessageMockTest, TestDisplay) {
    COutputList outputList;
    CString strCmp = _T("20200101: first time test");
    systemDeque.PushMessage(_T("first time test"));
    EXPECT_EQ(systemDeque.GetDequeSize(), 1);
    EXPECT_CALL(systemDeque, SysCallOutputListAddString(&outputList, strCmp)).Times(1);
    systemDeque.Display(&outputList, _T("20200101"));
    EXPECT_EQ(systemDeque.GetDequeSize(), 0);
  }
}