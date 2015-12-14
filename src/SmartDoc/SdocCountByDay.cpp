#include "SmartDoc/SdocCountByDay.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/DocSelObj.h"
#include "SEInterfaces/TaskObj.h"

#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include "boost/date_time/local_time/local_time.hpp"
#include <iostream>
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace std;


AosSdocCountByDay::AosSdocCountByDay(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_COUNTBYDAY, AosSdocId::eCountByDay, flag)
{
}

AosSdocCountByDay::~AosSdocCountByDay()
{
}

bool 
AosSdocCountByDay::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This function will update the attribute defined in sdoc the next day.
	// The smartdoc should be this format:
	// <sdoc zky_deadline="xxx" zky_modattr="xxx" />
	if (!sdoc)
	{
		AosSetError(rdata, AOSLT_MISSING_SDOC);
		// AOSMONITORLOG_FINISH(rdata);
		return false;
	}
	
	// Get the deadline user defined, the deadline from frontend is wei miao.
	u64 deadline = sdoc->getAttrU64(AOSTAG_DEADLINE, 0);
	deadline = deadline/1000;
	if (deadline == 0)
	{
		AosSetError(rdata, AOSLT_MISSING_DEADLINE);
		// AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	// Get the attribute user defined to modify.
	OmnString attr = sdoc->getAttrStr(AOSTAG_MODATTR, "");
	if (attr == "")
	{
		AosSetError(rdata, AOSLT_MISSING_MODATTR);
		// AOSMONITORLOG_FINISH(rdata);
		return false;
	}
	
	// The tag AOSTAG_TIME_UP is for time up to proc actions.
	AosXmlTagPtr dActions = sdoc->getFirstChild(AOSTAG_TIME_UP);
	if (!dActions)
	{
		AosSetError(rdata, AOSLT_MISSING_MODATTR);
		// AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	AosActionObjPtr actobj = AosActionObj::getActionObj();
	aos_assert_r(actobj, false);
	// Get now and make sure that the deadline is larger than now.
	u64 now = getNow();
	aos_assert_r(now>0, false);
	if (deadline <= now)
	{
		aos_assert_r(actobj->runActions(dActions, rdata), false);
		return true;
	}
	
	// Modify the attribute.
	// AosDocSelObjPtr dsobj = AosDocSelObj::getDocSelector();
	// aos_assert_r(dsobj, false);

	// Get the tag AOSTAG_SELECT_DOC to select doc.
	AosXmlTagPtr docsel = sdoc->getFirstChild(AOSTAG_SELECT_DOC);
	if (!docsel)
	{
		AosSetError(rdata, AOSLT_MISSING_DOC_SELECTOR);
		// AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	// AosXmlTagPtr doc = dsobj->doSelectDoc(docsel, rdata);
	AosXmlTagPtr doc = AosRunDocSelector(rdata, docsel);
	if (!doc)
	{
		AosSetError(rdata, AOSLT_MISSING_TARGET_DOC);
		// AOSMONITORLOG_FINISH(rdata);
		return false;
	}
	
	ptime pnow = from_time_t(now);
	ptime pdeadline = from_time_t(deadline);
	OmnString value;
	value << (pdeadline.date().day()-pnow.date().day());
	AosDocClientObjPtr dobj = AosDocClientObj::getDocClient();
	aos_assert_r(dobj, false);
	if (!dobj->modifyAttrStr1(rdata,
		doc->getAttrU64(AOSTAG_DOCID, 0), doc->getAttrStr(AOSTAG_OBJID, ""),
		attr, value, "", false, false, true))
	{
		AosSetError(rdata, AOSLT_MISSING_TARGET_DOC);
		// AOSMONITORLOG_FINISH(rdata);
		return false;
	}
	return true;
}


u64
AosSdocCountByDay::getNow()
{
	time_t t;
	t = time(NULL);
	return (u64)t;
}
