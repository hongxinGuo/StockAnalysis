/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 网络通用数据包结构。
//
// 用于存储从网路上提取的各种原始数据。
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include"stdafx.h"

using namespace std;
#include<memory>
#include<vector>

class CWebData : public CObject {
public:
  CWebData() noexcept {
    m_lTime = 0;
    m_lBufferLength = 2048 * 1024;
    m_pCurrentPos = nullptr;
    m_lCurrentPos = 0;
  }
  ~CWebData() {
  }

  bool IsProcessedAllTheData(void) noexcept { if (m_lCurrentPos < m_lBufferLength) return false; else return true; }

  void IncreaseCurrentPos(long lNumberOfChars = 1) noexcept { m_pCurrentPos += lNumberOfChars; m_lCurrentPos += lNumberOfChars; }
  void ResetCurrentPos(void) noexcept { m_pCurrentPos = m_pDataBuffer; m_lCurrentPos = 0; }

  //void Resize(long lSize) noexcept { m_pDataBuffer.resize(lSize); m_lBufferLength = lSize; }

  INT64 GetTime(void) noexcept { return m_lTime; }
  void SetTime(INT64 lTime) noexcept { m_lTime = lTime; }
  long GetBufferLength(void) noexcept { return m_lBufferLength; }
  void SetBufferLength(long lValue) noexcept { m_lBufferLength = lValue; }
  long GetCurrentPos(void) noexcept { return m_lCurrentPos; }
  void SetCurrentPos(long lValue) noexcept { m_lCurrentPos = lValue; }

  bool GetData(char* buffer, long lDataLength, long lStartPosition);
  bool GetData(char* buffer, long lDataLength); // 默认从m_lCurrentPos开始拷贝
  bool SetData(char* buffer, long lDataLength, long lStartPosition);
  bool SetData(char* buffer, long lDataLength); // 默认从m_lCurrenPos开始填充。

  char GetData(long lIndex) { return m_pDataBuffer[lIndex]; }
  void SetData(long lIndex, char cValue) { m_pDataBuffer[lIndex] = cValue; }
  char GetCurrentPosData(void) { return m_pDataBuffer[m_lCurrentPos]; }
  void SetCurrentPosData(char cValue) { m_pDataBuffer[m_lCurrentPos] = cValue; }

  // 测试用函数
  void __TEST_SetBuffer__(CString strBuffer);
  char* __TEST_GetBufferAddr(void) noexcept { return m_pDataBuffer; }

public:
  char* m_pCurrentPos; // 当前处理的位置

protected:
  INT64 m_lTime; // 此数据的提取时间。格式为yyyymmddhhmmss
  char m_pDataBuffer[2048 * 1024];
  long m_lBufferLength;
  long m_lCurrentPos;

  // 采用vector实现所需之变量
  vector<char> m_vDataBuffer;
};

typedef shared_ptr<CWebData> CWebDataPtr;
