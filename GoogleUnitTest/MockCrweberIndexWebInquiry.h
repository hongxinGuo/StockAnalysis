#pragma once
#include"CrweberIndexWebInquiry.h"

namespace testing {
  class CMockCrweberIndexWebInquiry : public CCrweberIndexWebInquiry {
  public:
    MOCK_METHOD(void, StartReadingThread, (), (override));
    MOCK_METHOD(bool, ReadWebData, ());
  };
  typedef shared_ptr<CMockCrweberIndexWebInquiry> CMockCrweberIndexWebInquiryPtr;
}