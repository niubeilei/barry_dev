////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Trans.h
// Description:
//	This is the super class for transactions.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "IILServer/IILProcAdd.h"
#include "TransServer/TransMgr.h"

#include "IILMgr/IILMgr.h"
#include "XmlInterface/XmlRc.h"
#include "Porting/Sleep.h"



AosIILProcAdd::AosIILProcAdd(const bool reg_flag)
{
	if(reg_flag)
	{
		registerProc(this);
	}
}

AosIILProcAdd::~AosIILProcAdd()
{
}


bool 
AosIILProcAdd::doTrans(const AosTransPtr &trans)
{
	// This function adds an entry into an IIL.
	// It first uses IILMgr to add the entry into the IIL
	// (in memory). Once finished, it will save the 
	// changes back to the files.
	//
	// 'trans' contains the operation def, an XML doc, 
	// which is in the form:
	//  <trans AOSTAG_TRANID="xxx"
	//        AOSTAG_TRANS_OPERATION="xxx"
	//        AOSTAG_DOCID="xxx"
	//        AOSTAG_TRANSTIMER="xxx">
	//        word
	//  </trans>
	// If priority is high, it should save the changes 
	// to files immediately. Otherwise, saving will be
	// handled through another thread.
	
	OmnScreen << "--------------------------------hello in IILAdd proc---------------------" << endl;
	AosXmlTagPtr doc = trans->getTrans();
	OmnString word = doc->getNodeText();
	u64 did = doc->getAttrU64(AOSTAG_DOCID, 0);
	if (!AosIILMgrSelf->addDoc(word, word.length(), did))
	{
		//Update the transaction status;
		trans->setStatus(AosTrans::eTransProcessed);
		trans->setResponse("addDoc Error!");
		
		AosXmlRc errcode = eAosXmlInt_InternalError;
		OmnString errmsg;
		AosTransMgr::getSelf()->finishTransInMemory(trans, errcode, errmsg);
		OmnAlarm << "IILServer addDoc Error!" << enderr;
	}
	else
	{
		//Update the transaction status;
		trans->setStatus(AosTrans::eTransFailed);
		
		AosXmlRc errcode = eAosXmlInt_Ok;
		OmnString errmsg;

		AosTransMgr::getSelf()->finishTransInMemory(trans, errcode, errmsg);
	}
	return true;
}

