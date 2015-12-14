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
// 17/08/2010: Created by ketty//////////////////////////////////////////////////////////////////////////
#include "GICs/GicMathGame.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlUtil/HtmlUtil.h"

// static AosGicPtr sgGic = new AosGicMathGame();

AosGicMathGame::AosGicMathGame(const bool flag)
:
AosGic(AOSGIC_MATHGAME, AosGicType::eMathGame, flag)
{
}


AosGicMathGame::~AosGicMathGame()
{
}


bool	
AosGicMathGame::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	
	OmnString gic_id = htmlPtr->getInstanceId(vpd);
	OmnString gic_difficulty = vpd->getAttrStr("difficulty","1");
	OmnString gic_endTime = vpd->getAttrStr("endTime","null");
	OmnString gic_totalQuestion = vpd->getAttrStr("totalQuestion","1000");

	//当图片的地址变化时,除了要改此处礼花的src,还要修改math_game.css中的图片地址,还要修改gic_math_game.js中数字图片的src,还有end()函数中作为背景判断对错的src
	code.mHtml = "";
	code.mHtml << "<div class=\"math_game_bgbox\" style=\"position:absolute\" id=\"'+" << parentid << "\"+'\">"
			  		<< "<p class=\"math_game_time\"><span id=\"'+" << parentid << "-time\"+'\">0</span></p>"
					<< "<div class=\"math_game_blackboard\">"
				 		<< "<div class=\"questionHori\" id=\"'+" << parentid << "-questionHori\"+'\"></div>"
						<< "<div class=\"questionVert\" id=\"'+" << parentid << "-questionVert\"+'\"></div>"
						<< "<div class=\"math_game_answer\">"
							<< "<input type=\"text\" id=\"'+" << parentid << "-answer\"+'\" maxlength=\"6\" style=\"z-index:1000\"/>"
						<< "</div>"
					<< "</div>"
					<< "<div class=\"math_game_list\">"
						<< "<table id=\"'+" << parentid << "-list\"+'\"></table>"
						<< "<input type=\"button\" class=\"previous\" id=\"'+" << parentid << "-previous\"+'\" value=\"<<\" disabled=\"disabled\" />"
						<< "<input type=\"button\" class=\"next\" id=\"'+" << parentid << "-next\"+'\" value=\">>\" disabled=\"disabled\" />"
 					<< "</div>"
					<< "<input type=\"button\" id=\"'+" << parentid << "-replay\"+'\" value=\"Replay\" style=\"visibility:hidden\" class=\"math_game_replay\" />"
					<< "<input type=\"button\" id=\"'+" << parentid << "-redo\"+'\" value=\"Redo\" style=\"visibility:hidden\" class=\"math_game_redo\" />"
					<< "<embed id=\"'+" << parentid << "-flower\"+'\" src=\"http://218.64.170.28:8080/lps-4.7.2/prod/Ext/resources/math_game/2.swf\" style=\"position:absolute; top:50px;visibility:hidden \" width=\"400\" height=\"400\" type=\"application/x-shockwave-flash\" quality=\"high\" wmode=\"transparent\" ></embed>";

   code.mJson  << ",difficulty:" << gic_difficulty << ","
			   << "endTime:" << gic_endTime << ","
			   << "totalQuestion:" << gic_totalQuestion;

	return true;
}
