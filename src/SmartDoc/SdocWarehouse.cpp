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
// 11/29/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocWarehouse.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Actions/ActCreateDoc.h"
#include "Actions/ActCreateLog.h"
#include "Actions/Ptrs.h"
#include "API/AosApi.h"
#include "DbQuery/Query.h"
//#include "SEInterfaces/DocClientObj.h"
#include "MultiLang/LangTermIds.h"
#include "Actions/SdocAction.h"
#include "SeLogClient/SeLogClient.h"
#include "SmartDoc/SmartDoc.h"
#include "XmlUtil/XmlTag.h"
#if 0

AosSdocWarehouse::AosSdocWarehouse(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_WAREHOUSE, AosSdocId::eWarehouse, flag)
{
}


AosSdocWarehouse::~AosSdocWarehouse()
{
}


bool
AosSdocWarehouse::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	aos_assert_r(rdata, false);
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve the working doc
	AosXmlTagPtr working_doc = rdata->getWorkingDoc();
	if (!working_doc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingWorkingDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve the warehouse ID
	OmnString aname = sdoc->getAttrStr("zky_warehsid_aname", "zky_warehouse_id");
	if (aname == "") aname = "zky_warehouse_id";
	OmnString warehouse_id = working_doc->getAttrStr(aname);
	if (warehouse_id == "")
	{
		AosSetError(rdata, AOSLT_MISSING_WAREHOUSE_ID);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve the Quantity Attribute name
	OmnString quantity_aname = sdoc->getAttrStr("quantity_aname", "zky_quantity");
	if (quantity_aname == "") quantity_aname = "zky_quantity";

	// Retrieve the subtag name
	OmnString tagname = sdoc->getAttrStr("zky_records_aname", "records");
	if (tagname == "") tagname = "records";
	AosXmlTagPtr records = working_doc->getFirstChild(tagname);
	if (!records)
	{
		AosSetError(rdata, AOSLT_MISSING_WAREHOUSE_ITEMS);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	/*
	// Clone the records
	AosXmlTagPtr records = objs->clone(AosMemoryCheckerArgsBegin);

	if (sdoc->getAttrBool("zky_need_remove_records", true))
	{
		// Remove the records
		int nn = working_doc->removeNode(tagname, false, false);
		if (nn != 1)
		{
			AosSetError(rdata, AosErrmsgId::eInternalError);
			OmnAlarm << rdata->getErrmsg() << ". Failed removing node: " 
				<< working_doc->toString() << enderr;
			return false;
		}
	}
*/
	// New action createdoc, find warehouse, if not exist, create.
	AosActionObjPtr crtact = OmnNew AosActCreateDoc(sdoc);
	aos_assert_r(crtact, false);
	AosXmlTagPtr warehouse = AosDocClientObj::getDocClient()->getDocByObjid(warehouse_id, rdata);
	if (!warehouse)
	{
		AosXmlTagPtr cdoc = sdoc->getFirstChild("create_doc_actions");
		if (!cdoc)
		{
			AosSetError(rdata, AOSLT_MISSING_CREATEDOC_SDOC);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		rdata->setReceivedDoc(working_doc, true);
		aos_assert_r(crtact->run(cdoc, rdata), false);
	}

	// Retrieve the Item ID Attribute Name
	OmnString item_id_aname = sdoc->getAttrStr("item_id_aname", "zky_item_id");
	if (item_id_aname == "") item_id_aname = "zky_item_id";

	if (!records)// || !(record = records->getFirstChild()))
	{
		AosSetError(rdata, AOSLT_MISSING_WAREHOUSE_ITEMS);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	AosXmlTagPtr record;
	if(records->isRootTag())
	{
	    records = records->getFirstChild();
		record = records->getFirstChild();
	}
	else
	{
		record = records->getFirstChild();
    }
	
	if (!record)
	{
		AosSetError(rdata, AOSLT_MISSING_WAREHOUSE_ITEMS);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	// Check all records
	vector<AosXmlTagPtr> items;
	vector<int64_t> quantities;
	AosXmlTagPtr item;
	AosXmlTagPtr received_doc = rdata->setReceivedDoc(0, false);
	while (record)
	{
		// Retrieve the item id
		OmnString item_id = record->getAttrStr(item_id_aname);
		if (item_id == "")
		{
			AosSetError(rdata, AOSLT_MISSING_ITEM_ID);
			OmnAlarm << rdata->getErrmsg() << ". Items: " 
				<< working_doc->toString() << enderr;
			rdata->setReceivedDoc(received_doc, false);
			return false;
		}

		// Retrieve the doc from the warehouse
		bool is_unique;
		u64 item_docid = AosQuery::getSelf()->getMember(warehouse_id, item_id_aname, 
				item_id, is_unique, rdata);
		if (item_docid == 0)
		{
			// No item found. Need to create it.
			AosXmlTagPtr create_tag = sdoc->getFirstChild("create_item_actions");
			aos_assert_r(create_tag, false);
			AosXmlTagPtr child = create_tag->getFirstChild();
			aos_assert_r(child, false);
			
			record->setAttr("zky_container", AOSTAG_PARENTC);
			record->setAttr(AOSTAG_PARENTC, warehouse_id);
			record->setAttr("zky_item", item_id_aname);
			record->setAttr(item_id_aname, item_id);
			record->setAttr("public_ctnr", "zky_public_ctnr");
			record->setAttr("zky_public_ctnr", "true");
			record->setAttr("public_doc", "zky_public_doc");
			record->setAttr("zky_public_doc", "true");
			
			rdata->setReceivedDoc(record, false);
			aos_assert_r(crtact->run(child, rdata), false);
			item = rdata->getCreatedDoc();
			if (!item)
			{
				AosSetError(rdata, AOSLT_FAILED_CREATING_DOC);
				OmnAlarm << rdata->getErrmsg() << enderr;
				rdata->setReceivedDoc(received_doc, false);
				return false;
			}
OmnScreen << "---------------------------------------------" << endl;
OmnScreen << item->toString() << endl;
OmnScreen << "---------------------------------------------" << endl;
			rdata->setReceivedDoc(working_doc, false);
		}
		else
		{
			if (!is_unique)
			{
				AosSetError(rdata, AOSLT_ITEM_NOT_UNIQUE) << ". "
					<< AOSLT_ITEM_ID << ":" << item_id;
				OmnAlarm << rdata->getErrmsg() << enderr;
				rdata->setReceivedDoc(received_doc, false);
				return false;
			}
			
			item = AosDocClientObj::getDocClient()->getDocByDocid(item_docid, rdata);
			if (!item)
			{
				AosSetError(rdata, AOSLT_FAILED_RETRIEVE_DOC);
				OmnAlarm << rdata->getErrmsg() << enderr;
				rdata->setReceivedDoc(received_doc, false);
				return false;
			}
		}

		int64_t quantity = record->getAttrInt64(quantity_aname, 0);
		if (quantity == 0)
		{
			AosSetError(rdata, AOSLT_QUANTITY_IS_ZERO) << ". " << record->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			rdata->setReceivedDoc(received_doc, false);
			return false;
		}

		items.push_back(item);
		quantities.push_back(quantity);
		record = records->getNextChild();
	}
	
	// Retrieve the log name
	OmnString logname = sdoc->getAttrStr("invoice_logname");

	// In the following loop, we will add the quantity to the warehouse items
	// and create an log for each item.
	record = records->getFirstChild();
	int idx = 0;
	while (record)
	{
		// Update the quantity
		item = items[idx];
		int64_t vv = item->getAttrInt64(quantity_aname, 0);
		vv += quantities[idx];
		OmnString str;
		str << vv;
		bool rslt = AosModifyDocAttrStr(rdata, item, quantity_aname, str,
				false, false);
		if (!rslt)
		{
			OmnAlarm << "Failed modify the attribute: " << rdata->getErrmsg() << enderr;
		}
		else
		{
			// Create the invoice
			AosXmlTagPtr logtag = sdoc->getFirstChild("create_invoice_sdoc");
			aos_assert_r(logtag, false);
			AosXmlTagPtr logact_tag = logtag->getFirstChild();
			aos_assert_r(logact_tag, false);
			AosXmlTagPtr crtlog_tag = logtag->getNextChild();
			aos_assert_r(crtlog_tag, false);
			AosXmlTagPtr rr = record->clone(AosMemoryCheckerArgsBegin);
			rdata->setReceivedDoc(rr, false);
			
			// create log based on action createdoc
			aos_assert_r(crtact->run(logact_tag, rdata), false);

			// set container and logname 
			AosXmlTagPtr crtdoc = rdata->getCreatedDoc();
			if (!crtdoc)
			{
				AosSetError(rdata, AOSLT_FAILED_CREATING_DOC);
				OmnAlarm << rdata->getErrmsg() << enderr;
				rdata->setReceivedDoc(received_doc, false);
				return false;
			}
			crtdoc->setAttr(AOSTAG_PARENTC, warehouse_id); 
			crtdoc->setAttr(AOSTAG_LOGNAME, logname); 
			rdata->setReceivedDoc(crtdoc, false);

			// Create log
			AosActionObjPtr clgact = OmnNew AosActCreateLog(false);
			aos_assert_r(clgact, false);
			aos_assert_r(clgact->run(crtlog_tag, rdata), false);
		}
		record = records->getNextChild();
		idx++;
	}

	rdata->setReceivedDoc(received_doc, false);
	return true;
}

#endif
