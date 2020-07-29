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
#include"ChinaMarket.h"

#include"RTDataContainer.h"

namespace StockAnalysisTest {
  TEST(RTDataContainerTest, TestInitialize) {
    ASSERT_FALSE(gl_fNormalMode);
    size_t l = gl_systemMessage.GetInformationDequeSize();
    for (int i = 0; i < l; i++) {
      CString str = gl_systemMessage.PopInformationMessage(); // 清除信息队列
    }
    EXPECT_EQ(gl_systemMessage.GetInformationDequeSize(), 0);
  }

  // 测试有优先级的队列存储临时实时数据。
  TEST(RTDataContainerTest, TestGetSinaRTDataDuqueSize) {
    ASSERT_FALSE(gl_fNormalMode);
    EXPECT_EQ(gl_RTDataContainer.GetSinaRTDataSize(), 0);
    CWebRTDataPtr pRTData = make_shared<CWebRTData>();
    pRTData->SetTransactionTime(100100100);
    gl_RTDataContainer.PushSinaRTData(pRTData);
    CWebRTDataPtr pRTData2 = make_shared<CWebRTData>();
    pRTData2->SetTransactionTime(200200200);
    pRTData2->SetBuy(1);
    gl_RTDataContainer.PushSinaRTData(pRTData2);
    CWebRTDataPtr pRTData3 = make_shared<CWebRTData>();
    pRTData3->SetTransactionTime(200200);
    gl_RTDataContainer.PushSinaRTData(pRTData3);
    CWebRTDataPtr pRTData4 = make_shared<CWebRTData>();
    pRTData4->SetTransactionTime(200);
    gl_RTDataContainer.PushSinaRTData(pRTData4);
    CWebRTDataPtr pRTData5 = make_shared<CWebRTData>();
    pRTData5->SetTransactionTime(200200200);
    pRTData5->SetBuy(2);
    gl_RTDataContainer.PushSinaRTData(pRTData5);  // 这个与pRTData2的时间相同，应该位于pRTData2之后
    EXPECT_EQ(gl_RTDataContainer.GetSinaRTDataSize(), 5);
    CWebRTDataPtr p2 = gl_RTDataContainer.PopSinaRTData();
    EXPECT_EQ(gl_RTDataContainer.GetSinaRTDataSize(), 4);
    EXPECT_EQ(p2->GetTransactionTime(), 200);
    p2 = gl_RTDataContainer.PopSinaRTData();
    EXPECT_EQ(gl_RTDataContainer.GetSinaRTDataSize(), 3);
    EXPECT_EQ(p2->GetTransactionTime(), 200200);
    p2 = gl_RTDataContainer.PopSinaRTData();
    EXPECT_EQ(gl_RTDataContainer.GetSinaRTDataSize(), 2);
    EXPECT_EQ(p2->GetTransactionTime(), 100100100);
    p2 = gl_RTDataContainer.PopSinaRTData();
    EXPECT_EQ(gl_RTDataContainer.GetSinaRTDataSize(), 1);
    EXPECT_EQ(p2->GetTransactionTime(), 200200200);
    EXPECT_EQ(p2->GetBuy(), 1);
    p2 = gl_RTDataContainer.PopSinaRTData();
    EXPECT_EQ(gl_RTDataContainer.GetSinaRTDataSize(), 0);
    EXPECT_EQ(p2->GetBuy(), 2); // 后放入的相同时间的数据应该位于后面
    EXPECT_EQ(p2->GetTransactionTime(), 200200200);
  }

  TEST(RTDataContainerTest, TestGetTengxunRTDataDuqueSize) {
    ASSERT_FALSE(gl_fNormalMode);
    EXPECT_EQ(gl_RTDataContainer.GetTengxunRTDataSize(), 0);
    CWebRTDataPtr pRTData = make_shared<CWebRTData>();
    pRTData->SetTransactionTime(100100100);
    gl_RTDataContainer.PushTengxunRTData(pRTData);
    CWebRTDataPtr pRTData2 = make_shared<CWebRTData>();
    pRTData2->SetTransactionTime(200200200);
    pRTData2->SetBuy(1);
    gl_RTDataContainer.PushTengxunRTData(pRTData2);
    CWebRTDataPtr pRTData3 = make_shared<CWebRTData>();
    pRTData3->SetTransactionTime(200200);
    gl_RTDataContainer.PushTengxunRTData(pRTData3);
    CWebRTDataPtr pRTData4 = make_shared<CWebRTData>();
    pRTData4->SetTransactionTime(200);
    gl_RTDataContainer.PushTengxunRTData(pRTData4);
    CWebRTDataPtr pRTData5 = make_shared<CWebRTData>();
    pRTData5->SetTransactionTime(200200200);
    pRTData5->SetBuy(2);
    gl_RTDataContainer.PushTengxunRTData(pRTData5);  // 这个与pRTData2的时间相同，应该位于pRTData2之后
    EXPECT_EQ(gl_RTDataContainer.GetTengxunRTDataSize(), 5);
    CWebRTDataPtr p2 = gl_RTDataContainer.PopTengxunRTData();
    EXPECT_EQ(gl_RTDataContainer.GetTengxunRTDataSize(), 4);
    EXPECT_EQ(p2->GetTransactionTime(), 200);
    p2 = gl_RTDataContainer.PopTengxunRTData();
    EXPECT_EQ(gl_RTDataContainer.GetTengxunRTDataSize(), 3);
    EXPECT_EQ(p2->GetTransactionTime(), 200200);
    p2 = gl_RTDataContainer.PopTengxunRTData();
    EXPECT_EQ(gl_RTDataContainer.GetTengxunRTDataSize(), 2);
    EXPECT_EQ(p2->GetTransactionTime(), 100100100);
    p2 = gl_RTDataContainer.PopTengxunRTData();
    EXPECT_EQ(gl_RTDataContainer.GetTengxunRTDataSize(), 1);
    EXPECT_EQ(p2->GetTransactionTime(), 200200200);
    EXPECT_EQ(p2->GetBuy(), 1);
    p2 = gl_RTDataContainer.PopTengxunRTData();
    EXPECT_EQ(gl_RTDataContainer.GetTengxunRTDataSize(), 0);
    EXPECT_EQ(p2->GetBuy(), 2); // 后放入的相同时间的数据应该位于后面
    EXPECT_EQ(p2->GetTransactionTime(), 200200200);
  }

  TEST(RTDataContainerTest, TestGetNeteaseRTDataDuqueSize) {
    ASSERT_FALSE(gl_fNormalMode);
    EXPECT_EQ(gl_RTDataContainer.GetNeteaseRTDataSize(), 0);
    CWebRTDataPtr pRTData = make_shared<CWebRTData>();
    pRTData->SetTransactionTime(100100100);
    gl_RTDataContainer.PushNeteaseRTData(pRTData);
    CWebRTDataPtr pRTData2 = make_shared<CWebRTData>();
    pRTData2->SetTransactionTime(200200200);
    pRTData2->SetBuy(1);
    gl_RTDataContainer.PushNeteaseRTData(pRTData2);
    CWebRTDataPtr pRTData3 = make_shared<CWebRTData>();
    pRTData3->SetTransactionTime(200200);
    gl_RTDataContainer.PushNeteaseRTData(pRTData3);
    CWebRTDataPtr pRTData4 = make_shared<CWebRTData>();
    pRTData4->SetTransactionTime(200);
    gl_RTDataContainer.PushNeteaseRTData(pRTData4);
    CWebRTDataPtr pRTData5 = make_shared<CWebRTData>();
    pRTData5->SetTransactionTime(200200200);
    pRTData5->SetBuy(2);
    gl_RTDataContainer.PushNeteaseRTData(pRTData5);  // 这个与pRTData2的时间相同，应该位于pRTData2之后
    EXPECT_EQ(gl_RTDataContainer.GetNeteaseRTDataSize(), 5);
    CWebRTDataPtr p2 = gl_RTDataContainer.PopNeteaseRTData();
    EXPECT_EQ(gl_RTDataContainer.GetNeteaseRTDataSize(), 4);
    EXPECT_EQ(p2->GetTransactionTime(), 200);
    p2 = gl_RTDataContainer.PopNeteaseRTData();
    EXPECT_EQ(gl_RTDataContainer.GetNeteaseRTDataSize(), 3);
    EXPECT_EQ(p2->GetTransactionTime(), 200200);
    p2 = gl_RTDataContainer.PopNeteaseRTData();
    EXPECT_EQ(gl_RTDataContainer.GetNeteaseRTDataSize(), 2);
    EXPECT_EQ(p2->GetTransactionTime(), 100100100);
    p2 = gl_RTDataContainer.PopNeteaseRTData();
    EXPECT_EQ(gl_RTDataContainer.GetNeteaseRTDataSize(), 1);
    EXPECT_EQ(p2->GetTransactionTime(), 200200200);
    EXPECT_EQ(p2->GetBuy(), 1);
    p2 = gl_RTDataContainer.PopNeteaseRTData();
    EXPECT_EQ(gl_RTDataContainer.GetNeteaseRTDataSize(), 0);
    EXPECT_EQ(p2->GetBuy(), 2); // 后放入的相同时间的数据应该位于后面
    EXPECT_EQ(p2->GetTransactionTime(), 200200200);
  }
}