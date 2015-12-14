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
//
// Modification History:
// 09/14/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "HtmlModules/VpdChecker.h"

#include "ErrorMgr/ErrmsgId.h"
#include "Rundata/Rundata.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"


bool
AosVpdChecker::checkVpd(
		const AosXmlTagPtr &vpd,
		const AosRundataPtr &rdata)
{
	OmnString vpdtype = vpd->getAttrStr("container_type");
	if(vpdtype == "")
	{
		vpdtype = vpd->getAttrStr("pane_type");
	}

	if(vpdtype == "")
	{
		vpdtype = vpd->getAttrStr("panel_type");
	}

	if(vpdtype == "")
	{
		vpdtype = vpd->getAttrStr("gic_type");
		if(vpdtype != "")
		{
			vpdtype = "gic";
		}
	}

	if(vpdtype == "")
	{
		AosXmlTagPtr gic_creators = vpd->getFirstChild("gic_creators");
		if(gic_creators)
		{
			vpdtype = "atomic_panel";
		}
	}

	if(vpdtype == "PWT" || vpdtype == "pwt" || vpdtype =="editor_panel" ||
			vpdtype == "atomic_panel" || vpdtype == "comp_pane" ||
			vpdtype == "normal_pane" || vpdtype =="gic_ftoolbar" || vpdtype == "gic")
	{
		OmnString otype = vpd->getAttrStr(AOSTAG_OTYPE);
		if(otype != "vpd" || otype == "")
		{
			OmnString docid = vpd->getAttrStr(AOSTAG_DOCID);
			OmnScreen << "The otype is not vpd , the docid :" << docid
				<< ". Objid: " << vpd->getAttrStr(AOSTAG_OBJID)
				<< ". Otype: " << otype << endl;
			vpd->setAttr(AOSTAG_OTYPE, "vpd");
		}
	}

	if (vpd->getAttrStr(AOSTAG_OTYPE) == "vpd")
	{
		bool changed;
		bool checkrslt = verifyVpd(vpd, changed, rdata);
		aos_assert_r(checkrslt, false);
	}
	
	return true;
}

bool
AosVpdChecker::verifyVpd(
		const AosXmlTagPtr &vpd, 
		bool &changed,
		const AosRundataPtr &rdata)
{
	// This function verifies 'vpd' is a valid VPD. 
	aos_assert_rr(vpd, rdata, false);

	OmnString vpd_type = vpd->getAttrStr(AOSTAG_VPD_TYPE);
	if (vpd_type == "")
	{
		vpd_type = determineVpdType(vpd, rdata);
		if (vpd_type == "")
		{
			AosSetError(rdata, AosErrmsgId::eUnrecognizedVpdType);
			OmnAlarm << rdata->getErrmsg() << ". VPD: " << vpd->toString() << enderr;
			return false;
		}
	}

	VpdType vtype = toEnum(vpd_type);
	bool rslt;
	switch (vtype)
	{
	case eAPane:
		 rslt = verifyAtomicPane(vpd, rdata);
		 aos_assert_rr(rslt, rdata, false);
		 break;

	case eCPane:
		 rslt = verifyCompPane(vpd, rdata);
		 aos_assert_rr(rslt, rdata, false);
		 break;
	
	case eNPane:
		 rslt = verifyNormalPane(vpd, rdata);
		 aos_assert_rr(rslt, rdata, false);
		 break;

	case ePanel:
		 rslt = verifyPanel(vpd, rdata);
		 aos_assert_rr(rslt, rdata, false);
		 break;

	case eGic:
		 rslt = verifyGic(vpd, rdata);
		 aos_assert_rr(rslt, rdata, false);
		 break;

	default:
		 AosSetError(rdata, AosErrmsgId::eUnrecognizedVpdType);
		 OmnAlarm << rdata->getErrmsg() << ". Type: " << vpd_type << enderr;
		 return false;
	}

	return true;
}


OmnString
AosVpdChecker::determineVpdType(
		const AosXmlTagPtr &vpd, 
		const AosRundataPtr &rdata)
{
	// This function determines the VPD type. 
	
	// 1. pane_type | container_type = "PWT|pwt", <panes>: compound pane
	if (vpd->getAttrStr("pane_type") == "PWT" ||
		vpd->getAttrStr("pane_type") == "pwt" ||
		vpd->getAttrStr("container_type") == "PWT" ||
		vpd->getAttrStr("container_type") == "pwt")
	{
		AosXmlTagPtr panes = vpd->getFirstChild("panes");
		if (!panes)
		{
			OmnAlarm << "Compound pane missing <panes> tag: " 
				<< vpd->toString() << enderr;
			return "";
		}
		vpd->setAttr("pane_type", "pwt");
		vpd->setAttr(AOSTAG_VPD_TYPE, AOSVPDTYPE_COMPOUND_PANE);
		return AOSVPDTYPE_COMPOUND_PANE;
	}
	
	// 2. panel_type = "atomic_panel", <gic_creators>:
	OmnString panel_type = vpd->getAttrStr("panel_type");
	if (panel_type == "atomic_panel" || panel_type == "editor_panel")
	{
		AosXmlTagPtr creators = vpd->getFirstChild("gic_creators");
		if (!creators)
		{
			OmnAlarm << "Atomic Pane missing <gic_creators> tag: " 
				<< vpd->toString() << enderr;
			return "";
		}
		vpd->setAttr(AOSTAG_VPD_TYPE, AOSVPDTYPE_ATOMIC_PANE);
		return AOSVPDTYPE_ATOMIC_PANE;
	}

	// 3. container_type = "normal_pane", <panel></panel>:
	OmnString container_type = vpd->getAttrStr("container_type");
	if (container_type == "normal_pane")
	{
		AosXmlTagPtr panel = vpd->getFirstChild("panel");
		if (!panel)
		{
			OmnAlarm << "Normal Pane missing <panel> tag:"
				<< vpd->toString() << enderr;
			return "";
		}
		
		vpd->setAttr(AOSTAG_VPD_TYPE,AOSVPDTYPE_NORMAL_PANE);
		return AOSVPDTYPE_NORMAL_PANE;
	}

	// 4. container_type = "",pane_type = "", panel_type = ""
	//    gic_type = "", <gic_creators>
	// 5. gic_type = "xxx",
	OmnString vpdtype = vpd->getAttrStr("container_type");
	if (vpdtype == "")
	{
		vpdtype = vpd->getAttrStr("panel_type");
	}

	if (vpdtype == "")
	{
		vpdtype = vpd->getAttrStr("pane_type");
	}

	if (vpdtype == "")
	{
		vpdtype = vpd->getAttrStr("gic_type");
		vpdtype = "gic";
	}

	if (vpdtype == "")
	{
		AosXmlTagPtr creators = vpd->getFirstChild("gic_creators");
		if (!creators)
		{
			OmnAlarm << "Atomic Pane missing <gic_creators> tag:"
				<< vpd->toString() << enderr;
			return "";
		}

		vpd->setAttr(AOSTAG_VPD_TYPE, AOSVPDTYPE_ATOMIC_PANE);
		return AOSVPDTYPE_ATOMIC_PANE;
	}
	else if (vpdtype == "gic")
	{
		vpd->setAttr(AOSTAG_VPD_TYPE, AOSVPDTYPE_GIC);
		return AOSVPDTYPE_GIC;
	}
	
	OmnAlarm << "VPD Structure Unrecognized: " << vpd->getAttrStr(AOSTAG_OBJID) << enderr;
	return "";
}

bool
AosVpdChecker::verifyNormalPane(
		const AosXmlTagPtr &vpd, 
		const AosRundataPtr &rdata)
{
	// Normal Pane must meet the following:
	// 1. vpd type must be AOSVPDTYPE_NORMAL_PANE
	// 2. Must have <panel>subtag 
	OmnString vpdtype = vpd->getAttrStr(AOSTAG_VPD_TYPE);
	if (vpdtype == "" || vpdtype != AOSVPDTYPE_NORMAL_PANE)
	{
		OmnAlarm << "Normal pane's vpdtype is not " << AOSVPDTYPE_NORMAL_PANE
			<< " ! vpd tag: " << vpd->toString() << enderr;
		return false;
	}

	AosXmlTagPtr panel = vpd->getFirstChild("panel");
	if (!panel)
	{
		OmnAlarm << "Normal Pane missing <panel> tag:"
			<< vpd->toString() << enderr;
		return false;
	}

	return true;
}

bool
AosVpdChecker::verifyCompPane(
		const AosXmlTagPtr &vpd, 
		const AosRundataPtr &rdata)
{
	// Compound Panes must meet the following:
	// 	1. vpd type must be AOSVPDTYPE_COMPOUND_PANE
	// 	2. Must have <panes> subtag
	OmnString vpdtype = vpd->getAttrStr(AOSTAG_VPD_TYPE);
	if (vpdtype == "" || vpdtype != AOSVPDTYPE_COMPOUND_PANE)
	{
		OmnAlarm << "Compound pane's vpdtype is not " << AOSVPDTYPE_COMPOUND_PANE
			<< " ! vpd tag: " << vpd->toString() << enderr;
		return false;
	}

	AosXmlTagPtr panes = vpd->getFirstChild("panes");
	if (!panes)
	{
		OmnAlarm << "Compound pane missing <panes> tag: " 
			<< vpd->toString() << enderr;
		return false;
	}
 	
	return true;
}


bool
AosVpdChecker::verifyAtomicPane(
		const AosXmlTagPtr &vpd, 
		const AosRundataPtr &rdata)
{

	// Atomic Pane must meet the following :
	// 1. vpd type must be AOSVPDTYPE_ATOMIC_PANE
	// 2. Must have <gic_creators> subtag
	
	OmnString vpdtype = vpd->getAttrStr(AOSTAG_VPD_TYPE);
	if (vpdtype == "" || vpdtype != AOSVPDTYPE_ATOMIC_PANE)
	{
		OmnAlarm << "Atomic pane's vpdtype is not " << AOSVPDTYPE_ATOMIC_PANE
			<< " ! vpd tag: " << vpd->toString() << enderr;
		return false;
	}

	AosXmlTagPtr creators = vpd->getFirstChild("gic_creators");
	if (!creators)
	{
		OmnAlarm << "Atomic pane missing <gic_creators> tag: " 
			<< vpd->toString() << enderr;
		return false;
	}

	return true;
}


bool
AosVpdChecker::verifyPanel(
		const AosXmlTagPtr &vpd, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosVpdChecker::verifyGic(
		const AosXmlTagPtr &vpd, 
		const AosRundataPtr &rdata)
{
	// Gic must have meet the following:
	// vpd type must be AOSVPDTYPE_GIC
	OmnString vpdtype = vpd->getAttrStr(AOSTAG_VPD_TYPE);
	if (vpdtype == "" || vpdtype != AOSVPDTYPE_GIC)
	{
		OmnAlarm << "Gic vpd's vpdtype is not " << AOSVPDTYPE_GIC
			<< " ! vpd tag: " << vpd->toString() << enderr;
		return false;
	}
	return true;
}
