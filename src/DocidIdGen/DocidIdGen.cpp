////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Modification History:
// 10/01/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DocidIdGen/DocidIdGen.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "DocServer/DocSvr.h"
#include "IdGen/U64IdGen.h"
#include "IdGen/IdGenMgr.h"
#include "SEUtil/DocTags.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SiteMgr/SyncServer.h"
#include "Thread/Mutex.h"
#include "Util/StrSplit.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(AosDocidIdGenSingleton,
	AosDocidIdGen,
	AosDocidIdGenSelf,
	"AosDocidIdGen");

const OmnString sgDftCidPrefix = "";

AosDocidIdGen::AosDocidIdGen()
{
	OmnScreen << "SeIdGenCreated" << endl;	
}


AosDocidIdGen::~AosDocidIdGen()
{
}


bool
AosDocidIdGen::start()
{
	AosRundataPtr rdata = OmnApp::getRundata();
	aos_assert_r(OmnApp::getAppConfig(), false);
	AosXmlTagPtr def = OmnApp::getAppConfig()->getFirstChild(AOSCONFIG_IDGENS);
	if (!def) return true;

	AosXmlTagPtr defs = def->getFirstChild("defs");
	if (defs)
	{
		try
		{
			AosXmlTagPtr def = defs->getFirstChild(AOSIDGEN_DOCID);
			if (def)
			{
				mDocidIdGen = OmnNew AosU64IdGen(def);
			}
		}

		catch (const OmnExcept &e)
		{
			OmnAlarm << "Failed to create ID GEN: " << e.getErrmsg() << enderr;
			return false;
		}
	}

	// Verify Docid ID Gen is correct, which means that the current docid
	// must not be used by anyone. To ensure its correctness, we will try
	// a number of new docids to make sure they are indeed not used.
	if (mDocidIdGen)
	{
		u64 crtid = mDocidIdGen->loadId();
		if (crtid == 0)
		{
			OmnAlarm << "Docid not correct: " << enderr;
			exit(-1);
		}
		verifyDocid(def, crtid, rdata);

		OmnScreen << "Current DOCID:   " << mDocidIdGen->getCrtid1() 
			<< ":" << mDocidIdGen->getCrtMaxId() << endl;
	}

	return true;
}


bool
AosDocidIdGen::stop()
{
	return true;
}


bool 
AosDocidIdGen::setDocid(const char *docidstr, const char *blocksize)
{
	aos_assert_r(mDocidIdGen, false);
	return modifyDef(mDocidIdGen, docidstr, blocksize);
}

bool
AosDocidIdGen::modifyDef(
		const AosU64IdGenPtr &idgen, 
		const char *idstr, 
		const char *blocksize)
{
	// If 'docidstr' is '-', ignore it. Otherwise, save it.
	// If 'blocksize' is '-', ignore it. Otherwise, save it.
	aos_assert_r(idgen, false);
	aos_assert_r(idstr, false);
	if (strcmp(idstr, "-"))
	{
		u64 id = atoll(idstr);
		aos_assert_r(id != 0, false);
		aos_assert_r(idgen, false);
		aos_assert_r(idgen->setCrtid(id), false);
	}

	aos_assert_r(blocksize, false);
	if (strcmp(blocksize, "-"))
	{
		u64 bsize = atoll(blocksize);
		aos_assert_r(bsize != 0, false);
		aos_assert_r(idgen, false);
		aos_assert_r(idgen->setBlocksize(bsize), false);
	}
	return true;
}


bool
AosDocidIdGen::verifyDocid(
	const AosXmlTagPtr &def, 
	const u64 &start_docid, 
	const AosRundataPtr &rdata)
{
	// This function checks whether the 'start_docid' is valid, which means
	// it should not point to a valid doc. In addition, all the docids after
	// it should not be used.
	
	if (def->getAttrStr("check_docids") == "false") return true;
	
	u64 crtid = start_docid;
	if (crtid == 0) crtid++;
	for (int i=0; i<eMaxValidationTries; i++)
	{
		//AosXmlTagPtr doc = AosDocSvr::getSelf()->getDoc(crtid, rdata);
		AosXmlTagPtr doc = AosDocSvr::getSelf()->getVerifyDoc(crtid, rdata);
		if (!doc)
		{
			// 'crtid' does not point to a valid doc. Try the next one
			crtid++;
			continue;
		}

		// This is a very serious problem. We will exit the program.
		// Search for the first available id
		int guard = eMaxTries;
		crtid++;
		while (guard--)
		{
			OmnScreen << "Trying: " << crtid << endl;
			//doc = AosDocSvr::getSelf()->getDoc(crtid,rdata);
			doc = AosDocSvr::getSelf()->getVerifyDoc(crtid,rdata);
			if (doc)
			{
				// It is still a used one. Continue;
				crtid++;
				continue;
			}

			// Found the first invalid one. Will search for eMaxInvalidTries
			// more to ensure it is correct.
			int gg = eMaxInvalidTries;
			bool all_invalid = true;
			u64 possible_iilid = crtid;
			crtid++;
			while (gg--)
			{
				OmnScreen << "Trying: " << crtid << endl;
				//doc = AosDocSvr::getSelf()->getDoc(crtid, rdata);
				doc = AosDocSvr::getSelf()->getVerifyDoc(crtid, rdata);
				if (doc)
				{
					// This means that some used ids are found.
					// This is very bad.
					OmnScreen << "Found the first unused Docid: "
						<< possible_iilid << ", but found a valid one: "
						<< crtid << ". Will continue search!" << endl;
					all_invalid = false;
					break;
				}
				crtid++;
			}

			if (all_invalid)
			{
				// This means that 'possible_iilid' is invalid, and
				// the following 'eMaxInvalidTries' are all invalid. 
				// This is a good indication that 'possible_iilid'
				// is good.
				OmnScreen << "Found the first unused Docid: "
					<< possible_iilid << " (after "
					<< eMaxInvalidTries << " number of tries)"
					<< endl;
				exit(0);
			}

			// This means there are some used ones and some unused ones. 
			// This is normally a serious problem. Will continue the searching.
			guard = eMaxTries;
		}

		// This means that after all the tries, the IDs are still 
		// used ones. Will give up
		OmnAlarm << "Tried IDs: " << crtid << " but all are used ones!" << enderr;
		exit(0);
	}

	return true;
}

u64	
AosDocidIdGen::nextDocid(const AosRundataPtr &rdata) 
{
	// This function assumes the caller ensured the function is thread
	// safe. For the time being, to ensure docids are managed correctly,
	// we will check whether the docid was used. If yes, it will raise
	// alarm and try the next one.
	aos_assert_r(mDocidIdGen, 0);
	int guard = eMaxTries;
	bool alarm_raised = false;
	while (guard-- > 0)
	{
		u64 docid = mDocidIdGen->getNextId1();
		OmnScreen << "Trying next docid: " << docid << endl;
		//AosXmlTagPtr doc = AosDocSvr::getSelf()->getDoc(docid, rdata);
		AosXmlTagPtr doc = AosDocSvr::getSelf()->getVerifyDoc(docid, rdata);
		if (!doc) 
		{
			OmnScreen << "Found a good one: " << docid << endl;
			return docid;
		}
		if (!alarm_raised)
		{
			alarm_raised = true;
			OmnAlarm << "Docid is not unique: " << docid 
				<< ". Will try " << eMaxTries << enderr;
		}
	}
	OmnShouldNeverComeHere;
	return AOS_INVDID;
}
#endif
