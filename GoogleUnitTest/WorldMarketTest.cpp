#include"pch.h"
#include"globedef.h"
#include"WebInquirer.h"

#include"WorldStock.h"
#include"WorldMarket.h"

#include"SetFinnhubForexExchange.h"

using namespace testing;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace StockAnalysisTest {
	TEST(WebInquiryTest, TestPriority) {
		WebInquiry inquiry, inquiry2, inquiry3;
		priority_queue<WebInquiry, vector<WebInquiry>, WebInquiry> qInquiry;

		inquiry.m_iPriority = 10;
		inquiry2.m_iPriority = 20;
		qInquiry.push(inquiry);
		qInquiry.push(inquiry2);
		inquiry3 = qInquiry.top();
		EXPECT_EQ(inquiry3.m_iPriority, 20) << "按优先级排列";
	}

	class CWorldMarketTest : public ::testing::Test
	{
	protected:
		static void SetUpTestSuite(void) {
			gl_pWorldMarket->SetFinnhubInquiring(false);
			gl_pWorldMarket->SetCountryListUpdated(false);
			gl_pWorldMarket->SetFinnhubSymbolUpdated(false);
			gl_pWorldMarket->SetFinnhubStockProfileUpdated(false);
			gl_pWorldMarket->SetFinnhubDayLineUpdated(false);
			gl_pWorldMarket->SetFinnhubPeerUpdated(false);
			gl_pWorldMarket->SetFinnhubEconomicCalendarUpdated(false);
			gl_pWorldMarket->SetFinnhubEPSSurpriseUpdated(true);
			gl_pWorldMarket->SetFinnhubForexExchangeUpdated(false);
			gl_pWorldMarket->SetFinnhubForexSymbolUpdated(false);
			gl_pWorldMarket->SetFinnhubForexDayLineUpdated(false);

			EXPECT_THAT(gl_systemMessage.GetInformationDequeSize(), 0);
		}
		static void TearDownTestSuite(void) {
			EXPECT_THAT(gl_systemMessage.GetInformationDequeSize(), 0);
		}
		virtual void SetUp(void) override {
		}

		virtual void TearDown(void) override {
			// clearup
			gl_pWorldMarket->SetFinnhubInquiring(false);
			gl_pWorldMarket->SetCountryListUpdated(false);
			gl_pWorldMarket->SetFinnhubSymbolUpdated(false);
			gl_pWorldMarket->SetFinnhubStockProfileUpdated(false);
			gl_pWorldMarket->SetFinnhubDayLineUpdated(false);
			gl_pWorldMarket->SetFinnhubPeerUpdated(false);
			gl_pWorldMarket->SetFinnhubEconomicCalendarUpdated(false);
			gl_pWorldMarket->SetFinnhubEPSSurpriseUpdated(true);
			gl_pWorldMarket->SetFinnhubForexExchangeUpdated(false);
			gl_pWorldMarket->SetFinnhubForexSymbolUpdated(false);
			gl_pWorldMarket->SetFinnhubForexDayLineUpdated(false);

			gl_pWorldMarket->SetTiingoInquiring(false);
			gl_pWorldMarket->SetTiingoDayLineUpdated(false);

			while (gl_systemMessage.GetInformationDequeSize() > 0) gl_systemMessage.PopInformationMessage();
			while (gl_systemMessage.GetDayLineInfoDequeSize() > 0) gl_systemMessage.PopDayLineInfoMessage();
			while (gl_systemMessage.GetInnerSystemInformationDequeSize() > 0) gl_systemMessage.PopInnerSystemInformationMessage();
		}

	protected:
	};

	TEST_F(CWorldMarketTest, TestTaskResetMarket) {
		gl_pWorldMarket->SetSystemReady(true);
		gl_pWorldMarket->SetPermitResetMarket(false);
		EXPECT_FALSE(gl_pWorldMarket->TaskResetMarket(101010)) << "不允许重置系统时，永远返回假";

		gl_pWorldMarket->SetPermitResetMarket(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskResetMarket(170000));
		gl_pWorldMarket->SetPermitResetMarket(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskResetMarket(170101));
		gl_pWorldMarket->SetPermitResetMarket(true);
		EXPECT_TRUE(gl_pWorldMarket->TaskResetMarket(170001));
		gl_pWorldMarket->SetPermitResetMarket(true);
		EXPECT_TRUE(gl_pWorldMarket->TaskResetMarket(170100));
		EXPECT_FALSE(gl_pWorldMarket->IsSystemReady());
		EXPECT_FALSE(gl_pWorldMarket->IsPermitResetMarket());
		EXPECT_TRUE(gl_pWorldMarket->IsResetMarket());

		gl_pWorldMarket->SetPermitResetMarket(true);
		gl_pWorldMarket->SetSystemReady(true);
		gl_pWorldMarket->SetResetMarket(false);
	}

	TEST_F(CWorldMarketTest, TestGetCurrentFinnhubInquiry) {
		WebInquiry inquiry, inquiry2;
		inquiry.m_iPriority = 10;
		inquiry.m_lInquiryIndex = 20;
		inquiry.m_lStockIndex = 30;
		gl_pWorldMarket->SetCurrentFinnhubInquiry(inquiry);
		inquiry2 = gl_pWorldMarket->GetCurrentFinnhubInquiry();
		EXPECT_EQ(inquiry.m_iPriority, inquiry2.m_iPriority);
		EXPECT_EQ(inquiry.m_lInquiryIndex, inquiry2.m_lInquiryIndex);
		EXPECT_EQ(inquiry.m_lStockIndex, inquiry2.m_lStockIndex);
	}

	TEST_F(CWorldMarketTest, TestGetCurrentTiingoInquiry) {
		WebInquiry inquiry, inquiry2;
		inquiry.m_iPriority = 10;
		inquiry.m_lInquiryIndex = 20;
		inquiry.m_lStockIndex = 30;
		gl_pWorldMarket->SetCurrentTiingoInquiry(inquiry);
		inquiry2 = gl_pWorldMarket->GetCurrentTiingoInquiry();
		EXPECT_EQ(inquiry.m_iPriority, inquiry2.m_iPriority);
		EXPECT_EQ(inquiry.m_lInquiryIndex, inquiry2.m_lInquiryIndex);
		EXPECT_EQ(inquiry.m_lStockIndex, inquiry2.m_lStockIndex);
	}

	TEST_F(CWorldMarketTest, TestGetCurrentQuandlInquiry) {
		WebInquiry inquiry, inquiry2;
		inquiry.m_iPriority = 10;
		inquiry.m_lInquiryIndex = 20;
		inquiry.m_lStockIndex = 30;
		gl_pWorldMarket->SetCurrentQuandlInquiry(inquiry);
		inquiry2 = gl_pWorldMarket->GetCurrentQuandlInquiry();
		EXPECT_EQ(inquiry.m_iPriority, inquiry2.m_iPriority);
		EXPECT_EQ(inquiry.m_lInquiryIndex, inquiry2.m_lInquiryIndex);
		EXPECT_EQ(inquiry.m_lStockIndex, inquiry2.m_lStockIndex);
	}

	TEST_F(CWorldMarketTest, TestIsFinnhubInquiring) {
		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubInquiring());
		gl_pWorldMarket->SetFinnhubInquiring(true);
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubInquiring());
		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubInquiring());
	}

	TEST_F(CWorldMarketTest, TestIsTiingoInquiring) {
		EXPECT_FALSE(gl_pWorldMarket->IsTiingoInquiring());
		gl_pWorldMarket->SetTiingoInquiring(true);
		EXPECT_TRUE(gl_pWorldMarket->IsTiingoInquiring());
		gl_pWorldMarket->SetTiingoInquiring(false);
		EXPECT_FALSE(gl_pWorldMarket->IsTiingoInquiring());
	}

	TEST_F(CWorldMarketTest, TestGetTotalStock) {
		EXPECT_EQ(gl_pWorldMarket->GetTotalStock(), 4846) << "默认状态下数据库总数为5059(全部上海股票和小部分美国股票)";
	}

	TEST_F(CWorldMarketTest, TestIsStock) {
		EXPECT_FALSE(gl_pWorldMarket->IsStock(_T("000000.SS")));
		EXPECT_TRUE(gl_pWorldMarket->IsStock(_T("000001.SS")));
		EXPECT_TRUE(gl_pWorldMarket->IsStock(_T("600601.SS")));
		EXPECT_TRUE(gl_pWorldMarket->IsStock(_T("A")));
		EXPECT_FALSE(gl_pWorldMarket->IsStock(_T("000001.SZ"))) << "目前测试数据库中只有上海和美国股票集";

		CWorldStockPtr pStock = make_shared<CWorldStock>();
		pStock->SetSymbol(_T("000000.SS"));
		EXPECT_FALSE(gl_pWorldMarket->IsStock(pStock));
		pStock->SetSymbol(_T("000001.SS"));
		EXPECT_TRUE(gl_pWorldMarket->IsStock(pStock));
		pStock->SetSymbol(_T("600601.SS"));
		EXPECT_TRUE(gl_pWorldMarket->IsStock(pStock));
		pStock->SetSymbol(_T("A"));
		EXPECT_TRUE(gl_pWorldMarket->IsStock(pStock));
		pStock->SetSymbol(_T("000001.SZ"));
		EXPECT_FALSE(gl_pWorldMarket->IsStock(pStock));
	}

	TEST_F(CWorldMarketTest, TestIsTiingoStock) {
		EXPECT_FALSE(gl_pWorldMarket->IsTiingoStock(_T("000000.SS")));
		EXPECT_TRUE(gl_pWorldMarket->IsTiingoStock(_T("AA")));
		EXPECT_FALSE(gl_pWorldMarket->IsTiingoStock(_T("600601.SS")));
		EXPECT_TRUE(gl_pWorldMarket->IsTiingoStock(_T("A")));
		EXPECT_FALSE(gl_pWorldMarket->IsTiingoStock(_T("000001.SZ"))) << "目前测试数据库中只有上海和美国股票集";

		CWorldStockPtr pStock = make_shared<CWorldStock>();
		pStock->SetSymbol(_T("000000.SS"));
		EXPECT_FALSE(gl_pWorldMarket->IsTiingoStock(pStock));
		pStock->SetSymbol(_T("AA"));
		EXPECT_TRUE(gl_pWorldMarket->IsTiingoStock(pStock));
		pStock->SetSymbol(_T("600601.SS"));
		EXPECT_FALSE(gl_pWorldMarket->IsTiingoStock(pStock));
		pStock->SetSymbol(_T("A"));
		EXPECT_TRUE(gl_pWorldMarket->IsTiingoStock(pStock));
		pStock->SetSymbol(_T("000001.SZ"));
		EXPECT_FALSE(gl_pWorldMarket->IsTiingoStock(pStock));

		CTiingoStockPtr pTiingoStock = make_shared<CTiingoStock>();
		pTiingoStock->m_strTicker = _T("000000.SS");
		EXPECT_FALSE(gl_pWorldMarket->IsTiingoStock(pTiingoStock));
		pTiingoStock->m_strTicker = _T("AA");
		EXPECT_TRUE(gl_pWorldMarket->IsTiingoStock(pTiingoStock));
		pTiingoStock->m_strTicker = _T("600601.SS");
		EXPECT_FALSE(gl_pWorldMarket->IsTiingoStock(pTiingoStock));
		pTiingoStock->m_strTicker = _T("A");
		EXPECT_TRUE(gl_pWorldMarket->IsTiingoStock(pTiingoStock));
		pTiingoStock->m_strTicker = _T("000001.SZ");
		EXPECT_FALSE(gl_pWorldMarket->IsTiingoStock(pTiingoStock));
	}

	TEST_F(CWorldMarketTest, TestGetTiingoStock) {
		CTiingoStockPtr pStock = gl_pWorldMarket->GetTiingoStock(0); // A
		EXPECT_STREQ(pStock->m_strTicker, _T("A")) << "第一个股票代码为A";
		pStock = gl_pWorldMarket->GetTiingoStock(_T("A"));
		EXPECT_FALSE(pStock == nullptr);
		EXPECT_STREQ(pStock->m_strName, _T("Agilent Technologies Inc"));
	}

	TEST_F(CWorldMarketTest, TestIsUpdateProfileDB) {
		CWorldStockPtr pStock = nullptr;
		for (int i = 0; i < gl_pWorldMarket->GetTotalStock(); i++) {
			pStock = gl_pWorldMarket->GetStock(i);
			pStock->SetUpdateProfileDB(false);
		}
		EXPECT_FALSE(gl_pWorldMarket->IsStockProfileNeedUpdate());

		gl_pWorldMarket->GetStock(0)->SetUpdateProfileDB(true);
		EXPECT_TRUE(gl_pWorldMarket->IsStockProfileNeedUpdate());

		gl_pWorldMarket->GetStock(0)->SetUpdateProfileDB(false);
		EXPECT_FALSE(gl_pWorldMarket->IsStockProfileNeedUpdate());
	}

	TEST_F(CWorldMarketTest, TestAddStock) {
		CWorldStockPtr pStock = make_shared<CWorldStock>();
		long lTotalStock = gl_pWorldMarket->GetTotalStock();
		pStock->SetSymbol(_T("000001.SZ"));

		EXPECT_FALSE(gl_pWorldMarket->IsStock(pStock));
		gl_pWorldMarket->AddStock(pStock);
		EXPECT_TRUE(gl_pWorldMarket->IsStock(pStock));
		EXPECT_EQ(gl_pWorldMarket->GetTotalStock(), lTotalStock + 1);
		gl_pWorldMarket->DeleteStock(pStock);
		EXPECT_FALSE(gl_pWorldMarket->IsStock(pStock));
		EXPECT_EQ(gl_pWorldMarket->GetTotalStock(), lTotalStock);
	}

	TEST_F(CWorldMarketTest, TestDeleteStock) {
		// do nothing. 已经在TestAddStock中测试了DeleteStock函数
		CWorldStockPtr pStock = nullptr;

		EXPECT_FALSE(gl_pWorldMarket->DeleteStock(pStock)) << "空指针";

		pStock = make_shared<CWorldStock>();
		pStock->SetSymbol(_T("000001.SZ"));
		EXPECT_FALSE(gl_pWorldMarket->DeleteStock(pStock)) << "此股票代码不存在于代码集中";
	}

	TEST_F(CWorldMarketTest, TestGetStock) {
		CWorldStockPtr pStock = gl_pWorldMarket->GetStock(0); // 000001.SS
		EXPECT_STREQ(pStock->GetSymbol(), _T("000001.SS")) << "第一个股票代码为000001.SS";
		pStock = gl_pWorldMarket->GetStock(_T("000001.SS"));
		EXPECT_FALSE(pStock == nullptr);
		EXPECT_STREQ(pStock->GetDescription(), _T("SSE Composite Index"));
	}

	TEST_F(CWorldMarketTest, TestAddTiingoStock) {
		CTiingoStockPtr pStock = make_shared<CTiingoStock>();
		long lTotalStock = gl_pWorldMarket->GetTotalTiingoStock();
		pStock->m_strTicker = _T("ABCDEF");

		EXPECT_FALSE(gl_pWorldMarket->IsTiingoStock(pStock));
		gl_pWorldMarket->AddTiingoStock(pStock);
		EXPECT_TRUE(gl_pWorldMarket->IsTiingoStock(pStock));
		EXPECT_EQ(gl_pWorldMarket->GetTotalTiingoStock(), lTotalStock + 1);
		gl_pWorldMarket->DeleteTiingoStock(pStock);
		EXPECT_FALSE(gl_pWorldMarket->IsTiingoStock(pStock));
		EXPECT_EQ(gl_pWorldMarket->GetTotalTiingoStock(), lTotalStock);
	}

	TEST_F(CWorldMarketTest, TestDeleteTiingoStock) {
		// do nothing. 已经在TestAddStock中测试了DeleteStock函数
		CTiingoStockPtr pStock = nullptr;

		EXPECT_FALSE(gl_pWorldMarket->DeleteTiingoStock(pStock)) << "空指针";

		pStock = make_shared<CTiingoStock>();
		pStock->m_strTicker = _T("ABCDEF");
		EXPECT_FALSE(gl_pWorldMarket->DeleteTiingoStock(pStock)) << "此股票代码不存在于代码集中";
	}

	TEST_F(CWorldMarketTest, TestIsForexExchange) {
		EXPECT_FALSE(gl_pWorldMarket->IsForexExchange(_T("ABC")));
		EXPECT_TRUE(gl_pWorldMarket->IsForexExchange(_T("forex.com")));
		EXPECT_TRUE(gl_pWorldMarket->IsForexExchange(_T("ic markets")));
	}

	TEST_F(CWorldMarketTest, TestAddForexExchange) {
		long lTotalForexExchange = gl_pWorldMarket->GetForexExchangeSize();
		CString strForexExchange = _T("000001.SZ");

		EXPECT_FALSE(gl_pWorldMarket->IsForexExchange(strForexExchange));
		gl_pWorldMarket->AddForexExchange(strForexExchange);
		EXPECT_TRUE(gl_pWorldMarket->IsForexExchange(strForexExchange));
		EXPECT_EQ(gl_pWorldMarket->GetForexExchangeSize(), lTotalForexExchange + 1);
		gl_pWorldMarket->DeleteForexExchange(strForexExchange);
		EXPECT_FALSE(gl_pWorldMarket->IsForexExchange(strForexExchange));
		EXPECT_EQ(gl_pWorldMarket->GetForexExchangeSize(), lTotalForexExchange);
	}

	TEST_F(CWorldMarketTest, TestDeleteForexExchange) {
		// do nothing. 已经在TestAddForexExchange中测试了DeleteForexExchange函数
		EXPECT_FALSE(gl_pWorldMarket->DeleteForexExchange(_T("US.US.US"))) << "此符号在符号集中不存在";
	}

	TEST_F(CWorldMarketTest, TestIsForexSymbol) {
		EXPECT_FALSE(gl_pWorldMarket->IsForexSymbol(_T("ABC")));
		EXPECT_TRUE(gl_pWorldMarket->IsForexSymbol(_T("OANDA:XAU_SGD")));
		EXPECT_TRUE(gl_pWorldMarket->IsForexSymbol(_T("FXCM:EUR/CHF")));

		CForexSymbolPtr pForexSymbol = make_shared<CFinnhubForexSymbol>();
		pForexSymbol->SetSymbol(_T("ABC"));
		EXPECT_FALSE(gl_pWorldMarket->IsForexSymbol(pForexSymbol));
		pForexSymbol->SetSymbol(_T("OANDA:XAU_SGD"));
		EXPECT_TRUE(gl_pWorldMarket->IsForexSymbol(pForexSymbol));
		pForexSymbol->SetSymbol(_T("FXCM:EUR/CHF"));
		EXPECT_TRUE(gl_pWorldMarket->IsForexSymbol(pForexSymbol));
	}

	TEST_F(CWorldMarketTest, TestAddForexSymbol) {
		CForexSymbolPtr pForexSymbol = make_shared<CFinnhubForexSymbol>();
		long lTotalForexSymbol = gl_pWorldMarket->GetForexSymbolSize();
		pForexSymbol->SetSymbol(_T("000001.SZ"));

		EXPECT_FALSE(gl_pWorldMarket->IsForexSymbol(pForexSymbol));
		gl_pWorldMarket->AddForexSymbol(pForexSymbol);
		EXPECT_TRUE(gl_pWorldMarket->IsForexSymbol(pForexSymbol));
		EXPECT_EQ(gl_pWorldMarket->GetForexSymbolSize(), lTotalForexSymbol + 1);
		EXPECT_TRUE(gl_pWorldMarket->DeleteForexSymbol(pForexSymbol));
		EXPECT_FALSE(gl_pWorldMarket->IsForexSymbol(pForexSymbol));
		EXPECT_EQ(gl_pWorldMarket->GetForexSymbolSize(), lTotalForexSymbol);
	}

	TEST_F(CWorldMarketTest, TestDeleteForexSymbol) {
		// do nothing. 已经在TestAddForexSymbol中测试了DeleteForexSymbol函数
		CForexSymbolPtr pForexSymbol = nullptr;

		EXPECT_FALSE(gl_pWorldMarket->DeleteForexSymbol(pForexSymbol)) << "空指针";

		pForexSymbol = make_shared<CFinnhubForexSymbol>();
		pForexSymbol->SetSymbol(_T("000001.SZ"));
		EXPECT_FALSE(gl_pWorldMarket->DeleteForexSymbol(pForexSymbol)) << "此符号在符号集中不存在";
	}

	TEST_F(CWorldMarketTest, TestIsCountry) {
		CCountryPtr pCountry = make_shared<CCountry>();

		EXPECT_FALSE(gl_pWorldMarket->IsCountry(_T("ABC")));
		EXPECT_TRUE(gl_pWorldMarket->IsCountry(_T("American Samoa")));

		pCountry->m_strCountry = _T("ABC");
		EXPECT_FALSE(gl_pWorldMarket->IsCountry(pCountry));
		pCountry->m_strCountry = _T("American Samoa");
		EXPECT_TRUE(gl_pWorldMarket->IsCountry(pCountry));
	}

	TEST_F(CWorldMarketTest, TestAddCountry) {
		CCountryPtr pCountry = make_shared<CCountry>();
		long lTotalCountry = gl_pWorldMarket->GetTotalCountry();
		pCountry->m_strCountry = _T("SZ");

		EXPECT_FALSE(gl_pWorldMarket->IsCountry(pCountry));
		gl_pWorldMarket->AddCountry(pCountry);
		EXPECT_TRUE(gl_pWorldMarket->IsCountry(pCountry));
		EXPECT_EQ(gl_pWorldMarket->GetTotalCountry(), lTotalCountry + 1);
		gl_pWorldMarket->DeleteCountry(pCountry);
		EXPECT_FALSE(gl_pWorldMarket->IsCountry(pCountry));
		EXPECT_EQ(gl_pWorldMarket->GetTotalCountry(), lTotalCountry);
	}

	TEST_F(CWorldMarketTest, TestDeleteCountry) {
		// do nothing. 已经在TestAddCountry中测试了DeleteCountry函数

		CCountryPtr pCountry = nullptr;

		EXPECT_FALSE(gl_pWorldMarket->DeleteCountry(pCountry)) << "空指针";

		pCountry = make_shared<CCountry>();
		pCountry->m_strCountry = _T("SZ");
		EXPECT_FALSE(gl_pWorldMarket->DeleteCountry(pCountry)) << "此股票代码不存在于代码集中";
	}

	TEST_F(CWorldMarketTest, TestLoadOption) {
		EXPECT_STREQ(gl_pFinnhubWebInquiry->GetInquiringStringSuffix(), _T("&token=c1i57rv48v6vit20lrc0"));

		gl_pFinnhubWebInquiry->SetInquiryingStringSuffix(_T(""));
		gl_pTiingoWebInquiry->SetInquiryingStringSuffix(_T(""));
		gl_pQuandlWebInquiry->SetInquiryingStringSuffix(_T(""));
		gl_pWorldMarket->LoadOption();
		EXPECT_STREQ(gl_pFinnhubWebInquiry->GetInquiringStringSuffix(), _T("&token=c1i57rv48v6vit20lrc0"));
		EXPECT_STREQ(gl_pTiingoWebInquiry->GetInquiringStringSuffix(), _T("&token=fad87279362b9e580e4fb364a263cda3c67336c8"));
		EXPECT_STREQ(gl_pQuandlWebInquiry->GetInquiringStringSuffix(), _T("&api_key=zBMXMyoTyiy_N3pMb3ex"));

		gl_pFinnhubWebInquiry->SetInquiryingStringSuffix(_T("&token=bv4ac1n48v6tcp17l5cg"));
		gl_pTiingoWebInquiry->SetInquiryingStringSuffix(_T("&token=859bd66ca24b2a81a2b5f4de6616e2c408b2a769"));
		gl_pQuandlWebInquiry->SetInquiryingStringSuffix(_T("&api_key=zBMXMyoTyiy_N3pMb3ex"));
	}

	TEST_F(CWorldMarketTest, TestLoadExchangeCode) {
		// 暂缓
	}

	TEST_F(CWorldMarketTest, TestUpdateCountryDB) {
		CCountryPtr pCountry;
		size_t lTotal = gl_pWorldMarket->GetTotalCountry();

		pCountry = make_shared<CCountry>();
		pCountry->m_strCode2 = _T("AB");
		pCountry->m_strCountry = _T("NoName");
		EXPECT_FALSE(gl_pWorldMarket->IsCountry(pCountry));
		gl_pWorldMarket->AddCountry(pCountry);
		EXPECT_EQ(gl_pWorldMarket->GetTotalCountry(), lTotal + 1);
		gl_pWorldMarket->UpdateCountryListDB(); // 此测试函数执行完后，新增了一个Country没有删除（数据库中的删除了）。

		CSetCountry setCountry;
		setCountry.m_strFilter = _T("[Country] = 'NoName'");
		setCountry.Open();
		EXPECT_FALSE(setCountry.IsEOF());
		setCountry.m_pDatabase->BeginTrans();
		while (!setCountry.IsEOF()) {
			setCountry.Delete();
			setCountry.MoveNext();
		}
		setCountry.m_pDatabase->CommitTrans();
		setCountry.Close();
	}

	TEST_F(CWorldMarketTest, TestUpdateStockProfileDB) {
		CWorldStockPtr pStock = make_shared<CWorldStock>();
		pStock->SetSymbol(_T("SS.SS.US"));
		EXPECT_FALSE(gl_pWorldMarket->IsStock(pStock)); // 确保是一个新股票代码
		gl_pWorldMarket->AddStock(pStock);
		pStock = gl_pWorldMarket->GetStock(_T("000001.SS"));
		EXPECT_STREQ(pStock->GetCurrency(), _T(""));
		pStock->SetUpdateProfileDB(true);
		pStock->SetCurrency(_T("No Currency")); // 更新这个条目

		gl_pWorldMarket->UpdateStockProfileDB();

		CSetWorldStock setWorldStock;
		setWorldStock.m_strFilter = _T("[Symbol] = '000001.SS'");
		setWorldStock.Open();
		EXPECT_STREQ(setWorldStock.m_Currency, _T("No Currency")) << "此条目已更新";
		setWorldStock.m_pDatabase->BeginTrans();
		setWorldStock.Edit();
		setWorldStock.m_Currency = _T("");
		setWorldStock.Update();
		setWorldStock.m_pDatabase->CommitTrans();
		setWorldStock.Close();

		setWorldStock.m_strFilter = _T("[Symbol] = 'SS.SS.US'");
		setWorldStock.Open();
		EXPECT_FALSE(setWorldStock.IsEOF());
		setWorldStock.m_pDatabase->BeginTrans();
		while (!setWorldStock.IsEOF()) {
			setWorldStock.Delete();
			setWorldStock.MoveNext();
		}
		setWorldStock.m_pDatabase->CommitTrans();
		setWorldStock.Close();

		pStock = gl_pWorldMarket->GetStock(_T("SS.SS.US"));
		gl_pWorldMarket->DeleteStock(pStock); // 恢复原状
	}

	TEST_F(CWorldMarketTest, TestUpdateDayLineDB) {
		EXPECT_TRUE(gl_pWorldMarket->UpdateStockDayLineDB());

		EXPECT_FALSE(gl_pWorldMarket->GetStock(0)->IsDayLineNeedSaving()) << "此标识被重置";
		gl_pWorldMarket->GetStock(0)->SetDayLineNeedSaving(true);

		EXPECT_TRUE(gl_pWorldMarket->UpdateStockDayLineDB());
		for (int i = 0; i < gl_pWorldMarket->GetTotalStock(); i++) {
			EXPECT_FALSE(gl_pWorldMarket->GetStock(i)->IsDayLineNeedSaving()) << "此标识被重置";
		}
	}

	TEST_F(CWorldMarketTest, TestUpdateForexSymbolDB) {
		CForexSymbolPtr pForexSymbol = make_shared<CFinnhubForexSymbol>();
		pForexSymbol->SetSymbol(_T("SS.SS.US")); // 新符号
		gl_pWorldMarket->AddForexSymbol(pForexSymbol);
		pForexSymbol = gl_pWorldMarket->GetForexSymbol(_T("OANDA:GBP_ZAR")); // 第二个现存的符号
		EXPECT_EQ(pForexSymbol->GetIPOStatus(), __STOCK_IPOED__);
		pForexSymbol->SetUpdateProfileDB(true);
		pForexSymbol->SetIPOStatus(__STOCK_DELISTED__);
		gl_pWorldMarket->UpdateForexSymbolDB();

		CSetFinnhubForexSymbol setWorldStock;
		setWorldStock.m_strFilter = _T("[Symbol] = 'OANDA:GBP_ZAR'");
		setWorldStock.Open();
		EXPECT_EQ(setWorldStock.m_IPOStatus, __STOCK_DELISTED__);
		setWorldStock.m_pDatabase->BeginTrans();
		setWorldStock.Edit();
		setWorldStock.m_IPOStatus = __STOCK_IPOED__;
		setWorldStock.Update();
		setWorldStock.m_pDatabase->CommitTrans();
		setWorldStock.Close();

		setWorldStock.m_strFilter = _T("[Symbol] = 'SS.SS.US'");
		setWorldStock.Open();
		EXPECT_FALSE(setWorldStock.IsEOF()) << "存入了新符号";
		setWorldStock.m_pDatabase->BeginTrans();
		while (!setWorldStock.IsEOF()) {
			setWorldStock.Delete();
			setWorldStock.MoveNext();
		}
		setWorldStock.m_pDatabase->CommitTrans();
		setWorldStock.Close();

		pForexSymbol = gl_pWorldMarket->GetForexSymbol(_T("SS.SS.US"));
		gl_pWorldMarket->DeleteForexSymbol(pForexSymbol); // 恢复原状
	}

	TEST_F(CWorldMarketTest, TestUpdateTiingoStockDB) {
		CSetTiingoStock setTiingoStock;

		CTiingoStockPtr pTiingoStock = make_shared<CTiingoStock>();
		pTiingoStock->m_fIsActive = true;
		pTiingoStock->m_fIsADR = false;
		pTiingoStock->m_iSICCode = 1002;
		pTiingoStock->m_lDailyDataUpdateDate = 20200101;
		pTiingoStock->m_lStatementUpdateDate = 20210101;
		pTiingoStock->m_strCompanyWebSite = _T("www.abc.com");
		pTiingoStock->m_strLocation = _T("Irvine CA USA");
		pTiingoStock->m_strName = _T("ABCDEF"); // 新代码
		pTiingoStock->m_strReportingCurrency = _T("US Doller");
		pTiingoStock->m_strSECFilingWebSite = _T("abc");
		pTiingoStock->m_strSICIndustry = _T("Computer Science");
		pTiingoStock->m_strSICSector = _T("Communication");
		pTiingoStock->m_strTicker = _T("ABCDEF"); // 新代码
		pTiingoStock->m_strTiingoIndustry = _T("Computer");
		pTiingoStock->m_strTiingoPermaTicker = _T("abcdefg");
		pTiingoStock->m_strTiingoSector = _T("gfedcba");

		gl_pWorldMarket->AddTiingoStock(pTiingoStock);

		gl_pWorldMarket->UpdateTiingoStockDB(); // 更新代码集

		// 恢复原状
		setTiingoStock.m_strFilter = _T("[Ticker] = 'ABCDEF'");
		setTiingoStock.Open();
		EXPECT_FALSE(setTiingoStock.IsEOF()) << "存入了ABCDEF股票代码";
		setTiingoStock.m_pDatabase->BeginTrans();
		while (!setTiingoStock.IsEOF()) {
			setTiingoStock.Delete();
			setTiingoStock.MoveNext();
		}
		setTiingoStock.m_pDatabase->CommitTrans();
		setTiingoStock.Close();

		gl_pWorldMarket->DeleteTiingoStock(pTiingoStock);
	}

	TEST_F(CWorldMarketTest, TestUpdateForexExchangeDB) {
		CString strSymbol = _T("US.US.US");

		EXPECT_FALSE(gl_pWorldMarket->UpdateForexExchangeDB()) << "没有新Forex Exchange";

		EXPECT_FALSE(gl_pWorldMarket->IsForexExchange(strSymbol)); // 确保是一个新股票代码
		gl_pWorldMarket->AddForexExchange(strSymbol);
		EXPECT_TRUE(gl_pWorldMarket->UpdateForexExchangeDB());

		CSetFinnhubForexExchange setForexExchange;
		setForexExchange.m_strFilter = _T("[Exchange] = 'US.US.US'");
		setForexExchange.Open();
		EXPECT_FALSE(setForexExchange.IsEOF());
		setForexExchange.m_pDatabase->BeginTrans();
		while (!setForexExchange.IsEOF()) {
			setForexExchange.Delete();
			setForexExchange.MoveNext();
		}
		setForexExchange.m_pDatabase->CommitTrans();
		setForexExchange.Close();

		gl_pWorldMarket->DeleteForexExchange(strSymbol); // 恢复原状
	}

	TEST_F(CWorldMarketTest, TaskUpdateInsiderTransactionDB) {
		EXPECT_FALSE(gl_pWorldMarket->GetStock(_T("A"))->HaveInsiderTransaction());
		EXPECT_EQ(gl_systemMessage.GetDayLineInfoDequeSize(), 0);

		CWorldStockPtr pStock;
		vector<CInsiderTransactionPtr> vInsiderTransaction;
		CInsiderTransactionPtr pInsiderTransaction;
		CSetInsiderTransaction setInsiderTransaction;

		pInsiderTransaction = make_shared<CInsiderTransaction>();
		pInsiderTransaction->m_strSymbol = _T("B");
		pInsiderTransaction->m_strPersonName = _T("a b c");
		pInsiderTransaction->m_lTransactionDate = 20200101; // 这个股票代码不符，需要添加进数据库
		vInsiderTransaction.push_back(pInsiderTransaction);
		pInsiderTransaction = make_shared<CInsiderTransaction>();
		pInsiderTransaction->m_strSymbol = _T("A");
		pInsiderTransaction->m_strPersonName = _T("a b c d");
		pInsiderTransaction->m_lTransactionDate = 20210101; // 这个内部交易人员名称不符，需要添加进数据库
		vInsiderTransaction.push_back(pInsiderTransaction);
		pInsiderTransaction = make_shared<CInsiderTransaction>();
		pInsiderTransaction->m_strSymbol = _T("A");
		pInsiderTransaction->m_strPersonName = _T("a b c");
		pInsiderTransaction->m_lTransactionDate = 20210107;
		pInsiderTransaction->m_strTransactionCode = _T("M"); // 这个数据库中有，无需添加
		vInsiderTransaction.push_back(pInsiderTransaction);
		pInsiderTransaction = make_shared<CInsiderTransaction>();
		pInsiderTransaction->m_strSymbol = _T("A");
		pInsiderTransaction->m_strPersonName = _T("a b c");
		pInsiderTransaction->m_lTransactionDate = 20210124; // 这个日期不符，需要添加进数据库
		vInsiderTransaction.push_back(pInsiderTransaction);
		pInsiderTransaction->m_strSymbol = _T("A");
		pInsiderTransaction->m_strPersonName = _T("a b c");
		pInsiderTransaction->m_strTransactionCode = _T("S"); // 这个交易类型不符，需要添加进数据库
		vInsiderTransaction.push_back(pInsiderTransaction);

		pStock = gl_pWorldMarket->GetStock(_T("A"));
		EXPECT_FALSE(pStock->HaveInsiderTransaction()) << "此时尚未存入数据";

		pStock->SetInsiderTransactionNeedSave(true);
		pStock->SetInsiderTransactionUpdateDate(20210123);
		pStock->UpdateInsiderTransaction(vInsiderTransaction);

		EXPECT_TRUE(gl_pWorldMarket->UpdateInsiderTransactionDB());

		EXPECT_EQ(gl_systemMessage.GetDayLineInfoDequeSize(), 1);
		CString str = gl_systemMessage.PopDayLineInfoMessage();
		EXPECT_STREQ(str, _T("A内部交易资料更新完成"));
		EXPECT_FALSE(gl_pWorldMarket->GetStock(_T("A"))->IsInsiderTransactionNeedSave());
		EXPECT_TRUE(gl_pWorldMarket->GetStock(_T("A"))->HaveInsiderTransaction()) << "存储后并没有删除数据";

		// 验证并恢复原状
		setInsiderTransaction.m_strFilter = _T("[Symbol] = 'B'");
		setInsiderTransaction.Open();
		setInsiderTransaction.m_pDatabase->BeginTrans();
		EXPECT_FALSE(setInsiderTransaction.IsEOF());
		setInsiderTransaction.Delete();
		setInsiderTransaction.m_pDatabase->CommitTrans();
		setInsiderTransaction.Close();

		setInsiderTransaction.m_strFilter = _T("[PersonName] = 'a b c d'");
		setInsiderTransaction.Open();
		setInsiderTransaction.m_pDatabase->BeginTrans();
		EXPECT_FALSE(setInsiderTransaction.IsEOF());
		setInsiderTransaction.Delete();
		setInsiderTransaction.m_pDatabase->CommitTrans();
		setInsiderTransaction.Close();

		setInsiderTransaction.m_strFilter = _T("[TransactionDate] = '20210124'");
		setInsiderTransaction.Open();
		setInsiderTransaction.m_pDatabase->BeginTrans();
		EXPECT_FALSE(setInsiderTransaction.IsEOF());
		setInsiderTransaction.Delete();
		setInsiderTransaction.m_pDatabase->CommitTrans();
		setInsiderTransaction.Close();

		setInsiderTransaction.m_strFilter = _T("[TransactionCode] = 'S'");
		setInsiderTransaction.Open();
		setInsiderTransaction.m_pDatabase->BeginTrans();
		EXPECT_FALSE(setInsiderTransaction.IsEOF());
		setInsiderTransaction.Delete();
		setInsiderTransaction.m_pDatabase->CommitTrans();
		setInsiderTransaction.Close();
	}

	TEST_F(CWorldMarketTest, TestUpdateEconomicCalendarDB) {
		CSetEconomicCalendar setEconomicCalendar;
		CEconomicCalendarPtr pEconomicCalendar = make_shared<CEconomicCalendar>();
		vector<CEconomicCalendarPtr> vEconomicCalendar;

		pEconomicCalendar->m_strCountry = _T("USA");
		pEconomicCalendar->m_strTime = _T("20200101");
		pEconomicCalendar->m_strEvent = _T("abc");
		pEconomicCalendar->m_dActual = 1.0;
		pEconomicCalendar->m_dEstimate = 2.0;
		pEconomicCalendar->m_dPrev = 3.0;
		pEconomicCalendar->m_strImpact = _T("s");
		pEconomicCalendar->m_strUnit = _T("USD");

		vEconomicCalendar.push_back(pEconomicCalendar);

		EXPECT_TRUE(gl_pWorldMarket->UpdateEconomicCalendar(vEconomicCalendar));
		EXPECT_TRUE(gl_pWorldMarket->UpdateEconomicCalendarDB());

		// 测试并恢复原状
		setEconomicCalendar.Open();
		EXPECT_FALSE(setEconomicCalendar.IsEOF());
		EXPECT_STREQ(setEconomicCalendar.m_Country, _T("USA"));
		EXPECT_STREQ(setEconomicCalendar.m_Time, _T("20200101"));
		EXPECT_STREQ(setEconomicCalendar.m_Event, _T("abc"));
		EXPECT_DOUBLE_EQ(atof(setEconomicCalendar.m_Actual), 1.0);
		EXPECT_DOUBLE_EQ(atof(setEconomicCalendar.m_Estimate), 2.0);
		EXPECT_DOUBLE_EQ(atof(setEconomicCalendar.m_Prev), 3.0);
		EXPECT_STREQ(setEconomicCalendar.m_Impact, _T("s"));
		EXPECT_STREQ(setEconomicCalendar.m_Unit, _T("USD"));
		setEconomicCalendar.m_pDatabase->BeginTrans();
		while (!setEconomicCalendar.IsEOF()) {
			setEconomicCalendar.Delete();
			setEconomicCalendar.MoveNext();
		}
		setEconomicCalendar.m_pDatabase->CommitTrans();
		setEconomicCalendar.Close();
	}

	TEST_F(CWorldMarketTest, TestGetFinnhubInquiry) {
		WebInquiry inquiry, inquiry2;

		EXPECT_EQ(gl_pWorldMarket->GetFinnhubInquiryQueueSize(), 0);
		inquiry.m_lInquiryIndex = 0;
		inquiry.m_iPriority = 10;
		inquiry.m_lStockIndex = 0;
		gl_pWorldMarket->PushFinnhubInquiry(inquiry);
		EXPECT_EQ(gl_pWorldMarket->GetFinnhubInquiryQueueSize(), 1);
		inquiry2 = gl_pWorldMarket->GetFinnhubInquiry();
		EXPECT_EQ(inquiry.m_iPriority, inquiry2.m_iPriority);
		EXPECT_EQ(inquiry.m_lInquiryIndex, inquiry2.m_lInquiryIndex);
		EXPECT_EQ(inquiry.m_lStockIndex, inquiry2.m_lStockIndex);
		EXPECT_EQ(gl_pWorldMarket->GetFinnhubInquiryQueueSize(), 0);
	}

	TEST_F(CWorldMarketTest, TestGetTiingoInquiry) {
		WebInquiry inquiry, inquiry2;

		EXPECT_EQ(gl_pWorldMarket->GetTiingoInquiryQueueSize(), 0);
		inquiry.m_lInquiryIndex = 0;
		inquiry.m_iPriority = 10;
		inquiry.m_lStockIndex = 0;
		gl_pWorldMarket->PushTiingoInquiry(inquiry);
		EXPECT_EQ(gl_pWorldMarket->GetTiingoInquiryQueueSize(), 1);
		inquiry2 = gl_pWorldMarket->GetTiingoInquiry();
		EXPECT_EQ(inquiry.m_iPriority, inquiry2.m_iPriority);
		EXPECT_EQ(inquiry.m_lInquiryIndex, inquiry2.m_lInquiryIndex);
		EXPECT_EQ(inquiry.m_lStockIndex, inquiry2.m_lStockIndex);
		EXPECT_EQ(gl_pWorldMarket->GetTiingoInquiryQueueSize(), 0);
	}

	TEST_F(CWorldMarketTest, TestIsCountryListUpdated) {
		EXPECT_FALSE(gl_pWorldMarket->IsCountryListUpdated());
		gl_pWorldMarket->SetCountryListUpdated(true);
		EXPECT_TRUE(gl_pWorldMarket->IsCountryListUpdated());
		gl_pWorldMarket->SetCountryListUpdated(false);
		EXPECT_FALSE(gl_pWorldMarket->IsCountryListUpdated());
	}

	TEST_F(CWorldMarketTest, TestIsFinnhubSymbolUpdated) {
		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubSymbolUpdated());
		gl_pWorldMarket->SetFinnhubSymbolUpdated(true);
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubSymbolUpdated());
		gl_pWorldMarket->SetFinnhubSymbolUpdated(false);
		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubSymbolUpdated());
	}

	TEST_F(CWorldMarketTest, TestIsFinnhubStockProfileUpdated) {
		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubStockProfileUpdated());
		gl_pWorldMarket->SetFinnhubStockProfileUpdated(true);
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubStockProfileUpdated());
		gl_pWorldMarket->SetFinnhubStockProfileUpdated(false);
		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubStockProfileUpdated());
	}

	TEST_F(CWorldMarketTest, TestIsFinnhubDayLineUpdated) {
		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubDayLineUpdated());
		gl_pWorldMarket->SetFinnhubDayLineUpdated(true);
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubDayLineUpdated());
		gl_pWorldMarket->SetFinnhubDayLineUpdated(false);
		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubDayLineUpdated());
	}

	TEST_F(CWorldMarketTest, TestIsFinnhubForexExhangeUpdated) {
		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubForexExchangeUpdated());
		gl_pWorldMarket->SetFinnhubForexExchangeUpdated(true);
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubForexExchangeUpdated());
		gl_pWorldMarket->SetFinnhubForexExchangeUpdated(false);
		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubForexExchangeUpdated());
	}

	TEST_F(CWorldMarketTest, TestIsFinnhubForexSymbolUpdated) {
		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubForexSymbolUpdated());
		gl_pWorldMarket->SetFinnhubForexSymbolUpdated(true);
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubForexSymbolUpdated());
		gl_pWorldMarket->SetFinnhubForexSymbolUpdated(false);
		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubForexSymbolUpdated());
	}

	TEST_F(CWorldMarketTest, TestIsFinnhubForexDayLineUpdated) {
		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubForexDayLineUpdated());
		gl_pWorldMarket->SetFinnhubForexDayLineUpdated(true);
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubForexDayLineUpdated());
		gl_pWorldMarket->SetFinnhubForexDayLineUpdated(false);
		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubForexDayLineUpdated());
	}

	TEST_F(CWorldMarketTest, TestIsFinnhubPeerUpdated) {
		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubPeerUpdated());
		gl_pWorldMarket->SetFinnhubPeerUpdated(true);
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubPeerUpdated());
		gl_pWorldMarket->SetFinnhubPeerUpdated(false);
		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubPeerUpdated());
	}

	TEST_F(CWorldMarketTest, TestIsFinnhubInsiderTransactionUpdated) {
		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubInsiderTransactionUpdated());
		gl_pWorldMarket->SetFinnhubInsiderTransactionUpdated(true);
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubInsiderTransactionUpdated());
		gl_pWorldMarket->SetFinnhubInsiderTransactionUpdated(false);
		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubInsiderTransactionUpdated());
	}

	TEST_F(CWorldMarketTest, TestIsFinnhubEconomicCalendarUpdated) {
		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubEconomicCalendarUpdated());
		gl_pWorldMarket->SetFinnhubEconomicCalendarUpdated(true);
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubEconomicCalendarUpdated());
		gl_pWorldMarket->SetFinnhubEconomicCalendarUpdated(false);
		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubEconomicCalendarUpdated());
	}

	TEST_F(CWorldMarketTest, TestIsFinnhubEPSSurpriseUpdated) {
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubEPSSurpriseUpdated());
		gl_pWorldMarket->SetFinnhubEPSSurpriseUpdated(false);
		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubEPSSurpriseUpdated());
		gl_pWorldMarket->SetFinnhubEPSSurpriseUpdated(true);
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubEPSSurpriseUpdated());
	}

	TEST_F(CWorldMarketTest, TestIsTiingoSymbolUpdated) {
		EXPECT_FALSE(gl_pWorldMarket->IsTiingoSymbolUpdated());
		gl_pWorldMarket->SetTiingoSymbolUpdated(true);
		EXPECT_TRUE(gl_pWorldMarket->IsTiingoSymbolUpdated());
		gl_pWorldMarket->SetTiingoSymbolUpdated(false);
		EXPECT_FALSE(gl_pWorldMarket->IsTiingoSymbolUpdated());
	}\
		TEST_F(CWorldMarketTest, TestIsTiingoDayLineUpdated) {
		EXPECT_FALSE(gl_pWorldMarket->IsTiingoDayLineUpdated());
		gl_pWorldMarket->SetTiingoDayLineUpdated(true);
		EXPECT_TRUE(gl_pWorldMarket->IsTiingoDayLineUpdated());
		gl_pWorldMarket->SetTiingoDayLineUpdated(false);
		EXPECT_FALSE(gl_pWorldMarket->IsTiingoDayLineUpdated());
	}

	TEST_F(CWorldMarketTest, TestTaskInquiryFinnhubCountryList) {
		EXPECT_FALSE(gl_pWorldMarket->IsCountryListUpdated());
		gl_pWorldMarket->SetFinnhubInquiring(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubCountryList());
		EXPECT_EQ(gl_pWorldMarket->GetFinnhubInquiryQueueSize(), 0);
		gl_pWorldMarket->SetCountryListUpdated(true);
		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubCountryList());
		EXPECT_EQ(gl_pWorldMarket->GetFinnhubInquiryQueueSize(), 0);
		gl_pWorldMarket->SetCountryListUpdated(false);
		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_TRUE(gl_pWorldMarket->TaskInquiryFinnhubCountryList());
		EXPECT_EQ(gl_pWorldMarket->GetFinnhubInquiryQueueSize(), 1);
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubInquiring());
		WebInquiry inquiry = gl_pWorldMarket->GetFinnhubInquiry();
		EXPECT_EQ(inquiry.m_iPriority, 10);
		EXPECT_EQ(inquiry.m_lInquiryIndex, __ECONOMIC_COUNTRY_LIST__);
		EXPECT_EQ(inquiry.m_lStockIndex, 0);
		EXPECT_EQ(gl_pWorldMarket->GetFinnhubInquiryQueueSize(), 0);
	}

	TEST_F(CWorldMarketTest, TestTaskInquiryFinnhubCompanySymbol) {
		CFinnhubExchangePtr pExchange;
		WebInquiry inquiry;

		EXPECT_EQ(gl_pWorldMarket->GetExchangeSize(), 70);
		for (int i = 0; i < gl_pWorldMarket->GetExchangeSize(); i++) {
			pExchange = gl_pWorldMarket->GetExchange(i);
			pExchange->m_fUpdated = true;
		}
		gl_pWorldMarket->GetExchange(1)->m_fUpdated = false;
		gl_pWorldMarket->GetExchange(10)->m_fUpdated = false;
		gl_pWorldMarket->SetFinnhubSymbolUpdated(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubCompanySymbol()) << "Symbol Updated";

		gl_pWorldMarket->SetFinnhubSymbolUpdated(false);
		gl_pWorldMarket->SetFinnhubInquiring(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubCompanySymbol()) << "其他FinnhubInquiry正在进行";

		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_TRUE(gl_pWorldMarket->TaskInquiryFinnhubCompanySymbol());
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubInquiring());
		inquiry = gl_pWorldMarket->GetFinnhubInquiry();
		EXPECT_EQ(inquiry.m_lInquiryIndex, __COMPANY_SYMBOLS__);
		EXPECT_EQ(inquiry.m_lStockIndex, 1) << "第一个待查询交易所索引";
		EXPECT_TRUE(gl_pWorldMarket->GetExchange(1)->m_fUpdated);
		EXPECT_FALSE(gl_pWorldMarket->GetExchange(10)->m_fUpdated);

		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_TRUE(gl_pWorldMarket->TaskInquiryFinnhubCompanySymbol());
		inquiry = gl_pWorldMarket->GetFinnhubInquiry();
		EXPECT_EQ(inquiry.m_lInquiryIndex, __COMPANY_SYMBOLS__);
		EXPECT_EQ(inquiry.m_lStockIndex, 10) << "第二个待查询交易所索引";
		EXPECT_TRUE(gl_pWorldMarket->GetExchange(1)->m_fUpdated);
		EXPECT_TRUE(gl_pWorldMarket->GetExchange(10)->m_fUpdated);

		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubCompanySymbol()) << "第三次查询时没有找到待查询的交易所";
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubSymbolUpdated()) << "交易所都查询完了";
		CString str = gl_systemMessage.PopInformationMessage();
		EXPECT_STREQ(str, _T("交易所代码数据查询完毕"));
	}

	TEST_F(CWorldMarketTest, TestTaskInquiryFinnhubCompanyProfileConcise) {
		CWorldStockPtr pStock;
		WebInquiry inquiry;

		gl_pWorldMarket->SetSystemReady(true);
		for (int i = 0; i < gl_pWorldMarket->GetTotalStock(); i++) {
			pStock = gl_pWorldMarket->GetStock(i);
			pStock->SetProfileUpdated(true);
		}
		gl_pWorldMarket->GetStock(1)->SetProfileUpdated(false);
		gl_pWorldMarket->GetStock(10)->SetProfileUpdated(false);
		gl_pWorldMarket->SetFinnhubStockProfileUpdated(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubCompanyProfileConcise()) << "Stock Profile Updated";

		gl_pWorldMarket->SetFinnhubStockProfileUpdated(false);
		gl_pWorldMarket->SetFinnhubInquiring(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubCompanyProfileConcise()) << "其他FinnhubInquiry正在进行";

		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_TRUE(gl_pWorldMarket->TaskInquiryFinnhubCompanyProfileConcise());
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubInquiring());
		inquiry = gl_pWorldMarket->GetFinnhubInquiry();
		EXPECT_EQ(inquiry.m_lInquiryIndex, __COMPANY_PROFILE_CONCISE__);
		EXPECT_EQ(inquiry.m_lStockIndex, 1) << "第一个待查询股票位置";
		EXPECT_FALSE(gl_pWorldMarket->GetStock(1)->IsProfileUpdated()) << "此更新标识需要等待处理完数据后才设置";
		EXPECT_FALSE(gl_pWorldMarket->GetStock(10)->IsProfileUpdated());
		gl_pWorldMarket->GetStock(1)->SetProfileUpdated(true);

		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_TRUE(gl_pWorldMarket->TaskInquiryFinnhubCompanyProfileConcise());
		inquiry = gl_pWorldMarket->GetFinnhubInquiry();
		EXPECT_EQ(inquiry.m_lInquiryIndex, __COMPANY_PROFILE_CONCISE__);
		EXPECT_EQ(inquiry.m_lStockIndex, 10) << "第二个待查询股票位置";
		EXPECT_TRUE(gl_pWorldMarket->GetStock(1)->IsProfileUpdated());
		EXPECT_FALSE(gl_pWorldMarket->GetStock(10)->IsProfileUpdated()) << "此更新标识需要等待处理完数据后才设置";
		gl_pWorldMarket->GetStock(10)->SetProfileUpdated(true);

		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubCompanyProfileConcise()) << "第三次查询时没有找到待查询的股票";
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubStockProfileUpdated()) << "股票都查询完了";
		CString str = gl_systemMessage.PopInformationMessage();
		EXPECT_STREQ(str, _T("Finnhub股票简介更新完毕"));
	}

	TEST_F(CWorldMarketTest, TestTaskInquiryFinnhubDayLine) {
		CWorldStockPtr pStock;
		WebInquiry inquiry;

		gl_pWorldMarket->SetSystemReady(true);
		for (int i = 0; i < gl_pWorldMarket->GetTotalStock(); i++) {
			pStock = gl_pWorldMarket->GetStock(i);
			pStock->SetDayLineNeedUpdate(false);
		}
		gl_pWorldMarket->GetStock(3001)->SetDayLineNeedUpdate(true); // 测试数据库中，上海市场的股票排在前面（共2462个），美国市场的股票排在后面
		gl_pWorldMarket->GetStock(3010)->SetDayLineNeedUpdate(true);
		gl_pWorldMarket->SetFinnhubDayLineUpdated(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubDayLine()) << "DayLine Updated";

		gl_pWorldMarket->SetFinnhubDayLineUpdated(false);
		gl_pWorldMarket->SetFinnhubInquiring(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubDayLine()) << "其他FinnhubInquiry正在进行";

		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_TRUE(gl_pWorldMarket->TaskInquiryFinnhubDayLine());
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubInquiring());
		inquiry = gl_pWorldMarket->GetFinnhubInquiry();
		EXPECT_EQ(inquiry.m_lInquiryIndex, __STOCK_CANDLES__);
		EXPECT_EQ(inquiry.m_lStockIndex, 3001) << "第一个待查询股票位置";
		EXPECT_FALSE(gl_pWorldMarket->GetStock(3001)->IsDayLineNeedUpdate());
		EXPECT_TRUE(gl_pWorldMarket->GetStock(3010)->IsDayLineNeedUpdate());

		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_TRUE(gl_pWorldMarket->TaskInquiryFinnhubDayLine());
		inquiry = gl_pWorldMarket->GetFinnhubInquiry();
		EXPECT_EQ(inquiry.m_lInquiryIndex, __STOCK_CANDLES__);
		EXPECT_EQ(inquiry.m_lStockIndex, 3010) << "第二个待查询股票位置";
		EXPECT_FALSE(gl_pWorldMarket->GetStock(3001)->IsDayLineNeedUpdate());
		EXPECT_FALSE(gl_pWorldMarket->GetStock(3010)->IsDayLineNeedUpdate());

		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubDayLine()) << "第三次查询时没有找到待查询的股票";
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubDayLineUpdated()) << "股票都查询完了";
		CString str = gl_systemMessage.PopInformationMessage();
		EXPECT_STREQ(str, _T("US Market日线历史数据更新完毕"));

		// 恢复原状
		for (int i = 0; i < gl_pWorldMarket->GetTotalStock(); i++) {
			pStock = gl_pWorldMarket->GetStock(i);
			pStock->SetDayLineNeedUpdate(true);
		}
	}

	TEST_F(CWorldMarketTest, TestTaskInquiryFinnhubRTQuote) {
		WebInquiry inquiry;

		gl_pWorldMarket->SetSystemReady(true);

		gl_pWorldMarket->SetFinnhubDayLineUpdated(false);
		gl_pWorldMarket->SetFinnhubInquiring(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubRTQuote()) << "其他FinnhubInquiry正在进行";

		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_TRUE(gl_pWorldMarket->TaskInquiryFinnhubRTQuote());
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubInquiring());
		inquiry = gl_pWorldMarket->GetFinnhubInquiry();
		EXPECT_EQ(inquiry.m_lInquiryIndex, __STOCK_QUOTE__);
		EXPECT_EQ(inquiry.m_iPriority, 10);
	}

	TEST_F(CWorldMarketTest, TestTaskInquiryFinnhubPeer) {
		CWorldStockPtr pStock;
		WebInquiry inquiry;

		gl_pWorldMarket->SetSystemReady(true);
		for (int i = 0; i < gl_pWorldMarket->GetTotalStock(); i++) {
			pStock = gl_pWorldMarket->GetStock(i);
			pStock->SetPeerUpdated(true);
		}
		gl_pWorldMarket->GetStock(1)->SetPeerUpdated(false); // 测试数据库中，上海市场的股票排在前面（共2462个），美国市场的股票排在后面
		gl_pWorldMarket->GetStock(10)->SetPeerUpdated(false);
		gl_pWorldMarket->SetFinnhubPeerUpdated(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubPeer()) << "Peers Updated";

		gl_pWorldMarket->SetFinnhubPeerUpdated(false);
		gl_pWorldMarket->SetFinnhubInquiring(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubPeer()) << "其他FinnhubInquiry正在进行";

		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_TRUE(gl_pWorldMarket->TaskInquiryFinnhubPeer());
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubInquiring());
		inquiry = gl_pWorldMarket->GetFinnhubInquiry();
		EXPECT_EQ(inquiry.m_lInquiryIndex, __PEERS__);
		EXPECT_EQ(inquiry.m_lStockIndex, 1) << "第一个待查询股票位置";
		EXPECT_FALSE(gl_pWorldMarket->GetStock(1)->IsPeerUpdated()) << "此更新标识需要等待处理完数据后才设置";
		EXPECT_FALSE(gl_pWorldMarket->GetStock(10)->IsPeerUpdated());
		gl_pWorldMarket->GetStock(1)->SetPeerUpdated(true);

		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_TRUE(gl_pWorldMarket->TaskInquiryFinnhubPeer());
		inquiry = gl_pWorldMarket->GetFinnhubInquiry();
		EXPECT_EQ(inquiry.m_lInquiryIndex, __PEERS__);
		EXPECT_EQ(inquiry.m_lStockIndex, 10) << "第二个待查询股票位置";
		EXPECT_TRUE(gl_pWorldMarket->GetStock(1)->IsPeerUpdated());
		EXPECT_FALSE(gl_pWorldMarket->GetStock(10)->IsPeerUpdated()) << "此更新标识需要等待处理完数据后才设置";
		gl_pWorldMarket->GetStock(10)->SetPeerUpdated(true);

		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubPeer()) << "第三次查询时没有找到待查询的股票";
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubPeerUpdated()) << "股票都查询完了";
		CString str = gl_systemMessage.PopInformationMessage();
		EXPECT_STREQ(str, _T("Finnhub Peer Updated"));
	}

	TEST_F(CWorldMarketTest, TestTaskInquiryFinnhubInsiderTransaction) {
		CWorldStockPtr pStock;
		WebInquiry inquiry;

		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubInsiderTransactionUpdated()) << "股票都查询完了";

		gl_pWorldMarket->SetSystemReady(true);
		for (int i = 0; i < gl_pWorldMarket->GetTotalStock(); i++) {
			pStock = gl_pWorldMarket->GetStock(i);
			pStock->SetInsiderTransactionNeedUpdate(false);
		}
		gl_pWorldMarket->GetStock(1)->SetInsiderTransactionNeedUpdate(true); // 测试数据库中，上海市场的股票排在前面（共2462个），美国市场的股票排在后面
		gl_pWorldMarket->GetStock(2500)->SetInsiderTransactionNeedUpdate(true); // 这个是美国股票
		gl_pWorldMarket->SetFinnhubInsiderTransactionUpdated(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubInsiderTransaction()) << "InsiderTransactions Updated";

		gl_pWorldMarket->SetFinnhubInsiderTransactionUpdated(false);
		gl_pWorldMarket->SetFinnhubInquiring(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubInsiderTransaction()) << "其他FinnhubInquiry正在进行";

		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_TRUE(gl_pWorldMarket->TaskInquiryFinnhubInsiderTransaction());
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubInquiring());
		inquiry = gl_pWorldMarket->GetFinnhubInquiry();
		EXPECT_EQ(inquiry.m_lInquiryIndex, __INSIDER_TRANSACTION__);
		EXPECT_EQ(inquiry.m_lStockIndex, 2500) << "第一个待查询股票为中国股票，故而无需查询；第二个待查询股票为美国股票";
		EXPECT_TRUE(gl_pWorldMarket->GetStock(1)->IsInsiderTransactionNeedUpdate()) << "第一个股票为中国股票，没有复原";
		EXPECT_TRUE(gl_pWorldMarket->GetStock(2500)->IsInsiderTransactionNeedUpdate()) << "需要接收到数据后方才设置此标识";
		gl_pWorldMarket->GetStock(1)->SetInsiderTransactionNeedUpdate(false);
		gl_pWorldMarket->GetStock(2500)->SetInsiderTransactionNeedUpdate(false);

		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubInsiderTransaction()) << "第二次查询时没有找到待查询的股票";
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubInsiderTransactionUpdated()) << "股票都查询完了";
		CString str = gl_systemMessage.PopInformationMessage();
		EXPECT_STREQ(str, _T("US Market Insider Transaction数据更新完毕"));

		// 恢复原状
		gl_pWorldMarket->SetFinnhubInsiderTransactionUpdated(false);
	}

	TEST_F(CWorldMarketTest, TestTaskInquiryFinnhubEconomicCalendar) {
		WebInquiry inquiry;

		gl_pWorldMarket->SetFinnhubEconomicCalendarUpdated(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubEconomicCalendar()) << "EconomicCalendar Updated";

		gl_pWorldMarket->SetFinnhubEconomicCalendarUpdated(false);
		gl_pWorldMarket->SetFinnhubInquiring(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubEconomicCalendar()) << "其他FinnhubInquiry正在进行";

		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_TRUE(gl_pWorldMarket->TaskInquiryFinnhubEconomicCalendar());
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubInquiring());
		inquiry = gl_pWorldMarket->GetFinnhubInquiry();
		EXPECT_EQ(inquiry.m_lInquiryIndex, __ECONOMIC_CALENDAR__);
		EXPECT_EQ(inquiry.m_iPriority, 10);
	}

	TEST_F(CWorldMarketTest, TestTaskInquiryFinnhubEPSSurprise) {
		CWorldStockPtr pStock;
		WebInquiry inquiry;

		gl_pWorldMarket->SetSystemReady(true);
		for (int i = 0; i < gl_pWorldMarket->GetTotalStock(); i++) {
			pStock = gl_pWorldMarket->GetStock(i);
			pStock->SetEPSSurpriseUpdated(true);
		}
		gl_pWorldMarket->GetStock(1)->SetEPSSurpriseUpdated(false); // 测试数据库中，上海市场的股票排在前面（共2462个），美国市场的股票排在后面
		gl_pWorldMarket->GetStock(10)->SetEPSSurpriseUpdated(false);
		gl_pWorldMarket->SetFinnhubEPSSurpriseUpdated(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubEPSSurprise()) << "Finnhub EPS Surprise  Updated";

		gl_pWorldMarket->SetFinnhubEPSSurpriseUpdated(false);
		gl_pWorldMarket->SetFinnhubInquiring(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubEPSSurprise()) << "其他FinnhubInquiry正在进行";

		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_TRUE(gl_pWorldMarket->TaskInquiryFinnhubEPSSurprise());
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubInquiring());
		inquiry = gl_pWorldMarket->GetFinnhubInquiry();
		EXPECT_EQ(inquiry.m_lInquiryIndex, __STOCK_EPS_SURPRISE__);
		EXPECT_EQ(inquiry.m_lStockIndex, 1) << "第一个待查询股票位置";
		EXPECT_TRUE(gl_pWorldMarket->GetStock(1)->IsEPSSurpriseUpdated());
		EXPECT_FALSE(gl_pWorldMarket->GetStock(10)->IsEPSSurpriseUpdated());
		gl_pWorldMarket->GetStock(1)->SetEPSSurpriseUpdated(true);

		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_TRUE(gl_pWorldMarket->TaskInquiryFinnhubEPSSurprise());
		inquiry = gl_pWorldMarket->GetFinnhubInquiry();
		EXPECT_EQ(inquiry.m_lInquiryIndex, __STOCK_EPS_SURPRISE__);
		EXPECT_EQ(inquiry.m_lStockIndex, 10) << "第二个待查询股票位置";
		EXPECT_TRUE(gl_pWorldMarket->GetStock(1)->IsEPSSurpriseUpdated());
		EXPECT_TRUE(gl_pWorldMarket->GetStock(10)->IsEPSSurpriseUpdated());
		gl_pWorldMarket->GetStock(10)->SetPeerUpdated(true);

		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubEPSSurprise()) << "第三次查询时没有找到待查询的股票";
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubEPSSurpriseUpdated()) << "股票都查询完了";
		CString str = gl_systemMessage.PopInformationMessage();
		EXPECT_STREQ(str, _T("Finnhub EPS Surprise Updated"));
	}

	TEST_F(CWorldMarketTest, TestTaskInquiryFinnhubForexExchange) {
		WebInquiry inquiry;

		gl_pWorldMarket->SetFinnhubForexExchangeUpdated(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubForexExchange()) << "FinnhubForexExchange Updated";

		gl_pWorldMarket->SetFinnhubForexExchangeUpdated(false);
		gl_pWorldMarket->SetFinnhubInquiring(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubForexExchange()) << "其他FinnhubInquiry正在进行";

		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_TRUE(gl_pWorldMarket->TaskInquiryFinnhubForexExchange());
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubInquiring());
		inquiry = gl_pWorldMarket->GetFinnhubInquiry();
		EXPECT_EQ(inquiry.m_lInquiryIndex, __FOREX_EXCHANGE__);
		EXPECT_EQ(inquiry.m_iPriority, 10);
		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubForexExchangeUpdated()) << "此标识需要等处理完数据后方设置";
	}

	TEST_F(CWorldMarketTest, TestTaskInquiryFinnhubForexSymbol) {
		WebInquiry inquiry;

		gl_pWorldMarket->SetFinnhubForexSymbolUpdated(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubForexSymbol()) << "ForexSymbol Updated";

		gl_pWorldMarket->SetFinnhubForexSymbolUpdated(false);
		gl_pWorldMarket->SetFinnhubInquiring(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubForexSymbol()) << "其他FinnhubInquiry正在进行";

		size_t lTotal = gl_pWorldMarket->GetForexExchangeSize();
		for (int i = 0; i < lTotal - 1; i++) {
			gl_pWorldMarket->SetFinnhubInquiring(false);
			EXPECT_TRUE(gl_pWorldMarket->TaskInquiryFinnhubForexSymbol());
			EXPECT_TRUE(gl_pWorldMarket->IsFinnhubInquiring());
			inquiry = gl_pWorldMarket->GetFinnhubInquiry();
			EXPECT_EQ(inquiry.m_lInquiryIndex, __FOREX_SYMBOLS__);
			EXPECT_EQ(inquiry.m_lStockIndex, i);
			EXPECT_EQ(inquiry.m_iPriority, 10);
			EXPECT_FALSE(gl_pWorldMarket->IsFinnhubForexSymbolUpdated());
		}
		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_TRUE(gl_pWorldMarket->TaskInquiryFinnhubForexSymbol());
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubInquiring());
		inquiry = gl_pWorldMarket->GetFinnhubInquiry();
		EXPECT_EQ(inquiry.m_lInquiryIndex, __FOREX_SYMBOLS__);
		EXPECT_EQ(inquiry.m_lStockIndex, lTotal - 1);
		EXPECT_EQ(inquiry.m_iPriority, 10);
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubForexSymbolUpdated());
		gl_pWorldMarket->SetFinnhubForexSymbolUpdated(false); //恢复原状
	}

	TEST_F(CWorldMarketTest, TestTaskInquiryFinnhubForexDayLine) {
		CForexSymbolPtr pStock;
		WebInquiry inquiry;

		gl_pWorldMarket->SetSystemReady(true);
		for (int i = 0; i < gl_pWorldMarket->GetForexSymbolSize(); i++) {
			pStock = gl_pWorldMarket->GetForexSymbol(i);
			pStock->SetDayLineNeedUpdate(false);
		}
		gl_pWorldMarket->GetForexSymbol(1)->SetDayLineNeedUpdate(true);
		gl_pWorldMarket->GetForexSymbol(10)->SetDayLineNeedUpdate(true);
		gl_pWorldMarket->SetFinnhubForexDayLineUpdated(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubForexDayLine()) << "Finnhub Forex Symbol DayLine Updated";

		gl_pWorldMarket->SetFinnhubForexDayLineUpdated(false);
		gl_pWorldMarket->SetFinnhubInquiring(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubForexDayLine()) << "其他FinnhubInquiry正在进行";

		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_TRUE(gl_pWorldMarket->TaskInquiryFinnhubForexDayLine());
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubInquiring());
		inquiry = gl_pWorldMarket->GetFinnhubInquiry();
		EXPECT_EQ(inquiry.m_lInquiryIndex, __FOREX_CANDLES__);
		EXPECT_EQ(inquiry.m_lStockIndex, 1) << "第一个待查询股票位置";
		EXPECT_FALSE(gl_pWorldMarket->GetForexSymbol(1)->IsDayLineNeedUpdate());
		EXPECT_TRUE(gl_pWorldMarket->GetForexSymbol(10)->IsDayLineNeedUpdate());

		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_TRUE(gl_pWorldMarket->TaskInquiryFinnhubForexDayLine());
		inquiry = gl_pWorldMarket->GetFinnhubInquiry();
		EXPECT_EQ(inquiry.m_lInquiryIndex, __FOREX_CANDLES__);
		EXPECT_EQ(inquiry.m_lStockIndex, 10) << "第二个待查询股票位置";
		EXPECT_FALSE(gl_pWorldMarket->GetForexSymbol(1)->IsDayLineNeedUpdate());
		EXPECT_FALSE(gl_pWorldMarket->GetForexSymbol(10)->IsDayLineNeedUpdate());

		gl_pWorldMarket->SetFinnhubInquiring(false);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryFinnhubForexDayLine()) << "第三次查询时没有找到待查询的股票";
		EXPECT_TRUE(gl_pWorldMarket->IsFinnhubForexDayLineUpdated()) << "股票都查询完了";
		CString str = gl_systemMessage.PopInformationMessage();
		EXPECT_STREQ(str, _T("Forex DayLine Updated"));

		for (int i = 0; i < gl_pWorldMarket->GetForexSymbolSize(); i++) {
			pStock = gl_pWorldMarket->GetForexSymbol(i);
			pStock->SetDayLineNeedUpdate(true);
		}
	}

	TEST_F(CWorldMarketTest, TestTaskInquiryTiingoCompanySymbol) {
		WebInquiry inquiry;

		gl_pWorldMarket->SetTiingoSymbolUpdated(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryTiingoCompanySymbol()) << "TiingoCompanySymbol Updated";

		gl_pWorldMarket->SetTiingoSymbolUpdated(false);
		gl_pWorldMarket->SetTiingoInquiring(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryTiingoCompanySymbol()) << "其他TiingoInquiry正在进行";

		gl_pWorldMarket->SetTiingoInquiring(false);
		EXPECT_TRUE(gl_pWorldMarket->TaskInquiryTiingoCompanySymbol());
		EXPECT_TRUE(gl_pWorldMarket->IsTiingoInquiring());
		inquiry = gl_pWorldMarket->GetTiingoInquiry();
		EXPECT_EQ(inquiry.m_lInquiryIndex, __COMPANY_SYMBOLS__);
		EXPECT_EQ(inquiry.m_iPriority, 10);
		EXPECT_FALSE(gl_pWorldMarket->IsTiingoSymbolUpdated()) << "此标识需要等处理完数据后方设置";
		CString str = gl_systemMessage.PopInformationMessage();
		EXPECT_STREQ(str, _T("Inquiry Tiingo Symbol"));
	}

	TEST_F(CWorldMarketTest, TestTaskInquiryTiingoDayLine) {
		CWorldStockPtr pStock;
		WebInquiry inquiry;
		long lStockIndex = 0;

		gl_pWorldMarket->SetSystemReady(true);
		for (int i = 0; i < gl_pWorldMarket->GetTotalStock(); i++) {
			pStock = gl_pWorldMarket->GetStock(i);
			pStock->SetDayLineNeedUpdate(false);
		}
		gl_pWorldMarket->GetChoicedStock(1)->SetDayLineNeedUpdate(true);
		gl_pWorldMarket->GetChoicedStock(3)->SetDayLineNeedUpdate(true);
		gl_pWorldMarket->SetTiingoDayLineUpdated(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryTiingoDayLine()) << "DayLine Updated";

		gl_pWorldMarket->SetTiingoDayLineUpdated(false);
		gl_pWorldMarket->SetTiingoInquiring(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryTiingoDayLine()) << "其他TiingoInquiry正在进行";

		gl_pWorldMarket->SetTiingoInquiring(false);
		EXPECT_TRUE(gl_pWorldMarket->TaskInquiryTiingoDayLine());
		EXPECT_TRUE(gl_pWorldMarket->IsTiingoInquiring());
		lStockIndex = gl_pWorldMarket->GetStockIndex(gl_pWorldMarket->GetChoicedStock(1)->GetSymbol());
		inquiry = gl_pWorldMarket->GetTiingoInquiry();
		EXPECT_EQ(inquiry.m_lInquiryIndex, __STOCK_CANDLES__);
		EXPECT_EQ(inquiry.m_lStockIndex, lStockIndex) << "第一个待查询股票位置";
		EXPECT_FALSE(gl_pWorldMarket->GetChoicedStock(1)->IsDayLineNeedUpdate());
		EXPECT_TRUE(gl_pWorldMarket->GetChoicedStock(3)->IsDayLineNeedUpdate());

		gl_pWorldMarket->SetTiingoInquiring(false);
		EXPECT_TRUE(gl_pWorldMarket->TaskInquiryTiingoDayLine());
		lStockIndex = gl_pWorldMarket->GetStockIndex(gl_pWorldMarket->GetChoicedStock(3)->GetSymbol());
		inquiry = gl_pWorldMarket->GetTiingoInquiry();
		EXPECT_EQ(inquiry.m_lInquiryIndex, __STOCK_CANDLES__);
		EXPECT_EQ(inquiry.m_lStockIndex, lStockIndex) << "第二个待查询股票位置";
		EXPECT_FALSE(gl_pWorldMarket->GetChoicedStock(1)->IsDayLineNeedUpdate());
		EXPECT_FALSE(gl_pWorldMarket->GetChoicedStock(3)->IsDayLineNeedUpdate());

		gl_pWorldMarket->SetTiingoInquiring(false);
		EXPECT_FALSE(gl_pWorldMarket->TaskInquiryTiingoDayLine()) << "第三次查询时没有找到待查询的股票";
		EXPECT_TRUE(gl_pWorldMarket->IsTiingoDayLineUpdated()) << "股票都查询完了";
		CString str = gl_systemMessage.PopInformationMessage();
		EXPECT_STREQ(str, _T("美国市场自选股票日线历史数据更新完毕"));

		// 恢复原状
		for (int i = 0; i < gl_pWorldMarket->GetTotalStock(); i++) {
			pStock = gl_pWorldMarket->GetStock(i);
			pStock->SetDayLineNeedUpdate(true);
		}
	}

	TEST_F(CWorldMarketTest, TestTaskCheckSystemReady) {
		gl_pWorldMarket->SetSystemReady(true);
		EXPECT_TRUE(gl_pWorldMarket->TaskCheckSystemReady());

		gl_pWorldMarket->SetSystemReady(false);
		gl_pWorldMarket->SetFinnhubSymbolUpdated(false);
		gl_pWorldMarket->SetFinnhubForexExchangeUpdated(false);
		gl_pWorldMarket->SetFinnhubForexSymbolUpdated(false);
		EXPECT_FALSE(gl_pWorldMarket->TaskCheckSystemReady());
		EXPECT_FALSE(gl_pWorldMarket->IsSystemReady());

		gl_pWorldMarket->SetFinnhubSymbolUpdated(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskCheckSystemReady());
		EXPECT_FALSE(gl_pWorldMarket->IsSystemReady());

		gl_pWorldMarket->SetFinnhubForexExchangeUpdated(true);
		EXPECT_FALSE(gl_pWorldMarket->TaskCheckSystemReady());
		EXPECT_FALSE(gl_pWorldMarket->IsSystemReady());

		gl_pWorldMarket->SetFinnhubForexSymbolUpdated(true);
		EXPECT_TRUE(gl_pWorldMarket->TaskCheckSystemReady());
		EXPECT_TRUE(gl_pWorldMarket->IsSystemReady());
		CString str = gl_systemMessage.PopInformationMessage();
		EXPECT_STREQ(str, _T("世界市场初始化完毕"));
	}

	TEST_F(CWorldMarketTest, TestRebuildEPSSurprise) {
		CWorldStockPtr pStock = nullptr;
		for (int i = 0; i < gl_pWorldMarket->GetTotalStock(); i++) {
			pStock = gl_pWorldMarket->GetStock(i);
			pStock->SetLastEPSSurpriseUpdateDate(20200101);
			pStock->m_fEPSSurpriseUpdated = true;
		}
		gl_pWorldMarket->SetFinnhubEPSSurpriseUpdated(true);

		EXPECT_TRUE(gl_pWorldMarket->RebuildEPSSurprise());

		for (int i = 0; i < gl_pWorldMarket->GetTotalStock(); i++) {
			pStock = gl_pWorldMarket->GetStock(i);
			EXPECT_EQ(pStock->GetLastEPSSurpriseUpdateDate(), 19800101);
			EXPECT_FALSE(pStock->m_fEPSSurpriseUpdated);
		}
		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubEPSSurpriseUpdated());
	}

	TEST_F(CWorldMarketTest, TestRebuildPeer) {
		CWorldStockPtr pStock = nullptr;
		for (int i = 0; i < gl_pWorldMarket->GetTotalStock(); i++) {
			pStock = gl_pWorldMarket->GetStock(i);
			pStock->SetPeerUpdateDate(20200101);
			pStock->SetPeerUpdated(true);
			pStock->SetUpdateProfileDB(false);
		}
		gl_pWorldMarket->SetFinnhubPeerUpdated(true);

		EXPECT_TRUE(gl_pWorldMarket->RebuildPeer());

		for (int i = 0; i < gl_pWorldMarket->GetTotalStock(); i++) {
			pStock = gl_pWorldMarket->GetStock(i);
			EXPECT_EQ(pStock->GetPeerUpdateDate(), 19800101);
			EXPECT_FALSE(pStock->IsPeerUpdated());
			EXPECT_TRUE(pStock->IsUpdateProfileDB());
		}
		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubPeerUpdated());
	}

	TEST_F(CWorldMarketTest, TestRebuildStockDayLine) {
		CWorldStockPtr pStock = nullptr;
		for (int i = 0; i < gl_pWorldMarket->GetTotalStock(); i++) {
			pStock = gl_pWorldMarket->GetStock(i);
			pStock->SetIPOStatus(__STOCK_IPOED__);
			pStock->SetDayLineStartDate(20200101);
			pStock->SetDayLineEndDate(20200101);
			pStock->SetDayLineNeedUpdate(false);
			pStock->SetUpdateProfileDB(false);
		}
		gl_pWorldMarket->SetFinnhubStockProfileUpdated(true);

		EXPECT_TRUE(gl_pWorldMarket->RebuildStockDayLineDB());

		for (int i = 0; i < gl_pWorldMarket->GetTotalStock(); i++) {
			pStock = gl_pWorldMarket->GetStock(i);
			EXPECT_EQ(pStock->GetDayLineStartDate(), 29900101);
			EXPECT_EQ(pStock->GetDayLineEndDate(), 19800101);
			EXPECT_TRUE(pStock->IsDayLineNeedUpdate());
			EXPECT_TRUE(pStock->IsUpdateProfileDB());
		}
		EXPECT_FALSE(gl_pWorldMarket->IsFinnhubStockProfileUpdated());
	}

	TEST_F(CWorldMarketTest, TestUpdateTiingoIndustry) {
		EXPECT_THAT(gl_pWorldMarket->UpdateTiingoIndustry(), IsFalse());
	}

	TEST_F(CWorldMarketTest, TestUpdateSICIndustry) {
		EXPECT_THAT(gl_pWorldMarket->UpdateSICIndustry(), IsFalse());
	}

	TEST_F(CWorldMarketTest, TestUpdateNaicsIndustry) {
		EXPECT_THAT(gl_pWorldMarket->UpdateNaicsIndustry(), IsFalse());
	}
}