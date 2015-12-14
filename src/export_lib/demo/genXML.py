#!/usr/bin/python
import xml.dom.minidom
import sys
import string
import commands

is_debug = 1

xmlPath = sys.argv[1]
'''
convert file
'''
tmpFile = "tmpfile"
f_src = open(xmlPath, "r")
f_dst = open(tmpFile, "w")
lines = f_src.xreadlines()
f_dst.write("<Cmds>\n")

for line in lines:
    if string.count(line, "//") > 0:
        if is_debug == 1:
            print "omit\n"
        continue
    elif string.count(line, "<Usage>") > 0:
        if is_debug == 1:
            print "omit\n"
        continue
    else:
        f_dst.write(line)

f_dst.write("</Cmds>")

f_src.close()
f_dst.close()

xmlDoc = xml.dom.minidom.parse(tmpFile)
f = open(sys.argv[2], "w")
allLines = ""
'''
get all cmds
'''
cmds = xmlDoc.getElementsByTagName("Cmd")

for cmd in cmds:
    
    api_name = ""
    cli_prefix = ""
    description = ""
    xml_api = ""
    '''
    start to parsing command
    '''
    api_name = "aos_api_" + cmd.getElementsByTagName("OprID")[0].firstChild.data
    cli_prefix = cmd.getElementsByTagName("Prefix")[0].firstChild.data
    if is_debug == 1:
        print "api_name is " + api_name
        print "cli_prefix is " + cli_prefix
    '''
    parsing params
    '''
    type_names = []
    params = cmd.getElementsByTagName("Parm")
    for param in params:
        type_name = param.childNodes[1].firstChild.data

        if is_debug == 1:
            print "param type is " + type_name
        
        type_names.append(type_name)
    xml_api = "\n\t\t<api_name>" + api_name + "</api_name>\n"
    xml_api = xml_api + "\t\t<protype>\n"

    counter_int = 0
    counter_str = 0
    for type_name in type_names:
        if type_name == "int":
            type_name = "int"
            counter_int = counter_int + 1
        else:
            type_name = "char*"
            counter_str = counter_str + 1
            
        xml_api = xml_api + "\t\t\t<param>\n\t\t\t\t<type>" + type_name + "</type>\n\t\t\t\t<var>"
        if type_name == "int":
            xml_api = xml_api + "int" + str(counter_int) + "</var>\n\t\t\t</param>\n"
        else:
            xml_api = xml_api + "str" + str(counter_str) + "</var>\n\t\t\t</param>\n"
    
    xml_api = xml_api + "\t\t</protype>\n"
    xml_api = xml_api + "\t\t<prefix>" + cli_prefix + "</prefix>\n"
    xml_api = xml_api + "\t\t<description>this is description</description>\n"

    xml_api = "\t<api>" + xml_api + "\t\t<testcases>\n\t\t</testcases>\n\t</api>\n"

    if is_debug == 1:
        print xml_api
    allLines = allLines + xml_api

allLines = "<apis>\n\t<module>" + sys.argv[3] + "</module>\n" + allLines + "</apis>\n" 
f.write(allLines);

f.close()

'''
delete tmp file
'''
commands.getoutput("rm " + tmpFile)

    
    
