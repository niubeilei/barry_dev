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
// 09/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SengTorturer/TesterCreateCtnr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "SengTorUtil/SengTesterThrd.h"
#include "SengTorUtil/SengTesterMgr.h"
#include "SengTorUtil/SengTesterFileMgr.h"
#include "SengTorUtil/StUtil.h"
#include "SengTorUtil/StOprTranslator.h"
#include "SengTorUtil/StDoc.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SEModules/SeRundata.h" 
#include "SmartDoc/SmartDoc.h"
#include "SEUtil/Ptrs.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include <stdlib.h>


AosCreateCtnrTester::AosCreateCtnrTester(const bool regflag)
:
AosSengTester(AosSengTester_CreateCtnr, AosTesterId::eCreateCtnr, regflag),
mUserParentCtnrWeight(20),
mDocCtnrWeight(50),
mCreatePubCtnrWeight(50),
mWithCidWeight(50),
mUsePubParentWeight(50),
mParentWithCidWeight(50)
{
}


AosCreateCtnrTester::AosCreateCtnrTester()
:
AosSengTester(AosSengTester_CreateCtnr, "ctnr", AosTesterId::eCreateCtnr),
mUserParentCtnrWeight(20),
mDocCtnrWeight(50),
mCreatePubCtnrWeight(50),
mWithCidWeight(50),
mUsePubParentWeight(50),
mParentWithCidWeight(50)
{
}


AosCreateCtnrTester::~AosCreateCtnrTester()
{
}

bool 
AosCreateCtnrTester::test()
{
	AosRundataPtr rdata = mThread->getRundata(); 

	aos_assert_r(createRawCtnrDoc(), false);
	aos_assert_r(pickParents(), false);
	aos_assert_r(determinePublic(), false);
	aos_assert_r(createCtnr(), false);
	//rdata->clearLogs();
	aos_assert_r(checkCreation(rdata), false);
	aos_assert_r(addCtnr(rdata), false);
	return true;
}


bool
AosCreateCtnrTester::createRawCtnrDoc()
{

	mRawCtnr = mThread->createDoc();
	aos_assert_r(mRawCtnr, false);
	mRawCtnr->setAttr(AOSTEST_DOCID, mThread->getNewDocId());
	mRawCtnr->setAttr(AOSTAG_OTYPE, AOSOTYPE_CONTAINER);
	return true;
}

 
bool
AosCreateCtnrTester::pickParents()
{
	OmnString cid = mThread->getCrtCid();
	aos_assert_r(cid!="", false);
	AosStContainerPtr container;
	mParentctnr = "";
	if ((rand() % 100) < mUserParentCtnrWeight)
	{
		// Get a container and the container's access record
		if ((rand() % 100) < mDocCtnrWeight)
		{
			mType = AosStContainer1::eDocCtnr;
		}
		else
		{
			mType = AosStContainer1::eUserCtnr;
		}

		container = mThread->pickContainer(mType);
		aos_assert_r(container, false);
		aos_assert_r(container->mType == mType, false);
		mParentctnr = container->mObjid;
		aos_assert_r(mParentctnr != "", false);
	}

	if (mParentctnr == "")
	{
		// No parent is selected. The new container's parent container
		// should be a private container.
		mParent_is_public = false;
		mParent_is_cid = false;
		if (rand() % 100 <= 50)
		{
			mRawCtnr->setAttr(AOSTAG_CTNR_PUBLIC, "true");
			mParent_is_public = true;
		}

		return true;
	}

	// Parent container is not empty
	mRawCtnr->setAttr(AOSTAG_PARENTC, mParentctnr);

	// 3. Check whether parent container has CID
	OmnString prefix, cc;
	AosObjid::decomposeObjid(mParentctnr, prefix, cc);

	if (cc == "")
	{
		// Parent has no cid. Determine whether to use
		// a public or private container
		mParent_is_public = false;
		mParent_is_cid = false;
		if ((rand() % 100) < mUsePubParentWeight)
		{
			mRawCtnr->setAttr(AOSTAG_CTNR_PUBLIC, "true");
			mParent_is_public = true;	
			return true;
		}
		
		// Treat the parent as private
		mRawCtnr->setAttr(AOSTAG_CTNR_PUBLIC, "false");

		// Determine whether to attach cid to it
		if ((rand() % 100) < mParentWithCidWeight)
		{
			mParentctnr= AosObjid::compose(mParentctnr, cid);
			mParent_is_cid = true;
			mRawCtnr->setAttr(AOSTAG_PARENTC, mParentctnr);
		}
		return true;
	}
	
	// Parent container already has a cid. 
	mParent_is_cid = true;

	// Check whether the cloud id is the same as the current
	// cloud id.
	if (cc == cid)
	{
		// It is the same as the current cloud id.
		mParent_is_public = false;
		if (rand() % 100 <= 50)
		{
			mRawCtnr->setAttr(AOSTAG_CTNR_PUBLIC, "true");
			mParent_is_public = true; 
		}
		return true;
	}
	
	// The cloud is different.
	mParent_is_public = false;
	mParent_is_cid = false;
	if ((rand() % 100) < mUsePubParentWeight)
	{
		mRawCtnr->setAttr(AOSTAG_CTNR_PUBLIC, "true");
		mParent_is_public = true;	
		return true;
	}
	
	// Treat the parent as private
	mRawCtnr->setAttr(AOSTAG_CTNR_PUBLIC, "false");
	mParent_is_public = false;	
	mParent_is_cid = false;

	// Determine whether to attach cid to it
	if ((rand() % 100) < mParentWithCidWeight)
	{
		mParentctnr = AosObjid::compose(mParentctnr, cid);
		mParent_is_cid = true;
		mRawCtnr->setAttr(AOSTAG_PARENTC, mParentctnr);
	}

	return true;
}


bool
AosCreateCtnrTester::determinePublic()
{
	mObjid = mThread->pickNewCtnrObjid();
	aos_assert_r(mObjid!="", false);
	
	mRawCtnr->setAttr(AOSTAG_OBJID, mObjid);
	mCtnr_is_public = false;
	mCtnr_with_cid = false;
	if ((rand() % 100) < mCreatePubCtnrWeight)
	{
		// To create a public container
		mCtnr_is_public = true;
		mRawCtnr->setAttr(AOSTAG_PUBLIC_DOC, "true");
		return true;
	}
	
	// It is a private container. Determine whether to attach the cloud id
	if ((rand() % 100) < mWithCidWeight)
	{
		// To attach the cid
		mCtnr_with_cid = true;
		mObjid = AosObjid::compose(mObjid, mThread->getCrtCid());
		mRawCtnr->setAttr(AOSTAG_OBJID, mObjid);
		return true;
	}

	// It is a private container and no cid is attached.
	mCtnr_with_cid = false;
	return true;
}


bool
AosCreateCtnrTester::createCtnr()
{
	aos_assert_r(mRawCtnr->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_CONTAINER, false); 
	mResp = AosSengAdmin::getSelf()->createUserCtnr(
									mThread->getSiteid(), 
									mRawCtnr, 
									mThread->getCrtSsid(),
									mThread->getUrlDocid());

	if (mResp)
	{
		OmnString objid = mResp->getAttrStr(AOSTAG_OBJID);
		aos_assert_r(objid != "", false);
		mServerCtnr = AosSengAdmin::getSelf()->retrieveDocByObjid(
									mThread->getSiteid(), 
									mThread->getCrtSsid(),
									mThread->getUrlDocid(),
									objid, 
									true 
									);
		aos_assert_r(mServerCtnr, false);
		aos_assert_r(mRawCtnr->getAttrU64(AOSTEST_DOCID, AOS_INVDID) ==
					mServerCtnr->getAttrU64(AOSTEST_DOCID, AOS_INVDID), false);
		aos_assert_r(mServerCtnr->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_CONTAINER, false);
		aos_assert_r(mServerCtnr->getAttrStr(AOSTAG_OBJID, "") == objid, false);
	}

	return true;
}


bool
AosCreateCtnrTester::checkCreation(const AosRundataPtr &rdata)
{
	mCanCreate = determineCanCreate();
	if (!mCanCreate)
	{
		aos_assert_r(!mResp, false);
		aos_assert_r(!mServerCtnr, false);
		return true;
	}
	aos_assert_r(checkServerDoc(), false);
	aos_assert_r(checkObjid(), false);
	aos_assert_r(checkParents(rdata), false);
	return true;

}

bool
AosCreateCtnrTester::determineCanCreate()
{
	if (mThread->getCrtSsid() == "") return false;
	return true;
}


bool
AosCreateCtnrTester::checkServerDoc()
{
	aos_assert_r(mServerCtnr, false);

	// Retrieve the doc by docid from the server
	AosXmlTagPtr doc = AosSengAdmin::getSelf()->retrieveDocByDocid(
			mThread->getSiteid(), mThread->getCrtSsid(),
			mServerCtnr->getAttrU64(AOSTAG_DOCID, AOS_INVDID),
			false, mThread->getUrlDocid());
	aos_assert_r(doc, false);

	// Retrieve the doc by objid from the server
	AosXmlTagPtr doc2 = AosSengAdmin::getSelf()->retrieveDocByObjid(
			mThread->getSiteid(), 
			mThread->getCrtSsid(),
			mThread->getUrlDocid(),
			mServerCtnr->getAttrStr(AOSTAG_OBJID), 
			true
			);
	aos_assert_r(doc2, false);

	aos_assert_r(doc->getAttrStr(AOSTAG_OBJID) == doc2->getAttrStr(AOSTAG_OBJID), false);
	aos_assert_r(doc->getAttrStr(AOSTAG_DOCID) == doc2->getAttrStr(AOSTAG_DOCID), false);

	aos_assert_r(doc->getAttrStr(AOSTAG_OBJID) ==
			mServerCtnr->getAttrStr(AOSTAG_OBJID), false);
	aos_assert_r(doc->getAttrU64(AOSTAG_DOCID, AOS_INVDID) ==
			mServerCtnr->getAttrU64(AOSTAG_DOCID, AOS_INVDID), false);
	aos_assert_r(doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_CONTAINER, false);
	aos_assert_r(doc->getAttrStr(AOSTAG_CTNR_PUBLIC) == "", false);
	aos_assert_r(doc->getAttrStr(AOSTAG_PUBLIC_DOC) == "", false);
	return true;
}


/*
bool
AosCreateCtnrTester::checkParentObjid()
{
	OmnString parent_objid = mServerCtnr->getAttrStr(AOSTAG_PARENTC);
	OmnString prefix, cc;
	AosObjid::decomposeObjid(parent_objid, prefix, cc);
	if (mParentctnr == "")
	{
		// If parent was not set in mRawCtnr, it should have a parent
		// and its parent should be private. 
		aos_assert_r(cc != "", false);
		aos_assert_r(cc == mThread->getCrtCid(), false);
		return true;
	}

	OmnString cid;
	AosObjid::decomposeObjid(mParentctnr, prefix, cid);
	if (cid == mThread->getCrtCid())
	{
		// It means that the parent container contains cid and it 
		// is the same as the current cid. Regardless of whether
		// we said parent container is public or not, it is private.
		aos_assert_r(cid == cc, false);

		OmnString pp, ddd;
		AosObjid::decomposeObjid(prefix, pp, ddd);
		aos_assert_r(ddd != cid, false);
		aos_assert_r(ddd != mThread->getCrtCid(), false);
		return true;
	}

	if (cid == "" || cid != mThread->getCrtCid())
	{
		// The parent container does not have a cid. Check whether
		// it is a public or not.
		if (mParent_is_public)
		{
			aos_assert_r(cc == "", false);
			return true;
		}

		aos_assert_r(cc != "", false);
		aos_assert_r(cc == mThread->getCrtCid(), false);
		return true;
	}

	// The parent container has a cid and it is the same as the
	// current cid. 
	aos_assert_r(cc != "", false);
	aos_assert_r(cc == mThread->getCrtCid(), false);

	OmnString pp, ddd;
	AosObjid::decomposeObjid(prefix, pp, ddd);
	aos_assert_r(ddd != cc, false);
	return true;
}
*/


bool
AosCreateCtnrTester::checkObjid()
{
	OmnString objid = mServerCtnr->getAttrStr(AOSTAG_OBJID);
	OmnString prefix, cc;
	AosObjid::decomposeObjid(objid, prefix, cc);
	if (mCtnr_is_public)
	{
		// The objid should not contain the cid
		aos_assert_r(cc == "", false);
		aos_assert_r(objid == mObjid, false);
		return true;
	}
	
	aos_assert_r(cc != "", false);
	aos_assert_r(cc == mThread->getCrtCid(), false);
	if (mCtnr_with_cid)
	{
		aos_assert_r(objid == mObjid, false);
		return true;
	}
	
	aos_assert_r(cc == mThread->getCrtCid(), false);
	aos_assert_r(prefix == mObjid, false);
	mObjid = objid;
	return true;
}


bool
AosCreateCtnrTester::checkParents(const AosRundataPtr &rdata)
{
	OmnString cid = mThread->getCrtCid();
	aos_assert_r(cid!="", false);
	// Verify parent container
	OmnString pctr_objid = mServerCtnr->getAttrStr(AOSTAG_PARENTC);
	aos_assert_r(pctr_objid != "", false);
	OmnString prefix1, cc1;
	AosObjid::decomposeObjid(pctr_objid, prefix1, cc1);
	aos_assert_r(prefix1 != "", false);
	if (mParentctnr == "")
	{
		if (!mParent_is_public)
		{
			aos_assert_r(cc1 != "", false);
			aos_assert_r(cc1 == cid, false);
		}
	}
	else
	{
		if (mParent_is_public)
		{
			//Parent Container:AOSTAG_CTNR_PUBLIC =  true ;
			//Tester set zky_pctr =  pctrname.100001(name.cludid)
			//Server drop cid; zky_pctr == pctrname
			OmnString prefix2, cc2;
			AosObjid::decomposeObjid(mParentctnr, prefix2, cc2);
			aos_assert_r(prefix2 != "", false);
			if (cc2 != "")
			{
				aos_assert_r(strcmp(prefix2.data(), pctr_objid.data()) == 0, false);
				if (strcmp(prefix2.data(), pctr_objid.data()))
				{
					OmnMark;
				}

				AosStContainerPtr ct = mThread->getContainer(prefix2);
				if (!ct)
				{
					AosXmlTagPtr ctnr = mThread->retrieveDocByObjid(pctr_objid);
					aos_assert_r(ctnr, false);
					//u64 ldid = mThread->getNewDocId();
					u64 ldid = mThread->JudgeLocalDocid(ctnr);
					ctnr->setAttr(AOSTEST_DOCID, ldid);
					aos_assert_r(ctnr->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_CONTAINER, false);
					aos_assert_r(mThread->addContainer(mType, ctnr, rdata), false);
					bool rslt = AosSengAdmin::getSelf()->sendModifyReq(
						mThread->getSiteid(),
						mThread->getCrtSsid(), 
						mThread->getUrlDocid(),
						(char *)ctnr->getData(), 
						rdata, 
						true
						);
					aos_assert_r(rslt, false);
				}
			}
			else
			{
				aos_assert_r(strcmp(mParentctnr.data(), pctr_objid.data()) == 0, false);
			}
			//aos_assert_r(strcmp(mParentctnr, pctr_objid) == 0, false);
			//aos_assert_r(mParentctnr== pctr_objid, false);
		}
		else
		{
			//Parent Container:AOSTAG_CTNR_PUBLIC =  false ;
			//Parent Objid :mParent_is_cid = container.cid
			//Tester set zky_pctr =  pctrname.100001(name.cludid)
			//Server drop cid; zky_pctr == pctrname
			if (mParent_is_cid)
			{
				AosStContainerPtr ct = mThread->getContainer(pctr_objid);
				if (!ct)
				{
					AosXmlTagPtr ctnr = mThread->retrieveDocByObjid(pctr_objid);
					aos_assert_r(ctnr, false);
					//u64 ldid = mThread->getNewDocId();
					u64 ldid = mThread->JudgeLocalDocid(ctnr);
					ctnr->setAttr(AOSTEST_DOCID, ldid);
					aos_assert_r(ctnr->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_CONTAINER, false);
					aos_assert_r(mThread->addContainer(mType, ctnr, rdata), false);
					bool rslt = AosSengAdmin::getSelf()->sendModifyReq(
						mThread->getSiteid(),
						mThread->getCrtSsid(), 
						mThread->getUrlDocid(),
						(char *)ctnr->getData(), 
						rdata, 
						true);
					aos_assert_r(rslt, false);
				}

				aos_assert_r(strcmp(mParentctnr.data(), pctr_objid.data()) == 0, false);
			}
			else
			{
				aos_assert_r(cc1 != "", false);
				//aos_assert_r(cc1 == cid, false);

				//OmnString prefix2, cc2;
				//AosObjid::decomposeObjid(mParentctnr, prefix2, cc2);
				//aos_assert_r(prefix2 == prefix1, false);
				aos_assert_r(mParentctnr== prefix1, false);
				//aos_assert_r(cc2 == cc1, false);
			}
		}
	}
	return true;
}


bool
AosCreateCtnrTester::addCtnr(const AosRundataPtr &rdata)
{
	if (!mCanCreate) return true;
	aos_assert_r(mServerCtnr, false);
	aos_assert_r(mRawCtnr, false);
	aos_assert_r(mThread, false);
	u64 locdid1 = mServerCtnr->getAttrU64(AOSTEST_DOCID, 0);
	u64 locdid2 = mRawCtnr->getAttrU64(AOSTEST_DOCID, 0);
	aos_assert_r(locdid1 == locdid2, false);
	aos_assert_r(mObjid == mServerCtnr->getAttrStr(AOSTAG_OBJID), false);
	aos_assert_r(mServerCtnr->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_CONTAINER, false);
	aos_assert_r(mThread->addContainer(mType, mServerCtnr, rdata), false);
	return true;
}

