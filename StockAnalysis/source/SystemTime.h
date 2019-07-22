#pragma once

#include"afxmt.h"

class CSystemTime {
public:
	CSystemTime(void);
	~CSystemTime(void);

  time_t				Gett_time(void) { return m_ttime; }
  void					Sett_time(time_t Time);

  long          GetDay(void) noexcept { return m_lToday; }
  long          GetTime(void) noexcept { return m_lTime; }
  long          GetLastTradeDay(void) noexcept { return m_lLastTradeDay; }
  long          GetDayOfWeek(void) noexcept { return m_tm.tm_wday; }


  void          CalculateTime(void);
  void          CalculateLastTradeDay(void);

	long					ChangeTimeToDay( time_t time ); // 将时间转变为整型(YYYYMMDD)形式
	time_t				ChangeDayToMarketCloseTime( long lDay );// 将整型(YYYYMMDD)转变为time_t形式
  CString       GetTimeString(void);

protected:
  long    m_lToday;          // 今日日期。格式为：19990102
  long    m_lTime;				  // 今日时间。格式为：hhmmss
  long    m_lLastTradeDay; // 上一个交易日，用于判断是否申请日线数据。日线数据的最新日，采用上一个交易日，本交易日的数据由实时数据产生。
  tm      m_tm;						  // 全局时间。由CMainFrame的OnTimer函数负责更新
  time_t  m_ttime;       // 全局时间。由CMainFrame的OnTimer函数负责更新

};
