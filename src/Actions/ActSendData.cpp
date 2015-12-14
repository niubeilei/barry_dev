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
// This action sets an attribute:
// 	<action zky_type="AOSACTION_SETATTR" xpath="xxx">
// 		<doc .../>
// 		<value .../>
// 	</action>
// 	where <doc> is a Doc Selection tag. <value> is a Value Tag. 
// 	This is the value to be set.
//
// Modification History:
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActSendData.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DocSelector/DocSelector.h"
#include "RemoteBackupClt/RemoteBackupClt.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DocSelObj.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/OmnNew.h"
#include "Util/DynArray.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include <string>
using namespace std;

OmnMutexPtr    AosActSendData::smLock = OmnNew OmnMutex();
map<OmnString, AosRemoteBackupCltPtr> AosActSendData::mBackupClts;

AosActSendData::AosActSendData(const bool flag)
:
AosSdocAction(AOSACTTYPE_SENDDATA, AosActionType::eSendData, flag)
{
}

AosActSendData::~AosActSendData()
{
}


bool	
AosActSendData::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// <action remote_addr="xxx" remote_port="xxx" use_queue="false" queue_full_size="xxx" reconn_freq="5">
	// <zky_docselector .../>
	// </action>
	// config format:
	// <remote_back_client reconn_freq="5" remote_addr="0.0.0.0" remote_port="5555" queue_full_size="5000" use_queue="false">
	//     <idgen init_value="0">
	//     </idgen>
	// </remote_back_client>
	// 
	// reconn_freq means how many seconds to reconnect
	
	if (!sdoc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	AosXmlTagPtr root = sdoc->getRoot();
	if (!root)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString docid= root->getAttrStr(AOSTAG_DOCID);
	if (docid == "")
	{
		AosSetError(rdata, AosErrmsgId::eMissingDocid);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr newdoc = AosRunDocSelector(rdata, sdoc, AOSTAG_DOCSELECTOR);
	if (!newdoc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr data = sdoc->getFirstChild("dataheader");
	if (data)
	{
		data = data->clone(AosMemoryCheckerArgsBegin);
		data->addNode(newdoc);
	}

	map<OmnString, AosRemoteBackupCltPtr>::iterator itr;
	smLock->lock();
	itr = mBackupClts.find(docid);
	AosRemoteBackupCltPtr backupclt;
	if (itr != mBackupClts.end())
	{
		backupclt = itr->second;
	
		smLock->unlock();
		bool rslt = backupclt->docCreated(data, rdata);
		return rslt;
	}
	smLock->unlock();

	OmnString remote_addr = sdoc->getAttrStr("remote_addr", "0.0.0.0");
	OmnString remote_port = sdoc->getAttrStr("remote_port", "5555");
	OmnString use_queue = sdoc->getAttrStr("use_queue", "false");
	OmnString queue_full_size = sdoc->getAttrStr("queue_full_size", "500");
	OmnString reconn_freq = sdoc->getAttrStr("reconn_freq", "5");
	OmnString db_table_name = "table_";
	db_table_name << docid;
	OmnString configstr = "<config><remote_back_client ";
	configstr << "remote_addr=\"" << remote_addr << "\" "
		   << "remote_port=\"" << remote_port << "\" "
		   << "use_queue=\"" << use_queue << "\" "
		   << "queue_full_size=\"" << queue_full_size << "\" "
		   << "db_table_name=\"" << db_table_name << "\" "
		   << "reconn_freq=\"" << reconn_freq << "\"";
	configstr << "><idgen init_value=\"3000\" block_size=\"1000\"></idgen></remote_back_client></config>";
	AosXmlParser parser;
	AosXmlTagPtr config = parser.parse(configstr, "" AosMemoryCheckerArgs);
	try
	{
		backupclt = OmnNew AosRemoteBackupClt(config);
		smLock->lock();
		mBackupClts.insert(make_pair(docid, backupclt));
		smLock->unlock();
		bool rslt = backupclt->docCreated(data, rdata);
		return rslt;
	}
	catch (const OmnExcept &e)
	{
		rdata->setError() << e.getErrmsg();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
}


AosActionObjPtr
AosActSendData::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActSendData(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


