#pragma once

#include"stdafx.h"

class CVirtualMarket : public CObject {
public:
  CVirtualMarket(void);
  ~CVirtualMarket(void);

  virtual bool SchedulingTask(void); // 由程序的定时器调度，大约每100毫秒一次
  virtual void ResetMarket(void) = 0;

  bool SchedulingTaskPerSecond(long lSecondNumber); // 每秒调度一次
  bool SchedulingTaskPer10Seconds(long lSecondNumber, long lCurrentTime); // 每十秒调度一次
  bool SchedulingTaskPer1Minute(long lSecondNumber, long lCurrentTime); // 每一分钟调度一次
  void ResetSystemFlagAtMidnight(long lCurrentTime);
  bool SchedulingTaskPer5Minutes(long lSecondNumber, long lCurrentTime); // 每五分钟调度一次
  bool SchedulingTaskPerHour(long lSecondNumber, long lCurrentTime); // 每小时调度一次

  bool IsReadyToRun(void) noexcept { return m_fReadyToRun; }
  void SetReadyToRun(bool fFlag) noexcept { m_fReadyToRun = fFlag; }
  bool IsPermitResetSystem(void) noexcept { return m_fPermitResetSystem; }
  void SetPermitResetSystem(bool fFlag) noexcept { m_fPermitResetSystem = fFlag; }
  bool IsResetSystem(void) noexcept { return m_fResetSystem; }
  void SetResetSystem(bool fFlag) noexcept { m_fResetSystem = fFlag; }

protected:
  bool m_fPermitResetSystem; // 允许重置系统（如果不断机多日运行的话，需要每日重置系统
  bool m_fResetSystem; // 重启系统标识
  bool m_fReadyToRun;
};
