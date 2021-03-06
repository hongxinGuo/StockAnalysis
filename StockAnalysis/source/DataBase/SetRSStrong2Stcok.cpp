#include"pch.h"
#include"globedef.h"
#include"Accessory.h"

#include "SetRSStrong2Stock.h"

IMPLEMENT_DYNAMIC(CSetRSStrong2Stock, CRecordset)

CSetRSStrong2Stock::CSetRSStrong2Stock(CDatabase* pdb) : CRecordset(pdb) {
  m_Symbol = _T("");
  m_nFields = 1;
}

CString CSetRSStrong2Stock::GetDefaultConnect() {
  return GetChinaMarketSchemaConnect();
}

CString CSetRSStrong2Stock::GetDefaultSQL() {
  return _T("[10rs2above55]");
}

void CSetRSStrong2Stock::DoFieldExchange(CFieldExchange* pFX) {
  pFX->SetFieldType(CFieldExchange::outputColumn);
  // RFX_Text() 和 RFX_Int() 这类宏依赖的是
  // 成员变量的类型，而不是数据库字段的类型。
  // ODBC 尝试自动将列值转换为所请求的类型
  RFX_Text(pFX, _T("[Symbol]"), m_Symbol);
}
/////////////////////////////////////////////////////////////////////////////
// CSetRSStrong2Stock 诊断

#ifdef _DEBUG
void CSetRSStrong2Stock::AssertValid() const {
  CRecordset::AssertValid();
}

void CSetRSStrong2Stock::Dump(CDumpContext& dc) const {
  CRecordset::Dump(dc);
}
#endif //_DEBUG