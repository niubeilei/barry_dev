// Michael 2011 0511
#include "SeReqProc/BatchDelete.h"

#include "SEInterfaces/DocClientObj.h"
#include "SeReqProc/RegisterHook.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "Thread/Mutex.h"
#include "Util/StrSplit.h"

AosBatchDelete::AosBatchDelete(const bool rflag)
:AosSeRequestProc(AOSREQIDNAME_BATCHDELETE,AosSeReqid::eBatchDelete, rflag)
{
}

bool 
AosBatchDelete::proc(const AosRundataPtr &rdata)
{
	// Docids or objids format: 
	// You can get the infomation from req
	// Using a for loop detete those docs
	// <request> 
	// 	   <item name = "args"><![CDATA[OBJID|$|objid1|$|DOCID|$|docid1|$|OBJID|$|objid2]]><item>
	// 	   ...
	// </request>
	AOSLOG_ENTER_R(rdata, false);
	aos_assert_r(rdata, false);
	AosXmlTagPtr req = rdata->getRequestRoot();
	aos_assert_r(req, false);
    OmnString delStr = req->getChildTextByAttr("name", "args");   		
	if (delStr == "")
	{
		rdata->setError() << "Missing objects to delete";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	bool rslt;
	OmnString failed = "<";
	failed << AOSTAG_FAILED << ">";
	OmnString success = "<";
	success << AOSTAG_SUCCESS<<">";
	OmnString contents = "<Contents>";
	
	AosStrSplit parser(delStr, "|$|");
	OmnString word, flagstr, idname;
	OmnString errmsg;
	while ((flagstr = parser.nextWord()) != "")
	{
		rdata->setOk();
		errmsg = "";
		word = parser.nextWord();
		if (flagstr == "DOCID")
		{
			// Delete by docid
			rslt = AosDocClientObj::getDocClient()->deleteObj(rdata, word, "", "", true);
			idname = AOSTAG_DOCID;
			if (!rslt) errmsg = rdata->getErrmsg();
		}
		else if(flagstr == "OBJID")
		{
			// Delete by objid
			rslt = AosDocClientObj::getDocClient()->deleteObj(rdata, "", word, "", true);
			idname = AOSTAG_OBJID;
			if (!rslt) errmsg = rdata->getErrmsg();
		}
		else
		{
			// Unrecognized type
			rslt = false;
			errmsg = "Unrecognized type: ";
			errmsg << flagstr;
		}

		if (rslt)
		{
			success << "<entry " <<  idname <<"=\"" << word << "\"/>"; 
		}
		else
		{
			failed << "<entry " << idname <<"=\"" << word << "\"><![CDATA[" 
				<< errmsg << "]]></entry>";
		}
	}

	failed << "</" << AOSTAG_FAILED << ">";
	success << "</" << AOSTAG_SUCCESS << ">";
	contents << success << failed << "</Contents>";
	rdata->setResults(contents);
	AOSLOG_LEAVE(rdata);
	return true;
}
