#include"globedef.h"

CPriorityQueueRTData::CPriorityQueueRTData() {
}

CPriorityQueueRTData::~CPriorityQueueRTData() {
}

void CPriorityQueueRTData::Reset(void) {
  size_t lTotal = m_priorityqueueRTStockData.size();
  for (int i = 0; i < lTotal; i++) { // ��ն���
    m_priorityqueueRTStockData.pop();
  }
}

void CPriorityQueueRTData::PushRTData(CWebRTDataPtr pData) {
  m_priorityqueueRTStockData.push(pData);
}

CWebRTDataPtr CPriorityQueueRTData::PopRTData(void) {
  CWebRTDataPtr pData;
  pData = m_priorityqueueRTStockData.top();
  m_priorityqueueRTStockData.pop();
  return pData;
}

size_t CPriorityQueueRTData::GetRTDataSize(void) {
  return(m_priorityqueueRTStockData.size());
}