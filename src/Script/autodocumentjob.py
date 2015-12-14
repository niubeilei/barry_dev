import socket  
import struct;
from xml.dom import minidom
import time
import string
import subprocess
import sys
import os 

#define the class

class ConfigObj :
	mDict = {
			'auto':'', 'server_ip':'','server_port':'', 'loginname':'','loginpasswd':'','dir_backup_name':'','source1_dir_name':'',
			'source2_dir_name':'','dest_dir_name':'','dest_dir_name_tmp':'','copydata':'','sendlog':'','zky_sdoc_objid':'','zky_jobname':''
			}
	def __init__(self):
		print "init"
	
		
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

#read config file
print "read config file"
xmldoc = minidom.parse('/home/prod/Script/documentconfig.xml')
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

#check dir is empty
if os.listdir(configobj.mDict['source1_dir_name']) == []:
	print "SIP DIR IS EMPTY!"
	sys.exit(0)

cptime = time.strftime('%Y%m%d%H%M%S',time.localtime(time.time()))	
backupdir = configobj.mDict['dir_backup_name'] + "/" + cptime 

print "make new dir"
mkcmd = ""
mkcmd = "mkdir -p " + backupdir
print mkcmd 

mkrslt = subprocess.call(mkcmd, shell=True)
if mkrslt != 0:
	if mkrslt < 0:
		print "Killed by signal, on making dir", mkrslt 
	else:
		print "Command failed with return code, on making dir", mkrslt 
	sys.exit(0)
else:
	print "--------MAKE NEW  DIR SUCCESS---------"


print "Copy SIP xml data to BackUp Dir"
cpsipcmd = ""
cpsipcmd = "cp " + configobj.mDict['source1_dir_name'] + "/*.xml " + backupdir 
print cpsipcmd 

cpsiprslt = subprocess.call(cpsipcmd, shell=True)
if cpsiprslt != 0:
	if cpsiprslt < 0:
		print "Killed by signal, on coping", cpsiprslt 
	else:
		print "Command failed with return code, on coping", cpsiprslt 
#	sys.exit(0)
else:
	print "--------COPY XML DATA SUCCESS---------"

print "move SIP xml data"
mvcmd = ""
mvcmd = "mv " + configobj.mDict['source1_dir_name'] + "/*.xml " + configobj.mDict['dest_dir_name_tmp']
print mvcmd

rv = subprocess.call(mvcmd, shell=True)
if rv != 0:
	if rv < 0:
		print "Killed by signal, on moving", rv
	else:
		print "Command failed with return code, on moving", rv
#	sys.exit(0)
else:
	print "--------MOVE XML DATA SUCCESS---------"

print "copy File eippack data to BackuUp DIR"
cpfile = ""
cpfile = "cp " + configobj.mDict['source2_dir_name'] + "/*.eippack " + backupdir 
print cpfile 

cpfilerslt = subprocess.call(cpfile, shell=True)
if cpfilerslt != 0:
	if cpfilerslt < 0:
		print "Killed by signal, on copying", cpfilerslt 
	else:
		print "Command failed with return code, on copying", cpfilerslt 
#	sys.exit(0)
else:
	print "--------COPY EIPPACK DATA SUCCESS---------"

print "move File eippack data"
mvcmd2 = ""
mvcmd2 = "mv " + configobj.mDict['source2_dir_name'] + "/*.eippack " + configobj.mDict['dest_dir_name_tmp']
print mvcmd2

rv2 = subprocess.call(mvcmd2, shell=True)
if rv2 != 0:
	if rv2 < 0:
		print "Killed by signal, on moving", rv2
	else:
		print "Command failed with return code, on moving", rv2
#	sys.exit(0)
else:
	print "--------MOVE EIPPACK DATA SUCCESS---------"


if (cpfilerslt != 0) and (cpsiprslt != 0):
	sys.exit(0)
else:
	print "--------COPY DATA SUCCESS---------"



#change source filename to utf-8

print "it is ready to change format"
cmd = "convmv -f GBK -t UTF-8 --notest " + configobj.mDict['dest_dir_name_tmp'] + "/*";
print cmd

ret = subprocess.call(cmd, shell=True)
if ret != 0:
	if ret < 0:
		print "Killed by signal", ret
	else:
		print "Command failed with return code", ret
	sys.exit(0)
else:
	print "--------CHANGE DATA FORMAT SUCCESS---------"

print "move TMP xml data"
destcmd = ""
destcmd = "mv " + configobj.mDict['dest_dir_name_tmp'] + "/* " + configobj.mDict['dest_dir_name']
print destcmd 

destrv = subprocess.call(destcmd, shell=True)
if destrv != 0:
	if destrv < 0:
		print "Killed by signal, on removing", destrv 
	else:
		print "Command failed with return code, on removing", destrv 
	sys.exit(0)
else:
	print "--------MOVE TMP XML DATA SUCCESS---------"

#login request
print "------login------"
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
	"<embedobj zky_jobname=\"" + zky_jobname  + "\" zky_scheduled=\"false\" zky_timing_objid=\"system_addtimer_after\" zky_job_stime=\"" + crttime + "\" zky_singlerun=\"true\" zky_sdoc_objid=\"" + zky_sdoc_objid + "\" zky_runjob_objid=\"system_runjob\" zky_status=\"stop\" zky_jobid=\"rootjob\" zky_public_doc=\"true\" zky_public_ctnr=\"true\" zky_pctrs=\"jobunicom\">"
	"<env>"
	"<entry><![CDATA[" + crtdate + "]]></entry>"
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

	if errorflag == "true":
		print "create job error!"
	else: 
		print "create job success!"
		print "--------run job--------"
		#run job
		runjobstarttime = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time()))	
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


closeSock(sock) 
