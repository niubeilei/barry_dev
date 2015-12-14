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
// Given a collection of data D, for each data d in D, its value:
// 			(m1, m2, ...)
// identifies a 'Virtual Object' M, and its value:
// 			(b1, b2, ...)
// identifies a 'Virtual Object' B as a member of M. This relation 
// creates the following 'Virtual Table':
//      -----------------------------------------------------
// 		|   (m1, m2, ...)      |   (b1, b2, ...)            |
// 		-----------------------------------------------------
//
// The objid of the 'Virtual Object' M is constructed as:
// 		AOSZTG_RELATION + "_" + Smartdoc Docid + "_m_" + SHA1(m1, m2, ...)
// where SHA1 is the SHA1 value of:
// 			m1 + "_" + m2 + "_" ...
// The objid of the 'Virtual Object' B is constructed as:
// 		AOSZTG_RELATION + "_" + Smartdoc Docid + "_m_" + SHA1(b1, b2, ...)
//
// This class will create the following objects:
// 1. It may create an object for each combination of (m1, m2, ...). 
//    These objects are stored in a given container. Its SHA1 value is included 
//    in the object. This is optional. 
// 2. It may create an object for each combination of (b1, b2, ...). 
//    These objects are stored in a given container. Its SHA1 value is included
//    in the object. THis is optional.
// 3. A counter:
// 			b.m.container.counter_id
// 	  may be created, where 'b' is the objid of (b1, b2, ...), 'm' is the 
// 	  objid of (m1, m2, ...). 
// 4. It may create a Relation Object whose values are from the object d:
// 		Relation Object:
// 			m_objid: xxx (from (m1, m2, ...))
// 			b_objid: xxx (from (b1, b2, ...))
// 			field1: xxx (from b)
// 			field2: xxx (from b)
// 			...
// 
// Modification History:
// 11/06/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocRelation2107.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Actions/SdocAction.h"
#include "CounterClt/CounterClt.h"
#include "SmartDoc/SdocUtil.h"
#include "ValueSel/VsUtil.h"
#include "XmlUtil/SeXmlParser.h"

AosSdocRelation2107::AosSdocRelation2107(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_RELATION2107, AosSdocId::eRelation2107, flag)
{
}


AosSdocRelation2107::~AosSdocRelation2107()
{
}


bool
AosSdocRelation2107::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// Refer to the comments at the beginning of this file.
	//
	// <sdoc ..
	
	// Chen Ding, 2013/06/10
	OmnNotImplementedYet;
	return false;
	/*
	aos_assert_rr(sdoc, rdata, false);

	// if (rdata->isBatchProcess()) return batchProcess(sdoc, rdata);

	// 1. Retrieve the working doc
	AosXmlTagPtr working_doc = AosRunDocSelector(rdata, sdoc, AOSTAG_WORKING_DOC);
	if (!working_doc)
	{
		working_doc = rdata->getWorkingDoc();
		if (!working_doc)
		{
			AosSetError(rdata, AosErrmsgId::eMissingWorkingDoc);
			OmnAlarm << rdata->getErrmsg() << ". Missing working doc: " 
				<< sdoc->toString() << enderr;
			return false;
		}
	}

	// 2. Retrieve M values
	vector<OmnString> m_values;
	OmnString m_objid;
	AosXmlTagPtr mvaluesdoc = sdoc->getFirstChild("mvalues");
	if (mvaluesdoc)
	{
		bool rslt = AosSdocUtil::retrieveAttrValues(m_values, working_doc, sdoc, "mvalues", rdata);	
		aos_assert_rr(rslt, rdata, false);
		if (m_values.size() == 0)
		{
			AosSetError(rdata, AosErrmsgId::eMissingMValues);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		//  Calculate M's objid
		OmnString m_shavalue;
		m_objid = calculateObjid(m_values, sdoc, m_shavalue, rdata);
		aos_assert_rr(m_objid != "", rdata, false);
		rdata->setArg1(AOSARG_MSHA_VALUE, m_shavalue);
		rdata->setArg1(AOSARG_M_OBJID, m_objid);
	}
	
	// 3. retrieve B values
	vector<OmnString> b_values;
	OmnString b_objid;
	AosXmlTagPtr bvaluesdoc = sdoc->getFirstChild("bvalues");
	if (bvaluesdoc)
	{
		bool rslt = AosSdocUtil::retrieveAttrValues(b_values, working_doc, sdoc, "bvalues", rdata); 
		aos_assert_rr(rslt, rdata, false);
		if (b_values.size() == 0)
		{
			AosSetError(rdata, AosErrmsgId::eMissingMValues);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		// Calculate B's objid
		OmnString b_shavalue;
		b_objid = calculateObjid(b_values, sdoc, b_shavalue, rdata);
		aos_assert_rr(b_objid != "", rdata, false);
		rdata->setArg1(AOSARG_MSHA_VALUE, b_shavalue);
		rdata->setArg1(AOSARG_B_OBJID, b_objid);
	}

	// 6. retrieve or create the doc
	AosXmlTagPtr mdatafield = sdoc->getFirstChild("mdatafield");
	if (mdatafield)
	{
		AosXmlTagPtr doc = getDoc(m_objid, mdatafield, rdata);
		aos_assert_rr(doc, rdata, false);
		incrementValue(mdatafield, doc, rdata);
		rdata->setSourceDoc(doc, true);
	}

	AosXmlTagPtr workingdoc = rdata->setWorkingDoc(working_doc, false);
	AosXmlTagPtr bdatafield = sdoc->getFirstChild("bdatafield");
	if (bdatafield)
	{
		AosXmlTagPtr doc = getDoc(b_objid, bdatafield, rdata);
		aos_assert_rr(doc, rdata, false);
		incrementValue(bdatafield, doc, rdata);
		rdata->setTargetDoc(doc, true);
	}
	rdata->setWorkingDoc(workingdoc, false);

	//7. Retrieve or create the relation doc as needed (note that we may or
	//		may not need to create the relation doc).
	workingdoc = rdata->setWorkingDoc(working_doc, false);
	OmnString relationobjid = calculateRelationObjid(m_objid, b_objid, rdata);
	aos_assert_rr(relationobjid != "", rdata, false);
	AosXmlTagPtr relation_sdoc = sdoc->getFirstChild("relationobj");
	if (relation_sdoc)
	{
		rdata->setArg1(AOSARG_RELATION_OBJID, relationobjid);
		AosXmlTagPtr relation_doc = getDoc(relationobjid, relation_sdoc, rdata);
		aos_assert_rr(relation_doc, rdata, false);
		incrementValue(relation_sdoc, relation_doc, rdata);
	}
	rdata->setWorkingDoc(workingdoc, false);

	// We may need to create the counter.
	bool is_createclt = sdoc->getAttrBool("zky_iscreateclt");
	if (is_createclt)
	{
		OmnString cname;
		for (u32 i=0; i<b_values.size(); i++)
		{
			cname << b_values[i];
		}
		cname << ".";
		for (u32 i=0; i<m_values.size(); i++)
		{
			cname << m_values[i];
		}
		cname << "." << working_doc->getAttrStr(AOSTAG_PARENTC, "");

		// Chen Ding, 04/09/2012
		// Need to determine the counter id
		// AosCounterClt::getSelf()->procCounter(cname, rdata);
		OmnNotImplementedYet;
		return false;
	}
	rdata->setOk();
	return true;
	*/
}


OmnString 
AosSdocRelation2107::calculateObjid(
		const vector<OmnString> &values, 
		const AosXmlTagPtr &sdoc,
		OmnString &sha1,
		const AosRundataPtr &rdata)
{
	// This function calculates the objid for the virtual container object. 
	// It is calculated as:
	// 	AOSZTG_RELATION + "_" + virtual container id + "_" + SHA1
	aos_assert_rr(sdoc, rdata, "");
	aos_assert_rr(values.size() > 0, rdata, "");
	OmnString vid = sdoc->getAttrStr("virtual_ctnr_id");
	if (vid == "")
	{
		AosSetError(rdata, AosErrmsgId::eMissingVirtualCtnrId);
		OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << sdoc->toString() << enderr;
		return "";
	}

	OmnString nn;
	for (u32 i=0; i<values.size(); i++)
	{
		nn << "_" << values[i];
	}
	sha1 = AosCalSHA1(nn);
	AosSdocUtil::normalizeSha1(sha1);
	OmnString objid = AOSZTG_RELATION;
	objid << "_" << vid << "_" << sha1;
	return objid;
}


AosXmlTagPtr 
AosSdocRelation2107::getDoc(
		const OmnString &objid, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(sdoc, rdata, 0);
	aos_assert_rr(objid != "", rdata, 0);
	AosXmlTagPtr doc = AosGetDocByObjid(objid, rdata);
	if (doc) return doc;

	// The doc is not there yet. Need to create it.
	AosXmlTagPtr def = sdoc->getFirstChild("doc_creator");
	if (!def)
	{
		AosSetError(rdata, AosErrmsgId::eMissingDocCreator);
		OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << sdoc->toString() << enderr;
		return 0;
	}

	if (AosActionType::toEnum(def->getAttrStr(AOSTAG_ZKY_TYPE)) != AosActionType::eCreateDoc)
	{
		AosSetError(rdata, AosErrmsgId::eNotDocCreator);
		OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << sdoc->toString() << enderr;
		return 0;
	}

	bool rslt = AosSdocAction::runAction(def, rdata);
	aos_assert_rr(rslt, rdata, 0);
	return rdata->getCreatedDoc();
}


OmnString
AosSdocRelation2107::calculateRelationObjid(
				const OmnString &m_objid,
				const OmnString &b_objid,
				const AosRundataPtr &rdata)
{
	OmnString nn = m_objid;
	nn << b_objid;
	OmnString sha1 = AosCalSHA1(nn);
	AosSdocUtil::normalizeSha1(sha1);
	OmnString objid = AOSZTG_RELATION;
	objid << "_" << sha1;
	return objid;
}


bool
AosSdocRelation2107::incrementValue(
			const AosXmlTagPtr &sdoc,
			const AosXmlTagPtr &family_doc,
			const AosRundataPtr &rdata)
{
	// Chen Ding, 2013/06/10
	OmnNotImplementedYet;
	return false;
	/*
	OmnString aname = sdoc->getAttrStr("num_members_aname");
	if (aname != "")
	{
		OmnString nn = family_doc->getAttrStr(aname);
		if (nn == "")
		{
			bool rslt = AosModifyAttrStr1(rdata,
					family_doc->getAttrU64(AOSTAG_DOCID, 0),
					family_doc->getAttrStr(AOSTAG_OBJID, ""),
					aname, "1", false, false, true);
			aos_assert_rr(rslt, rdata, false);
			family_doc->setAttr(aname, 1);
		}
		else
		{
			int nnnn = atoi(nn.data());
			aos_assert_rr(nnnn > 0, rdata, false);
			nnnn++;
			OmnString newvalue;
			newvalue << nnnn;
			bool rslt = AosModifyAttrStr1(rdata,
					family_doc->getAttrU64(AOSTAG_DOCID, 0),
					family_doc->getAttrStr(AOSTAG_OBJID, ""),
					aname, newvalue, false, false, true);
			aos_assert_rr(rslt, rdata, false);
			family_doc->setAttr(aname, newvalue);
		}
	}
	return true;
	*/
}


