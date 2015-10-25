
//////////////////////////////////////////////////////////////////////////
// constant macro
#define OUT
#define STR_LEN                 260
#define STRING_16               16
#define STRING_32               32
#define STRING_64               64
#define STRING_128              128
#define STRING_256              256
#define STRING_512              512
#define STACK_BUFFER            4096

#define INPUT_BLOCK         0x01
#define OUTPUT_BLOCK        0x02

#define INT_REV(N)      (((N & 0x000000FF) << 24) | ((N & 0x0000FF00) << 8) | ((N & 0x00FF0000) >> 8) | ((N & 0xFF000000) >> 24))
#define SHORT_REV(N)    (((N & 0xFF00) >> 8) | ((N & 0x00FF) << 8))

#define READ_SHORT(p)   ((p[0] << 8) | p[1])
#define READ_INT(p)     ((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3])
#define READ_INT64(Value, Buffer) \
    Value |= Buffer[0]; \
    Value <<= 8; \
    Value |= Buffer[1]; \
    Value <<= 8; \
    Value |= Buffer[2]; \
    Value <<= 8; \
    Value |= Buffer[3]; \
    Value <<= 8; \
    Value |= Buffer[4]; \
    Value <<= 8; \
    Value |= Buffer[5]; \
    Value <<= 8; \
    Value |= Buffer[6]; \
    Value <<= 8; \
    Value |= Buffer[7];

#define WRITE_SHORT(p, N) p[0] = ((N >> 8) & 0xFF); p[1] = (N & 0xFF);
#define WRITE_INT(p, N) p[0] = ((N >> 24) & 0xFF); p[1] = ((N >> 16) & 0xFF); p[2] = ((N >> 8) & 0xFF); p[3] = (N & 0xFF);
#define WRITE_INT64(p, N) \
    p[0] = ((N >> 56) & 0xFF); \
    p[1] = ((N >> 48) & 0xFF); \
    p[2] = ((N >> 40) & 0xFF); \
    p[3] = ((N >> 32) & 0xFF); \
    p[4] = ((N >> 24) & 0xFF); \
    p[5] = ((N >> 16) & 0xFF); \
    p[6] = ((N >>  8) & 0xFF); \
    p[7] = (N & 0xFF);


#define ROUND_TIME(TimeVal) \
TimeVal.tv_sec += TimeVal.tv_usec / 1000000; \
TimeVal.tv_usec %= 1000000; \
if(0 > TimeVal.tv_usec) \
{ \
    TimeVal.tv_sec -= 1; \
    TimeVal.tv_usec += 1000000; \
}

//////////////////////////////////////////////////////////////////////////
// OS related objects
#ifdef WIN32    // Windows
#define WAIT_TIME                       5000

#define THREAD_ID                                           DWORD
#define THREAD_HANDLE                                       HANDLE
#define DECLARE_THREAD_START_ROUTINE                        static DWORD WINAPI
#define DEFINE_THREAD_START_ROUTINE                         DWORD WINAPI
#define CREATE_THREAD(handle, ThreadRoutine, arg)           handle = CreateThread(NULL, 0, ThreadRoutine, arg, 0, NULL)
#define WAIT_THREAD_FINISH(handle)                          WaitForSingleObject(handle, INFINITE)
#define GET_THREAD_ID(handle)                               GetThreadId(handle)
#define GET_CURRENT_THREAD_ID()                             GetCurrentThreadId()

#define DEFINE_LOCKER(mutex)    CRITICAL_SECTION mutex
#define INIT_LOCKER(mutex)      InitializeCriticalSection(&mutex)
#define UNINIT_LOCKER(mutex)    DeleteCriticalSection(&mutex)
#define MUTEX_LOCK(mutex)       EnterCriticalSection(&mutex)
#define MUTEX_UNLOCK(mutex)     LeaveCriticalSection(&mutex)

#define DEFINE_SIGNAL()         HANDLE 
#define INIT_SIGNAL()           CreateEvent()

#define SEM_DEFINE(sem)								HANDLE sem
#define SEM_INIT(sem, InitCount, MaxCount)			sem = CreateSemaphore(NULL, InitCount, MaxCount, NULL)
#define SEM_WAIT(sem)								WaitForSingleObject(sem, INFINITE)
#define SEM_TRYWAIT(sem, OK) \
    if(WAIT_TIMEOUT == WaitForSingleObject(sem, 0))\
        OK = 1;
#define SEM_RELEASE(sem, count)						ReleaseSemaphore(sem, count, NULL)
#define SEM_FINI(sem)								CloseHandle(sem)

//////////////////////////////////////////////////////////////////////////
#define LONG64BIT

#define R_OK                    0x04
#define W_OK                    0x02
#define O_DIRECT                0
#define IPPROTO_GRE             0x2F
typedef int pid_t;
#define PATH_SEPARATOR          '\\'

#define sleep(t)                        Sleep(t * 1000)
#define usleep(t)                       Sleep(t / 1000)
#define strncasecmp                     _strnicmp
#define strtoull(nptr, endptr, base)    _atoi64(nptr)
#define snprintf                        _snprintf
#define mkdir(pPath, nMode)             _mkdir(pPath)
int gettimeofday(struct timeval *tp, void *tzp);
struct tm *localtime_r(const time_t *timep, struct tm *result);

#define _prefetch0(p)       PreFetchCacheLine((void *)p, PF_TEMPORAL_LEVEL_1)
#define _prefetch1(p)       PreFetchCacheLine((void *)p, PF_TEMPORAL_LEVEL_2)
#define _prefetch2(p)       PreFetchCacheLine((void *)p, PF_TEMPORAL_LEVEL_3)

#else           // Linux

#define THREAD_ID                                           pthread_t
#define THREAD_HANDLE                                       pthread_t
#define DECLARE_THREAD_START_ROUTINE                        static void *
#define DEFINE_THREAD_START_ROUTINE                         void *
#define CREATE_THREAD(handle, ThreadRoutine, arg)           pthread_create(&handle, NULL, ThreadRoutine, arg)
#define WAIT_THREAD_FINISH(handle)                          pthread_join(handle, NULL)
#define GET_THREAD_ID(handle)                               handle
#define GET_CURRENT_THREAD_ID()                             pthread_self()

#define DEFINE_LOCKER(mutex)    pthread_mutex_t mutex
#define INIT_LOCKER(mutex)      pthread_mutex_init(&mutex, NULL)
#define UNINIT_LOCKER(mutex)    pthread_mutex_destroy(&mutex)
#define MUTEX_LOCK(mutex)       pthread_mutex_lock(&mutex)
#define MUTEX_UNLOCK(mutex)     pthread_mutex_unlock(&mutex)

#define SEM_DEFINE(sem)								sem_t sem
#define SEM_INIT(sem, InitCount, MaxCount)			sem_init(&sem, 0, InitCount)
#define SEM_WAIT(sem)								while(-1 == sem_wait(&sem))
#define SEM_TRYWAIT(sem, OK) \
    while(-1 == sem_trywait(&sem)) \
    { \
        if(EAGAIN == errno) \
        { \
            OK = 1; \
            break; \
        } \
    }

#define SEM_RELEASE(sem, count)						sem_post(&sem)
#define SEM_FINI(sem)								sem_destroy(&sem)

#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
    ((unsigned int)(unsigned char)(ch0) | ((unsigned int)(unsigned char)(ch1) << 8) |   \
    ((unsigned int)(unsigned char)(ch2) << 16) | ((unsigned int)(unsigned char)(ch3) << 24 ))

#define IN
#define OUT
#define MAX_PATH		        260
#define min(A, B)               (A < B ? A : B)
#define PATH_SEPARATOR          '/'

#endif

//////////////////////////////////////////////////////////////////////////
// public typedef
typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;


enum ThreadState
{
    THREAD_STOP = 0,
    THREAD_WAITING = 1,
    THREAD_RUNNING = 1,
    THREAD_JUMP = 2,
};

//////////////////////////////////////////////////////////////////////////
// Function declare

void PrintMem(const unsigned char * pBuffer, int nSize);


//int GetAlignedMem(OUT unsigned char *& pStart, int nSize);


int base64_decode(const char *bdata, char *buf, int buf_size);

typedef enum UnitType_en
{
    UNIT_TYPE_INT,  // int      =
    UNIT_TYPE_I64,  // long long
    UNIT_TYPE_STR,  // "hello"  strcpy
    UNIT_TYPE_PTR,  // *        =
    UNIT_TYPE_MEM,  // *        memcpy
    UNIT_TYPE_CLS,  // string   *((string *)pValue) =;
}UnitType_t;


typedef struct UnitInfo_s
{
    UnitType_t enType;
    void * pUnit;
    int nAttribute;
}UnitInfo_t;

typedef struct ParamInfo_s
{
    const char * pName;
    int nNameLen;

    UnitInfo_t stUnitInfo;
}ParamInfo_t;

#ifdef WIN32
//////////////////////////////////////////////////////////////////////////
// ERROR
#define ERROR_LOGICAL_PRINT(strFormat, ...)\
    printf(strFormat, __VA_ARGS__);

#define ERROR_OUTSIDE_PRINT(strFormat, ...)\
    printf(strFormat, __VA_ARGS__);

#define ERROR_RESOURCE_PRINT(strFormat, ...)\
    printf(strFormat, __VA_ARGS__);

#define ERROR_DATA_PRINT(strFormat, ...)\
    printf(strFormat, __VA_ARGS__);

#define ERROR_CONFIG_PRINT(strFormat, ...)\
    printf(strFormat, __VA_ARGS__);
//////////////////////////////////////////////////////////////////////////
// DEBUG
#ifdef DEBUG
#define DEBUG_PRINT(strFormat, ...)\
    (strFormat, __VA_ARGS__);
#else
#define DEBUG_PRINT(strFormat, ...)
#endif
//////////////////////////////////////////////////////////////////////////
// STAT
#define STATE_PRINT(strFormat, ...)\
    printf(strFormat, __VA_ARGS__);

#define TRACE_PRINT(strFormat, ...)\
    printf(strFormat, __VA_ARGS__);

#define LOG_PRINT(strFormat, ...)\
    printf(strFormat, __VA_ARGS__);
//////////////////////////////////////////////////////////////////////////
#else
//////////////////////////////////////////////////////////////////////////
// ERROR
#define ERROR_LOGICAL_PRINT(strFormat, args...)\
{\
    printf("\033[5;31m"strFormat"\033[0m", ##args);\
    fflush(stdout);\
}

#define ERROR_OUTSIDE_PRINT(strFormat, args...)\
{\
    printf("\033[5m"strFormat"\033[0m", ##args);\
    fflush(stdout);\
}

#define ERROR_RESOURCE_PRINT(strFormat, args...)\
    printf(strFormat, ##args);

#define ERROR_DATA_PRINT(strFormat, args...)\
    printf(strFormat, ##args);

#define ERROR_CONFIG_PRINT(strFormat, args...)\
{\
    printf("\033[5;33m"strFormat"\033[0m", ##args);\
    fflush(stdout);\
}

//////////////////////////////////////////////////////////////////////////
// DEBUG
#define DEBUG_PRINT(strFormat, args...)\
    printf(strFormat, ##args);

//////////////////////////////////////////////////////////////////////////
// STAT
#define  STATE_PRINT(strFormat, args...)\
    printf(strFormat, ##args);

#define TRACE_PRINT(strFormat, args...)\
    printf(strFormat, ##args);

#define LOG_PRINT(strFormat, args...)\
    printf(strFormat, ##args);

#endif

