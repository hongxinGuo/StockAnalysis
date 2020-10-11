#pragma once

#include"stdafx.h"

using namespace std;
#include<memory>

class CWebData : public CObject {
public:
  CWebData() noexcept {
    m_lTime = 0;
    m_pDataBuffer = nullptr;
    m_lBufferLength = 0;
    m_pCurrentPos = nullptr;
    m_lCurrentPos = 0;
  }
  ~CWebData() {
    if (m_pDataBuffer != nullptr) delete m_pDataBuffer;
  }

  bool IsProcessedAllTheData(void) noexcept { if (m_lCurrentPos < m_lBufferLength) return false; else return true; }

  long GetCurrentPos(void) noexcept { return m_lCurrentPos; }
  void IncreaseCurrentPos(long lNumberOfChars = 1) noexcept { m_pCurrentPos += lNumberOfChars; m_lCurrentPos += lNumberOfChars; }
  void ResetCurrentPos(void) noexcept { m_pCurrentPos = m_pDataBuffer; m_lCurrentPos = 0; }
  long GetBufferLength(void) noexcept { return m_lBufferLength; }
  void SetBufferLength(long lValue) noexcept { m_lBufferLength = lValue; }

  // 测试用函数
  void __TEST_SetBuffer__(CString strBuffer);
  char* __TEST_GetBufferAddr(void) noexcept { return m_pDataBuffer; }

public:
  INT64 m_lTime; // 此数据的提取时间。格式为yyyymmddhhmmss
  char* m_pDataBuffer;
  long m_lBufferLength;
  char* m_pCurrentPos; // 当前处理的位置
  long m_lCurrentPos;
};

typedef shared_ptr<CWebData> CWebDataPtr;
