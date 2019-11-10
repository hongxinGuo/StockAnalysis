#pragma once
#include "WebData.h"

class CSinaRTWebData final : public CWebData {
public:
  CSinaRTWebData();
  ~CSinaRTWebData();

  virtual bool SucceedReadingAndStoringOneWebData(void) override;
  virtual void ProcessWebDataStored(void) override;
  virtual void ReportDataError(void) override;
  virtual void ReportCommunicationError(void) override;
  virtual void InquireNextWebData(void) override;
  virtual int GetInquiringStr(CString& strInquire, long lTotalNumer = 900, bool fSkipUnactiveStock = true) override;
  virtual void StartReadingThread(void) override;
  virtual bool ReportStatus(long lNumberOfData) override;

  bool IsCreateOnce(void) noexcept { return sm_fCreatedOnce; }

private:
  static bool sm_fCreatedOnce;  // 此种类只允许生成一个实例
};
