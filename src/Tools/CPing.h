/*************************************************
 * This program provide a ping function class
 *
 */
#ifndef CPING_H  
#define CPING_H  
#include <string>  
#include <signal.h>  
#include <arpa/inet.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <unistd.h>  
#include <netinet/in.h>  
#include <netinet/ip.h>  
#include <netinet/ip_icmp.h>  
#include <netdb.h>  
#include <setjmp.h>  
#include <errno.h>  
#include <sys/time.h>  
using namespace std;  
  
    
#define PACKET_SIZE     4096  
#define SEND_DATA_LEN   56  
#define ERROR           -1  
#define SUCCESS         1  
#define MAX_WAIT_TIME   5  
#define MAX_NO_PACKETS  4  
  
class CPing  
{  
public:  
    CPing(const char * ip, int timeout);  
    CPing(const CPing& orig);  
    virtual ~CPing();  
private:  
    std::string m_strIp;  
    std::string m_Ip;  
    int m_nTimeOut;  
    int m_nPkgLen;  
    double m_dAvgTime;  
    double m_dFasterResponseTime;  
    double m_dLowerResponseTime;  
    double m_dTotalResponseTimes;  
    int m_nSend;  
    int m_nRecv;  
    int m_nSocketfd;  
    pid_t m_Pid;  
    struct sockaddr_in m_dest_addr;  
    struct sockaddr_in m_from;  
    char m_sendpacket[PACKET_SIZE];  
    char m_recvpacket[PACKET_SIZE];  
    struct timeval m_tvrecv;  
  
    bool m_bTimeOut;  
  
    int m_nMaxTimeWait;  
    int m_nMaxTestpkg;  

public:  
    enum  
    {  
        PING_FAILED,  
        PING_SUCCEED  
    };  
  
    void SetMaxTimeWait(int nMaxTimeWait) { m_nMaxTimeWait = nMaxTimeWait; }  
    void SetMaxTestpkg(int nMaxTestpkg) { m_nMaxTestpkg = nMaxTestpkg; }  
  
    //void SetPingManager(CPingManager * pPingManager) { m_pPingManager = pPingManager; }  
  
    int GetSuccessTimes() { return m_nRecv; }  
    int GetSendTimes() { return m_nSend; }  
    std::string GetIp() { return m_Ip; }  
    int GetTimeOut() { return m_nTimeOut; }  
    int GetPkgLen() { return m_nPkgLen; }  
  
    void SetIp(const char * ip) { m_strIp = ip; m_Ip = ip; }  
    void SetTimeOut(int timeout) { m_nTimeOut = timeout; }  
    void SetPkgLen(int pkglen) { m_nPkgLen = pkglen; }  
  
    double GetAvgResponseTime() { return m_dAvgTime; }  
    double GetFasterResponseTime() { return m_dFasterResponseTime; }  
    double GetLowerResponseTime() { return m_dLowerResponseTime; }  
    unsigned int GetPingStatus();  
  
    static unsigned short cal_chksum(unsigned short *addr, int len);  
    //void statistics(int sig);  
    int pack(int pack_no);  
    void send_packet(void);  
    void recv_packet(void);  
    int unpack(char *buf, int len);  
    void tv_sub(struct timeval *out, struct timeval *in);  
  
    bool ping(int times);  
    void statistics(int sig);  
  
    bool CreateSocket();  
    bool CloseSocket();  
    //double ping_m();  
	
	bool start();
	bool finish();
};  
  
#endif  /* CPING_H */  
