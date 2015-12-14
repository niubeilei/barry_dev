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
// 01/28/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILTrans/IILTransTester.h"

#include "TransUtil/IILTrans.h"
#include "Thread/Mutex.h"

// Ketty 2013/03/20
#if 0
// Static member data
vector<u64> *	AosIILTransTester::smDocids[AosIILTransTester::eMaxIILID];
u32 			AosIILTransTester::smLastSeqno[AosIILTransTester::eMaxIILID];
AosU642U64_t 	AosIILTransTester::smIILID2DistIDMap;
AosU642U32_t 	AosIILTransTester::smIILID2SeqnoMap;
AosXmlTagPtr	AosIILTransTester::smDocs[AosIILTransTester::eMaxDocTrans];
AosIILTransTester::TransMap_t	AosIILTransTester::smRecvTrans;
AosIILTransTester::TransMap_t	AosIILTransTester::smSentTrans;
AosIILTransTester::TransMap_t	AosIILTransTester::smIILID2TransidMap;
vector<AosIILTransTester::Opr> *AosIILTransTester::smOprs[AosIILTransTester::eMaxIILID];

static OmnMutex sgLock;
static bool sgInited = false;

void
AosIILTransTester::init()
{
	sgLock.lock();
	if (sgInited)
	{
		sgLock.unlock();
		return;
	}
	memset(smDocids, 0, sizeof(smDocids));
	memset(smOprs, 0, sizeof(smOprs));
	memset(smLastSeqno, 0, sizeof(smLastSeqno));
	sgInited = true;
	sgLock.unlock();
}


bool
AosIILTransTester::addDoc(const u64 &iilid, AosIILTrans *trans)
{
	if (!sgInited) init();
	if (iilid >= eMaxIILID) return true;
	u32 seqno = trans->getRecvSeqno();
	if (seqno == 0) return true;
	u64 docid = trans->getIILTestDocid();
	if (docid == 0) return true;

	sgLock.lock();
	if (!smDocids[iilid]) 
	{
		// smDocids[iilid] = new vector<u64>();
		// smOprs[iilid] = new vector<Opr>();
		// smLastSeqno[iilid] = eInitRecvSeqno-1;
		OmnAlarm << "Internal Error" << enderr;
		sgLock.unlock();
		return false;
	}
	// if (seqno != smLastSeqno[iilid]+1)
	// {
	// 	OmnAlarm << "Seqno incorrect: " << seqno << ":" << smLastSeqno[iilid] << enderr;
	// }

	smLastSeqno[iilid]++;
	smDocids[iilid]->push_back(docid);
	smOprs[iilid]->push_back(eAdd);
	sgLock.unlock();
	return true;
}


bool
AosIILTransTester::removeDoc(const u64 &iilid, AosIILTrans *trans)
{
	if (!sgInited) init();
	if (iilid >= eMaxIILID) return true;
	u32 seqno = trans->getRecvSeqno();
	if (seqno == 0) return true;

	u64 docid = trans->getIILTestDocid();
	if (docid == 0) return true;

	sgLock.lock();
	if (!smDocids[iilid]) 
	{
		OmnAlarm << "Transaction incorrect: " << iilid << enderr;
	}
	else
	{
		smDocids[iilid]->push_back(docid);
		smOprs[iilid]->push_back(eRemove);
		if (seqno != smLastSeqno[iilid]+1)
		{
			OmnAlarm << "Seqno incrrect: " << seqno << ":" << smLastSeqno[iilid]
				<< ":" << iilid << enderr;
			smLastSeqno[iilid] = seqno;
		}
		else
		{
			smLastSeqno[iilid]++;
		}
	}
	sgLock.unlock();
	return true;
}


bool
AosIILTransTester::transReceived(const u64 &iilid, AosIILTrans *trans)
{
	// An IIL transaction is received by IILTransServer. The transaction
	// is added to IILMgr. This function is called by IILMgr. This class
	// maintains an IILID to DistId map. 'DistId' is used by the client
	// to determine how to route an IIL request. Once received by the server,
	// the server will resolve it into the corresponding IILID. 
	//
	// 1. It first resolves the IILID into the DiskID. If not created yet, 
	//    it will create it.
	// 2. It assigns a seqno to the transaction. When procesing the transactions
	//    of the same IIL, the seqnos can be used to ensure that they were
	//    processed in the order in which they were received.
	if (!sgInited) init();
	if (iilid >= eMaxIILID) return true;
	u64 docid = trans->getIILTestDocid();
	if (docid == 0) 
	{
		if (trans->getOperation() == AosIILFuncType::eHitRemoveDocByName ||
			trans->getOperation() == AosIILFuncType::eHitAddDocByName)
		{
			OmnAlarm << "Internal error" << enderr;
		}
		return true;
	}

	sgLock.lock();
	AosU642U64Itr_t itr = smIILID2DistIDMap.find(iilid);
	u64 distid = trans->getDistId();
	if (itr == smIILID2DistIDMap.end())
	{
		// Not added to the map yet, add it.
		smIILID2DistIDMap[iilid] = distid;
	}
	else
	{
		if (distid != itr->second)
		{
			OmnAlarm << "Distid incorrect: " << iilid << ":" << distid
				<< ":" << itr->second << enderr;
		}
	}

	AosU642U32Itr_t itr2 = smIILID2SeqnoMap.find(iilid);
	u32 seqno = eInitRecvSeqno;
	if (itr2 == smIILID2SeqnoMap.end())
	{
		// The entry has not been created yet.
		smIILID2SeqnoMap[iilid] = eInitRecvSeqno;
//OmnScreen << "Add iilid: " << iilid << endl;
	}
	else
	{
//OmnScreen << "iilid Entry exist: " << iilid << endl;
		seqno = ++(itr2->second);
	}

	if (!trans->setRecvSeqno(seqno))
	{
		OmnAlarm << "In testing the IIL Trans but 'setRecvSeqno(...)' not defined!" << enderr;
	}

	if (!smDocids[iilid]) 
	{
		if (seqno != eInitRecvSeqno)
		{
			OmnAlarm << "Internal error!" << enderr;
		}

//OmnScreen << "Add vector: " << iilid << endl;
		smDocids[iilid] = OmnNew vector<u64>();
		smOprs[iilid] = OmnNew vector<Opr>();

		smDocids[iilid]->push_back(docid);
		smOprs[iilid]->push_back(eReceived);
		smLastSeqno[iilid] = eInitRecvSeqno-1;
	}
	else
	{
//OmnScreen << "Vector exist: " << iilid << endl;
	}

	// Retrieve the transaction id and add it to 'smRecvTrans'
	u64 transid = trans->getTransId();
	u64 srcid = (transid >> 24);
	TransMapItr_t transitr = smRecvTrans.find(srcid);
	vector<u64> *vec;
	if (transitr == smRecvTrans.end())
	{
		vec = OmnNew vector<u64>();
		smRecvTrans[srcid] = vec;
	}
	else
	{
		vec = transitr->second;
	}
	vec->push_back(transid);

	// Check whether there is one in the senttrans.
	/*
	transitr = smSentTrans.find(srcid);
	if (transitr == smSentTrans.end())
	{
		OmnAlarm << "Failed finding the trans: " << transid << enderr;
	}
	else
	{
		vec = transitr->second;
		bool found = false;
		for (u32 i=0; i<vec->size(); i++)
		{
			if ((*vec)[i] == transid)
			{
				(*vec)[i] = iilid;
				found = true;
				break;
			}
		}
		if (!found)
		{
			OmnAlarm << "Failed find the trans: " << transid << enderr;
		}
	}
	*/

	// Add it to the IILID trans vector
	TransMapItr_t itr3 = smIILID2TransidMap.find(iilid);
	vec = 0;
	if (itr3 == smIILID2TransidMap.end())
	{
		vec = OmnNew vector<u64>();
		smIILID2TransidMap[iilid] = vec;
	}
	else
	{
		vec = itr3->second;
	}
	vec->push_back(transid);
	sgLock.unlock();
	return true;
}


bool
AosIILTransTester::transSent(const u64 &transid)
{
	if (!sgInited) init();
	u64 srcid = (transid >> 24);
	sgLock.lock();
	TransMapItr_t transitr = smSentTrans.find(srcid);
	vector<u64> *vec;
	if (transitr == smSentTrans.end())
	{
		vec = OmnNew vector<u64>();
		smSentTrans[srcid] = vec;
	}
	else
	{
		vec = transitr->second;
	}
	vec->push_back(transid);
	sgLock.unlock();
	return true;
}


vector<u64> *
AosIILTransTester::getSentTrans(AosIILTrans *trans)
{
	if (!sgInited) init();
	u64 transid = trans->getTransId();
	u64 srcid = (transid >> 24);
	sgLock.lock();
	TransMapItr_t transitr = smSentTrans.find(srcid);
	if (transitr == smSentTrans.end())
	{
		OmnAlarm << "Internal Error" << enderr;
		sgLock.unlock();
		return 0;
	}
	vector<u64> *vec = transitr->second;
	sgLock.unlock();
	return vec;
}


vector<u64> *
AosIILTransTester::getRecvTrans(AosIILTrans *trans)
{
	if (!sgInited) init();
	u64 transid = trans->getTransId();
	u64 srcid = (transid >> 24);
	sgLock.lock();
	TransMapItr_t transitr = smRecvTrans.find(srcid);
	if (transitr == smRecvTrans.end())
	{
		// OmnAlarm << "internal error" << enderr;
		sgLock.unlock();
		return 0;
	}
	vector<u64> *vec = transitr->second;
	sgLock.unlock();
	return vec;
}


bool 
AosIILTransTester::addDoc(
		const OmnString &opr, 
		const u64 &transid, 
		const AosXmlTagPtr &doc)
{
	if (!sgInited) init();
	u64 tid = (transid >> 24);
//	OmnScreen << "DumpDoc: " << tid << ":" << opr << ":" << doc->toString() << endl;
	return true;
}


bool 
AosIILTransTester::checkTrans(
		const u64 &iilid,
		const AosIDTransVectorPtr &trans_list)
{
	// This function checks whether the transactions collected for 
	// 'iilid' are the same as the ones in 'trans'. If yes, it removes
	// all the transactions from this class.
	if (!sgInited) init();
	if (iilid >= eMaxIILID) return true;
	AosIDTransVector::iterator itr = trans_list->begin();	
	sgLock.lock();
	TransMapItr_t trans_itr = smIILID2TransidMap.find(iilid);
	if (trans_itr == smIILID2TransidMap.end())
	{
		// It is not monitored
		sgLock.unlock();
		return true;
	}

	AosIILTransPtr trans;
	vector<u64> *vec = trans_itr->second;
	if (!vec)
	{
		OmnAlarm << "Internal error" << enderr;
		sgLock.unlock();
		return false;
	}
	u32 idx = 0;
	for (; itr != trans_list->end(); itr++)
	{
		trans = (AosIILTrans *)(*itr).getPtr();
		if (idx >= vec->size() || trans->getTransId() != (*vec)[idx])
		{
			// It is incorrect.
			AosIDTransVector::iterator itr1 = trans_list->begin();
			AosIILTransPtr trans1;
//			OmnScreen << "***************** IILID: " << iilid << ":" << idx << endl;
			idx = 0;
			bool is_same = true;
			for (; itr1 != trans_list->end(); itr1++)
			{
				trans1 = (AosIILTrans *)(*itr1).getPtr();
//				cout << "; " << trans1->getTransId() << ":" << trans1->getOperation()
//					<< ":" << trans1->getIILTestDocid() 
//					<< ":" << trans1->getRecvSeqno();
				if (!(is_same && idx < vec->size() && trans1->getTransId() == (*vec)[idx]))
				{
					is_same =false;
					if (idx < vec->size())
					{
//						cout << "; DDD: " << (*vec)[idx];
					}
				}
				else
				{
//					cout << "; EEE: " << (*vec)[idx];
				}
				idx++;
			}
//			OmnScreen << "***************** IILID: " << iilid << endl;
			sgLock.unlock();
			return false;
		}
		idx++;
	}

	vec->clear();
	sgLock.unlock();
	return true;
}
#endif
