#include"stdafx.h"
#include"globedef.h"
#include"Thread.h"
#include"ChinaMarket.h"

#include"Accessory.h"

#include "PotenDailyBriefingWebInquiry.h"

using namespace std;
#include<thread>

CPotenDailyBriefingWebInquiry::CPotenDailyBriefingWebInquiry() : CVirtualWebInquiry() {
  m_strWebDataInquirePrefix = _T("http://energy.poten.com/poten-daily-briefing-webpage-");
  m_strWebDataInquireSuffix = _T("");
  m_strConnection = _T("PotenDailyBriefing");

  m_lInquiringDay = 20180411; //poten.com网站的最新格式从此日期之后开始，之前的格式暂时不去读取。
}

CPotenDailyBriefingWebInquiry::~CPotenDailyBriefingWebInquiry() {
}

bool CPotenDailyBriefingWebInquiry::PrepareNextInquiringStr(void) {
  m_lInquiringDay = gl_pPotenDailyBriefingMarket->GetCurrentInquiringDay();
  CString strMiddle = _T("");
  char buffer[50];
  long year = m_lInquiringDay / 10000;
  long month = m_lInquiringDay / 100 - year * 100;
  long day = m_lInquiringDay - year * 10000 - month * 100;

  sprintf_s(buffer, _T("%02d/%02d/%04d"), month, day, year);
  strMiddle = buffer;
  CreateTotalInquiringString(strMiddle);
  TRACE(_T("读取%08d日的poten数据\n"), m_lInquiringDay);
  return true;
}
void CPotenDailyBriefingWebInquiry::StartReadingThread(void) {
  thread thread1(ThreadReadPotenDailyBriefing, this);
  thread1.detach();
}