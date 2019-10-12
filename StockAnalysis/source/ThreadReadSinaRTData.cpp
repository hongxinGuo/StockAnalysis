/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ����ʵʱ����ӿڶ�ȡ�̡߳�
//
// ���˵ķ������ӳٲ�����100ms���ʶ��ڵȴ�100ms�󼴿ɿ�ʼ��ȡ���յ������ݡ�
//
// 
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include"Market.h"

UINT ThreadReadSinaRTData(LPVOID) {
  CInternetSession session;
  CHttpFile* pFile = nullptr;
  long iCount = 0;
  bool fDone = false;
  char* pChar = gl_stSinaRTDataInquire.buffer;

  const clock_t tt = clock();

  try {
    gl_ThreadStatus.SetReadingSinaRTData(true);  //
    gl_stSinaRTDataInquire.fError = false;
    gl_stSinaRTDataInquire.lByteRead = 0;
    pFile = dynamic_cast<CHttpFile*>(session.OpenURL((LPCTSTR)gl_stSinaRTDataInquire.strInquire));
    Sleep(100); // ���˷�����100ms�ӳټ��ɡ�
    while (!fDone) {
      do {
        iCount = pFile->Read(pChar, 1024);
        if (iCount > 0) {
          pChar += iCount;
          gl_stSinaRTDataInquire.lByteRead += iCount;
        }
      } while (iCount > 0);
      Sleep(30); // �ȴ�30������ٶ�һ�Σ�ȷ��û�������ݺ�ŷ��ء�
      iCount = pFile->Read(pChar, 1024);
      if (iCount > 0) {
        pChar += iCount;
        gl_stSinaRTDataInquire.lByteRead += iCount;
      }
      else fDone = true;
    }
    gl_stSinaRTDataInquire.buffer[gl_stSinaRTDataInquire.lByteRead] = 0x000;
    gl_ThreadStatus.SetSinaRTDataReceived(true);
  }
  catch (CInternetException * e) {
    e->Delete();
    gl_stSinaRTDataInquire.fError = true;
    gl_ThreadStatus.SetSinaRTDataReceived(false);
  }
  if (pFile) pFile->Close();
  if (pFile) delete pFile;
  gl_ThreadStatus.SetReadingSinaRTData(false);

  gl_ChinaStockMarket.SetReadingSinaRTDataTime(clock() - tt);

  return 1;
}