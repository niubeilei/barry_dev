////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <boost/regex.hpp>

using namespace boost ;
using namespace std ;

void match();
void search();
void replace();

int main(int argc, char * argv[])
{
	cout << "1. Match testing ..." << endl;
	match();
	return 0;
	cout << "\n2. Search testing ..." << endl;
	search();
	cout << "\n3. Replace testing ..." << endl;
	replace();
}


void match()
{
	string s = "(\\w+)\\s+(\\w+)\\s+['\"]{0,1}(\\w+)['\"]{0,1}(\\s*;|\\s+(\\{[\\s\\S]*\\})\\s*;)"; 
	//string s = "(\\w+)\\s+(\\w+)\\s+['\"]{0,1}(\\w+)['\"]{0,1}\\s+\\{[\\s\\S]*\\};"; //({[\\s\\S]*})\\s*;";
	try 
	{ 
		boost::regex reg(s); 
		std::string s1 = "create dataproc dp {name:111,age:[10,20],sex:{name:aaa,age:b}};";
		std::string s2 = "create schema \"name\";";
		cout << "String 1:" << s1 << endl;
		cout << "String 2:" << s2 << endl;
		cout << "Reg Str:" << s << endl;
		if (boost::regex_match(s1, reg))
			cout << "String1" << "\t Matched" << endl;
		else
			cout << "String1" << "\t Miss" << endl;
		if (boost::regex_match(s2, reg))
			cout << "String2" << "\t Matched" << endl;
		else
			cout << "String2" << "\t Miss" << endl;
	}
	catch(const boost::bad_expression& e)
	{
		cout << "That's not a valid regular expression!" << e.what() << endl;
	}
}


void search()
{
	string r = "(\\d{3})-(\\w+)";
	try 
	{ 
		boost::regex reg(r); 
		boost::smatch m;
		std::string s1 = "010-abcdae";
		std::string s2 = "0a0-abde";
		std::string s3 = "050-addde";
		std::string s = s1 + "," + s2 + "," + s3;
		cout << "String:" << s << endl;
		cout << "Reg Str:" << r << endl;
		std::string::const_iterator it = s.begin();
		std::string::const_iterator end = s.end();
		string match, submatch1, submatch2;
		while(boost::regex_search(it, end, m, reg))
		{
			match += m[0] + ",";
			submatch1 += m[1] + ",";
			submatch2 += m[2] + ",";
			it = m[0].second;   
		}
		cout << "macth :" << match.erase(match.size()-1,1) << endl;
		cout << "subMacth1 :" << submatch1.erase(submatch1.size()-1,1) << endl;
		cout << "subMacth2 :" << submatch2.erase(submatch2.size()-1,1) << endl;
	} 
	catch(const boost::bad_expression& e)
	{ 
		cout << "That's not a valid regular expression!" << e.what() << endl;
	}
}


void replace()
{
	std::string r = "<(\\w+)([^/>]*)(/>)";
	try 
	{ 
		boost::regex reg(r); 
		boost::smatch m;
		std::string s = "<node1><node2 att1=\"b\"/><node3/><node4 attr2=\"a\" attr3=\"a\"/><node5>text1</node5></node1>";
		cout << "String:" << s << endl;
		cout << "Reg Str:" << r << endl;
		s = boost::regex_replace(s, reg, "<$1$2></$1>");
		cout << "Replace format:" << "<$1$2></$1>" << endl;
		cout << s << endl;
	} 
	catch(const boost::bad_expression& e)
	{ 
		cout << "That's not a valid regular expression!" << e.what() << endl;
	}
}
