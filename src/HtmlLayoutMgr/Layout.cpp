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
// 06/28/2010: Created by Lynch yang 
////////////////////////////////////////////////////////////////////////////
#include "HtmlLayoutMgr/Layout.h"

#include "HtmlLayoutMgr/AllLayout.h"
#include "HtmlServer/HtmlUtil.h"
#include "XmlUtil/XmlTag.h"

AosLayoutPtr AosLayout::mLayouts[eAosHtmlLayoutType_Max];

static AosVHLayout       	sgAosVHLayout;
static AosAutoLayout     	sgAosAutoLayout;
static AosAbsoluteLayout    sgAosAbsoluteLayout;
static AosBorderLayout      sgAosBorderLayout;


AosLayout::AosLayout()
{
}


AosLayout::AosLayout(const AosXmlTagPtr &vpd)
{
}

AosLayout::~AosLayout()
{
}


bool
AosLayout::registerLayout(
		const AosHtmlLayoutType layOutType, 
		const AosLayoutPtr &layout)
{
	aos_assert_r(layOutType> eAosHtmlLayoutType_Invalid && layOutType < eAosHtmlLayoutType_Max, false);
	mLayouts[layOutType] = layout;
	return true;
}


int
AosLayout::createLayoutVpd(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd, 
		AosXmlTagPtr &obj, 
		const OmnString parentId,
		AosHtmlCode &code,
		const int parentWidth,
		const int parentHeight)
{
	aos_assert_r(vpd, 0);

	OmnString layoutType;
	int status = 0;
	OmnString type = vpd->getAttrStr("pane_type",vpd->getAttrStr("container_type"));
	if (type=="")
		type = vpd->getAttrStr("panel_type",vpd->getAttrStr("container_type"));
	if (type=="")
		type = vpd->getAttrStr("gic_type");

	bool needToolBar = false;
	if(type == "xmleditor_panel")
	{
		htmlPtr->addGic("xmleditor_panel");
		layoutType = "absolute";
		needToolBar = true;
		status = AOSHTML_PANEL_STATUS;
	}
	else if(type == "atomic_panel" || type == "" || type== "gic_ftoolbar")
	{
		AosXmlTagPtr lytChild = vpd->getFirstChild("layoutmgr");
		if (lytChild)
		{
			layoutType = lytChild->getAttrStr("lm_name", "absolute");
		}
		else
		{
			layoutType = "absolute";
		}
		status = AOSHTML_PANEL_STATUS;
	}
	else if(type == "PWT" || type == "pwt")
	{
		AosXmlTagPtr pwt = vpd->getFirstChild("pwt");
		if (pwt)
		{
			OmnString isBorder = vpd->getAttrStr("ctn_isborder", "false");
			if (isBorder == "true")
				layoutType = "border";
			else
				layoutType = "vhbox";
		}
		else
		{
			layoutType = "vhbox";
		}
		status = AOSHTML_PANE_STATUS;
	}
	else if(type == "normal_pane")
	{
		layoutType = "auto";
		status = AOSHTML_PANE_STATUS;
	}
	else if(type == "editor_pane")
	{
		htmlPtr->createEditorPane(code, vpd, obj, parentId, parentWidth, parentHeight);
		return AOSHTML_EDITORPANE_STATUS;
	}
	else
	{
		//htmlPtr->createGic(code, vpd, obj, parentId);
		htmlPtr->createGic(code, vpd, obj, parentId, parentWidth, parentHeight);	//Ketty 2011/09/22
		return AOSHTML_GIC_STATUS;
	}
	
	AosHtmlCode baseCode;
	OmnString extStyle;

	OmnString dvsb = vpd->getAttrStr("pane_dvsb","true");
	OmnString dhsb = vpd->getAttrStr("pane_dhsb","true");
	OmnString isAutoScroll = "true";
	if ( dvsb == "true" && dhsb == "true" )
		isAutoScroll = "false";

	//felicia, 2012/02/16
	OmnString pane_bwidth = vpd->getAttrStr("pane_bwidth", "0");
	OmnString pane_bcolor = vpd->getAttrStr("pane_bcolor", "#ffffff");
	OmnString pane_bstyle = vpd->getAttrStr("pane_bstyle", "solid");
	OmnString pane_bmleft = vpd->getAttrStr("pane_bmleft", "0px");
	OmnString pane_bmright = vpd->getAttrStr("pane_bmright", "0px");
	OmnString pane_bmtop = vpd->getAttrStr("pane_bmtop", "0px");
	OmnString pane_bmbottom = vpd->getAttrStr("pane_bmbottom", "0px");
	OmnString pane_bpleft = vpd->getAttrStr("pane_bpleft", "0px");
	OmnString pane_bpright = vpd->getAttrStr("pane_bpright", "0px");
	OmnString pane_bptop = vpd->getAttrStr("pane_bptop", "0px");
	OmnString pane_bpbottom = vpd->getAttrStr("pane_bpbottom", "0px");

	OmnString bgcolor = vpd->getAttrStr("container_bgcolor","transparent");
	OmnString ctn_panebg = vpd->getAttrStr("container_background","");
	OmnString norepeat = vpd->getAttrStr("norepeat","");
	AosHtmlReqProcPtr  hserver;
	bgcolor = AosHtmlUtil::getWebColor(bgcolor);
	if(bgcolor != "")
	{
		extStyle << "background-color:" << bgcolor << ";";
	}
	if(ctn_panebg != "")
	{
		OmnString bgimgpath = ctn_panebg;
		OmnString mImgPath = hserver->getImagePath(); 
		if(ctn_panebg.indexOf("http://",0) == -1)	
		{
			bgimgpath = mImgPath;
			bgimgpath << ctn_panebg;
		}
		extStyle << "background:url(" << bgimgpath << ")";
		if(norepeat != "" && norepeat != "false")
		{
			extStyle << ";background-repeat:no-repeat;background-position:center top";	
		}
	}
	int width = vpd->getAttrInt("container_width", vpd->getAttrInt("ctn_ww", 100));
	int miniw = vpd->getAttrInt("miniw", 100);
	int minih = vpd->getAttrInt("minih", 100);
	int maxw = vpd->getAttrInt("maxw", 4000);
	int maxh = vpd->getAttrInt("maxh", 6000);
	int vpdwidth = width;
	OmnString width_policy = vpd->getAttrStr("container_width_def", "percent");
	if(width_policy == "percent")
	{
		//if(parentId != "" && parentWidth != -1)
		if (parentWidth > 0)	//Ketty 2011/10/28
			width = parentWidth * width/100;
	}

	int height= vpd->getAttrInt("container_height", vpd->getAttrInt("ctn_hh", 100));
	int vpdheight = height;
	OmnString height_policy = vpd->getAttrStr("container_height_def", "percent");
	if(height_policy == "percent")
	{
		//if(parentId != "" && parentHeight != -1)
		if (parentHeight > 0)	//Ketty 2011/10/28
			height = parentHeight * height/100;
	}

	//felicia, for npane's border,margin and padding
	OmnString elstyle = "";
	OmnString bodystyle = "";
	if (type == "normal_pane")
	{
		width = width - (atoi(pane_bmleft.data()) + atoi(pane_bmright.data()) + 2 * atoi(pane_bwidth.data()));
		height = height - (atoi(pane_bmtop.data()) + atoi(pane_bmbottom.data()) + 2 * atoi(pane_bwidth.data()));
		elstyle << "border-color:" << pane_bcolor << ";border-width:" << pane_bwidth << ";"
			    << "border-style:" << pane_bstyle << ";margin-left:" << pane_bmleft << ";"
				<< "margin-right:" << pane_bmright << ";margin-top:" << pane_bmtop << ";"
				<< "margin-top:" << pane_bmtop << ";";
		bodystyle << ";padding-left:" << pane_bpleft << ";padding-right:" << pane_bmright << ";"
			    << "padding-top:" << pane_bptop << ";padding-bottom:" << pane_bpbottom << ";";
	}


	baseCode.mJson << "{";
	
	if (parentId !="")
		baseCode.mJson << "parentId:" << parentId << "\", ";
	
	OmnString compid = htmlPtr->getInstanceId(vpd);
	baseCode.mJson << "id:" << compid << "\", ";
	baseCode.mJson << "applyTo:" << compid << "\", ";
	
	OmnString title = vpd->getAttrStr("ctn_title");
	if(title != "")
		baseCode.mJson << "title:'" << title << "', ";
	
	OmnString pane_pvnbd = vpd->getAttrStr("pane_pvnbd","");
	if(pane_pvnbd != "")
		baseCode.mJson << "pane_pvnbd:'" << pane_pvnbd << "', ";
	
	OmnString border = vpd->getAttrStr("ctn_border");
	if(border != "")
		baseCode.mJson << "border:" << border <<", ";
	
	OmnString cls = vpd->getAttrStr("ctn_collapsible");
	if(cls != "")
		baseCode.mJson << "collapsible:" << cls << ", ";
	
	AosXmlTagPtr cvpd = vpd->getFirstChild("pwt");
	OmnString ltype;
	if(cvpd)
		ltype = cvpd ->getAttrStr("gic_type");
	
	OmnString split = vpd->getAttrStr("ctn_split");
	if(split != "")
		baseCode.mJson << "split:" << split << ", ";
	
	OmnString ctn_fit = vpd->getAttrStr("ctn_fit");
	if(ctn_fit != "")
		baseCode.mJson << "ctn_fit:'" << ctn_fit << "',";
	
	OmnString clpsed = vpd->getAttrStr("ctn_collapsed");
	if(clpsed != "")
		baseCode.mJson << "collapsed:" << clpsed << ", ";
	
	OmnString ctn_dds = vpd->getAttrStr("container_ddsource");
	if(ctn_dds != "")
		baseCode.mJson << "container_ddsource:'" << ctn_dds << "', ";
	
	OmnString ctn_ddt = vpd->getAttrStr("container_ddtarget");
	if(ctn_ddt != "")
		baseCode.mJson << "container_ddtarget:'" << ctn_ddt << "', ";
	
	bool pnl_gdclbd = vpd->getAttrBool("pnl_gdclbd");
	if(pnl_gdclbd)
		baseCode.mJson << "pnl_gdclbd:" << pnl_gdclbd << ", ";
	
	//felicia, 2011/02/02 for xmleditor
	bool movable = vpd->getAttrBool("movable_xmleditor", false);
	baseCode.mJson << "movable_xmleditor:" << movable << ", ";
	
	bool pnl_xmlobj = vpd->getAttrBool("pnl_xmlobj");
	if(pnl_xmlobj)
		baseCode.mJson << "pnl_xmlobj:" << pnl_xmlobj << ", ";
	
	bool pnl_onecreator = vpd->getAttrBool("pnl_onecreator");
	if(pnl_onecreator)
		baseCode.mJson << "pnl_onecreator:" << pnl_onecreator<< ", ";
	
	bool pnl_autoid = vpd->getAttrBool("pnl_autoid");
	if(pnl_autoid)
		baseCode.mJson << "pnl_autoid:" << pnl_autoid << ", ";
	
	OmnString pnl_datafilter= vpd->getAttrStr("pnl_datafilter");
	if(pnl_datafilter!= "")
		baseCode.mJson << "pnl_datafilter:'" << pnl_datafilter << "', ";
	
	OmnString pnl_datasort = vpd->getAttrStr("pnl_datasort");
	if(pnl_datasort!= "")
		baseCode.mJson << "pnl_datasort:'" << pnl_datasort<< "', ";

	OmnString container_x = vpd->getAttrStr("container_x","0");
	OmnString container_y = vpd->getAttrStr("container_y","0");

	if(htmlPtr->isHomePage())
	{
		baseCode.mHtml << "<div id=" << compid << "\" ";
	}
	else
	{
		baseCode.mHtml << "<div id='+" << compid << "\"+' ";
	}

	baseCode.mHtml << "class=\"x-panel x-panel-noborder ";
	AosXmlTag* pet = vpd->getParentTag();
	if(pet && (!pet->isDeleted()) && pet->getNumSubtags() > 1)
	{
		baseCode.mHtml << "x-box-item";
	}
	baseCode.mHtml << "\" style=\"width:" << width << "px;left:" << container_x << "px;top:" << container_y << "px;" << elstyle << "\">" 
				   << "<div class=\"x-panel-bwrap\">"
				   << (needToolBar ? "<div class=\"x-panel-tbar x-panel-tbar-noheader x-panel-tbar-noborder\"></div>" : "")
				   << "<div class=\"x-panel-body x-panel-body-noheader x-panel-body-noborder\" "
				   << "style=\"width:" << width << "px;height:" << height << "px;" << extStyle << bodystyle << "\">";

	OmnString isInEditor;
	isInEditor << htmlPtr->isInEditor();
	baseCode.mJson << "isInEditor:" << isInEditor << ", ";
	
	baseCode.mJson << "width:" << width << ", ";
	baseCode.mJson << "height:" << height << ", ";
	baseCode.mJson << "maxw:" << maxw << ", ";
	baseCode.mJson << "maxh:" << maxh << ", ";
	//baseCode.mJson << "miniw:" << miniw << ", ";
	//baseCode.mJson << "minih:" << minih << ", ";
	//baseCode.mJson << "layouttype:'" << ltype << "', ";
	//baseCode.mJson << "ctn_widthdef:'" << width_policy << "', ";
	//baseCode.mJson << "vpdwidth:" << vpdwidth << ", ";
	//baseCode.mJson << "vpdheight:" << vpdheight << ", ";
	//baseCode.mJson << "ctn_heightdef:'" << height_policy << "', ";
	baseCode.mJson //<< "mlLayoutType" << 	//in VHLayout.cpp 
				<< "mlMinWidth:" << miniw << ", "
				<< "mlMinHeight:" << minih << ", "
				<< "mlWdef:'" << width_policy << "', "
				<< "mlHdef:'" << height_policy << "', "
				<< "mlVpdWidth:" << vpdwidth << ", "
				<< "mlVpdHeight:" << vpdheight << ", ";
	baseCode.mJson << "bodyStyle:'" << extStyle << "', ";
	baseCode.mJson << "autoScroll:" << isAutoScroll << ", ";
	if (type == "normal_pane")
	{
		baseCode.mJson << "pane_bwidth:\"" << pane_bwidth << "\", ";
		baseCode.mJson << "pane_bcolor:\"" << pane_bcolor << "\", ";
		baseCode.mJson << "pane_bstyle:\"" << pane_bstyle << "\", ";
		baseCode.mJson << "pane_bmleft:\"" << pane_bmleft << "\", ";
		baseCode.mJson << "pane_bmright:\"" << pane_bmright << "\", ";
		baseCode.mJson << "pane_bmtop:\"" << pane_bmtop << "\", ";
		baseCode.mJson << "pane_bmbottom:\"" << pane_bmbottom << "\", ";
		baseCode.mJson << "pane_bpleft:\"" << pane_bpleft << "\", ";
		baseCode.mJson << "pane_bpright:\"" << pane_bpright << "\", ";
		baseCode.mJson << "pane_bptop:\"" << pane_bptop << "\", ";
		baseCode.mJson << "pane_bpbottom:\"" << pane_bpbottom << "\", ";
	}
	baseCode.mJson << "items:[";
	AosHtmlLayoutType lytType = AosHtmlLayoutType_strToCode(layoutType);
	if (lytType <= eAosHtmlLayoutType_Invalid || lytType >= eAosHtmlLayoutType_Max)
	{
		OmnWarn << "Layout type incorrect: " << layoutType << enderr;
		lytType = eAosHtmlLayoutType_absolute;
	}

	mLayouts[lytType]->generateLayoutCode(htmlPtr, vpd, obj, compid, baseCode, width, height);
	baseCode.mJson << "]}";

	baseCode.mHtml << "</div></div></div>";

	code.append(baseCode);
	return status;
}
