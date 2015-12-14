#!/usr/bin/python
import xml.dom.minidom
import sys

xmlPath = "getset.xml"
xmlDoc = xml.dom.minidom.parse(xmlPath)
'''
get the module name
'''
module_name = xmlDoc.getElementsByTagName("module")

'''
parse api section
'''
allLines_cpp = "\n"

allLines_h = "\n"

apis = xmlDoc.getElementsByTagName("api");

dstPath_cpp = module_name[0].firstChild.data + ".cpp"
dstPath_h = "../include/" + module_name[0].firstChild.data + ".h"

f_cpp = open(dstPath_cpp, "w")
f_h = open(dstPath_h, "w")

    
fns = xmlDoc.getElementsByTagName("getParm")
for  fn in fns:
	n = fn.childNodes[1].firstChild.data
	t = fn.childNodes[3].firstChild.data
	allLines_cpp = allLines_cpp +"\n\t"+ t + "\t\tget" + n + "()const {return m" + n + ";}"  	

f_h.write(allLines_h)
fns = xmlDoc.getElementsByTagName("setParm")
for  fn in fns:
	n = fn.childNodes[1].firstChild.data
	nd = fn.childNodes[3].firstChild.data
	t = fn.childNodes[5].firstChild.data
	allLines_cpp = allLines_cpp +"\n\tvoid\t\tset" + n + "(const " + t + " &" + nd + "){" + n + " = " + nd + ";}"

f_h.write(allLines_h)
f_cpp.write(allLines_cpp)
f_cpp.close()
f_h.close()        
