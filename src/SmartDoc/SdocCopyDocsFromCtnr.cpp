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
// 07/22/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocCopyDocsFromCtnr.h"

#include "Actions/ActSeqno.h"
#include "ValueSel/ValueSel.h"
#include "ValueSel/ValueRslt.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEUtil/IILName.h"
#include "Rundata/Rundata.h"
#include "UtilComm/TcpClient.h"
#include "XmlInterface/WebRequest.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

#if 0

AosSdocCopyDocsFromCtnr::AosSdocCopyDocsFromCtnr(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_COPYDOCS_FROMCTNR, AosSdocId::eCopyDocsFromCtnr, flag)
{
}


AosSdocCopyDocsFromCtnr::~AosSdocCopyDocsFromCtnr()
{
}

bool
AosSdocCopyDocsFromCtnr::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) 
{
	// This smart doc copies all docs of a given container to another
	// container. Note that it copies, not moves docs. 
	// 	<sdoc type="xxx" is_public="true|false" cid_required="true|false">
	// 		<orig_ctnr .../>
	// 		<target_ctnr .../>
	// 	</sdoc>
	
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosValueRslt orig_ctnr;
	bool rslt1;
	OmnString orig = orig_ctnr.getValueStr("", rslt1);
	bool rslt = AosValueSel::getValueStatic(orig_ctnr, sdoc, "orig_ctnr", rdata);
	if (!rslt || !orig_ctnr.isValid() || !rslt1 || orig == "")
	{
		rdata->setError() << "Failed retrieving the original container!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosValueRslt target_ctnr;
	OmnString target = target_ctnr.getValueStr("", rslt1);
	rslt = AosValueSel::getValueStatic(target_ctnr, sdoc, "target_ctnr", rdata);
	if (!rslt || !target_ctnr.isValid() || !rslt1 || target == "")
	{
		rdata->setError() << "Failed retrieving the target container!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool is_public = sdoc->getAttrBool("is_public");
	bool cid_required = sdoc->getAttrBool("cid_required");

	return copyDocs(orig, target, is_public, cid_required, rdata);
}


bool
AosSdocCopyDocsFromCtnr::copyDocs(
		const OmnString &origctnr_objid, 
		const OmnString &targetctnr_objid, 
		const bool is_public,
		const bool cid_required,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(origctnr_objid != "", rdata, false);
	aos_assert_rr(targetctnr_objid != "", rdata, false);
	OmnString iilname = AosIILName::composeCtnrMemberObjidIILName(origctnr_objid);
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
		for (int i=0; i<num; i++)
		{
			AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByDocid(docids[i], rdata);
			if (!doc)
			{
				OmnAlarm << "Failed retrieving doc: " << docids[i] << enderr;
				continue;
			}

			doc->removeAttr(AOSTAG_DOCID);
			doc->removeAttr(AOSTAG_OBJID);
			doc->removeAttr(AOSTAG_CTIME);
			doc->removeAttr(AOSTAG_MTIME);
			doc->removeAttr(AOSTAG_VERSION);
			doc->setAttr(AOSTAG_PARENTC, targetctnr_objid);

			// Ready to create the doc
			AosXmlTagPtr rslt = AosDocClientObj::getDocClient()->createDocSafe3(rdata, 
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

#endif
