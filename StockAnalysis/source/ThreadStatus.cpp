#include"stdafx.h"

#include"globedef.h"

#include"ThreadStatus.h"
#include"SystemMessage.h"

CThreadStatus::CThreadStatus() {
  static int siInstance = 0;
  if (siInstance++ > 0) {
    TRACE("ϵͳ״ֻ̬��������һ��ʵ��\n");
    gl_systemMessage.PushInformationMessage(_T("ϵͳ״ֻ̬��������һ��ʵ��"));
  }
  m_CounterOfCalculatingRSThreads = 0;

  m_CalculateDayLineRelativeStrong = false;
  m_RTDataNeedCalculate = false;
  m_CalculatingRTData = false;
  m_SavingTempData = false;

  m_SavingDayLine = 0;
  m_NumberOfRunningThread = 0;
}

CThreadStatus::~CThreadStatus() {
}

bool CThreadStatus::IsSavingDayLine(void) {
  if (m_SavingDayLine > 0) {
    return true;
  }
  else return false;
}