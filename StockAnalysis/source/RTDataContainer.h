/////////////////////////////////////////////////////////////////////////////////////
//
// 存储实时数据的聚合类。只有头文件。
//
//
/////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include"QueueRTData.h"
#include"PriorityQueueRTData.h"

class CRTDataContainer final : public CObject {
public:
  CRTDataContainer();
  ~CRTDataContainer();
  void ResetSinaRTQueue(void) { m_qSinaRTData.Reset(); }
  size_t GetSinaRTDataSize(void) { return m_qSinaRTData.GetRTDataSize(); }
  void PushSinaRTData(CWebRTDataPtr pData) { m_qSinaRTData.PushRTData(pData); }
  CWebRTDataPtr PopSinaRTData(void) { return m_qSinaRTData.PopRTData(); }
  size_t GetTengxunRTDataSize(void) { return m_qTengxunRTData.GetRTDataSize(); }
  void PushTengxunRTData(CWebRTDataPtr pData) { m_qTengxunRTData.PushRTData(pData); }
  CWebRTDataPtr PopTengxunRTData(void) { return m_qTengxunRTData.PopRTData(); }
  size_t GetNeteaseRTDataSize(void) { return m_qNeteaseRTData.GetRTDataSize(); }
  void PushNeteaseRTData(CWebRTDataPtr pData) { m_qNeteaseRTData.PushRTData(pData); }
  CWebRTDataPtr PopNeteaseRTData(void) { return m_qNeteaseRTData.PopRTData(); }

protected:
  CPriorityQueueRTData m_qSinaRTData; // 新浪实时数据队列。
  CQueueRTData m_qSinaRTDataForSave; // 用于存储的新浪实时数据队列
  CPriorityQueueRTData m_qTengxunRTData; // 腾讯实时数据队列。
  CPriorityQueueRTData m_qNeteaseRTData; // 网易实时数据队列。
};
