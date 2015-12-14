import socket  
import struct;
from xml.dom import minidom
import datetime
import time
import string
import subprocess
import sys

import os  
from os.path import join, getsize  
import dircache

#define the class

class ConfigObj :
	mDict = {
			'auto':'', 'server_ip':'','server_port':'', 'loginname':'','loginpasswd':'','source_dir_name':'', 
				'dest_dir_name':'','deal_dir_name':'','copydata':'','sendlog':'','zky_sdoc_objid':'','zky_jobname':''
			}
	def __init__(self):
		print "init"
	

def get_size(start_path = '.'):
    total_size = 0
    for dirpath, dirnames, filenames in os.walk(start_path):
        for f in filenames:
            fp = os.path.join(dirpath, f)
            total_size += os.path.getsize(fp)
    return total_size  
		
# define the function

def initSocket(addr, port):
	address = (addr, string.atoi(port))  
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
	s.connect(address)  
	return s
def closeSock(sock):
	sock.close() 
	
def sendMsg(sock, request):

	slen = socket.htonl(len(request));
	print "----------- send msg ---------- length : " + str(slen) 
	bytes=struct.pack('I',slen);
	sock.send(bytes);
	sock.send(request);
	rlen, = struct.unpack('I',sock.recv(4));
	rlen = socket.ntohl(rlen & 0xffffffff);
	response = sock.recv(rlen);
	print "\nResponse:len=", rlen
	print response;
	return response
def addLog(element, name, value):
	element.setAttribute(name, value)
def sendLog(sock, logelement, ssid):
	logstr = (
	"<request>"
	"<item name='operation'><![CDATA[serverreq]]></item>"
	"<item name='zky_siteid'><![CDATA[100]]></item>"
	"<item name='username'><![CDATA[nonameyet]]></item>"
	"<item name='trans_id'><![CDATA[1]]></item>"
	"<objdef>"
	"<sdoc zky_sdoctp='runaction'>"
	"<actions>"
	"<action zky_type='createlog'>"
	"<container zky_value_type='const'>unicombinarylog</container>"
	"<logname zky_value_type='const'>ubl</logname>"
	"<zky_docselector zky_doc_xpath='actions/action/zky_docselector/log' zky_type='workingdoc'>" + logelement.toxml() + "</zky_docselector>"
	"</action>"
	"</actions>"
	"</sdoc>"
	"</objdef>"
	"<item name='sendto'><![CDATA[seserver]]></item>"
	"<item name='loginobj'><![CDATA[true]]></item>"
	"<item name='zkyurldocdid'><![CDATA[0]]></item>"
	"<item name='reqid'><![CDATA[sdocrun]]></item>"
	"<item name='trans_id'><![CDATA[161]]></item>"
	"<zky_cookies>"
	"<cookie zky_name='JSESSIONID'><![CDATA[29D5725B945544B08FC23F8EBC627E3E]]></cookie>"
	"<cookie zky_name='zky_browserid'><![CDATA[1372712330]]></cookie>"
	"<cookie zky_name='zky_ssid_6073'><![CDATA[" + ssid +"]]></cookie>"
	"</zky_cookies>"
	"</request>"
	)
	sendMsg(sock, logstr)

#init log xml
logdom = minidom.Document()
logdom.appendChild(logdom.createComment("This is a log xml."))
logelement = logdom.createElement("log")
logdom.appendChild(logelement)

#read config file
print "read config file"
addLog(logelement, "initconfig", "true")

xmldoc = minidom.parse('/home/prod/Script/config.xml')
itemlist = xmldoc.getElementsByTagName('item') 
print len(itemlist)
configobj = ConfigObj()
for s in itemlist :
	key = s.attributes['name'].value
	if s.firstChild.nodeType != s.TEXT_NODE:
		raise Exception("node does not contain text")
	nodevalue = s.firstChild.nodeValue
	configobj.mDict[key] = nodevalue
	print key
	print configobj.mDict[key] 


print "remove old data"
rmcmd = ""
rmcmd = "rm -rf " + configobj.mDict['dest_dir_name'] + "/*"
print rmcmd

rv = subprocess.call(rmcmd, shell=True)
if rv != 0:
	if rv < 0:
		print "Killed by signal, on removing", rv
	else:
		print "Command failed with return code, on removing", rv
	sys.exit(0)
else:
	print "--------REMOVE DATA SUCCESS---------"


#copy source file to remote dirname

print "it is ready to copy data"
copystarttime = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time()))	
addLog(logelement, "copy_data_start_time", copystarttime)
cmd = ""
baseTime= ""
autoflag = configobj.mDict['auto']
if autoflag == "true":
	print "auto operation"
	today = time.strftime('%Y%m%d',time.localtime(time.time()))	
	baseTime = today
else:
	baseTime = configobj.mDict['deal_dir_name']
	print "manual operation"

#get real date
deltaFileHandler = open('/home/prod/Script/delta.txt', 'r+')
delta = deltaFileHandler.readline()
baseDate = datetime.datetime(string.atoi(baseTime[0:4]),string.atoi(baseTime[4:6]),string.atoi(baseTime[6:8]))
realDate = baseDate + datetime.timedelta(days = string.atoi(delta))
realDateStr=realDate.strftime("%Y%m%d")

cmd = "cp -r " + configobj.mDict['source_dir_name'] + "/" + realDateStr +  " " + configobj.mDict['dest_dir_name']
print cmd

ret = subprocess.call(cmd, shell=True)
if ret != 0:
	if ret < 0:
		print "Killed by signal", ret
	else:
		print "Command failed with return code", ret
	sys.exit(0)
else:
	print "--------COPY DATA SUCCESS---------"

copyendtime = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time()))	
addLog(logelement, "copy_data_end_time", copyendtime)



#login request
print "------login------"
addLog(logelement, "server_ip", configobj.mDict['server_ip'])
addLog(logelement, "server_port", configobj.mDict['server_port'])
addLog(logelement, "loginname", configobj.mDict['loginname'])
addLog(logelement, "loginpwd", configobj.mDict['loginpasswd'])
sock = initSocket(configobj.mDict['server_ip'], configobj.mDict['server_port'])
request = r"<request><item name='operation'><![CDATA[serverCmd]]></item><item name='zky_siteid'><![CDATA[100]]></item><item name='username'><![CDATA[nonameyet]]></item><item name='trans_id'><![CDATA[15]]></item><command><embedobj zky_uname='" + configobj.mDict['loginname'] + "' login_group='yunyuyan_account' container='yunyuyan_account' rattrs='zky_category|sep418|zky_cloudid__a|sep418|zky_hpvpd|sep418|zky_wdvpd|sep418|zky_objimg|sep418|zky_heditor|sep418|zky_hmctnr|sep418|zky_blgctnr|sep418|zky_cmtctnr|sep418|zky_atclctnr|sep418|zky_publish|sep418|zky_diary|sep418|zky_abmctnr|sep418|zky_ushmdir|sep418|zky_usimgdir|sep418|zky_usrscdir|sep418|zky_fans|sep418|zky_objnm|sep418|zky_unit|sep418|zky_class|sep418|zky_realnm|sep418|zky_uname|sep418|zky_passwd__n|sep417|zky_passwd__n|sep417|2' opr='login'><zky_passwd__n><![CDATA[" + configobj.mDict['loginpasswd'] + "]]></zky_passwd__n></embedobj></command><item name='sendto'><![CDATA[seserver]]></item><item name='loginobj'><![CDATA[true]]></item><item name='zkyurldocdid'><![CDATA[190591]]></item><item name='operation'><![CDATA[retrieveFullVpd]]></item><item name='OsName'><![CDATA[Linux]]></item><item name='userAgent'><![CDATA[Mozilla/5.0 (Windows NT 5.1; rv:21.0) Gecko/20100101 Firefox/21.0]]></item><item name='trans_id'><![CDATA[196]]></item><item name='subOpr'><![CDATA[]]></item><item name='OsArch'><![CDATA[amd64]]></item><item name='loginobj'><![CDATA[true]]></item><item name='OsVersion'><![CDATA[3.0.0-12-server]]></item><zky_cookies><cookie zky_name='JSESSIONID'><![CDATA[7980BCD69DE3BFFC79F170D71EF5A50A]]></cookie><cookie zky_name='zky_browserid'><![CDATA[1372194217]]></cookie></zky_cookies></request>"

response = sendMsg(sock, request)

#parse response
loginresponse = "<xml>" + response + "</xml>"
print loginresponse
doc = minidom.parseString(loginresponse)
node = doc.getElementsByTagName("status")[0] 
errorflag = node.getAttribute("error") 
ContentsNode  = doc.getElementsByTagName("Contents")[0]  
loginobj = ContentsNode.getElementsByTagName("zky_lgnobj")[0]
cloudid = loginobj.getAttribute("zky_crtor") 
ssid = loginobj.getAttribute("zky_ssid") 
print errorflag, cloudid, ssid

if errorflag == "true":
	print "login error!"
else: 
	print "login success!"
	print "it is ready to create job"
	#create job
	print time.time()
	crttime = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time()))	
	crtdate = time.strftime('%Y%m%d',time.localtime(time.time()))	
	zky_jobname= configobj.mDict['zky_jobname']  + "_" + str(crttime)
	zky_sdoc_objid = configobj.mDict['zky_sdoc_objid'] 
	addLog(logelement, "createjob_start_time", crttime)
	addLog(logelement, "zky_jobname", configobj.mDict['zky_jobname'])
	addLog(logelement, "zky_sdoc_objid", configobj.mDict['zky_sdoc_objid'])

	myname = socket.getfqdn(socket.gethostname())
	ip = socket.gethostbyname(myname)
	source_dir_name = configobj.mDict['source_dir_name'] 
	dest_dir_name = configobj.mDict['dest_dir_name'] 
	total_size = str(get_size(dest_dir_name))
	list = dircache.listdir(dest_dir_name)
	num_files = str(len(list)) 
	print total_size, num_files, ip

	jobrequest = (
	"<request>"
	"<item name='operation'><![CDATA[serverCmd]]></item>"
	"<item name='zky_siteid'><![CDATA[100]]></item>"
	"<item name='username'><![CDATA[nonameyet]]></item>"
	"<item name='trans_id'><![CDATA[6]]></item>"
	"<command>"
	"<obj opr='createcobj'/>"
	"</command>"
	"<objdef>"
"<embedobj zky_jobname=\"" + zky_jobname  + "\" zky_scheduled=\"false\" zky_timing_objid=\"system_addtimer_after\" zky_job_stime=\"" + crttime + "\" zky_singlerun=\"true\" zky_sdoc_objid=\"" + zky_sdoc_objid + "\" zky_runjob_objid=\"system_runjob\" zky_status=\"stop\" zky_jobid=\"rootjob\" zky_public_doc=\"true\" zky_public_ctnr=\"true\" zky_pctrs=\"jobunicom\" zky_otype=\"jobdoc\" staging_server=\"" + ip + "\" source_dir_name=\"" + source_dir_name + "\" working_dir=\"" + dest_dir_name + "\" total_size=\"" + total_size + "\" num_files=\"" + num_files + "\" deal_dir_name=\"" + realDateStr + "\" >"
	"<env>"
	"<entry zky_key=\"time\"><![CDATA[" + crtdate + "]]></entry>"
	"<entry zky_key=\"dirname\"><![CDATA[" + realDateStr + "]]></entry>"
	"</env>"
	"</embedobj>"
	"</objdef>"
	"<item name='sendto'><![CDATA[seserver]]></item>"
	"<item name='zkyurldocdid'><![CDATA[6073]]></item>"
	"<item name='operation'><![CDATA[retrieveFullVpd]]></item>"
	"<item name='OsName'><![CDATA[Linux]]></item>"
	"<item name='userAgent'><![CDATA[Mozilla/5.0 (Windows NT 5.1; rv:21.0) Gecko/20100101 Firefox/21.0]]></item>"
	"<item name='trans_id'><![CDATA[133]]></item>"
	"<item name='subOpr'><![CDATA[]]></item>"
	"<item name='OsArch'><![CDATA[amd64]]></item>"
	"<item name='OsVersion'><![CDATA[3.2.0-35-generic]]></item>"
	"<zky_cookies>"
	"<cookie zky_name='JSESSIONID'><![CDATA[65945E0978F76ED32C799F7956C80D1E]]></cookie>"
	"<cookie zky_name='zky_browserid'><![CDATA[0]]></cookie>"
	"<cookie zky_name='zky_ssid_6086'><![CDATA[BGAsAEIBAATlJQSAyAKCQcvCKvWqe9kj]]></cookie>"
	"<cookie zky_name='zky_ssid_6073'><![CDATA[" + ssid + "]]></cookie>"
	"</zky_cookies>"
	"</request>"
	)
	jobrsps = sendMsg(sock, jobrequest)


	#parse response
	jobresponse = "<xml>" + jobrsps + "</xml>"
	jobrespdoc = minidom.parseString(jobresponse)
	jobrespnode = jobrespdoc.getElementsByTagName("status")[0] 
	ContentsNode  = jobrespdoc.getElementsByTagName("Contents")[0]  
	jobdoc_objid = ContentsNode.getAttribute("zky_objid") 
	errorflag = jobrespnode.getAttribute("error") 
	print errorflag,jobdoc_objid
	addLog(logelement, "job_docid", jobdoc_objid)

	if errorflag == "true":
		print "create job error!"
	else: 
		print "create job success!"
		print "--------run job--------"
		#run job
		runjobstarttime = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time()))	
		addLog(logelement, "runjob_start_time", runjobstarttime)
		runjobrequest = (
		"<request>"
		"<item name='operation'><![CDATA[serverreq]]></item>"
		"<item name='zky_siteid'><![CDATA[100]]></item>"
		"<item name='username'><![CDATA[nonameyet]]></item>"
		"<item name='trans_id'><![CDATA[6]]></item>"
		"<item name='args'><![CDATA[mid=jobmgr,cmdid=startjob,objid=" + jobdoc_objid + "]]></item>"
		"<item name='sendto'><![CDATA[seserver]]></item>"
		"<item name='zkyurldocdid'><![CDATA[6073]]></item>"
		"<item name='reqid'><![CDATA[runcommand]]></item>"
		"<item name='operation'><![CDATA[retrieveFullVpd]]></item>"
		"<item name='OsName'><![CDATA[Linux]]></item>"
		"<item name='userAgent'><![CDATA[Mozilla/5.0 (Windows NT 5.1; rv:21.0) Gecko/20100101 Firefox/21.0]]></item>"
		"<item name='trans_id'><![CDATA[131]]></item>"
		"<item name='subOpr'><![CDATA[]]></item>"
		"<item name='OsArch'><![CDATA[amd64]]></item>"
		"<item name='OsVersion'><![CDATA[3.2.0-35-generic]]></item>"
		"<zky_cookies>"
		"<cookie zky_name='JSESSIONID'><![CDATA[65945E0978F76ED32C799F7956C80D1E]]></cookie>"
		"<cookie zky_name='zky_browserid'><![CDATA[0]]></cookie>"
		"<cookie zky_name='zky_ssid_6086'><![CDATA[BGAsAEIBAATlJQSAyAKCQcvCKvWqe9kj]]></cookie>"
		"<cookie zky_name='zky_ssid_6073'><![CDATA[" + ssid + "]]></cookie>"
		"</zky_cookies>"
		"</request>"
		)

		jobrsps = sendMsg(sock, runjobrequest)
		sendLog(sock, logelement, ssid)


closeSock(sock) 

#run job successful, then delta add 1
delta_new = string.atoi(delta) + 1
print delta_new
deltaFileHandler.seek(0, 0)
deltaFileHandler.write(str(delta_new))
deltaFileHandler.close()
