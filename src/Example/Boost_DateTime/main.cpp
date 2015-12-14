#include "boost/date_time/posix_time/posix_time.hpp"
#include <iostream>	
#include <stdio.h>	
using namespace std;
int	main()	
{	  
	using namespace boost::gregorian;	  
	using namespace boost::posix_time;
	//date d(2000,Jan,20);
	/*date d = from_string("2000/01/01");
	ptime start(d);
	ptime end = start + hours(1);
	time_iterator titr(start,minutes(15)); 
	while (titr < end) {
		std::cout << to_simple_string(*titr) << std::endl;	    
		++titr;	  
	}	  
	std::cout << "Now backward" << std::endl;*/
	ptime p1 = time_from_string("2010-02-22 14:19:43");
	//ptime p1 = time_from_string("02-22-2010 14:19:43");
	/*
	std::cout << "P1:" << to_simple_string(p1) << std::endl;
	ptime p2 = second_clock::local_time();
	std::cout << "P2:" << to_simple_string(p2) << std::endl;
	time_duration p3 = p2-p1;
	std::cout << "p2-p1:" << p3.hours() << std::endl;

	ptime p4 = p1 - hours(3);
	std::cout << "p1-3h:" << to_simple_string(p4) << std::endl;
	std::cout <<  p1.date().year() << "/" <<
		   p1.date().month() << "/"  <<  p1.date().day() <<  " " << 
		   p1.time_of_day().hours() <<  ":"  << 
		   p1.time_of_day().minutes() <<  ":" <<
		   p1.time_of_day().seconds() << std::endl;

	date d = p1.date();
	std::cout << d.month()<< endl;
	//time_facet tfacet("%Y/%m/%d %H:%M:%S");
	*/
	//time_facet *tfacet = new time_facet("%m-%d-%Y %H:%M:%S");
	time_facet *tfacet = new time_facet("%Y年%m月%d日%H:%M:%S");
	//std::cout.imbue(std::locale(std::cout.getloc(), tfacet));
	//std::cout << p1 << std::endl;

	{
		ostringstream os;
		os.imbue(std::locale(std::cout.getloc(), tfacet));
		os << p1;

		cout << os.str() << endl;
	}
	/*
	date_facet* f = new date_facet();
	f->month_format("%m"); 
	std::cout.imbue(std::locale(std::cout.getloc(), f));
	std::cout << d.month() << endl;
	*/
}



//g++ -g -Wall main.cpp -o datetest.exe -lboost_date_time
