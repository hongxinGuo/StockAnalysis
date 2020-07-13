#include"stdafx.h"
#include"pch.h"

#include"globedef.h"
#include"ChinaMarket.h"

#include"MockStockAnalysisView.h"
using namespace testing;

namespace StockAnalysisTest {
  class CMockStockAnalysisViewTest : public ::testing::Test {
  public:
    static void SetUpTestSuite(void) {
      EXPECT_FALSE(gl_fNormalMode);
      EXPECT_TRUE(gl_fTestMode);
    }

    static void TearDownTestSuite(void) {
      EXPECT_FALSE(gl_fNormalMode);
      EXPECT_TRUE(gl_fTestMode);
    }
    virtual void SetUp(void) override {
      s_pStockAnalysisView = new CMockStockAnalysisView;
      EXPECT_EQ(s_pStockAnalysisView->GetClientSize().Height(), 0);
      EXPECT_EQ(s_pStockAnalysisView->GetClientSize().Width(), 0);
      EXPECT_FALSE(s_pStockAnalysisView->IsShowRS());
      EXPECT_FALSE(s_pStockAnalysisView->IsShow3DayRS());
      EXPECT_FALSE(s_pStockAnalysisView->IsShow5DayRS());
      EXPECT_TRUE(s_pStockAnalysisView->IsShow10DayRS());
      EXPECT_TRUE(s_pStockAnalysisView->IsShow30DayRS());
      EXPECT_FALSE(s_pStockAnalysisView->IsShow60DayRS());
      EXPECT_FALSE(s_pStockAnalysisView->IsShow120DayRS());
      EXPECT_TRUE(s_pStockAnalysisView->IsShowRSInIndex());
      EXPECT_FALSE(s_pStockAnalysisView->IsShowRSInLinear());
    }
    virtual void TearDown(void) override {
      gl_pChinaStockMarket->ResetCurrentStock();
      gl_pChinaStockMarket->SetCurrentStockChanged(false);
      delete s_pStockAnalysisView;
    }
  public:
    CMockStockAnalysisView* s_pStockAnalysisView;
  };

  TEST_F(CMockStockAnalysisViewTest, TestGetCurrentShowType) {
    EXPECT_EQ(s_pStockAnalysisView->GetCurrentShowType(), 1) << _T("��ʼ״̬Ϊ��ʾ���ߣ�1��");
    s_pStockAnalysisView->SetCurrentShowType(2);
    EXPECT_EQ(s_pStockAnalysisView->GetCurrentShowType(), 2);
    s_pStockAnalysisView->SetCurrentShowType(1);
  }

  TEST_F(CMockStockAnalysisViewTest, TestShowOrdinaryBuySell) {
    CDC DC;
    CChinaStockPtr pStock = gl_pChinaStockMarket->GetStock(1);
    CRect rectArea(0, 0, 300, 300);

    pStock->SetVolume(10000);
    EXPECT_CALL(*s_pStockAnalysisView, SysCallFillSolidRect(&DC, _, _))
      .Times(14);

    s_pStockAnalysisView->ShowOrdinaryBuySell(&DC, pStock, rectArea);
  }

  TEST_F(CMockStockAnalysisViewTest, TestShowCanceledBuySell) {
    CDC DC;
    CChinaStockPtr pStock = gl_pChinaStockMarket->GetStock(1);
    CRect rectArea(0, 0, 300, 300);

    pStock->SetCanceledBuyVolume(10000);
    EXPECT_CALL(*s_pStockAnalysisView, SysCallFillSolidRect(&DC, _, _))
      .Times(14);

    s_pStockAnalysisView->ShowCanceledBuySell(&DC, pStock, rectArea);
  }

  TEST_F(CMockStockAnalysisViewTest, TestShowCurrentRS) {
    CDC DC;
    vector<double> vRS{ 5, 10, 20, 30 };
    CRect rect(0, 0, 100, 100);
    s_pStockAnalysisView->SetClientSize(rect);
    EXPECT_CALL(*s_pStockAnalysisView, SysCallMoveTo(&DC, 99, 85))
      .Times(1);
    EXPECT_CALL(*s_pStockAnalysisView, SysCallLineTo(&DC, _, _))
      .Times(2);
    s_pStockAnalysisView->ShowCurrentRS(&DC, vRS);
  }

  TEST_F(CMockStockAnalysisViewTest, TestRSLineTo) {
    CDC DC;
    CRect rect(0, 0, 100, 100);
    s_pStockAnalysisView->SetClientSize(rect);
    EXPECT_CALL(*s_pStockAnalysisView, SysCallLineTo(&DC, 69, 75))
      .Times(1)
      .WillOnce(Return(TRUE));
    EXPECT_TRUE(s_pStockAnalysisView->RSLineTo(&DC, 10, 50.0, 100));

    EXPECT_CALL(*s_pStockAnalysisView, SysCallLineTo(&DC, -21, 75))
      .Times(1)
      .WillOnce(Return(TRUE));
    EXPECT_FALSE(s_pStockAnalysisView->RSLineTo(&DC, 40, 50.0, 100));

    EXPECT_CALL(*s_pStockAnalysisView, SysCallLineTo(&DC, 39, 75))
      .Times(1)
      .WillOnce(Return(TRUE));
    EXPECT_FALSE(s_pStockAnalysisView->RSLineTo(&DC, 20, 50.0, 50));
  }

  TEST_F(CMockStockAnalysisViewTest, TestOnSize) {
    EXPECT_CALL(*s_pStockAnalysisView, SysCallOnSize(1, 100, 200))
      .Times(1);
    s_pStockAnalysisView->OnSize(1, 100, 200);
    CRect rect = s_pStockAnalysisView->GetClientSize();
    EXPECT_EQ(rect.Height(), 200);
    EXPECT_EQ(rect.Width(), 100);
  }

  TEST_F(CMockStockAnalysisViewTest, TestOnUpdateShowRs3) {
    CCmdUI cmdUI;
    EXPECT_FALSE(s_pStockAnalysisView->IsShow3DayRS());
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUISetCheck(_, 0))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowRs3(&cmdUI);
    s_pStockAnalysisView->OnShowRs3();
    EXPECT_TRUE(s_pStockAnalysisView->IsShow3DayRS());
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUISetCheck(_, 1))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowRs3(&cmdUI);
    s_pStockAnalysisView->OnShowRs3();
    EXPECT_FALSE(s_pStockAnalysisView->IsShow3DayRS());
  }

  TEST_F(CMockStockAnalysisViewTest, TestOnUpdateShowRs5) {
    CCmdUI cmdUI;
    EXPECT_FALSE(s_pStockAnalysisView->IsShow5DayRS());
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUISetCheck(_, 0))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowRs5(&cmdUI);
    s_pStockAnalysisView->OnShowRs5();
    EXPECT_TRUE(s_pStockAnalysisView->IsShow5DayRS());
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUISetCheck(_, 1))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowRs5(&cmdUI);
    s_pStockAnalysisView->OnShowRs5();
    EXPECT_FALSE(s_pStockAnalysisView->IsShow5DayRS());
  }

  TEST_F(CMockStockAnalysisViewTest, TestOnUpdateShowRs10) {
    CCmdUI cmdUI;
    EXPECT_TRUE(s_pStockAnalysisView->IsShow10DayRS());
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUISetCheck(_, 1))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowRs10(&cmdUI);
    s_pStockAnalysisView->OnShowRs10();
    EXPECT_FALSE(s_pStockAnalysisView->IsShow10DayRS());
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUISetCheck(_, 0))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowRs10(&cmdUI);
    s_pStockAnalysisView->OnShowRs10();
    EXPECT_TRUE(s_pStockAnalysisView->IsShow10DayRS());
  }

  TEST_F(CMockStockAnalysisViewTest, TestOnUpdateShowRs30) {
    CCmdUI cmdUI;
    EXPECT_TRUE(s_pStockAnalysisView->IsShow30DayRS());
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUISetCheck(_, 1))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowRs30(&cmdUI);
    s_pStockAnalysisView->OnShowRs30();
    EXPECT_FALSE(s_pStockAnalysisView->IsShow30DayRS());
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUISetCheck(_, 0))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowRs30(&cmdUI);
    s_pStockAnalysisView->OnShowRs30();
    EXPECT_TRUE(s_pStockAnalysisView->IsShow30DayRS());
  }

  TEST_F(CMockStockAnalysisViewTest, TestOnUpdateShowRs60) {
    CCmdUI cmdUI;
    EXPECT_FALSE(s_pStockAnalysisView->IsShow60DayRS());
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUISetCheck(_, 0))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowRs60(&cmdUI);
    s_pStockAnalysisView->OnShowRs60();
    EXPECT_TRUE(s_pStockAnalysisView->IsShow60DayRS());
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUISetCheck(_, 1))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowRs60(&cmdUI);
    s_pStockAnalysisView->OnShowRs60();
    EXPECT_FALSE(s_pStockAnalysisView->IsShow60DayRS());
  }

  TEST_F(CMockStockAnalysisViewTest, TestOnUpdateShowRs120) {
    CCmdUI cmdUI;
    EXPECT_FALSE(s_pStockAnalysisView->IsShow120DayRS());
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUISetCheck(_, 0))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowRs120(&cmdUI);
    s_pStockAnalysisView->OnShowRs120();
    EXPECT_TRUE(s_pStockAnalysisView->IsShow120DayRS());
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUISetCheck(_, 1))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowRs120(&cmdUI);
    s_pStockAnalysisView->OnShowRs120();
    EXPECT_FALSE(s_pStockAnalysisView->IsShow120DayRS());
  }

  TEST_F(CMockStockAnalysisViewTest, TestOnUpdateShowRSInLogarithm) {
    CCmdUI cmdUI;
    gl_pChinaStockMarket->ResetCurrentStock();
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUIEnable(_, false))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowRsInLogarithm(&cmdUI);

    gl_pChinaStockMarket->SetCurrentStock(_T("sh600000"));
    EXPECT_TRUE(gl_pChinaStockMarket->GetCurrentStock() != nullptr);
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUIEnable(_, true))
      .Times(1);
    EXPECT_FALSE(s_pStockAnalysisView->IsShowRSInLogarithm());
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUISetCheck(_, 0))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowRsInLogarithm(&cmdUI);

    s_pStockAnalysisView->OnShowRsInLogarithm();
    EXPECT_TRUE(s_pStockAnalysisView->IsShowRSInLogarithm());
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUIEnable(_, true))
      .Times(1);
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUISetCheck(_, 1))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowRsInLogarithm(&cmdUI);
    s_pStockAnalysisView->OnShowRsInLinear();

    EXPECT_FALSE(s_pStockAnalysisView->IsShowRSInLogarithm());
    EXPECT_FALSE(s_pStockAnalysisView->IsShowRSInIndex());
    EXPECT_TRUE(s_pStockAnalysisView->IsShowRSInLinear());
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUIEnable(_, true))
      .Times(1);
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUISetCheck(_, 1))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowRsInLinear(&cmdUI);
    s_pStockAnalysisView->OnShowRsIndex();
  }

  TEST_F(CMockStockAnalysisViewTest, TestOnUpdateShowRSInLinear) {
    CCmdUI cmdUI;
    gl_pChinaStockMarket->ResetCurrentStock();
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUIEnable(_, false))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowRsInLinear(&cmdUI);
    gl_pChinaStockMarket->SetCurrentStock(_T("sh600000"));
    EXPECT_TRUE(gl_pChinaStockMarket->GetCurrentStock() != nullptr);
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUIEnable(_, true))
      .Times(1);
    EXPECT_FALSE(s_pStockAnalysisView->IsShowRSInLinear());
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUISetCheck(_, 0))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowRsInLinear(&cmdUI);
    s_pStockAnalysisView->OnShowRsInLinear();
    EXPECT_TRUE(s_pStockAnalysisView->IsShowRSInLinear());
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUIEnable(_, true))
      .Times(1);
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUISetCheck(_, 1))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowRsInLinear(&cmdUI);
    s_pStockAnalysisView->OnShowRsIndex();
    EXPECT_FALSE(s_pStockAnalysisView->IsShowRSInLinear());
  }

  TEST_F(CMockStockAnalysisViewTest, TestOnUpdateShowRSIndex) {
    CCmdUI cmdUI;
    gl_pChinaStockMarket->ResetCurrentStock();
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUIEnable(_, false))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowRsIndex(&cmdUI);
    gl_pChinaStockMarket->SetCurrentStock(_T("sh600000"));
    s_pStockAnalysisView->SetShowRSOption(2);
    EXPECT_TRUE(gl_pChinaStockMarket->GetCurrentStock() != nullptr);
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUIEnable(_, true))
      .Times(1);
    EXPECT_FALSE(s_pStockAnalysisView->IsShowRSInIndex());
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUISetCheck(_, 0))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowRsIndex(&cmdUI);
    s_pStockAnalysisView->OnShowRsIndex();
    EXPECT_TRUE(s_pStockAnalysisView->IsShowRSInIndex());
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUIEnable(_, true))
      .Times(1);
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUISetCheck(_, 1))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowRsIndex(&cmdUI);
    s_pStockAnalysisView->OnShowRsInLinear();
    EXPECT_FALSE(s_pStockAnalysisView->IsShowRSInIndex());
  }

  TEST_F(CMockStockAnalysisViewTest, TestOnShowDayLine) {
    s_pStockAnalysisView->SetCurrentShowType(2);
    s_pStockAnalysisView->OnShowDayLine();
    EXPECT_EQ(s_pStockAnalysisView->GetCurrentShowType(), 1);
  }

  TEST_F(CMockStockAnalysisViewTest, TestOnShowRealTime) {
    EXPECT_EQ(s_pStockAnalysisView->GetCurrentShowType(), 1);
    s_pStockAnalysisView->OnShowRealTime();
    EXPECT_EQ(s_pStockAnalysisView->GetCurrentShowType(), 2);

    s_pStockAnalysisView->SetCurrentShowType(1);
  }

  TEST_F(CMockStockAnalysisViewTest, TestOnUpdateShowDayLine) {
    CCmdUI cmdUI;
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUISetCheck(_, 1))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowDayLine(&cmdUI);
    s_pStockAnalysisView->SetCurrentShowType(2);
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUISetCheck(_, 0))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowDayLine(&cmdUI);

    s_pStockAnalysisView->SetCurrentShowType(1);
  }

  TEST_F(CMockStockAnalysisViewTest, TestOnUpdateShowRealTime) {
    CCmdUI cmdUI;
    s_pStockAnalysisView->SetCurrentShowType(1);
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUISetCheck(_, 0))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowRealTime(&cmdUI);
    s_pStockAnalysisView->SetCurrentShowType(2);
    EXPECT_CALL(*s_pStockAnalysisView, SysCallCmdUISetCheck(_, 1))
      .Times(1);
    s_pStockAnalysisView->OnUpdateShowRealTime(&cmdUI);
  }

  TEST_F(CMockStockAnalysisViewTest, TestOnShow) {
    CDC dc;
    gl_pChinaStockMarket->SetCurrentStock(_T("sh600000"));
    gl_pChinaStockMarket->GetCurrentStock()->SetDayLineLoaded(true);
    s_pStockAnalysisView->SetCurrentShowType(1);
    EXPECT_CALL(*s_pStockAnalysisView, SysCallGetClientRect(_))
      .Times(1);
    EXPECT_CALL(*s_pStockAnalysisView, ShowStockDayLine(_))
      .Times(1);
    s_pStockAnalysisView->Show(&dc);
    EXPECT_CALL(*s_pStockAnalysisView, SysCallGetClientRect(_))
      .Times(1);
    EXPECT_CALL(*s_pStockAnalysisView, ShowRealtimeData(_))
      .Times(1);
    s_pStockAnalysisView->SetCurrentShowType(2);
    s_pStockAnalysisView->Show(&dc);

    gl_pChinaStockMarket->GetCurrentStock()->SetDayLineLoaded(false);
  }
}