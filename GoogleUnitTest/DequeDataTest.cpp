//////////////////////////////////////////////////////////////////////////////////////////
//
// 系统队列数据中的各个函数皆采用的同步机制以防止线程之间出现数据不同步问题，但此下的测试函数没有测试这种同步机制，
// 只是简单的测试了内部逻辑。
//
//
///////////////////////////////////////////////////////////////////////////////////////////

#include"stdafx.h"
#include"pch.h"

#include"globedef.h"
#include"Market.h"

#include"QueueRTData.h"

namespace StockAnalysisTest {
  TEST(DequeDataTest, TestInitialize) {
    ASSERT_FALSE(gl_fNormalMode);
    long l = gl_systemMessage.GetInformationDequeSize();
    for (int i = 0; i < l; i++) {
      CString str = gl_systemMessage.PopInformationMessage(); // 清除信息队列
    }
    EXPECT_EQ(gl_systemMessage.GetInformationDequeSize(), 0);
  }

  // 测试有优先级的队列存储临时实时数据。
  TEST(DequeDataTest, TestGetRTDataDuqueSize) {
    ASSERT_FALSE(gl_fNormalMode);
    EXPECT_EQ(gl_queueRTData.GetSinaRTDataSize(), 0);
    CRTDataPtr pRTData = make_shared<CRTData>();
    pRTData->SetTransactionTime(100100100);
    gl_queueRTData.PushSinaRTData(pRTData);
    CRTDataPtr pRTData2 = make_shared<CRTData>();
    pRTData2->SetTransactionTime(200200200);
    pRTData2->SetBuy(1);
    gl_queueRTData.PushSinaRTData(pRTData2);
    CRTDataPtr pRTData3 = make_shared<CRTData>();
    pRTData3->SetTransactionTime(200200);
    gl_queueRTData.PushSinaRTData(pRTData3);
    CRTDataPtr pRTData4 = make_shared<CRTData>();
    pRTData4->SetTransactionTime(200);
    gl_queueRTData.PushSinaRTData(pRTData4);
    CRTDataPtr pRTData5 = make_shared<CRTData>();
    pRTData5->SetTransactionTime(200200200);
    pRTData5->SetBuy(2);
    gl_queueRTData.PushSinaRTData(pRTData5);  // 这个与pRTData2的时间相同，应该位于pRTData2之后
    EXPECT_EQ(gl_queueRTData.GetSinaRTDataSize(), 5);
    CRTDataPtr p2 = gl_queueRTData.PopSinaRTData();
    EXPECT_EQ(gl_queueRTData.GetSinaRTDataSize(), 4);
    EXPECT_EQ(p2->GetTransactionTime(), 200);
    p2 = gl_queueRTData.PopSinaRTData();
    EXPECT_EQ(gl_queueRTData.GetSinaRTDataSize(), 3);
    EXPECT_EQ(p2->GetTransactionTime(), 200200);
    p2 = gl_queueRTData.PopSinaRTData();
    EXPECT_EQ(gl_queueRTData.GetSinaRTDataSize(), 2);
    EXPECT_EQ(p2->GetTransactionTime(), 100100100);
    p2 = gl_queueRTData.PopSinaRTData();
    EXPECT_EQ(gl_queueRTData.GetSinaRTDataSize(), 1);
    EXPECT_EQ(p2->GetTransactionTime(), 200200200);
    EXPECT_EQ(p2->GetBuy(), 1);
    p2 = gl_queueRTData.PopSinaRTData();
    EXPECT_EQ(gl_queueRTData.GetSinaRTDataSize(), 0);
    EXPECT_EQ(p2->GetBuy(), 2); // 后放入的相同时间的数据应该位于后面
    EXPECT_EQ(p2->GetTransactionTime(), 200200200);
  }
}