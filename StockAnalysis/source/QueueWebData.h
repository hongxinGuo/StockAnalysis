///////////////////////////////////////////////////////////
//
// 单向队列，用于存储从网络上提取的数据。由于提取数据采用线程模式，
// 故而此队列需要使用互斥区，以防止发生同步问题。
//
///////////////////////////////////////////////////////////
#pragma once

#include"afxmt.h"

#include"WebData.h"

using namespace std;
#include<queue>
#include<mutex>

class CQueueWebData final
{
public:
  CQueueWebData();
  ~CQueueWebData();
  void Reset(void);

  // 通用接口函数
  void PushWebData(CWebDataPtr pData);
  CWebDataPtr PopWebData(void);
  size_t GetWebDataSize(void);

protected:
  queue<CWebDataPtr> m_qWebData;
  mutex m_MutexAccessWebData;
};
