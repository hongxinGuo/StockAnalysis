// CSetWorldMarketOption.cpp : CSetWorldMarketOption 类的实现

// CSetWorldMarketOption 实现

// 代码生成在 2019年6月2日, 5:10

#include"pch.h"
#include"afxdb.h"
#include"globedef.h"
#include"accessory.h"

#include "SetWorldMarketOption.h"

IMPLEMENT_DYNAMIC(CSetWorldMarketOption, CRecordset)

CSetWorldMarketOption::CSetWorldMarketOption(CDatabase* pdb)
  : CRecordset(pdb) {
  m_ID = 0;
  m_FinnhubToken = _T(" ");
  m_TiingoToken = _T(" ");
  m_QuandlToken = _T(" ");
  m_nFields = 4;
}

CString CSetWorldMarketOption::GetDefaultConnect() {
  return GetWorldMarketSchemaConnect();
}

CString CSetWorldMarketOption::GetDefaultSQL() {
  return _T("[world_market_option]");
}

void CSetWorldMarketOption::DoFieldExchange(CFieldExchange* pFX) {
  pFX->SetFieldType(CFieldExchange::outputColumn);
  // RFX_Text() 和 RFX_Int() 这类宏依赖的是
  // 成员变量的类型，而不是数据库字段的类型。
  // ODBC 尝试自动将列值转换为所请求的类型
  RFX_Long(pFX, _T("[ID]"), m_ID);
  RFX_Text(pFX, _T("FinnhubToken"), m_FinnhubToken);
  RFX_Text(pFX, _T("TiingoToken"), m_TiingoToken);
  RFX_Text(pFX, _T("QuandlToken"), m_QuandlToken);
}
/////////////////////////////////////////////////////////////////////////////
// CSetWorldForexDayLine 诊断

#ifdef _DEBUG
void CSetWorldMarketOption::AssertValid() const {
  CRecordset::AssertValid();
}

void CSetWorldMarketOption::Dump(CDumpContext& dc) const {
  CRecordset::Dump(dc);
}
#endif //_DEBUG