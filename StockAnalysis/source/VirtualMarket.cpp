#include"stdafx.h"

#include"globedef.h"

#include "VirtualMarket.h"

// 所有的市场使用同一个当地时间。
time_t CVirtualMarket::sm_tLocal = 0;

CVirtualMarket::CVirtualMarket(void) {
  m_fPermitResetMarket = true; // 允许系统被重置标识，唯独此标识不允许系统重置。初始时设置为真：允许重置系统。
  m_fResetMarket = true;
  m_fReadyToRun = true;

  m_lMarketLastTradeDay = 0;
  m_lMarketTime = 0;
  m_lMarketToday = 0;
  m_tMarket = 0;

  m_strMarketId = _T("Warning: CVirtualMarket Called.");
  m_lTimeZoneOffset = -8 * 3600; // 本系统默认标准时间为东八区（北京标准时间）。
}

CVirtualMarket::~CVirtualMarket(void) {
}

#ifdef _DEBUG
void CVirtualMarket::AssertValid() const {
  CObject::AssertValid();
}

void CVirtualMarket::Dump(CDumpContext& dc) const {
  CObject::Dump(dc);
}
#endif //_DEBUG

bool CVirtualMarket::SchedulingTask(void) {
  CalculateTime();
  return true;
}

void CVirtualMarket::ResetMarket(void) {
}

void CVirtualMarket::CalculateTime(void) noexcept {
  time(&sm_tLocal);
  m_tMarket = sm_tLocal - m_lTimeZoneOffset;
  gmtime_s(&m_tmMarket, &m_tMarket);
  m_lMarketToday = (m_tmMarket.tm_year + 1900) * 10000 + (m_tmMarket.tm_mon + 1) * 100 + m_tmMarket.tm_mday;
  m_lMarketTime = m_tmMarket.tm_hour * 10000 + m_tmMarket.tm_min * 100 + m_tmMarket.tm_sec;
}

void CVirtualMarket::CalculateLastTradeDay(void) noexcept {
  time_t ttime = 0;

  switch (m_tmMarket.tm_wday) {
  case 1: // 星期一
  ttime = m_tMarket - 3 * 24 * 3600; //
  break;
  case 0: //星期日
  ttime = m_tMarket - 3 * 24 * 3600; //
  break;
  case 6: // 星期六
  ttime = m_tMarket - 2 * 24 * 3600; //
  break;
  default: // 其他
  ttime = m_tMarket - 24 * 3600; //
  }
  tm tm_;
  gmtime_s(&tm_, &ttime);
  m_lMarketLastTradeDay = (tm_.tm_year + 1900) * 10000 + (tm_.tm_mon + 1) * 100 + tm_.tm_mday;
}

bool CVirtualMarket::IsWorkingDay(void) noexcept {
  if ((m_tmMarket.tm_wday == 0) || (m_tmMarket.tm_wday == 6)) {
    return false;
  }
  else return true;
}

bool CVirtualMarket::IsWorkingDay(CTime timeCurrent) noexcept {
  if ((timeCurrent.GetDayOfWeek() == 1) || (timeCurrent.GetDayOfWeek() == 7)) {
    return false;
  }
  else return true;
}

bool CVirtualMarket::IsWorkingDay(long lDay) noexcept {
  const long year = lDay / 10000;
  const long month = lDay / 100 - year * 100;
  const long day = lDay - year * 10000 - month * 100;
  CTime ct(year, month, day, 12, 0, 0);

  if ((ct.GetDayOfWeek() == 1) || (ct.GetDayOfWeek() == 7)) {
    return false;
  }
  else return true;
}

bool CVirtualMarket::IsEarlyThen(long lEarlyDay, long lLatelyDay, long lTimeSpawnOfDays) noexcept {
  CTimeSpan ts(lTimeSpawnOfDays, 0, 0, 0);
  const long year = lEarlyDay / 10000;
  const long month = lEarlyDay / 100 - year * 100;
  const long day = lEarlyDay - year * 10000 - month * 100;
  CTime ctEarly(year, month, day, 12, 0, 0);
  ctEarly += ts;
  long lNewDay = ctEarly.GetYear() * 10000 + ctEarly.GetMonth() * 100 + ctEarly.GetDay();
  return (lNewDay < lLatelyDay);
}

long CVirtualMarket::GetNextDay(long lDay, long lTimeSpanDays) noexcept {
  CTimeSpan ts(lTimeSpanDays, 0, 0, 0);
  const long year = lDay / 10000;
  const long month = lDay / 100 - year * 100;
  const long day = lDay - year * 10000 - month * 100;
  CTime ctDay(year, month, day, 12, 0, 0);
  ctDay += ts;
  long lNewDay = ctDay.GetYear() * 10000 + ctDay.GetMonth() * 100 + ctDay.GetDay();
  return (lNewDay);
}

CString CVirtualMarket::GetTimeString(void) {
  char buffer[30];
  tm tmLocal;

  localtime_s(&tmLocal, &sm_tLocal);
  sprintf_s(buffer, "%02d:%02d:%02d ", tmLocal.tm_hour, tmLocal.tm_min, tmLocal.tm_sec);
  CString str;
  str = buffer;
  return(str);
}

CString CVirtualMarket::GetMarketTimeString(void) {
  char buffer[30];
  tm tmMarket;

  gmtime_s(&tmMarket, &m_tMarket);
  sprintf_s(buffer, "%02d:%02d:%02d ", tmMarket.tm_hour, tmMarket.tm_min, tmMarket.tm_sec);
  CString str;
  str = buffer;
  return(str);
}

void CVirtualMarket::TaskResetMarketFlagAtMidnight(long lCurrentTime) {
  // 午夜过后重置各种标识
  if (lCurrentTime <= 1500 && !IsPermitResetMarket()) {  // 在零点到零点十五分，重置系统标识
    m_fPermitResetMarket = true;
    CString str;
    str = m_strMarketId + _T("重置系统重置标识");
    TRACE(_T("%s\n"), str);
    gl_systemMessage.PushInformationMessage(str);
  }
}