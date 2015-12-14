#include <iostream>
#include <fstream>
#include "../TinyXml.h"
#include "Debug/Debug.h"
#include <iostream>
#include <sstream>

void PrintXMLInfo(const TiXmlNode* pXmlNode)
{
	if (pXmlNode == NULL)
	{
		return;
	}
	const TiXmlNode* pChildNode;
	for (pChildNode=pXmlNode->FirstChild(); pChildNode!=NULL; pChildNode=pChildNode->NextSibling())
	{
		std::cout << pChildNode->Value() << std::endl;
		PrintXMLInfo(pChildNode);
	}
	return;
}

void test1();
void test2();
void test3();
void test4();
void test5();
void test6();

int main()
{
	TiXmlDocument writeXmlDoc("./test.xml");
	writeXmlDoc.LoadFile();
	
	cout << "test.xml: " << writeXmlDoc << endl;
	TiXmlNode *root = writeXmlDoc.FirstChild();
	cout << "root: " << *root << endl;

	const char *objdef = root->getChildTextByAttr("name", "objdef", "");
	const char *operation = root->getChildTextByAttr("name", "operation", "");
	TiXmlNode* pNode = writeXmlDoc.FirstChild("player");
	if (pNode)
	{
		TiXmlElement element("player1");
		writeXmlDoc.InsertBeforeChild(pNode, element);
		writeXmlDoc.RemoveChild(pNode);

	}

//	TiXmlNode* pNode1 = pNode->FirstChild("realPlayer");
//	if (!pNode1)return 0;
//	pNode1->Clear();
//	TiXmlText xText("Text1");
//	pNode1->InsertEndChild(xText);

	/*
	TiXmlElement xElement("player");
	xElement.SetAttribute("admin", "12");

	TiXmlElement xPos("pos");
	xPos.SetAttribute("x", "2");

	xElement.InsertEndChild(xPos);

	TiXmlDeclaration Declaration( "1.0","ISO-8859-1", "yes" ); 
    writeXmlDoc.InsertEndChild( Declaration ); 
	writeXmlDoc.InsertEndChild(xElement);

	TiXmlNode* pElement = writeXmlDoc.FirstChild("player");

	TiXmlElement xMediaElement("mediaPlayer");
	TiXmlElement xRealElement("realPlayer");
	TiXmlElement xPowerElement("PowerPlayer");
	xMediaElement.SetValue("mmmaaa");
	TiXmlText xText("Text");
	xMediaElement.InsertEndChild(xText);

	pElement->InsertEndChild(xRealElement);
	pElement->InsertEndChild(xMediaElement);
	pElement->InsertEndChild(xPowerElement);


*/
	writeXmlDoc.SaveFile("test.xml");

	TiXmlDocument readXmlDoc;
	std::ifstream ifs("test.xml");
	char buffer[102400];
	char c, *p = buffer;
	while(ifs.get(c))
	{
		*p++=c;
	}
	*p = 0;
	ifs.close();

	test1();
	/*
	test2();
	test3();
	test4();
	test5();
	test6();
	*/

	if(!readXmlDoc.Parse(buffer))
	{
		std::cout << readXmlDoc.ErrorDesc() << std::endl;
	}
	else
	{
		PrintXMLInfo(readXmlDoc.RootElement());		
	}

	return 0;
}


const char *mydoc = 
	"<firstnode attr1='123'>"
		"<second attr2='234'>"
			"the contents of second"
			"<third>the contents of third</third>"
			"<fourth>the contents of fourth</fourth>"
		"</second>"
	"</firstnode>";

const char *mydoc2 = 
"<xmlobj><tree objid=\"giccreators_tree\" gic_width='400' gic_height='200' gic_type='gic_tree' tre_input_data_type='in_mem_records' tre_object='system/getCrtVpdEditor' tre_func_name='getLayers' gic_ignore_layout='false' gic_grp_ignore='false' gic_bind_type='auto' gic_layer='1' gic_lbbd='@name' gic_obbd='@dataid' gic_vvpd='' gic_evpd='' gic_vvpd_bd='@vvpdname' gic_evpd_bd='@evpdname' __actions='evt_entselected|$$|act_obj_call_memfunc(object:system/getCrtVpdEditor, fname:loadPanel, parm2:userdata/1, parm4:userdata:attr|$|mDataId/0, parm5:caller/, parm6:userdata/2)' tre_input_data_type='in_mem_records' gic_rootContainer='objtable_a00000000'><datacol gic_type='db_datacol' cmp_tname='objtable' cmp_objid_fname='name' cmp_fnames='name|$|name|$|1' cmp_psize='20' cmp_query='' cmp_order='name'/></tree></xmlobj>";
void test1()
{
	TiXmlDocument thedoc;
	thedoc.Parse(mydoc2);
	TiXmlNode *root = thedoc.FirstChild();

	ostringstream oss(ostringstream::out);
	oss << "========== This is a test: " << *root << "\n";
	cout << oss.str();

	printf("Root name: %s\n", root->Value()); 

	TiXmlElement *ee = root->ToElement();
	printf("Root attr: %s\n", ee->Attribute("attr1")); 

	TiXmlNode *ss = root->FirstChild();
	printf("----------- Second name: %s\n", ss->Value()); 

	TiXmlElement *elem = ss->ToElement();
	printf("Second text: %s\n", elem->GetText()); 
}


void test2()
{
	// This function tests TiXmlElement::getNodeAttribute(...)
	TiXmlDocument thedoc;
	thedoc.Parse(mydoc);	
	TiXmlNode *root = thedoc.FirstChild();
	TiXmlElement *ee = root->ToElement();
	printf("Attr2: %s\n", ee->getNodeAttribute("second/attr2", "Not Found"));

	// Test the function Attribute(name, dft_value)
	printf("Attr1 found: %s\n", ee->Attribute("attr1", "not found"));
	printf("Attr1 not found: %s\n", ee->Attribute("attr2", "not found"));

	ostringstream oss(ostringstream::out);
	oss << "This is a test: " << *ee << "\n";
	cout << oss.str();

	TiXmlNode *child = ee->FirstChild();
	cout << "The child: " << *child << endl;

	TiXmlNode *cc = 0;
	while ((cc = child->IterateChildren(cc)))
	{
		cout << "To check child: " << *cc << endl;
		TiXmlText *tt = cc->ToText();
		if (tt)
		{
			cout << "It is text: " << tt->Value() << endl;
		}
		else
		{
			cout << "It is node: " << *cc << endl;
		}
	}

	cout << "Text: " << child->GetText() << endl;

	cout << "Root: " << *root << endl;
	if (ee->removeAttribute("second/@text"))
	{
		cout << "Attribute removed: " << endl;
		const char *ccc = child->GetText();
		if (ccc) cout << "New text: " << ccc << endl;
		else cout << "Text is null" << endl;
	}
	else
	{
		cout << "Failed to remove attribute" << endl;
	}

	TiXmlElement *child_elem = child->ToElement();
	cout << "Before removing: " << *child_elem << endl;
	cout << "Before removing attribute: " << child_elem->Attribute("attr2") << endl;
	const char *attr = ee->getNodeAttribute("second/attr2", "not found");
	cout << "Before removing: " << attr << endl;
	if (!ee->removeAttribute("second/attr2"))
	{
		cout << "********* Failed to remove attribute attr2" << endl;
	}
	if (!child_elem->Attribute("attr2"))
	{
		cout << "After removing, no attr2" << endl;
	}
	else
	{
		cout << "After removing attribute: " << child_elem->Attribute("attr2") << endl;
	}

	attr = ee->getNodeAttribute("second/attr2", "not found");
	cout << "After removing: " << attr << endl;
}


void test3()
{
	// Test TiXmlNode::get
	// const char *
	// TiXmlNode::getChildTextByAttr(const char *attr_name,
	// 		const char *attr_value,
	// 		const char *dft_value)
	static const char *contents = 
		"<firstnode attr1='123'>"
		    "<second attr2='234'>"
		        "the contents of second"
		        "<third>the contents of third</third>"
		        "<fourth>the contents of fourth</fourth>"
		    "</second>"
			"<second attr2='345'>AAAAAAAAA</second>"
			"<second attr2='456'>BBBBBBBBB</second>"
		"</firstnode>";

	TiXmlDocument thedoc;
	thedoc.Parse(contents);	
	TiXmlNode *root = thedoc.FirstChild();

	const char *v1 = root->getChildTextByAttr("attr111", "234", "not found 1");
	if (strcmp(v1, "not found 1")) cout << "********** Error: " << __LINE__ << endl;

	v1 = root->getChildTextByAttr("attr2", "234", "not found 1");
	if (strcmp(v1, "the contents of second")) 
		cout << "********** Error: " << __LINE__ 
			<< ": " << v1 << endl;

	v1 = root->getChildTextByAttr("attr2", "345", "not found 1");
	if (strcmp(v1, "AAAAAAAAA")) 
		cout << "********** Error: " << __LINE__ << endl;

	v1 = root->getChildTextByAttr("attr2", "456", "not found 1");
	if (strcmp(v1, "BBBBBBBBB")) 
		cout << "********** Error: " << __LINE__ << endl;

	cout << "test3 passed!!!!!!!!!" << endl;
	return;
}


void test4()
{
	// Test the following function:
	// const char *
	// TiXmlNode::getChildText(const char *path, const char *dft_value)
	static const char *contents = 
		"<firstnode attr1='123'>"
		    "<xyz attr2='234'/>"
		    "<second attr2='234'>"
		        "the contents of second"
		        "<third>the contents of third</third>"
		        "<fourth>the contents of fourth</fourth>"
		    "</second>"
			"<second attr2='345'>AAAAAAAAA"
				"<third>CCCCC</third>"
			"</second>"
			"<second attr2='456'>BBBBBBBBB</second>"
		"</firstnode>";

	TiXmlDocument thedoc;
	thedoc.Parse(contents);	
	TiXmlNode *root = thedoc.FirstChild();

	const char *v1 = root->getChildText("second/third", "not found 1");
	bool failed = false;
	if (strcmp(v1, "not found 1") == 0) 
	{
		failed = true;
		cout << "********** Error: " << __LINE__ << endl;
	}

	v1 = root->getChildText("second/fourth", "not found 1");
	if (strcmp(v1, "not found 1") == 0) 
	{
		failed = true;
		cout << "********** Error: " << __LINE__ << endl;
	}

	v1 = root->getChildText("second/fifth", "not found 1");
	if (strcmp(v1, "not found 1")) 
	{
		failed = true;
		cout << "********** Error: " << __LINE__ << endl;
	}

	if (!failed) cout << "test4 passed!!!!!!!!!" << endl;
	else cout << "********** test4 failed!" << endl;
	return;
}


void test5()
{
	// Test the following function:
	// const char *
	// TiXmlNode::firstChildByAttr(const char *attr_name, const char *attr_value)
	static const char *contents = 
		"<firstnode attr1='123'>"
		    "<xyz attr2='234'/>"
		    "<second attr2='234'>"
		        "the contents of second"
		        "<third>the contents of third</third>"
		        "<fourth>the contents of fourth</fourth>"
		    "</second>"
			"<second attr2='345'>AAAAAAAAA"
				"<third>CCCCC</third>"
			"</second>"
			"<second attr2='456'>BBBBBBBBB</second>"
		"</firstnode>";

	TiXmlDocument thedoc;
	thedoc.Parse(contents);	
	TiXmlNode *root = thedoc.FirstChild();

	bool failed = false;
	TiXmlNode *node = root->firstChildByAttr("attr2", "000");
	if (node)
	{
		failed = true;
		cout << "********** Failed at: " << __LINE__ << endl;
	}

	node = root->firstChildByAttr("attr2", "234");
	if (!node)
	{
		failed = true;
		cout << "********** Failed at: " << __LINE__ << endl;
	}

	node = root->firstChildByAttr("attr2", "345");
	if (!node)
	{
		failed = true;
		cout << "********** Failed at: " << __LINE__ << endl;
	}

	if (!failed) cout << "test5 passed!!!!!!!!!" << endl;
	else cout << "********** test5 failed!" << endl;

	return;
}

const char *sgTesting = "<request><item name='operation'><![CDATA[saveObject]]></item><xmlobj><advtab objid=\"giccreators_adv_tab1\" gic_vpdnames=\"vpd_home|$|vpd_product|$|_2|$|vpd_service|$|vpd_about\" gic_labels=\"Home|$|Product|$|Embedded|$|Services|$|About|$|+\" gic_vspace=\"-10\" gic_hspace=\"-10\" gic_menubar_fsize=\"12\" gic_bottominset=\"10\" gic_rightinset=\"10\" gic_topinset=\"10\" gic_leftinset=\"10\" gic_menu_width=\"80\" gic_menubar_h=\"50\" gic_layout=\"top\" gic_height=\"400\" gic_width=\"600\" gic_type=\"gic_adv_tab\" width=\"100%\" height=\"100%\"><panels><_2/></panels><menubar gic_labels=\"Home|$|Product|$|Embedded|$|Services|$|About\" gic_label_fsize=\"12\" gic_bottomInset=\"0\" gic_topInset=\"0\" gic_rightInset=\"6\" gic_leftInset=\"5\" gic_spacing=\"2\" gic_type=\"gic_simpleMenubar\" gic_layout=\"top\" gic_meu_height=\"50\" gic_meu_width=\"80\" gic_height=\"60\" gic_width=\"600\"><layoutmgr lm_ctn_halign=\"left\" lm_hspace=\"0\" lm_vspace=\"0\" lm_l_inset=\"4\" lm_t_inset=\"0\" lm_name=\"lm_hori\"/><menuitem cmp_xoffset=\"0\" cmp_yoffset=\"0\" cmp_height=\"50\" cmp_width=\"-1\" cmp_fsize=\"12\" cmp_voffset=\"0\" cmp_hoffset=\"0\" cmp_r_inset=\"10\" cmp_l_inset=\"10\" name=\"comp_labeledrec\"><framer cmp_yoffset=\"0\" cmp_xoffset=\"0\" cmp_height=\"80\" cmp_width=\"69\" cmp_sizedef=\"framer_3pc005\" cmp_hot=\"2\" gic_type=\"framer_3pc\" cmp_src=\"3pc005\"/></menuitem><bgrec cmp_height=\"80\" cmp_width=\"580\" gic_type=\"framer_empty\" cmp_sizedef=\"framer_NinePiece001\" cmp_src=\"NinePiece001\"/></menubar><pane container_y_pos=\"ignore\" container_x_pos=\"ignore\" container_height_def=\"fixed\" container_width_def=\"fixed\" container_bgcolor=\"0xffffff\" pane_type=\"normal_pane\"><frame cmp_height=\"310\" cmp_width=\"580\" cmp_pc8_is_5=\"true\" cmp_src=\"NinePiece001\" cmp_sizedef=\"framer_NinePiece001\" gic_type=\"framer_8psimple\"/></pane></advtab></xmlobj><item name='objid'><![CDATA[giccreators_adv_tab]]></item><item name='trans_id'><![CDATA[10]]></item></request>";


void test6()
{
	TiXmlDocument thedoc;
	thedoc.Parse(sgTesting);
	TiXmlNode *root = thedoc.FirstChild();

	ostringstream oss(ostringstream::out);
	oss << *root;
	cout << "Result: " << oss.str().c_str() << endl;
}

