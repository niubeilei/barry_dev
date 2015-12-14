//////////////////////////////////////////////////////////////////////////////////
//
//	Copyright (C) 2005
//
//	Define html compiler util
//	create by lynch yang 2010/05/28
///////////////////////////////////////////////////////////////////////////////////
#ifndef Aos_HtmlServer_HtmlUtil_h
#define Aos_HtmlServer_HtmlUtil_h

#define AOSHTML_RETRIEVEOTHER		0				// OTHER
#define AOSHTML_RETRIEVEOBJ 		1				// RETRIEVE OBJ
#define AOSHTML_RETRIEVEVPD			2				// RETRIEVE VPD
#define AOSHTML_OPENDIALOG			3				// RETRIEVE VPD INCLUDE OPENDLG
#define AOSHTML_RETRIEVEFULL		4				// RETRIEVE FULL PAGE
#define AOSHTML_SIMULATE			5				// SIMULATE vpd
#define AOSHTML_RETRIEVESITES  		6 		        // RetrieveSites
#define AOSHTML_PRINT			    7				// print vpd

#define AOSHTML_GIC_STATUS 			1				// Create a Gic
#define AOSHTML_PANEL_STATUS		2				// Create a panel
#define AOSHTML_PANE_STATUS			3				// Create a pane
#define AOSHTML_EDITORPANE_STATUS	4				// Create a editor_pane

#define AOSHTML_RETRIEVE         	"zky_retrieve"  // RETRIEVE
#define AOSHTML_OBJNAME         	"zky_objname"   // OBJNAME 
#define AOSHTML_OPERATION         	"operation"   	// OPERATION 
#define AOSHTML_XMLVPD         		"zky_xmlvpd"   	// XMLVPD 
#define AOSHTML_XMLOBJ         		"zky_xmlobj"   	// XMLOBJ 
#define AOSHTML_VPDID         		"zky_vpdid"   	// VPDID 
#define AOSHTML_ISINEDITOR        	"zky_isineditor"// ineditor 
#define AOSHTML_JSPATH              "js_path"       // JavaScript Path
#define AOSHTML_CSSPATH             "css_path"      // CSS Path
#define AOSHTML_FILEPATH            "file_path"     // file Path
#define AOSHTML_IMAGEPATH        	"image_path"    // Image Path
#define AOSHTML_AJAXPATH			"ajax_path"      // ajax Path
#define AOSHTML_SIMULATEPATH        "simulate_path" // Simulate Path
#define AOSHTML_PRINTPATH           "print_path"    // Print Path
#define AOSHTML_CHECKFILEEXIST		"check_file_exist"	// Check File Exist
#define webEnd 						"\n"			// Control line break

#define AOSHTML_SESERVERNAME        "seserver_name"  // seserver name
#define AOSHTML_HTMLSERVERNAME      "htmlserver_name"// htmlserver name
#define AOSHTML_ACCESSSERVERNAME    "accessserver_name"// accessserver name

#include "HtmlServer/HtmlClassName.h"
#include "Util/RCObject.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"

#include <vector>

using namespace std;

struct AosHtmlVpdStruct : virtual public OmnRCObject
{
	OmnDefineRCObject;

	AosXmlTagPtr    mVpd;
	AosXmlTagPtr    mObj;
	OmnString		mFlag;
	OmnString 		mParentId;
	OmnString		mVpdPath;
	int				pWidth;
	int				pHeight;
	
	~ AosHtmlVpdStruct()
	{
	}
};

struct AosHtmlFileStruct : virtual public OmnRCObject
{
	OmnDefineRCObject;
	
	OmnString	mName;
	OmnString	mFname;
	OmnString	mSrc;
	int			mFileId;
	u32			mLastMdfTime;
	
	~ AosHtmlFileStruct(){}
};

struct AosHtmlGicStruct : virtual public OmnRCObject
{
	OmnDefineRCObject;
	
	OmnString			mGicType;
	vector<int>			mFileIds;
	
	~ AosHtmlGicStruct()
	{
	}
};

class AosHtmlUtil 
{
	public:
		AosHtmlUtil();
		~AosHtmlUtil();
		bool	setOverflowStyle(
					OmnString &style,
					const OmnString dhsb,
					const OmnString dvsh);
	    
		static OmnString
				getWebColor(const OmnString &color);
		
		static bool	checkABC(const char &a);
};
#endif

