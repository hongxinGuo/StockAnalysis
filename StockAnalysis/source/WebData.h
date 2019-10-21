#pragma once

#include"stdafx.h"
#include"globedef.h"

class CWebData {
public:
  CWebData() noexcept {
    m_lByteRead = 0;
    m_fError = false;
    m_strInquire = "";
  }
  ~CWebData() {}

  // 公共接口函数
  bool GetWebData(void);

  virtual void ProcessCurrentWebData(void);
  virtual bool SucceedReadingAndStoringWebData(void);

  // 下列为继承类必须实现的几个功能函数，完成具体任务。
  virtual bool SucceedReadingAndStoringOneWebData(char*& pCurrentPos, long& iCount) = 0; // 读取并存储一个数据进数据队列
  virtual void ProcessWebDataStored(void) = 0; // 处理存储后的数据队列
  virtual void ReportDataError(void) = 0;       // 读取数据中报错
  virtual void ReportCommunicationError(void) = 0;  // 网络出现问题时报错
  virtual void InquireNextWebData(void) = 0;        // 申请下一个网络数据
  virtual int  GetInquiringStockStr(CString& strInquire) = 0;// 申请下一个查询用字符串
  virtual void StartReadingThread(void) = 0;    // 调用网络读取线程。

  bool IsReadingSucceed(void) { if (m_fError) return false; else return true; }
  bool IsWebDataReceived(void);
  void SetWebDataReceived(bool fFlag);
  bool IsReadingWebData(void);
  void SetReadingWebData(bool fFlag);

public:
  CString m_strInquire;         // 查询所需的字符串
  char m_buffer[2048 * 1024];   // 接收到数据的缓冲区
  long m_lByteRead;             // 接收到的字符数
  bool m_fError;                // 网络是否异常

  CString m_strWebDataInquirePrefix; // 查询字符串前缀
  CString m_strWebDataInquireSuffix; // 查询字符串后缀

  bool m_fReadingWebData;     // 接收实时数据线程是否执行标识
  CCriticalSection m_ReadingWebDataLock;  //是否接收实时数据的同步锁
  bool m_fWebDataReceived; // 实时数据已接收完毕标识
  CCriticalSection m_WebDataReceivedLock;  // 实时数据接收完毕标识的同步锁
};
