#pragma once
#include "WebData.h"
#include"RTData.h"

class CSinaWebRTData final : public CWebData {
  friend class CRTData;
public:
  CSinaWebRTData();
  ~CSinaWebRTData();

  virtual bool GetWebData(void) override;

  virtual void InquireNextWebData(void) override;
  virtual int GetInquiringStr(CString& strInquire, long lTotalNumer = 900, bool fSkipUnactiveStock = true) override;
  virtual void StartReadingThread(void) override;

  virtual bool ReportStatus(long lNumberOfData) override;

private:
};