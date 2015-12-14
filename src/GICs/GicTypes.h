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
// 06/28/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GICs_GicTypes_h
#define Aos_GICs_GicTypes_h

#include "Util/String.h"

#define AOSGIC_ADVTAB						"gic_adv_tab"
#define AOSGIC_ALBUM						"gic_album"
#define AOSGIC_ANALYTICSCHART 				"gic_analyticschart"
#define AOSGIC_ANIMATEDCHART 				"gic_animatedchart"
#define AOSGIC_ATTRPICKER       			"gic_attrpicker"
#define AOSGIC_BARCHART         			"gic_barchart"
#define AOSGIC_BREADCRUMBS 					"gic_breadcrumbs"
#define AOSGIC_BUTTON 						"gic_button"
#define AOSGIC_CALENDAR 					"gic_calendar"
#define AOSGIC_CHARTPIE 					"gic_chartpie"

#define AOSGIC_CHECKBOX 					"gic_checkbox"
#define AOSGIC_CHECKBOXGRP      			"gic_checkgrp"
#define AOSGIC_CHECKBOXTWO      			"gic_checkboxtwo"
#define AOSGIC_CLOCK 						"gic_clock"
#define AOSGIC_COLORPICKER					"gic_colorpicker"
#define AOSGIC_COLUMNTREE 					"gic_columntree"
#define AOSGIC_COMBOX 						"gic_combox"
#define AOSGIC_CONTACTLIST					"gic_contactlist"
#define AOSGIC_CONTAINER 					"gic_container"
#define AOSGIC_CREATOR						"gsc_wgt_creator"

#define AOSGIC_CUTEEDITOR 					"gic_cute_editor"
#define AOSGIC_DATA  						"gic_data"
#define AOSGIC_DATEPICKER       			"gic_datepicker"
#define AOSGIC_DRESSROOM 					"gic_dressroom"
#define AOSGIC_FACECHANGE       			"gic_facechange"
#define AOSGIC_FLASH 						"gic_flash"
#define AOSGIC_FLOATINGVPD 					"gic_floatingvpd"
#define AOSGIC_FRAMER 						"gic_framer"
#define AOSGIC_GRID 						"gic_grid"
#define AOSGIC_GRIDAPP 						"gic_gridapp"

#define AOSGIC_GROWINGCHART 				"gic_growingchart"
#define AOSGIC_HTML 	    				"gic_html"
#define AOSGIC_HINTBUTTON 					"gic_hintbutton"
#define AOSGIC_HTML8PCFRAMER    			"gic_html_8pcframer"
#define AOSGIC_HTMLANIMATION    			"gic_html_animation"
#define AOSGIC_HTMLBUTTON       			"gic_html_button"
#define AOSGIC_HTMLCHECKBOX     			"gic_html_checkbox"
#define AOSGIC_HTMLCHECKBOXGRP     			"gic_htmlcheckbox"
#define AOSGIC_HTMLCHECKBOXTWO  			"gic_htmlcheckboxtwo"
#define AOSGIC_HTMLCMP 	    				"gic_htmlcmp"

#define AOSGIC_HTMLCMPDD	    			"gic_htmlcmp_dd"
#define AOSGIC_HTMLCOMBOX       			"gic_html_combox"
#define AOSGIC_HTMLCOMBOXNEW    			"gic_html_combox_new"
#define AOSGIC_HTMLCOMPGIC					"gic_html_compgic"
#define AOSGIC_HTMLCOUNT    				"gic_htmlcount"
#define AOSGIC_HTMLDATAVIEW     			"gic_html_dataview"
#define AOSGIC_HTMLEDITOR 	    			"gic_htmleditor"
#define AOSGIC_HTMLFRAMER       			"gic_html_framer"
#define AOSGIC_HTMLGRID       				"gic_html_grid"
#define AOSGIC_HTMLGROUPLIST    			"gic_html_grouplist"

#define AOSGIC_HTMLHTML       				"gic_html_html"
#define AOSGIC_HTMLIMAGE       				"gic_htmlimage"
#define AOSGIC_HTMLINPUT        			"gic_html_input"
#define AOSGIC_HTMLLIST       				"gic_html_list"
#define AOSGIC_HTMLMENU         			"gic_html_menu"
#define AOSGIC_HTMLMULTIMENU				"gic_html_multimenu"
#define AOSGIC_HTMLPOPVPD       			"gic_html_popvpd"
#define AOSGIC_HTMLRADIO        			"gic_htmlradio"
#define AOSGIC_HTMLRANK         			"gic_htmlrank"
#define AOSGIC_HTMLTAB          			"gic_html_tab"

#define AOSGIC_HTMLTABLIST      			"gic_html_tablist"
#define AOSGIC_HTMLTIMEPICKER				"gic_html_timepicker"
#define AOSGIC_HTMLTREE         			"gic_htmltree"
#define AOSGIC_HTMLSIMPLETREE				"gic_htmltreesim"
#define AOSGIC_HTMLTREEVIEW					"gic_html_treeview"
#define AOSGIC_HTMLREPEATER					"gic_html_repeater"
#define AOSGIC_HTMLPROGRESSBAR				"gic_html_progressbar"
#define AOSGIC_HTMLCHART					"gic_html_chart"
#define AOSGIC_HTMLTWOLEVELNAV				"gic_html_twolevelnav"
#define AOSGIC_IMAGE 						"gic_image"
#define AOSGIC_IMAROLLING 					"gic_imgrolling"	
#define AOSGIC_IMGSHOW						"gic_imgshow"	
#define AOSGIC_INPUT 						"gic_input"

#define AOSGIC_INTERACTIVECHART 			"gic_interactivechart"
#define AOSGIC_LABELIMG						"gic_label_img"
#define AOSGIC_LABELIMGSTR      			"gic_label_img_string"
#define AOSGIC_LABELSHAPE 					"gic_label_shape"
#define AOSGIC_LABELSHAPESTR				"gic_label_shape_string"
#define AOSGIC_LABELSTR						"gic_label_string"
#define AOSGIC_LINECHART 					"gic_linechart"
#define AOSGIC_LIST			    			"gic_list"
#define AOSGIC_LISTICON 					"gic_iconlist"
#define AOSGIC_LOGIN       					"gic_login"

#define AOSGIC_MAP 		        			"gic_map"
#define AOSGIC_MATHGAME 					"gic_math_game"
#define AOSGIC_MSG							"gic_msg"
#define AOSGIC_MUTILIST         			"gic_mutilist"
#define AOSGIC_MYTREE						"gic_mytree"	
#define AOSGIC_NAMEVALUE 					"gic_namevalue"
#define AOSGIC_NEWSLIST						"gic_newslist"
#define AOSGIC_NOTICE						"gic_notice"
#define AOSGIC_NUMCLOCK      				"gic_num_clock"
#define AOSGIC_PANELTREE 					"gic_paneltree"

#define AOSGIC_PIECHART		    			"gic_piechart"
#define AOSGIC_PLATE       					"gic_plate"
#define AOSGIC_POSTIT 						"gic_postit"
#define AOSGIC_PROMPT 						"gic_prompt"
#define AOSGIC_PROPERTYLIST	    			"gic_propertylist"
#define AOSGIC_PULLDOWNMENU	    			"gic_pulldownmenu"
#define AOSGIC_QUESTIONANSWER   			"gic_questionanswer"
#define AOSGIC_RADIOGRP 					"gic_radiogrp"
#define AOSGIC_RANDGENINT 					"gic_randgenint"
#define AOSGIC_REFLECTIONIMAGE 				"gic_reflectionimage"

#define AOSGIC_ROTATIONIMAGE 				"gic_rotationimage"
#define AOSGIC_ROWLIST						"gic_rowlist"
#define AOSGIC_RSS							"gic_rss"
#define AOSGIC_SCHEDULE 					"gic_schedule"
#define AOSGIC_SCROLLMENU 					"gic_scrollmenu"
#define AOSGIC_SHAPE 						"gic_shape"
#define AOSGIC_SHAPESTR						"gic_shape_str"
#define AOSGIC_SIMHTML          			"gic_sim_html"
#define AOSGIC_SIMMENU      				"gic_sim_menu"
#define AOSGIC_SIMPLELIST					"gic_simple_list"

#define AOSGIC_SIMPHTMLEDITOR   			"gic_simple_htmleditor"
#define AOSGIC_SLIDEIMAGE       			"gic_slideimage"
#define AOSGIC_SLIDER						"gic_slider"
#define AOSGIC_STACKCHART					"gic_stackchart"
#define AOSGIC_TABLIST 						"gic_tablist"
#define AOSGIC_TABMENU 						"gic_tabmenu"
#define AOSGIC_TREE 						"gic_tree"
#define AOSGIC_TRENDCHART 					"gic_trendchart"
#define AOSGIC_HEARTCHART 					"gic_heartchart"
#define AOSGIC_HISTORYCHART 				"gic_historychart"
#define AOSGIC_THUMBNAILCHART 				"gic_thumbnailchart"
#define AOSGIC_TURNTABLE 					"gic_turntable"
#define AOSGIC_TYPESET 						"gic_typeset"

#define AOSGIC_UPLOADER						"gic_uploader"
#define AOSGIC_VERTICALMENU					"gic_verticalmenu"
#define AOSGIC_VIEWLIST      				"gic_viewlist"
#define AOSGIC_XMLTREE 						"gic_xmltree"
#define AOSGIC_HTMLTABMENU 					"gic_html_tabmenu"

class AosGicType
{
public:
	enum E
	{
		eInvalid = 0,

		eAlbum,
		eAnalyticsChart,
		eAnimatedChart,
		eAttrPicker,
		eBarChart,
		eBreadCrumbs,
		eButton,
		eCalendar,
		eChartPie,
		eCheckBox,
		
		eCheckBoxGrp,
		eCheckBoxTwo,
		eClock,
		eColorPicker,
		eColumnTree,
		eCombox,
		eContainer,
		eCreator,
		eCuteEditor,
		eData,
		
		eDatePicker,
		eDressRoom,
		eFaceChange,
		eFlash,
		eFloatingVpd,
		eFramer,
		eGrid,
		eGridApp,
		eGrowingChart,
		eHtml,
		
		eHintButton,
		eHtml8PCFramer,
		eHtmlAnimation,
		eHtmlButton,
		eHtmlCheckbox,
		eHtmlCheckboxGrp,
		eHtmlCheckBoxTwo,
		eHtmlCmp,
		eHtmlCmpDD,
		eHtmlCombox,
		
		eHtmlComboxNew,
		eHtmlCompGic,
		eHtmlCount,
		eHtmlDataView,
		eHtmlEditor,
		eHtmlFramer,
		eHtmlGrid,
		eHtmlGroupList,
		eHtmlHtml,
		eHtmlImage,
		
		eHtmlInput,
		eHtmlList,
		eHtmlMenu,
		eHtmlMultiMenu,
		eHtmlPopVpd,
		eHtmlRadio,
		eHtmlRank,
		eHtmlTab,
		eHtmlTabList,
		eHtmlTimePicker,
		
		eHtmlTree,
		eHtmlTreeSimple,
		eHtmlTreeView,	
		eHtmlRepeater,	
		eHtmlProgressBar,	
		eHtmlChart,	
		eHtmlTwoLevelNav,
		eImage,
		eImgRolling,
		eImgShow,
		eInput,
		eInteractiveChart,
		eLabelImg,
		
		eLabelImgStr,
		eLabelStr,
		eLineChart,
		eListIcon,
		eLogin,
		eMap,
		eMathGame,
		eMsg,
		eMutiList,
		eMyTree,
		
		eNameValue,
		eNewsList,
		eNotice,
		eNumClock,
		ePanelTree,
		ePieChart,
		ePlate,
		ePostit,
		ePrompt,
		ePropertyList,
		
		ePullDownMenu,
		eQuestionAnswer,
		eRadioGrp,
		eRandgenInt,
		eReflectionImage,
		eRotationImage,
		eRowList,
		eRss,
		eSchedule,
		eScrollMenu,
		
		eShape,
		eShapeStr,
		eSimHtml,
		eSimMenu,
		eSimpHtmlEditor,
		eSlideImage,
		eSlider,
		eStackChart,
		eTabList,
		eTabMenu,
		
		eTree,
		eTrendChart,
		eHeartChart,
		eHistoryChart,
		eThumbnailChart,
		eTurnTable,
		eTypeSet,
		eUpLoader,
		eVerticalMenu,
		eViewList,
		eXmlTree,
		eHtmlTabMenu,
		
		eMax
	};

	static bool isValid(const E id) {return id > eInvalid && id < eMax;}
	static E toEnum(const OmnString &typeName);
	static OmnString toStr(const E id);
	static bool addName(const OmnString &name, const E id, OmnString &errmsg);
};
#endif

