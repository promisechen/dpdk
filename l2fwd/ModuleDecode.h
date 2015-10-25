
/*class CModuleDecode
{
public:
    int Init();
    int Fini();

    int Decode(Packet_t * pPacket);
};*/

// ETHERNET
#define ETH_PROTOCOL_IP_REV     0x0008
#define ETH_PROTOCOL_VLAN_REV   0x0081
#define ETH_PROTOCOL_IPV6_REV   0xDD86

#define VLAN_PROTOCOL_IP_REV

// IP
#ifndef IPPROTO_TCP          
#define IPPROTO_TCP          0x06
#endif
#ifndef IPPROTO_UDP           
#define IPPROTO_UDP          0x11
#endif

#ifndef  IPPROTO_GRE  
#define IPPROTO_GRE          0x2F

#endif
// TCP
#define TCP_PORT_FTP_DATA       0x0014
#define TCP_PORT_FTP_DATA_REV   0x1400
#define TCP_PORT_FTP_CTRL       0x0015
#define TCP_PORT_FTP_CTRL_REV   0x1500
#define TCP_PORT_SMTP           0x0019
#define TCP_PORT_SMTP_REV       0x1900
#define TCP_PORT_DNS            0x0035
#define TCP_PORT_DNS_REV        0x3500
#define TCP_PORT_HTTP           0x0050
#define TCP_PORT_HTTP_REV       0x5000
#define TCP_PORT_POP3           0x6E00
#define TCP_PROT_POP3_REV       0x006E
#define TCP_PORT_RTSP           0x022A
#define TCP_PORT_RTSP_REV       0x2A02
#define TCP_PORT_WAP_CSS        0x23F0
#define TCP_PORT_WAP_CSS_REV    0xF023
#define TCP_PORT_WAP_SS         0x23F1
#define TCP_PORT_WAP_SS_REV     0xF123
#define TCP_PORT_WAP_SCSS       0x23F2
#define TCP_PORT_WAP_SCSS_REV   0xF223
#define TCP_PORT_WAP_SSS        0x23F3
#define TCP_PORT_WAP_SSS_REV    0xF323

// UDP
#define UDP_PORT_DNS            0x0035
#define UDP_PORT_DNS_REV        0x3500
#define UDP_PORT_HTTP           0x0050
#define UDP_PORT_HTTP_REV       0x5000
#define UDP_PORT_USERLOG        0x178E
#define UDP_PORT_USERLOG_REV    0x8E17
#define UDP_PORT_FIFO           0x1283
#define UDP_PORT_FIFO_REV       0x8312
#define UDP_PORT_GTPV1_C        0x084B
#define UDP_PORT_GTPV1_C_REV    0x4B08
#define UDP_PORT_GTPV1_U        0x0868
#define UDP_PORT_GTPV1_U_REV    0x6808
#define UDP_PORT_WAP_CSS        0x23F0
#define UDP_PORT_WAP_CSS_REV    0xF023
#define UDP_PORT_WAP_SS         0x23F1
#define UDP_PORT_WAP_SS_REV     0xF123
#define UDP_PORT_WAP_SCSS       0x23F2
#define UDP_PORT_WAP_SCSS_REV   0xF223
#define UDP_PORT_WAP_SSS        0x23F3
#define UDP_PORT_WAP_SSS_REV    0xF323
#define UDP_PORT_CAPWAP         0x147F
#define UDP_PORT_CAPWAP_REV     0x7F14

// GTP
#define GTP_MESSAGE_GTPU        0xFF

// GPRS Network Service
#define NS_UNITDATA             0x00

// BSSGP
#define DL_UNITDATA             0x00
#define UL_UNITDATA             0x01
#define LLC_PDU                 0x0E

//////////////////////////////////////////////////////////////////////////

typedef struct EthernetHeader_s
{
    uint16 src_mac2;
    uint16 src_mac4;
    uint16 src_mac6;
    uint16 dst_mac2;
    uint16 dst_mac4;
    uint16 dst_mac6;
    uint16 protocol;
}EthernetHeader_t;

typedef struct VLanHeader_s
{
    uint16 usUnknown;
    uint16 usProtocol;
}VLanHeader_t;

typedef struct ip_header_s
{
    uint8 ihl :4;
    uint8 version :4;
    uint8 tos;
    uint16 tot_len;
    uint16 id;
    uint16 frag_off;
    uint8 ttl;
    uint8 protocol;
    uint16 check;
    uint32 saddr;
    uint32 daddr;
    /*The options start here. */
}ip_header_t;

typedef struct ip6_header_s
{
    uint8 TrafficeClass1 :4;
    uint8 Version :4;
    uint8 FlowLabel1 : 4;
    uint8 TrafficeClass2 :4;
    uint16 FlowLabel2;
    uint16 PayloadLength;
    uint8 NextHeader;
    uint8 HopLimit;
    uint8 saddr[16];
    uint8 daddr[16];
}ip6_header_t;

typedef struct tcp_header_s
{
    uint16 sport;
    uint16 dport;
    uint32 sn;
    uint32 an;
    uint16 other;
    uint16 window_size;
    uint16 check_sum;
    uint16 urgent_pointer;
    //u_int32_t option;
} tcp_header_t;

typedef struct udp_header_s
{
    uint16 sport;
    uint16 dport;
    uint16 len;
    uint16 crc;
} udp_header_t;

typedef struct gre_header_s
{
    uint16 usFlagVersion;
    uint16 usProtocolType;
    uint8 ucChannel;
    uint8 ucSequence;
    uint8 ucPacketVersion;
    uint8 ucTypeSource;
    uint8 ucDirection;
    uint16 usType;
}gre_header_t;

typedef struct gtp_header_s
{
    uint8 ucFlag;
    uint8 ucMessageType;
    uint16 usLength;
    uint32 unTEID;
}gtp_header_t;

//////////////////////////////////////////////////////////////////////////
#define COPY_IP(Dst, Src) \
    Dst.unIP4 = Src.unIP4;\
    if(0 == Src.unIP4)\
    memcpy(Dst.ucIP6, Src.ucIP6, sizeof(Dst.ucIP6));

#define IDENTICAL_IP(IPL, IPR) (((0 != IPL.unIP4) && (IPL.unIP4 == IPR.unIP4)) || ((0 == IPL.unIP4) && (0 == memcmp(IPL.ucIP6, IPR.ucIP6, sizeof(IPR.ucIP6)))))

#define PRINT_IP(Dst, Src, IPProto) \
    if(IPv4 == IPProto)\
    INT2IP(Dst, Src.unIP4);\
else\
    INT2IPv6(Dst, Src.ucIP6);

#define DECODE_IP(SrcIP, DstIP, Protocol, IPHeader) \
    if(0x40 == (*(IPHeader) & 0xF0))\
{\
    unsigned char * pSrc;\
    pSrc = (unsigned char *)(IPHeader) + (long)&(((ip_header_t *)NULL)->saddr);\
    (SrcIP).unIP4 = READ_INT(pSrc);\
    pSrc = (unsigned char *)(IPHeader) + (long)&(((ip_header_t *)NULL)->daddr);\
    (DstIP).unIP4 = READ_INT(pSrc);\
    pSrc = (unsigned char *)(IPHeader) + (long)&(((ip_header_t *)NULL)->protocol);\
    Protocol = *pSrc;\
}\
else\
{\
    unsigned char * pSrc;\
    pSrc = IPHeader + (long)&(((ip6_header_t *)NULL)->saddr);\
    memcpy(SrcIP.ucIP6, pSrc, sizeof(SrcIP.ucIP6));\
    pSrc = IPHeader + (long)&(((ip6_header_t *)NULL)->daddr);\
    memcpy(DstIP.ucIP6, pSrc, sizeof(DstIP.ucIP6));\
    pSrc = IPHeader + (long)&(((ip6_header_t *)NULL)->NextHeader);\
    Protocol = *pSrc;\
}

void * DecodeEth(Packet_t * pPacket);
