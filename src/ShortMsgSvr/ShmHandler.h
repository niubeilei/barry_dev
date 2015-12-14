////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 06/23/2011 Created by Brian Zhang 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_ShortMsgSvr_ShmHandler_h
#define AOS_ShortMsgSvr_ShmHandler_h

#include "Rundata/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "ShortMsgSvr/ShortMsgSvr.h"
#include "ShortMsgUtil/ShmReqids.h"
#include "ShortMsgUtil/ShmHandlerId.h"
#include "Util/String.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"



class AosShmHandler : virtual public OmnRCObject
{
	OmnDefineRCObject;

protected:
	AosShmHandlerId::E			mId;
	OmnString		 			mReqidName;

private:
	AosShmHandlerPtr			mPrev;
	AosShmHandlerPtr			mNext;
	
public:
	AosShmHandler(
			const OmnString &name,
			const AosShmHandlerId::E &id,
			const bool flag);
	~AosShmHandler();
	
	virtual bool procMsg(
						const OmnString &sender,
						const OmnString &modem_cellnum, 
						const OmnString &msg, 
						bool &stop_flag,
						bool &remove_flag,
						const AosRundataPtr &rdata) = 0;
	bool 		sendRespToSender(
				const OmnString &msg,
				const OmnString &receiver,
				const AosRundataPtr &rdata)
	{
		OmnString docstr = "<request ";
		docstr << AOSTAG_OPERATOR << "=\""
		    << AOSSHORTMSGREQ_SENDMSG << "\" "
		    << AOSTAG_SHM_RECEIVERS << "=\"" << receiver << "\" >"
		    << "<msg><![CDATA[" << msg << "]]></msg>"
		    << "</request>";
		AosXmlParser parser;
		AosXmlTagPtr xml = parser.parse(docstr, "" AosMemoryCheckerArgs);
		if (!xml)
		{
			AosSetError(rdata, AosErrmsgId::eParseXmlFailed);
		    OmnAlarm << rdata->getErrmsg()
				<< ". Doc: " << docstr << enderr;
		    return false;
		}
		bool rslt = AosShortMsgSvr::getSelf()->proc(xml, rdata);
		if (!rslt)
		{
			AosSetError(rdata, AosErrmsgId::eSendShortMsgFailed);
			OmnAlarm << rdata->getErrmsg() 
				<< ". sender: " << receiver 
				<< ". Contents: " << msg << enderr;
			return false;
		}
		return true;
	}
	AosShmHandlerPtr	getNext() const {return mNext;}
	AosShmHandlerPtr	getPrev() const {return mPrev;}
	void				setPrev(const AosShmHandlerPtr &p) {mPrev = p;}
	void				setNext(const AosShmHandlerPtr &n) {mNext = n;}
	void resetPointers() 
	{
		AosShmHandlerPtr thisptr(this, false);
		mNext = thisptr;
		mPrev = thisptr;
	}

	void removeFromList()
	{
		// This function removes this node from the list. 
		//AosShmHandlerPtr hh = handler->getNext();
		this->mPrev->mNext = this->mNext;
		this->mNext->mPrev = this->mPrev;
	}

	void appendToList(const AosShmHandlerPtr &head)
	{
		// This function appends itself to the list. 
		AosShmHandlerPtr thisptr(this, false);
		AosShmHandlerPtr tail = head->mPrev;
		thisptr->mNext = head;
		head->mPrev = thisptr;
		thisptr->mPrev = tail;
		tail->mNext = thisptr;
	}
	bool	registerSeProc(const AosShmHandlerPtr &proc);
	static AosShmHandlerPtr	getProc(const OmnString &idstr);
	void				createLog(
						const OmnString &receiver,
						const OmnString &msg,
						const AosRundataPtr &rdata);
};
#endif

