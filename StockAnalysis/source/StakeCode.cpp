#include "StakeCode.h"

CStakeCode::CStakeCode() : CObject() {
  m_wMarket = 0;
  m_strStockCode = _T("");
  m_strStockName = _T("");
  m_lDayLineEndDate = 19900101;
  m_lDayLineStartDate = 19900101;
  m_lIPOStatus = __STAKE_NOT_CHECKED__;
  m_lOffsetInContainer = 0;
  m_nHand = 100;
}

CStakeCode::~CStakeCode(void) {
}

void CStakeCode::LoadActiveStakeCodeDB(CSetActiveStakeCode& setStakeCode) {
  m_wMarket = setStakeCode.m_StakeType;
  m_strStockCode = setStakeCode.m_StakeCode;
  CString str = setStakeCode.m_StakeName; // 用str中间过渡一下，就可以读取UniCode制式的m_StockName了。
  m_strStockName = str;
  m_lIPOStatus = setStakeCode.m_IPOStatus;
  m_lDayLineStartDate = setStakeCode.m_DayLineStartDate;
  if (m_lDayLineEndDate < setStakeCode.m_DayLineEndDate) { // 有时一个股票会有多个记录，以最后的日期为准。
    m_lDayLineEndDate = setStakeCode.m_DayLineEndDate;
  }
  m_lIPOStatus = setStakeCode.m_IPOStatus;
}

void CStakeCode::SaveActiveStakeCodeDB(CSetActiveStakeCode& setStakeCode) {
  ASSERT(setStakeCode.IsOpen());
  setStakeCode.AddNew();
  setStakeCode.m_StakeType = m_wMarket;
  setStakeCode.m_StakeCode = m_strStockCode;
  setStakeCode.m_StakeName = m_strStockName;
  setStakeCode.m_DayLineStartDate = m_lDayLineStartDate;
  setStakeCode.m_DayLineEndDate = m_lDayLineEndDate;
  setStakeCode.m_IPOStatus = m_lIPOStatus;
  setStakeCode.Update();
}