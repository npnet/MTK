
/****************************************************************************
*
* Copyright (c) 2016 Wi-Fi Alliance
*
* Permission to use, copy, modify, and/or distribute this software for any
* purpose with or without fee is hereby granted, provided that the above
* copyright notice and this permission notice appear in all copies.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
* WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
* SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
* RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
* NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE
* USE OR PERFORMANCE OF THIS SOFTWARE.
*
*****************************************************************************/

/*
 *    File: wfa_tg.c
 *    Library functions for traffic generator.
 *    They are shared with both TC and DUT agent.
 */
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#ifndef _FREERTOS
#include <netdb.h>
#include <signal.h>
#include <pthread.h>
#endif

#include <math.h>

#include "wfa_portall.h"
#include "wfa_stdincs.h"
#include "wfa_debug.h"
#include "wfa_ver.h"
#include "wfa_main.h"
#include "wfa_tlv.h"
#include "wfa_tg.h"
#include "wfa_cmds.h"
#include "wfa_sock.h"
#include "wfa_rsp.h"
#include "wfa_wmmps.h"
#include "wfa_miscs.h"
#ifdef _FREERTOS
/* FreeRTOS Perf Improve */
#include "memory_attribute.h"
#endif

extern tgStream_t gStreams[];
extern BOOL gtgRecv;
extern BOOL gtgSend;
extern BOOL gtgTransac;
extern int gtimeOut;
extern int gRegSec;
extern BOOL gtgCaliRTD;

int btSockfd = -1;
int adj_latency;

extern tgStream_t *findStreamProfile(int);
extern int wfaTrafficSendTo(int, char *, int, struct sockaddr *);
extern int wfaTrafficRecv(int, char *, struct sockaddr *, int);
extern void wfaSendPing(tgPingStart_t *staPing, float *interval, int streamid);
extern int wfaStopPing(dutCmdResponse_t *stpResp, int streamid);
extern unsigned short wfa_defined_debug;
extern int tgSockfds[];

extern tgWMM_t wmm_thr[];

extern double min_rttime;
extern double gtgPktRTDelay;
extern void int2BuffBigEndian(int val, char *buf);
extern int bigEndianBuff2Int(char *buff);

#ifdef WFA_WMM_PS_EXT
extern int gtgWmmPS;
extern wfaWmmPS_t wmmps_info;
extern tgWMM_t wmmps_mutex_info;

extern int psSockfd;
extern unsigned int psTxMsg[];
extern unsigned int psRxMsg[];
extern int gtgPsPktRecvd;

extern void wfaSetDUTPwrMgmt(int mode);
void wmmps_wait_state_proc();
extern void wfaWmmpsInitFlag();

#endif



static int streamId = 0;
static int totalTranPkts = 0, sentTranPkts = 0;
int slotCnt = 0;

extern int usedThread;
extern int runLoop;
extern int sendThrId;

char e2eResults[124];
#if 0  /* for test purpose only */
char *e2eResults = "/tmp/e2e1198798626.txt";
#endif


extern dutCmdResponse_t gGenericResp;
static int  tableDscpToTos[15] [2] = {{0,0},{8,32},{10,40},{14,56},{18,72},{22,88},{24,96},{28,112},{34,136},{36,144},{38,152},{40,160},{46,184},{48,192},{56,224}};


/* Some devices may only support UDP ECHO and do not have ICMP level ping */
// #define WFA_PING_UDP_ECHO_ONLY     1

#ifdef CONFIG_MTK_AP
extern char *getApMainInterface();
#endif

/*
 * findStreamProfile(): search existing stream profile by stream id
 * input: id - stream id;
 * return: matched stream profile
 */
#ifdef _FREERTOS
ATTR_TEXT_IN_SYSRAM
#endif
tgStream_t *findStreamProfile(int id)
{
    int i;
    tgStream_t *myStream = gStreams;

    for(i = 0; i< WFA_MAX_TRAFFIC_STREAMS; i++)
    {
        if(myStream->id == id)
            return myStream;

        myStream++;
    }

    return NULL;
}

tgProfile_t *findTGProfile(int streamId)
{
    volatile int i;
    tgStream_t *myStream = gStreams;

    for(i = 0; i< WFA_MAX_TRAFFIC_STREAMS; i++)
    {
       if(myStream->id == streamId)
          return &(myStream->profile);

       myStream++;
    }

    return NULL;
}

void wfaSleepMilsec(int milSec)
{
    if (milSec >0)
        usleep(milSec * 1000);
}

int convertDscpToTos(int dscp)// return >=0 as TOS, otherwise error.
{
    int i =0;

    for (i=0; i< WFA_DSCP_TABLE_SIZE; i++)
    {
        if ( tableDscpToTos[i][0] == dscp)
            return tableDscpToTos[i][1];
    }
    return  -1;
}
/*
 * wfaTGSendPing(): Instruct Traffic Generator to send ping packets
 *
 */
int wfaTGSendPing(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    int streamid = ++streamId;
    float interval;      /* it could be subseconds/100s minisecond */
    tgPingStart_t *staPing = (tgPingStart_t *)caCmdBuf;
    dutCmdResponse_t *spresp = &gGenericResp;

#ifdef WFA_PING_UDP_ECHO_ONLY
    tgStream_t *myStream = NULL;
#endif

    DPRINT_INFO(WFA_OUT, "\nEntering wfaTGSendPing ...\n");
    if(staPing->frameSize == 0)
         staPing->frameSize = 100;


    if(staPing->frameRate == 0)
         staPing->frameRate = 1;

    interval = (float) 1/staPing->frameRate;


    if(staPing->duration == 0)
         staPing->duration = 30;
    printf("framerate %f interval %f streamID %d duration %d\n",
            staPing->frameRate, interval, streamId,staPing->duration);

    switch(staPing->type)
    {
    case WFA_PING_ICMP_ECHO:
#ifndef WFA_PING_UDP_ECHO_ONLY

        wfaSendPing(staPing, &interval, streamId);

        spresp->status = STATUS_COMPLETE;
        spresp->streamId = streamid;
#else
        printf("Only support UDP ECHO\n");
#endif
        break;

    case WFA_PING_UDP_ECHO:
    {
#ifdef WFA_PING_UDP_ECHO_ONLY
        /*
         * Make this like a transaction testing
         * Then make it a profile and run it
         */
        myStream = &gStreams[slotCnt++];
        memset(myStream, 0, sizeof(tgStream_t));
        memcpy(&myStream->profile, caCmdBuf, len);
        myStream->id = streamid; /* the id start from 1 */
        myStream->tblidx = slotCnt-1;

        btSockfd = wfaCreateUDPSock("127.0.0.1", WFA_UDP_ECHO_PORT);
        if((btSockfd = wfaConnectUDPPeer(btSockfd, staPing->dipaddr, WFA_UDP_ECHO_PORT)) > 0)
        {
            gtgTransac = streamid;
            gtgSend = streamid;
            totalTranPkts = 512;
            sentTranPkts = 0;

            /*
             * the framerate here is used to derive the timeout
             * value for waiting transaction echo responses.
             */
            gtimeOut = MINISECONDS/staPing->frameRate;  /* in msec */

            /* set to longest time */
            if(staPing->duration == 0)
                staPing->duration = 3600;
        }
#else
        printf("Doesn't support UDP Echo\n");
#endif
        break;
    }
    default:
    {
        spresp->status = STATUS_INVALID;
        spresp->streamId = streamid;
    }
    }


    wfaEncodeTLV(WFA_TRAFFIC_SEND_PING_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)spresp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);

    return WFA_SUCCESS;
}

/*
 * tgStopPing(): Instruct Traffic Generator to stop ping packets
 *
 */
int wfaTGStopPing(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    int streamid = *(int *)(caCmdBuf);
    dutCmdResponse_t *stpResp = &gGenericResp;
    tgStream_t *myStream;
    int i;

    stpResp->status = STATUS_COMPLETE;

    printf("CS: The length %d and the command buff is \n",len);

    for (i=0; i<8; i++)
        printf(" %x ",caCmdBuf[i]);

    printf("\nthe stream id is %d\n",streamid);

    if( gtgTransac == streamid&&gtgSend == streamid)
    {
        gtgTransac =0;
        gtgSend = 0;
        gtgRecv = 0;
        wALARM(0);

        myStream = findStreamProfile(streamid);
        if(myStream == NULL)
        {
            stpResp->status = STATUS_INVALID;
        }

        stpResp->cmdru.pingStp.sendCnt = myStream->stats.txFrames;
        stpResp->cmdru.pingStp.repliedCnt = myStream->stats.rxFrames;
    }
    else
    {
#if 0
        if(wfaStopPing(stpResp, streamid)== WFA_FAILURE)
        {
            stpResp->status = STATUS_COMPLETE;

            wfaEncodeTLV(WFA_TRAFFIC_STOP_PING_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)stpResp, respBuf);
            *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);
        }
#endif
        wfaStopPing(stpResp, streamid);
    }

    wfaEncodeTLV(WFA_TRAFFIC_STOP_PING_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)stpResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);

    return WFA_SUCCESS;
}

/*
 * wfaTGConfig: store the traffic profile setting that will be used to
 *           instruct traffic generation.
 * input: cmd -- not used
 * response: send success back to controller
 * return: success or fail
 * Note: the profile storage is a global space.
 */
int wfaTGConfig(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    int ret = WFA_FAILURE;
    tgStream_t *myStream = NULL;
    dutCmdResponse_t *confResp = &gGenericResp;

    /* if the stream table over maximum, reset it */
    if(slotCnt == WFA_MAX_TRAFFIC_STREAMS)
        slotCnt = 0;

    if(slotCnt == 0)
    {
        printf("resetting stream table\n");
        wMEMSET(gStreams, 0, WFA_MAX_TRAFFIC_STREAMS*sizeof(tgStream_t));
    }

    DPRINT_INFO(WFA_OUT, "entering tcConfig ...\n");
    myStream = &gStreams[slotCnt++];
    wMEMSET(myStream, 0, sizeof(tgStream_t));
    wMEMCPY(&myStream->profile, caCmdBuf, len);
    myStream->id = ++streamId; /* the id start from 1 */
    myStream->tblidx = slotCnt-1;

#ifdef CONFIG_MTK_COMMON
    DPRINT_INFO(WFA_OUT, "profile %i direction %i dest ip %s dport %i source %s sport %i rate %i duration %i size %i class %i delay %i\n", myStream->profile.profile, myStream->profile.direction, myStream->profile.dipaddr, myStream->profile.dport, myStream->profile.sipaddr, myStream->profile.sport, myStream->profile.rate, myStream->profile.duration, myStream->profile.pksize, myStream->profile.trafficClass, myStream->profile.startdelay);
#endif

#ifdef CONFIG_MTK_AP
    dump_wmm_paraters();
#endif

    confResp->status = STATUS_COMPLETE;
    confResp->streamId = myStream->id;
    wfaEncodeTLV(WFA_TRAFFIC_AGENT_CONFIG_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)confResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);


    return ret;
}

/* RecvStart: instruct traffic generator to start receiving
 *                 based on a profile
 * input:      cmd -- not used
 * response:   inform controller for "running"
 * return:     success or failed
 */
int wfaTGRecvStart(int len, BYTE *parms, int *respLen, BYTE *respBuf)
{
    int status = STATUS_COMPLETE, i;
    int numStreams = len/4;
    int streamid;
    tgProfile_t *theProfile;
    tgStream_t *myStream;

    DPRINT_INFO(WFA_OUT, "entering tgRecvStart\n");

    /*
     * The function wfaSetProcPriority called here is to enhance the real-time
     * performance for packet receiving. It is only for tuning and optional
     * to implement
     */

    for(i=0; i<numStreams; i++)
    {
        wMEMCPY(&streamid, parms+(4*i), 4); /* changed from 2 to 4, bug reported by n.ojanen */
        myStream = findStreamProfile(streamid);
        if(myStream == NULL)
        {
            status = STATUS_INVALID;
            return status;
        }

        theProfile = &myStream->profile;
        if(theProfile == NULL)
        {
            status = STATUS_INVALID;
            return status;
        }

        /* calculate the frame interval which is used to derive its jitter */
        if(theProfile->rate != 0 && theProfile->rate < 5000)
            myStream->fmInterval = 1000000/theProfile->rate; /* in ms */
        else
            myStream->fmInterval = 0;

        if(theProfile->direction != DIRECT_RECV)
        {
            status = STATUS_INVALID;
            return status;
        }

        wMEMSET(&myStream->stats, 0, sizeof(tgStats_t));

        // mark the stream active
        myStream->state = WFA_STREAM_ACTIVE;

        switch(theProfile->profile)
        {
#ifdef WFA_WPA2_SINGLE_THREAD
        case PROF_MCAST:
        case PROF_FILE_TX:
            btSockfd = wfaCreateUDPSock(theProfile->dipaddr, theProfile->dport);
            gtgRecv = streamid;

            if(btSockfd < 0)
                status = STATUS_ERROR;
            else
            {
                /* get current flags setting */
                int ioflags = wFCNTL(btSockfd, F_GETFL, 0);

                /* set only BLOCKING flag to non-blocking */
                wFCNTL(btSockfd, F_SETFL, ioflags | O_NONBLOCK);
            }
            break;
#else

        case PROF_TRANSC:
        case PROF_CALI_RTD:  /* Calibrate roundtrip delay */
            gtgTransac = streamid;
        case PROF_MCAST:
        case PROF_FILE_TX:
        case PROF_IPTV:
            gtgRecv = streamid;
            wmm_thr[usedThread].thr_flag = streamid;
#ifndef _FREERTOS
            wPT_MUTEX_LOCK(&wmm_thr[usedThread].thr_flag_mutex);
            wPT_COND_SIGNAL(&wmm_thr[usedThread].thr_flag_cond);
            wPT_MUTEX_UNLOCK(&wmm_thr[usedThread].thr_flag_mutex);
#else
            xEventGroupSetBits(wmm_thr[usedThread].thr_flag_wait, WFA_TG_BIT);
#endif
            printf("Recv Start in thread %i for streamid %i\n", usedThread, streamid);
            usedThread++;
            break;
#endif
        case PROF_UAPSD:
#ifdef WFA_WMM_PS_EXT
#if 0
            status = STATUS_COMPLETE;
            psSockfd = wfaCreateUDPSock(theProfile->dipaddr, WFA_WMMPS_UDP_PORT);

            wmmps_info.sta_state = 0;
            wmmps_info.wait_state = WFA_WAIT_STAUT_00;

            wMEMSET(&wmmps_info.psToAddr, 0, sizeof(wmmps_info.psToAddr));
            wmmps_info.psToAddr.sin_family = AF_INET;
            wmmps_info.psToAddr.sin_addr.s_addr = inet_addr(theProfile->sipaddr);
            wmmps_info.psToAddr.sin_port = htons(theProfile->sport);
            wmmps_info.reset = 0;

            wmm_thr[usedThread].thr_flag = streamid;
            wmmps_info.streamid = streamid;
            wPT_MUTEX_LOCK(&wmm_thr[usedThread].thr_flag_mutex);
            wPT_COND_SIGNAL(&wmm_thr[usedThread].thr_flag_cond);
            gtgWmmPS = streamid;;
            wPT_MUTEX_UNLOCK(&wmm_thr[usedThread].thr_flag_mutex);

            DPRINT_INFO(WFA_OUT, "wfaTGRecvStart, WMMPS rcv start, streamId=%d, usedThread=%d\n", streamid, usedThread);
            usedThread++;
#endif // remove for now.
            status = STATUS_COMPLETE;
            wfaWmmpsInitFlag();
            theProfile->trafficClass = 0;  // init to no traffic Class set
            // from STA point view, in the WMMPS, source addr is PCEnd also as dest address to send to
            strcpy(theProfile->dipaddr, theProfile->sipaddr);

            memset(&wmmps_info.psToAddr, 0, sizeof(wmmps_info.psToAddr));
            wmmps_info.psToAddr.sin_family = AF_INET;
            wmmps_info.psToAddr.sin_addr.s_addr = inet_addr(theProfile->sipaddr);
            wmmps_info.psToAddr.sin_port = htons(theProfile->sport);
            wmmps_info.streamid = streamid;

            wmmps_mutex_info.thr_flag = streamid;
            gtgWmmPS = streamid;
#ifndef _FREERTOS
            pthread_cond_signal(&wmmps_mutex_info.thr_flag_cond);//Aaron's//Wake up the wfa_wmmps_thread
#else
            /*ToDo : Replace Thread */
#endif
            DPRINT_INFO(WFA_OUT, "wfaTGRecvStart PROF_UAPSD srcIPAddr=%s desIPAddr=%s streamId=%d\n",
                        theProfile->sipaddr, theProfile->dipaddr, wmmps_info.streamid );
            gtimeOut = MINISECONDS/10;  /* in msec */

#endif   /* WFA_WMM_PS_EXT */
            break;
        }
    }

    /* encode a TLV for response for "complete/error ..." */
    wfaEncodeTLV(WFA_TRAFFIC_AGENT_RECV_START_RESP_TLV, sizeof(int),
                 (BYTE *)&status, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(int);

    return WFA_SUCCESS;
}

/*
 * tgRecvStop: instruct traffic generator to stop receiving based on a profile
 * input:      cmd -- not used
 * response:   inform controller for "complete"
 * return:     success or failed
 */
int wfaTGRecvStop(int len, BYTE *parms, int *respLen, BYTE *respBuf)
{
    int status = STATUS_COMPLETE, i;
    int numStreams = len/4;
    unsigned int streamid;
    tgProfile_t *theProfile;
    tgStream_t *myStream=NULL;
#ifdef CONFIG_MTK_COMMON
    dutTgResponse_t statResp;
#else
    dutCmdResponse_t statResp;
#endif
    BYTE dutRspBuf[WFA_RESP_BUF_SZ];
    int id_cnt = 0;
    int resp_size = sizeof(statResp);

    DPRINT_INFO(WFA_OUT, "entering tgRecvStop with length %d\n",len);

    /* in case that send-stream not done yet, an optional delay */
    while(sendThrId != 0)
        wSLEEP(1);

    /*
     * After finishing the receiving command, it should lower itself back to
     * normal level. It is optional implementation if it is not called
     * while it starts receiving for raising priority level.
     */
    wMEMSET(dutRspBuf, 0, WFA_RESP_BUF_SZ);
    for(i=0; i<numStreams; i++)
    {
        wMEMCPY(&streamid, parms+(4*i), 4);
        printf(" stop stream id %i\n", streamid);
        myStream = findStreamProfile(streamid);
        if(myStream == NULL)
        {
            status = STATUS_INVALID;
            wfaEncodeTLV(WFA_TRAFFIC_AGENT_RECV_STOP_RESP_TLV, 4, (BYTE *)&status, respBuf);
            *respLen = WFA_TLV_HDR_LEN + 4;
            printf("stream table empty\n");
            continue;
        }

        theProfile = &myStream->profile;
        if(theProfile == NULL)
        {
            status = STATUS_INVALID;
            wfaEncodeTLV(WFA_TRAFFIC_AGENT_RECV_STOP_RESP_TLV, 4, (BYTE *)&status, respBuf);
            *respLen = WFA_TLV_HDR_LEN + 4;

            return WFA_SUCCESS;
        }

        if(theProfile->direction != DIRECT_RECV)
        {
            status = STATUS_INVALID;
            wfaEncodeTLV(WFA_TRAFFIC_AGENT_RECV_STOP_RESP_TLV, 4, (BYTE *)&status, respBuf);
            *respLen = WFA_TLV_HDR_LEN + 4;

            return WFA_SUCCESS;
        }

        /* reset its flags , close sockets */
        switch(theProfile->profile)
        {
        case PROF_TRANSC:
        case PROF_CALI_RTD:
            gtgTransac = 0;
        case PROF_MCAST:
        case PROF_FILE_TX:
        case PROF_IPTV:
            gtgRecv = 0;
            if(tgSockfds[myStream->tblidx] != -1)
            {
                wCLOSE(tgSockfds[myStream->tblidx]);
                tgSockfds[myStream->tblidx] = -1;
            }
            break;

        case PROF_UAPSD:
#ifdef WFA_WMM_PS_EXT
#if 0
            gtgWmmPS = 0;
            gtgPsPktRecvd = 0;

            if(psSockfd != -1)
            {
                wCLOSE(psSockfd);
                psSockfd = -1;
            }

            wMEMSET(&wmmps_info, 0, sizeof(wfaWmmPS_t));

            wfaSetDUTPwrMgmt(PS_OFF);
#endif // remove old code

            DPRINT_INFO(WFA_OUT, "entering tgRecvStop PROF_UAPSD\n");
            if(psSockfd  > 0)
            {
                wCLOSE(psSockfd);
                psSockfd = -1;
            }
            wMEMSET(&wmmps_info, 0, sizeof(wfaWmmPS_t));

            wfaSetDUTPwrMgmt(PS_OFF);
            wSLEEP(3);
            gtgWmmPS = 0;
            gtgPsPktRecvd = 0;
#endif /* WFA_WMM_PS_EXT */
            break;

        }

        /* encode a TLV for response for "complete/error ..." */
        statResp.status = STATUS_COMPLETE;
        statResp.streamId = streamid;

#if 1
        DPRINT_INFO(WFA_OUT, "stream Id %u rx %u total %llu\n", streamid, myStream->stats.rxFrames, myStream->stats.rxPayloadBytes);
#endif
        wMEMCPY(&statResp.cmdru.stats, &myStream->stats, sizeof(tgStats_t));
        wMEMCPY((dutRspBuf + i * resp_size), (BYTE *)&statResp, resp_size);
        id_cnt++;

        // Not empty it but require to reset the entire table before test starts.
        //wMEMSET(myStream, 0, sizeof(tgStream_t));
    }

    // mark the stream inactive
    myStream->state = WFA_STREAM_INACTIVE;

    printf("Sending back the statistics at recvstop\n");
    wfaEncodeTLV(WFA_TRAFFIC_AGENT_RECV_STOP_RESP_TLV, id_cnt * resp_size, dutRspBuf, respBuf);

    /* done here */
    *respLen = WFA_TLV_HDR_LEN + numStreams * resp_size;

    return WFA_SUCCESS;
}

/*
 * wfaTGS endStart: instruct traffic generator to start sending based on a profile
 * input:      cmd -- not used
 * response:   inform controller for "running"
 * return:     success or failed
 */
int wfaTGSendStart(int len, BYTE *parms, int *respLen, BYTE *respBuf)
{
    int i=0, streamid=0;
    int numStreams = len/4;
    char gCmdStr[WFA_CMD_STR_SZ];

    tgProfile_t *theProfile;
    tgStream_t *myStream = NULL;

#ifdef CONFIG_MTK_COMMON
    dutTgResponse_t staSendResp;
#else
    dutCmdResponse_t staSendResp;
#endif
    int resp_size = sizeof(staSendResp);

    DPRINT_INFO(WFA_OUT, "Entering tgSendStart for %i streams ...\n", numStreams);
    for(i=0; i<numStreams; i++)
    {
        wMEMCPY(&streamid, parms+(4*i), 4);
        myStream = findStreamProfile(streamid);
        if(myStream == NULL)
        {
            staSendResp.status = STATUS_INVALID;
            wfaEncodeTLV(WFA_TRAFFIC_AGENT_SEND_RESP_TLV, 4, (BYTE *)&staSendResp, respBuf);
            *respLen = WFA_TLV_HDR_LEN + 4;
            return WFA_SUCCESS;
        }

        theProfile = &myStream->profile;
        if(theProfile == NULL)
        {
            staSendResp.status = STATUS_INVALID;
            wfaEncodeTLV(WFA_TRAFFIC_AGENT_SEND_RESP_TLV, 4, (BYTE *)&staSendResp, respBuf);
            *respLen = WFA_TLV_HDR_LEN + 4;

            return WFA_SUCCESS;
        }

        if(theProfile->direction != DIRECT_SEND)
        {
            staSendResp.status = STATUS_INVALID;
            wfaEncodeTLV(WFA_TRAFFIC_AGENT_SEND_RESP_TLV, 4, (BYTE *)&staSendResp, respBuf);
            *respLen = WFA_TLV_HDR_LEN + 4;

            return WFA_SUCCESS;
        }

        /*
         * need to reset the stats
         */
        wMEMSET(&myStream->stats, 0, sizeof(tgStats_t));

        // mark the stream active;
        myStream->state = WFA_STREAM_ACTIVE;

        switch(theProfile->profile)
        {
        case PROF_FILE_TX:
        case PROF_MCAST:
        case PROF_TRANSC:
            gtgTransac = streamid;
            gtgSend = streamid;
        case PROF_CALI_RTD:
            gtgCaliRTD = streamid;
        case PROF_IPTV:
            gtgSend = streamid;
            /*
             * singal the thread to Sending WMM traffic
             */
            wmm_thr[usedThread].thr_flag = streamid;
#ifndef _FREERTOS
            wPT_MUTEX_LOCK(&wmm_thr[usedThread].thr_flag_mutex);
            wPT_COND_SIGNAL(&wmm_thr[usedThread].thr_flag_cond);
            wPT_MUTEX_UNLOCK(&wmm_thr[usedThread].thr_flag_mutex);
#else
            xEventGroupSetBits(wmm_thr[usedThread].thr_flag_wait, WFA_TG_BIT);
#endif
            usedThread++;

            *respLen = 0;
            break;
        case PROF_UAPSD:
        {
            int ttout = 20;

            printf(" Run wfa_con timer = %d sec\n", ttout);
#ifdef CONFIG_MTK_AP
            sprintf(gCmdStr,"/data/bin/wfa_con -t %d -i %s %s",
                    ttout,
                    getApMainInterface(),
                    theProfile->WmmpsTagName);
#else
            sprintf(gCmdStr,"/usr/bin/wfa_con -t %d %s",ttout,theProfile->WmmpsTagName);
#endif
            if(system(gCmdStr))
                printf("Done with wfa_con\n");

            staSendResp.status = STATUS_COMPLETE;
            staSendResp.streamId = streamid;
            myStream->stats.txFrames = 10;
            myStream->stats.txPayloadBytes = 10000;
            wMEMCPY(&staSendResp.cmdru.stats, &myStream->stats, sizeof(tgStats_t));
            wfaEncodeTLV(WFA_TRAFFIC_AGENT_SEND_RESP_TLV, resp_size,
                         (BYTE *)&staSendResp, (BYTE *)respBuf);
            *respLen = WFA_TLV_HDR_LEN + resp_size;
        }
        break;
        } /* switch  */
    }/*  for */

    return WFA_SUCCESS;
}

int wfaTGReset(int len, BYTE *parms, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t *resetResp = &gGenericResp;
    int i;

    /* need to reset all traffic socket fds */
    if(btSockfd != -1)
    {
        wCLOSE(btSockfd);
        btSockfd = -1;
    }

    for(i = 0; i<WFA_MAX_TRAFFIC_STREAMS; i++)
    {
        if(tgSockfds[i] != -1)
        {
            wCLOSE(tgSockfds[i]);
            tgSockfds[i] = -1;
        }
    }


    /* reset the timer alarm if it was armed */
    wALARM(0);

    /* just reset the flags for the command */
    gtgRecv = 0;
    gtgSend = 0;
    gtgTransac = 0;
#ifdef WFA_VOICE_EXT
    gtgCaliRTD = 0;
    min_rttime = 0xFFFFFFFF;
    gtgPktRTDelay = 0xFFFFFFFF;
#endif

    totalTranPkts = 0;

    runLoop = 0;

    usedThread = 0;
#ifdef WFA_WMM_PS_EXT
    gtgWmmPS = 0;
    gtgPsPktRecvd = 0;

    if(psSockfd != -1)
    {
        wCLOSE(psSockfd);
        psSockfd = -1;
    }

    wMEMSET(&wmmps_info, 0, sizeof(wfaWmmPS_t));

#endif

    e2eResults[0] = '\0';

    /* Also need to clean up WMM streams NOT DONE YET!*/
    slotCnt = 0;             /* reset stream profile container */
    wMEMSET(gStreams, 0, WFA_MAX_TRAFFIC_STREAMS);

    /*
     * After be asked to reset, it should lower itself back to
     * normal level. It is optional implementation if it is not called
     * while it starts sending/receiving for raising priority level.
     */

    /* encode a TLV for response for "complete ..." */
    resetResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_TRAFFIC_AGENT_RESET_RESP_TLV, 4,
                 (BYTE *)resetResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}

/*
 * calculate the sleep time for different frame rate
 * It should be done according the device
 * This is just one way to adjust the packet delivery speed. If you find
 * you device does not meet the test requirements, you MUST re-adjust
 * the method.
 */

/* The HZ value could be found in the build header file */
/* 100 -> 10ms, 1000 -> 1ms , etc                       */
#define WFA_KERNEL_MIN_TIMER_RES   100   /* HZ Value for 10 ms */

void wfaTxSleepTime(int profile, int rate, int *sleepTime, int *throttledRate)
{
    *sleepTime=0;     /* in microseconds */
    /* calculate the sleep time based on frame rate */

    /*
     * Framerate is still required for Multicast traffic
     * Sleep and hold for a timeout.
     *
     * For WMM traffic, the framerate must also need for VO and VI.
     * the framerate 500, OS may not handle it precisely.
     */
    switch(profile)
    {
    /*
     * make it a fix rate
     * according to test plan, it requires ~80kbps which is around 50 frames/s
     * For other cases which may want to run experiments for very high rate,
     * the change should accommodate the requirement.
     */
    case PROF_MCAST:
        if(rate < 500 && rate >= 50)
        {
            *sleepTime = 100000;  /* sleep for 100 ms */
            *throttledRate = WFA_MCAST_FRATE;
        }
        else
        {
            *sleepTime = 100000;
            *throttledRate = rate;
        }
#if 0
        *throttledRate = WFA_MCAST_FRATE;
#endif
        break;

    /*
     * Vendor must find ways to better adjust the speed for their own device
     */
    case PROF_IPTV:
    case PROF_FILE_TX:
        if(rate >=50 || rate == 0)
        {
            /*
             * this sleepTime indeed is now being used for time period
             * to send packets in the throttled Rate.
             * The idea here is that in each fixed 20 minisecond period,
             * The device will send rate/50 (rate = packets / second),
             * then go sleep for rest of time.
             */
            *sleepTime = 20000; /* fixed 20 miniseconds */
            *throttledRate = (rate?rate:10000)/50;
            printf("Hi Sleep time %i, throttledRate %i\n", *sleepTime, *throttledRate);
        }
        else if(rate == 0)
        {
            *sleepTime = 20000; /* fixed 20 miniseconds */
            *throttledRate = (rate?rate:10000)/50;
            printf("Hi Sleep time %i, throttledRate %i\n", *sleepTime, *throttledRate);
        }
        else if (rate > 0 && rate <= 50) /* typically for voice */
        {
            *throttledRate = 1;
            *sleepTime = 1000*1000/rate;
        }
        break;
    default:
        DPRINT_ERR(WFA_ERR, "Incorrect profile\n");
    }
}

#ifndef _FREERTOS
#define WFA_TIME_DIFF(before, after, rtime, dtime) \
             dtime = rtime + (after.tv_sec*1000000 + after.tv_usec) - (before.tv_sec*1000000 + before.tv_usec);
#else
#ifdef MTK_MINISUPP_ENABLE
#define WFA_TIME_DIFF(before, after, rtime, dtime) \
             dtime = rtime + (after.sec*1000000 + after.usec) - (before.sec*1000000 + before.usec);
#endif
#endif

#ifndef _FREERTOS
void buzz_time(int delay)
{
    struct timeval now, stop;
    int diff;
    int remain_time = 0;

    wGETTIMEOFDAY(&stop, 0);

    stop.tv_usec += delay;
    if(stop.tv_usec > 1000000)
    {
        stop.tv_usec -=1000000;
        stop.tv_sec +=1;
    }

    do
    {
        wGETTIMEOFDAY(&now, 0);
        WFA_TIME_DIFF(now, stop, remain_time, diff);
    }
    while(diff>0);
}
#else
void buzz_time(int delay)
{
    struct os_time now, stop;
    int diff;
    int remain_time = 0;

    wGETTIMEOFDAY(&stop, 0);

    stop.usec += delay;
    if(stop.usec > 1000000)
    {
        stop.usec -=1000000;
        stop.sec +=1;
    }

    do
    {
        wGETTIMEOFDAY(&now, 0);
        WFA_TIME_DIFF(now, stop, remain_time, diff);
    }
    while(diff>0);
}
#endif

/**************************************************/
/* the actually functions to send/receive packets */
/**************************************************/
/* This is going to be a blocking SEND till it finishes */
#ifdef _FREERTOS
ATTR_TEXT_IN_SYSRAM
#endif
int wfaSendLongFile(int mySockfd, int streamid, BYTE *aRespBuf, int *aRespLen)
{
    tgProfile_t           *theProf = NULL;
    tgStream_t            *myStream = NULL;
    struct sockaddr_in    toAddr;
    char                  *packBuf;
    int  packLen;
    int  bytesSent;
#ifdef CONFIG_MTK_COMMON
    dutTgResponse_t sendResp;
#else
    dutCmdResponse_t sendResp;
#endif
    int errcnt = 0;
    int resp_size = sizeof(sendResp);
    int sleepTime = 0;
    int throttledRate = 0;
    int difftime = 0, counter = 0;
    int elapsed_time = 0;
#ifndef _FREERTOS
    struct timeval stime;
    struct timeval before, after, af, start;
#else
    struct os_time stime;
    struct os_time before, after, af, start;
#endif
    int act_sleep_time;
    wGETTIMEOFDAY(&af,0);

    DPRINT_INFO(WFA_OUT, "Entering sendLongFile %i\n", streamid);

    /* find the profile */
    myStream = findStreamProfile(streamid);
    if(myStream == NULL)
    {
        return WFA_FAILURE;
    }

    theProf = &myStream->profile;

    if(theProf == NULL)
    {
        return WFA_FAILURE;
    }

    /* If RATE is 0 which means to send as much as possible, the frame size set to max UDP length */
    if(theProf->rate == 0)
    {
        packLen = MAX_UDP_LEN;
#ifdef CONFIG_MTK_COMMON
        if(theProf->pksize > 0)
        {
            packLen = theProf->pksize;
        }
#endif
    }
    else
        packLen = theProf->pksize;

    /* allocate a buf */
    packBuf = (char *)malloc(packLen + 1);
    if (packBuf == NULL)
    {
        DPRINT_ERR(WFA_ERR, "Failed to malloc packet buffer\n");
        return WFA_FAILURE;
    }
    wMEMSET(packBuf, 0, packLen + 1);

    /* fill in the header */
    wSTRNCPY(packBuf, "1345678", sizeof(tgHeader_t));

    /* initialize the destination address */
    wMEMSET(&toAddr, 0, sizeof(toAddr));
    toAddr.sin_family = AF_INET;
    toAddr.sin_addr.s_addr = inet_addr(theProf->dipaddr);
    toAddr.sin_port = htons(theProf->dport);

    /* if a frame rate and duration are defined, then we know
     * interval for each packet and how many packets it needs to
     * send.
     */
    if(theProf->duration != 0)
    {
        printf("duration %i\n", theProf->duration);

        /*
         *  use this to decide periodical interval sleep time and frames to send
         *  int the each interval.
         *  Each device should adopt a own algorithm for better performance
         */
        wfaTxSleepTime(theProf->profile, theProf->rate, &sleepTime, &throttledRate);
        /*
         * alright, we need to raise the priority level of the process
         * to improve the real-time performance of packet sending.
         * Since this is for tuning purpose, it is optional implementation.
         */

        act_sleep_time = sleepTime - adj_latency;
        if (act_sleep_time <= 0)
            act_sleep_time = sleepTime;

        printf("sleep time %i act_sleep_time %i\n", sleepTime, act_sleep_time);

        wGETTIMEOFDAY(&start, NULL);
        runLoop=1;
        while(runLoop)
        {
            counter++;
            /* fill in the counter */
            int2BuffBigEndian(counter, &((tgHeader_t *)packBuf)->hdr[8]);

            /*
             * the following code is only used to slow down
             * over fast traffic flooding the buffer and cause
             * packet drop or the other end not able to receive due to
             * some limitations, purely for experiment purpose.
             * each implementation needs some fine tune to it.
             */
            if(counter ==1)
            {
                wGETTIMEOFDAY(&before, NULL);
#ifndef _FREERTOS
                before.tv_usec += sleepTime;
                if(before.tv_usec > 1000000)
                {
                    before.tv_usec -= 1000000;
                    before.tv_sec +=1;
                }
#else
                before.usec += sleepTime;
                if(before.usec > 1000000)
                {
                    before.usec -= 1000000;
                    before.sec +=1;
                }
#endif
            }

            if(throttledRate != 0)
            {
                if(counter%throttledRate == 0)
                {
                    wGETTIMEOFDAY(&after, NULL);
                    difftime = wfa_itime_diff(&after, &before);

                    if(difftime > adj_latency)
                    {
                        // too much time left, go sleep
                        wUSLEEP(difftime-adj_latency);

                        wGETTIMEOFDAY(&after, NULL);
                        difftime = wfa_itime_diff(&after, &before);
                    }

                    // burn the rest to absort latency
                    if(difftime >0)
                        buzz_time(difftime);
#ifndef _FREERTOS
                    before.tv_usec += sleepTime;
                    if(before.tv_usec > 1000000)
                    {
                        before.tv_usec -= 1000000;
                        before.tv_sec +=1;
                    }
#else
                    before.usec += sleepTime;
                    if(before.usec > 1000000)
                    {
                        before.usec -= 1000000;
                        before.sec +=1;
                    }
#endif
                }
            } // otherwise, it floods

            /*
             * Fill the timestamp to the header.
             */
            wGETTIMEOFDAY(&stime, NULL);
#ifndef _FREERTOS
            elapsed_time = stime.tv_sec - start.tv_sec;
#else
            elapsed_time = stime.sec - start.sec;
#endif
            /* calculate time time to break */
            if (elapsed_time > theProf->duration)
                break;
#ifndef _FREERTOS
            int2BuffBigEndian(stime.tv_sec, &((tgHeader_t *)packBuf)->hdr[12]);
            int2BuffBigEndian(stime.tv_usec, &((tgHeader_t *)packBuf)->hdr[16]);
#else
            int2BuffBigEndian(stime.sec, &((tgHeader_t *)packBuf)->hdr[12]);
            int2BuffBigEndian(stime.usec, &((tgHeader_t *)packBuf)->hdr[16]);
#endif

            bytesSent = wfaTrafficSendTo(mySockfd, packBuf, packLen,
                                         (struct sockaddr *)&toAddr);

            if(bytesSent != -1)
            {
                myStream->stats.txPayloadBytes += bytesSent;
                myStream->stats.txFrames++;
            }
            else
            {
                int errsv = errno;
                switch(errsv)
                {
                case ENOMEM:
                case EAGAIN:
                case ENOBUFS:
#if CONFIG_TG_DEBUG
                    if (errcnt++ % 20 == 0)
                        DPRINT_ERR(WFA_ERR, "send error %d\n", errsv);
#endif
                    wfaSleepMilsec(1);           /* hold for 1 ms */
                    counter-- ;
                    break;
                case ECONNRESET:
                    runLoop = 0;
                    break;
                case EPIPE:
                    runLoop = 0;
                    break;
                default:
                    DPRINT_ERR(WFA_ERR, "sendto: Packet sent error: fd=%d, len=%d\n",
                        mySockfd, packLen);
                }
            }

        }


        /*
         * lower back to an original level if the process is raised previously
         * It is optional.
         */
    }
    else /* invalid parameters */
    {
        /* encode a TLV for response for "invalid ..." */
        sendResp.status = STATUS_INVALID;
        wfaEncodeTLV(WFA_TRAFFIC_AGENT_SEND_RESP_TLV, 4,
                     (BYTE *)&sendResp, (BYTE *)aRespBuf);

        /* done here */
        *aRespLen = WFA_TLV_HDR_LEN + 4;

        return DONE;
    }

    gtgSend = 0;

    /* free the buffer */
    wFREE(packBuf);

    //printf("done sending long\n");
    /* return statistics */
    sendResp.status = STATUS_COMPLETE;
    sendResp.streamId = myStream->id;
    wMEMCPY(&sendResp.cmdru.stats, &myStream->stats, sizeof(tgStats_t));

#ifdef CONFIG_MTK_COMMON
    DPRINT_INFO(WFA_OUT, "stream Id %u tx %u total %llu\n", myStream->id, myStream->stats.txFrames, myStream->stats.txPayloadBytes);
#endif

    wfaEncodeTLV(WFA_TRAFFIC_AGENT_SEND_RESP_TLV, resp_size,
                 (BYTE *)&sendResp, (BYTE *)aRespBuf);

    *aRespLen = WFA_TLV_HDR_LEN + resp_size;

    return DONE;
}

/* this only sends one packet a time */
#ifdef _FREERTOS
ATTR_TEXT_IN_SYSRAM
#endif
int wfaSendShortFile(int mySockfd, int streamid, BYTE *sendBuf, int pksize, BYTE *aRespBuf, int *aRespLen)
{
    BYTE *packBuf = sendBuf;
    struct sockaddr_in toAddr;
    tgProfile_t *theProf;
    tgStream_t *myStream;
    int packLen, bytesSent=-1;
#ifdef CONFIG_MTK_COMMON
    dutTgResponse_t sendResp;
#else
    dutCmdResponse_t sendResp;
#endif
    int resp_size = sizeof(sendResp);

    if(mySockfd == -1)
    {
        /* stop */
        gtgTransac = 0;
        gtgRecv = 0;
        gtgSend = 0;
        printf("stop short traffic\n");

        myStream = findStreamProfile(streamid);
        if(myStream != NULL)
        {
            sendResp.status = STATUS_COMPLETE;
            sendResp.streamId = streamid;
            wMEMCPY(&sendResp.cmdru.stats, &myStream->stats, sizeof(tgStats_t));

            wfaEncodeTLV(WFA_TRAFFIC_AGENT_SEND_RESP_TLV, resp_size, (BYTE *)&sendResp, aRespBuf);

            *aRespLen = WFA_TLV_HDR_LEN + resp_size;
        }

        return DONE;
    }

    /* find the profile */
    myStream = findStreamProfile(streamid);

    if(myStream == NULL)
    {
        return WFA_FAILURE;
    }

    theProf = &myStream->profile;
    if(theProf == NULL)
    {
        return WFA_FAILURE;
    }

    if(pksize == 0)
        packLen = theProf->pksize;
    else
        packLen = pksize;

    wMEMSET(&toAddr, 0, sizeof(toAddr));
    toAddr.sin_family = AF_INET;
    toAddr.sin_addr.s_addr = inet_addr(theProf->sipaddr);
    toAddr.sin_port = htons(theProf->sport);

    if(gtgRecv && gtgTransac)
    {
        toAddr.sin_addr.s_addr = inet_addr(theProf->sipaddr);
        toAddr.sin_port = htons(theProf->sport);
    }
    else if(gtgSend && gtgTransac)
    {
        toAddr.sin_addr.s_addr = inet_addr(theProf->dipaddr);
        toAddr.sin_port = htons(theProf->dport);
    }

    int2BuffBigEndian(myStream->stats.txFrames, &((tgHeader_t *)packBuf)->hdr[8]);

    if(mySockfd != -1)
        bytesSent = wfaTrafficSendTo(mySockfd, (char *)packBuf, packLen, (struct sockaddr *)&toAddr);

    if(bytesSent != -1)
    {
        myStream->stats.txFrames++;
        myStream->stats.txPayloadBytes += bytesSent;
    }
    else
    {
        int errsv = errno;
        switch(errsv)
        {
        case ENOMEM:
        case EAGAIN:
        case ENOBUFS:
            DPRINT_ERR(WFA_ERR, "send error %d\n", errsv);
            wfaSleepMilsec(1);       /* hold for 1 ms */
            break;
        default:
            ;;
        }
    }

    sentTranPkts++;

    return WFA_SUCCESS;
}

#ifdef _FREERTOS
ATTR_TEXT_IN_SYSRAM
#endif
/* always receive from a specified IP address and Port */
int wfaRecvFile(int mySockfd, int streamid, char *recvBuf)
{
    /* how many packets are received */
    char *packBuf = recvBuf;
    tgProfile_t *theProf;
    tgStream_t *myStream;
    unsigned int bytesRecvd;
    int lostPkts;
#ifndef _FREERTOS
    struct sockaddr_in fromAddr;
#endif

    /* find the profile */
    myStream = findStreamProfile(streamid);
    if(myStream == NULL)
    {
        return WFA_ERROR;
    }

    theProf = &myStream->profile;
    if(theProf == NULL)
    {
        return WFA_ERROR;
    }

    wMEMSET(packBuf, 0, sizeof(tgHeader_t));
#ifndef _FREERTOS
    wMEMSET(&fromAddr, 0, sizeof(fromAddr));
    fromAddr.sin_family = AF_INET;
    fromAddr.sin_addr.s_addr = inet_addr(theProf->dipaddr);
    fromAddr.sin_port = htons(theProf->dport);

    if(gtgRecv && gtgTransac)
    {
        fromAddr.sin_addr.s_addr = inet_addr(theProf->sipaddr);
        fromAddr.sin_port = htons(theProf->sport);
    }
    else if(gtgSend && gtgTransac)
    {
        fromAddr.sin_addr.s_addr = inet_addr(theProf->dipaddr);
        fromAddr.sin_port = htons(theProf->dport);
    }

    /* it is always to receive at least one packet, in case more in the
       queue, just pick them up.
     */
    bytesRecvd = wfaTrafficRecv(mySockfd, packBuf, (struct sockaddr *)&fromAddr, MAX_UDP_LEN);
#else
    bytesRecvd = recv(mySockfd, packBuf, MAX_UDP_LEN, 0);
#endif
    if(bytesRecvd != -1)
    {
        myStream->stats.rxFrames++;
        myStream->stats.rxPayloadBytes +=bytesRecvd;

        /*
         *  Get the lost packet count
         */
        lostPkts = bigEndianBuff2Int(&((tgHeader_t *)packBuf)->hdr[8]) - 1 - myStream->lastPktSN;
        myStream->stats.lostPkts += lostPkts;
        myStream->lastPktSN = bigEndianBuff2Int(&((tgHeader_t *)packBuf)->hdr[8]);
    }
    else
    {
#ifdef CONFIG_MTK_COMMON
        DPRINT_WARNING(WFA_WNG, "Packet received error: %s(%d), sock=%d\n", strerror(errno),
                       errno, mySockfd);
#endif
    }
    return (bytesRecvd);
}

/* send limite bitrate data only
  Condition under Win7:
   payload <= 1000
   rate    <= 3500
*/
#ifdef _FREERTOS
ATTR_TEXT_IN_SYSRAM
#endif
int wfaSendBitrateData(int mySockfd, int streamId, BYTE *pRespBuf, int *pRespLen)
{
    tgProfile_t           *theProf = findTGProfile(streamId);
    tgStream_t            *myStream =findStreamProfile(streamId);
    int                   ret = WFA_SUCCESS;
    struct sockaddr_in    toAddr;
    char                  *packBuf=NULL;
    int                   packLen, bytesSent, rate;
    int                   sleepTimePerSent = 0, nOverTimeCount = 0, nDuration=0, nOverSend=0;
    int                   errcnt = 0;
    unsigned long long int sleepTotal=0, extraTimeSpendOnSending=0;   /* sleep mil-sec per sending  */
    int                   counter = 0, i, iSleep;     /*  frame data sending count */
    int                   elapsed_time = 0;
    unsigned long         difftime;
#ifdef CONFIG_MTK_COMMON
    dutTgResponse_t sendResp;
#else
    dutCmdResponse_t sendResp;
#endif
    int resp_size = sizeof(sendResp);

    //int throttledRate = 0;
#ifndef _FREERTOS
    struct timeval        before, after, stime, start;
#else
    struct os_time        before, after, stime, start;
#endif

    DPRINT_INFO(WFA_OUT, "wfaSendBitrateData entering\n");
    /* error check section  */
    if ( (mySockfd < 0) || (streamId < 0) || ( pRespBuf == NULL)
            || ( pRespLen == NULL) )
    {
        DPRINT_INFO(WFA_OUT, "wfaSendBitrateData pass-in parameter err mySockfd=%i streamId=%i pRespBuf=0x%p pRespLen=0x%p\n",
            mySockfd,streamId,pRespBuf,pRespLen );
        ret= WFA_FAILURE;
        goto errcleanup;
    }

    if ( theProf == NULL || myStream == NULL)
    {
        DPRINT_INFO(WFA_OUT, "wfaSendBitrateData parameter err in NULL pt theProf=%p myStream=%p\n",
            theProf, myStream);
        ret= WFA_FAILURE;
        goto errcleanup;
    }
    if (theProf->rate == 0 || theProf->duration == 0)
    {  /*  rate == 0 means unlimited to push data out which is not this routine to do */
        DPRINT_INFO(WFA_OUT, "wfaSendBitrateData usage error, bitrate=%i or duration=%i\n" ,
            theProf->rate, theProf->duration);
        ret= WFA_FAILURE;
        goto errcleanup;
    }

    if ((theProf->rate > 3500) || (theProf->pksize > 1000))
    {
        DPRINT_INFO(WFA_OUT, "wfaSendBitrateData Warning, must set total streams rate<=10000 and payload<=1000; stream bitrate may not meet\n");
    }

    /* calculate bitrate asked */
    if ( (rate = theProf->pksize * theProf->rate * 8) > WFA_SEND_FIX_BITRATE_MAX)
    {
        DPRINT_INFO(WFA_OUT, "wfaSendBitrateData over birate can do in the routine, req bitrate=%d\n",rate);
        ret= WFA_FAILURE;
        goto errcleanup;
    }

    /* alloc sending buff  */
    //packLen = (theProf->pksize * theProf->rate)/ WFA_SEND_FIX_BITRATE_FRAMERATE ;
    packLen = theProf->pksize;
    packBuf = (char *)malloc(packLen+4);
    if ( packBuf == NULL)
    {
        DPRINT_INFO(WFA_OUT, "wfaSendBitrateData malloc err \n");
        ret= WFA_FAILURE;
        goto errcleanup;
    }
    memset(packBuf, 0, packLen + 4);
	/* fill in the header */
    wSTRNCPY(packBuf, "1345678", sizeof(tgHeader_t));

    /*  initialize the destination address  */
    memset(&toAddr, 0, sizeof(toAddr));
    toAddr.sin_family = AF_INET;
    toAddr.sin_addr.s_addr = inet_addr(theProf->dipaddr);
    toAddr.sin_port = htons(theProf->dport);

    /*  set sleep time per sending */
    if ( theProf->rate < 100  )
        sleepTimePerSent =  WFA_SEND_FIX_BITRATE_SLEEP_PER_SEND  +1;
    else sleepTimePerSent = WFA_SEND_FIX_BITRATE_SLEEP_PER_SEND;

    wGETTIMEOFDAY(&start, NULL);
    runLoop=1; /* global defined share with thread routine, should remove it later  */
    while ( runLoop)
    {
        iSleep = 1;
        wGETTIMEOFDAY(&before, NULL);
        /* send data per second loop */
        for ( i=0; i<= (theProf->rate); i++)
        {
           counter++;
           //iSleep++;
           /* fill in the counter */
           int2BuffBigEndian(counter, &((tgHeader_t *)packBuf)->hdr[8]);
           /*
            * Fill the timestamp to the header.
           */
           wGETTIMEOFDAY(&stime, NULL);
#ifndef _FREERTOS
           int2BuffBigEndian(stime.tv_sec, &((tgHeader_t *)packBuf)->hdr[12]);
           int2BuffBigEndian(stime.tv_usec, &((tgHeader_t *)packBuf)->hdr[16]);
#else
           int2BuffBigEndian(stime.sec, &((tgHeader_t *)packBuf)->hdr[12]);
           int2BuffBigEndian(stime.usec, &((tgHeader_t *)packBuf)->hdr[16]);
#endif
#ifdef CONFIG_MTK_COMMON
           bytesSent = wSENDTO(mySockfd, packBuf, packLen, MSG_DONTWAIT,
               (struct sockaddr *)&toAddr, sizeof(struct sockaddr));
           if(bytesSent >= 0)
           {
               myStream->stats.txPayloadBytes += bytesSent;
               myStream->stats.txFrames++;
           }
           else
           {
               counter--;

               switch (errno) {
               case ENOMEM:
               case EAGAIN:
               case ENOBUFS:
#if CONFIG_TG_DEBUG
                   if (errcnt++ % 20 == 0)
                       DPRINT_INFO(WFA_OUT, "stream %d sendto ERR counter=%i i=%i; send busy, sleep %i MilSec then send\n",
                           streamId, counter, i, sleepTimePerSent);
#endif
                   wfaSleepMilsec(1);
                   sleepTotal++;
                   break;
               case ECONNRESET:
               case EPIPE:
                   DPRINT_INFO(WFA_ERR, "stream %d wfaSendBitrateData stop???", streamId);
                   break;
               default:
                   DPRINT_ERR(WFA_ERR, "sendto: Packet sent error: fd=%d\n",
                       mySockfd);
                   perror("send");
                   break;
               }

               i--;
           }
#else
           bytesSent = wfaTrafficSendTo(mySockfd, packBuf, packLen,
                 (struct sockaddr *)&toAddr);
           if(bytesSent != -1)
           {
               myStream->stats.txPayloadBytes += bytesSent;
               myStream->stats.txFrames++ ;
            }
           else
           {
               counter--;
#if CONFIG_TG_DEBUG
               DPRINT_INFO(WFA_OUT, "wfaSendBitrateData wfaTrafficSendTo call ERR counter=%i i=%i; send busy, sleep %i MilSec then send\n", counter, i, sleepTimePerSent);
#endif
               wfaSleepMilsec(1);
               sleepTotal++;
               i--;
           }
#endif
           /*  sleep per batch sending */
           if ( i == ((int)(theProf->rate/50)) * iSleep)
           {
              wfaSleepMilsec(10);
              sleepTotal = sleepTotal + (unsigned long long int) 10;
              iSleep++;
           }
        }// for loop per second sending
        //iSleep = 0;
        nDuration++;


        wGETTIMEOFDAY(&after, NULL);
#ifndef _FREERTOS
        elapsed_time = after.tv_sec - start.tv_sec;
#else
        elapsed_time = after.sec - start.sec;
#endif
        /* calculate time time to break */
        if (elapsed_time > theProf->duration)
            break;

        /* calculate second rest part need to sleep */
        difftime = wfa_itime_diff(&before, &after);
        if ( difftime < 0 || difftime >= 1000000 )
        {/* over time used, no sleep, go back to send */
            nOverTimeCount++;
            //if (difftime > 1200000 )
               //DPRINT_INFO(WFA_OUT, "wfaSendBitrateData Warning difftime=%i nDuration=%i\n",difftime, nDuration);
            if ( difftime > 1000000)
                extraTimeSpendOnSending += (difftime - 1000000);
            wfaSleepMilsec(1);
            sleepTotal++;
            continue;
        }

        /* difftime < 1 sec case, use sleep to catchup time as 1 sec per sending  */
        /*  check with accumulated extra time spend on previous sending, difftime < 1 sec case */
        if (extraTimeSpendOnSending > 0)
        {
            if ( extraTimeSpendOnSending > 1000000 - difftime )
            {/* reduce sleep time to catch up over all on time sending   */
                extraTimeSpendOnSending = (extraTimeSpendOnSending - (1000000 - difftime));
                wfaSleepMilsec(5);
                sleepTotal = sleepTotal + (unsigned long long int) 5;
                continue;
            }
            else
            {  /* usec used to   */
#ifdef CONFIG_MTK_COMMON
                difftime =(unsigned long)( difftime + extraTimeSpendOnSending);
#else
                difftime =(unsigned long)( difftime - extraTimeSpendOnSending);
#endif
                extraTimeSpendOnSending = 0;
            }
        }

        difftime = difftime/1000; // convert to mil-sec

        if(1000 - difftime > 0)
        {/*  only sleep if there is extrac time with in the second did not spend on sending */
            wfaSleepMilsec(1000 - difftime);
            sleepTotal = sleepTotal + (unsigned long long int)(1000 - difftime);
        }

        if ( theProf->duration < nDuration)
        {
            nOverSend++;
        }

    }// while loop

    if (packBuf) free(packBuf);

    /* return statistics */
    sendResp.status = STATUS_COMPLETE;
    sendResp.streamId = myStream->id;

    memcpy(&sendResp.cmdru.stats, &myStream->stats, sizeof(tgStats_t));
    wfaEncodeTLV(WFA_TRAFFIC_AGENT_SEND_RESP_TLV, resp_size,
                 (BYTE *)&sendResp, (BYTE *)pRespBuf);

    *pRespLen = WFA_TLV_HDR_LEN + resp_size;

    extraTimeSpendOnSending = extraTimeSpendOnSending/1000;
    DPRINT_INFO(WFA_OUT, "*** wfg_tg.cpp wfaSendBitrateData Count=%i txFrames=%i totalByteSent=%i sleepTotal=%llu milSec extraTimeSpendOnSending=%llu nOverTimeCount=%d nOverSend=%i rate=%d nDuration=%d iSleep=%d ***\n",
        counter, (myStream->stats.txFrames),(unsigned int) (myStream->stats.txPayloadBytes), sleepTotal,extraTimeSpendOnSending, nOverTimeCount, nOverSend, theProf->rate , nDuration,iSleep);
    wfaSleepMilsec(1000);
    return ret;

errcleanup:
    /* encode a TLV for response for "invalid ..." */
    if (packBuf) free(packBuf);

    sendResp.status = STATUS_INVALID;
    wfaEncodeTLV(WFA_TRAFFIC_AGENT_SEND_RESP_TLV, 4,
                 (BYTE *)&sendResp, (BYTE *)pRespBuf);
    /* done here */
    *pRespLen = WFA_TLV_HDR_LEN + 4;

    return ret;
}/*  wfaSendBitrateData  */
