#include"stdafx.h"
#include"pch.h"

#include"globedef.h"

#include"QueueWebData.h"

namespace StockAnalysisTest {
  static CQueueWebData gl_QueueWebData;

  class CQueueWebDataTest : public ::testing::Test
  {
  protected:
    virtual void SetUp(void) override {
      ASSERT_FALSE(gl_fNormalMode);
      ASSERT_TRUE(gl_fTestMode);
    }

    virtual void TearDown(void) override {
      // clearup
      gl_QueueWebData.Reset();
    }
  };

  TEST_F(CQueueWebDataTest, TestReset) {
    CWebDataPtr pWebData = make_shared<CWebData>();
    pWebData->m_lTime = 1;
    gl_QueueWebData.PushWebData(pWebData);
    EXPECT_EQ(gl_QueueWebData.GetWebDataSize(), 1);
    gl_QueueWebData.Reset();
    EXPECT_EQ(gl_QueueWebData.GetWebDataSize(), 0);
  }

  TEST_F(CQueueWebDataTest, TestPushPopData) {
    CWebDataPtr pWebData = make_shared<CWebData>();
    pWebData->m_lTime = 1;
    CWebDataPtr pWebData2 = make_shared<CWebData>();
    pWebData2->m_lTime = 0;
    gl_QueueWebData.PushWebData(pWebData);
    gl_QueueWebData.PushWebData(pWebData2);
    CWebDataPtr pWebData3 = gl_QueueWebData.PopWebData();
    EXPECT_EQ(pWebData3->m_lTime, 1) << "������Ȩ�Ķ��У��뽻��ʱ���޹أ��������е��Ⱥ�˳�������";
  }
}