#include"stdafx.h"
#include"pch.h"

#include"globedef.h"

#include"PriorityQueueRTData.h"

namespace StockAnalysisTest {
  static CPriorityQueueRTData gl_PriorityQueueRTData;

  class CPriorityQueueRTDataTest : public ::testing::Test
  {
  protected:
    virtual void SetUp(void) override {
      ASSERT_FALSE(gl_fNormalMode);
      ASSERT_TRUE(gl_fTestMode);
    }

    virtual void TearDown(void) override {
      // clearup
      gl_PriorityQueueRTData.Reset();
    }
  };

  TEST_F(CPriorityQueueRTDataTest, TestReset) {
    CRTDataPtr pRTData = make_shared<CRTData>();
    pRTData->SetTransactionTime(1);
    gl_PriorityQueueRTData.PushRTData(pRTData);
    EXPECT_EQ(gl_PriorityQueueRTData.GetRTDataSize(), 1);
    gl_PriorityQueueRTData.Reset();
    EXPECT_EQ(gl_PriorityQueueRTData.GetRTDataSize(), 0);
  }

  TEST_F(CPriorityQueueRTDataTest, TestPushPopData) {
    CRTDataPtr pRTData = make_shared<CRTData>();
    pRTData->SetTransactionTime(1);
    CRTDataPtr pRTData2 = make_shared<CRTData>();
    pRTData2->SetTransactionTime(0);
    gl_PriorityQueueRTData.PushRTData(pRTData);
    gl_PriorityQueueRTData.PushRTData(pRTData2);
    CRTDataPtr pRTData3 = gl_PriorityQueueRTData.PopRTData();
    EXPECT_EQ(pRTData3->GetTransactionTime(), 0) << "������Ȩ�Ķ��У�����ʱ�����λ��ǰ��";
  }
}