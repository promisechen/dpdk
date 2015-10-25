
#define PORT_NB         8
typedef enum PortType_s
{
    PortUnknown,
    PortOut,
    PortIn,
    PortEx,
    PortMax,
}PortType;

#define THREAD_NB       64
typedef enum ThreadType_s
{
    ThreadUnknown,
    ThreadLive,
    ThreadReplay,
    ThreadMax,
}ThreadType;

#define EX_PORT_NB          4
#define THREAD_RX_MAX       4
#define THREAD_TX_MAX       4
#define THREAD_EX_MAX       4
#define THREAD_SINK_MAX     8
#define THREAD_SPOT_MAX     8

#define SPOT_NAME_LEN   32

//////////////////////////////////////////////////////////////////////////
// userlog

typedef struct UserMsg_s
{
    void * pData;
}UserMsg_t;

typedef struct Userlog_s
{
    void * pData;
}Userlog_t;

//////////////////////////////////////////////////////////////////////////
// userdata

#define DPI_PATTERN_HIT     1
#define DPI_PATTERN_MISS    0

typedef enum Direction_s
{
    DIRECTION_OUT,
    DIRECTION_IN,
    DIRECTION_UNKNOWN,
    DIRECTION_MAX,
}Direction;

typedef enum PacketType_s
{
    PacketTypeUnknown,
    PacketTypePacket,
    PacketTypeRecord,
    PacketTypeUserlog,
}PacketType;  

#define PACKET_OFFLOAD_FLAG_IP_XSUM     0x01
#define PACKET_OFFLOAD_FLAG_TCP_XSUM     0x02
#define PACKET_OFFLOAD_FLAG_UDP_XSUM     0x04

typedef enum IPProtocol_s
{
    UNKNOWN,
    IPv4,
    IPv6,
}IPProtocol;

typedef union IPAddr_u
{
    unsigned int unIP4;
    unsigned char ucIP6[16];
}IPAddr_t;

typedef struct Packet_s
{
    void * pMBuf;
    // unsigned int unTimeSec;
    // unsigned int unTimeUSec;
    unsigned long long ullTimeMS;
    unsigned int unNicPortSrc;
    unsigned int unNicPortDst;
    unsigned int unNicPortGroup;

    Direction enDirection;
    PacketType enType;
    int nOffloadFlag;

    // IP header info
    IPProtocol enIPProtocol;
    IPAddr_t stIpSrc;
    IPAddr_t stIpDst;

    // TCP UDP header info
    unsigned int unCtrlProtocol;
    unsigned short usPortSrc;
    unsigned short usPortDst;
    unsigned int unTCPFin;

    // spot general info
    int nPatternHit;

    // packet data info
    unsigned int unLen;
    unsigned int unPayloadLen;
    unsigned char * pData;
    unsigned char * pCurrent;
    unsigned char * pEnd;
    unsigned char * pPayload;

    unsigned char * pTMAIP;
    unsigned char * pTMAGRE;
    unsigned char ucSeq;
    unsigned char ucChannel;

    unsigned char * pIPOuter;
    unsigned char * pUDPOuter;
    unsigned char * pIPInner;
    unsigned char * pTcpUdpInner;
    // unsigned char * pStart;
}Packet_t;

typedef struct Record_s
{
    void * pData;
}Record_t;

//////////////////////////////////////////////////////////////////////////
// stream data

typedef struct StreamKey_s
{
    int nVersion;
    IPAddr_t IPSrc;
    IPAddr_t IPDst;
    unsigned int unProtocol;
    unsigned short usPortSrc;
    unsigned short usPortDst;
}StreamKey_t;

typedef struct Stream_s
{
    StreamKey_t stStreamKey;
    // unsigned int unStartTimeSec;
    // unsigned int unStartTimeUSec;
    unsigned long long ullStartTimeMS;

    // unsigned int unTimeSec;
    // unsigned int unTimeUSec;
    unsigned long long ullTimeMS;

    Direction enKeyDirection;
    Direction enInitDirection;
    unsigned short usTCPFlag;

    unsigned int unLCoreID;
    unsigned int unPortGroup;
    unsigned int unPortOutID;
    unsigned int unPortInID;

    unsigned int unProtocolID;
    unsigned short usServiceID;
    unsigned short usServiceSubID;

    //////////////////////////////////////////////////////////////////////////
    unsigned short usPatternID;
    unsigned short usCarryID;
    //////////////////////////////////////////////////////////////////////////

    unsigned long long ullPacketTotal;
    unsigned long long ullPacketUp;
    unsigned long long ullPacketDn;
    unsigned long long ullByteUp;
    unsigned long long ullByteDn;
}Stream_t;

//////////////////////////////////////////////////////////////////////////
enum PacketTempType
{
    PacketTempType_HttpAnchor,
    PacketTempType_Reserve1,
    PacketTempType_Reserve2,
    PacketTempType_Reserve3,
    PacketTempType_MAX,
};

#define PacketTempSize      512

typedef struct Truck_s
{
    Stream_t * pStream;
    Userlog_t * pUserlog;
    void * pStreamPrivate;
    unsigned char ucPacketTempData[PacketTempType_MAX][PacketTempSize];
    int nCharacter;
}Truck_t;
#if 0
class IModuleThread
{
public:
    virtual ThreadType GetType() = 0;
    virtual int GetSinkNumber() = 0;
    virtual IModuleThread * GetSinkObj(int nIndex) = 0;
    virtual int GetLogID() = 0;
    virtual int GetPrivateMax() = 0;
    //////////////////////////////////////////////////////////////////////////
    virtual int InputUserlog(UserMsg_t * pUserMsg) = 0;
    virtual int InputPacket(Packet_t * pPacket) = 0;
    virtual int InputRecord(Record_t * pRecord) = 0;

    //////////////////////////////////////////////////////////////////////////
    virtual Packet_t * GetPacketBuf() = 0;
    virtual int SendPacket(Packet_t *) = 0;
    virtual int DropPacket(Packet_t *) = 0;

    virtual int StreamFin(Userlog_t * pUserlog, Stream_t * pStream) = 0;
    virtual int UserlogFin(Userlog_t * pUserlog) = 0;
};

enum Event_en
{
    EventSlience,
    EventUserlog,
    EventPacket,
    EventRecord,

    EventUserlogFin,
    EventStreamFin,
    EventMax
};

#define FLAG_EVENT_USERLOG          (1 << EventUserlog)
#define FLAG_EVENT_PACKET           (1 << EventPacket)
#define FLAG_EVENT_RECORD           (1 << EventRecord)
#define FLAG_EVENT_USERLOGFIN       (1 << EventUserlogFin)
#define FLAG_EVENT_STREAMFIN        (1 << EventStreamFin)

class IModuleSpot
{
public:
    virtual int Init(int nID, int unLCoreID, int nSocketID, IModuleThread * pThread) = 0;
    virtual int Fini() = 0;

    virtual int GetPrivateDataSize() = 0;
    virtual int GetInterest() = 0;

    virtual int ExecuteCmd(char * pReq, int nReqLen, char * pResp, int nRespLen) = 0;

#if 0
    virtual void * Process(int nType, void * pData, Truck_t * pTruck) = 0;
#else
    virtual UserMsg_t * ProcessUserlog(UserMsg_t * pUserMsg, Truck_t * pTruck) = 0;
    virtual Packet_t * ProcessPacket(Packet_t * pPacket, Truck_t * pTruck) = 0;
    virtual Record_t * ProcessRecord(Record_t * pRecord, Truck_t * pTruck) = 0;
    virtual int DoLaundry() = 0;
#endif
    virtual int StreamFin(Truck_t * pTruck) = 0;
    virtual int UserlogFin(Userlog_t * pUserlog) = 0;
};

// class ILog
// {
// public:
//     virtual int Add(char *ModuleName,int Level,char *cFormat,...) = 0;
//     virtual int SetTid(int tid) = 0;
// };
//////////////////////////////////////////////////////////////////////////
extern "C" int GetDeviceID();
extern "C" int GetPort(PortType enPortType, int nIndex);
extern "C" int GetThreadCount(ThreadType enThreadType);
extern "C" IModuleThread * GetThread(ThreadType enThreadType, int nID);
extern "C" void * AllocHugeMem(long long llSize, int nSocketID);
extern "C" int FreeHugeMem(void * p);
typedef int (* CmdActionFunc)(int Argc, char *Argv[], char **Buf, int *BufLen);
extern "C" int RegCmdNode(const char *ParentName, const char *Name, const char *Desc, CmdActionFunc SaveFun);
extern "C" int RegCmdFunc(const char *ParentName, const char *CmdSpec, const char *Desc, CmdActionFunc Fun);
extern "C" int WriteLog(int nID, char * pName, int nLogLevel, const char *cFormat, ...);

enum LogLevel
{
    LogLevelFatalError,
    LogLevelLogicalError,
    LogLevelDataError,
    LogLevelEnvError,
    LogLevelConfError,
    LogLevelEvent,
    LogLevelNormal,
};

#ifdef WIN32
#define RUBICON_WRITE_LOG(LogID, SpotName, LogLevel, LogStr, ...) \
    WriteLog(LogID, (char *)SpotName, LogLevel, (char *)LogStr, __VA_ARGS__);\
    if(LogLevel < LogLevelEvent)\
    {\
        printf("%s:" LogStr, SpotName, __VA_ARGS__);\
    }
#else
#define RUBICON_WRITE_LOG(LogID, SpotName, LogLevel, LogStr, ...) \
    WriteLog(LogID, (char *)SpotName, LogLevel, (char *)LogStr,## __VA_ARGS__);\
    if(LogLevel < LogLevelEvent)\
    {\
        printf(SpotName ":" LogStr,## __VA_ARGS__);\
    }
#endif

//////////////////////////////////////////////////////////////////////////
/*add HS_PRINT by clx 2015-9-24 */
#ifdef HS_PRINT
#define FOOTPRINT               printf("%s:%d %s\n", __FILE__, __LINE__, __FUNCTION__)
#define FOOTPRINT_ERROR         printf("\033[5;33m %s:%d %s error\033[0m\n", __FILE__, __LINE__, __FUNCTION__)
#else
#define FOOTPRINT
#define FOOTPRINT_ERROR
#endif
#define CHECK_POINTER(pvalue) \
if(NULL == pvalue)\
{\
    FOOTPRINT_ERROR;\
    return -1;\
}

#define CHECK_RETURN(value) \
if(0 > value)\
{\
    FOOTPRINT_ERROR;\
    return -1;\
}


//////////////////////////////////////////////////////////////////////////
class IModuleStat
{
public:
    virtual int Stat(unsigned char * pUserlog, unsigned char * pStream);
};
#endif
