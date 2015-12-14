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
//
// Modification History:
// 05/11/2012: Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicHtmlChart.h"

#include "HtmlLayoutMgr/Layout.h"
#include "HtmlLayoutMgr/AllLayout.h"
#include "DbQuery/Query.h"
#include "HtmlModules/DclDb.h"
#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

const int sgContainerMemberPsize = 200;

AosGicHtmlChart::AosGicHtmlChart(const bool flag)
:
AosGic(AOSGIC_HTMLCHART, AosGicType::eHtmlChart, flag)
{
}


AosGicHtmlChart::~AosGicHtmlChart()
{
}


bool	
AosGicHtmlChart::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	code.mJson << ",gic_cp: " << vpd->getAttrStr("gic_cp", "80")
			   << ",gic_max_y: " << vpd->getAttrStr("gic_max_y", "0")
			   << ",gic_min_y: " << vpd->getAttrStr("gic_min_y", "0")
			   << ",gic_num_y: " << vpd->getAttrInt("gic_num_y", 10)
			   << ",gic_num_x: " << vpd->getAttrInt("gic_num_x", 0)
			   << ",gic_cleft: " << vpd->getAttrStr("gic_cleft", "0")
			   << ",gic_cbottom: " << vpd->getAttrStr("gic_cbottom", "0")
			   << ",gic_dot_r: " << vpd->getAttrStr("gic_dot_r", "2")
			   << ",gic_line_w: " << vpd->getAttrStr("gic_line_w", "2")
			   << ",gic_frame_op: " << vpd->getAttrStr("gic_frame_op", "0.6")
			   << ",gic_numrcd: " << vpd->getAttrStr("gic_numrcd", "0")
			   << ",gic_bar_space: " << vpd->getAttrStr("gic_bar_space", "10")
			   << ",gic_dis_xrule: " << vpd->getAttrStr("gic_dis_xrule", "false")
			   << ",gic_dis_yrule: " << vpd->getAttrStr("gic_dis_yrule", "false")
			   << ",gic_userdef: " << vpd->getAttrStr("gic_userdef", "false")
			   << ",gic_closeani: " << vpd->getAttrStr("gic_closeani", "false")
			   << ",gic_ruler_c: \"" << vpd->getAttrStr("gic_ruler_c", "#000000") << "\""
			   << ",gic_ruler_w: " << vpd->getAttrStr("gic_ruler_w", "1")
			   << ",gic_dis_xaid: " << vpd->getAttrStr("gic_dis_xaid", "false")
			   << ",gic_dis_yaid: " << vpd->getAttrStr("gic_dis_yaid", "true")
			   << ",gic_ruler_bc: \"" << vpd->getAttrStr("gic_ruler_bc", "#efefef") << "\""
			   << ",gic_ignore_p: " << vpd->getAttrStr("gic_ignore_p", "true")
			   << ",gic_to_fixed: " << vpd->getAttrStr("gic_to_fixed", "2")
			   << ",gic_per_num: " << vpd->getAttrStr("gic_per_num", "0")

			   //new data mod
			   << ",gic_valuebd: \"" << vpd->getAttrStr("gic_valuebd", "") << "\""
			   << ",gic_lxbd: \"" << vpd->getAttrStr("gic_lxbd", "") << "\""
			   << ",gic_descbd: \"" << vpd->getAttrStr("gic_descbd", "") << "\""

			   //old data mod
			   << ",gic_descs: \"" << vpd->getAttrStr("gic_descs", "") << "\""
			   << ",gic_attrs: \"" << vpd->getAttrStr("gic_attrs", "") << "\""

			   << ",gic_frame_bcolor: \"" << vpd->getAttrStr("gic_frame_bcolor", "#fffff") << "\""
			   << ",gic_fontsize: \"" << vpd->getAttrStr("gic_fontsize", "18px") << "\""
			   << ",gic_datamod: \"" << vpd->getAttrStr("gic_datamod", "default") << "\""


			   //pre constructdata
			   << ",gic_pre_datatype: \"" << vpd->getAttrStr("gic_pre_datatype", "") << "\""
			   << ",gic_pie_range: \"" << vpd->getAttrStr("gic_pie_range", "") << "\""
			   << ",gic_lname: \"" << vpd->getAttrStr("gic_lname", "") << "\""
			   << ",gic_lvalue: \"" << vpd->getAttrStr("gic_lvalue", "") << "\""
			   << ",gic_baseline: \"" << vpd->getAttrStr("gic_baseline", "") << "\""
			   << ",gic_pie_range: \"" << vpd->getAttrStr("gic_pie_range", "") << "\""

			   << ",gic_charttype: \"" << vpd->getAttrStr("gic_charttype", "") << "\""
			   << ",gic_pos: \"" << vpd->getAttrStr("gic_pos", "right") << "\""
			   << ",hasmask:" << vpd->getAttrBool("hasmask", false)
			   << ",gic_totaldesc: \"" << vpd->getAttrStr("gic_totaldesc", "") << "\"";
	return true;
}
