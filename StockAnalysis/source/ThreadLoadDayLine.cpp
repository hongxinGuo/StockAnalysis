#include"globedef.h"

#include"DayLine.h"
#include"SetDayLineInfo.h"
#include"Market.h"

#include"Thread.h"

using namespace std;
#include<memory>

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 日线装载工作线程
//
// 从数据库中装入相应股票的日线数据，然后计算各相对强度
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
UINT ThreadLoadDayLine(LPVOID) {
  CSetDayLine setDayLine;
  CDayLinePtr pDayLine;
  CSetDayLineInfo setDayLineInfo;

  // 装入DayLine数据
  setDayLine.m_strFilter = _T("[StockCode] = '");
  setDayLine.m_strFilter += gl_ChinaStockMarket.m_pCurrentStock->GetStockCode();
  setDayLine.m_strFilter += _T("'");
  setDayLine.m_strSort = _T("[Time]");
  setDayLine.Open();
  gl_ChinaStockMarket.m_pCurrentStock->m_vDayLine.clear();
  while (!setDayLine.IsEOF()) {
    pDayLine = make_shared<CDayLine>();
    pDayLine->SetData(&setDayLine);
    gl_ChinaStockMarket.m_pCurrentStock->m_vDayLine.push_back(pDayLine);
    setDayLine.MoveNext();
  }
  setDayLine.Close();

  // 装入DayLineInfo数据
  setDayLineInfo.m_strFilter = _T("[StockCode] = '");
  setDayLineInfo.m_strFilter += gl_ChinaStockMarket.m_pCurrentStock->GetStockCode();
  setDayLineInfo.m_strFilter += _T("'");
  setDayLineInfo.m_strSort = _T("[Time]");
  setDayLineInfo.Open();
  int iPosition = 0;
  while (!setDayLineInfo.IsEOF()) {
    pDayLine = gl_ChinaStockMarket.m_pCurrentStock->m_vDayLine[iPosition];
    while ((pDayLine->GetDay() < setDayLineInfo.m_Day)
      && (gl_ChinaStockMarket.m_pCurrentStock->m_vDayLine.size() > (iPosition + 1))) {
      iPosition++;
      pDayLine = gl_ChinaStockMarket.m_pCurrentStock->m_vDayLine[iPosition];
    }
    if (pDayLine->GetDay() == setDayLineInfo.m_Day) {
      pDayLine->SetData(&setDayLineInfo);
    }
    if (gl_ChinaStockMarket.m_pCurrentStock->m_vDayLine.size() <= (iPosition + 1)) break;
    setDayLineInfo.MoveNext();
  }
  setDayLineInfo.Close();

  // 计算各相对强度
  double dTempRS = 0;
  const long lTotalNumber = gl_ChinaStockMarket.m_pCurrentStock->m_vDayLine.size();
  for (int i = 3; i < lTotalNumber; i++) {
    dTempRS = 0;
    for (int j = i - 3; j < i; j++) {
      dTempRS += gl_ChinaStockMarket.m_pCurrentStock->m_vDayLine.at(j)->GetRelativeStrong();
    }
    gl_ChinaStockMarket.m_pCurrentStock->m_vDayLine.at(i)->m_d3DayRS = dTempRS / 3;
  }

  dTempRS = 0;
  for (int i = 5; i < lTotalNumber; i++) {
    dTempRS = 0;
    for (int j = i - 5; j < i; j++) {
      dTempRS += gl_ChinaStockMarket.m_pCurrentStock->m_vDayLine.at(j)->GetRelativeStrong();
    }
    gl_ChinaStockMarket.m_pCurrentStock->m_vDayLine.at(i)->m_d5DayRS = dTempRS / 5;
  }

  for (int i = 10; i < lTotalNumber; i++) {
    dTempRS = 0;
    for (int j = i - 10; j < i; j++) {
      dTempRS += gl_ChinaStockMarket.m_pCurrentStock->m_vDayLine.at(j)->GetRelativeStrong();
    }
    gl_ChinaStockMarket.m_pCurrentStock->m_vDayLine.at(i)->m_d10DayRS = dTempRS / 10;
  }

  for (int i = 30; i < lTotalNumber; i++) {
    dTempRS = 0;
    for (int j = i - 30; j < i; j++) {
      dTempRS += gl_ChinaStockMarket.m_pCurrentStock->m_vDayLine.at(j)->GetRelativeStrong();
    }
    gl_ChinaStockMarket.m_pCurrentStock->m_vDayLine.at(i)->m_d30DayRS = dTempRS / 30;
  }

  for (int i = 60; i < lTotalNumber; i++) {
    dTempRS = 0;
    for (int j = i - 60; j < i; j++) {
      dTempRS += gl_ChinaStockMarket.m_pCurrentStock->m_vDayLine.at(j)->GetRelativeStrong();
    }
    gl_ChinaStockMarket.m_pCurrentStock->m_vDayLine.at(i)->m_d60DayRS = dTempRS / 60;
  }

  for (int i = 120; i < lTotalNumber; i++) {
    dTempRS = 0;
    for (int j = i - 120; j < i; j++) {
      dTempRS += gl_ChinaStockMarket.m_pCurrentStock->m_vDayLine.at(j)->GetRelativeStrong();
    }
    gl_ChinaStockMarket.m_pCurrentStock->m_vDayLine.at(i)->m_d120DayRS = dTempRS / 120;
  }

  gl_ChinaStockMarket.m_pCurrentStock->SetDayLineLoaded(true);

  return 7;
}
