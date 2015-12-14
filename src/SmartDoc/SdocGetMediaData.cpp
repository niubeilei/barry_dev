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
// 2011/12/22	Created by Jackie
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocGetMediaData.h"

#include "SEInterfaces/DocClientObj.h"
#include "MediaData/DataPicker.h"
#include "UtilComm/TcpClient.h"
#include "Actions/ActSeqno.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlInterface/WebRequest.h"



AosSdocGetMediaData::AosSdocGetMediaData(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_GET_MEDIA_DATA, AosSdocId::eGetMediaData, flag)
{
}


AosSdocGetMediaData::~AosSdocGetMediaData()
{
}


bool
AosSdocGetMediaData::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) 
{
	//sdoc format:
	//<sdoc zky_sdoctp="get_med_data" zky_otype="zky_smtdoc" zky_objid="xxx" zky_data_sep="," zky_rslt_sep="," zky_data_format="1" 
	//	zky_sampling_size="100" zky_seg_size="500" zky_is_integral="true" zky_max_segs="100000" algorithm="xxx" sel_tags="xxx, xxx, xxx">
	//</sdoc>

	// <request>
	// 		<objdef>
	// 			<record zky_objid="xxx">
	// 		</objdef>
	// </request>
	// This smart doc retrieves the media data. Media data are arranged as
	// 		<doc ...><![CDATA[xxx...]]></doc>
	// 
	// 1. Retrieve the smart doc
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve the request
	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		AosSetError(rdata, AOSLT_MISSING_REQUEST_DOC);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr child = root->getFirstChild();
	if (!child)
	{
		rdata->setError() << "Request incorrect!";
		return false;
	}

	AosXmlTagPtr objdef = child->getFirstChild("objdef");
	if (!objdef)
	{
		rdata->setError() << "Request incorrect!";
		return false;
	}

	AosXmlTagPtr obj = objdef->getFirstChild();
	if (!obj)
	{
		rdata->setError() << "Failed to Get Original doc!";
		return false;
	}

	// Retrieve the objid
	OmnString objid = obj->getAttrStr(AOSTAG_OBJID, "");
	if (objid == "")
	{
		rdata->setError() << "Failed to Get the objid!";
		return false;
	}

	OmnString zky_data_format = sdoc->getAttrStr("zky_data_format", "1");

	OmnString sel_tags = sdoc->getAttrStr("sel_tags", "");
	if (sel_tags == "")
	{
		rdata->setError() << "Failed to Get the selected tag!";
		return false;
	}
	vector<OmnString> type_array;
	int nn = AosStrSplit::splitStrByChar(sel_tags, ", ", type_array, eMaxTypes);

	if (nn <= 0 || type_array.size() <= 0)
	{
		rdata->setError() << "Failed to Get the type !";
		return false;
	}

	OmnString algorithm = sdoc->getAttrStr("algorithm", "");
	if (algorithm == "")
	{
		rdata->setError() << "Failed to Get the algorithm!";
		return false;
	}

	OmnString contents = "<Contents>";
	bool dup;
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDoc(rdata, "", objid, dup);
	if (!doc)
	{
		AosSetError(rdata, AOSLT_DOC_NOT_FOUND);
		OmnAlarm << rdata->getErrmsg() << ": " << objid << enderr;
		contents << "<[!CDATA[Failed to get the doc]]>" << "</Contents>";
		rdata->setResults(contents);
		return false;
	}
	doc = doc->clone(AosMemoryCheckerArgsBegin);
	OmnString mergeData;
	OmnString nodestr;
	nodestr << "<SelData>";
	for (u32 i=0; i<type_array.size(); i++)
	{
		OmnString rslt = "";
		AosXmlTagPtr node;
		AosXmlTagPtr target = doc->xpathGetFirstChild(type_array[i]);
		if(!target)
		{
			OmnAlarm << rdata->getErrmsg() << "Missing the selected tag : " << type_array[i] << enderr;
			contents << "<[!CDATA[Missing the selected tag : ]]>" << type_array[i] << "</Contents>";
			rdata->setResults(contents);
			return false;

		}
		OmnString data = target->getNodeText();
		if(zky_data_format == "1")
		{
			AosDataPicker::pickDataStatic(algorithm, data, rslt, sdoc, rdata);
			target->setNodeText(rslt, true);
		}
		else
		{
			doc->removeNode(target);
			if(i!=type_array.size()-1)
			{
				mergeData << data << "[SPLIT]";
			}
			else
			{
				mergeData << data;
				nodestr  << mergeData << "</SelData>";
				AosXmlParser parser2;
				node = parser2.parse(nodestr, "" AosMemoryCheckerArgs);
				doc->addNode(node);
			}
			//AosDataPicker::pickDataStatic2(algorithm, data, node, sdoc, rdata);
			//target->setNodeText("", true);
			//target->addNode(node);
		}
	}
	contents << doc->toString();
	contents << "</Contents>";
	rdata->setResults(contents);
	rdata->setOk();
	return true;
}

