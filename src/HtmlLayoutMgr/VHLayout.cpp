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
// 07/16/2010: Created by lynch yang
////////////////////////////////////////////////////////////////////////////
#include "HtmlLayoutMgr/VHLayout.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


AosVHLayout::AosVHLayout()
{
	AosLayoutPtr ptr(this, false);
	registerLayout(eAosHtmlLayoutType_vhbox, ptr);
}


AosVHLayout::~AosVHLayout()
{
}


bool	
AosVHLayout::generateLayoutCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		AosXmlTagPtr &obj,
		const OmnString parentId,
		AosHtmlCode &code,
		const int parentWidth,
		const int parentHeight)
{
	// This function will generate:
	// 	1. HTML code
	// 	2. CSS code
	// 	3. JavaScript Code
	// 	4. Flash code
	aos_assert_r(vpd, false);
	reComputeVpd(vpd, htmlPtr, parentWidth, parentHeight);
	
	OmnString vorh = "pwt_vert";
	AosXmlTagPtr pwt = vpd->getFirstChild("pwt");
	if (pwt)
	{
		vorh = pwt->getAttrStr("gic_type", "pwt_vert");
	}
	OmnString layout, temp;
	//layout = (vorh == "pwt_vert"? "vbox" : "hbox");
	//temp << "layout:'" << layout << "', layoutConfig:{align:'stretch',pack:'start'}, xtype:'aos_cpane', "; 
	layout = (vorh == "pwt_vert"? "lm_vert" : "lm_hori");
	temp << "mlLayoutType:'" << layout << "', xtype:'aos_cpane', "; 
	code.mJson.insert(temp, 1);

	AosXmlTagPtr panes = vpd->getFirstChild("panes");
	AosXmlTagPtr child, parent;
	if (panes)
	{	
		parent = panes;
	}
	else
	{
		parent = vpd;
	}

	child = parent->getFirstChild("pane");
	while(child)
	{
		htmlPtr->createPane(code, child, obj, parentId, parentWidth, parentHeight);
		child = parent->getNextChild("pane"); 
		if (child)
		{

			OmnString split = child->getAttrStr("ctn_split");
			// michael 2011 3 18
			// Create the splitbar
			bool isInEditor = htmlPtr->isInEditor();
			if(split=="true" || isInEditor)
			{
				int splitsize = child->getAttrInt("split_size",2);
				OmnString splitcolor = child->getAttrStr("split_color","#aaaaaa");
				AosXmlTagPtr pwtTag = vpd->getFirstChild("pwt");
				OmnString laytype;
				if(pwtTag)
				{
					laytype = pwtTag->getAttrStr("gic_type");
				}

				int width = 0 ,height =0 ;
				if(laytype == "pwt_vert")
				{
					height = splitsize;
					width = parentWidth; 
				}
				else if(laytype == "pwt_hori")
				{
					width = splitsize;
					height = parentHeight;
				}

				OmnString compid = htmlPtr ->getInstanceId(child) << "_splitbar";
				if(htmlPtr->isHomePage())
				{
					code.mHtml << "<div id=" << compid << "\" class=\"x-panel x-panel-noborder\"";
				}
				else
				{
					code.mHtml << "<div id='+" << compid << "\"+' class=\"x-panel x-panel-noborder\"";
				}

				code.mHtml << "style=\"width:" << width << "px;left:0px;background-color:#aaaaaa;top:0px; position:absolute;\">"
						   << "<div class=\"x-panel-bwrap\"><div class=\"x-panel-body x-panel-body-noheader x-panel-body-noborder\"" 
						   << "style=\"width:" << width << "px;background-color:" << splitcolor <<";height:" << height << "px;\">"
						   <<"</div></div></div>";
				
				code.mJson << ",{"
						   << "applyTo:" << compid << "\", "
						   << "isInEditor:false, "
						   << "isDefault:true, "
						   << "isSplitbar:true, "
						   << "bodyStyle:'background-color:" << splitcolor <<"', "
						   << "width:" << width << ", "
						   << "xtype:'aos_npane', "
						   << "mlLayoutType:'lm_npane', " 
						   << "id:"<< compid << "\", "
						   << "ctn_widthdef:'fixed', "
						   << "ctn_widthdef:'fixed', "
						   << "height:" << height << ", "
						   << "ctn_heightdef:'fixed' "
						   << "}";
			}
			code.mJson << ",";
		}
	}
	return true;
}

bool
AosVHLayout::reComputeVpd(
		AosXmlTagPtr &vpd,
		const AosHtmlReqProcPtr &htmlPtr,
		const int parentWidth,
		const int parentHeight)
{
	aos_assert_r(vpd, false);
	
	AosXmlTagPtr panes = vpd->getFirstChild("panes");
	if(!panes)
	{
		return false;
	}
	AosXmlTagPtr parent = panes;
	
	AosXmlTagPtr pwt = vpd->getFirstChild("pwt");
	OmnString vorh = "pwt_vert";
	bool isVert = false; // pwt type
	if (pwt)
	{
		vorh = pwt->getAttrStr("gic_type", "pwt_vert");
	}
	isVert = (vorh == "pwt_vert"? true : false);
	
	int childLen = 0;  // vpd's child length
	childLen = parent->getNumSubtags();
	if(childLen <= 0)
		return false;
	AosXmlTagPtr pvpd = vpd->getParentTag();
	OmnString def;
	if (htmlPtr->needReturnHVpd() || htmlPtr->needReturnVVpd())
		return true;
	if(isVert)
	{
		return calcChildSize(isVert, parentHeight, parentWidth, childLen, parent);
	}
	else
	{
		return calcChildSize(isVert, parentWidth, parentHeight, childLen, parent);
	}
}


bool
AosVHLayout::calcChildSize(bool isVert, int parentSize, int parentIgnoreSize, int childLen, AosXmlTagPtr parent)
{
	if(parentSize == -1)
	{
		return false;
	}
	
	OmnString resetAttr, resetAttrDef, resetAttr2, resetIgnoreAttr, resetIgnoreAttrDef;
	if(isVert)
	{
		resetAttr = "container_height";
		resetAttrDef = "container_height_def";
		resetIgnoreAttr = "container_width";
		resetIgnoreAttrDef = "container_width_def";
		resetAttr2 = "ct_hh";
	}
	else
	{
		resetAttr = "container_width";
		resetAttrDef = "container_width_def";
		resetIgnoreAttr = "container_height";
		resetIgnoreAttrDef = "container_height_def";
		resetAttr2 = "ct_ww";
	}

   	AosXmlTagPtr childs[childLen]; // keep every child
	AosXmlTagPtr child = parent->getFirstChild("pane");
	int relt[childLen];   // keep every child's width or height
	int total=0;
	int i=0;
	int restCount=0;
	while(child && i < childLen)
	{
		int childSize = child->getAttrInt(resetAttr, child->getAttrInt(resetAttr2, 100));
		OmnString childSizeDef = child->getAttrStr(resetAttrDef, "percent");
		if (childSizeDef == "fixed")  //fixed
		{
			relt[i] = childSize;
			total += childSize;
		}
		else if (childSizeDef == "percent") //percent
		{
			if (childSize<0) childSize =childSize*-1;
			if (childSize>100) childSize = 100;
			relt[i] = childSize*parentSize/100;
			total += relt[i];
		}
		else if (childSizeDef == "rest")	//rest
		{
			restCount ++ ;
			relt[i] = -1;  			// rest size flag
		}
		childs[i] = child;
		i++;
		child = parent->getNextChild("pane");
	}

	if(restCount > 0)
	{
		int tmp = 0;
		int noRest = parentSize - restCount * 50;
		if(noRest > total)
		{
			tmp = (parentSize - total)/restCount;  // the average 
			total = parentSize;
		}
		else if(noRest == total)
		{
			tmp = 50;
			total = parentSize;
		}
		else
		{
			tmp = 50;
			total = total + restCount * 50;
		}

		for(int i = 0; i < childLen; i++)
		{
			if(relt[i] == -1)
			{
				relt[i] = tmp;
			}
		}
	}
	
	if(total > 0) 
	{
		int flex = 0;
		int vhsum=0;
		for(int i = 0;i < childLen; i++)
		{
			flex = relt[i]*100/total;
			OmnString def = childs[i]->getAttrStr(resetAttrDef,"percent");
			
			if(!isVert)
			{
				childs[i]->setAttr("container_x", vhsum);
				childs[i]->setAttr("container_y", 0);
			}
			else
			{
				childs[i]->setAttr("container_y", vhsum);
				childs[i]->setAttr("container_x", 0);
			}
			vhsum+=relt[i];// get the x/y
			if(def == "percent")
			{
				//relt[i] = relt[i]*100/total;  	// Why?? the relt[i] sometimes will smaller than the percent value.
				//childs[i]->setAttr(resetAttr, relt[i]);	// Ketty 2011/09/23
				childs[i]->setAttr("zky_flex", flex);
			}
			else if(def == "fixed")
			{
				// Ketty 2011/10/10		fixed will always the fixed. the width will never change.
				//relt[i] = relt[i]*parentSize/total;   
				//childs[i]->setAttr(resetAttr, relt[i]);
			}
			else if(def == "rest")
			{
				childs[i]->setAttr(resetAttr, relt[i]);
			}
			// Ketty 2011/10/26
			childs[i]->setAttr(resetIgnoreAttr, 100);
			childs[i]->setAttr(resetIgnoreAttrDef, "percent");
			//childs[i]->setAttr(resetIgnoreAttr, parentIgnoreSize);
			//childs[i]->setAttr(resetIgnoreAttrDef, "fixed");
		}
	}
	return true;
}
