#pragma once

#include "VirtualWebInquiry.h"
#include"RTData.h"

class CSinaRTWebInquiry : public CVirtualWebInquiry {
public:
  CSinaRTWebInquiry();
  virtual ~CSinaRTWebInquiry();

  virtual bool PrepareNextInquiringStr(void) override;
  virtual CString GetNextInquiringMiddleStr(long lTotalNumer = 900, bool fSkipUnactiveStock = true) override;
  virtual void StartReadingThread(void) override;
  virtual bool ReportStatus(long lNumberOfData) override;

  virtual bool RunningThreadReadSinaRTData(void);
};

typedef shared_ptr<CSinaRTWebInquiry> CSinaRTWebInquiryPtr;
