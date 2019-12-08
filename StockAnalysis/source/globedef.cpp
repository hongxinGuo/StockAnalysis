#include"globedef.h"

#include "Market.h"
#include "SystemTime.h"

// 测试用变量
CRTDataPtr gl_TESTpRTData = nullptr;

CSinaWebRTData gl_SinaWebRTData; // 新浪实时数据采集
CTengxunWebRTData gl_TengxunWebRTData; // 腾讯实时数据采集
CNeteaseWebRTData gl_NeteaseWebRTData; // 网易实时数据采集
CNeteaseWebDayLineData gl_NeteaseWebDayLineData; // 网易日线历史数据
CNeteaseWebDayLineData gl_NeteaseWebDayLineDataSecond; // 网易日线历史数据
CNeteaseWebDayLineData gl_NeteaseWebDayLineDataThird; // 网易日线历史数据
CNeteaseWebDayLineData gl_NeteaseWebDayLineDataFourth; // 网易日线历史数据
CNeteaseWebDayLineData gl_NeteaseWebDayLineDataFive; // 网易日线历史数据
CNeteaseWebDayLineData gl_NeteaseWebDayLineDataSix; // 网易日线历史数据
CCrweberIndexWebData gl_CrweberIndexWebData; // crweber.com上的每日油运指数

CCrweberIndex gl_CrweberIndex;
CCrweberIndex gl_CrweberIndexLast;

CSystemMessage gl_systemMessage; // 系统消息汇总类。此变量必须放在第一位，其他全局变量初始化时用到此变量。
CSystemTime gl_systemTime; // 系统时间汇总。

CPriorityQueueRTData gl_QueueSinaRTData; // 系统实时数据队列。
//CQueueRTData gl_QueueSinaRTDataForSave; // 用于存储的新浪实时数据队列
CPriorityQueueRTData gl_QueueTengxunRTData; // 系统实时数据队列。
CPriorityQueueRTData gl_QueueNeteaseRTData; // 系统实时数据队列。

CQueueWebRTData gl_QueueSinaWebRTData; // 新浪网络数据暂存队列
CQueueWebRTData gl_QueueTengxunWebRTData; // 腾讯网络数据暂存队列
CQueueWebRTData gl_QueueNeteaseWebRTData; // 网易网络数据暂存队列

CThreadStatus gl_ThreadStatus; // 系统中的各种状态，被各个工作线程所使用

atomic<bool> gl_ExitingSystem = false; //  系统退出标识，用于终止其他线程。
bool gl_fExitingCalculatingRS = false; // 用于通知工作线程退出的信号
bool gl_fResetSystem = false; // 系统重启标识

bool gl_fNormalMode = false; // 测试模式标识，默认为真。系统需要在启动时设置此标识，否则只有读取数据库的权利，无法添加和更改。
bool gl_fTestMode = true; // 是否设置了gl_fTestMode标识（用于判断是否在实际系统中使用了MySQLTest驱动）。

const int gl_cMaxSavingOneDayLineThreads = 3;
constexpr int gl_cMaxCalculateDayLineRS = 8;
// 信号量必须声明为全局变量（为了初始化）
Semaphore gl_SaveOneStockDayLine(4);
Semaphore gl_ProcessSinaRTDataQueue(1);   // 新浪实时数据处理同时只允许一个线程存在
Semaphore gl_ProcessTengxunRTDataQueue(1);
Semaphore gl_ProcessNeteaseRTDataQueue(1);

Semaphore gl_SemaphoreCalculateDayLineRS(8);

CMarket gl_ChinaStockMarket;  // 此市场实例必须位于全局变量的最后，因为其初始化需要其他全局变量的支持。