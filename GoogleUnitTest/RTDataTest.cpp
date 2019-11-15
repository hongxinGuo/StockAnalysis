#include"stdafx.h"
#include"pch.h"
#include"globedef.h"

#include"RTData.h"
#include"SinaRTWebData.h"

using namespace testing;

namespace StockAnalysisTest {
  TEST(CStockRTDataTest, TestInitialize) {
    ASSERT_FALSE(gl_fNormalMode);
    CRTData RTData;
    EXPECT_EQ(RTData.GetTransactionTime(), 0);
    EXPECT_EQ(RTData.GetMarket(), 0);
    EXPECT_STREQ(RTData.GetStockCode(), _T(""));
    EXPECT_STREQ(RTData.GetStockName(), _T(""));
    EXPECT_EQ(RTData.GetOpen(), 0);
    EXPECT_EQ(RTData.GetLastClose(), 0);
    EXPECT_EQ(RTData.GetNew(), 0);
    EXPECT_EQ(RTData.GetHigh(), 0);
    EXPECT_EQ(RTData.GetLow(), 0);
    EXPECT_EQ(RTData.GetBuy(), 0);
    EXPECT_EQ(RTData.GetSell(), 0);
    EXPECT_EQ(RTData.GetVolume(), 0);
    EXPECT_EQ(RTData.GetAmount(), 0);
    for (int i = 0; i < 5; i++) {
      EXPECT_EQ(RTData.GetVBuy(i), 0);
      EXPECT_EQ(RTData.GetPBuy(i), 0);
      EXPECT_EQ(RTData.GetVSell(i), 0);
      EXPECT_EQ(RTData.GetPSell(i), 0);
    }
    EXPECT_FALSE(RTData.IsActive());
  }

  TEST(CRTDataTest, TestIsDataTimeAtCurrentDay) {
    tm tm_;
    tm_.tm_year = 2019 - 1900;
    tm_.tm_mon = 10;
    tm_.tm_mday = 7; // 2019年11月7日是星期四。
    tm_.tm_hour = 12;
    tm_.tm_min = 0;
    tm_.tm_sec = 0;
    time_t tt = mktime(&tm_);
    gl_systemTime.Sett_time(tt);
    CRTData data;
    data.SetTransactionTime(tt);
    EXPECT_TRUE(data.IsDataTimeAtCurrentTradingDay());
    data.SetTransactionTime(tt - 3600 * 24);
    EXPECT_TRUE(data.IsDataTimeAtCurrentTradingDay());
    data.SetTransactionTime(tt - 3600 * 24 - 1);
    EXPECT_FALSE(data.IsDataTimeAtCurrentTradingDay());
    data.SetTransactionTime(tt + 3600);
    EXPECT_TRUE(data.IsDataTimeAtCurrentTradingDay());
    data.SetTransactionTime(tt + 3600 + 1);
    EXPECT_FALSE(data.IsDataTimeAtCurrentTradingDay());
    data.SetTransactionTime(tt + 2 * 3600 * 24 + 1);
    EXPECT_FALSE(data.IsDataTimeAtCurrentTradingDay());

    tm_.tm_year = 2019 - 1900;
    tm_.tm_mon = 10;
    tm_.tm_mday = 9; // 2019年11月9日是星期六。
    tm_.tm_hour = 12;
    tm_.tm_min = 0;
    tm_.tm_sec = 0;
    tt = mktime(&tm_);
    gl_systemTime.Sett_time(tt);
    data.SetTransactionTime(tt);
    EXPECT_TRUE(data.IsDataTimeAtCurrentTradingDay());
    data.SetTransactionTime(tt - 2 * 3600 * 24);
    EXPECT_TRUE(data.IsDataTimeAtCurrentTradingDay());
    data.SetTransactionTime(tt - 2 * 3600 * 24 - 1);
    EXPECT_FALSE(data.IsDataTimeAtCurrentTradingDay());
    data.SetTransactionTime(tt + 3600);
    EXPECT_TRUE(data.IsDataTimeAtCurrentTradingDay());
    data.SetTransactionTime(tt + 3600 + 1);
    EXPECT_FALSE(data.IsDataTimeAtCurrentTradingDay());

    tm_.tm_year = 2019 - 1900;
    tm_.tm_mon = 10;
    tm_.tm_mday = 10; // 2019年11月10日是星期日。
    tm_.tm_hour = 12;
    tm_.tm_min = 0;
    tm_.tm_sec = 0;
    tt = mktime(&tm_);
    gl_systemTime.Sett_time(tt);
    data.SetTransactionTime(tt);
    EXPECT_TRUE(data.IsDataTimeAtCurrentTradingDay());
    data.SetTransactionTime(tt - 3 * 3600 * 24);
    EXPECT_TRUE(data.IsDataTimeAtCurrentTradingDay());
    data.SetTransactionTime(tt - 3 * 3600 * 24 - 1);
    EXPECT_FALSE(data.IsDataTimeAtCurrentTradingDay());
    data.SetTransactionTime(tt + 3600);
    EXPECT_TRUE(data.IsDataTimeAtCurrentTradingDay());
    data.SetTransactionTime(tt + 3600 + 1);
    EXPECT_FALSE(data.IsDataTimeAtCurrentTradingDay());
    data.SetTransactionTime(tt + 2 * 3600 * 24 + 1);
    EXPECT_FALSE(data.IsDataTimeAtCurrentTradingDay());
  }

  TEST(CRTDataTest, TestMapNeteaseSymbolToIndex) {
    CRTData rtData;
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("time")), 1);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("code")), 2);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("name")), 3);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("type")), 4);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("symbol")), 5);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("status")), 6);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("update")), 7);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("open")), 10);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("yestclose")), 11);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("high")), 12);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("low")), 13);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("price")), 14);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("volume")), 15);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("precloseioev")), 16);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("bid1")), 20);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("bid2")), 21);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("bid3")), 22);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("bid4")), 23);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("bid5")), 24);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("bidvol1")), 30);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("bidvol2")), 31);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("bidvol3")), 32);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("bidvol4")), 33);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("bidvol5")), 34);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("ask1")), 40);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("ask2")), 41);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("ask3")), 42);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("ask4")), 43);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("ask5")), 44);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("askvol1")), 50);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("askvol2")), 51);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("askvol3")), 52);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("askvol4")), 53);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("askvol5")), 54);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("percent")), 60);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("updown")), 61);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("arrow")), 62);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("turnover")), 63);
    EXPECT_EQ(rtData.GetNeteaseSymbolIndex(_T("turnvoer")), 0); // 错误的符号返回值皆为零
  }

  struct NeteaseRTData {
    NeteaseRTData(long lCount, CString strIndex, long lIndex, CString strValue) {
      m_lCount = lCount;
      m_lIndex = lIndex;
      m_strIndex = strIndex;
      m_strValue = strValue;
    }
  public:
    long m_lCount;
    long m_lIndex;
    CString m_strIndex;
    CString m_strValue;
  };

  NeteaseRTData neteaseData1(1, _T("time"), 1, _T("2019/01/01 12:20:30"));
  NeteaseRTData neteaseData2(2, _T("code"), 2, _T("0601872"));
  NeteaseRTData neteaseData3(3, _T("name"), 3, _T("招商轮船"));
  NeteaseRTData neteaseData4(4, _T("type"), 4, _T("SH"));
  NeteaseRTData neteaseData5(5, _T("symbol"), 5, _T("601872"));
  NeteaseRTData neteaseData6(6, _T("status"), 6, _T("0"));
  NeteaseRTData neteaseData7(7, _T("update"), 7, _T("2019/10/11 01:10:02"));
  NeteaseRTData neteaseData10(10, _T("open"), 10, _T("5.7"));
  NeteaseRTData neteaseData11(11, _T("yestclose"), 11, _T("5.86"));
  NeteaseRTData neteaseData12(12, _T("high"), 12, _T("5.90"));
  NeteaseRTData neteaseData13(13, _T("low"), 13, _T("5.45"));
  NeteaseRTData neteaseData14(14, _T("price"), 14, _T("5.55"));
  NeteaseRTData neteaseData15(15, _T("volume"), 15, _T("10101010"));
  NeteaseRTData neteaseData16(16, _T("precloseioev"), 16, _T("10.123"));
  NeteaseRTData neteaseData20(20, _T("bid1"), 20, _T("5.54"));
  NeteaseRTData neteaseData21(21, _T("bid2"), 21, _T("5.53"));
  NeteaseRTData neteaseData22(22, _T("bid3"), 22, _T("5.52"));
  NeteaseRTData neteaseData23(23, _T("bid4"), 23, _T("5.51"));
  NeteaseRTData neteaseData24(24, _T("bid5"), 24, _T("5.50"));
  NeteaseRTData neteaseData30(30, _T("bidvol1"), 30, _T("10101"));
  NeteaseRTData neteaseData31(31, _T("bidvol2"), 31, _T("20202"));
  NeteaseRTData neteaseData32(32, _T("bidvol3"), 32, _T("30303"));
  NeteaseRTData neteaseData33(33, _T("bidvol4"), 33, _T("40404"));
  NeteaseRTData neteaseData34(34, _T("bidvol5"), 34, _T("50505"));
  NeteaseRTData neteaseData40(40, _T("ask1"), 40, _T("5.55"));
  NeteaseRTData neteaseData41(41, _T("ask2"), 41, _T("5.56"));
  NeteaseRTData neteaseData42(42, _T("ask3"), 42, _T("5.57"));
  NeteaseRTData neteaseData43(43, _T("ask4"), 43, _T("5.58"));
  NeteaseRTData neteaseData44(44, _T("ask5"), 44, _T("5.59"));
  NeteaseRTData neteaseData50(50, _T("askvol1"), 50, _T("101010"));
  NeteaseRTData neteaseData51(51, _T("askvol2"), 51, _T("202020"));
  NeteaseRTData neteaseData52(52, _T("askvol3"), 52, _T("303030"));
  NeteaseRTData neteaseData53(53, _T("askvol4"), 53, _T("404040"));
  NeteaseRTData neteaseData54(54, _T("askvol5"), 54, _T("505050"));
  NeteaseRTData neteaseData60(60, _T("percent"), 60, _T("0.02345"));
  NeteaseRTData neteaseData61(61, _T("updown"), 61, _T("0.21"));
  NeteaseRTData neteaseData62(62, _T("arrow"), 62, _T("2191"));
  NeteaseRTData neteaseData63(63, _T("turnover"), 63, _T("4433221100"));

  class NeteaseRTDataTest : public::testing::TestWithParam<NeteaseRTData*> {
  protected:
    void SetUp(void) override {
      ASSERT_FALSE(gl_fNormalMode);
      NeteaseRTData* pData = GetParam();
      m_iCount = pData->m_lCount;
      m_strSymbol = pData->m_strIndex;
      m_lIndex = pData->m_lIndex;
      m_strValue = pData->m_strValue;
    }

    void TearDown(void) override {
      // clearup
    }

  public:
    int m_iCount;
    CString m_strSymbol;
    long m_lIndex;
    CString m_strValue;
    CRTData m_RTData;
  };

  INSTANTIATE_TEST_CASE_P(TestNeteaseRTData, NeteaseRTDataTest, testing::Values(&neteaseData1, &neteaseData2, &neteaseData3,
    &neteaseData4, &neteaseData5, &neteaseData6, &neteaseData7,
    &neteaseData10, &neteaseData11, &neteaseData12, &neteaseData13, &neteaseData14, &neteaseData15,
    &neteaseData20, &neteaseData21, &neteaseData22, &neteaseData23, &neteaseData24,
    &neteaseData30, &neteaseData31, &neteaseData32, &neteaseData33, &neteaseData34,
    &neteaseData40, &neteaseData41, &neteaseData42, &neteaseData43, &neteaseData44,
    &neteaseData50, &neteaseData51, &neteaseData52, &neteaseData53, &neteaseData54,
    &neteaseData60, &neteaseData61, &neteaseData62, &neteaseData63));

  TEST_P(NeteaseRTDataTest, TestGetNeteaseSymbolIndex) {
    CString strFormat;
    time_t tt2;
    long lIndex = m_RTData.GetNeteaseSymbolIndex(m_strSymbol);
    EXPECT_EQ(lIndex, m_lIndex);
    m_RTData.SetValue(lIndex, m_strValue);
    switch (m_iCount) {
    case 0: // 出错
      break;
    case 1: // time
      EXPECT_STREQ(m_strSymbol, _T("time"));
      break;
    case 2: // code
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh601872"));
      break;
    case 3: // name
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_STREQ(m_RTData.GetStockName(), _T("招商轮船"));
      break;
    case 4: // name
      break;
    case 5:
      break;
    case 6:
      break;
    case 7:
      time_t tt;
      tm tm_;
      int year, month, day, hour, minute, second;
      strFormat = _T("%04d/%02d/%02d %02d:%02d:%02d");
      sscanf_s(m_strValue.GetBuffer(), strFormat.GetBuffer(), &year, &month, &day, &hour, &minute, &second);
      tm_.tm_year = year - 1900;
      tm_.tm_mon = month - 1;
      tm_.tm_mday = day;
      tm_.tm_hour = hour;
      tm_.tm_min = minute;
      tm_.tm_sec = second;
      tm_.tm_isdst = 0;
      tt = mktime(&tm_);
      m_RTData.SetValue(lIndex, m_strValue);
      tt2 = m_RTData.GetTransactionTime();
      EXPECT_EQ(tt, tt2);
      break;
    case 10: // open
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetOpen(), 5700);
      break;
    case 11: // yestclose
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetLastClose(), 5860);
      break;
    case 12: // high
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetHigh(), 5900);
      break;
    case 13: // low
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetLow(), 5450);
      break;
    case 14: // price
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetNew(), 5550);
      break;
    case 15: // volume
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetVolume(), 10101010);
      break;
    case 16: // precloseioev
      break;
    case 20: // bid1
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetPBuy(0), 5540);
      break;
    case 21: // bid2
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetPBuy(1), 5530);
      break;
    case 22: // bid3
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetPBuy(2), 5520);
      break;
    case 23: // bid4
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetPBuy(3), 5510);
      break;
    case 24: // bid5
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetPBuy(4), 5500);
      break;
    case 30: // bidvol1
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetVBuy(0), 10101);
      break;
    case 31: // bidvol2
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetVBuy(1), 20202);
      break;
    case 32: // bidvol3
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetVBuy(2), 30303);
      break;
    case 33: // bidvol4
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetVBuy(3), 40404);
      break;
    case 34: // bidvol5
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetVBuy(4), 50505);
      break;
    case 40: // ask1
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetPSell(0), 5550);
      break;
    case 41: // ask2
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetPSell(1), 5560);
      break;
    case 42: // ask3
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetPSell(2), 5570);
      break;
    case 43: // ask4
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetPSell(3), 5580);
      break;
    case 44: // ask5
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetPSell(4), 5590);
      break;
    case 50: // askvol1
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetVSell(0), 101010);
      break;
    case 51: // askvol2
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetVSell(1), 202020);
      break;
    case 52: // askvol3
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetVSell(2), 303030);
      break;
    case 53: // askvol4
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetVSell(3), 404040);
      break;
    case 54: // askvol5
      m_RTData.SetValue(lIndex, m_strValue);
      EXPECT_EQ(m_RTData.GetVSell(4), 505050);
      break;
    case 60: // percent
    case 61: // updown
    case 62: // arrow
    case 63: // turnover
      break;
    default:
      break;
    }
  }

  struct SinaRTData {
    SinaRTData(int count, CString Data) {
      m_iCount = count;
      m_strData = Data;
    }
  public:
    int m_iCount;
    CString m_strData;
  };

  // 无错误数据
  SinaRTData Data1(0, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 所有的价格皆为0
  SinaRTData Data2(1, _T("var hq_str_sz002385=\"平安银行,0,0,0,0,0,0,0,21606007,248901949.000,19900,0,54700,0,561500,0,105600,0,172400,0,259981,0,206108,0,325641,0,215109,0,262900,0,2019-07-16,15:00:00,00\";\n"));
  // 所有的数量皆为零
  SinaRTData Data3(2, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,0,0,0,11.540,0,11.530,0,11.520,0,11.510,0,11.500,0,11.550,0,11.560,0,11.570,0,11.580,0,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data4(3, _T("var hq_str_sh600000=\"浦发银行,-11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data5(4, _T("var hq_str_sh600000=\"浦发银行,11.510,-11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data6(5, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,-11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data7(6, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,-11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data8(7, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,-11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data9(8, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,-11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data10(9, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,-11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data11(10, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,-21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data12(11, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,-248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data13(12, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,-19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data14(13, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,-11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data15(14, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,-54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data16(15, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,-11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data17(16, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,-561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data18(17, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,-11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data19(18, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,-105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data20(19, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,-11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data21(20, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,-172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data22(21, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,-11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data23(22, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,-259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data24(23, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,-11.550,206108,11.560,325641,11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data25(24, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,-206108,11.560,325641,11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data26(25, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,-11.560,325641,11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data27(26, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,-325641,11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data28(27, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,-11.570,215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data29(28, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,-215109,11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data30(29, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,-11.580,262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data31(30, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,-262900,11.590,2019-07-16,15:00:00,00\";\n"));
  // 出现负值
  SinaRTData Data32(31, _T("var hq_str_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,-11.590,2019-07-16,15:00:00,00\";\n"));
  // 没有实时数据
  SinaRTData Data33(32, _T("var hq_str_sz000001=\"\";\n"));
  // 格式出错（前缀）
  SinaRTData Data34(33, _T("var hq_st_sh600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,-11.590,2019-07-16,15:00:00,00\";\n"));
  // 格式出错（）
  SinaRTData Data35(34, _T("var hq_str_sa600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,-11.590,2019-07-16,15:00:00,00\";\n"));
  // 格式出错（不是sh或者sz）
  SinaRTData Data36(35, _T("var hq_str_sa600000=\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,-11.590,2019-07-16,15:00:00,00\";\n"));
  // 格式出错（股票代码后面不是'='号
  SinaRTData Data37(36, _T("var hq_str_sa600000a\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,-11.590,2019-07-16,15:00:00,00\";\n"));
  // 格式出错（'='好后面不是'"'号
  SinaRTData Data38(37, _T("var hq_str_sa600000a'浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,-11.590,2019-07-16,15:00:00,00\";\n"));
  // 格式出错（股票代码后面不是'='号
  SinaRTData Data39(38, _T("var hq_str_sa600000a\"浦发银行,11.510,11.490,11.560,11.570,11.440,11.540,11.550,21606007,248901949.000,19900,11.540,54700,11.530,561500,11.520,105600,11.510,172400,11.500,259981,11.550,206108,11.560,325641,11.570,215109,11.580,262900,-11.590,2019-07-16,15:00:00,00\";\n"));

  class CalculateSinaRTDataTest : public::testing::TestWithParam<SinaRTData*> {
  protected:
    void SetUp(void) override {
      ASSERT_FALSE(gl_fNormalMode);
      SinaRTData* pData = GetParam();
      m_iCount = pData->m_iCount;
      m_lStringLength = pData->m_strData.GetLength();
      m_pData = m_SinaRTWebData.GetBufferAddr();
      for (int i = 0; i < m_lStringLength; i++) {
        m_pData[i] = pData->m_strData[i];
      }
      m_SinaRTWebData.ResetCurrentPos();
      for (int i = 0; i < 5; i++) {
        m_RTData.SetPBuy(i, -1);
        m_RTData.SetPSell(i, -1);
        m_RTData.SetVBuy(i, -1);
        m_RTData.SetVSell(i, -1);
      }
      m_RTData.SetAmount(-1);
      m_RTData.SetVolume(-1);
      m_RTData.SetOpen(-1);
      m_RTData.SetNew(-1);
      m_RTData.SetLastClose(-1);
      m_RTData.SetHigh(-1);
      m_RTData.SetLow(-1);
      m_RTData.SetSell(-1);
      m_RTData.SetBuy(-1);
    }

    void TearDown(void) override {
      // clearup
    }

  public:
    int m_iCount;
    char* m_pData;
    long m_lStringLength;
    CSinaRTWebData m_SinaRTWebData;
    CRTData m_RTData;
  };

  INSTANTIATE_TEST_CASE_P(TestSinaRTData, CalculateSinaRTDataTest, testing::Values(&Data1, &Data2, &Data3,
    &Data4, &Data5, &Data6, &Data7, &Data8, &Data9, &Data10,
    &Data11, &Data12, &Data13, &Data14, &Data15, &Data16, &Data17, &Data18, &Data19, &Data20,
    &Data21, &Data22, &Data23, &Data24, &Data25, &Data26, &Data27, &Data28, &Data29, &Data30,
    &Data31, &Data32, &Data33, &Data34, &Data35, &Data36, &Data37, &Data38
  ));

  TEST_P(CalculateSinaRTDataTest, TestSinaRTData) {
    bool fSucceed = m_RTData.ReadSinaData(&m_SinaRTWebData);
    time_t ttime;
    tm tm_;
    tm_.tm_year = 2019 - 1900;
    tm_.tm_mon = 7 - 1;
    tm_.tm_mday = 16;
    tm_.tm_hour = 15;
    tm_.tm_min = 0;
    tm_.tm_sec = 0;
    ttime = mktime(&tm_);
    switch (m_iCount) {
    case 0:
      EXPECT_TRUE(fSucceed); // 没有错误
      EXPECT_EQ(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      EXPECT_EQ(m_RTData.GetBuy(), 11540);
      EXPECT_EQ(m_RTData.GetSell(), 11550);
      EXPECT_EQ(m_RTData.GetVolume(), 21606007);
      EXPECT_EQ(m_RTData.GetAmount(), 248901949);
      EXPECT_EQ(m_RTData.GetVBuy(0), 19900);
      EXPECT_EQ(m_RTData.GetPBuy(0), 11540);
      EXPECT_EQ(m_RTData.GetVBuy(1), 54700);
      EXPECT_EQ(m_RTData.GetPBuy(1), 11530);
      EXPECT_EQ(m_RTData.GetVBuy(2), 561500);
      EXPECT_EQ(m_RTData.GetPBuy(2), 11520);
      EXPECT_EQ(m_RTData.GetVBuy(3), 105600);
      EXPECT_EQ(m_RTData.GetPBuy(3), 11510);
      EXPECT_EQ(m_RTData.GetVBuy(4), 172400);
      EXPECT_EQ(m_RTData.GetPBuy(4), 11500);
      EXPECT_EQ(m_RTData.GetVSell(0), 259981);
      EXPECT_EQ(m_RTData.GetPSell(0), 11550);
      EXPECT_EQ(m_RTData.GetVSell(1), 206108);
      EXPECT_EQ(m_RTData.GetPSell(1), 11560);
      EXPECT_EQ(m_RTData.GetVSell(2), 325641);
      EXPECT_EQ(m_RTData.GetPSell(2), 11570);
      EXPECT_EQ(m_RTData.GetVSell(3), 215109);
      EXPECT_EQ(m_RTData.GetPSell(3), 11580);
      EXPECT_EQ(m_RTData.GetVSell(4), 262900);
      EXPECT_EQ(m_RTData.GetPSell(4), 11590);
      EXPECT_EQ(m_RTData.GetTransactionTime(), ttime);
      break;
    case 1:
      EXPECT_TRUE(fSucceed); // 没有错误
      EXPECT_EQ(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sz002385"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("平安银行"));
      EXPECT_EQ(m_RTData.GetOpen(), -1);
      EXPECT_EQ(m_RTData.GetLastClose(), -1);
      EXPECT_EQ(m_RTData.GetNew(), -1);
      EXPECT_EQ(m_RTData.GetHigh(), -1);
      EXPECT_EQ(m_RTData.GetLow(), -1);
      EXPECT_EQ(m_RTData.GetBuy(), -1);
      EXPECT_EQ(m_RTData.GetSell(), -1);
      EXPECT_EQ(m_RTData.GetVolume(), 21606007);
      EXPECT_EQ(m_RTData.GetAmount(), 248901949);
      EXPECT_EQ(m_RTData.GetVBuy(0), 19900);
      EXPECT_EQ(m_RTData.GetPBuy(0), -1);
      EXPECT_EQ(m_RTData.GetVBuy(1), 54700);
      EXPECT_EQ(m_RTData.GetPBuy(1), -1);
      EXPECT_EQ(m_RTData.GetVBuy(2), 561500);
      EXPECT_EQ(m_RTData.GetPBuy(2), -1);
      EXPECT_EQ(m_RTData.GetVBuy(3), 105600);
      EXPECT_EQ(m_RTData.GetPBuy(3), -1);
      EXPECT_EQ(m_RTData.GetVBuy(4), 172400);
      EXPECT_EQ(m_RTData.GetPBuy(4), -1);
      EXPECT_EQ(m_RTData.GetVSell(0), 259981);
      EXPECT_EQ(m_RTData.GetPSell(0), -1);
      EXPECT_EQ(m_RTData.GetVSell(1), 206108);
      EXPECT_EQ(m_RTData.GetPSell(1), -1);
      EXPECT_EQ(m_RTData.GetVSell(2), 325641);
      EXPECT_EQ(m_RTData.GetPSell(2), -1);
      EXPECT_EQ(m_RTData.GetVSell(3), 215109);
      EXPECT_EQ(m_RTData.GetPSell(3), -1);
      EXPECT_EQ(m_RTData.GetVSell(4), 262900);
      EXPECT_EQ(m_RTData.GetPSell(4), -1);
      EXPECT_EQ(m_RTData.GetTransactionTime(), ttime);
      break;
    case 2:
      EXPECT_TRUE(fSucceed); // 没有错误
      EXPECT_EQ(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      EXPECT_EQ(m_RTData.GetBuy(), 11540);
      EXPECT_EQ(m_RTData.GetSell(), 11550);
      EXPECT_EQ(m_RTData.GetVolume(), -1);
      EXPECT_EQ(m_RTData.GetAmount(), -1);
      EXPECT_EQ(m_RTData.GetVBuy(0), -1);
      EXPECT_EQ(m_RTData.GetPBuy(0), 11540);
      EXPECT_EQ(m_RTData.GetVBuy(1), -1);
      EXPECT_EQ(m_RTData.GetPBuy(1), 11530);
      EXPECT_EQ(m_RTData.GetVBuy(2), -1);
      EXPECT_EQ(m_RTData.GetPBuy(2), 11520);
      EXPECT_EQ(m_RTData.GetVBuy(3), -1);
      EXPECT_EQ(m_RTData.GetPBuy(3), 11510);
      EXPECT_EQ(m_RTData.GetVBuy(4), -1);
      EXPECT_EQ(m_RTData.GetPBuy(4), 11500);
      EXPECT_EQ(m_RTData.GetVSell(0), -1);
      EXPECT_EQ(m_RTData.GetPSell(0), 11550);
      EXPECT_EQ(m_RTData.GetVSell(1), -1);
      EXPECT_EQ(m_RTData.GetPSell(1), 11560);
      EXPECT_EQ(m_RTData.GetVSell(2), -1);
      EXPECT_EQ(m_RTData.GetPSell(2), 11570);
      EXPECT_EQ(m_RTData.GetVSell(3), -1);
      EXPECT_EQ(m_RTData.GetPSell(3), 11580);
      EXPECT_EQ(m_RTData.GetVSell(4), -1);
      EXPECT_EQ(m_RTData.GetPSell(4), 11590);
      EXPECT_EQ(m_RTData.GetTransactionTime(), ttime);
      break;
    case 3:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      break;
    case 4:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      break;
    case 5:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      break;
    case 6:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      break;
    case 7:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      break;
    case 8:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      break;
    case 9:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      EXPECT_EQ(m_RTData.GetBuy(), 11540);
      break;
    case 10:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      EXPECT_EQ(m_RTData.GetBuy(), 11540);
      EXPECT_EQ(m_RTData.GetSell(), 11550);
      break;
    case 11:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      EXPECT_EQ(m_RTData.GetBuy(), 11540);
      EXPECT_EQ(m_RTData.GetSell(), 11550);
      EXPECT_EQ(m_RTData.GetVolume(), 21606007);
      break;
    case 12:
      EXPECT_FALSE(fSucceed); // 没有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      EXPECT_EQ(m_RTData.GetBuy(), 11540);
      EXPECT_EQ(m_RTData.GetSell(), 11550);
      EXPECT_EQ(m_RTData.GetVolume(), 21606007);
      EXPECT_EQ(m_RTData.GetAmount(), 248901949);
      break;
    case 13:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      EXPECT_EQ(m_RTData.GetBuy(), 11540);
      EXPECT_EQ(m_RTData.GetSell(), 11550);
      EXPECT_EQ(m_RTData.GetVolume(), 21606007);
      EXPECT_EQ(m_RTData.GetAmount(), 248901949);
      EXPECT_EQ(m_RTData.GetVBuy(0), 19900);
      EXPECT_EQ(m_RTData.GetVBuy(0), 19900);
      break;
    case 14:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      EXPECT_EQ(m_RTData.GetBuy(), 11540);
      EXPECT_EQ(m_RTData.GetSell(), 11550);
      EXPECT_EQ(m_RTData.GetVolume(), 21606007);
      EXPECT_EQ(m_RTData.GetAmount(), 248901949);
      EXPECT_EQ(m_RTData.GetVBuy(0), 19900);
      EXPECT_EQ(m_RTData.GetPBuy(0), 11540);
      break;
    case 15:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      EXPECT_EQ(m_RTData.GetBuy(), 11540);
      EXPECT_EQ(m_RTData.GetSell(), 11550);
      EXPECT_EQ(m_RTData.GetVolume(), 21606007);
      EXPECT_EQ(m_RTData.GetAmount(), 248901949);
      EXPECT_EQ(m_RTData.GetVBuy(0), 19900);
      EXPECT_EQ(m_RTData.GetPBuy(0), 11540);
      EXPECT_EQ(m_RTData.GetVBuy(1), 54700);
      break;
    case 16:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      EXPECT_EQ(m_RTData.GetBuy(), 11540);
      EXPECT_EQ(m_RTData.GetSell(), 11550);
      EXPECT_EQ(m_RTData.GetVolume(), 21606007);
      EXPECT_EQ(m_RTData.GetAmount(), 248901949);
      EXPECT_EQ(m_RTData.GetVBuy(0), 19900);
      EXPECT_EQ(m_RTData.GetPBuy(0), 11540);
      EXPECT_EQ(m_RTData.GetVBuy(1), 54700);
      EXPECT_EQ(m_RTData.GetPBuy(1), 11530);
      break;
    case 17:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      EXPECT_EQ(m_RTData.GetBuy(), 11540);
      EXPECT_EQ(m_RTData.GetSell(), 11550);
      EXPECT_EQ(m_RTData.GetVolume(), 21606007);
      EXPECT_EQ(m_RTData.GetAmount(), 248901949);
      EXPECT_EQ(m_RTData.GetVBuy(0), 19900);
      EXPECT_EQ(m_RTData.GetPBuy(0), 11540);
      EXPECT_EQ(m_RTData.GetVBuy(1), 54700);
      EXPECT_EQ(m_RTData.GetPBuy(1), 11530);
      EXPECT_EQ(m_RTData.GetVBuy(2), 561500);
      break;
    case 18:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      EXPECT_EQ(m_RTData.GetBuy(), 11540);
      EXPECT_EQ(m_RTData.GetSell(), 11550);
      EXPECT_EQ(m_RTData.GetVolume(), 21606007);
      EXPECT_EQ(m_RTData.GetAmount(), 248901949);
      EXPECT_EQ(m_RTData.GetVBuy(0), 19900);
      EXPECT_EQ(m_RTData.GetPBuy(0), 11540);
      EXPECT_EQ(m_RTData.GetVBuy(1), 54700);
      EXPECT_EQ(m_RTData.GetPBuy(1), 11530);
      EXPECT_EQ(m_RTData.GetVBuy(2), 561500);
      EXPECT_EQ(m_RTData.GetPBuy(2), 11520);
      break;
    case 19:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      EXPECT_EQ(m_RTData.GetBuy(), 11540);
      EXPECT_EQ(m_RTData.GetSell(), 11550);
      EXPECT_EQ(m_RTData.GetVolume(), 21606007);
      EXPECT_EQ(m_RTData.GetAmount(), 248901949);
      EXPECT_EQ(m_RTData.GetVBuy(0), 19900);
      EXPECT_EQ(m_RTData.GetPBuy(0), 11540);
      EXPECT_EQ(m_RTData.GetVBuy(1), 54700);
      EXPECT_EQ(m_RTData.GetPBuy(1), 11530);
      EXPECT_EQ(m_RTData.GetVBuy(2), 561500);
      EXPECT_EQ(m_RTData.GetPBuy(2), 11520);
      EXPECT_EQ(m_RTData.GetVBuy(3), 105600);
      EXPECT_EQ(m_RTData.GetVBuy(3), 105600);
      break;
    case 20:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      EXPECT_EQ(m_RTData.GetBuy(), 11540);
      EXPECT_EQ(m_RTData.GetSell(), 11550);
      EXPECT_EQ(m_RTData.GetVolume(), 21606007);
      EXPECT_EQ(m_RTData.GetAmount(), 248901949);
      EXPECT_EQ(m_RTData.GetVBuy(0), 19900);
      EXPECT_EQ(m_RTData.GetPBuy(0), 11540);
      EXPECT_EQ(m_RTData.GetVBuy(1), 54700);
      EXPECT_EQ(m_RTData.GetPBuy(1), 11530);
      EXPECT_EQ(m_RTData.GetVBuy(2), 561500);
      EXPECT_EQ(m_RTData.GetPBuy(2), 11520);
      EXPECT_EQ(m_RTData.GetVBuy(3), 105600);
      EXPECT_EQ(m_RTData.GetPBuy(3), 11510);
      EXPECT_EQ(m_RTData.GetVBuy(3), 105600);
      EXPECT_EQ(m_RTData.GetPBuy(3), 11510);
      break;
    case 21:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      EXPECT_EQ(m_RTData.GetBuy(), 11540);
      EXPECT_EQ(m_RTData.GetSell(), 11550);
      EXPECT_EQ(m_RTData.GetVolume(), 21606007);
      EXPECT_EQ(m_RTData.GetAmount(), 248901949);
      EXPECT_EQ(m_RTData.GetVBuy(0), 19900);
      EXPECT_EQ(m_RTData.GetPBuy(0), 11540);
      EXPECT_EQ(m_RTData.GetVBuy(1), 54700);
      EXPECT_EQ(m_RTData.GetPBuy(1), 11530);
      EXPECT_EQ(m_RTData.GetVBuy(2), 561500);
      EXPECT_EQ(m_RTData.GetPBuy(2), 11520);
      EXPECT_EQ(m_RTData.GetVBuy(3), 105600);
      EXPECT_EQ(m_RTData.GetPBuy(3), 11510);
      EXPECT_EQ(m_RTData.GetVBuy(4), 172400);
      EXPECT_EQ(m_RTData.GetVBuy(3), 105600);
      EXPECT_EQ(m_RTData.GetPBuy(3), 11510);
      EXPECT_EQ(m_RTData.GetVBuy(4), 172400);
      break;
    case 22:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      EXPECT_EQ(m_RTData.GetBuy(), 11540);
      EXPECT_EQ(m_RTData.GetSell(), 11550);
      EXPECT_EQ(m_RTData.GetVolume(), 21606007);
      EXPECT_EQ(m_RTData.GetAmount(), 248901949);
      EXPECT_EQ(m_RTData.GetVBuy(0), 19900);
      EXPECT_EQ(m_RTData.GetPBuy(0), 11540);
      EXPECT_EQ(m_RTData.GetVBuy(1), 54700);
      EXPECT_EQ(m_RTData.GetPBuy(1), 11530);
      EXPECT_EQ(m_RTData.GetVBuy(2), 561500);
      EXPECT_EQ(m_RTData.GetPBuy(2), 11520);
      EXPECT_EQ(m_RTData.GetVBuy(3), 105600);
      EXPECT_EQ(m_RTData.GetPBuy(3), 11510);
      EXPECT_EQ(m_RTData.GetVBuy(4), 172400);
      EXPECT_EQ(m_RTData.GetPBuy(4), 11500);
      break;
    case 23:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      EXPECT_EQ(m_RTData.GetBuy(), 11540);
      EXPECT_EQ(m_RTData.GetSell(), 11550);
      EXPECT_EQ(m_RTData.GetVolume(), 21606007);
      EXPECT_EQ(m_RTData.GetAmount(), 248901949);
      EXPECT_EQ(m_RTData.GetVBuy(0), 19900);
      EXPECT_EQ(m_RTData.GetPBuy(0), 11540);
      EXPECT_EQ(m_RTData.GetVBuy(1), 54700);
      EXPECT_EQ(m_RTData.GetPBuy(1), 11530);
      EXPECT_EQ(m_RTData.GetVBuy(2), 561500);
      EXPECT_EQ(m_RTData.GetPBuy(2), 11520);
      EXPECT_EQ(m_RTData.GetVBuy(3), 105600);
      EXPECT_EQ(m_RTData.GetPBuy(3), 11510);
      EXPECT_EQ(m_RTData.GetVBuy(4), 172400);
      EXPECT_EQ(m_RTData.GetPBuy(4), 11500);
      EXPECT_EQ(m_RTData.GetVSell(0), 259981);
      break;
    case 24:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      EXPECT_EQ(m_RTData.GetBuy(), 11540);
      EXPECT_EQ(m_RTData.GetSell(), 11550);
      EXPECT_EQ(m_RTData.GetVolume(), 21606007);
      EXPECT_EQ(m_RTData.GetAmount(), 248901949);
      EXPECT_EQ(m_RTData.GetVBuy(0), 19900);
      EXPECT_EQ(m_RTData.GetPBuy(0), 11540);
      EXPECT_EQ(m_RTData.GetVBuy(1), 54700);
      EXPECT_EQ(m_RTData.GetPBuy(1), 11530);
      EXPECT_EQ(m_RTData.GetVBuy(2), 561500);
      EXPECT_EQ(m_RTData.GetPBuy(2), 11520);
      EXPECT_EQ(m_RTData.GetVBuy(3), 105600);
      EXPECT_EQ(m_RTData.GetPBuy(3), 11510);
      EXPECT_EQ(m_RTData.GetVBuy(4), 172400);
      EXPECT_EQ(m_RTData.GetPBuy(4), 11500);
      EXPECT_EQ(m_RTData.GetVSell(0), 259981);
      EXPECT_EQ(m_RTData.GetPSell(0), 11550);
      break;
    case 25:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      EXPECT_EQ(m_RTData.GetBuy(), 11540);
      EXPECT_EQ(m_RTData.GetSell(), 11550);
      EXPECT_EQ(m_RTData.GetVolume(), 21606007);
      EXPECT_EQ(m_RTData.GetAmount(), 248901949);
      EXPECT_EQ(m_RTData.GetVBuy(0), 19900);
      EXPECT_EQ(m_RTData.GetPBuy(0), 11540);
      EXPECT_EQ(m_RTData.GetVBuy(1), 54700);
      EXPECT_EQ(m_RTData.GetPBuy(1), 11530);
      EXPECT_EQ(m_RTData.GetVBuy(2), 561500);
      EXPECT_EQ(m_RTData.GetPBuy(2), 11520);
      EXPECT_EQ(m_RTData.GetVBuy(3), 105600);
      EXPECT_EQ(m_RTData.GetPBuy(3), 11510);
      EXPECT_EQ(m_RTData.GetVBuy(4), 172400);
      EXPECT_EQ(m_RTData.GetPBuy(4), 11500);
      EXPECT_EQ(m_RTData.GetVSell(0), 259981);
      EXPECT_EQ(m_RTData.GetPSell(0), 11550);
      EXPECT_EQ(m_RTData.GetVSell(1), 206108);
      break;
    case 26:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      EXPECT_EQ(m_RTData.GetBuy(), 11540);
      EXPECT_EQ(m_RTData.GetSell(), 11550);
      EXPECT_EQ(m_RTData.GetVolume(), 21606007);
      EXPECT_EQ(m_RTData.GetAmount(), 248901949);
      EXPECT_EQ(m_RTData.GetVBuy(0), 19900);
      EXPECT_EQ(m_RTData.GetPBuy(0), 11540);
      EXPECT_EQ(m_RTData.GetVBuy(1), 54700);
      EXPECT_EQ(m_RTData.GetPBuy(1), 11530);
      EXPECT_EQ(m_RTData.GetVBuy(2), 561500);
      EXPECT_EQ(m_RTData.GetPBuy(2), 11520);
      EXPECT_EQ(m_RTData.GetVBuy(3), 105600);
      EXPECT_EQ(m_RTData.GetPBuy(3), 11510);
      EXPECT_EQ(m_RTData.GetVBuy(4), 172400);
      EXPECT_EQ(m_RTData.GetPBuy(4), 11500);
      EXPECT_EQ(m_RTData.GetVSell(0), 259981);
      EXPECT_EQ(m_RTData.GetPSell(0), 11550);
      EXPECT_EQ(m_RTData.GetVSell(1), 206108);
      EXPECT_EQ(m_RTData.GetPSell(1), 11560);
      break;
    case 27:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      EXPECT_EQ(m_RTData.GetBuy(), 11540);
      EXPECT_EQ(m_RTData.GetSell(), 11550);
      EXPECT_EQ(m_RTData.GetVolume(), 21606007);
      EXPECT_EQ(m_RTData.GetAmount(), 248901949);
      EXPECT_EQ(m_RTData.GetVBuy(0), 19900);
      EXPECT_EQ(m_RTData.GetPBuy(0), 11540);
      EXPECT_EQ(m_RTData.GetVBuy(1), 54700);
      EXPECT_EQ(m_RTData.GetPBuy(1), 11530);
      EXPECT_EQ(m_RTData.GetVBuy(2), 561500);
      EXPECT_EQ(m_RTData.GetPBuy(2), 11520);
      EXPECT_EQ(m_RTData.GetVBuy(3), 105600);
      EXPECT_EQ(m_RTData.GetPBuy(3), 11510);
      EXPECT_EQ(m_RTData.GetVBuy(4), 172400);
      EXPECT_EQ(m_RTData.GetPBuy(4), 11500);
      EXPECT_EQ(m_RTData.GetVSell(0), 259981);
      EXPECT_EQ(m_RTData.GetPSell(0), 11550);
      EXPECT_EQ(m_RTData.GetVSell(1), 206108);
      EXPECT_EQ(m_RTData.GetPSell(1), 11560);
      EXPECT_EQ(m_RTData.GetVSell(2), 325641);
      EXPECT_EQ(m_RTData.GetVSell(2), 325641);
      break;
    case 28:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      EXPECT_EQ(m_RTData.GetBuy(), 11540);
      EXPECT_EQ(m_RTData.GetSell(), 11550);
      EXPECT_EQ(m_RTData.GetVolume(), 21606007);
      EXPECT_EQ(m_RTData.GetAmount(), 248901949);
      EXPECT_EQ(m_RTData.GetVBuy(0), 19900);
      EXPECT_EQ(m_RTData.GetPBuy(0), 11540);
      EXPECT_EQ(m_RTData.GetVBuy(1), 54700);
      EXPECT_EQ(m_RTData.GetPBuy(1), 11530);
      EXPECT_EQ(m_RTData.GetVBuy(2), 561500);
      EXPECT_EQ(m_RTData.GetPBuy(2), 11520);
      EXPECT_EQ(m_RTData.GetVBuy(3), 105600);
      EXPECT_EQ(m_RTData.GetPBuy(3), 11510);
      EXPECT_EQ(m_RTData.GetVBuy(4), 172400);
      EXPECT_EQ(m_RTData.GetPBuy(4), 11500);
      EXPECT_EQ(m_RTData.GetVSell(0), 259981);
      EXPECT_EQ(m_RTData.GetPSell(0), 11550);
      EXPECT_EQ(m_RTData.GetVSell(1), 206108);
      EXPECT_EQ(m_RTData.GetPSell(1), 11560);
      EXPECT_EQ(m_RTData.GetVSell(2), 325641);
      EXPECT_EQ(m_RTData.GetPSell(2), 11570);
      EXPECT_EQ(m_RTData.GetVSell(2), 325641);
      EXPECT_EQ(m_RTData.GetPSell(2), 11570);
      break;
    case 29:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      EXPECT_EQ(m_RTData.GetBuy(), 11540);
      EXPECT_EQ(m_RTData.GetSell(), 11550);
      EXPECT_EQ(m_RTData.GetVolume(), 21606007);
      EXPECT_EQ(m_RTData.GetAmount(), 248901949);
      EXPECT_EQ(m_RTData.GetVBuy(0), 19900);
      EXPECT_EQ(m_RTData.GetPBuy(0), 11540);
      EXPECT_EQ(m_RTData.GetVBuy(1), 54700);
      EXPECT_EQ(m_RTData.GetPBuy(1), 11530);
      EXPECT_EQ(m_RTData.GetVBuy(2), 561500);
      EXPECT_EQ(m_RTData.GetPBuy(2), 11520);
      EXPECT_EQ(m_RTData.GetVBuy(3), 105600);
      EXPECT_EQ(m_RTData.GetPBuy(3), 11510);
      EXPECT_EQ(m_RTData.GetVBuy(4), 172400);
      EXPECT_EQ(m_RTData.GetPBuy(4), 11500);
      EXPECT_EQ(m_RTData.GetVSell(0), 259981);
      EXPECT_EQ(m_RTData.GetPSell(0), 11550);
      EXPECT_EQ(m_RTData.GetVSell(1), 206108);
      EXPECT_EQ(m_RTData.GetPSell(1), 11560);
      EXPECT_EQ(m_RTData.GetVSell(2), 325641);
      EXPECT_EQ(m_RTData.GetPSell(2), 11570);
      EXPECT_EQ(m_RTData.GetVSell(3), 215109);
      break;
    case 30:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      EXPECT_EQ(m_RTData.GetBuy(), 11540);
      EXPECT_EQ(m_RTData.GetSell(), 11550);
      EXPECT_EQ(m_RTData.GetVolume(), 21606007);
      EXPECT_EQ(m_RTData.GetAmount(), 248901949);
      EXPECT_EQ(m_RTData.GetVBuy(0), 19900);
      EXPECT_EQ(m_RTData.GetPBuy(0), 11540);
      EXPECT_EQ(m_RTData.GetVBuy(1), 54700);
      EXPECT_EQ(m_RTData.GetPBuy(1), 11530);
      EXPECT_EQ(m_RTData.GetVBuy(2), 561500);
      EXPECT_EQ(m_RTData.GetPBuy(2), 11520);
      EXPECT_EQ(m_RTData.GetVBuy(3), 105600);
      EXPECT_EQ(m_RTData.GetPBuy(3), 11510);
      EXPECT_EQ(m_RTData.GetVBuy(4), 172400);
      EXPECT_EQ(m_RTData.GetPBuy(4), 11500);
      EXPECT_EQ(m_RTData.GetVSell(0), 259981);
      EXPECT_EQ(m_RTData.GetPSell(0), 11550);
      EXPECT_EQ(m_RTData.GetVSell(1), 206108);
      EXPECT_EQ(m_RTData.GetPSell(1), 11560);
      EXPECT_EQ(m_RTData.GetVSell(2), 325641);
      EXPECT_EQ(m_RTData.GetPSell(2), 11570);
      EXPECT_EQ(m_RTData.GetVSell(3), 215109);
      EXPECT_EQ(m_RTData.GetPSell(3), 11580);
      break;
    case 31:
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sh600000"));
      EXPECT_STREQ(m_RTData.GetStockName(), _T("浦发银行"));
      EXPECT_EQ(m_RTData.GetOpen(), 11510);
      EXPECT_EQ(m_RTData.GetLastClose(), 11490);
      EXPECT_EQ(m_RTData.GetNew(), 11560);
      EXPECT_EQ(m_RTData.GetHigh(), 11570);
      EXPECT_EQ(m_RTData.GetLow(), 11440);
      EXPECT_EQ(m_RTData.GetBuy(), 11540);
      EXPECT_EQ(m_RTData.GetSell(), 11550);
      EXPECT_EQ(m_RTData.GetVolume(), 21606007);
      EXPECT_EQ(m_RTData.GetAmount(), 248901949);
      EXPECT_EQ(m_RTData.GetVBuy(0), 19900);
      EXPECT_EQ(m_RTData.GetPBuy(0), 11540);
      EXPECT_EQ(m_RTData.GetVBuy(1), 54700);
      EXPECT_EQ(m_RTData.GetPBuy(1), 11530);
      EXPECT_EQ(m_RTData.GetVBuy(2), 561500);
      EXPECT_EQ(m_RTData.GetPBuy(2), 11520);
      EXPECT_EQ(m_RTData.GetVBuy(3), 105600);
      EXPECT_EQ(m_RTData.GetPBuy(3), 11510);
      EXPECT_EQ(m_RTData.GetVBuy(4), 172400);
      EXPECT_EQ(m_RTData.GetPBuy(4), 11500);
      EXPECT_EQ(m_RTData.GetVSell(0), 259981);
      EXPECT_EQ(m_RTData.GetPSell(0), 11550);
      EXPECT_EQ(m_RTData.GetVSell(1), 206108);
      EXPECT_EQ(m_RTData.GetPSell(1), 11560);
      EXPECT_EQ(m_RTData.GetVSell(2), 325641);
      EXPECT_EQ(m_RTData.GetPSell(2), 11570);
      EXPECT_EQ(m_RTData.GetVSell(3), 215109);
      EXPECT_EQ(m_RTData.GetPSell(3), 11580);
      EXPECT_EQ(m_RTData.GetVSell(4), 262900);
      break;
    case 32: // 没有实时数据
      EXPECT_TRUE(fSucceed); // 读取正确
      EXPECT_EQ(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_EQ(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_STREQ(m_RTData.GetStockCode(), _T("sz000001"));
      EXPECT_FALSE(m_RTData.IsActive()); // 此股票不是活跃股票
      break;
    case 33: // 有错误，前缀出错
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_FALSE(m_RTData.IsActive()); // 此股票是活跃股票
      break;
    case 34: // 有错误，前缀出错
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_FALSE(m_RTData.IsActive()); // 此股票是活跃股票
      break;
    case 35: // 有错误，前缀出错
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_FALSE(m_RTData.IsActive()); // 此股票是活跃股票
      break;
    case 36: // 有错误，前缀出错
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_FALSE(m_RTData.IsActive()); // 此股票是活跃股票
      break;
    case 37: // 有错误，前缀出错
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_FALSE(m_RTData.IsActive()); // 此股票是活跃股票
      break;
    case 38: // 有错误，前缀出错
      EXPECT_FALSE(fSucceed); // 有错误
      EXPECT_GT(m_lStringLength, m_SinaRTWebData.GetCurrentPos());
      EXPECT_FALSE(m_RTData.IsActive()); // 此股票是活跃股票
      break;
    default:
      break;
    }
  }

  struct ReadSinaOneValueExceptPeriodData {
    ReadSinaOneValueExceptPeriodData(int count, CString Data) {
      m_iCount = count;
      m_strData = Data;
    }
  public:
    int m_iCount;
    CString m_strData;
  };

  // 成功
  ReadSinaOneValueExceptPeriodData data1(1, _T("11.050,"));
  // 小数点后两位
  ReadSinaOneValueExceptPeriodData data2(2, _T("11.05,"));
  // 小数点后一位
  ReadSinaOneValueExceptPeriodData data3(3, _T("11.0,"));
  // 小数点前出现0x00a
  ReadSinaOneValueExceptPeriodData data4(4, _T("1\n1.050,"));
  // 小数点后出现0x00a
  ReadSinaOneValueExceptPeriodData data5(5, _T("11.0\n50,"));
  // 缺少','
  ReadSinaOneValueExceptPeriodData data6(6, _T("11.050"));
  // 读取小数点后三位后，放弃气候多余的数值
  ReadSinaOneValueExceptPeriodData data7(7, _T("11.050000,"));
  // 0x00a出现于‘，’前。
  ReadSinaOneValueExceptPeriodData data8(8, _T("11.05000\n,"));

  class ReadOneValueExceptPeriodTest : public::testing::TestWithParam<ReadSinaOneValueExceptPeriodData*> {
  protected:
    void SetUp(void) override {
      ReadSinaOneValueExceptPeriodData* pData = GetParam();
      m_iCount = pData->m_iCount;
      long lLength = pData->m_strData.GetLength();
      m_pData = m_SinaRTWebData.GetBufferAddr();
      for (int i = 0; i < lLength; i++) {
        m_pData[i] = pData->m_strData[i];
      }
      m_pData[lLength] = 0x000;
      m_SinaRTWebData.ResetCurrentPos();
    }

    void TearDown(void) override {
      // clearup
    }

  public:
    int m_iCount;
    char* m_pData;
    CSinaRTWebData m_SinaRTWebData;
    CRTData m_RTData;
  };

  INSTANTIATE_TEST_CASE_P(TestReadOneValueExceptPeriod, ReadOneValueExceptPeriodTest,
    testing::Values(&data1, &data2, &data3, &data4, &data5, &data6, &data7, &data8
    ));

  TEST_P(ReadOneValueExceptPeriodTest, TestReadOneValue) {
    char buffer[30];
    bool fSucceed = m_RTData.ReadSinaOneValueExceptPeriod(&m_SinaRTWebData, buffer);
    CString str;
    str = buffer;
    switch (m_iCount) {
    case 1:
      EXPECT_TRUE(fSucceed);
      EXPECT_EQ(m_SinaRTWebData.GetCurrentPos(), 7);
      EXPECT_STREQ(str, _T("11050"));
      break;
    case 2:
      EXPECT_TRUE(fSucceed);
      EXPECT_EQ(m_SinaRTWebData.GetCurrentPos(), 6);
      EXPECT_STREQ(str, _T("11050"));
      break;
    case 3:
      EXPECT_TRUE(fSucceed);
      EXPECT_EQ(m_SinaRTWebData.GetCurrentPos(), 5);
      EXPECT_STREQ(str, _T("11000"));
      break;
    case 4:
      EXPECT_FALSE(fSucceed);
      break;
    case 5:
      EXPECT_FALSE(fSucceed);
      break;
    case 6:
      EXPECT_FALSE(fSucceed);
      EXPECT_STREQ(str, _T("11050"));
      break;
    case 7:
      EXPECT_TRUE(fSucceed);
      EXPECT_EQ(m_SinaRTWebData.GetCurrentPos(), 10);
      EXPECT_STREQ(str, _T("11050"));
      break;
    case 8:
      EXPECT_FALSE(fSucceed);
      EXPECT_STREQ(str, _T("11050"));
      break;
    default:
      break;
    }
  }

  struct ReadSinaOneCalueData {
    ReadSinaOneCalueData(int count, CString Data) {
      m_iCount = count;
      m_strData = Data;
    }
  public:
    int m_iCount;
    CString m_strData;
  };

  // 成功
  ReadSinaOneCalueData rdata1(1, _T("11.050,"));
  // 小数点后两位
  ReadSinaOneCalueData rdata2(2, _T("11.05,"));
  // 小数点后一位
  ReadSinaOneCalueData rdata3(3, _T("11.0,"));
  // 小数点前出现0x00a
  ReadSinaOneCalueData rdata4(4, _T("1\n1.050,"));
  // 小数点后出现0x00a
  ReadSinaOneCalueData rdata5(5, _T("11.0\n50,"));
  // 缺少','
  ReadSinaOneCalueData rdata6(6, _T("11.050"));
  // 读取小数点后三位后，放弃气候多余的数值
  ReadSinaOneCalueData rdata7(7, _T("11.050000,"));
  // 0x00a出现于‘，’前。
  ReadSinaOneCalueData rdata8(8, _T("11.05000\n,"));
  // 只有','
  ReadSinaOneCalueData rdata9(9, _T(","));

  class ReadOneValueTest : public::testing::TestWithParam<ReadSinaOneCalueData*> {
  protected:
    void SetUp(void) override {
      ReadSinaOneCalueData* pData = GetParam();
      m_iCount = pData->m_iCount;
      long lLength = pData->m_strData.GetLength();
      m_pData = m_SinaRTWebData.GetBufferAddr();
      for (int i = 0; i < lLength; i++) {
        m_pData[i] = pData->m_strData[i];
      }
      m_pData[lLength] = 0x000;
      m_SinaRTWebData.ResetCurrentPos();
    }

    void TearDown(void) override {
      // clearup
    }

  public:
    int m_iCount;
    char* m_pData;
    CSinaRTWebData m_SinaRTWebData;
    CRTData m_RTData;
  };

  INSTANTIATE_TEST_CASE_P(TestReadOneValue, ReadOneValueTest,
    testing::Values(&rdata1, &rdata2, &rdata3, &rdata4, &rdata5, &rdata6, &rdata7, &rdata8, &rdata9
    ));

  TEST_P(ReadOneValueTest, TestReadSinaOneValue1) {
    INT64 llTemp = 0;
    bool fSucceed = m_RTData.ReadSinaOneValue(&m_SinaRTWebData, llTemp);
    switch (m_iCount) {
    case 1:
      EXPECT_TRUE(fSucceed);
      EXPECT_EQ(m_SinaRTWebData.GetCurrentPos(), 7);
      EXPECT_EQ(llTemp, 11);
      break;
    case 2:
      EXPECT_TRUE(fSucceed);
      EXPECT_EQ(m_SinaRTWebData.GetCurrentPos(), 6);
      EXPECT_EQ(llTemp, 11);
      break;
    case 3:
      EXPECT_TRUE(fSucceed);
      EXPECT_EQ(m_SinaRTWebData.GetCurrentPos(), 5);
      EXPECT_EQ(llTemp, 11);
      break;
    case 4:
      EXPECT_FALSE(fSucceed);
      break;
    case 5:
      EXPECT_FALSE(fSucceed);
      break;
    case 6:
      EXPECT_FALSE(fSucceed);
      break;
    case 7:
      EXPECT_TRUE(fSucceed);
      EXPECT_EQ(m_SinaRTWebData.GetCurrentPos(), 10);
      EXPECT_EQ(llTemp, 11);
      break;
    case 8:
      EXPECT_FALSE(fSucceed);
      break;
    case 9:
      EXPECT_TRUE(fSucceed);
      EXPECT_EQ(m_SinaRTWebData.GetCurrentPos(), 1);
      EXPECT_EQ(llTemp, 0);
      break;
    default:
      break;
    }
  }

  TEST_P(ReadOneValueTest, TestReadSinaOneValue2) {
    long lTemp = 0;
    bool fSucceed = m_RTData.ReadSinaOneValue(&m_SinaRTWebData, lTemp);
    switch (m_iCount) {
    case 1:
      EXPECT_TRUE(fSucceed);
      EXPECT_EQ(m_SinaRTWebData.GetCurrentPos(), 7);
      EXPECT_EQ(lTemp, 11);
      break;
    case 2:
      EXPECT_TRUE(fSucceed);
      EXPECT_EQ(m_SinaRTWebData.GetCurrentPos(), 6);
      EXPECT_EQ(lTemp, 11);
      break;
    case 3:
      EXPECT_TRUE(fSucceed);
      EXPECT_EQ(m_SinaRTWebData.GetCurrentPos(), 5);
      EXPECT_EQ(lTemp, 11);
      break;
    case 4:
      EXPECT_FALSE(fSucceed);
      break;
    case 5:
      EXPECT_FALSE(fSucceed);
      break;
    case 6:
      EXPECT_FALSE(fSucceed);
      break;
    case 7:
      EXPECT_TRUE(fSucceed);
      EXPECT_EQ(m_SinaRTWebData.GetCurrentPos(), 10);
      EXPECT_EQ(lTemp, 11);
      break;
    case 8:
      EXPECT_FALSE(fSucceed);
      break;
    case 9:
      EXPECT_TRUE(fSucceed);
      EXPECT_EQ(m_SinaRTWebData.GetCurrentPos(), 1);
      EXPECT_EQ(lTemp, 0);
      break;
    default:
      break;
    }
  }

  TEST_P(ReadOneValueTest, TestReadSinaOneValue3) {
    char buffer[30];
    bool fSucceed = m_RTData.ReadSinaOneValue(&m_SinaRTWebData, buffer);
    CString str;
    str = buffer;
    switch (m_iCount) {
    case 1:
      EXPECT_TRUE(fSucceed);
      EXPECT_EQ(m_SinaRTWebData.GetCurrentPos(), 7);
      EXPECT_STREQ(str, _T("11.050"));
      break;
    case 2:
      EXPECT_TRUE(fSucceed);
      EXPECT_EQ(m_SinaRTWebData.GetCurrentPos(), 6);
      EXPECT_STREQ(str, _T("11.05"));
      break;
    case 3:
      EXPECT_TRUE(fSucceed);
      EXPECT_EQ(m_SinaRTWebData.GetCurrentPos(), 5);
      EXPECT_STREQ(str, _T("11.0"));
      break;
    case 4:
      EXPECT_FALSE(fSucceed);
      break;
    case 5:
      EXPECT_FALSE(fSucceed);
      break;
    case 6:
      EXPECT_FALSE(fSucceed);
      break;
    case 7:
      EXPECT_TRUE(fSucceed);
      EXPECT_EQ(m_SinaRTWebData.GetCurrentPos(), 10);
      EXPECT_STREQ(str, _T("11.050000"));
      break;
    case 8:
      EXPECT_FALSE(fSucceed);
      break;
    case 9:
      EXPECT_TRUE(fSucceed);
      EXPECT_EQ(m_SinaRTWebData.GetCurrentPos(), 1);
      EXPECT_STREQ(str, _T(""));
      break;
    default:
      break;
    }
  }
}