///////////////////////////////////////////////////////////////////////////////////
//
// �����gl_lrelativeStrongEndDay��gl_lDay�����ǿ���̡߳�
//
// ���̵߳���ThreadCalculateRelativeStrongAtThisDayProc�̣߳����ͬʱ����16���̡߳�
//
//
///////////////////////////////////////////////////////////////////////////////////
#include"Market.h"
#include"Thread.h"

UINT ThreadCalculateDayLineRS(LPVOID startCalculatingDay) {
  gl_ThreadStatus.SetCalculatingDayLineRS(true);
  long lToday = (long)startCalculatingDay;

  const long year = lToday / 10000;
  const long month = lToday / 100 - year * 100;
  const long day = lToday - year * 10000 - month * 100;
  CTime ctCurrent(year, month, day, 12, 0, 0);
  const CTimeSpan oneDay(1, 0, 0, 0);

  if (lToday >= gl_systemTime.GetDay()) return(true);

  time_t tStart = 0, tEnd = 0;
  time(&tStart);
  do {
    if (gl_systemTime.IsWorkingDay(ctCurrent)) { // ���������������޽��ף��Թ�
      // ���ù����̣߳�ִ��ʵ�ʼ��㹤���� ���๤���̵߳����ȼ�Ϊ��ͣ��������Ա�ֻ֤����CPU�Ŀ���ʱ�䡣
      AfxBeginThread(ThreadCalculateThisDayRS, (LPVOID)lToday, THREAD_PRIORITY_LOWEST);
    }
    ctCurrent += oneDay;
    lToday = ctCurrent.GetYear() * 10000 + ctCurrent.GetMonth() * 100 + ctCurrent.GetDay();
  } while (lToday <= gl_systemTime.GetDay()); // ��������ǰ���ڣ��������գ�

  while (gl_ThreadStatus.IsCalculatingRS()) Sleep(1); // �ȴ����еĹ����߳̽���

  if (!gl_fExitingCalculatingRS) { // ���˳������˼�������
    gl_ChinaStockMarket.SetRelativeStrongEndDay(gl_systemTime.GetDay());
    gl_ChinaStockMarket.UpdateOptionDB(); // ����ѡ�����ݿ�
    // ��ʾ���ѵ�ʱ��
    time(&tEnd);
    const long tDiffer = tEnd - tStart;
    const long hour = tDiffer / 3600;
    const long min = tDiffer / 60 - hour * 60;
    const long second = tDiffer - hour * 3600 - min * 60;
    char buffer[100];
    sprintf_s(buffer, "�����Ʊ���ǿ����ʱ%02dСʱ%02d����%02d��", hour, min, second);
    CString str;
    str = buffer;
    gl_systemMessage.PushInformationMessage(str);
  }
  else {
    gl_fExitingCalculatingRS = false;// ����Ǽ��������ֹ�ˣ���������ֹ��ʶ��
    gl_systemMessage.PushInformationMessage(_T("��ֹ�����¼����������ǿ�ȵĹ���"));
  }
  gl_ThreadStatus.SetCalculatingDayLineRS(false); // ���߳�˳���˳������ڷ�����״̬

  return 11;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// ����������ڵ��������ǿ�ȡ�ʹ��C++11mutex��condition_variable�����Semaphore��
//
// pParam�� ���������ڣ������ͣ�
//
//
/////////////////////////////////////////////////////////////////////////////////////////
UINT ThreadCalculateThisDayRS(LPVOID thisDay) {
  long lToday;

  gl_ThreadStatus.IncreaseNunberOfCalculatingRSThreads();     // ���ڹ������߳�����һ
  gl_SemaphoreCalculateDayLineRS.Wait();
  if (!gl_ExitingSystem && !gl_fExitingCalculatingRS) {
    lToday = (long)thisDay;
    gl_ChinaStockMarket.CalculateOneDayRelativeStrong(lToday);  // ����ʵ��ִ�к���
  }
  gl_ThreadStatus.DecreaseNumberOfCalculatingRSThreads(); // ���ڹ������߳�����һ
  gl_SemaphoreCalculateDayLineRS.Signal();

  return 12;
}