//////////////////////////////////////////////////////////////////////////////////////
//
// �洢������ʷ���ݿ�ʱ�����ƻ���ͬ�����⣬���µ����ݿ��д��ھ�����ʱ�����µ���;�ͻᷢ�����ݿ�ͬ�����Ļ��⣬
// �����Ҷ�MySQL���ݿⲻ̫��Ϥ���޷��ҵ�����������ʶ����������ź������ֱ������������ݿ�Ϊ�գ�ϵͳ����)ʱ,
// ʹ�ö��̷߳�ʽ�洢���ݣ��������ݿ��д��ھ�����ʱ��ʹ�õ�һ�̴߳洢��
// ���ַ�ʽ����Ӱ���ٶȣ���Ϊ��������ʱ���������Ǻ�С�ġ�
//
// ���յĽ������������Ҫ�о��������ͬ�����⡣
//
//
/////////////////////////////////////////////////////////////////////////////////////////
#include"globedef.h"

UINT ThreadSaveDayLineBasicInfoOfStock(not_null<CChinaStock*> pStake) {
  CString str;
  bool fDataSaved = false;
  gl_ThreadStatus.IncreaseRunningThread();

  gl_ThreadStatus.IncreaseSavingDayLineThreads();
  gl_SaveOneStockDayLine.Wait(); //ʹ�ö��߳�ģʽ����������ȫ����ʷ����ʱʹ��4���̣߳�������ʷ����ʱֻʹ��һ���̣߳���ʱʹ�ö���̷߳��������ֻ�����󣩡�
  if (!gl_fExitingSystem) {
    fDataSaved = pStake->SaveDayLineBasicInfo();
    pStake->UpdateDayLineStartEndDate();
    if (fDataSaved) {
      pStake->SetDayLineDBUpdated(true);
      pStake->SetUpdateStockProfileDB(true);
    }
    pStake->UnloadDayLine();
    if (fDataSaved) {
      str = pStake->GetStockCode() + _T("�������ϴ洢���");
      gl_systemMessage.PushDayLineInfoMessage(str);
    }
  }
  gl_SaveOneStockDayLine.Signal();
  gl_ThreadStatus.DecreaseSavingDayLineThreads();
  gl_ThreadStatus.DecreaseRunningThread();

  return 15;
}