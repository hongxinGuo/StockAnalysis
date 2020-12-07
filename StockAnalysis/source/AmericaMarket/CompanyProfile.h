#pragma once

#include"stdafx.h"

#include"SetCompanyProfile.h"
#include"CompanySymbol.h"

using namespace std;
#include<memory>

class CCompanyProfile : public CObject {
public:
  CCompanyProfile();
public:
  void Load(CSetCompanyProfile& setCompanyProfile);
  void Save(CSetCompanyProfile& setCompanyProfile);

  void Update(CCompanySymbolPtr pSymbol);

public:
  CString m_strDescription;
  CString m_strDisplaySymbol;
  CString m_strSymbol;
  CString m_strType;
  CString m_strCurrency;
  CString m_strCountry;
  CString m_strExchange;
  long m_lIPODate;
  CString m_strPhone;
  CString m_strMarketCapitalization;
  CString m_strShareOutstanding;
  CString m_strName;
  CString m_strTicker; // 不存储
  CString m_strWebURL;
  CString m_strLogo; // 不存储
  CString m_strFinnHubIndustry;

  // 无需存储变量
  long m_lLastUpdateDate; // 上次更新时间
};

typedef shared_ptr<CCompanyProfile> CCompanyProfilePtr;