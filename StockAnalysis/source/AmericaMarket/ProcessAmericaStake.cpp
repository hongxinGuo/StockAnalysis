#include"stdafx.h"
#include"globedef.h"
#include"WebData.h"
#include"ProcessAmericaStake.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace boost::property_tree;

static char s_buffer[2048 * 1024];

bool ConvertToJSon(ptree& pt, CWebDataPtr pWebData) {
  for (int i = 0; i < pWebData->GetBufferLength(); i++) {
    s_buffer[i] = pWebData->GetData(i);
  }
  s_buffer[pWebData->GetBufferLength()] = 0x000;
  string strTemp = s_buffer;
  stringstream ss(strTemp);

  try {
    read_json(ss, pt);
  }
  catch (ptree_error&) {
    return false;
  }
  return true;
}

bool ProcessAmericaStakeProfile(CWebDataPtr pWebData) {
  CAmericaStakePtr pStake = make_shared<CAmericaStake>();
  ptree pt;
  string s;

  if (pWebData->GetBufferLength() < 20) {
    pStake = gl_pAmericaStakeMarket->GetCurrentProcessingAmericaStake();
    pStake->m_lAmericaStakeUpdateDate = gl_pAmericaStakeMarket->GetFormatedMarketDate();
    pStake->m_fUpdateDatabase = true;
    return false; // 没有公司简介
  }
  if (!ConvertToJSon(pt, pWebData)) return false;
  pStake = gl_pAmericaStakeMarket->GetCurrentProcessingAmericaStake();
  s = pt.get<string>(_T("ticker"));
  if (s.size() > 0) pStake->m_strTicker = s.c_str();
  s = pt.get<string>(_T("country"));
  if (s.size() > 0) pStake->m_strCountry = s.c_str();
  s = pt.get<string>(_T("currency"));
  if (s.size() > 0) pStake->m_strCurrency = s.c_str();
  s = pt.get<string>(_T("exchange"));
  if (s.size() > 0) pStake->m_strExchange = s.c_str();
  s = pt.get<string>(_T("name"));
  if (s.size() > 0) pStake->m_strName = s.c_str();
  s = pt.get<string>(_T("finnhubIndustry"));
  if (s.size() > 0) pStake->m_strFinnhubIndustry = s.c_str();
  s = pt.get<string>(_T("logo"));
  if (s.size() > 0) pStake->m_strLogo = s.c_str();
  s = pt.get<string>(_T("marketCapitalization"));
  if (s.size() > 0) pStake->m_dMarketCapitalization = atof(s.c_str());
  s = pt.get<string>(_T("phone"));
  if (s.size() > 0) pStake->m_strPhone = s.c_str();
  if (s.size() > 0) pStake->m_dShareOutstanding = pt.get<double>(_T("shareOutstanding"));
  s = pt.get<string>(_T("weburl"));
  if (s.size() > 0) pStake->m_strWebURL = s.c_str();
  s = pt.get<string>(_T("ipo"));
  if (s.size() > 0) pStake->m_strIPODate = s.c_str();
  pStake->m_lAmericaStakeUpdateDate = gl_pAmericaStakeMarket->GetFormatedMarketDate();
  pStake->m_fUpdateDatabase = true;
  return true;
}

bool ProcessAmericaStakeSymbol(CWebDataPtr pWebData) {
  CAmericaStakePtr pStake = make_shared<CAmericaStake>();
  ptree pt, pt2;
  string s;

  if (!ConvertToJSon(pt, pWebData)) return false;
  for (ptree::iterator it = pt.begin(); it != pt.end(); ++it) {
    pStake = make_shared<CAmericaStake>();
    pt2 = it->second;
    s = pt2.get<string>(_T("description"));
    if (s.size() > 0) pStake->m_strDescription = s.c_str();
    s = pt2.get<string>(_T("displaySymbol"));
    pStake->m_strDisplaySymbol = s.c_str();
    s = pt2.get<string>(_T("symbol"));
    pStake->m_strSymbol = s.c_str();
    s = pt2.get<string>(_T("type"));
    if (s.size() > 0) pStake->m_strType = s.c_str();
    s = pt2.get<string>(_T("currency"));
    if (s.size() > 0) pStake->m_strCurrency = s.c_str();
    if (!gl_pAmericaStakeMarket->IsAmericaStake(pStake->m_strSymbol)) { // 新代码？
      gl_pAmericaStakeMarket->AddAmericaStake(pStake);
    }
  }
  return true;
}

bool ProcessAmericaStakeCandle(CWebDataPtr pWebData, CAmericaStakePtr& pStake) {
  vector<CDayLinePtr> vDayLine;
  ptree pt, pt2, pt3;
  string s;
  double dTemp = 0;
  long lTemp = 0;
  INT64 llTemp = 0;
  time_t tTemp = 0;
  CDayLinePtr pDayLine = nullptr;
  int i = 0;

  if (!ConvertToJSon(pt, pWebData)) return false;
  s = pt.get<string>(_T("s"));
  if (s.compare(_T("ok")) != 0) return false;
  try {
    pt2 = pt.get_child(_T("t"));
    for (ptree::iterator it = pt2.begin(); it != pt2.end(); ++it) {
      pt3 = it->second;
      tTemp = pt3.get_value<time_t>();
      pDayLine = make_shared<CDayLine>();
      pDayLine->SetTime(tTemp);
      lTemp = FormatToDate(tTemp);
      pDayLine->SetDate(lTemp);
      vDayLine.push_back(pDayLine);
    }
  }
  catch (ptree_error&) {
    return false;
  }
  try {
    pt2 = pt.get_child(_T("c"));
    i = 0;
    for (ptree::iterator it = pt2.begin(); it != pt2.end(); ++it) {
      pt3 = it->second;
      dTemp = pt3.get_value<double>();
      pDayLine = vDayLine.at(i++);
      pDayLine->SetClose(dTemp * 1000);
    }
  }
  catch (ptree_error&) {
  }
  try {
    pt2 = pt.get_child(_T("o"));
    i = 0;
    for (ptree::iterator it = pt2.begin(); it != pt2.end(); ++it) {
      pt3 = it->second;
      dTemp = pt3.get_value<double>();
      pDayLine = vDayLine.at(i++);
      pDayLine->SetOpen(dTemp * 1000);
    }
  }
  catch (ptree_error&) {
  }
  try {
    pt2 = pt.get_child(_T("h"));
    i = 0;
    for (ptree::iterator it = pt2.begin(); it != pt2.end(); ++it) {
      pt3 = it->second;
      dTemp = pt3.get_value<double>();
      pDayLine = vDayLine.at(i++);
      pDayLine->SetHigh(dTemp * 1000);
    }
  }
  catch (ptree_error&) {
  }
  try {
    pt2 = pt.get_child(_T("l"));
    i = 0;
    for (ptree::iterator it = pt2.begin(); it != pt2.end(); ++it) {
      pt3 = it->second;
      dTemp = pt3.get_value<double>();
      pDayLine = vDayLine.at(i++);
      pDayLine->SetLow(dTemp * 1000);
    }
  }
  catch (ptree_error&) {
  }
  try {
    pt2 = pt.get_child(_T("v"));
    i = 0;
    for (ptree::iterator it = pt2.begin(); it != pt2.end(); ++it) {
      pt3 = it->second;
      llTemp = pt3.get_value<INT64>();
      pDayLine = vDayLine.at(i++);
      pDayLine->SetVolume(llTemp);
    }
  }
  catch (ptree_error&) {
  }
  pStake->UpdateDayLine(vDayLine);
  pStake->SetDayLineNeedUpdate(false);
  pStake->SetDayLineNeedSaving(true);
  return true;
}