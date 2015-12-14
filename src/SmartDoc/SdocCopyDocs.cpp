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
// 08/19/2011	Created by Felicia
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocCopyDocs.h"

#include "Actions/ActSeqno.h"
#include "ValueSel/ValueSel.h"
#include "ValueSel/ValueRslt.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEUtil/IILName.h"
#include "Rundata/Rundata.h"
#include "UtilComm/TcpClient.h"
#include "XmlInterface/WebRequest.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

AosSdocCopyDocs::AosSdocCopyDocs(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_COPYDOCS, AosSdocId::eCopyDocs, flag)
{
}


AosSdocCopyDocs::~AosSdocCopyDocs()
{
}

bool
AosSdocCopyDocs::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) 
{
	// This smart doc copies all docs of a given container to another
	// container. Note that it copies, not moves docs. 
	// 	<sdoc type="xxx" is_public="true|false" cid_required="true|false" prefix="xxx">
	// 		<record octnr_objid="xxx" tctnr_objid="xxx">
	// 		     <record octnr_objid="xxx" tctnr_objid="xxx">
	// 		        .....
	// 		     </record>
	// 		     .....
	// 		</record>
	// 		.....
	// 	</sdoc>
	
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// 1. get the prefix
	// prefix is for the new doc_objid
	// the new doc_objid will add the prefix
	OmnString prefix = sdoc->getAttrStr("prefix");
	if(prefix == "")
	{
		rdata->setError() << "Missing prefix!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool is_public = sdoc->getAttrBool("is_public");
	bool cid_required = sdoc->getAttrBool("cid_required");


	AosXmlTagPtr rec = sdoc->getFirstChild();
	while(rec)
	{
		bool rslt = copyDocs(rec, prefix, is_public, cid_required, rdata);
		aos_assert_rr(rslt, rdata, false);
		rec = sdoc->getNextChild();
	}

	return true;
}


bool
AosSdocCopyDocs::copyDocs(
		const AosXmlTagPtr &precord,
		const OmnString &prefix,
		const bool is_public,
		const bool cid_required,
		const AosRundataPtr &rdata)
{
	OmnString octnr_objid = precord->getAttrStr("octnr_objid");
	OmnString tctnr_objid = precord->getAttrStr("tctnr_objid");
	aos_assert_rr(octnr_objid != "", rdata, false);
	aos_assert_rr(tctnr_objid != "", rdata, false);
	OmnString iilname = AosIILName::composeCtnrMemberObjidIILName(tctnr_objid);
	aos_assert_rr(iilname != "", rdata, false);
	u64 docids[eMaxNumDocs];
	int idx = -10;
	int iilidx = -10;
	OmnString failed_docids;
	while (1)
	{
		int oldiilidx = iilidx;
		int oldidx = idx;
		//int num = AosIILClient::getSelf()->getDocids(iilname, iilidx, idx, docids, eMaxNumDocs, rdata);
		int num = AosIILClientObj::getIILClient()->getDocids(iilname, iilidx, idx, docids, eMaxNumDocs, rdata);
		if (num == 0)
		{
			// It finished. 
			rdata->setOk();
			return true;
		}

		aos_assert_rr(iilidx >= oldiilidx, rdata, false);
		if (iilidx == oldiilidx)
		{
			aos_assert_rr(idx > oldidx, rdata, false);
		}

		aos_assert_rr(num > 0, rdata, false);
		OmnString otype, objid, newobjid = prefix;
		for (int i=0; i<num; i++)
		{
			AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByDocid(docids[i], rdata);
			if (!doc)
			{
				OmnAlarm << "Failed retrieving doc: " << docids[i] << enderr;
				continue;
			}
			
			objid = doc->getAttrStr(AOSTAG_OBJID);
			otype = doc->getAttrStr(AOSTAG_OTYPE);			
			if(otype == AOSTAG_PARENTC)
			{
			
				AosXmlTagPtr childrec = precord->getChildByAttr("octnr_objid", objid);
				if(!childrec)
				{
					
					OmnString recstr = "<record octnr_objid=\"";
					recstr << objid << "\" tctnr_objid=\"" 
						<< newobjid << "_" << objid << "\" />";
					AosXmlParser parser;
					childrec = parser.parse(recstr, "" AosMemoryCheckerArgs);
					aos_assert_rr(childrec, rdata, false);
				}
				
				bool rslt = copyDocs(childrec, prefix, is_public, cid_required, rdata);
				aos_assert_rr(rslt, rdata, false);
				continue;
			}
			
			newobjid << "_" <<  objid;
			doc->setAttr(AOSTAG_OBJID, objid);
				
			doc->removeAttr(AOSTAG_DOCID);
			doc->removeAttr(AOSTAG_CTIME);
			doc->removeAttr(AOSTAG_MTIME);
			doc->removeAttr(AOSTAG_VERSION);
			doc->setAttr(AOSTAG_PARENTC, tctnr_objid);

			// Ready to create the doc
			bool rslt = AosDocClientObj::getDocClient()->createDocSafe3(rdata, 
					doc, rdata->getCid(), "", is_public, true, false, 
					false, cid_required, true, true);
			if (!rslt)
			{
				if (failed_docids != "") failed_docids << ";";
				failed_docids << docids[i];
			}
		}

		if (num >= eMaxNumDocs) break;
	}

	rdata->setOk();
	return true;
}

