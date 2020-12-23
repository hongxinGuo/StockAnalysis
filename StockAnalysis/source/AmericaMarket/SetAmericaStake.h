// SetStockCode.h : CSetStockCode 的声明

#pragma once
#include"afxdb.h"

// 代码生成在 2019年5月26日, 8:14

class CSetAmericaStake : public CRecordset
{
public:
  CSetAmericaStake(CDatabase* pDatabase = nullptr);
  DECLARE_DYNAMIC(CSetAmericaStake)

  // 字段/参数数据

  // 以下字符串类型(如果存在)反映数据库字段(ANSI 数据类型的 CStringA 和 Unicode
  // 数据类型的 CStringW)的实际数据类型。
  //  这是为防止 ODBC 驱动程序执行可能
  // 不必要的转换。如果希望，可以将这些成员更改为
  // CString 类型，ODBC 驱动程序将执行所有必要的转换。
  // (注意: 必须使用 3.5 版或更高版本的 ODBC 驱动程序
  // 以同时支持 Unicode 和这些转换)。

  long m_ID;
  CString m_Description;
  CString m_DisplaySymbol;
  CString m_Symbol;
  CString m_Type;
  CString m_Mic;
  CString m_Figi;
  CString m_Currency;
  CString m_Address;
  CString m_City;
  CString m_Country;
  CString m_Cusip;
  CString m_Sedol;
  long m_EmployeeTotal;
  CString m_Exchange;
  CString m_Ggroup;
  CString m_Gind;
  CString m_Gsector;
  CString m_Gsubind;
  CString m_IPODate;
  CString m_Isin;
  CString m_MarketCapitalization;
  CString m_Naics;
  CString m_NaicsNationalIndustry;
  CString m_NaicsSector;
  CString m_NaicsSubsector;
  CString m_Name;
  CString m_Phone;
  CString m_ShareOutstanding;
  CString m_State;
  CString m_Ticker;
  CString m_WebURL;
  CString m_Logo;
  CString m_FinnhubIndustry;
  CString m_Peer;
  long m_ProfileUpdateDate;
  long m_DayLineStartDate;
  long m_DayLineEndDate;
  long m_LastRTDataUpdateDate; // 最新实时数据更新日期
  long m_LastEPSSurpriseUpdateDate;
  long m_IPOStatus;

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
