#pragma once

#include "VirtualWebInquiry.h"
#include"WebRTData.h"

class CSinaRTWebInquiry : public CVirtualWebInquiry {
public:
  CSinaRTWebInquiry();
  virtual ~CSinaRTWebInquiry();

  virtual bool PrepareNextInquiringStr(void) override;
  virtual CString GetNextInquiringMiddleStr(long lTotalNumer = 900, bool fCheckActiveStock = false) override;
  virtual void StartReadingThread(void) override;
  virtual bool ReportStatus(long lNumberOfData) const override;
};

typedef shared_ptr<CSinaRTWebInquiry> CSinaRTWebInquiryPtr;
