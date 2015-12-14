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
// 08/20/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "GICs/AllGics.h"

#include "GICs/GicScrollMenu.h"
#include "GICs/GicAnalyticsChart.h"
#include "GICs/GicGrowingChart.h"
#include "GICs/GicInteractiveChart.h"
#include "GICs/GicPrompt.h"
#include "GICs/GicXmlTree.h"
#include "GICs/GicPanelTree.h"
#include "GICs/GicColumnTree.h"
#include "GICs/GicChartPie.h"
#include "GICs/GicContainer.h"
#include "GICs/GicDressRoom.h"
#include "GICs/GicInput.h"
#include "GICs/GicButton.h"
#include "GICs/GicHintButton.h"
#include "GICs/GicLineChart.h"
#include "GICs/GicBarChart.h"
#include "GICs/GicListIcon.h"
#include "GICs/GicInput.h"
#include "GICs/GicImage.h"
#include "GICs/GicGridApp.h"
#include "GICs/GicFloatingVpd.h"
#include "GICs/GicTypeSet.h"
#include "GICs/GicCalendar.h"
#include "GICs/GicClock.h"
#include "GICs/GicCombox.h"
#include "GICs/GicHtml.h"
#include "GICs/GicHtmlEditor.h"
#include "GICs/GicColorPicker.h"
#include "GICs/GicDatePicker.h"
#include "GICs/GicCheckbox.h"
#include "GICs/GicCheckboxTwo.h"
#include "GICs/GicCheckboxGrp.h"
#include "GICs/GicMathGame.h"
#include "GICs/GicRadioGrp.h"
#include "GICs/GicPieChart.h"
#include "GICs/GicPostIt.h"
#include "GICs/GicPullDownMenu.h"
#include "GICs/GicQuestionAnswer.h"
#include "GICs/GicTabList.h"
#include "GICs/GicTurnTable.h"
#include "GICs/GicImgrolling.h"
#include "GICs/GicImgShow.h"
#include "GICs/GicMyTree.h"
#include "GICs/GicRss.h"
#include "GICs/GicFlash.h"
#include "GICs/GicNameValue.h"
#include "GICs/GicFramer.h"
#include "GICs/GicShape.h"
#include "GICs/GicSchedule.h"
#include "GICs/GicLabelImg.h"
#include "GICs/GicTabMenu.h"
#include "GICs/GicTree.h"
#include "GICs/GicGrid.h"
#include "GICs/GicLabelStr.h"
#include "GICs/GicShapeStr.h"
#include "GICs/GicStackChart.h"
#include "GICs/GicLabelImgStr.h"
#include "GICs/GicHtmlCmp.h"
#include "GICs/GicUpLoader.h"
#include "GICs/GicViewList.h"
#include "GICs/GicHtmlCmpDD.h"
#include "GICs/GicNewsList.h"
#include "GICs/GicCreator.h"
#include "GICs/GicSimpHtmlEditor.h"
#include "GICs/GicMutiList.h"
#include "GICs/GicHtmlGroupList.h"
#include "GICs/GicMap.h"
#include "GICs/GicBreadCrumbs.h"
#include "GICs/GicPropertyList.h"
#include "GICs/GicRowList.h"
#include "GICs/GicReflectionImage.h"
#include "GICs/GicRotationImage.h"
#include "GICs/GicSlider.h"
#include "GICs/GicCuteEditor.h"
#include "GICs/GicVerticalMenu.h"
#include "GICs/GicNotice.h"
#include "GICs/GicData.h"
#include "GICs/GicAttrPicker.h"
#include "GICs/GicNumClock.h"
#include "GICs/GicLogin.h"
#include "GICs/GicPlate.h"
#include "GICs/GicHtmlHtml.h"
#include "GICs/GicHtmlGrid.h"
#include "GICs/GicSimHtml.h"
#include "GICs/GicHtmlList.h"
#include "GICs/GicHtmlTabList.h"
#include "GICs/GicHtmlCombox.h"
#include "GICs/GicHtmlComboxNew.h"
#include "GICs/GicHtmlImage.h"
#include "GICs/GicHtmlInput.h"
#include "GICs/GicSlideImage.h"
#include "GICs/GicHtmlAnimation.h"
#include "GICs/GicHtmlFramer.h"
#include "GICs/GicHtml8pcFramer.h"
#include "GICs/GicHtmlPopVpd.h"
#include "GICs/GicHtmlDataView.h"
#include "GICs/GicHtmlButton.h"
#include "GICs/GicFaceChange.h"
#include "GICs/GicHtmlTab.h"
#include "GICs/GicHtmlTree.h"
#include "GICs/GicHtmlMenu.h"
#include "GICs/GicHtmlCheckboxGrp.h"
#include "GICs/GicHtmlRadio.h"
#include "GICs/GicHtmlCheckboxTwo.h"
#include "GICs/GicMsg.h"
#include "GICs/GicHtmlRank.h"
#include "GICs/GicHtmlCount.h"
#include "GICs/GicSimMenu.h"
#include "GICs/GicHtmlTreeSimple.h"
#include "GICs/GicHtmlTreeView.h"
#include "GICs/GicHtmlRepeater.h"
#include "GICs/GicHtmlProgressBar.h"
#include "GICs/GicHtmlChart.h"
#include "GICs/GicHtmlMultiMenu.h"
#include "GICs/GicTrendChart.h"
#include "GICs/GicHeartChart.h"
#include "GICs/GicHistoryChart.h"
#include "GICs/GicThumbnailChart.h"
#include "GICs/GicHtmlTimePicker.h"
#include "GICs/GicHtmlTwoLevelNav.h"
#include "GICs/GicHtmlCompGic.h"
#include "GICs/GicHtmlTabMenu.h"

AosGicPtr    AosGic::mGics[AosGicType::eMax];
AosStr2U32_t     sgGicIdMap;
AosAllGics 	 sgAosAllGics;


AosAllGics::AosAllGics()
{
	static AosGicMsg					sgAosGicMsg(true);
	static AosGicAnalyticsChart 		sgAosGicAnalyticsChart(true);
	static AosGicGrowingChart   		sgAosGicGrowingChart(true);
	static AosGicInteractiveChart 		sgAosGicInteractiveChart(true);
	static AosGicXmlTree				sgAosGicXmlTree(true);
	static AosGicPanelTree				sgAosGicPanelTree(true);
	static AosGicColumnTree				sgAosGicColumnTree(true);
	static AosGicChartPie       		sgAosGicChartPie(true);
	static AosGicBarChart       		sgAosGicBarChart(true);
	static AosGicContainer				sgAosGicContainer(true);
	static AosGicDressRoom				sgAosGicDressRoom(true);
	static AosGicPrompt         		sgAosGicPrompt(true);
	static AosGicGridApp        		sgAosGicGridApp(true);
	static AosGicFloatingVpd    		sgAosGicFloatingVpd(true);
	static AosGicTypeSet        		sgAosGicTypeSet(true);
	static AosGicImage        			sgAosGicImage(true);
	static AosGicGrid					sgAosGicGrid(true);
	static AosGicPullDownMenu			sgAosGicPullDownMenu(true);
	static AosGicButton 				sgAosGicButton(true);
	static AosGicCombox					sgAosGicCombox(true);
	static AosGicHintButton     		sgAosGicHintButton(true);
	static AosGicShape					sgAosGicShape(true);
	static AosGicSchedule				sgAosGicSchedule(true);
	static AosGicLineChart      		sgAosGicLineChart(true);
	static AosGicListIcon 				sgAosGicListIcon(true);
	static AosGicTabList 				sgAosGicTabList(true);
	static AosGicTurnTable 				sgAosGicTurnTable(true);
	static AosGicCheckbox 				sgAosGicCheckbox(true);
	static AosGicCheckboxTwo 			sgAosGicCheckboxTwo(true);
	static AosGicCheckboxGrp 			sgAosGicCheckboxGrp(true);
	static AosGicClock					sgAosGicClock(true);
	static AosGicMathGame   			sgAosGicMathGame(true);
	static AosGicRadioGrp 				sgAosGicRadioGrp(true);
	static AosGicPostIt         		sgAosGicPostIt(true);
	static AosGicImgrolling 			sgAosGicImgrolling(true);
	static AosGicImgShow 				sgAosGicImgShow(true);
	static AosGicMyTree					sgAosGicMyTree(true);
	static AosGicRss					sgAosGicRss(true);
	static AosGicRowList				sgAosGicRowList(true);
	static AosGicReflectionImage 		sgAosGicReflectionImage(true);
	static AosGicRotationImage  		sgAosGicRotationImage(true);
	static AosGicFlash					sgAosGicFlash(true);
	static AosGicNameValue				sgAosGicNameValue(true);
	static AosGicInput					sgAosGicInput(true);
	static AosGicFramer					sgAosGicFramer(true);
	static AosGicCalendar 				sgAosGicCalendar(true);
	static AosGicHtml 		    		sgAosGicHtml(true);
	static AosGicHtmlEditor 			sgAosGicHtmlEditor(true);
	static AosGicColorPicker			sgAosGicColorPicker(true);
	static AosGicDatePicker				sgAosGicDatePicker(true);
	static AosGicPieChart				sgAosGicPieChart(true);
	static AosGicLabelImg				sgAosGicLabelImg(true);
	static AosGicLabelStr				sgAosGicLabelStr(true);
	static AosGicShapeStr				sgAosGicShapeStr(true);
	static AosGicStackChart     		sgAosGicStackChart(true);
	static AosGicLabelImgStr    		sgAosGicLabelImgStr(true);
	static AosGicTabMenu				sgAosGicTabMenu(true);
	static AosGicTree					sgAosGicTree(true);
	static AosGicUpLoader				sgAosGicUpLoader(true);
	static AosGicHtmlCmp        		sgAosGicHtmlCmp(true);
	static AosGicViewList       		sgAosGicViewList(true);
	static AosGicNewsList       		sgAosGicNewsList(true);
	static AosGicHtmlCmpDD      		sgAosGicHtmlCmpDD(true);
	static AosGicCreator        		sgAosGicCreator(true);
	static AosGicSimpHtmlEditor 		sgAosGicSimpHtmlEditor(true);
	static AosGicMutiList       		sgAosGicMutiList(true);
	static AosGicHtmlGroupList  		sgAosGicHtmlGroupList(true);
	static AosGicPropertyList   		sgAosGicPropertyList(true);
	static AosGicQuestionAnswer 		sgAosGicQuestionAnswer(true);
	static AosGicMap            		sgAosGicMap(true);
	static AosGicBreadCrumbs    		sgAosGicBreadCrumbs(true);
	static AosGicSlider         		sgAosGicSlider(true);
	static AosGicCuteEditor     		sgAosGicCuteEditor(true);
	static AosGicNotice         		sgAosGicNotice(true);
	static AosGicScrollMenu     		sgAosGicScrollMenu(true);
	static AosGicData           		sgAosGicData(true);
	static AosGicVerticalMenu   		sgAosGicVerticalMenu(true);
	static AosGicAttrPicker     		sgAosGicAttrPicker(true);
	static AosGicNumClock     			sgAosGicNumClock(true);
	static AosGicLogin     				sgAosGicLogin(true);
	static AosGicPlate     				sgAosGicPlate(true);
	static AosGicHtmlHtml     			sgAosGicHtmlHtml(true);
	static AosGicSimHtml     			sgAosGicSimHtml(true);
	static AosGicHtmlList     			sgAosGicHtmlList(true);

	static AosGicHtmlComboxNew			sgAosGicHtmlComboxNew(true);
	static AosGicHtmlCombox				sgAosGicHtmlCombox(true);
	static AosGicHtmlGrid				sgAosGicHtmlGrid(true);
	static AosGicHtmlTabList    		sgAosGicHtmlTabList(true);
	static AosGicHtmlImage 				sgAosGicHtmlImage(true);
	static AosGicHtmlInput 				sgAosGicHtmlInput(true);
	static AosGicSlideImage     		sgAosGicSlideImage(true);
	static AosGicHtmlAnimation  		sgAosGicHtmlAnimation(true);
	static AosGicHtmlFramer     		sgAosGicHtmlFramer(true);
	static AosGicHtml8pcFramer  		sgAosGicHtml8pcFramer(true);
	static AosGicHtmlPopVpd     		sgAosGicHtmlPopVpd(true);
	static AosGicHtmlDataView   		sgAosGicHtmlDataView(true);
	static AosGicHtmlButton   			sgAosGicHtmlButton(true);
	static AosGicFaceChange     		sgAosGicFaceChange(true);
	static AosGicHtmlTab       			sgAosGicHtmlTab(true);
	static AosGicHtmlTree       		sgAosGicHtmlTree(true);
	static AosGicHtmlMenu       		sgAosGicHtmlMenu(true);
	static AosGicHtmlCheckboxGrp   		sgAosGicHtmlCheckboxGrp(true);
	static AosGicHtmlRadio       		sgAosGicHtmlRadio(true);
	static AosGicHtmlCheckboxTwo 		sgAosGicHtmlCheckboxTwo(true);
	static AosGicHtmlRank        		sgAosGicHtmlRank(true);
	static AosGicHtmlCount   	 		sgAosGicHtmlCount(true);
	static AosGicSimMenu   	     		sgAosGicSimMenu(true);
	static AosGicHtmlTreeSimple			sgAosGicHtmlTreeSimple(true);
	static AosGicHtmlTreeView 			sgAosGicHtmlTreeView(true);
	static AosGicHtmlRepeater			sgAosGicHtmlRepeater(true);
	static AosGicHtmlProgressBar		sgAosGicHtmlProgressBar(true);
	static AosGicHtmlChart				sgAosGicHtmlChart(true);
	static AosGicHtmlMultiMenu 			sgAosGicHtmlMultiMenu(true);
	static AosGicTrendChart				sgAosGicTrendChart(true);
	static AosGicHtmlTimePicker			sgAosGicHtmlTimePicker(true);
	static AosGicHtmlTwoLevelNav		sgAosGicHtmlTwoLevelNav(true);
	static AosGicHtmlCompGic			sgAosGicHtmlCompGic(true);
	static AosGicHeartChart				sgAosGicHeartChart(true);
	static AosGicHistoryChart			sgAosGicHistoryChart(true);
	static AosGicThumbnailChart			sgAosGicThumbnailChart(true);
	static AosGicHtmlTabMenu			sgAosGicHtmlTabMenu(true);
}

