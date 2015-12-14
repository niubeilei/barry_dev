////////////////////////////////////////////////////////////////////////////
//
// Modification History:
// 07/20/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_HtmlModules_Paging_h
#define Aos_HtmlModules_Paging_h

#include "XmlUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlModules/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "alarm_c/alarm.h"

class AosPaging
{
public:
	AosPaging();
	~AosPaging() {}

	virtual OmnString getJsonConfig(const AosXmlTagPtr &vpd);

	OmnString
	retrievePaging(
		const OmnString &siteid,
		const AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &ssid,
		const OmnString &tagname);
};

#endif
