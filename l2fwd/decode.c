#include <stdio.h>
#include <memory.h>
#ifdef WIN32
#include <Windows.h>
#else
#endif

#include "decode.h"


//////////////////////////////////////////////////////////////////////////

void * DecodeEth(Packet_t * pPacket);
void * DecodeVLan(Packet_t * pPacket);
void * DecodeIP(Packet_t * pPacket);
void * DecodeIPv6(Packet_t * pPacket);
void * DecodeGRE(Packet_t * pPacket);
void * DecodeTCP(Packet_t * pPacket);
void * DecodeUDP(Packet_t * pPacket);
void * DecodeGTP(Packet_t * pPacket);
void * DecodeCAPWAP(Packet_t * pPacket);
void * DecodeGB(Packet_t * pPacket);

void * DecodeEth(Packet_t * pPacket)
{
    EthernetHeader_t * pEthHeader = (EthernetHeader_t *)pPacket->pCurrent;
    if(DIRECTION_UNKNOWN == pPacket->enDirection)
    {
        if(pPacket->pCurrent[9] & 0x02)
            pPacket->enDirection = DIRECTION_IN;
        else
            pPacket->enDirection = DIRECTION_OUT;
    }

    pPacket->pCurrent += 14;

    switch(pEthHeader->protocol)
    {
    case ETH_PROTOCOL_IP_REV:
        return (void *)DecodeIP;
    case ETH_PROTOCOL_VLAN_REV:
        return (void *)DecodeVLan;
    case ETH_PROTOCOL_IPV6_REV:
        return (void *)DecodeIPv6;
    }

    return NULL;
}

void * DecodeVLan(Packet_t * pPacket)
{
    VLanHeader_t * pVLanHeader = (VLanHeader_t *)pPacket->pCurrent;
    pPacket->pCurrent += 4;

    switch(pVLanHeader->usProtocol)
    {
    case ETH_PROTOCOL_IP_REV:
        return (void *)DecodeIP;
    case ETH_PROTOCOL_IPV6_REV:
        return (void *)DecodeIPv6;
    }
    return NULL;
}

void * DecodeIP(Packet_t * pPacket)
{
    ip_header_t * pIPHeader = (ip_header_t *)pPacket->pCurrent;
    if(pIPHeader->ihl * 4 < 20)
        return NULL;
    if(NULL == pPacket->pTMAIP)
        pPacket->pTMAIP = pPacket->pCurrent;
    if(NULL == pPacket->pIPOuter)
        pPacket->pIPOuter = pPacket->pCurrent;
    pPacket->enIPProtocol = IPv4;
    pPacket->pIPInner = pPacket->pCurrent;
    pPacket->pCurrent += pIPHeader->ihl * 4;
    pPacket->pTcpUdpInner = NULL;

    // if(0 != (pIPHeader->frag_off & 0xFF1F))
    // {
    //     // ip fragment; User ip or network ip shoulb be descriminate
    //     pPacket->enType = PacketTypeUnknown;
    //     return NULL;
    // }

    switch(pIPHeader->protocol)
    {
    case IPPROTO_TCP:
        return (void *)DecodeTCP;
    case IPPROTO_UDP:
        return (void *)DecodeUDP;
    case IPPROTO_GRE:
        return (void *)DecodeGRE;
    }
    return NULL;
}

void * DecodeIPv6(Packet_t * pPacket)
{
    ip6_header_t * pIPHeader = (ip6_header_t *)pPacket->pCurrent;
    if(NULL == pPacket->pTMAIP)
        pPacket->pTMAIP = pPacket->pCurrent;
    if(NULL == pPacket->pIPOuter)
        pPacket->pIPOuter = pPacket->pCurrent;
    pPacket->enIPProtocol = IPv6;
    pPacket->pIPInner = pPacket->pCurrent;

    pPacket->pCurrent += 40;
    switch(pIPHeader->NextHeader)
    {
    case IPPROTO_TCP:
        return (void *)DecodeTCP;
    case IPPROTO_UDP:
        return (void *)DecodeUDP;
    case IPPROTO_GRE:
        return (void *)DecodeGRE;
    }

    return NULL;
}

void * DecodeGRE(Packet_t * pPacket)
{
#define GRE_FLAG_CHECKSUM       0x80
#define GRE_FLAG_ROUTE          0x40
#define GRE_FLAG_KEY            0x20
#define GRE_FLAG_SEQ            0x10
    if((NULL == pPacket->pTMAGRE) && (NULL == pPacket->pUDPOuter))
    {
        pPacket->pTMAGRE = pPacket->pCurrent;
        pPacket->pIPInner = NULL;
        pPacket->pIPOuter = NULL;
        //pPacket->pStart = pPacket->pCurrent;
    }
    int nHeadLength = 4;
    if(*pPacket->pCurrent & GRE_FLAG_ROUTE)
    {
        nHeadLength += 8;
    }
    else if(*pPacket->pCurrent & GRE_FLAG_CHECKSUM)
    {
        nHeadLength += 4;
    }

    if(*pPacket->pCurrent & GRE_FLAG_KEY)
    {
        if(0xFD == (pPacket->pCurrent + nHeadLength)[2])
        {
            pPacket->enType = PacketTypeRecord;
            nHeadLength += 4;
            pPacket->pCurrent += nHeadLength;
            return NULL;
        }
        else
        {
            pPacket->enType = PacketTypePacket;
            pPacket->ucSeq = (pPacket->pCurrent + nHeadLength)[0];
            pPacket->ucChannel = (pPacket->pCurrent + nHeadLength)[1] >> 4;
            if((pPacket->pCurrent + nHeadLength)[3] & 0x08)
                pPacket->enDirection = DIRECTION_IN;
            else
                pPacket->enDirection = DIRECTION_OUT;
            nHeadLength += 4;
        }
    }
    if(*pPacket->pCurrent & GRE_FLAG_SEQ)
    {
        nHeadLength += 4;
    }

    pPacket->pCurrent += nHeadLength;

    switch(((gre_header_t*)(pPacket->pTMAGRE))->usProtocolType)
    {
    case ETH_PROTOCOL_IP_REV:
        return (void *)DecodeIP;
    case ETH_PROTOCOL_VLAN_REV:
        return (void *)DecodeVLan;
    case ETH_PROTOCOL_IPV6_REV:
        return (void *)DecodeIPv6;
    }

    return NULL;
}

void * DecodeTCP(Packet_t * pPacket)
{
    tcp_header_t * pTCPHeader = (tcp_header_t *)pPacket->pCurrent;
    pPacket->pTcpUdpInner = pPacket->pCurrent;
    pPacket->pCurrent += (pTCPHeader->other & 0xF0) >> 2;

    return NULL;
}

void * DecodeUDP(Packet_t * pPacket)
{
    udp_header_t * pUDPHeader = (udp_header_t *)pPacket->pCurrent;
    pPacket->pTcpUdpInner = pPacket->pCurrent;
    pPacket->pCurrent += 8;

    if((NULL == pPacket->pTMAGRE) && (pPacket->pIPInner == pPacket->pIPOuter))
    {
        switch(pUDPHeader->dport)
        {
        case UDP_PORT_FIFO_REV:
            pPacket->enType = PacketTypeRecord;
            return NULL;
        case UDP_PORT_USERLOG_REV:
            pPacket->enType = PacketTypeUserlog;
            return NULL;
        default:
            break;
        }
    }

#ifdef ENV_GB
    if(pPacket->pIPInner == pPacket->pIPOuter)
        return (void *)DecodeGB;
#endif

    switch(pUDPHeader->sport)
    {
    case UDP_PORT_GTPV1_C_REV:
    case UDP_PORT_GTPV1_U_REV:
        if(pPacket->pIPInner == pPacket->pIPOuter)
            return (void *)DecodeGTP;
        break;
    case UDP_PORT_CAPWAP_REV:
        return (void *)DecodeCAPWAP;
    }
    switch(pUDPHeader->dport)
    {
    case UDP_PORT_GTPV1_C_REV:
    case UDP_PORT_GTPV1_U_REV:
        if(pPacket->pIPInner == pPacket->pIPOuter)
            return (void *)DecodeGTP;
        break;
    case UDP_PORT_CAPWAP_REV:
        return (void *)DecodeCAPWAP;
    }

    return NULL;
}

void * DecodeGTP(Packet_t * pPacket)
{
    pPacket->pIPInner = NULL;
    pPacket->pTcpUdpInner = NULL;

    if(GTP_MESSAGE_GTPU != pPacket->pCurrent[1])
    {
        pPacket->enType = PacketTypeUserlog;
        printf("outer gtp\n");
        return NULL;
    }
    if((*pPacket->pCurrent & 0x0F) == 0x00)
        pPacket->pCurrent += 8;
    else if((*pPacket->pCurrent & 0x0F) == 0x02)
        pPacket->pCurrent += 12;
    else
        return NULL;

    return (void *)DecodeIP;
}

void * DecodeCAPWAP(Packet_t * pPacket)
{
    // char cVersion = (*pPacket->pCurrent >> 4) & 0x0F;
    char cType = (*pPacket->pCurrent & 0x0F);
    if(0 == cType)
    {
        int nHeaderLen = (pPacket->pCurrent[1] & 0xF1) >> 1; //((pCurrent[1] >> 3) & 0x1F) * 4;
        pPacket->pCurrent += nHeaderLen;
    }
    else
        pPacket->pCurrent += 4;

    return (void *)DecodeEth;
}
