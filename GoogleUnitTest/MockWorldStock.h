#pragma once
#include"WorldStock.h"

namespace testing {
  class CMockWorldStock : public CWorldStock {
  public:
    MOCK_METHOD(bool, UpdateEPSSurpriseDB, (), (override));
  };
  typedef shared_ptr<CMockWorldStock> CMockWorldStockPtr;
}