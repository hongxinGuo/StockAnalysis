#pragma once

#include"WebData.h"

class CCrweberIndexWebData final : public CWebData
{
public:
  CCrweberIndexWebData();
  ~CCrweberIndexWebData();

  virtual bool SucceedReadingAndStoringOneWebData(void) override;
  virtual void ProcessWebDataStored(void) override;
  virtual void ReportDataError(void) override;
  virtual void ReportCommunicationError(void) override;
  virtual void InquireNextWebData(void) override;
  virtual int GetInquiringStr(CString& strInquire) override;
  virtual void StartReadingThread(void) override;

  bool IsCreateOnce(void) noexcept { return sm_fCreatedOnce; }

private:
  double ConvertStringToTC(CString str);
  long ConvertStringToTime(CString str);
  double GetOneValue(void);
  CString GetNextString(void);

  static bool sm_fCreatedOnce;  // 此种类只允许生成一个实例
};
