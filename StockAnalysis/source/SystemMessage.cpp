#include"globedef.h"

#include "SystemMessage.h"

CSystemMessage::CSystemMessage()
{
  static int siCounter = 0;
  if (siCounter++ > 0) {
    TRACE("系统消息只允许一个实例\n");
    gl_systemMessage.PushInformationMessage(_T("错误：系统不允许生成多个CMarket实例"));
  }
}

CSystemMessage::~CSystemMessage()
{
  m_dequeDayLineInfoMessage.clear();
  m_dequeTransactionMessage.clear();
  m_dequeInformationMessage.clear();
  m_dequeCancelSellMessage.clear();
  m_dequeCancelBuyMessage.clear();
  m_dequeTrace2Message.clear();
}

void CSystemMessage::PushInformationMessage(CString str)
{
  CSingleLock singleLock(&m_InformationLock);
  singleLock.Lock();
  if (singleLock.IsLocked()) {
    m_dequeInformationMessage.push_back(str);
    singleLock.Unlock();
  }
}

CString CSystemMessage::PopInformationMessage(void)
{
  CString str;
  CSingleLock singleLock(&m_InformationLock);
  singleLock.Lock();
  if (singleLock.IsLocked()) {  // 如果没有锁住，则线程终止，一直等待
    str = m_dequeInformationMessage.front();
    m_dequeInformationMessage.pop_front();
    singleLock.Unlock();
    return str;     // 只能从这里返回
  }
  ASSERT(0);
  return ""; // 此分支不可能执行到，只为了消除编译器的警告而存在
}

CString CSystemMessage::PopDayLineInfoMessage(void)
{
  CString str;
  CSingleLock singleLock(&m_DayLineInfoLock);
  singleLock.Lock();
  if (singleLock.IsLocked()) {
    str = m_dequeDayLineInfoMessage.front();
    m_dequeDayLineInfoMessage.pop_front();
    singleLock.Unlock();
    return str;
  }
  ASSERT(0);
  return ""; // 此分支不可能执行到，只为了消除编译器的警告而存在
}

CString CSystemMessage::PopTransactionMessage(void)
{
  CString str;
  CSingleLock singleLock(&m_TransactionLock);
  singleLock.Lock();
  if (singleLock.IsLocked()) {
    str = m_dequeTransactionMessage.front();
    m_dequeTransactionMessage.pop_front();
    singleLock.Unlock();
    return str;
  }
  ASSERT(0);
  return ""; // 此分支不可能执行到，只为了消除编译器的警告而存在
}

CString CSystemMessage::PopCancelSellMessage(void)
{
  CString str;
  CSingleLock singleLock(&m_CancelSellLock);
  singleLock.Lock();
  if (singleLock.IsLocked()) {
    str = m_dequeCancelSellMessage.front();
    m_dequeCancelSellMessage.pop_front();
    singleLock.Unlock();
    return str;
  }
  ASSERT(0);
  return ""; // 此分支不可能执行到，只为了消除编译器的警告而存在
}

CString CSystemMessage::PopCancelBuyMessage(void)
{
  CString str;
  CSingleLock singleLock(&m_CancelBuyLock);
  singleLock.Lock();
  if (singleLock.IsLocked()) {
    str = m_dequeCancelBuyMessage.front();
    m_dequeCancelBuyMessage.pop_front();
    singleLock.Unlock();
    return str;
  }
  ASSERT(0);
  return ""; // 此分支不可能执行到，只为了消除编译器的警告而存在
}

CString CSystemMessage::PopTrace2Message(void)
{
  CString str;
  CSingleLock singleLock(&m_Trace2Lock);
  singleLock.Lock();
  if (singleLock.IsLocked()) {
    str = m_dequeTrace2Message.front();
    m_dequeTrace2Message.pop_front();
    singleLock.Unlock();
    return str;
  }
  ASSERT(0);
  return ""; // 此分支不可能执行到，只为了消除编译器的警告而存在
}

long CSystemMessage::GetInformationDequeSize(void)
{
  CSingleLock singleLock(&m_InformationLock);
  singleLock.Lock();
  if (singleLock.IsLocked()) {
    const long lCount = m_dequeInformationMessage.size();
    singleLock.Unlock();
    return lCount;
  }
  ASSERT(0);
  return false; // 此分支不可能执行到，只为了消除编译器的警告而存在
}

long CSystemMessage::GetDayLineInfoDequeSize(void)
{
  CSingleLock singleLock(&m_DayLineInfoLock);
  singleLock.Lock();
  if (singleLock.IsLocked()) {
    const long lCount = m_dequeDayLineInfoMessage.size();
    singleLock.Unlock();
    return lCount;
  }
  ASSERT(0);
  return false; // 此分支不可能执行到，只为了消除编译器的警告而存在
}

long CSystemMessage::GetTransactionDequeSize(void)
{
  CSingleLock singleLock(&m_TransactionLock);
  singleLock.Lock();
  if (singleLock.IsLocked()) {
    const long lCount = m_dequeTransactionMessage.size();
    singleLock.Unlock();
    return lCount;
  }
  ASSERT(0);
  return false; // 此分支不可能执行到，只为了消除编译器的警告而存在
}

long CSystemMessage::GetCancelSellDequeSize(void)
{
  CSingleLock singleLock(&m_CancelSellLock);
  singleLock.Lock();
  if (singleLock.IsLocked()) {
    const long lCount = m_dequeCancelSellMessage.size();
    singleLock.Unlock();
    return lCount;
  }
  ASSERT(0);
  return false; // 此分支不可能执行到，只为了消除编译器的警告而存在
}

long CSystemMessage::GetCancelBuyDequeSize(void)
{
  CSingleLock singleLock(&m_CancelBuyLock);
  singleLock.Lock();
  if (singleLock.IsLocked()) {
    const long lCount = m_dequeCancelBuyMessage.size();
    singleLock.Unlock();
    return lCount;
  }
  ASSERT(0);
  return false; // 此分支不可能执行到，只为了消除编译器的警告而存在
}

long CSystemMessage::GetTrace2DequeSize(void)
{
  CSingleLock singleLock(&m_Trace2Lock);
  singleLock.Lock();
  if (singleLock.IsLocked()) {
    const long lCount = m_dequeTrace2Message.size();
    singleLock.Unlock();
    return lCount;
  }
  ASSERT(0);
  return false; // 此分支不可能执行到，只为了消除编译器的警告而存在
}

void CSystemMessage::PushDayLineInfoMessage(CString str)
{
  CSingleLock singleLock(&m_DayLineInfoLock);
  singleLock.Lock();
  if (singleLock.IsLocked()) {
    m_dequeDayLineInfoMessage.push_back(str);
    singleLock.Unlock();
  }
}

void CSystemMessage::PushTransactionMessage(CString str)
{
  CSingleLock singleLock(&m_TransactionLock);
  singleLock.Lock();
  if (singleLock.IsLocked()) {
    m_dequeTransactionMessage.push_back(str);
    singleLock.Unlock();
  }
}

void CSystemMessage::PushCancelSellMessage(CString str)
{
  CSingleLock singleLock(&m_CancelSellLock);
  singleLock.Lock();
  if (singleLock.IsLocked()) {
    m_dequeCancelSellMessage.push_back(str);
    singleLock.Unlock();
  }
}

void CSystemMessage::PushCancelBuyMessage(CString str)
{
  CSingleLock singleLock(&m_CancelBuyLock);
  singleLock.Lock();
  if (singleLock.IsLocked()) {
    m_dequeCancelBuyMessage.push_back(str);
    singleLock.Unlock();
  }
}

void CSystemMessage::PushTrace2Message(CString str)
{
  CSingleLock singleLock(&m_Trace2Lock);
  singleLock.Lock();
  if (singleLock.IsLocked()) {
    m_dequeTrace2Message.push_back(str);
    singleLock.Unlock();
  }
}