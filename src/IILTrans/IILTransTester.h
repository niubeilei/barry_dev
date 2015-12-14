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
#ifndef AOS_IILTrans_IILTransTester_h
#define AOS_IILTrans_IILTransTester_h

#include "aosUtil/Types.h"
#include "IDTransMap/Ptrs.h"
#include "IDTransMap/IDTransMap.h"
#include "IILTrans/IILTransInclude.h"
#include "Util/HashUtil.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosIILTrans;

#define AOSIILTRANSTEST_CHECKTRANS(iilid, trans) AosIILTransTester::checkTrans(iilid, trans)
#define AOSIILTRANSTEST_ADDDOC(opr, transid, doc) AosIILTransTester::addDoc(opr, transid, doc)

// #define AOSIILTRANSTEST_DEFINE_RECV_SEQNO u32 mIILTesterRecvSeqno
#define AOSIILTRANSTEST_TRANS_SENT(transid) AosIILTransTester::transSent(transid)
#define AOSIILTRANSTEST_RESET_RECV_SEQNO mIILTesterRecvSeqno = 0
#define AOSIILTRANSTEST_IILTRANS_RECVED(iilid, trans) AosIILTransTester::transReceived((iilid), (trans))
// Moved to IILTrans.h by Chen Ding, 12/12/2012
// #define AOSIILTRANSTEST_DEFINE_MEMBERFUNCS 
// 	virtual u32 getRecvSeqno() const {return mIILTesterRecvSeqno;} 
// 	virtual bool setRecvSeqno(const u32 s) {mIILTesterRecvSeqno=s;return true;} 
// 	virtual u64 getIILTestDocid() const {return mDocid;} 

class AosIILTransTester
{
public:
	typedef hash_map<const u64, vector<u64>*, u64_hash, u64_cmp> TransMap_t;
	typedef hash_map<const u64, vector<u64>*, u64_hash, u64_cmp>::iterator TransMapItr_t;

	enum
	{
		eMaxIILID = 200000,
		eInitRecvSeqno = 100,
		eMaxDocTrans = 30000
	};

	enum Opr
	{
		eInvalid,
		eAdd,
		eRemove,
		eReceived
	};

	static vector<u64> *smDocids[eMaxIILID];
	static vector<Opr> *smOprs[eMaxIILID];
	static AosU642U64_t smIILID2DistIDMap;
	static TransMap_t   smIILID2TransidMap;
	static AosU642U32_t smIILID2SeqnoMap;
	static u32 			smLastSeqno[eMaxIILID];
	static TransMap_t	smRecvTrans;
	static TransMap_t	smSentTrans;
	static AosXmlTagPtr	smDocs[eMaxDocTrans];

	static void init();
	static bool addDoc(const u64 &iilid, AosIILTrans *trans);
	static bool removeDoc(const u64 &iilid, AosIILTrans *trans);
	static bool transReceived(const u64 &iilid, AosIILTrans *trans);
	static bool transSent(const u64 &transid);
	static vector<u64> * getSentTrans(AosIILTrans *trans);
	static vector<u64> * getRecvTrans(AosIILTrans *trans);
	static bool addDoc(const OmnString &opr, const u64 &transid, const AosXmlTagPtr &doc);
	static bool checkTrans(const u64 &iilid, const AosIDTransVectorPtr &trans);
};
#endif

