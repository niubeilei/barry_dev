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
// There are three attribute collections:
// M = {M1, M2, ..., M}: the combinations of these attributes identify Families;
// C = {C1, C2, ..., Cn}: a collection of attributes of "Families" identified by M;
// B = {B1, B2, ..., Bn}: a collection of attributes whose values identify 
//     					  members of "Families".
//
// Given a data d in Data Collection D, the values M(d) identifies a Family F, 
// F's attributes C(d), and a member B(d) of the family F. This smart doc 
// creates the following table:
//	    -------------------------
// 		C	   |	Family Counts
//	    -------------------------
//	    c      |    n
//	    c      |    n 
//	    ...    |    ...
//	    -------------------------
//
// where each 'c' is a different combination of C (i.e., different combination
// of the values of the attributes C = {C1, C2, ..., Cn} of families M.
//
// Example
// In this example, a family has an attribute "family_type", whose values can be
// 		no_kids
// 		single_parent
// 		lived_together
// 		married
//
// We want to build the following table:
// 		--------------------------
// 		family type		|	number
// 		--------------------------
// 		no_kids			|	16036
// 		single_parent	|	2634
// 		lived_together	|	3646
// 		married			|   236403
// 		--------------------------
//
// If "Family" is defined by an object (ideally), this can be done by querying.
// This class assumes "Family" is not defined by an object.
//
// Algorithm:
// 1. Given a d, it extracts M values. 
// 2. Calculate key K by concatenating all values M into a string.
// 3. Calculate SHA1 value for K. This value is the key that identifies Fs.
// 4. Create the objid:
// 			AOSZTG_RELATION + "_" + Smartdoc Docid + "_" + SHA1
// 5. Check whether the objid exists. 
// 6. If the doc does not exist yet, create it.
// 7. Get the name of the attribute that stores the members. If it is specified,
//    add the d->objid to the doc.
// 8. Get the name of the attribute that counts the number of members. If 
//    specified, increment the number. 
//
// Modification History:
// 11/06/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocRelation2106.h"

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

#if 0

AosSdocRelation2106::AosSdocRelation2106(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_RELATION2106, AosSdocId::eRelation2106, flag)
{
}


AosSdocRelation2106::~AosSdocRelation2106()
{
}


bool
AosSdocRelation2106::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
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

	// 3. Calculate the objid
	OmnString objid = calculateObjid(m_values, sdoc, rdata);
	aos_assert_rr(objid != "", rdata, false);
	rdata->setArg1(AOSARG_OBJID, objid);

	// 4. Retrieve or create the doc
	AosXmlTagPtr family_doc = getFamilyDoc(objid, sdoc, rdata);
	aos_assert_rr(family_doc, rdata, false);

	// 5. If the 'members' attribute is specified, add the member.
	OmnString aname = sdoc->getAttrStr(AOSTAG_MEMBER_ANAME);
	if (aname != "")
	{
		// Calculate the member id
		AosValueRslt value;
		bool rslt = AosValueSel::getValueStatic(value, sdoc, "member_id", rdata);
		if (!rslt)
		{
			OmnAlarm << "Failed retrieve the member id: " << rdata->getErrmsg() << enderr;
		}
		else
		{
			OmnString member_id = value.getValueStr1();
			if (member_id == "")
			{
				OmnAlarm << "Member id is empty" << enderr;
			}
			else
			{
				OmnString oldvalue = family_doc->getAttrStr(aname);
				OmnString newvalue;
				if (oldvalue == "")
				{
					newvalue = member_id;
				}
				else
				{
					newvalue = oldvalue;
					newvalue << "," << member_id;
				}
				bool rslt = AosModifyDocAttrStr(rdata, family_doc, 
						AOSTAG_MEMBER_ANAME, 
						newvalue, false, false);
				if (!rslt)
				{
					OmnAlarm << "Failed modifying attribute: " << rdata->getErrmsg() << enderr;
				}
			}
		}
	}

	// 6. If the 'number of members' attribute is specified, increment its value
	aname = sdoc->getAttrStr("num_members_aname");
	if (aname != "")
	{
		OmnString nn = family_doc->getAttrStr(aname);
		if (nn == "")
		{
			bool rslt = AosModifyDocAttrStr(rdata, family_doc, 
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
			bool rslt = AosModifyDocAttrStr(rdata, family_doc, 
					aname, newvalue, false, false);
			aos_assert_rr(rslt, rdata, false);
		}
	}
	rdata->setOk();
	return true;
}


OmnString 
AosSdocRelation2106::calculateObjid(
		const vector<OmnString> &m_values, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	// This function calculates the objid for the virtual container object. 
	// It is calculated as:
	// 	AOSZTG_RELATION + "_" + virtual container id + "_" + SHA1
	aos_assert_rr(sdoc, rdata, "");
	aos_assert_rr(m_values.size() > 0, rdata, "");
	OmnString vid = sdoc->getAttrStr("virtual_ctnr_id");
	if (vid == "")
	{
		AosSetError(rdata, AosErrmsgId::eMissingVirtualCtnrId);
		OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << sdoc->toString() << enderr;
		return "";
	}

	OmnString nn;
	for (u32 i=0; i<m_values.size(); i++)
	{
		nn << "_" << m_values[i];
	}
	OmnString sha1 = AosCalSHA1(nn);
	AosSdocUtil::normalizeSha1(sha1);
	OmnString objid = AOSZTG_RELATION;
	objid << "_" << vid << "_" << sha1;
	return objid;
}


AosXmlTagPtr 
AosSdocRelation2106::getFamilyDoc(
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

#endif
