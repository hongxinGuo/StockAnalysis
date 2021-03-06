// SetChoicedStock.h : CSetChoicedStock 类的实现

// CSetChoicedStock 实现

// 代码生成在 2019年5月26日, 8:15

#include"pch.h"
#include"globedef.h"
#include"Accessory.h"

#include "SetChoicedStock.h"
IMPLEMENT_DYNAMIC(CSetChoicedStock, CRecordset)

CSetChoicedStock::CSetChoicedStock(CDatabase* pdb) : CRecordset(pdb) {
  m_Symbol = _T("");
  m_nFields = 1;
}

CString CSetChoicedStock::GetDefaultConnect() {
  return GetChinaMarketSchemaConnect();
}

CString CSetChoicedStock::GetDefaultSQL() {
  return _T("[ChoicedStocks]");
}

void CSetChoicedStock::DoFieldExchange(CFieldExchange* pFX) {
  pFX->SetFieldType(CFieldExchange::outputColumn);
  // RFX_Text() 和 RFX_Int() 这类宏依赖的是
  // 成员变量的类型，而不是数据库字段的类型。
  // ODBC 尝试自动将列值转换为所请求的类型
  RFX_Text(pFX, _T("[Symbol]"), m_Symbol);
}
/////////////////////////////////////////////////////////////////////////////
// CSetChoicedStock 诊断

#ifdef _DEBUG
void CSetChoicedStock::AssertValid() const {
  CRecordset::AssertValid();
}

void CSetChoicedStock::Dump(CDumpContext& dc) const {
  CRecordset::Dump(dc);
}
#endif //_DEBUG