#pragma once
#include"TiingoWebInquiry.h"

namespace testing {
  class CMockTiingoWebInquiry : public CTiingoWebInquiry {
  public:
    MOCK_METHOD(void, StartReadingThread, (), (override));
    MOCK_METHOD(bool, ReadWebData, ());
  };
  typedef  shared_ptr<CMockTiingoWebInquiry> CMockTiingoWebInquiryPtr;
}
