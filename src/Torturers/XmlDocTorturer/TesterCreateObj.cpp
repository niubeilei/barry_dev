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
#include "Torturers/XmlDocTorturer/TesterCreateObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Torturers/XmlDocTorturer/TesterLogin.h"
#include "Torturers/XmlDocTorturer/Ptrs.h"
#include "SengTorUtil/TesterXmlDoc.h"
#include "SengTorUtil/SengTesterMgr.h"
#include "SengTorUtil/SengTesterThrd.h"
#include "SengTorUtil/SengTesterFileMgr.h"
#include "SengTorUtil/StUtil.h"
#include "SengTorUtil/StOprTranslator.h"
#include "SengTorUtil/StDoc.h"
#include "SEModules/SeRundata.h" 
#include "SmartDoc/SmartDoc.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "SEUtil/Objid.h"

#include "Thrift/AosJimoAPI.h"       
#include "ThriftClientCPP/Headers.h"


extern shared_ptr<AosJimoAPIClient> gThriftClient;

vector<string>	AosCreateObjTester::mEntries;

AosCreateObjTester::AosCreateObjTester(const bool regflag)
:
AosSengTester(AosSengTester_CreateObj, AosTesterId::eCreateObj, regflag)
{
	setDefaultWeights();
}


AosCreateObjTester::AosCreateObjTester(const AosCreateObjTester &rhs)
:
AosSengTester(rhs)
{
	mWtUseKnownCtnr = rhs.mWtUseKnownCtnr;
	mWtUsePubCtnr = rhs.mWtUsePubCtnr;
	mWtUsePrivateDoc = rhs.mWtUsePrivateDoc;
	mWtUseNewObjid = rhs.mWtUseNewObjid;
}


AosCreateObjTester::AosCreateObjTester()
:
AosSengTester(AosSengTester_CreateObj, "crt_obj", AosTesterId::eCreateObj)
{
	setDefaultWeights();
}


bool
AosCreateObjTester::setDefaultWeights()
{
	mWtUseKnownCtnr 				= 50;
	mWtUsePubCtnr		 			= 50;
	mWtUsePrivateDoc 				= 50;
	mWtUseNewObjid					= 50;
	mWtUseExistingObjid				= 50;
	mWtWithCid						= 50;
	mWtUseRandomCtnr				= 40;
	mWtUseMultipleCtnrs				= 50;
	return true;
}


AosCreateObjTester::~AosCreateObjTester()
{
}


bool 
AosCreateObjTester::configTester(const AosXmlTagPtr &config)
{
	if (!config) return true;
	AosXmlTagPtr testers = config->getFirstChild("testers");
	if (!testers) return true;

	AosXmlTagPtr c_obj = testers->getFirstChild("create_obj");
	if (!c_obj) return true;

	mWeight = c_obj->getAttrInt("weight", mWeight);
	aos_assert_r(mWeight >= 0, false);

	mWtUseKnownCtnr = c_obj->getAttrInt("wtuse_known_ctnr", mWtUseKnownCtnr);
	aos_assert_r(mWtUseKnownCtnr >= 0, false);
	
	mWtUsePubCtnr = c_obj->getAttrInt("wtuse_pub_ctnr", mWtUsePubCtnr);
	aos_assert_r(mWtUsePubCtnr >= 0, false);

	mWtUsePrivateDoc = c_obj->getAttrInt("wtuse_private_doc", mWtUsePrivateDoc);
	aos_assert_r(mWtUsePrivateDoc >= 0, false);
	
	mWtUseNewObjid= c_obj->getAttrInt("wtuse_new_objid", mWtUseNewObjid);
	aos_assert_r(mWtUseNewObjid >= 0, false);

	mWtUseExistingObjid = c_obj->getAttrInt("wtuse_existing_objid", mWtUseExistingObjid);
	aos_assert_r(mWtUseExistingObjid >= 0, false);
	
	mWtWithCid = c_obj->getAttrInt("wt_with_cid", mWtWithCid);
	aos_assert_r(mWtWithCid >= 0, false);
	
	mWtUseRandomCtnr = c_obj->getAttrInt("wt_use_random_ctnr", mWtUseRandomCtnr);
	aos_assert_r(mWtUseRandomCtnr >= 0, false);
	
	mWtUseMultipleCtnrs= c_obj->getAttrInt("wt_use_multiple_ctnrs", mWtUseMultipleCtnrs);
	aos_assert_r(mWtUseMultipleCtnrs >= 0, false);
	return true;
}


bool 
AosCreateObjTester::test()
{

	aos_assert_r(createDoc(), false);
	/*
	// This function creates a doc. 
	mLog = "";
	AosRundataPtr rdata = mThread->getRundata(); 
	
	aos_assert_r(createDoc(), false);
	aos_assert_r(createRawDoc(), false);
	aos_assert_r(determineCtnr(), false);
	aos_assert_r(determineMemberOf(), false);
	aos_assert_r(determineCreator(), false);
	aos_assert_r(determineMetaData(), false);
	aos_assert_r(determinePublic(), false);
	aos_assert_r(determineObjid(), false);
	aos_assert_r(resolveObjid(), false);
	aos_assert_r(checkCreation(rdata), false);
	aos_assert_r(addDoc(), false);
	*/
	return true;
}



bool
AosCreateObjTester::createRawDoc()
{
	// It randomly creates an XML doc.
	mRawDoc = mThread->createDoc(); 
	aos_assert_r(mRawDoc, false);
	mRawDoc->setAttr(AOSTEST_DOCID, mThread->getNewDocId());
	mRawDoc->setAttr(AOSTAG_OTYPE, mThread->pickOtype());
	mRawDoc->setAttr(AOSTAG_STYPE, mThread->pickStype());
	return true;
}


bool
AosCreateObjTester::determineCtnr()
{
	// This function determines whether to use a known container,
	// a container that has not been created yet, or no container
	// at all.
	//
	// This function determines the following:
	// 	mParentObjidStr
	// 		If a parent is selected (existing or new), it is saved
	// 		in this member data.
	// 	mValidParent
	// 		If no parent is selected, it is set to false
	// 		If one parent is selected, but it has not been created yet,
	// 		it is set to false.
	// 		Otherwise, it is set to true.
	AosStContainerPtr ctnr = 0;
	mParentObjidStr = "";
	mValidParent = false;
	OmnString containers;
	mParentObjids.clear();
	mParentObjidTypes.clear();
	switch (OmnRandom::percent(
				mWtUseKnownCtnr, 
				mWtUseRandomCtnr, 
				mWtUseNoContainers))
	{
	case 0:
		 ctnr = mThread->pickValidContainer();
		 if (!ctnr)
		 {
			 mParentObjidStr = "";
			 break;
		 }
		 mParentObjidStr= ctnr->mObjid;
		 mParentObjids.push_back(mParentObjidStr);
		 mParentObjidTypes.push_back(true);
		 break;

	case 1:
		 mParentObjidStr = mThread->pickNewObjid();
		 mParentObjids.push_back(mParentObjidStr);
		 mParentObjidTypes.push_back(false);
		 break;

	// case 2:
	// 	 // Use multiple containers
	// 	 // pickMultipleContainers();
	// 	 break;

	case 2:
		 mParentObjidStr = "";
		 break;

	default:
		 OmnAlarm << "Percent error" << enderr;
		 return false;
	}

	if (mParentObjidStr == "")
	{
		mRawDoc->removeAttr(AOSTAG_PARENTC);
	}
	else
	{
		mRawDoc->setAttr(AOSTAG_PARENTC, mParentObjidStr);
	}
	return true;
}


bool
AosCreateObjTester::determineMemberOf()
{
	int num = OmnRandom::intByRange(
			2, 3, 80, 
			4, 6, 15, 
			7, 10, 5); 
	mMemberOfObjids.clear();
	mMemberOfTypes.clear();
	mMemberOfStr = "";
	aos_assert_r(num >= 2, false);
	for (int i=0; i<num; i++)
	{
		bool picked = false;
		if (OmnRandom::percent(80))
		{
			AosStContainerPtr ctnr = mThread->pickValidContainer();
			if (ctnr)
			{
				mMemberOfTypes.push_back(true);
				mMemberOfObjids.push_back(ctnr->mObjid);
				picked = true;
			}
		}

		if (!picked)
		{
			mMemberOfTypes.push_back(false);
			mMemberOfObjids.push_back(mThread->pickNewObjid());
		}
	}

	mMemberOfStr = mMemberOfObjids[0];
	for (u32 i=1; i<mMemberOfObjids.size(); i++)
	{
		mMemberOfStr << "," << mMemberOfObjids[i];
	}

	if (mMemberOfStr == "")
	{
		mRawDoc->removeAttr(AOSTAG_MEMBEROF);
	}
	else
	{
		mRawDoc->setAttr(AOSTAG_MEMBEROF, mMemberOfStr);
	}
	return true;
}


bool
AosCreateObjTester::determineCreator()
{
	mCreator = "";
	switch (OmnRandom::percent(
				mWtCreatorAsCid, 
				mWtCreatorAsUserid,
				mWtCreatorAsAnyUser,
				mWtCreatorAsStr, 
				mWtNoCreator))
	{
	case 0:
		 // Creator as current CID
		 mCreator = mThread->getCrtCid();
		 break;

	case 1:
		 // Creator as current userid 
		 mCreator = "";
		 mCreator << mThread->getCrtUserid();
		 break;

	case 2:
		 // Creator as any user 
		 {
			 AosStUser *user = mThread->pickUser();
			 if (user) mCreator = user->cid;
		 }
		 break;

	case 3:
		 // Creator as any string 
		 {
			 int len = OmnRandom::intByRange(
					 1, 5, 80,
					 6, 10, 15,
					 11, 20, 4, 
					 21, 40, 1);
			 OmnString str = OmnRandom::word(len);
			 mCreator = str;
		 }
		 break;

	case 4:
		 // No creator
		 mCreator = "";
		 break;

	default:
		 OmnAlarm << "Invalid percent!" << enderr;
		 return false;
	}

	if (mCreator != "")
	{
		mRawDoc->setAttr(AOSTAG_CREATOR, mCreator);
	}
	else
	{
		mRawDoc->removeAttr(AOSTAG_CREATOR);
	}
	return true;
}


bool
AosCreateObjTester::determineMetaData()
{
	if (OmnRandom::percent(mWtWithModifier))
	{
		OmnString word = OmnRandom::word30();
		mRawDoc->setAttr(AOSTAG_MODUSER, word);
	}
	return true;
}


bool
AosCreateObjTester::addDoc()
{
	if (!mCanCreate) return true;
	aos_assert_r(mServerDoc, false);
	aos_assert_r(mRawDoc, false);
	aos_assert_r(mThread, false);
	u64 locdid = mServerDoc->getAttrU64(AOSTEST_DOCID, 0);
	aos_assert_r(locdid > 0, false);
	aos_assert_r(locdid == mRawDoc->getAttrU64(AOSTEST_DOCID, 0), false);
	aos_assert_r(mThread->addDoc1(locdid, mServerDoc), false);
	return true;
}


bool
AosCreateObjTester::checkCreation(const AosRundataPtr &rdata)
{
	
	// A request to create a doc was sent and the response was
	// received. This function checks whether it is correct.
	JmoCallData cdata;
	JmoRundata _return;
	mCanCreate = determineCanCreate();		
	if (!mCanCreate)
	{
		// No access permissions to create. The creation should fail.
		// aos_assert_r(!mResp, false);
		aos_assert_r(!mServerDoc, false);

		if (mObjid != "" && mObjidUnique)
		{
			//AosXmlTagPtr doc = AosSengAdmin::getSelf()->retrieveDocByObjid(
			//			mThread->getSiteid(), 
			//			mThread->getCrtSsid(), 
			//			mThread->getUrlDocid(),
			//			mObjid, 
			//			true 
			//			);
			gThriftClient->getDocByObjid(_return, cdata, mObjid);
			if(_return.rcode != JmoReturnCode::SUCCESS)
				 OmnAlarm << "can't get the doc" << enderr;
	
			//if (mRejectLine == eObjidInvalid)
			//{
			//	aos_assert_r(doc, false);
			//}
			//else
			//{
			//	aos_assert_r(!doc, false);
			//}
		}
		return true;
	}

	/*
	// It is allowed. 
	aos_assert_r(checkServerDoc(), false);
	aos_assert_r(checkObjid(), false);
	aos_assert_r(mServerDoc, false);
	aos_assert_r(checkParents(rdata), false);
	aos_assert_r(mServerDoc->getAttrU64(AOSTAG_DOCID, 0) != 0, false);
	aos_assert_r(mServerDoc->isDocSame(mRawDoc), false);

	return true;
*/
}


bool
AosCreateObjTester::checkServerDoc()
{
	/*
	aos_assert_r(mServerDoc, false);
	AosXmlTagPtr doc = AosSengAdmin::getSelf()->retrieveDocByDocid(
		mThread->getSiteid(), mThread->getCrtSsid(), 
		mThread->getUrlDocid(),
		mServerDoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID),
		false);
	aos_assert_r(doc, false);
	aos_assert_r(doc->getAttrStr(AOSTAG_OBJID) == 
		mServerDoc->getAttrStr(AOSTAG_OBJID), false);
	aos_assert_r(doc->getAttrU64(AOSTAG_DOCID, AOS_INVDID) == 
		mServerDoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID), false);
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosCreateObjTester::determineCanCreate()
{
	// 1. If not use other cid:
	//    a. If there are multiple parents, any parent denies
	// 	     the creation, the creation should be rejected.
	//    b. If no parent is specified:
	//       i.  If the parent is public, it should be rejected.
	//       ii. If the parent is private, it should be allowed.
	// 2. If on behalf of another cid:
	//    (not supported yet)
	mLog << ":" << __LINE__;
	mRejectLine = eAllowed;
	if (mThread->getCrtSsid() == "") 
	{
		mRejectLine = eNotLogin;
		mLog << ":" << __LINE__;
		return false;
	}

	if (!mObjidUnique && !mResolveObjid) 
	{
		mRejectLine = eObjidNotUnique;
		mLog << ":" << __LINE__;
		return false;
	}

	if (!mParentPublic && mObjid != "")
	{
		// Check whether the objid is in the form:
		// 		<prefix>.cid
		// and its parent objid is <prefix>. In this case, 
		// the server will convert the parent container
		// objid to:
		// 		<prefix>.cid
		// This will be the same as the doc's objid.
		OmnString prefix, cc;
		AosObjid::decomposeObjid(mObjid, prefix, cc);
		if (prefix == mParentObjidStr && cc == mCid)
		{
			// It is in the form.
			if (!mResolveObjid)
			{
				mRejectLine = eObjidInvalid;
				mLog << ":" << __LINE__;
				return false;
			}

			// The object should be in the form:
			// 	<prefix>(nn).cid
			const char *data = prefix.data();
			aos_assert_r(data[prefix.length()-1] == ')', false);
		}
	}

	mLog << ":" << __LINE__;
	if (!mParentPublic && mParentObjidStr != "")
	{
		OmnString parent_objid  = AosObjid::compose(mParentObjidStr, mThread->getCrtCid());
		aos_assert_r(parent_objid != "", false);
		AosXmlTagPtr checkctnr = mThread->retrieveDocByObjid(parent_objid);
		mLog << ":" << __LINE__;
		if (checkctnr && checkctnr->getAttrStr(AOSTAG_OTYPE) != AOSOTYPE_CONTAINER) 
		{
			mRejectLine = eParentNotContainer;
			mLog << ":" << __LINE__;
			return false;
		}
	}

	mLog << ":" << __LINE__;
	if (mThread->getCrtUserid() == mMgr->getRootDocid()) 
	{
		mLog << ":" << __LINE__;
		mRejectLine = eAllowed;
		return true;
	}

	mLog << ":" << __LINE__;
	if (mParentObjidStr == "")
	{
		// No parents. Check whether the container is public
		mLog << ":" << __LINE__;
		if (mParentPublic) 
		{
			mRejectLine = eParentIsPublic;
			mLog << ":" << __LINE__;
			return false;
		}
		mLog << ":" << __LINE__;
		mRejectLine = __LINE__;
		return true;
	}

	// Check the access
	mLog << ":" << __LINE__;
	OmnString cid = mThread->getCrtCid();
	aos_assert_r(mServerDoc, false);
	bool rslt = (mThread->canDoOpr(mServerDoc->getAttrU64(AOSTEST_DOCID, 0), 
					AosSecOpr::eCreate));
	mLog << ":" << __LINE__;
	if (!rslt) mRejectLine = eOperationReject;
	return rslt;
	/*
	if (ssid == "")
	{
		// it was not logged in. In the current implementations, it
		// should not create the doc
		aos_assert_r(cid == "", false);
		can_create = false;
		return true;
	}

	// It was logged in
	OmnString pobjid = parent_objid;
	if (!mParentPublic)
	{
		OmnString prefix, cid;
		AosObjid::decomposeObjid(parent_objid, prefix, cid);
		if (cid != mThread->getCrtCid())
		{
			pobjid = AosObjid::compose(pobjid, mThread->getCrtCid());
		}
	}

	if (!valid_parent)
	{
		// The parent did not exist. The parent is created under
		// the requester's lost+found (if parent is not public)
		// or the system's lost+found (if parent is public). 
		// If it is private, it is allowed. If it is public, it is
		// determined by the site's access control.
		if (!mParentPublic) return true;
		return mMgr->canCreateUnderLostFound(mThread->getCrtCid());
	}

	// The container already existed when the doc is created. 
	AosStContainer *container = mThread->getContainer(parent_objid);
	aos_assert_r(container, false);
	AosXmlTagPtr arcd = mMgr->getAccessRcd(mThread->getSiteid(), container->svrdid);
	aos_assert_r(arcd, false);
	OmnString accesses = arcd->getAttrStr(AOSTAG_CREATE_ACSTYPE);
	if (accesses == "") return false;

	AosXmlTagPtr doc = mMgr->retrieveXmlDoc(mThread->getSiteid(), parent_objid);
	aos_assert_r(doc, false);
	return mMgr->checkAccess(doc, accesses, mThread->getCrtCid());
	*/
}


bool
AosCreateObjTester::checkObjid()
{

	// It checks whether objid is correct:
	// 1. If mObjid == "", the system should assign an objid to it
	//    in the form:
	//    	xxx (if it is public)
	//    or 
	//      xxx.cid (if it is private)
	// 2. Otherwise:
	//    a. If mObjidUnique:
	//       i.  If mObjid contains the cid, the objid should not be changed.
	//       ii. If mObjid contains no cid:
	//           1. If mDocPublic, objid should not be modified.
	//           2. If !mDocPublic, objid should be appended the cid.

	// If mObjid is not unique and mResolveObjid is not true, it should
	// fail the creation.
	aos_assert_r(mServerDoc, false);
	OmnString objid = mServerDoc->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(objid != "", false);
	OmnString prefix2, cid2;
	AosObjid::decomposeObjid(objid, prefix2, cid2);
	aos_assert_r(prefix2 != "", false);

	if (mObjid == "")
	{
		if (mDocPublic)
		{
			aos_assert_r(cid2 == "", false);
			aos_assert_r(prefix2 == objid, false);
		}
		else
		{
			aos_assert_r(cid2 != "", false);
			aos_assert_r(prefix2 != "", false);
		}
		return true;
	}

	OmnString prefix1, cid1;
	AosObjid::decomposeObjid(mObjid, prefix1, cid1);
	aos_assert_r(prefix1 != "", false);

	if (mObjidUnique)
	{
		// If mObjid already contains the right cid, objid should not be changed.
		if (cid1 != "" && cid1 == mThread->getCrtCid())
		{
			aos_assert_r(mObjid == objid, false);
			return true;
		}

		// If it is public doc, objid should not be changed.
		if (mDocPublic)
		{
			aos_assert_r(mObjid == objid, false);
			return true;
		}

		// It is a private doc:
		aos_assert_r(cid2 != "", false);
		aos_assert_r(cid2 == mThread->getCrtCid(), false);
		return true;
	}

	// The objid is not unique. 
	aos_assert_r(mResolveObjid, false);
	aos_assert_r(mObjid != "", false);
	aos_assert_r(mObjid != objid, false);
	aos_assert_r(checkDuplicatedObjid(), false);
	return true;
}


bool
AosCreateObjTester::checkDuplicatedObjid()
{
	// This function assumes that the objid is not unique and the caller
	// wanted to resolve the objid. This means that the real objid 
	// should be in the form:
	// 	xxxx(nnn)
	aos_assert_r(mServerDoc, false);
	aos_assert_r(mObjid != "", false);
	aos_assert_r(mResolveObjid, false);
	OmnString objid = mServerDoc->getAttrStr(AOSTAG_OBJID);

	//public  mWithCid
	//if (!mWithCid) return true;
	// The objid should be in the form:
	// 		orig_objid + "(" + objid_num_used + ")"
	OmnString prefix1 = objid;
	OmnString  cc;
	if (!mDocPublic)
	{
		AosObjid::decomposeObjid(objid, prefix1, cc);
	}
	OmnString prefix, num;
	//aos_assert_r(decomposeDupObjid(prefix1, prefix, num), false);
	//aos_assert_r(decomposeDupObjid(objid, prefix, num), false);
//	if (mDocPublic)
//	{
	//	aos_assert_r(num != "", false);
//	}
//	else
//	{
//		if (mWithCid) aos_assert_r( num != "", false);
//	}
	return true;
}


bool
AosCreateObjTester::checkParents(const AosRundataPtr &rdata)
{
	// 1. The original doc does not have a parent. In this case, 
	//    the doc should contain only one parent. If the doc
	//    is private, the parent should be private. Otherwise, 
	//    the parent should public. 
	// 2. The original doc did specify parents. It may have multiple
	//    parents. For each parent, check the following:
	//    a. If the parent is public:
	//       i.  If the original parent is private, the parent 
	//           should still be private.
	//       ii. If the original parent is public, the parent
	//           should still be public.
	//    b. If the parent is private:
	//       The parent should be private regardless of whether
	//       the original parent is public or private.
	// 3. If the original parents have duplicated parents, 
	//    the duplicated parents should be removed. 
	aos_assert_r(mServerDoc, false);
	OmnString ctnr_objid = mServerDoc->getAttrStr(AOSTAG_PARENTC);
	aos_assert_r(ctnr_objid != "", false);
	if (mParentObjidStr == "")
	{
		aos_assert_r(ctnr_objid != "", false);
		aos_assert_r(mThread->addContainer(ctnr_objid, 
					AosStContainer1::eDocCtnr, rdata), false);

		OmnString prefix, cid;
		AosObjid::decomposeObjid(ctnr_objid, prefix, cid);
		if (mParentPublic)
		{
			// The container should be public. 
			aos_assert_r(cid == "", false);
			aos_assert_r(prefix == ctnr_objid, false);
		}
		else
		{
			// The container should be private. 
			aos_assert_r(cid != "", false);
			aos_assert_r(cid == mThread->getCrtCid(), false);
		}
		return true;
	}

	OmnString prefix, cid;
	AosObjid::decomposeObjid(ctnr_objid, prefix, cid);
	if (mParentPublic)
	{
		// Parent should be public. 
		aos_assert_r(ctnr_objid == mParentObjidStr, false);
	}
	else
	{
		// Parent should be private
		if (!mValidParent)
		{
			aos_assert_r(cid != "", false);
			OmnString ccc = mThread->getCrtCid();
			aos_assert_r(cid == mThread->getCrtCid(), false);
		}
	}
	aos_assert_r(mThread->addContainer(ctnr_objid, AosStContainer1::eDocCtnr, rdata), false);
	return true;
}


bool
AosCreateObjTester::decomposeDupObjid(
				const OmnString &objid,
				OmnString &prefix,
				OmnString &num)
{
	// This function assumes 'objid' is in the form:
	// 		xxxx(nnn)
	// It retrieves 'nnn'.
	const int len = objid.length();
	aos_assert_r(len>0, false);
	const char *data = objid.data();
	aos_assert_r(data[len-1] == ')', false);
	for (int i=len-1; i>=0; i--)
	{
		if (data[i] == '(')
		{
			aos_assert_r (i<len-1-num.length(), "");
			prefix.assign(data, i);
			num.assign(&data[i+1], len-i-1);
			return true;
		}
	}
	prefix = objid;
	num = "";
	return false;
}


bool
AosCreateObjTester::determinePublic()
{
	// Determine whether the parent container should be
	// public or private. 
	mParentPublic = false;
	if (OmnRandom::percent(mWtUsePubCtnr))
	{
		mRawDoc->setAttr(AOSTAG_CTNR_PUBLIC, "true");
		mParentPublic = true;
	}
	
	// Determine whether to use private objid
	mDocPublic = false;
	if (OmnRandom::percent(mWtUsePubDoc))
	{
		mRawDoc->setAttr(AOSTAG_PUBLIC_DOC, "true");
		mDocPublic = true;
		if (mObjid != "") mThread->removeCid(mObjid);
	}

	return true;
}


bool
AosCreateObjTester::determineObjid()
{
	// Determine whether to use a unique objid
	mObjidUnique = true;
	OmnString cid;
	//if (OmnRandom::percent(mWtWithCid)) cid = determineCid();
	cid = determineCid();
	mWithCid = false;
	mLog << ":" << __LINE__;
	switch (OmnRandom::percent(
				mWtUseNoObjid, 
				mWtUseNewObjid,
				mWtUseExistingObjid))
	{
	case 0:
		 // Use no objid
		 mObjid = "";
		 mRawDoc->removeAttr(AOSTAG_OBJID);
		 mWithCid = false;
		 mCid = "";
		 mLog << ":" << __LINE__;
		 return true;

	case 1:
		 // Use a new objid
		 mObjid = mThread->pickNewObjid();
		 aos_assert_r(mObjid != "", false);
		 mObjidUnique = true;
		 mLog << ":" << __LINE__ << ":" << mObjid;
		 if (OmnRandom::percent(mWtWithCid) && cid != "")
		 {
			 mObjid = AosObjid::compose(mObjid, cid);
			 aos_assert_r(mObjid != "", false);
			 mWithCid = true;
			 mCid = cid;
		 	 mLog << ":" << __LINE__ << ":" << mObjid << ":" << mCid;
		 }
		 mRawDoc->setAttr(AOSTAG_OBJID, mObjid);
		 return true;

	case 2:
		 // Use an existing objid
		 mObjid = mThread->pickUsedObjid();
		 aos_assert_r(mObjid != "", false);
		 mLog << ":" << __LINE__ << ":" << mObjid;
		 if (OmnRandom::percent(mWtWithCid) && cid != "")
		 {
			 AosObjid::attachCidAsNeeded(mObjid, cid);
			 mCid = cid;
			 mWithCid = true;
		 	 mLog << ":" << __LINE__ << ":" << mObjid;
			 mObjidUnique = !mThread->isObjidUsed(mObjid);
		 }
		 else
		 {
		 	mLog << ":" << __LINE__ ;
			if (!mDocPublic)
			{
		 	 	 mLog << ":" << __LINE__ ;
				 cid = mThread->getCrtCid();
				 AosObjid::attachCidAsNeeded(mObjid, cid);
				 mCid = cid;
				 mWithCid = true;
				 mObjidUnique = !mThread->isObjidUsed(mObjid);
		 	 	 mLog << ":" << __LINE__  << ":" << mObjidUnique << ":" << mCid;
			}
			else
			{
		 	 	mLog << ":" << __LINE__ ;
		 		mObjidUnique = false;
			}
		 }
		 mLog << ":" << __LINE__  << ":" << mObjid;
		 mRawDoc->setAttr(AOSTAG_OBJID, mObjid);
		 return true;

	default:
		 OmnAlarm << "Percent error" << enderr;
		 break;
	}

	OmnShouldNeverComeHere;
	return false;
}


OmnString
AosCreateObjTester::determineCid()
{
	return mThread->getCrtCid();
	// Determine whether to attach cid
	switch (OmnRandom::percent(
				mWtUseValidCid, 
				mWtUseOtherCid,
				mWtUseInvalidCid))
	{
	case 0:
		 // Use the valid cid, if possible
		 return mThread->getCrtCid();

//			 if (cid != "")
//			 {
//				bool rslt = AosObjid::attachCidAsNeeded(mObjid, cid);
//				if (mThread->doesObjidExist(mObjid))
//				{
//					mObjidUnique = false;
//				}
//				else
//				{
//					mObjidUnique = true;
//				}
//				mWithCid = true;
//			 }
//			 else
//			 {
//				 mWithCid = false;
//			 }
//		 aos_assert_r(mObjid != "", false);
//		 mRawDoc->setAttr(AOSTAG_OBJID, mObjid);
//		 return true;

	case 1:
		 // Use other CID
		 return mThread->pickCid();

	case 2:
		 // Use an invalid cid
		 return mThread->pickInvalidCid();

	default:
		 break;
	}
	OmnShouldNeverComeHere;
	return "";
}


/*
bool
AosCreateObjTester::createDoc()
{
	// This function creates the doc on the server.
	//mServerDoc = 0;
	//OmnString server_docid;
	//OmnString res = (mResolveObjid)?"true":"false";
	//AosRundataPtr rdata = mThread->getRundata();
	JmoCallData rdata = mMgr->getCallData();
	JmoRundata _return;
	//string objid = "string";
	//if (objid != "" )
	//{
	//AosXmlTagPtr doc  = AosSengAdmin::getSelf()->retrieveDocByObjid(
	//						mThread->getSiteid(), 
	//						mThread->getCrtSsid(), 
	//						mThread->getUrlDocid(),
	//						mRawDoc->getAttrStr(AOSTAG_OBJID), 
	//						true 
	//						);
	//aos_assert_r(doc, false);
	//	gThriftClient->getDocByObjid(_return, rdata, objid);
	//	if(_return.rcode != JmoReturnCode::SUCCESS)
	//		 OmnAlarm << "can't get the doc" << enderr;
	//}

	//mResp = AosSengAdmin::getSelf()->createDoc(
	//					mRawDoc, 
	//					mThread->getCrtSsid(), 
	//					mThread->getUrlDocid(), 
	//					"", 
	//					res, 
	//					"false", 
	//					rdata);

	//mRequest = rdata->getArg1(AOSARG_REQUEST_STR);
	//mResponse = rdata->getArg1(AOSARG_RESPONSE_STR);
	string container_objid;
	string doc;
	JmoDocType::type doc_type;
	string schema;

	gThriftClient->createDoc(_return, rdata, container_objid, mObjid, doc, doc_type, schema);
	if(_return.rcode != JmoReturnCode::SUCCESS)
		 OmnAlarm << "can't create doc" << enderr;
	else
	{
		gThriftClient->getDocByObjid(_return, rdata, objid);    
		if(_return.rcode != JmoReturnCode::SUCCESS)
			 OmnAlarm << "can't create doc" << enderr;
	}
		return true;
	
	
	//if (mResp)
	//{
	//	OmnString objid = mResp->getAttrStr(AOSTAG_OBJID);
	//	aos_assert_r(objid != "", false);
	//	mServerDoc = AosSengAdmin::getSelf()->retrieveDocByObjid(
	//						mThread->getSiteid(), 
	//						mThread->getCrtSsid(), 
	//						mThread->getUrlDocid(),
	//						objid, 
	//						true 
	//						);
	//	aos_assert_r(mServerDoc, false);

	//	u64 rdocid = mRawDoc->getAttrU64(AOSTEST_DOCID, AOS_INVDID);
	//	u64 sdocid = mServerDoc->getAttrU64(AOSTEST_DOCID, AOS_INVDID);
	//	aos_assert_r(rdocid == sdocid, false);
	//	aos_assert_r(mServerDoc->getAttrStr(AOSTAG_OBJID) == objid, false);
	//	aos_assert_r(mServerDoc->getAttrU64(AOSTAG_DOCID, 0) == 
	//			mResp->getAttrU64(AOSTAG_DOCID, 0), false);
	//	aos_assert_r(mRawDoc->getAttrU64(AOSTEST_DOCID, AOS_INVDID) ==
	//				mServerDoc->getAttrU64(AOSTEST_DOCID, AOS_INVDID), false);
	//}
}
*/


bool
AosCreateObjTester::createDoc()
{
	// This function creates the doc on the server.
	
	JmoCallData call_data = mMgr->getCallData();
	JmoRundata _return;
	OmnString doc;
	string doc_container = "yunyuyan_account";
	string doc_modifier = "prod";
	//string doc_objid = "3245431";
	//string doc_cid = "3242312";

	doc	<< "<embedobj "
		<< "zky_pctrs=\"" << doc_container << "\" "                                 
		<< "zky_modifier=\"" << doc_modifier << "\">"                                 
	    << "</embedobj>";

	gThriftClient->createDoc1(_return, call_data, doc);
	aos_assert_r(_return.rcode == JmoReturnCode::SUCCESS, false);
	OmnString doc_str = _return.str_value;
	AosXmlTagPtr doc_xml = AosXmlParser::parse(doc_str AosMemoryCheckerArgs);

	OmnString doc_xml_objid = doc_xml->getAttrStr("zky_objid");
	mEntries.push_back(doc_xml_objid.data());
	
	return true;
	
}


bool
AosCreateObjTester::resolveObjid()
{
	// It randomly determines whether to resolve the objid. 
	// If yes and if the objid is not unique, the server will
	// resolve the objid to be unique.
	mResolveObjid = OmnRandom::percent(mResolveObjid);
	return true;
}


bool
AosCreateObjTester::pickMultipleContainers()
{
	// This function picks multiple containers. In the current
	// implementations, a doc must have one and only one container.
	// If there are multiple containers, the remaining are put 
	// to memberof. 
	// This function will randomly determine multiple containers. 
	// For each container, it may pick an existing one or a 
	// new one.
	int num = OmnRandom::intByRange(
			2, 3, 80, 
			4, 6, 15, 
			7, 10, 5); 
	mParentObjids.clear();
	mParentObjidTypes.clear();
	mParentObjidStr = "";
	aos_assert_r(num >= 2, false);
	for (int i=0; i<num; i++)
	{
		bool picked = false;
		if (OmnRandom::percent(80))
		{
			AosStContainerPtr ctnr = mThread->pickValidContainer();
			if (ctnr)
			{
				mParentObjidTypes.push_back(true);
				mParentObjids.push_back(ctnr->mObjid);
				picked = true;
			}
		}

		if (!picked)
		{
			mParentObjidTypes.push_back(false);
			mParentObjids.push_back(mThread->pickNewObjid());
		}
	}
	
	mParentObjidStr = mParentObjids[0];
	/*
	for (u32 i=1; i<mParentObjids.size(); i++)
	{
		mParentObjidStr << "," << mParentObjids[i];
	}
	*/
	return true;
}

