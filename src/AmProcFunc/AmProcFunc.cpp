////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AccessMgr.cpp
// Description:
//
// Modification History:
// 2007-04-17: Created by CHK
////////////////////////////////////////////////////////////////////////////
#include "AccessManager/AccessMgr.h"
#include "AccessManager/AmThread.h"
#include "AmProcFunc/UserBasedAC.h"
#include "AmProcFunc/UserBasedACL4.h"
#include "AmProcFunc/UserBasedACVpn.h"
#include "AmProcFunc/UserAuth.h"

#include "AmUtil/AmRcvBuff.h"
#include "AmUtil/AmTrans.h"
#include "AmUtil/AmMsg.h"
#include "AmUtil/AmTagId.h"
#include "AmUtil/AmExternalSvr.h"
#include "aosUtil/Alarm.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util/OmnNew.h"
#include "UtilComm/TcpServerGrp.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/ReturnCode.h"
#include "UtilComm/TcpBouncerListener.h"

// database execution
#include "Database/DbRecord.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"

#include <stdlib.h>
#include <stdio.h>


#define	PKG_PUT_BYTE(val, cp) 	(*(cp)++ = (unsigned char)(val))

#define	PKG_PUT_SHORT(val, cp) \
do \
{ \
	unsigned short Xv; \
	Xv = (unsigned short)(val); \
	*(cp)++ = (unsigned char)(Xv >> 8); \
	*(cp)++ = (unsigned char)Xv; \
} \
while(0)

#define	PKG_PUT_NETSHORT(val, cp) \
do \
{ \
	unsigned char *Xvp; \
	unsigned short Xv = (unsigned short)(val); \
	Xvp = (unsigned char *)&Xv; \
	*(cp)++ = *Xvp++; \
	*(cp)++ = *Xvp++; \
} \
while(0)

#define	PKG_PUT_LONG(val, cp) \
do \
{ \
	unsigned long Xv; \
	Xv = (unsigned long)(val); \
	*(cp)++ = (unsigned char)(Xv >> 24); \
	*(cp)++ = (unsigned char)(Xv >> 16); \
	*(cp)++ = (unsigned char)(Xv >>  8); \
	*(cp)++ = (unsigned char)Xv; \
} \
while(0)

#define	PKG_PUT_NETLONG(val, cp) \
do \
{ \
	unsigned char *Xvp; \
	unsigned long Xv = (unsigned long)(val); \
	Xvp = (unsigned char *)&Xv; \
	*(cp)++ = *Xvp++; \
	*(cp)++ = *Xvp++; \
	*(cp)++ = *Xvp++; \
	*(cp)++ = *Xvp++; \
} \
while(0)


/////////////////////////////////////////////////////////
// Global functions for processing
/////////////////////////////////////////////////////////
//#include <sqlite3.h>
#include "AmUtil/ReturnCode.h"

int InternalErrorProcess(const AosAmTransPtr &trans, OmnString &usr, OmnString &errmsg)
{
	AosAmMsgPtr msgPtr = trans->getMsg();

	msgPtr->resetTags();
	msgPtr->addTag(AosAmTagId::eRsltCode, (u16)AosAmRespCode::eInternalError);
	msgPtr->addTag(AosAmTagId::eUser, usr);
	msgPtr->addTag(AosAmTagId::eErrmsg, errmsg);
	msgPtr->setMsgId((u8)AosAmMsgId::eResponse);
	trans->setMsg(msgPtr);

	return -eAosRc_AmInternalError;
}

int AmAttLocal(const AosAmTransPtr &trans, const AosAccessMgrPtr & amMgr)
{
	// do some thing and set the response message to trans
	// 
	if(!trans)
	{
		return -1; // NULL pointer!!!
	}

	AosAmMsgPtr msgPtr = trans->getMsg();
	// 1. get the input attributes first
	OmnString app;
	OmnString usr;
	OmnString opr;
	u32 uid = 0;
	u8 	msgId;
	OmnString rscPath;
	OmnString pwd;
	OmnString errMsg;
	OmnString denyReason;

	msgId = msgPtr->getMsgId();
	if(msgId == AosAmMsgId::eUserAuth)
	{
		// User authentication
		if(!msgPtr->getTag(usr, (u8)AosAmTagId::eUser))
		{
			OmnAlarm << "Server failed to get tag information!" << enderr;
			return InternalErrorProcess(trans, usr, errMsg);
		}
		if(!msgPtr->getTag(pwd, (u8)AosAmTagId::ePasswd))
		{
			OmnAlarm << "Server failed to get tag information!" << enderr;
			return InternalErrorProcess(trans, usr, errMsg);
		}
		// 2. access local data base for result
		AosUserAuth auth(usr, pwd);

		// 3. reset the transaction message
		msgPtr->resetTags();

		// 4. set response transaction message
		OmnString errmsg;
		u16 errcode;
		if(auth.checkAccess(errcode, errmsg))
		{
			u16 sessionId;
			if(amMgr->addUser(sessionId, usr))
			{
				msgPtr->setSessionId(sessionId);
				// success
				msgPtr->addTag(AosAmTagId::eRsltCode, (u16)0);
				msgPtr->addTag(AosAmTagId::eUser, usr);
				msgPtr->addTag(AosAmTagId::eUid, (u32)auth.getUserId());
OmnTrace << "user[" << usr << "] response code[" << 0 << "]!" << __LINE__ << endl;
			}
			else
			{
				errmsg << "Fail to add user session. Internal Error" << "\n";
				OmnAlarm << errmsg << enderr;
				// fail
				msgPtr->addTag(AosAmTagId::eRsltCode, (u16)AosAmRespCode::eInternalError);
				msgPtr->addTag(AosAmTagId::eUser, usr);
				msgPtr->addTag(AosAmTagId::eErrmsg, errmsg);
OmnTrace << "user[" << usr << "] response code[" << AosAmRespCode::eInternalError << "]!" << __LINE__ << endl;
			}
		}
		else
		{
			// denied
			msgPtr->addTag(AosAmTagId::eRsltCode, (u16)errcode);
			msgPtr->addTag(AosAmTagId::eUser, usr);
			msgPtr->addTag(AosAmTagId::eErrmsg, errmsg);
OmnTrace << "user[" << usr << "] response code[" << errcode << "]!" << __LINE__ << endl;
		}
		msgPtr->setMsgId((u8)AosAmMsgId::eResponse);
		trans->setMsg(msgPtr);
	}
	else if(msgId == AosAmMsgId::eUserBasedAccess)
	{
		// User authorization
		// User based access
		if((!msgPtr->getTag(usr, (u8)AosAmTagId::eUser))
		&& (!msgPtr->getTag(uid, (u8)AosAmTagId::eUid)))
		{
			OmnAlarm << "Server failed to get tag information!" << enderr;
			if(uid)
			{
				usr << uid;
				return InternalErrorProcess(trans, usr, errMsg);
			}
			else
			{
				return InternalErrorProcess(trans, usr, errMsg);
			}
		}
		if(!msgPtr->getTag(app, (u8)AosAmTagId::eApp))
		{
			OmnAlarm << "Server failed to get tag information!" << enderr;
			return InternalErrorProcess(trans, usr, errMsg);
		}
		if(!msgPtr->getTag(opr, (u8)AosAmTagId::eOpr))
		{
			OmnAlarm << "Server failed to get tag information!" << enderr;
			return InternalErrorProcess(trans, usr, errMsg);
		}
		if(!msgPtr->getTag(rscPath, (u8)AosAmTagId::eRsc))
		{
			OmnAlarm << "Server failed to get tag information!" << enderr;
			return InternalErrorProcess(trans, usr, errMsg);
		}

		opr.toLower();
		OmnTrace << "Local function usr!usr[" << usr 
				 << "] uid[" << uid << "] app[" << app << "] opr[" << opr << "] rsc[" 
				 << rscPath << "] " << endl;

		// 2. access local data base for result 
		AosUserBasedAC acc;
		if(uid)
		{
			acc = AosUserBasedAC(uid, "", app, opr, rscPath);
		}
		else
		{
			acc = AosUserBasedAC(usr, app, opr, rscPath);
		}

		// 3. reset the transaction message
		msgPtr->resetTags();
	
		// 4. set response transaction message
		OmnString errmsg;
		u16 errcode;
		acc.checkAccess(errcode, errmsg);
		msgPtr->addTag(AosAmTagId::eRsltCode, (u16)errcode);
		msgPtr->addTag(AosAmTagId::eUser, usr);
		msgPtr->addTag(AosAmTagId::eErrmsg, errmsg);

		msgPtr->setMsgId((u8)AosAmMsgId::eResponse);
		trans->setMsg(msgPtr);
	}
	else if(msgId == AosAmMsgId::eUserBasedAccessL4)
	{
		// User authorization
		// User based access
		u32 		ip_addr;
		u16 		port;
		if((!msgPtr->getTag(usr, (u8)AosAmTagId::eUser))
		&& (!msgPtr->getTag(uid, (u8)AosAmTagId::eUid)))
		{
			OmnAlarm << "Server failed to get tag information!" << enderr;
			return InternalErrorProcess(trans, usr, errMsg);
		}
OmnTrace << "Local function TTTTTTTT!" << __LINE__ << endl;
		if(!msgPtr->getTag(ip_addr, (u8)AosAmTagId::eIp))
		{
			OmnAlarm << "Server failed to get tag information!" << enderr;
			return InternalErrorProcess(trans, usr, errMsg);
		}
OmnTrace << "Local function TTTTTTTT!" << __LINE__ << endl;
		if(!msgPtr->getTag(port, (u8)AosAmTagId::ePort))
		{
			OmnAlarm << "Server failed to get tag information!" << enderr;
			return InternalErrorProcess(trans, usr, errMsg);
		}

OmnTrace << "Local function usr!" << usr << " " << ip_addr << " " << port << " " << endl;
		// 2. access local data base for result
		AosUserBasedACL4 acc;
		if(uid)
		{
			acc = AosUserBasedACL4(uid, usr, OmnIpAddr(ip_addr), port);
		}
		else
		{
			acc = AosUserBasedACL4(usr, OmnIpAddr(ip_addr), port);
		}

		// 3. reset the transaction message
		msgPtr->resetTags();
	
OmnTrace << "Local function TTTTTTTT!" << __LINE__ << endl;
		// 4. set response transaction message
		OmnString errmsg;
		u16 errcode;
		acc.checkAccess(errcode, errmsg);
		msgPtr->addTag(AosAmTagId::eRsltCode, (u16)errcode);
		msgPtr->addTag(AosAmTagId::eUser, usr);
		msgPtr->addTag(AosAmTagId::eErrmsg, errmsg);

		msgPtr->setMsgId((u8)AosAmMsgId::eResponse);
		trans->setMsg(msgPtr);
	}
	else if(msgId == AosAmMsgId::eUserBasedAccessVPN)
	{
		// User authorization
		// User based access
		u32 		sip_addr;
		u16 		sport;
		u32 		dip_addr;
		u16 		dport;
		u8 			proto;
		if((!msgPtr->getTag(usr, (u8)AosAmTagId::eUser))
		&& (!msgPtr->getTag(uid, (u8)AosAmTagId::eUid)))
		{
			OmnAlarm << "Server failed to get tag information!" << enderr;
			return InternalErrorProcess(trans, usr, errMsg);
		}
OmnTrace << "Local function TTTTTTTT!" << __LINE__ << endl;
		if(!msgPtr->getTag(sip_addr, (u8)AosAmTagId::eSIp))
		{
			OmnAlarm << "Server failed to get tag information!" << enderr;
			return InternalErrorProcess(trans, usr, errMsg);
		}
OmnTrace << "Local function TTTTTTTT!" << __LINE__ << endl;
		if(!msgPtr->getTag(sport, (u8)AosAmTagId::eSPort))
		{
			OmnAlarm << "Server failed to get tag information!" << enderr;
			return InternalErrorProcess(trans, usr, errMsg);
		}
		if(!msgPtr->getTag(dip_addr, (u8)AosAmTagId::eDIp))
		{
			OmnAlarm << "Server failed to get tag information!" << enderr;
			return InternalErrorProcess(trans, usr, errMsg);
		}
OmnTrace << "Local function TTTTTTTT!" << __LINE__ << endl;
		if(!msgPtr->getTag(dport, (u8)AosAmTagId::eDPort))
		{
			OmnAlarm << "Server failed to get tag information!" << enderr;
			return InternalErrorProcess(trans, usr, errMsg);
		}
		if(!msgPtr->getTag(proto, (u8)AosAmTagId::eProto))
		{
			OmnAlarm << "Server failed to get tag information!" << enderr;
			return InternalErrorProcess(trans, usr, errMsg);
		}

		// 2. access local data base for result
		AosUserBasedACVpn acc(usr, OmnIpAddr(sip_addr), sport, 
								OmnIpAddr(dip_addr), dport, proto);
OmnTrace << "Local vpn!" << acc.toString() << endl;

		// 3. reset the transaction message
		msgPtr->resetTags();

OmnTrace << "Local function TTTTTTTT!" << __LINE__ << endl;
		// 4. set response transaction message
		OmnString errmsg;
		u16 errcode;
		acc.checkAccess(errcode, errmsg);
		msgPtr->addTag(AosAmTagId::eRsltCode, (u16)errcode);
		msgPtr->addTag(AosAmTagId::eUser, usr);
		msgPtr->addTag(AosAmTagId::eErrmsg, errmsg);

		msgPtr->setMsgId((u8)AosAmMsgId::eResponse);
		trans->setMsg(msgPtr);
	}
	else if(msgId == AosAmMsgId::eCacheRequest)
	{
		// set response transaction message
OmnTrace << "Local function TTTTTTTT eCacheRequest!" << __LINE__ << endl;

		OmnDbTablePtr table;
		OmnRslt rslt;
		OmnDbRecordPtr record;
		OmnString rcd_buff;
		int i;
		aos_am_vpn_acl_t vpnacl;
		int vpnacl_num,vpnacl_len;
		unsigned char *vpnacl_buff,*vpnacl_buffpos;
		AosUserBasedACVpn acc;
		acc.getAllAcl(table);
OmnTrace << "Local function TTTTTTTT eCacheRequest!" << __LINE__ << endl;
		table->reset();
		vpnacl_num=table->entries();
		vpnacl_len=20*69;
OmnTrace << "vpnacl_len " << vpnacl_len << " number" << vpnacl_num << endl;
		vpnacl_buff = (unsigned char *)malloc(vpnacl_len);
		if(vpnacl_buff==NULL)
		{
			fprintf(stderr,"malloc error\n");
			return(-1);
		}
		vpnacl_buffpos=vpnacl_buff;

		while (table->hasMore())
		{
			for(i=0;i<20;i++)
			{
				if(table->hasMore())
				{
					record = table->next();
					// load value
					vpnacl.mId = record->getU32(0, 0, rslt);
OmnTrace << "vpnacl.mId " << vpnacl.mId << " number" << vpnacl_num << endl;
					vpnacl.mUid = record->getU32(1, 0, rslt);
					strncpy(vpnacl.mUser, record->getStr(2, "NoUser", rslt).data(), 16);
					vpnacl.mWeekday = record->getU32(3, 0, rslt);
					strncpy(vpnacl.mSTime, record->getStr(4, "00:00", rslt).data(), 6);
					strncpy(vpnacl.mETime, record->getStr(5, "00:00", rslt).data(), 6);
					vpnacl.mSip = (u32)atoll(record->getStr(6, "0", rslt).data());
					vpnacl.mSMask = (u32)atoll(record->getStr(7, "0", rslt).data());
					vpnacl.mSSPort = (unsigned short)record->getU32(8, 0, rslt);
					vpnacl.mSEPort = (unsigned short)record->getU32(9, 0, rslt);
					vpnacl.mDip = (u32)atoll(record->getStr(10, "0", rslt).data());
					vpnacl.mDMask = (u32)atoll(record->getStr(11, "0", rslt).data());
OmnTrace << "vpnacl.mSSPort " << vpnacl.mSSPort << endl;
OmnTrace << "vpnacl.mSEPort " << vpnacl.mSEPort << endl;
OmnTrace << "vpnacl.mSip(int) " << (int)vpnacl.mSip << endl;
OmnTrace << "vpnacl.mSMask(int) " << (int)vpnacl.mSMask << endl;
OmnTrace << "vpnacl.mDip(int) " << (int)vpnacl.mDip << endl;
OmnTrace << "vpnacl.mDMask(int) " << (int)vpnacl.mDMask << endl;
					vpnacl.mDSPort = (unsigned short)record->getU32(12, 0, rslt);
					vpnacl.mDEPort = (unsigned short)record->getU32(13, 0, rslt);
OmnTrace << "vpnacl.mDSPort " << vpnacl.mDSPort << endl;
OmnTrace << "vpnacl.mDEPort " << vpnacl.mDEPort << endl;
					vpnacl.mProto = (unsigned short)record->getU32(14, 0, rslt);
					vpnacl.mAllow_deny = record->getChar(15, 'd', rslt);
					vpnacl.mPriority = (unsigned short)record->getU32(15, 0, rslt);

					// set value to memory
					PKG_PUT_NETLONG(vpnacl.mId, vpnacl_buffpos);
					PKG_PUT_NETLONG(vpnacl.mWeekday, vpnacl_buffpos);
					memcpy(vpnacl_buffpos,vpnacl.mSTime,6); vpnacl_buffpos+=6;
					memcpy(vpnacl_buffpos,vpnacl.mETime,6); vpnacl_buffpos+=6;
					memcpy(vpnacl_buffpos,vpnacl.mUser,16); vpnacl_buffpos+=16;
					PKG_PUT_NETLONG(vpnacl.mUid, vpnacl_buffpos);
					memcpy(vpnacl_buffpos, &vpnacl.mSip, 4); vpnacl_buffpos+=4;
					memcpy(vpnacl_buffpos,&vpnacl.mSMask,4); vpnacl_buffpos+=4;
					memcpy(vpnacl_buffpos, &vpnacl.mSSPort, 2); vpnacl_buffpos+=2;
					memcpy(vpnacl_buffpos, &vpnacl.mSEPort, 2); vpnacl_buffpos+=2;
					memcpy(vpnacl_buffpos, &vpnacl.mDip, 4); vpnacl_buffpos+=4;
					memcpy(vpnacl_buffpos,&vpnacl.mDMask,4); vpnacl_buffpos+=4;
					memcpy(vpnacl_buffpos, &vpnacl.mDSPort, 2); vpnacl_buffpos+=2;
					memcpy(vpnacl_buffpos, &vpnacl.mDEPort, 2); vpnacl_buffpos+=2;
					PKG_PUT_NETSHORT(vpnacl.mProto, vpnacl_buffpos);
					PKG_PUT_BYTE(vpnacl.mAllow_deny, vpnacl_buffpos);
					PKG_PUT_NETSHORT(vpnacl.mPriority, vpnacl_buffpos);
				}
				else
				{
					break;
				}
			}
			rcd_buff = OmnString((char *) vpnacl_buff, i * 69);
			vpnacl_buff = 0;
			AosAmMsgPtr msg;
			msg = OmnNew AosAmMsg(*msgPtr);
			msg->resetTags();
			if(!table->hasMore())
			{
				// at the end of the data record 
				msg->addTag(AosAmTagId::eCacheFlag, (u16)0);
			}
			else
			{
				// not at the end of the data record 
				msg->addTag(AosAmTagId::eCacheFlag, (u16)1);
			}
			msg->addTag(AosAmTagId::eCacheRcd, rcd_buff);
			msg->setMsgId((u8)AosAmMsgId::eCacheResponse);
			trans->setMsg(msg);
		}
		free(vpnacl_buff); vpnacl_buff = 0;
	}
	else
	{
		// Unknown Error
		OmnAlarm << "Server unknown error!" << enderr;
		return -eAosRc_AmUnknownError;
	}
	
	return 0;
}


int AmAttExternal 	(const AosAmTransPtr &trans, const AosAccessMgrPtr & amMgr)
{
	// do some thing and set the response message to trans
	
	return 0;
}

// #include <ldap.h>
// dbconnstr: <rootname>:<rootpw>:<organization_name>
int AmAttLdap 		(const AosAmTransPtr &trans, const AosAccessMgrPtr & amMgr)
{
	// 
	// do some thing and set the response message to trans
	// 
	if(!trans)
	{
		return -1; // NULL pointer!!!
	}

	AosAmMsgPtr msgPtr = trans->getMsg();
	if(!msgPtr)
	{
		return -1; // NULL pointer!!!
	}
	// 1. get the input attributes first
	OmnString app;
	OmnString usr;
	OmnString opr;
	u8 	msgId;
	OmnString rscPath;
	OmnString pwd;
	OmnString errMsg;
	OmnString denyReason;

OmnTrace << "Ldap function TTTTTTTT!" << __LINE__ << endl;
	msgId = msgPtr->getMsgId();
	AosAmExternalSvrPtr svrPtr = trans->getExternalSvr();
	if(msgId == AosAmMsgId::eUserAuth)
	{
/*		char 		*LdapSvrConnStr;
		char 		*saveptr;			// *token, 
		char 		*uid, *organization_name, *rootpw;
		char 		tmp[256], tmppw[128];
		int 		find_entry = 0;

		//  LDAP handler 
		LDAP 		*ld;
		LDAPMessage *res, *e;
		// User authentication
		if(!msgPtr->getTag(usr, (u8)AosAmTagId::eUser))
		{
			OmnAlarm << "Server failed to get tag information!" << enderr;
			return InternalErrorProcess(trans, usr, errMsg);
		}
		if(!msgPtr->getTag(pwd, (u8)AosAmTagId::ePasswd))
		{
			OmnAlarm << "Server failed to get tag information!" << enderr;
			return InternalErrorProcess(trans, usr, errMsg);
		}

		// 2. access LDAP data base for result
		// open a connection 
		if ( (ld = ldap_init( svrPtr->getServerIp(), svrPtr->getServerPort() )) == NULL )
		{
			OmnAlarm << "Server failed connect to LDAP server! usr[" 
					 << usr << "] pwd[" << pwd << "] IP[" 
					 << svrPtr->getServerIp() << "] port[" << svrPtr->getServerPort() << "]!" << enderr;
			return InternalErrorProcess(trans, usr, errMsg);
		}
		// authenticate as somebody 
		LdapSvrConnStr = svrPtr->getConnStr();
		uid = strtok_r(LdapSvrConnStr, ":", &saveptr);
		rootpw = strtok_r(NULL, ":", &saveptr);
		organization_name = strtok_r(NULL, ":", &saveptr);
		snprintf(tmp, 256, "cn=%s,%s", uid, organization_name);
		if ( ldap_simple_bind( ld, tmp, rootpw ) <= 0 ) 
		{
			OmnAlarm << "Connect to LDAP server! usr[" 
					 << usr << "] pwd[" << pwd << "] IP[" 
					 << svrPtr->getServerIp() << "] port[" << svrPtr->getServerPort() << "]!" << enderr;
			OmnAlarm << "Fail to bind the connection! user[" << tmp << "] rootpw[" << rootpw << "]" << enderr;
			return InternalErrorProcess(trans, usr, errMsg);
		}

		// search for entries with cn of "Babs Jensen",
		//        return all attrs  
		snprintf(tmp, 256, "uid=%s,%s", usr.data(), organization_name);
		snprintf(tmppw, 128, "(userPassword=%s)", pwd.data());
		if ( ldap_search_s( ld, tmp,
		    LDAP_SCOPE_SUBTREE, tmppw, NULL, 0, &res )
		    != LDAP_SUCCESS ) 
		{
			ldap_perror( ld, "ldap_search_s" );
			OmnAlarm << "Fail to bind the connection!" << enderr;
			return InternalErrorProcess(trans, usr, errMsg);
		}

		// step through each entry returned 
		for ( e = ldap_first_entry( ld, res ); e != NULL;
		    e = ldap_next_entry( ld, e ) ) 
		{
			// find the entry
			find_entry = 1;
		}
		// free the search results 
		ldap_msgfree( res );
		// close and free connection resources 
		ldap_unbind( ld );

		// 3. reset the transaction message
		msgPtr->resetTags();

		// 4. set response transaction message
		OmnString errmsg;
		u16 errcode;
		if(find_entry)
		{
			u16 sessionId;
			u32 uid = 0;
			if(amMgr->addUser(sessionId, usr))
			{
				msgPtr->setSessionId(sessionId);
				// success
				msgPtr->addTag(AosAmTagId::eRsltCode, (u16)0);
				msgPtr->addTag(AosAmTagId::eUser, usr);
				msgPtr->addTag(AosAmTagId::eUid, (u32)uid);
			}
			else
			{
				errmsg << "Fail to add user session. Internal Error" << "\n";
				OmnAlarm << errmsg << enderr;
				// fail
				msgPtr->addTag(AosAmTagId::eRsltCode, (u16)AosAmRespCode::eInternalError);
				msgPtr->addTag(AosAmTagId::eUser, usr);
				msgPtr->addTag(AosAmTagId::eErrmsg, errmsg);
			}
		}
		else
		{
			// denied
			msgPtr->addTag(AosAmTagId::eRsltCode, (u16)errcode);
			msgPtr->addTag(AosAmTagId::eUser, usr);
			msgPtr->addTag(AosAmTagId::eErrmsg, errmsg);
		}
		msgPtr->setMsgId((u8)AosAmMsgId::eResponse);
		trans->setMsg(msgPtr);
 */	}
	else if(msgId == AosAmMsgId::eUserBasedAccess)
	{
	}
	else if(msgId == AosAmMsgId::eUserBasedAccessL4)
	{
	}
	else if(msgId == AosAmMsgId::eUserBasedAccessVPN)
	{
	}
	else
	{
		// Unknown Error
		OmnAlarm << "Server unknown error!" << enderr;
		return -eAosRc_AmUnknownError;
	}

	return 0;
}

int AmAttRadius 		(const AosAmTransPtr &trans, const AosAccessMgrPtr & amMgr)
{
	// do some thing and set the response message to trans
	
	return 0;
}

int AmAttActivedirectory(const AosAmTransPtr &trans, const AosAccessMgrPtr & amMgr)
{
	// do some thing and set the response message to trans
	
	return 0;
}

int AmAttReserved 	(const AosAmTransPtr &trans, const AosAccessMgrPtr & amMgr)
{
	// do some thing and set the response message to trans
	
	return 0;
}
