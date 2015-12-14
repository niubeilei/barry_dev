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
// All requests are stored in mRequests. When there are too many, it will 
// stop adding requests to mRequests. When mRequests are empty, it checks
// whether there are additional requests from the log file. If yes, it
// reads in all the requests to mRequests. 
//
// Modification History:
// 06/24/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ShortMsgSvr/ShortMsgSvr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Rundata/Rundata.h"
#include "SEUtil/DocTags.h"
#include "ShortMsgSvr/ShmReqProc.h"
#include "ShortMsgSvr/Ptrs.h"
#include "ShortMsgSvr/GsmMgr.h"
#include "ShortMsgUtil/GsmModem.h"
#include "ShortMsgUtil/Ptrs.h"
#include "ShortMsgSvr/ShmReqProc.h"
#include "ShortMsgSvr/ShmHandler.h"
#include "ShortMsgUtil/SmsProc.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "TransServer/TransServer.h"
//#include "TransServer/TransSvrConn.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/StrSplit.h"


OmnSingletonImpl(AosShortMsgSvrSingleton,
                 AosShortMsgSvr,
                 AosShortMsgSvrSelf,
                "AosShortMsgSvr");

extern int gAosLogLevel;

// Static member data
AosShortMsgSvr::HandlerMap 			AosShortMsgSvr::mSenderHandlerMap;
AosShortMsgSvr::HandlerMap 			AosShortMsgSvr::mModemHandlerMap;
static OmnMutex             		sgLock;


AosShortMsgSvr::AosShortMsgSvr()
{
}


AosShortMsgSvr::~AosShortMsgSvr()
{
}


bool
AosShortMsgSvr::start()
{
	return true;
}


bool
AosShortMsgSvr::stop()
{
	return true;
}

	
bool
AosShortMsgSvr::config(const AosXmlTagPtr &config)
{
	// init all Gsm
	// <Gsm is_simulating="true|false">
	// 	  <gsm smsp="xxx" mbln="xxxx" runner="xxx" arcd="xxx"/>
	// 	  .......
	// </Gsm>
	aos_assert_r(config, false);
	AosXmlTagPtr smssvr = config->getFirstChild("ShortMsgSvr");
	if (smssvr)
	{
		//distribute
		AosXmlTagPtr transConfig = smssvr->getFirstChild(AOSCONFIG_TRANS);
		aos_assert_r(transConfig, false);
	}
	if (smssvr)
	{
		AosXmlTagPtr gsm_config = smssvr->getFirstChild("Gsm");
		if (!gsm_config)
		{
			OmnAlarm << "Failed retrieving the GSM configuration" << enderr;
			return false;
		}
		
		mIsSimulate = gsm_config->getAttrBool("simulate",false);
		
		mGsmMgr = OmnNew AosGsmMgr(gsm_config);
		if (!mGsmMgr)
		{
			OmnAlarm << "Failed to init GSM MODEM Manager" << enderr;
			return false;
		}
	}
	return true;
}


void
AosShortMsgSvr::closeGsmMgr()
{
	mGsmMgr->close();
}


bool
AosShortMsgSvr::proc(
		const AosXmlTagPtr &data,
		const AosRundataPtr &rdata)
{
	// This function is called when the underline transporting layer
	// receives a request. The request is in 'request', which is in the
	// following format:
	// 	<request opr="xxx" ...>
	// 		...
	// 	</request>
	
	// Ketty 2013/03/25
	OmnNotImplementedYet;
	return false;
	aos_assert_r(data, false);

	OmnString opr = data->getAttrStr(AOSTAG_OPERATOR);
	AosShmReqProcPtr proc = AosShmReqProc::getProc(opr);
	if (!proc)
	{
		rdata->setError() << "Failed retrieving the proc: " << data->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		sendResponse(false, rdata);
		return false;
	}

	rdata->setReceivedDoc(data, false);
	bool rslt = proc->proc(rdata);
	if (!rslt)
	{
		OmnAlarm << "Failed to process proc!" << enderr;
		sendResponse(rslt, rdata);
		return false;
	}
	sendResponse(rslt, rdata);
	return true;
}


AosGsmModemPtr
AosShortMsgSvr::getGsmModem(const OmnString &key)
{
	return mGsmMgr->getGsmModem(key);
}


void 
AosShortMsgSvr::sendResponse(
		//const AosTrans1Ptr &trans,
		const bool &rslt,
		const AosRundataPtr &rdata)
{
	OmnString resp = "<response><status error=\"";
	if (rslt)
	{
		resp << "false\" code=\"200\" />";
	}
	else
	{
		resp << "true\" ><![CDATA[" << rdata->getErrmsg() << "]]></status>";
	}

	if (rdata->getResults() != "")
	{
		resp << rdata->getResults();
	}
	resp << "</response>";
	rdata->setResults(resp);
}


bool 
AosShortMsgSvr::shortMsgReceived(
		const OmnString &msg, 
		const OmnString &sender, 
		const OmnString &modem_cellnum,
		const AosRundataPtr &rdata)
{
	// A short message was received from 'sender' through 'local_phnum'. 
	// It looks up the map
	OmnScreen << "message received: " << msg << endl;
	static OmnString oriStr[] = {"〈", "〉", "﹠", "，"};	
	static OmnString ndStr[] = {"<", ">", "&", ","};

	OmnString str = msg;
	str.replace(oriStr[0], ndStr[0], true);
	str.replace(oriStr[1], ndStr[1], true);
	str.replace(oriStr[2], ndStr[2], true);
	str.replace(oriStr[3], ndStr[3], true);
	
	AosShmHandlerPtr handler = getHandler(str, rdata);
	bool stop, delete_flag;
	if (handler)
	{
		handler->procMsg(sender, modem_cellnum, str, stop, delete_flag, rdata);
	}
	return true;

	/*
	// now we change the way to get handler.
	sgLock.lock();
	HandlerMapItr itr = mSenderHandlerMap.find(sender);
	bool stop_flag = false;
	//AosShmHandlerPtr handler;
	if (itr != mSenderHandlerMap.end())
	{
		handler = itr->second;
		if (!handler)
		{
			rdata->setError() << "The handler is null: " << sender
				<< ":" << modem_cellnum;
			OmnAlarm << rdata->getErrmsg() << enderr;
		}
		else
		{
			processHandlers(sender, modem_cellnum, msg, handler, stop_flag, true, rdata);
			if (stop_flag) 
			{
				sgLock.unlock();
				return rdata->isOk();
			}
		}
	}

	// The message was not consumed yet. Need to check the modem handler.
	itr = mModemHandlerMap.find(modem_cellnum);
	if (itr == mModemHandlerMap.end())
	{
		// No handler for this cell message. Ignore it.
		sgLock.unlock();
		rdata->setOk();
		return true;
	}

	// There are handlers for this modem.
	handler = itr->second;
	if (!handler)
	{
		rdata->setError() << "The handler is null: " << sender
			<< ":" << modem_cellnum;
		OmnAlarm << rdata->getErrmsg() << enderr;
		sgLock.unlock();
		return false;
	}

	processHandlers(sender, modem_cellnum, msg, handler, stop_flag, false, rdata);
	sgLock.unlock();
	return rdata->isOk();
	*/
}


bool
AosShortMsgSvr::processHandlers(
		const OmnString &sender, 
		const OmnString &modem_cellnum, 
		const OmnString &msg, 
		AosShmHandlerPtr &handler, 
		bool &stop_flag,
		const bool remove_sender,
		const AosRundataPtr &rdata)
{
	bool remove_flag;
	int guard = eMaxHandlers;
	AosShmHandlerPtr head = handler;
	do
	{
		stop_flag = false;
		remove_flag = false;
		if (!handler->procMsg(sender, modem_cellnum, msg, stop_flag, remove_flag, rdata))
		{
			OmnAlarm << "Failed processing: " << rdata->getErrmsg() << enderr;
		}

		if (remove_flag)
		{
			// Need to remove the handler. Check whether the list contains only
			// one entry. If yes, it needs to remove the map entry. Otherwise, 
			// it should remove the entry from the list. 
			if (head->getNext() == head)
			{
				// It needs to remove the map entry.
				if (remove_sender)
				{
					mSenderHandlerMap.erase(sender);
				}
				else
				{
					mModemHandlerMap.erase(modem_cellnum);
				}
				break;
			}
			
			// It needs to remove the entry from the list.
			handler->removeFromList();
			AosShmHandlerPtr hh = handler->getNext();
			handler = hh;
		}
		else
		{
			handler = handler->getNext();
		}
	} while (handler != head && guard-- && !stop_flag);

	if (guard == 0)
	{
		rdata->setError() << "Infinite loop is found: " << eMaxHandlers
			<< ":" << sender << ":" << modem_cellnum
			<< ":" << msg;
		return false;
	}
	rdata->setOk();
	return true;
}


bool
AosShortMsgSvr::addHandler(
			const OmnString &sender,
			const AosShmHandlerPtr &req, 
			AosShortMsgSvr::HandlerMap &map)
{
	// This function add a receiver message req into 
	// the map.
	aos_assert_r(req, false);
	
	sgLock.lock();
	HandlerMapItr itr = map.find(sender);
	if (itr == map.end())
	{
		// There is no such a map yet. Create it.
		req->resetPointers();
		map[sender] = req;
		sgLock.unlock();
		return true;
	}

	// There is already a map. Append it to the list. 
	AosShmHandlerPtr head = itr->second;
	req->appendToList(head);
	sgLock.unlock();
	return true;
}


AosShmHandlerPtr
AosShortMsgSvr::getHandler(
		const OmnString &msg,
		const AosRundataPtr &rdata)
{
	// msg should be this format:
	// <&>opr<&>xxx<&>xxxx<&>
	AosStrSplit split(msg, "<&>");
	if (split.entries() < 2) return NULL;
	OmnString idstr = split.nextWord();
	if (idstr != "")
	{
		AosSetError(rdata, AosErrmsgId::eMissingFieldname);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}
	idstr = split.nextWord();
	if (idstr == "")
	{
		AosSetError(rdata, AosErrmsgId::eMissingFieldname);
		OmnAlarm << rdata->getErrmsg() << ". Message: " << msg << enderr;
		return 0;
	}
	return AosShmHandler::getProc(idstr);
}
