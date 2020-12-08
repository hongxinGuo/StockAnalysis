#include "CompanySymbol.h"

CCompanySymbol::CCompanySymbol() : CObject() {
  lCompanyProfileUpdateDate = 19900101;
}

void CCompanySymbol::Load(CSetCompanySymbol& setCompanySymbol) {
  m_strSymbol = setCompanySymbol.m_Symbol;
  m_strCurrency = setCompanySymbol.m_Currency;
  m_strDescription = setCompanySymbol.m_Description;
  m_strDisplaySymbol = setCompanySymbol.m_DisplaySymbol;
  m_strType = setCompanySymbol.m_Type;
  lCompanyProfileUpdateDate = setCompanySymbol.m_CompanyProfileUpdateDate;
}

void CCompanySymbol::Save(CSetCompanySymbol& setCompanySymbol) {
  setCompanySymbol.AddNew();
  setCompanySymbol.m_Symbol = m_strSymbol;
  setCompanySymbol.m_Currency = m_strCurrency;
  setCompanySymbol.m_Description = m_strDescription;
  setCompanySymbol.m_DisplaySymbol = m_strDisplaySymbol;
  setCompanySymbol.m_Type = m_strType;
  setCompanySymbol.m_CompanyProfileUpdateDate = lCompanyProfileUpdateDate;
  setCompanySymbol.Update();
}