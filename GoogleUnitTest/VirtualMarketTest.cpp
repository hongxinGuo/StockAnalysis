#include"pch.h"

#include"globedef.h"
#include"VirtualMarket.h"

namespace StockAnalysisTest {
  CVirtualMarket s_VirtualMarket;

  class CVirtualMarketTest : public ::testing::Test
  {
  protected:
    static void SetUpTestSuite(void) {
    }

    static void TearDownTestSuite(void) {
      while (gl_ThreadStatus.IsSavingThreadRunning()) Sleep(1);
    }

    virtual void SetUp(void) override {
      ASSERT_FALSE(gl_fNormalMode);
      ASSERT_TRUE(gl_fTestMode);
      s_VirtualMarket.SetPermitResetMarket(true);
      s_VirtualMarket.SetReadyToRun(true);
      s_VirtualMarket.SetResetMarket(true);
    }

    virtual void TearDown(void) override {
      // clearup
      while (gl_systemMessage.GetInformationDequeSize() > 0) gl_systemMessage.PopInformationMessage();
    }
  };

  TEST_F(CVirtualMarketTest, TestIsReadyToRun) {
    EXPECT_TRUE(s_VirtualMarket.IsReadyToRun());
    s_VirtualMarket.SetReadyToRun(false);
    EXPECT_FALSE(s_VirtualMarket.IsReadyToRun());
    s_VirtualMarket.SetReadyToRun(true);
    EXPECT_TRUE(s_VirtualMarket.IsReadyToRun());
  }

  TEST_F(CVirtualMarketTest, TestIsPermitResetMarket) {
    EXPECT_TRUE(s_VirtualMarket.IsPermitResetMarket()) << "PermitResetMarket should be true\n";
    s_VirtualMarket.SetPermitResetMarket(false);
    EXPECT_FALSE(s_VirtualMarket.IsPermitResetMarket());
    s_VirtualMarket.SetPermitResetMarket(true);
    EXPECT_TRUE(s_VirtualMarket.IsPermitResetMarket());
  }

  TEST_F(CVirtualMarketTest, TestMarketReady) {
    EXPECT_TRUE(s_VirtualMarket.IsSystemReady()) << "市场基类默认为真";
    s_VirtualMarket.SetSystemReady(true);
    EXPECT_TRUE(s_VirtualMarket.IsSystemReady());
    s_VirtualMarket.SetSystemReady(false);
    EXPECT_FALSE(s_VirtualMarket.IsSystemReady());

    s_VirtualMarket.SetSystemReady(true); // 恢复原态
  }

  TEST_F(CVirtualMarketTest, TestCalculateMarketTime) {
    ASSERT_FALSE(gl_fNormalMode);
    time_t ttime;
    tm tm_, tmLocal;
    time(&ttime);
    s_VirtualMarket.CalculateTime();
    EXPECT_EQ(ttime, s_VirtualMarket.GetLocalTime());
    EXPECT_EQ(ttime - s_VirtualMarket.GetMarketTimeZone(), s_VirtualMarket.GetMarketTime());
    localtime_s(&tmLocal, &ttime);
    ttime -= s_VirtualMarket.GetMarketTimeZone();
    gmtime_s(&tm_, &ttime);
    long lTimeZone;
    _get_timezone(&lTimeZone);
    s_VirtualMarket.CalculateLastTradeDate();
    long lTime = s_VirtualMarket.GetFormatedMarketTime();
    EXPECT_EQ(s_VirtualMarket.GetDayOfWeek(), tm_.tm_wday);

    long day = (tm_.tm_year + 1900) * 10000 + (tm_.tm_mon + 1) * 100 + tm_.tm_mday;
    EXPECT_EQ(s_VirtualMarket.GetFormatedMarketDate(), day);
    EXPECT_EQ(s_VirtualMarket.GetMonthOfYear(), tm_.tm_mon + 1);
    EXPECT_EQ(s_VirtualMarket.GetDateOfMonth(), tm_.tm_mday);
    EXPECT_EQ(s_VirtualMarket.GetYear(), tm_.tm_year + 1900);

    long time = tm_.tm_hour * 10000 + tm_.tm_min * 100 + tm_.tm_sec;
    EXPECT_EQ(s_VirtualMarket.GetFormatedMarketTime(), time);
    char buffer[30];
    sprintf_s(buffer, _T("%02d:%02d:%02d "), tmLocal.tm_hour, tmLocal.tm_min, tmLocal.tm_sec);
    CString str;
    str = buffer;

    EXPECT_EQ(str.Compare(s_VirtualMarket.GetStringOfLocalTime()), 0);

    sprintf_s(buffer, _T("%02d:%02d:%02d "), tm_.tm_hour, tm_.tm_min, tm_.tm_sec);
    str = buffer;
    EXPECT_EQ(str.Compare(s_VirtualMarket.GetStringOfMarketTime()), 0);

    switch (tm_.tm_wday) {
    case 1: // 星期一
    ttime -= 3 * 24 * 3600; //
    break;
    case 0: //星期日
    ttime -= 3 * 24 * 3600; //
    break;
    case 6: // 星期六
    ttime -= 2 * 24 * 3600; //
    break;
    default: // 其他
    ttime -= 24 * 3600; //
    }
    gmtime_s(&tm_, &ttime);
    long LastTradeDate = (tm_.tm_year + 1900) * 10000 + (tm_.tm_mon + 1) * 100 + tm_.tm_mday;
    EXPECT_EQ(s_VirtualMarket.GetLastTradeDate(), LastTradeDate);
  }

  TEST_F(CVirtualMarketTest, TestGetLastTradeDate) {
    time_t ttime;
    tm tm_, tm2;

    for (int i = 0; i < 7; i++) {
      time(&ttime);
      ttime += i * 60 * 60 * 24;
      gmtime_s(&tm2, &ttime);
      tm_ = tm2;
      s_VirtualMarket.__TEST_SetMarketTime(ttime);
      s_VirtualMarket.__TEST_SetMarketTM(tm2);

      switch (tm_.tm_wday) {
      case 1: // 星期一
      ttime -= 3 * 24 * 3600; //
      break;
      case 0: //星期日
      ttime -= 3 * 24 * 3600; //
      break;
      case 6: // 星期六
      ttime -= 2 * 24 * 3600; //
      break;
      default: // 其他
      ttime -= 24 * 3600; //
      }
      gmtime_s(&tm_, &ttime);
      long LastTradeDate = (tm_.tm_year + 1900) * 10000 + (tm_.tm_mon + 1) * 100 + tm_.tm_mday;
      EXPECT_EQ(s_VirtualMarket.GetLastTradeDate(), LastTradeDate);
    }
  }

  TEST_F(CVirtualMarketTest, TestIsWorkingDay) {
    CTime time1(2019, 11, 25, 0, 0, 0); // 此日为星期一
    EXPECT_TRUE(s_VirtualMarket.IsWorkingDay(time1));
    EXPECT_TRUE(s_VirtualMarket.IsWorkingDay(20191125));
    CTime time2(2019, 11, 26, 0, 0, 0); // 此日为星期二
    EXPECT_TRUE(s_VirtualMarket.IsWorkingDay(time2));
    EXPECT_TRUE(s_VirtualMarket.IsWorkingDay(20191126));
    CTime time3(2019, 11, 27, 0, 0, 0); // 此日为星期三
    EXPECT_TRUE(s_VirtualMarket.IsWorkingDay(time3));
    EXPECT_TRUE(s_VirtualMarket.IsWorkingDay(20191127));
    CTime time4(2019, 11, 28, 0, 0, 0); // 此日为星期四
    EXPECT_TRUE(s_VirtualMarket.IsWorkingDay(time4));
    EXPECT_TRUE(s_VirtualMarket.IsWorkingDay(20191128));
    CTime time5(2019, 11, 29, 0, 0, 0); // 此日为星期五
    EXPECT_TRUE(s_VirtualMarket.IsWorkingDay(time5));
    EXPECT_TRUE(s_VirtualMarket.IsWorkingDay(20191129));
    CTime time6(2019, 11, 30, 0, 0, 0); // 此日为星期六
    EXPECT_FALSE(s_VirtualMarket.IsWorkingDay(time6));
    EXPECT_FALSE(s_VirtualMarket.IsWorkingDay(20191130));
    CTime time7(2019, 12, 1, 0, 0, 0); // 此日为星期日
    EXPECT_FALSE(s_VirtualMarket.IsWorkingDay(time7));
    EXPECT_FALSE(s_VirtualMarket.IsWorkingDay(20191201));
  }

  TEST_F(CVirtualMarketTest, TestIsEarlyThen) {
    EXPECT_TRUE(s_VirtualMarket.IsEarlyThen(20200101, 20200115, 13));
    EXPECT_FALSE(s_VirtualMarket.IsEarlyThen(20200101, 20200115, 14));
    EXPECT_TRUE(s_VirtualMarket.IsEarlyThen(20200115, 20200201, 16));
    EXPECT_FALSE(s_VirtualMarket.IsEarlyThen(20200115, 20200201, 17));
    EXPECT_TRUE(s_VirtualMarket.IsEarlyThen(20191101, 20200115, 74));
    EXPECT_FALSE(s_VirtualMarket.IsEarlyThen(20191101, 20200115, 75));
  }

  TEST_F(CVirtualMarketTest, TestGetNextDate) {
    EXPECT_EQ(s_VirtualMarket.GetNextDay(20200101, 1), 20200102);
    EXPECT_EQ(s_VirtualMarket.GetNextDay(20200101, 11), 20200112);
    EXPECT_EQ(s_VirtualMarket.GetNextDay(20201231, 1), 20210101);
    EXPECT_EQ(s_VirtualMarket.GetNextDay(20201221, 14), 20210104);
  }

  TEST_F(CVirtualMarketTest, TestGetPrevDay) {
    EXPECT_EQ(s_VirtualMarket.GetPrevDay(20200102), 20200101);
    EXPECT_EQ(s_VirtualMarket.GetPrevDay(20200112, 10), 20200102);
    EXPECT_EQ(s_VirtualMarket.GetPrevDay(20200102, 11), 20191222);
    EXPECT_EQ(s_VirtualMarket.GetPrevDay(20200202, 11), 20200122);
  }

  TEST_F(CVirtualMarketTest, TestGetStringOfLocalTime) {
    s_VirtualMarket.CalculateTime();

    time_t tLocal;
    time(&tLocal);
    tm tmLocal;
    char buffer[30];
    CString str;

    localtime_s(&tmLocal, &tLocal);
    sprintf_s(buffer, _T("%02d:%02d:%02d "), tmLocal.tm_hour, tmLocal.tm_min, tmLocal.tm_sec);
    str = buffer;
    EXPECT_STREQ(s_VirtualMarket.GetStringOfLocalTime(), str);
  }

  TEST_F(CVirtualMarketTest, TestGetStringOfLocalDateTime) {
    s_VirtualMarket.CalculateTime();

    time_t tLocal = s_VirtualMarket.GetLocalTime();
    tm tmLocal;
    char buffer[100];
    CString str;

    localtime_s(&tmLocal, &tLocal);
    sprintf_s(buffer, _T("%04d年%02d月%02d日 %02d:%02d:%02d "), tmLocal.tm_year + 1900, tmLocal.tm_mon + 1, tmLocal.tm_mday, tmLocal.tm_hour, tmLocal.tm_min, tmLocal.tm_sec);
    str = buffer;
    EXPECT_STREQ(s_VirtualMarket.GetStringOfLocalDateTime(), str);
  }

  TEST_F(CVirtualMarketTest, TestGetStringOfMarketTime) {
    s_VirtualMarket.CalculateTime();

    tm tmMarket;
    time_t tMarket = s_VirtualMarket.GetMarketTime();
    gmtime_s(&tmMarket, &tMarket);
    char buffer[30];
    CString str;

    sprintf_s(buffer, _T("%02d:%02d:%02d "), tmMarket.tm_hour, tmMarket.tm_min, tmMarket.tm_sec);
    str = buffer;
    EXPECT_STREQ(s_VirtualMarket.GetStringOfMarketTime(), str);
  }

  TEST_F(CVirtualMarketTest, TestGetStringOfMarketDateTime) {
    s_VirtualMarket.CalculateTime();

    time_t tMarket = s_VirtualMarket.GetMarketTime();
    tm tmMarket;
    char buffer[100];
    CString str;

    gmtime_s(&tmMarket, &tMarket);
    sprintf_s(buffer, _T("%04d年%02d月%02d日 %02d:%02d:%02d "), tmMarket.tm_year + 1900, tmMarket.tm_mon + 1, tmMarket.tm_mday, tmMarket.tm_hour, tmMarket.tm_min, tmMarket.tm_sec);
    str = buffer;
    EXPECT_STREQ(s_VirtualMarket.GetStringOfMarketDateTime(), str);
  }

  TEST_F(CVirtualMarketTest, TestGetStringOfDate) {
    char buffer[30];
    sprintf_s(buffer, _T("%4d年%2d月%2d日"), 2020, 02, 02);
    CString str;
    str = buffer;
    EXPECT_STREQ(s_VirtualMarket.GetStringOfDate(20200202), str);

    long lDate = s_VirtualMarket.GetFormatedMarketDate();
    long year = lDate / 10000;
    long month = lDate / 100 - year * 100;
    long day = lDate - year * 10000 - month * 100;
    sprintf_s(buffer, _T("%4d年%2d月%2d日"), year, month, day);
    str = buffer;
    EXPECT_STREQ(s_VirtualMarket.GetStringOfMarketDate(), str);
  }

  TEST_F(CVirtualMarketTest, TestGetDateOfWeek) {
    ASSERT_FALSE(gl_fNormalMode);
    time_t ttime;
    tm tm_;
    time(&ttime);
    ttime -= s_VirtualMarket.GetMarketTimeZone();
    gmtime_s(&tm_, &ttime);

    s_VirtualMarket.CalculateTime();
    EXPECT_EQ(s_VirtualMarket.GetDayOfWeek(), tm_.tm_wday);
  }

  TEST_F(CVirtualMarketTest, TestTaskResetMarketFlagAtMidnight) { // 这个其实是测试的CVirtualMarket类中的函数。
    EXPECT_TRUE(s_VirtualMarket.IsPermitResetMarket());
    s_VirtualMarket.SetPermitResetMarket(false);
    s_VirtualMarket.TaskResetMarketFlagAtMidnight(0);
    EXPECT_TRUE(s_VirtualMarket.IsPermitResetMarket());
    s_VirtualMarket.SetPermitResetMarket(false);
    s_VirtualMarket.TaskResetMarketFlagAtMidnight(1501);
    EXPECT_FALSE(s_VirtualMarket.IsPermitResetMarket());
    s_VirtualMarket.TaskResetMarketFlagAtMidnight(1500);
    EXPECT_TRUE(s_VirtualMarket.IsPermitResetMarket());
  }

  TEST_F(CVirtualMarketTest, TestSchedulingTask) {
    EXPECT_TRUE(s_VirtualMarket.SchedulingTask()) << "第一次调用时，内部时间初值为0，故而返回真";
    EXPECT_FALSE(s_VirtualMarket.SchedulingTask()) << "第二次调用时，内部时间已经设置为当前时间了，间隔不超过一秒，故而返回假";
  }

  TEST_F(CVirtualMarketTest, TestSchedulingTaskPerSecond) {
    EXPECT_TRUE(s_VirtualMarket.SchedulingTaskPerSecond(90000));
  }

  TEST_F(CVirtualMarketTest, TestSchedulingTaskPerMinute) {
    EXPECT_TRUE(s_VirtualMarket.SchedulingTaskPerMinute(60, 10000));
    EXPECT_FALSE(s_VirtualMarket.SchedulingTaskPerMinute(59, 12000));
    EXPECT_TRUE(s_VirtualMarket.SchedulingTaskPerMinute(1, 12010));
  }
}