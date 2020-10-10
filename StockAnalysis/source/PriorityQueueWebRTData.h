#pragma once

#include"WebRTData.h"

using namespace std;
#include<queue>

class CPriorityQueueWebRTData final
{
public:
  CPriorityQueueWebRTData();
  ~CPriorityQueueWebRTData();
  void Reset(void);

  // 通用接口函数
  void PushData(CWebRTDataPtr pData);
  CWebRTDataPtr PopData(void);
  CWebRTDataPtr GetHead(void) noexcept { return m_priorityqueueStockData.top(); }
  size_t GetDataSize(void);

protected:
  // 需要定义下述结构，结构中重载（）运算符，定义如何确定指针的大小（按时间顺序从小到大排列,相同时间的按先后放入的顺序排列）。
  struct cmpRTData {
    bool operator() (const CWebRTDataPtr p1, const CWebRTDataPtr p2) const  noexcept {
      // 有优先级的队列默认排列顺序是从大到小，故而从小到大排列需要使用 > 符号。
      // 相同时间的数据，先放入的数据位于后放入的数据前面。
      return(p1->GetTransactionTime() > p2->GetTransactionTime());
    }
  };

protected:
  // 当系统更新到采集多元实时数据后，此队列需要改为有优先级的队列，优先级为实时数据的交易时间，
  // 这样就能够将多处采集到的实时数据按照时间排列为一个队列。
  // 大致为这样： priority_queue<CStockRTdataPtr, vector<CStockRTDataDataPtr>, cmpRTData> m_qRTStockData;
  // 结构cmpRTData定义为：
  // struct cmpRTData {
  //   bool operator () (const CWebRTDataPtr p1, const CWebRTDataPtr p2) const {
  //     return(p1->GetFormatedMarketTime() > p2->GetFormatedMarketTime());
  //   }
  // };
  priority_queue<CWebRTDataPtr, vector<CWebRTDataPtr>, cmpRTData> m_priorityqueueStockData;
};