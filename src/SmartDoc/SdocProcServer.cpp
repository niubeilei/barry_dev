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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocProcServer.h"

#include "API/AosApi.h"
#include "Debug/Error.h"
#include "SEInterfaces/DocClientObj.h"
#include "Porting/TimeOfDay.h"
#include "Porting/GetTime.h"
#include "DocSelector/DocSelector.h"
#include "Security/Session.h"
#include "Rundata/Rundata.h"
#include "Util/StrSplit.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/LockMonitor.h"
#include "XmlInterface/WebRequest.h"
#include "XmlUtil/XmlTag.h"


AosSdocProcServer::AosSdocProcServer(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_PROCSERVER, AosSdocId::eProcServer, flag),
mLock(OmnNew OmnMutex())
{
}


AosSdocProcServer::~AosSdocProcServer()
{
}


bool
AosSdocProcServer::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// The smart doc is in the following format:
	// <sdoc zky_otype="zky_smtdoc" zky_objid="xxx" zky_sdoctp="procserver"  AOSTAG_XPATH="xxx" 
	//		AOSTAG_BYOBJID="true|false"	zky_opr_type="xxx"/>
	//		<connectserver remote_ip="xxx" remote_port="xxx"/>
	// 		<AOSTAG_DOCSELECTOR .../>
	// 		<actions>
	// 			<action type="modifyattr">	
	// 				<zky_docselector zky_docselector_type="targetdoc"/>
	// 				<zky_valuedef zky_value_type="attr" zky_xpath = "attr1" >	
	// 					<zky_docselector zky_docselector_type="sourcedoc" >
	// 					</zky_docselector>
	// 				</zky_valuedef>
	// 			</action>
	// 		</actions>
	// 	</sdoc>
	// 
	// 1. check the zky_opr_type:
	// 		a. 	get_sys_info   
	// 				connect to procserver
	// 		b.	modify_attr
	// 				modify attribute 
	// 				connect to procserver
	
	// Retrieve the smart doc
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	//check the zky_opr_type
	OmnString opr_type = sdoc->getAttrStr("zky_opr_type", "");
	if(opr_type == "")
	{
		rdata->setError() << "Missing opration type to procserver!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	if(opr_type == "get_sys_info")
	{
		//connect to the procserver
		AosXmlTagPtr connxml = sdoc->getFirstChild("connectserver");
		if(!connxml)
		{
			rdata->setError() << "Missing the tag of the connectserver";
			return false;
		}

		OmnString send_req = "<request><source type=\"fromses\"/><item name='zky_opr_type'><![CDATA[";
		send_req << opr_type << "]]></item>";
		send_req << "</request>";
		OmnString resp;
		bool connect_rslt = connectToProcServer(connxml, send_req, resp, rdata);
		if(!connect_rslt)
		{
			return false;
		}
		if(resp == "")
		{
			rdata->setError() << "Missing the response of the connectserver";
			return false;
		}
		OmnString rslts = "<Contents>";
		rslts << resp;
		rslts << "</Contents>";
		rdata->setResults(rslts);
		return true;
	}
	//modify the procservice doc
	if(opr_type == "modify_attr")
	{
		// Retrieve the path that identifies <entries>
		OmnString path = sdoc->getAttrStr(AOSTAG_XPATH);
		if (path == "")
		{
			rdata->setError() << "Missing xpath";
			return false;
		}

		// Retrieve the working doc
		AosXmlTagPtr workingDoc = AosRunDocSelector(rdata, sdoc, AOSTAG_DOCSELECTOR);
		if (!workingDoc)
		{
			rdata->setError() << "Missing working data";
			return false;
		}

		AosXmlTagPtr entries = workingDoc->xpathGetChild(path);
		if (!entries)
		{
			// There are no docs. 
			rdata->setOk();
			return true;
		}

		// Retrieve the actions
		AosXmlTagPtr actions = sdoc->getFirstChild(AOSTAG_ACTIONS);
		if (!actions)
		{
			rdata->setError() << "Missing actions";
			return false;
		}

		bool actionrslt = doActions(sdoc, actions, entries, rdata);
		if(!actionrslt)
		{
			rdata->setError() << "it's wrong to run the actions";
			return false;
		}

		//connect to the procserver
		AosXmlTagPtr connxml = sdoc->getFirstChild("connectserver");
		if(!connxml)
		{
			rdata->setError() << "Missing the tag of the connectserver";
			return false;
		}

		OmnString send_req = "<request><item name='zky_opr_type'><![CDATA[";
		send_req << opr_type << "]]></item>";
		send_req << "</request>";
		OmnString resp;
		bool connect_rslt = connectToProcServer(connxml, send_req, resp, rdata);
		if(!connect_rslt)
		{
			return false;
		}
		if(resp == "")
		{
			rdata->setError() << "Missing the response of the connectserver";
			return false;
		}
		OmnString rslts = "<Contents>";
		rslts << resp;
		rslts << "</Contents>";
		rdata->setResults(rslts);
		return true;
	}
	return false;
}


bool
AosSdocProcServer::connectToProcServer(
		const AosXmlTagPtr &connxml,
		const OmnString &send_req,
		OmnString &resp,
		const AosRundataPtr &rdata)
{
	//create one connect to ProcServer
	OmnString addr=connxml->getAttrStr("remote_ip", "");
	if(addr == "")
	{
		rdata->setError() << "Missing the servert ip";
		return false;
	}
	int port=connxml->getAttrInt("remote_port", 0);
	if(!port)
	{
		rdata->setError() << "Missing the servert port";
		return false;
	}
	OmnTcpClientPtr conn= OmnNew OmnTcpClient("nn", addr, port, 1, eAosTLT_FirstFourHigh);
	OmnString errmsg;
	if (!conn->connect(errmsg))
	{
		conn = 0;
		errmsg = "Failed to connect to the process server!";
		AosSetErrorU(rdata, errmsg) << enderr;
		return false;
	}
	aos_assert_rl(conn->smartSend(send_req.data(), send_req.length()), mLock, false);
	OmnConnBuffPtr buff;
	conn->smartRead(buff);
	if (!buff)
	{
		errmsg = "Failed to read response!";
		AosSetErrorU(rdata, errmsg) << enderr;
		return false;
	}
	resp << buff->getData();
	return true;
}


bool
AosSdocProcServer::doActions(
		const AosXmlTagPtr &sdoc,
		const AosXmlTagPtr &actions, 
		const AosXmlTagPtr &entries,
		const AosRundataPtr &rdata)
{
	// Ready to run actions on all the docs.
	OmnString failed = "<";
	failed << AOSTAG_FAILED << ">";
	OmnString success = "<";
	success << AOSTAG_SUCCESS << ">";

	bool byobjid = sdoc->getAttrBool(AOSTAG_BYOBJID);
	AosXmlTagPtr entry = entries->getFirstChild();
	OmnString to_idname = sdoc->getAttrStr(AOSTAG_TO_IDNAME, AOSTAG_TO_IDNAME);
	OmnString from_idname = sdoc->getAttrStr(AOSTAG_FROM_IDNAME, AOSTAG_FROM_IDNAME);
	OmnString objid_aname = sdoc->getAttrStr(AOSTAG_OBJID_ANAME);
	u32 siteid = rdata->getSiteid();
	if (siteid == 0)
	{
		rdata->setError() << "Missing Siteid";
		return false;
	}

	if (byobjid)
	{
		if (objid_aname == "")
		{
			rdata->setError() << "Missing objid attribute name!";
			return false;
		}
	}

	int num_success = 0;
	int num_failed = 0;
	AosXmlTagPtr doc;
	while (entry)
	{
		doc = 0;
		if (byobjid)
		{
			OmnString objid = entry->getAttrStr(objid_aname);
			if (objid == "")
			{
				// It is by objid but failed retrieving the objid
				// from 'entry'. 
				failed << "<entry " << to_idname 
					<< "=\"" << entry->getAttrStr(from_idname) << "/>";
				num_failed++;
			}
			else
			{
				// Found the objid. Retrieve the doc
				doc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
				if (!doc)
				{
					failed << "<entry " << to_idname 
						<< "=\"" << entry->getAttrStr(from_idname) << "/>";
					num_failed++;
				}
			}
		}
		else
		{
			// 'entry' is the doc. 
			doc = entry;
		}

		if (doc)
		{
			rdata->setTargetDoc(doc, false);
			if (!AosSdocAction::runActions(actions, rdata))
			{
				// Failed the actions
				failed << "<entry " << to_idname 
					<< "=\"" << entry->getAttrStr(from_idname) << "/>";
				num_failed++;
			}
			else
			{
				success << "<entry " << to_idname 
					<< "=\"" << entry->getAttrStr(from_idname) << "/>";
				num_success++;
			}
		}

		entry = entries->getNextChild();
	}

	OmnString rslts = "<Contents>";
	if (num_success > 0)
	{
		rslts << success << "</" << AOSTAG_SUCCESS << ">";
	}

	if (num_failed > 0)
	{
		rslts << success << "</" << AOSTAG_FAILED << ">";
	}

	rslts << "</Contents>";
	rdata->setResults(rslts);
	rdata->setTargetDoc(0, false);
	return true;
}

