#pragma once
#include"afxdb.h"

class CSetRSOption : public CRecordset
{
public:
  CSetRSOption(CDatabase* pDatabase = nullptr);
  DECLARE_DYNAMIC(CSetRSOption)

  long m_ID;
  long m_Index;
  BOOL m_Active;
  long m_DayLengthFirst;
  long m_DayLengthSecond;
  long m_DayLengthThird;
  long m_DayLengthFourth;
  long m_StrongDayLengthFirst;
  long m_StrongDayLengthSecond;
  long m_StrongDayLengthThird;
  long m_StrongDayLengthFourth;
  CString m_RSStrongFirst;
  CString m_RSStrongSecond;
  CString m_RSStrongThird;
  CString m_RSStrongFourth;
public:
  virtual CString GetDefaultConnect();	// 默认连接字符串

  virtual CString GetDefaultSQL(); 	// 记录集的默认 SQL
  virtual void DoFieldExchange(CFieldExchange* pFX);	// RFX 支持

// 实现
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif
};
