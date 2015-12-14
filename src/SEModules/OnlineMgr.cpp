////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// Online Management
// 1. We use an IIL to manage all user's online status. The IIL name
//    is AOSZTG_ONLINESTATUS + siteid. This is a u64 IIL. The value
//    is the user's docid. The docid portion is organized as:
//    	Bit 63:		0:offline, 1:online
//    	Bit 31-0:	The online time
//   
// 2. Buddy Management
//    Each user defines a number of buddy lists. When a user goes online,
//    the user needs to download the status of all the friends defined
//    in the buddy list. There is an IIL that lists all the buddy lists 
//    of a given user. 
//
//    	AOSZTG_BUDDYLIST + siteid
//
//    The value portion is user docid. The docid portion is the docid of 
//    the buddy list obj. 
//
//    There is another IIL that defines all the buddy list:
//    	AOSZTG_BUDDY_MEMBER + siteid
//
//    The value portion is the docid of a buddy list, and the docid portion
//    is the docid of a user in the buddy list. 
//
//
// Modification History:
// 08/16/2010: Created by James
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SEModules/OnlineMgr.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "UtilComm/TcpClient.h"
#include "SEInterfaces/IILClientObj.h"
#include <time.h>



OmnSingletonImpl(AosOnlineMgrSingleton,
                 AosOnlineMgr,
                 AosOnlineMgrSelf,
                "AosOnlineMgr");


AosOnlineMgr::AosOnlineMgr()
:
mLock(OmnNew OmnMutex()),
mIsStopping(false)
{
}


AosOnlineMgr::~AosOnlineMgr()
{
}

bool      	
AosOnlineMgr::start()
{
	return true;
}


bool        
AosOnlineMgr::stop()
{
	mIsStopping = true;
	return true;
}


bool
AosOnlineMgr::config(const AosXmlTagPtr &def)
{
	return true;
}

bool 
AosOnlineMgr::procLogin(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &root, 
		const u64 &userid,
		const OmnString &siteid,
		OmnString &buddyStatus,
		const AosRundataPtr &rdata)
{
	// A user has logged in successfully. We need to update the User Login
	// Status IIL. In addition, it will retrieve the online status of 
	// all the buddies of the user. The results are returned through
	// 'onlineInfo', which has the following format:
	// 	<online>username:onlinename:status;username:onlinename:status...</online>
	updateOnlineInfo(siteid, userid, eOnline, rdata);
	//retrieveBuddyStatus(siteid, userid, buddyStatus);
	return true;
}


bool 
AosOnlineMgr::procLogout(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &root, 
		const u64 &userid,
		const OmnString &siteid,
		const AosRundataPtr &rdata)

{
	updateOnlineInfo(siteid, userid, eOffline, rdata);
	return true;
}


bool 
AosOnlineMgr::procOnlineHeartbeat(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &root, 
		const u64 &userid)
{
	// Update the user online status
	//updateOnlineInfo(userid, eOnline);
	return true;
}


bool
AosOnlineMgr::updateOnlineInfo(
		const OmnString &siteid,
		const u64 &userid, 
		const int status,
		const AosRundataPtr &rdata)
{
	/* 
	 * Chen Ding, 08/12/2011
	// The online info is managed by the IIL:
	// 	AOSZTG_ONLINESTATUS + siteid
	// 	"online_status"
	OmnString iilname;
	//iilname << AOSZTG_ONLINESTATUS << "." << siteid;
	iilname << "onlinestt1.100";

	time_t t = time(NULL);
	u64 crttime = t;
	u64 value = value & 0;                                                                                       
    value = crttime;
    value = value << 32;
    value = value | status;
	// Chen Ding, 05/27/2011
	// return AosIILClientObj::getIILClient()->updateKeyedValue(iilname, true, userid, value);
	// Chen Ding, 06/04/2011
	return AosIILClientObj::getIILClient()->setU64ValueDocUnique(iilname, userid, value, rdata);
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosOnlineMgr::CheckOnlineStatus(
			const u64 &userid,
			OmnString &tm,
			u32	  &status, 
			const AosRundataPtr &rdata)
{
	//1. retrieve the status iil.
	//2. search status by userid.
	//3. status: 0 offline, 1 online, ...
	//	if no status, is offline.
	//if (!mOnlineIIL) return false;

	// int idx = -10;
	// u64 vv;
	//  mOnlineIIL->nextDocidSafe(idx, iilidx, false, eAosOpr_eq, uid, vv, isunique, rdata);
	// if (vv == AOS_INVDID || idx < 0)
	// {
	// 	status = 0;
	// 	return true;
	// }
	// 	
	// u32 tt = (vv >> 32);
	// getTime((time_t)tt, tm);
	// status = (u32)vv;
	// return true;
	OmnNotImplementedYet;
	return false;
}


bool 
AosOnlineMgr::getTime(time_t tt, OmnString &stm)
{
     struct tm *p;
     p = gmtime(&tt);
     if (!p) return false;
     int hh = (p->tm_hour + 8)%24;
     stm << 1900+p->tm_year << "-" << 1+p->tm_mon << "-" << p->tm_mday 
         << "  " << hh << ":" << p->tm_min << ":" << p->tm_sec;
     return true;
}


//add friend to iil.
bool 
AosOnlineMgr::addFriend(
		OmnString &errmsg,
		const OmnString &iilname,
		const u64 &userid,
		const u64 &fid,
		const OmnString &siteid,
		bool flag)
{
#if 0
	// This function is not used anymore. Check with Joshi.
	// Chen Ding, 06/04/2011
	bool rslt;
	if (flag)
	{
		u64 iilid = 0;
		// 1. get buddy member iil. Set the value [fid, userid] and 
		// [userid, fid]. 
		bool rslt = AosIILClientObj::getIILClient()->updateDoc4(
				iilname, true, fid, userid, userid, fid, iilid);	
		if (!rslt)
		{
			 errmsg = "add friend failed.";
			 return false;
		}
		
		//4.add buddy member list to master iil.
		OmnString buddylist;
		//buddylist << AOSZTG_BUDDYLIST << "." << siteid;
		buddylist << "buddylist3.100";
		u64 iilid2 = 0;

		// Add the two entries: [iilid, userid], [iilid, fid]
		rslt = AosIILClientObj::getIILClient()->updateDoc4(
				buddylist, true, iilid, userid, iilid, fid, iilid2);	
		if (!rslt)
		{
			 errmsg = "add friend failed.";
			 return false;
		}
	}

	//5. remove Doc by apply iil.
	//AOSZTG_APPLY_BUDDY  : applybuddy.
	OmnString applylist;
	//applylist << AOSZTG_APPLY_BUDDY << "." << siteid;
	applylist << "buddyapply.100";
	u64 uid = userid;
	rslt = AosIILClientObj::getIILClient()->removeValueDoc(applylist,uid,fid);
	aos_assert_r(rslt, false);
	return true;
#endif
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosOnlineMgr::pullOnlineMsg(
		const u64 &userid,
		const AosXmlTagPtr &root, 
		OmnString &contents,
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	AosOnlineUserPtr user = getUser(userid, true);
	if (!user)
	{
		errcode = eAosXmlInt_General;
		errmsg = "Failed to create user!";
		return false;
	}
	bool istimeout = false;
	user->readMsg(contents, istimeout);
	if (!istimeout)
		errcode = eAosXmlInt_Ok;
	return true;
}


bool
AosOnlineMgr::sendOnlineMsg(
		const u64 &userid, 
		const OmnString &msg)
{
	// This function checks whether the user 'userid' is online. 
	// If yes, it sends the message 'msg' to that user. The caller
	// should have formated the message. This function treats the
	// message as a string.
	if (userid != 0)
	{
		AosOnlineUserPtr user = getUser(userid, false);
		if (!user)
		{
		// The user is not online. Need to store the message. 
		OmnNotImplementedYet;
		return false;
		}	
		user->sendMsg(msg);
	}
	else
	{
		//send message to all users.
		hash_map<int, AosOnlineUserPtr>::iterator item = mUsers.begin(); 
		while (item != mUsers.end())
		{
			item->second->sendMsg(msg);
			item++;
		}
	
	}
	return true;
}

AosOnlineUserPtr
AosOnlineMgr::getUser(
		const u64 &userid, 
		bool needJoin)
{
	//get online user by userid,
	//if not exist, create a new one.
	int key = (int) userid;
	hash_map<int, AosOnlineUserPtr>::iterator user = mUsers.find(key);
	if (user != mUsers.end())
		return (AosOnlineUserPtr)user->second;
	else if (needJoin)
	{
		AosOnlineUserPtr userptr = OmnNew AosOnlineUser();
		mUsers.insert(make_pair(key, userptr));
		return userptr;
	}
	else
		return NULL;
}
#endif
