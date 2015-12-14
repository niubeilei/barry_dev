////////////////////////////////////////////////////////////////////////////
//
// 03/11/2011: Created by Tracy 
////////////////////////////////////////////////////////////////////////////
#include "HtmlModules/Paging.h"


AosPaging::AosPaging()
{
}


OmnString
AosPaging::getJsonConfig(const AosXmlTagPtr &vpd)
{
	return "";
}


OmnString
AosPaging::retrievePaging(
		const OmnString &siteid,
		const AosXmlTagPtr &vpd,		//where the vpd from? 
		const AosXmlTagPtr &obj,
		const OmnString &ssid,
		const OmnString &tagname)
{
	aos_assert_r(vpd, 0);

	OmnString json;
	json
		<< ", page_isshowpaging: " << vpd->getAttrStr("page_isshowpaging", "true")
		<< ", page_isheadtail: " << vpd->getAttrStr("page_isheadtail", "true") 
		<< ", page_isprevnext: " << vpd->getAttrStr("page_isprevnext", "true")
		<< ", page_isreloadcurrent: " << vpd->getAttrStr("page_isreloadcurrent", "true")
		<< ", page_showpages:" << vpd->getAttrStr("page_showpages", "4")
		<< ", page_fcolor: \"" << vpd->getAttrStr("page_fcolor", "#222222")
		<< "\", page_headpage:\"" << vpd->getAttrStr("page_headpage", "首页")
		<< "\", page_tailpage:\"" << vpd->getAttrStr("page_tailpage", "尾页")
		<< "\", page_prevpage:\"" << vpd->getAttrStr("page_prevpage", "前一页")
		<< "\", page_nextpage:\"" << vpd->getAttrStr("page_nextpage", "后一页")
		<< "\", page_reloadcurrent:\"" << vpd->getAttrStr("page_reloadcurrent", "刷新")
		<< "\", page_showstyle:\"" << vpd->getAttrStr("page_showstyle", "2")
		<< "\", page_clickcolor:\"" << vpd->getAttrStr("page_clickcolor", "#FF00FF")
		<< "\", page_overcolor:\"" << vpd->getAttrStr("page_overcolor", "green")
		<< "\"";

	return json;
}

