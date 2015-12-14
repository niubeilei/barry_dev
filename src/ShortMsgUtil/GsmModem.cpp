////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 01/03/2010: Copy by Brian 
////////////////////////////////////////////////////////////////////////////
#include "ShortMsgUtil/GsmModem.h"

#include "Alarm/Alarm.h"
#include "ShortMsgUtil/CommOpr.h"
#include "XmlUtil/SeXmlParser.h"
#include "ShortMsgUtil/ShortMsgUtil.h"  
#include "ShortMsgUtil/Ptrs.h"   
#include "ShortMsgUtil/SmsReq.h"   
#include "ShortMsgUtil/SmsProc.h"   
#include "ShortMsgSvr/ShortMsgSvr.h"   
#include "Thread/Mutex.h"
#include "XmlUtil/SeXmlParser.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <time.h>

using namespace std;

static AosShortMsgUtil sgSmsUtil;


AosGsmModem::AosGsmModem(const AosXmlTagPtr &config)
{
	aos_assert(config);
	aos_assert(init(config));
}


AosGsmModem::~AosGsmModem()
{
}


bool
AosGsmModem::init(const AosXmlTagPtr &config)
{
	// init area code. 
	mAreaCode = config->getAttrStr(AOSTAG_AREA_CODE);
	if(mAreaCode == "")
	{
		OmnAlarm << "Areacode is empty!" << enderr;
		return false;
	}
	
	// init the phone number of server center.
	OmnString smsp = config->getAttrStr(AOSTAG_SMSP);
	if(smsp == "")
	{
		OmnAlarm << "Phone center number is empty!" << enderr;
		return false;
	}
	sgSmsUtil.telToPdu((AosUCharPtr_t)smsp.data(),
			(AosUCharPtr_t)mPduSvrPhone.data(), (AosUCharPtr_t)mAreaCode.data());

	// init the phone number of sim card.
	mPhone = config->getAttrStr(AOSTAG_MOBLE_NUMBER);
	if(mPhone == "")
	{
		OmnAlarm << "Phone is empty!" << enderr;
		return false;
	}
	
	// init how many times try to read and the time to delay
	mTryToRead = config->getAttrInt("try_read", 15);
	mDelayTime = config->getAttrInt("delay_time", 1);
		
	return true;
}


bool	
AosGsmModem::openModem(const int &port)
{
	// Open serial port
    mSerialPort = OpenComm(port);
    if (mSerialPort<1)
	{
		OmnAlarm << "Port init failed" << enderr;
		return false;  
	}

	// new smsproc instance
	mProc = OmnNew AosSmsProc(mSerialPort, mTryToRead, mPhone);
	if (!mProc)
	{
		OmnAlarm << "Failed to init sms proc" << enderr;
		return false;
	}

	// Set parameter of serial port
    SetCommParam(mSerialPort, 9600, 'N', 8, 1); 

	// Init GSM Modem
    for(int i=0; i<mTryToRead; i++)  
    {   
		mProc->writeToPort("AT\r");
        if(mProc->readOk())  
        {  
			mProc->writeToPort("ATE0\r");
            if(mProc->readOk()) 
            {  
				mProc->writeToPort("AT+CNMI=2,2,,1\r");
                if(mProc->readOk())
                {
					mProc->startThread();
					OmnScreen << "Init GSM Modem success!" << endl;
					return true;
                }  
            }  
        }
		OmnAlarm << "Try " << i+1 << "Times to init GSM Modem!" << enderr;
    }  
    closeModem(); 
    return false;  
}  
 

void
AosGsmModem::closeModem()  
{
	CloseComm(mSerialPort);
}  


bool
AosGsmModem::sendMessage(
			AosUCharPtr_t pUserCode, 
			AosUCharPtr_t pMessage, 
			const AosRundataPtr &rdata) 
{
	// The length of short message, need to "/2"
    aos_assert_r(strlen((char *)pMessage)>0, false);
   
	AosUChar_t szTextHEX[1024]; 
	AosUChar_t szUserCodePDU[20]; 
    memset(szTextHEX, 0, 1024);
    memset(szUserCodePDU, 0, 20);
	// Hex
    sgSmsUtil.str2hex(szTextHEX, pMessage); 
    sgSmsUtil.telToPdu(pUserCode, szUserCodePDU,
			(AosUCharPtr_t)mAreaCode.data());
    int hLen = strlen((char *)szTextHEX);  
    
	int cLen = hLen;	
	
	// PDU
	AosSmsReqPtr req = OmnNew AosSmsReq();
	if (cLen <= 280)
	{
		convertToPdu((unsigned char *)&szTextHEX, cLen, (unsigned char *)&szUserCodePDU, req, rdata);
	}
	else
	{
		int idx = 0;
		AosSmsReqPtr aReq = OmnNew AosSmsReq();
		AosUCharPtr_t text = szTextHEX;
		AosUChar_t szText[300]; 
		memset(szText, 0, 300);
		while (cLen>280)
		{
			int hexLen = (cLen>280) ? 280 : cLen;  
			strncpy((char *)szText, (char *)text, hexLen);  
			szText[hexLen]='\0';
			if (idx == 0)
			{
				aReq = req;
			}
			convertToPdu((unsigned char *)&szText, hexLen, (unsigned char *)&szUserCodePDU, aReq, rdata);
			cLen = hLen - 280;
			aReq = aReq->mtPart;
			text = text + 280;
		}
	}

	bool rslt = mProc->addProcSmsRequest(req, rdata);
	if (!rslt)
	{
		AosSetError(rdata, AosErrmsgId::eFailedAddProc);
		OmnAlarm << rdata->getErrmsg() 
			<< "Content: " << enderr;
		return false;
	}
	return true;
}  


bool
AosGsmModem::convertToPdu(
		AosUCharPtr_t szText,
		const int &length,
		AosUCharPtr_t szUserCodePDU,
		AosSmsReqPtr &sReq,
		const AosRundataPtr &rdata)
/*数据包结构说明: 
    1-2位:(短信中心号码长度+2(即3-4位长度))/2(16进制) 
    3-4位:短信中心号码有无国家编码(91有,81无) 
    X位:短信中心号码(需PDU编码),以下为X位后 
    1-2位:状态报告(31有,11无) *注意:从此处开始计算数据包长度 
    3-4位:分隔符(永远为00) 
    5-6位:目标号码长度(16进制) 
    7-8位:目标号码有无国家编码(91有,81无) 
    XX为:目标号码(需PDU编码),以下为XX位后 
    1-2位:TP-PID协议(一般为00) 
    3-4为:是否免提(18是,08否) 
    5-6位:有效期标志(一般为00) 
    7-8位:短信实际长度(16进制) 
    XXX位:短信内容,以下为XXX位后 
    1位:结束符(ASCII码26) 
*/
{
    AosUChar_t szBuff[64]; //临时变量  
    AosUChar_t szPacket[512]; //短信数据包  
	memset(szPacket, 0, 512); //清空数据包缓冲区  
    memset(szBuff, 0, 64); //清空数据包缓冲区  

    /* **********************************封装数据包***************************** */ 
	// 数据包的第一个长度字段,即(中心号码长度+2位国家编码长度)/2
	 int sCPlen= (strlen(mPduSvrPhone.data())+2)/2;  
	sprintf((char *)szPacket, (sCPlen>15) ? "%2X" : "0%1X", sCPlen);

	//中心号码有无国家编码
    sprintf((char *)szPacket, "%s%d", szPacket, ((mPduSvrPhone.data()[0]=='6')&&(mPduSvrPhone.data()[1]=='8')) ? 91 : 81);   
	// 中心号码
	sprintf((char *)szPacket, "%s%s", szPacket, mPduSvrPhone.data()); 


	// Now get the length of "at + cmgs= length/r"
	//从此处开始计算数据包长度(即短信中心号码以后的字符串长度的1/2)
   	int pLen = strlen((char *)szPacket); 

	//状态报告、分隔符
    sprintf((char *)szPacket, "%s31000D91", szPacket); 

	// 去掉最后一个'F'
    int uPlen = strlen((char *)szUserCodePDU)-3;

	// 目标号码长度
	//sprintf((char *)szPacket, (uPlen>15) ? "%s%2X" : "%s0%1X", szPacket, uPlen); 

	// 目标号码有无国家编码
   // sprintf((char *)szPacket, "%s%d", szPacket, ((szUserCodePDU[0]=='6')&&(szUserCodePDU[1]=='8')) ? 91 : 81); 
	// 目标号码
    sprintf((char *)szPacket, "%s%s", szPacket, szUserCodePDU); 

	// TP-PID协议、免提标志、有效期
    sprintf((char *)szPacket, "%s000800", szPacket); 

	// 短信内容实际长度
    int cLen = strlen((char *)szText)/2; 
    sprintf((char *)szPacket, (cLen>15) ? "%s%2X" : "%s0%1X", szPacket, cLen);

	// 短信内容及结束符(结束符为1A1D,也有为ASCII码26的
    //sprintf((char *)szPacket, "%s%s", szPacket, szText); 
    /***********************************封装结束****************************** */ 
    sprintf((char *)szPacket, "%s%s%c", szPacket, szText, 26); 
   	
	int sLen = strlen((char *)szPacket);

	int pckLen = (sLen - pLen)/2;  
	sprintf((char *)szBuff, "AT+CMGS=%d\r", pckLen);
    
	//sprintf((char *)szPacket, "%s%c", szPacket, 26); 
	
	// add to smsreq
	sReq->fd = mSerialPort;
	sReq->iNumStr = OmnString((char *)szBuff);
	sReq->sContent = OmnString((char *)szPacket);
	
	return true;
}  

