#pragma once

using namespace std;
#include<atomic>

constexpr int cMaxBackGroundTaskThreads = 8;

class CThreadStatus { // 个线程状态
public:
  CThreadStatus();
  ~CThreadStatus();

  // 计算若干天日线相对强度与否和设置
  void SetCalculatingDayLineRS(bool fFlag) noexcept { m_CalculatingDayLineRS = fFlag; }
  bool IsCalculatingDayLineRS(void) noexcept { return m_CalculatingDayLineRS; }

  // 计算若干天周线相对强度与否和设置
  void SetCalculatingWeekLineRS(bool fFlag) noexcept { m_CalculatingWeekLineRS = fFlag; }
  bool IsCalculatingWeekLineRS(void) noexcept { return m_CalculatingWeekLineRS; }

  // 实时数据需要计算与否和设置
  void SetRTDataNeedCalculate(bool fFlag) noexcept { m_RTDataNeedCalculate = fFlag; }
  bool IsRTDataNeedCalculate(void) noexcept { return m_RTDataNeedCalculate; }

  // 实时数据计算中与否和设置
  void SetCalculatingRTData(bool fFlag) noexcept { m_CalculatingRTData = fFlag; }
  bool IsCalculatingRTData(void) noexcept { return m_CalculatingRTData; }

  // 临时数据存储中与否和设置
  void SetSavingTempData(bool fFlag) noexcept { m_SavingTempData = fFlag; }
  bool IsSavingTempData(void) noexcept { return m_SavingTempData; }

  // 生成周线历史数据
  void SetCreatingWeekLine(bool fFlag) noexcept { m_CreatingWeekLine = fFlag; }
  bool IsCreatingWeekLine(void) noexcept { return m_CreatingWeekLine; }

  // 并发执行计算日线相对强度的计数器，最多允许cMaxBackGroundTaskThreads个线程同时执行
  void IncreaseBackGroundWorkingthreads(void) noexcept { m_NumberOfBackGroundWorkingThreads++; }  // 同时运行线程数加一
  void DecreaseBackGroundWorkingthreads(void) noexcept { m_NumberOfBackGroundWorkingThreads--; } // 同时运行线程数减一
  bool IsBackGroundthreadsWorking(void) noexcept { if (m_NumberOfBackGroundWorkingThreads > 0) return true; else return false; } // 计算日线的线程是否处于运行中
  int GetNumberOfBackGroundWorkingThread(void) noexcept { return m_NumberOfBackGroundWorkingThreads; }

  void IncreaseSavingThread(void) noexcept { m_NumberOfSavingThread++; }
  void DecreaseSavingThread(void) noexcept { if (m_NumberOfSavingThread > 0) m_NumberOfSavingThread--; }
  int GetNumberOfSavingThread(void) noexcept { const int iNumber = m_NumberOfSavingThread; return iNumber; }
  bool IsSavingThreadRunning(void) noexcept { if (GetNumberOfSavingThread() > 0) return true; else return false; }

  void IncreaseWebInquiringThread(void) noexcept { m_NumberOfWebInquiringThread++; }
  void DecreaseWebInquiringThread(void) noexcept { if (m_NumberOfWebInquiringThread > 0) m_NumberOfWebInquiringThread--; }
  int GetNumberOfWebInquiringThread(void) noexcept { const int iNumber = m_NumberOfWebInquiringThread; return iNumber; }
  bool IsWebInquiringThreadRunning(void) noexcept { if (GetNumberOfWebInquiringThread() > 0) return true; else return false; }

protected:
  atomic_bool m_CalculatingDayLineRS;
  atomic_bool m_CalculatingWeekLineRS;
  atomic_bool m_RTDataNeedCalculate;
  atomic_bool m_CalculatingRTData;
  atomic_bool m_SavingTempData;
  atomic_bool m_CreatingWeekLine;

  atomic_int m_NumberOfBackGroundWorkingThreads;  // 正在计算日线相对强度的线程数。目前最多同时允许cMaxBackGroundTaskThreads个线程
  atomic_int m_NumberOfSavingThread; // 正在运行的存储工作线程数量
  atomic_int m_NumberOfWebInquiringThread; // 正在运行的提取网络数据工作线程数量
};
