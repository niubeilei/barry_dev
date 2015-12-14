/*************************************************
 * This program provide a ping function class
 *
 */

#include "CPing.h"  
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "CPingManager.h"  
//#include "include/Log/CLog.h"  
#include <sys/types.h>  
#include <fcntl.h>  
#define gettid() syscall(224)  
  
void CPing::statistics(int sig)  
{  
    printf("------statistics------\n");  
    printf("%d packets transmitted, %d received , %%%d lost\n", 
			m_nSend, m_nRecv,  (m_nSend - m_nRecv) / m_nSend * 100);  
    //close(m_nSocketfd);  
    m_nTimeOut = m_nSend - m_nRecv;  
    m_dAvgTime = m_dTotalResponseTimes/m_nRecv;  
   // m_bTimeOut = true;  
}  
  
CPing::CPing(const char * ip, int timeout)  
{  
    m_strIp = ip;  
    m_Ip = ip;  
    m_nTimeOut = 0;  
    m_nSend = 0;  
    m_nRecv = 0;  
    m_nSocketfd = 0;  
    m_dFasterResponseTime = -1;  
    m_dLowerResponseTime = -1;  
    m_dAvgTime = -1;  
    m_dTotalResponseTimes = 0;  

    //m_pPingManager = NULL;  
    if(timeout > MAX_WAIT_TIME)  
        m_nMaxTimeWait = MAX_WAIT_TIME;  
    else  
        m_nMaxTimeWait = timeout;  
  
    m_nMaxTestpkg = MAX_NO_PACKETS;  
}  
  
CPing::CPing(const CPing& orig)  
{  
}  
  
CPing::~CPing()  
{  
    CloseSocket();  
}  
  
bool CPing::CreateSocket()  
{  
    CloseSocket();  
    struct hostent hostinfo,*host;  
    char buf[2048];  
    struct protoent *protocol;  
    unsigned long inaddr = 0l;  
    int size = 50 * 1024;  
  
    if ((protocol = getprotobyname("icmp")) == NULL)  
    {  
        printf("CreateSocket: getprotobyname failed:%d\n",errno);  
          
          
        return false;  
    }  
  
    /*生成使用ICMP的原始套接字,这种套接字只有root才能生成*/  
    if ((m_nSocketfd = socket(PF_INET, SOCK_RAW, protocol->p_proto)) < 0)  
    {  
        printf("CreateSocket: create socket failed:%d\n",errno);  
          
          
        return false;  
    }  
  
    /* 回收root权限,设置当前用户权限*/  
    //setuid(getuid());  
  
    /*扩大套接字接收缓冲区到50K这样做主要为了减小接收缓冲区溢出的 
      的可能性,若无意中ping一个广播地址或多播地址,将会引来大量应答*/  
    int nRet = setsockopt(m_nSocketfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof (size));  
    if(nRet != 0)  
    {          
        printf("CreateSocket: set socket receive buf failed:%d\n",errno);  
          
        return false;  
          
    }  
    bzero(&m_dest_addr, sizeof (m_dest_addr));  
    m_dest_addr.sin_family = PF_INET;  
  
    /*判断是主机名还是ip地址*/  
    if ((inaddr = inet_addr(m_strIp.c_str())) == INADDR_NONE)  
    {  
        int nret;  
        gethostbyname_r(m_strIp.c_str(), &hostinfo, buf, sizeof(buf), &host, &nret);  
        if (nret != 0) /*是主机名*/  
        {  
            printf("CreateSocket: gethostbyname error %s failed:%d\n",m_strIp.c_str(),errno);  
              
            return false;  
          
        }  
        memcpy((char *) &m_dest_addr.sin_addr, host->h_addr, host->h_length);  
    }  
    else /*是ip地址*/  
        memcpy((char *) &m_dest_addr.sin_addr, (char *) &inaddr, sizeof(inaddr));  
  
    m_Ip = inet_ntoa(m_dest_addr.sin_addr);  
    return true;  
}  
  
bool CPing::CloseSocket()  
{  
    if(m_nSocketfd !=0)  
        close(m_nSocketfd);  
    m_nSocketfd = 0;  
    return true;  
}  
  
bool CPing::ping(int times)  
{  
    int i = 0;  

    while(i < times)  
    {  
        bool bRet = CreateSocket();  
        if(!bRet)  
        {  
            printf("ping: create socket falied!\n");  
            return false;  
        }
  
        /*获取main的进程id,用于设置ICMP的标志符*/  
        int nh = gettid();  
        nh = nh<<8;  
        time_t t;  
        time(&t);  
        int nt = t;  
        nh = nh&0xff00;  
        nt = nt&0xff;  
        m_Pid = nt|nh;  
        printf("PING %s(%s): %d bytes data in ICMP packets.\n", m_strIp.c_str(),  
                m_Ip.c_str(), SEND_DATA_LEN);  
        i++;  
        m_nSend = 0;  
        m_nRecv = 0;  
        send_packet(); /*发送所有ICMP报文*/  
        recv_packet(); /*接收所有ICMP报文*/  
        //statistics(SIGALRM); /*进行统计*/  
        if(m_nRecv > 0)  
            break;  
    }  

    m_bTimeOut = false;  
    if(m_nRecv > 0)  
    {  
        m_nTimeOut = m_nSend - m_nRecv;  
        m_dAvgTime = m_dTotalResponseTimes/m_nRecv;  
    }  
    else  
    {  
        m_nTimeOut = m_nSend;  
        m_dAvgTime = -1;  
        return false;  
    }  

    return true;  
}  
  
unsigned short CPing::cal_chksum(unsigned short *addr, int len)  
{  
    int nleft=len;  
    int sum=0;  
    unsigned short *w=addr;  
    unsigned short answer=0;  
  
    while(nleft > 1)  
    {  
        sum += *w++;  
        nleft -= 2;  
    }  
  
    if( nleft == 1)  
    {  
        *(unsigned char *)(&answer) = *(unsigned char *)w;  
        sum += answer;  
    }  
  
    sum = (sum >> 16) + (sum & 0xffff);  
    sum += (sum >> 16);  
    answer = ~sum;  
  
    return answer;  
}  
  
  
void CPing::tv_sub(struct timeval *out,struct timeval *in)  
{         
    if( (out->tv_usec-=in->tv_usec)<0)  
    {         
        --out->tv_sec;  
        out->tv_usec+=1000000;  
    }  
    out->tv_sec-=in->tv_sec;  
}  
  
/*设置ICMP报头*/  
int CPing::pack(int pack_no)  
{  
    int packsize;  
    struct icmp *icmp;  
    struct timeval *tval;  
  
    icmp = (struct icmp*) m_sendpacket;  
    icmp->icmp_type = ICMP_ECHO;  
    icmp->icmp_code = 0;  
    icmp->icmp_cksum = 0;  
    icmp->icmp_seq = pack_no;  
    icmp->icmp_id = m_Pid;  
    packsize = 8 + SEND_DATA_LEN;  
    tval = (struct timeval *) icmp->icmp_data;  
    gettimeofday(tval, NULL); /*记录发送时间*/  
    icmp->icmp_cksum = cal_chksum((unsigned short *) icmp, packsize); /*校验算法*/  
    return packsize;  
}  
  
/*发送三个ICMP报文*/  
void CPing::send_packet()  
{  
    int packetsize;  
    while (m_nSend < m_nMaxTestpkg)  
    {  
        m_nSend++;  
        packetsize = pack(m_nSend); /*设置ICMP报头*/  
        if (sendto(m_nSocketfd, m_sendpacket, packetsize, 0,  
                (struct sockaddr *) &m_dest_addr, sizeof (m_dest_addr)) < 0)  
        {  
            printf("send_packet: send error :%d\n",errno);  
              
            continue;  
        }  
        usleep(100); /*每隔0.001second 发ICMP报文*/  
    }  
}  
  
/*接收所有ICMP报文*/  
void CPing::recv_packet()  
{  
    int n,fromlen;  
  
    
    while(m_nRecv < m_nSend)  
    {  
        struct timeval timeo;  
        fd_set readfds;  
        FD_ZERO(&readfds);  
        FD_SET(m_nSocketfd,&readfds);  
        int maxfds = m_nSocketfd +1;  
        timeo.tv_sec = m_nMaxTimeWait;  
        timeo.tv_usec = 0;  
  
        n = select(maxfds,&readfds,NULL,NULL,&timeo);  
        if(n == 0)  
        {  
            printf("recv_packet: select time out :%d\n",errno);  
              
            return ;  
        }  
        else if(n < 0)  
        {  
            printf("recv_packet: select error :%d\n",errno);  
              
            if(errno == EINTR)  
            {  
                printf("recv_packet: select error :%d\n",errno);  
                  
                continue;  
            }  
            else  
            {  
                printf("recv_packet: select error :%d\n",errno);  
                  
                return ;  
            }  
        }  
        if ((n = recvfrom(m_nSocketfd, m_recvpacket, sizeof (m_recvpacket), 0,  
                    (struct sockaddr *) &m_from, (socklen_t *)&fromlen)) <= 0)  
        {  
            printf("recv_packet: recv error :%d\n",errno);  
              
            return;  
        }  
        gettimeofday(&m_tvrecv, NULL); /*记录接收时间*/  
        if (unpack(m_recvpacket, n) == -1)  
        {  
            continue;  
        }  
            m_nRecv++;  
    }  
    //return ;  
}  
/*剥去ICMP报头*/  
int CPing::unpack(char *buf,int len)  
{  
    int i,iphdrlen;  
    struct ip *ip;  
    struct icmp *icmp;  
    struct timeval *tvsend;  
    double rtt;  
  
    ip = (struct ip *) buf;  
    iphdrlen = ip->ip_hl << 2; /*求ip报头长度,即ip报头的长度标志乘4*/  
    icmp = (struct icmp *) (buf + iphdrlen); /*越过ip报头,指向ICMP报头*/  
    len -= iphdrlen; /*ICMP报头及ICMP数据报的总长度*/  
    if (len < 8) /*小于ICMP报头长度则不合理*/  
    {  
        printf( "ICMP packets\'s length is less than 8\n");  
          
        return -1;  
    }  
    /*确保所接收的是我所发的的ICMP的回应*/  
    if ((icmp->icmp_type == ICMP_ECHOREPLY) && (icmp->icmp_id == m_Pid)&&(m_Ip == inet_ntoa(m_from.sin_addr)))  
    {  
        tvsend = (struct timeval *) icmp->icmp_data;  
        tv_sub(&m_tvrecv, tvsend); /*接收和发送的时间差*/  
        rtt = m_tvrecv.tv_sec * 1000 + m_tvrecv.tv_usec / 1000; /*以毫秒为单位计算rtt*/  
        m_dTotalResponseTimes += rtt;  
        if(m_dFasterResponseTime == -1)  
        {  
            m_dFasterResponseTime = rtt;  
        }  
        else if(m_dFasterResponseTime > rtt)  
        {  
            m_dFasterResponseTime = rtt;  
        }  
  
        if(m_dLowerResponseTime == -1)  
        {  
            m_dLowerResponseTime = rtt;  
        }  
        else if(m_dLowerResponseTime < rtt)  
        {  
            m_dLowerResponseTime = rtt;  
        }  
  
        /*显示相关信息*/  
        printf("%d\tbyte from %s\t: icmp_seq=%u\tttl=%d\trtt=%.3f\tms\n",  
                len,  
                inet_ntoa(m_from.sin_addr),  
                icmp->icmp_seq,  
                ip->ip_ttl,  
                rtt);  
    }  
    else  
    {  
       /*printf("throw away the old package %d\tbyte from %s\t: icmp_seq=%u\tttl=%d\trtt=%.3f\tms\n",  
                len,  
                inet_ntoa(m_from.sin_addr),  
                icmp->icmp_seq,  
                ip->ip_ttl,  
                rtt);  
        */  
        return -1;  
    }  
}  

//init ping parameters
bool CPing::start()
{
	bool bRet = CreateSocket();  
	if(!bRet)  
	{  
		printf("ping: create socket falied!\n");  
		return false;  
	}  

	/*获取main的进程id,用于设置ICMP的标志符*/  
	int nh = gettid();  
	time_t t;  
	int nt;

	nh = nh<<8;  
	time(&t);  
	nt = t;  
	nh = nh&0xff00;  
	nt = nt&0xff;  
	m_Pid = nt|nh;  
	printf("PING %s(%s): %d bytes data in ICMP packets.\n", m_strIp.c_str(),  m_Ip.c_str(), SEND_DATA_LEN);  

	m_nSend = 0;  
	m_nRecv = 0;  

	return true;
}

bool CPing::finish()
{
	CloseSocket();

    m_bTimeOut = false;  
    if(m_nRecv > 0)  
    {  
        m_nTimeOut = m_nSend - m_nRecv;  
        m_dAvgTime = m_dTotalResponseTimes/m_nRecv;  
    }  
    else  
    {  
        m_nTimeOut = m_nSend;  
        m_dAvgTime = -1;  
        return false;  
    }  

	return true;
}

