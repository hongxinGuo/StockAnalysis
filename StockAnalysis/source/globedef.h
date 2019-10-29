#ifndef __GLOBE_DEFINE__
#define __GLOBE_DEFINE__

#include"stdafx.h"

#include"RTData.h"

#include"SetDayLine.h"
#include"StockID.h"

#include"SinaRTWebData.h"
#include"TengxunRTWebData.h"
#include"NeteaseDayLineWebData.h"
#include"CrweberIndexWebData.h"

#include"CrweberIndex.h"

#include"SystemMessage.h"
#include"QueueRTData.h"
#include"ThreadStatus.h"
#include"SystemTime.h"

#include"CriticalSectionBool.h"

using namespace std;
#include<deque>
#include<vector>
#include<map>
#include<deque>
#include<memory>

extern CSinaRTWebData     gl_SinaRTWebData;     // 新浪实时数据采集
extern CTengxunRTWebData  gl_TengxunRTWebData;  // 腾讯实时数据采集
extern CNeteaseDayLineWebData gl_NeteaseDayLineWebData; // 网易日线历史数据
extern CNeteaseDayLineWebData gl_NeteaseDayLineWebDataSecond; // 网易日线历史数据
extern CNeteaseDayLineWebData gl_NeteaseDayLineWebDataThird; // 网易日线历史数据
extern CNeteaseDayLineWebData gl_NeteaseDayLineWebDataFourth; // 网易日线历史数据
extern CCrweberIndexWebData gl_CrweberIndexWebData;   // crweber.com上的每日油运指数

extern CCrweberIndex gl_CrweberIndex;

extern CSystemMessage     gl_systemMessage;     // 系统消息汇总类。此变量必须放在第一位，其他全局变量初始化时用到此变量。
extern CSystemTime        gl_systemTime;        // 系统时间汇总。
extern CQueueRTData       gl_QueueRTData;       // 系统实时数据队列。
extern CThreadStatus      gl_ThreadStatus;      // 系统中的各种状态，被各个工作线程所使用

extern deque<CString> gl_dequeRTStockInquire;   // 申请实时股票信息的股票队列
extern deque<CString> gl_dequeDayLineStockInquire; // 申请日线历史数据的股票队列。

extern CCriticalSectionBool gl_ExitingSystem; //  系统退出标识，用于终止其他线程。
extern bool gl_fExitingCalculatingRS;           // 用于通知工作线程退出的信号
extern bool gl_fResetSystem;                    // 系统重启标识

extern bool gl_fNormalMode;                     // 正常模式标识，默认为假。系统需要在启动时设置此标识，否则只有读取数据库的权利，无法添加和更改。
extern bool gl_fTestMode;                       // 是否设置了gl_fTestMode标识（用于判断是否在实际系统中使用了MySQLTest驱动）。

extern const int gl_cMaxSavingOneDayLineThreads;
// 信号量必须声明为全局变量（为了初始化）
extern CSemaphore gl_SaveOneStockDayLine;

class CMarket;
// 此市场变量必须位于全局变量的最后，因为其初始化需要其他全局变量的支持。
extern CMarket gl_ChinaStockMarket;        // 市场。所有活跃的股票皆位于其中，单一实例变量，仅允许存在一个实例。

#endif
