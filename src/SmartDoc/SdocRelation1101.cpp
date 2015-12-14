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
// Modification History:
// 11/16/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocRelation1101.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "SEInterfaces/DocClientObj.h"
#include "DocSelector/DocSelector.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SdocUtil.h"
#include "ValueSel/ValueRslt.h"
#include "ValueSel/ValueSel.h"
#include "ValueSel/VsUtil.h"



AosSdocRelation1101::AosSdocRelation1101(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_RELATION1101, AosSdocId::eRelation1101, flag)
{
}


AosSdocRelation1101::~AosSdocRelation1101()
{
}


bool
AosSdocRelation1101::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// Refer to the comments at the beginning of this file.
	//
	// 	<sdoc ...
	// 		members="xxx"
	// 		num_members="xxx">
	// 		<working_doc .../>	
	// 		<memberid .../>
	// 		<mvalues>
	// 			<valuesel .../>
	// 			<valuesel .../>
	// 			...
	//		</mvalues>	
	// 	</sdoc>
	
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
	bool rslt = AosSdocUtil::retrieveAttrValues(m_values, working_doc, sdoc, "mvalues", rdata);
	aos_assert_rr(rslt, rdata, false);
	if (m_values.size() == 0)
	{
		AosSetError(rdata, AosErrmsgId::eMissingMValues);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	rdata->setArg1(AOSARG_M_VALUE, getVectorValue(m_values));

	// 3. Retrieve B values
	vector<OmnString> b_values;
	rslt = AosSdocUtil::retrieveAttrValues(b_values, working_doc, sdoc, "bvalues", rdata);
	aos_assert_rr(rslt, rdata, false);
	if (b_values.size() == 0)
	{
		AosSetError(rdata, AosErrmsgId::eSmartDocSyntaxError);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	rdata->setArg1(AOSARG_M_VALUE, getVectorValue(m_values));

	// 4. Retrieve A values
	vector<OmnString> a_values;
	rslt = AosSdocUtil::retrieveAttrValues(a_values, working_doc, sdoc, "avalues", rdata);
	aos_assert_rr(rslt, rdata, false);
	if (a_values.size() == 0)
	{
		AosSetError(rdata, AosErrmsgId::eSmartDocSyntaxError);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	rdata->setArg1(AOSARG_A_VALUE, getVectorValue(a_values));

	// 5. Retrieve K values, optional
	vector<OmnString> k_values;
	rslt = AosSdocUtil::retrieveAttrValues(k_values, working_doc, sdoc, "kvalues", rdata);
	aos_assert_rr(rslt, rdata, false);
	rdata->setArg1(AOSARG_K_VALUE, getVectorValue(k_values));

	// 6. Retrieve C values, optional
	vector<OmnString> c_values;
	rslt = AosSdocUtil::retrieveAttrValues(c_values, working_doc, sdoc, "cvalues", rdata);
	aos_assert_rr(rslt, rdata, false);
	rdata->setArg1(AOSARG_C_VALUE, getVectorValue(c_values));

	// 7. Retrieve E values, optional
	vector<OmnString> e_values;
	rslt = AosSdocUtil::retrieveAttrValues(e_values, working_doc, sdoc, "evalues", rdata);
	aos_assert_rr(rslt, rdata, false);
	rdata->setArg1(AOSARG_E_VALUES, getVectorValue(e_values));

	// 8. Calculate the Objid
	OmnString objid = calculateObjid(m_values, b_values, a_values, k_values, rdata);
	rdata->setArg1(AOSARG_OBJID, objid); 

	// 9 Retrieve or create the doc
	AosXmlTagPtr relationdoc = getRelationDoc(objid, sdoc, rdata);
	aos_assert_rr(relationdoc, rdata, false); 
	
	// 10. If the 'number of members' attribute is specified, increment its value
	OmnString aname = sdoc->getAttrStr("num_members_aname");
	if (aname != "")
	{
		OmnString nn = relationdoc->getAttrStr(aname);
		if (nn == "")
		{
			bool rslt = AosModifyDocAttrStr(rdata, relationdoc, 
					aname, "1", false, false);
			aos_assert_rr(rslt, rdata, false);
		}
		else
		{
			int nnnn = atoi(nn.data());
			aos_assert_rr(nnnn > 0, rdata, false);
			nnnn++;
			OmnString newvalue;
			newvalue << nnnn;
			bool rslt = AosModifyDocAttrStr(rdata, relationdoc, 
					aname, newvalue, false, false);
			aos_assert_rr(rslt, rdata, false);
		}
	}
	rdata->setOk();
	return true;
}


OmnString
AosSdocRelation1101::calculateObjid(
			const vector<OmnString> m_values,
			const vector<OmnString> v_values,
			const vector<OmnString> a_values,
			const vector<OmnString> k_values,
			const AosRundataPtr &rdata)
{
	// This function calculates the objid for the virtual container object.
	// It is calculated as:
	//  AOSZTG_RELATION + "_" + SHA1
	OmnString key = getVectorValue(m_values); 
	key << getVectorValue(v_values)
		<< getVectorValue(a_values)
		<< getVectorValue(k_values);

	OmnString sha1 = AosCalSHA1(key);
	AosSdocUtil::normalizeSha1(sha1);
	OmnString objid = AOSZTG_RELATION;
	objid << "_" << sha1;
	return objid;
}


OmnString 
AosSdocRelation1101::getVectorValue(const vector<OmnString> values)
{
	OmnString nn = "";
	for (u32 i=0; i<values.size(); i++)
	{
	    nn << values[i];
	}
	return nn;
}


AosXmlTagPtr
AosSdocRelation1101::getRelationDoc(
		const OmnString &objid,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(sdoc, rdata, 0);
	aos_assert_rr(objid != "", rdata, 0);
	AosXmlTagPtr family_doc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	if (family_doc) return family_doc;

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

