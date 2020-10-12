#include"globedef.h"
#include "Accessory.h"

time_t ConvertBufferToTime(CString strFormat, const char* buffer) {
  time_t tt{ 0 };
  tm tm_;
  int year, month, day, hour, minute, second;

  sscanf_s(buffer, strFormat.GetBuffer(), &year, &month, &day, &hour, &minute, &second);
  tm_.tm_year = year - 1900;
  tm_.tm_mon = month - 1;
  tm_.tm_mday = day;
  tm_.tm_hour = hour;
  tm_.tm_min = minute;
  tm_.tm_sec = second;
  tm_.tm_isdst = 0;
  tt = mktime(&tm_);

  return tt;
}

time_t ConvertStringToTime(CString strFormat, CString strTime) {
  time_t tt{ 0 };
  tm tm_;
  int year, month, day, hour, minute, second;

  sscanf_s(strTime.GetBuffer(), strFormat.GetBuffer(), &year, &month, &day, &hour, &minute, &second);
  tm_.tm_year = year - 1900;
  tm_.tm_mon = month - 1;
  tm_.tm_mday = day;
  tm_.tm_hour = hour;
  tm_.tm_min = minute;
  tm_.tm_sec = second;
  tm_.tm_isdst = 0;
  tt = mktime(&tm_);

  return tt;
}

time_t FormatToTTime(long lDate, long lTime) {
  ASSERT(lDate > 19700000);
  const long lYear = lDate / 10000;
  const long lMonth = (lDate - lYear * 10000) / 100;
  const long lD = (lDate - lYear * 10000 - lMonth * 100);
  const long lHour = lTime / 10000;
  const long lMinute = (lTime - lHour * 10000) / 100;
  const long lSecond = lTime - lHour * 10000 - lMinute * 100;
  const CTime ct(lYear, lMonth, lD, lHour, lMinute, lSecond);	// 北京时间15时即UTC7时
  return (ct.GetTime());
}

long FormatToDate(time_t const tt) noexcept {
  tm tm_;
  localtime_s(&tm_, &tt);
  return((tm_.tm_year + 1900) * 10000 + (tm_.tm_mon + 1) * 100 + tm_.tm_mday);
}

long FormatToTime(time_t const tt) noexcept {
  tm tm_;
  localtime_s(&tm_, &tt);
  return(tm_.tm_hour * 10000 + tm_.tm_min * 100 + tm_.tm_sec);
}

INT64 FormatToDateTime(time_t const tt) noexcept {
  tm tm_;
  localtime_s(&tm_, &tt);
  return((static_cast<INT64>(tm_.tm_year) + 1900) * 10000000000 + (static_cast<INT64>(tm_.tm_mon) + 1) * 100000000 + static_cast<INT64>(tm_.tm_mday) * 1000000 + tm_.tm_hour * 10000 + tm_.tm_min * 100 + tm_.tm_sec);
}

long FormatToDate(const tm* ptm) noexcept {
  return((ptm->tm_year + 1900) * 10000 + (ptm->tm_mon + 1) * 100 + ptm->tm_mday);
}

long FormatToTime(const tm* ptm) noexcept {
  return(ptm->tm_hour * 10000 + ptm->tm_min * 100 + ptm->tm_sec);
}

INT64 FormatToDateTime(const tm* ptm) noexcept {
  return((static_cast<INT64>(ptm->tm_year) + 1900) * 10000000000 + (static_cast<INT64>(ptm->tm_mon) + 1) * 100000000 + static_cast<INT64>(ptm->tm_mday) * 1000000 + ptm->tm_hour * 10000 + ptm->tm_min * 100 + ptm->tm_sec);
}

const static CTimeSpan s_1Day(1, 0, 0, 0);
const static CTimeSpan s_2Day(2, 0, 0, 0);
const static CTimeSpan s_3Day(3, 0, 0, 0);
const static CTimeSpan s_4Day(4, 0, 0, 0);
const static CTimeSpan s_5Day(5, 0, 0, 0);
const static CTimeSpan s_6Day(6, 0, 0, 0);
const static CTimeSpan s_7Day(7, 0, 0, 0);

long GetNextMonday(long lDate) {
  const long year = lDate / 10000;
  const long month = lDate / 100 - (lDate / 10000) * 100;
  const long mday = lDate - (lDate / 100) * 100;
  const CTime ctCurrent(year, month, mday, 12, 0, 0);
  CTime ctNext;

  ctNext = ctCurrent;
  switch (ctCurrent.GetDayOfWeek()) {
  case 2: // 星期一
  ctNext += s_7Day;
  break;
  case 3:
  ctNext += s_6Day;
  break;
  case 4:
  ctNext += s_5Day;
  break;
  case 5:
  ctNext += s_4Day;
  break;
  case 6:
  ctNext += s_3Day;
  break;
  case 7:
  ctNext += s_2Day;
  break;
  case 1:
  ctNext += s_1Day;
  break;
  break;
  default: // 不可能
  break;
  }
  const long lNextDay = ctNext.GetYear() * 10000 + ctNext.GetMonth() * 100 + ctNext.GetDay();

  return lNextDay;
}

long GetPrevMonday(long lDate) {
  const long year = lDate / 10000;
  const long month = lDate / 100 - (lDate / 10000) * 100;
  const long mday = lDate - (lDate / 100) * 100;
  const CTime ctCurrent(year, month, mday, 12, 0, 0);
  CTime ctNext;

  ctNext = ctCurrent;
  switch (ctCurrent.GetDayOfWeek()) {
  case 2: // 星期一
  ctNext -= s_7Day;
  break;
  case 3: // 星期二
  ctNext -= s_1Day;
  break;
  case 4: // 星期三
  ctNext -= s_2Day;
  break;
  case 5: // 星期四
  ctNext -= s_3Day;
  break;
  case 6:
  ctNext -= s_4Day;
  break;
  case 7: // 星期六
  ctNext -= s_5Day;
  break;
  case 1: // 星期日
  ctNext -= s_6Day;
  break;
  break;
  default: // 不可能
  break;
  }
  const long lPrevMonday = ctNext.GetYear() * 10000 + ctNext.GetMonth() * 100 + ctNext.GetDay();

  return lPrevMonday;
}

long GetCurrentMonday(long lDate) {
  const long year = lDate / 10000;
  const long month = lDate / 100 - (lDate / 10000) * 100;
  const long mday = lDate - (lDate / 100) * 100;
  const CTime ctCurrent(year, month, mday, 12, 0, 0);
  CTime ctNext;

  ctNext = ctCurrent;
  switch (ctCurrent.GetDayOfWeek()) {
  case 2: // 星期一
  break;
  case 3: // 星期二
  ctNext -= s_1Day;
  break;
  case 4: // 星期三
  ctNext -= s_2Day;
  break;
  case 5: // 星期四
  ctNext -= s_3Day;
  break;
  case 6:
  ctNext -= s_4Day;
  break;
  case 7: // 星期六
  ctNext -= s_5Day;
  break;
  case 1: // 星期日
  ctNext -= s_6Day;
  break;
  break;
  default: // 不可能
  break;
  }
  const long lCurrentMonday = ctNext.GetYear() * 10000 + ctNext.GetMonth() * 100 + ctNext.GetDay();

  return lCurrentMonday;
}

////////////////////////////////////////////////////////////////////////////////
//
// 所有数据集的GetDefaultConnect()函数皆调用此函数完成具体工作，以保证一致性。
//
// 测试模式时使用mysqlTest驱动，正常模式时使用mysql驱动，以保证使用不同的Schema。
//
/////////////////////////////////////////////////////////////////////////////////
CString GetDefaultSchemaConnect() {
  if (!gl_fNormalMode) {
    gl_fTestMode = true;
    return _T("DSN=mysqlTest;UID=Test;PASSWORD=test;charset=utf8mb4"); // mysqlTest操作的是TestData Schema
  }
  else {
    gl_fTestMode = false;
    return _T("DSN=mysql;UID=hxguo;PASSWORD=hxguo;charset=utf8mb4"); // mysql操作的是StockData Schema
  }
}

CString ConvertValueToString(long lValue, int iDividend) {
  char buffer[50]{ 0 };
  const double d = (static_cast<double>(lValue)) / iDividend;
  CString str;

  sprintf_s(buffer, _T("%.3f"), d);
  str = buffer;
  return str;
}

CString ConvertValueToString(int iValue, int iDividend) {
  char buffer[50]{ 0 };
  const double d = (static_cast<double>(iValue)) / iDividend;
  CString str;

  sprintf_s(buffer, _T("%.3f"), d);
  str = buffer;

  return str;
}

CString ConvertValueToString(INT64 iValue, int iDividend) {
  char buffer[50]{ 0 };
  const double d = (static_cast<double>(iValue)) / iDividend;
  CString str;

  sprintf_s(buffer, _T("%.3f"), d);
  str = buffer;
  return str;
}

CString ConvertValueToString(double dValue, int iDividend) {
  char buffer[50]{ 0 };
  const double d = dValue / iDividend;
  CString str;

  sprintf_s(buffer, _T("%.3f"), d);
  str = buffer;
  return str;
}

bool ReadOneValueOfNeteaseDayLine(char*& pCurrentPos, char* buffer, long& iReadNumber) {
  int i = 0;

  while (*pCurrentPos != ',') { // 将下一个逗号前的字符存入缓冲区. 0x2c就是逗号。
    if ((*pCurrentPos == 0x0d) || (*pCurrentPos == 0x00a) || (*pCurrentPos == 0x000) || (i > 100)) { // 遇到回车、换行或者字符串结束符,或者超过了100个字符
      return false; // 数据出错，放弃载入
    }
    buffer[i++] = *pCurrentPos++;
  }
  buffer[i] = 0x000;
  pCurrentPos++;
  i++;
  iReadNumber += i;
  return true;
}

void ZoomIn(vector<double>& vData, double dLevel, double dRate) {
  double d = 0;

  for (int i = 0; i < vData.size(); i++) {
    d = dLevel + (vData.at(i) - dLevel) * dRate;
    if (d < 0) vData.at(i) = 0;
    else if (d > 100) vData.at(i) = 100;
    else vData.at(i) = d;
  }
}