#pragma once
#include"pch.h"

#include"NeteaseDayLineWebInquiry.h"

namespace testing {
  class CMockNeteaseDayLineWebInquiry : public CNeteaseDayLineWebInquiry {
  public:
    MOCK_METHOD(void, StartReadingThread, (), (override));
    MOCK_METHOD(bool, ReadWebDataTimeLimit, (long lFirstDelayTime, long lSecondDelayTime, long lThirdDelayTime));
  };
  typedef  shared_ptr<CMockNeteaseDayLineWebInquiry> CMockNeteaseDayLineWebInquiryPtr;
}
