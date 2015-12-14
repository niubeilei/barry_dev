#!/usr/bin env python
# -*- encoding:utf-8 -*-
# Create By Brian Zhang
# 07/22/2011 

import email,smtplib,poplib,time,string,base64,sys

text = ""
html = ""
userpath = ""
server = None

def login(se,user,passwd,path):
	
	global server
	global html
	userpath = path
	html = "<state "
	
	try:
		server=poplib.POP3(se)
	except:
		print "link to server failed, try again"
		login(se,user,passwd)
	
	try:
		server.user(user)
		server.pass_(passwd)
	except:
		html += " state=\"false\" "
		html += " errmsg=\"login failed, check user and passws\" />"
		print html
		return html
	html += "state=\"true\" />"
	print "login success"
	return html

def quit():
	
	global server
	global html
	html = "<state "
	
	if server == None:
		html += "state=\"false\" "
		html += "errmsg=\"server is null\" />"
		return html
	else:
		server.quit()
	
	html += "state=\"true\" />"
	print html
	return html

def showlist():
	
	global server
	global html
	html = "<state "

	if server == None:
		html += "state=\"false\" "
		html += "errmsg=\"server is null\" />"
		print "server is null"
		return html

	ret = server.list()[1]
	
	html += "state=\"true\" "
	html += "content=\"" + ret + "\" />"
	return html

def showlist(i):
	
	global server
	global html
	html = "<state "
	
	if server == None:
		html += "state=\"false\" "
		html += "errmsg=\"server is null\" />"
		print "server is null"
		return html
	
	try:
		ret = server.list()[1][i]
	except:
		html += "state=\"false\" "
		html += "errmsg=\"requested list is null\" />"
		return html

	html += "state=\"true\" "
	html += "content=\"" + ret + "\" />"
	return html

def getuidl():
	
	global server
	global html
	html = "<state "
	
	if server == None:
		html += "state=\"false\" "
		html += "errmsg=\"server is null\" />"
		print "server is null"
		return html 
	
	try:
		ret = server.uidl()[1]
	except:
		html += "state=\"false\" "
		html += "errmsg=\"Can't get uidl\" />"
		print "Can't get uidl"
		return html

	html += "state=\"true\" "
	html += "content=\"" + ret + "\" />"
	return html

def getuidlbyidx(i):
	
	global server
	global html
	html = "<state "
	
	if server == None:
		html += "state=\"false\" "
		html += "errmsg=\"server is null\" />"
		print "server is null"
		return html
	
	try:
		ret = server.uidl()[1][i]
	except:
		html += "state=\"false\" "
		html += "errmsg=\"Can't get uidl\" />"
		print "Can't get uidl"
		return html

	html += "state=\"true\" "
	html += "content=\"" + ret + "\" />"
	return html

def dele(i):
	
	global server
	global html
	html = "<state "
	
	if server == None:
		html += "state=\"false\" "
		html += "errmsg=\"server is null\" />"
		print "server is null"
		return html
	
	try:
		ret = server.dele(int(i))
	except:
		html += "state=\"false\" "
		html += "errmsg=\"delete failed\" />"
		print "Delete failed"
		return html

	resp = ret[1:2]
	
	if resp == "OK":
		html += "state=\"true\" />"
		return html
	else:
		html += "state=\"true\" />"
		return html
	return html

def rset():
	
	global server
	global html
	html = "<state "
	
	if server == None:
		html += "state=\"false\" "
		html += "errmsg=\"server is null\" />"
		print "server is null"
		return html
	
	try:
		ret = server.rset()
	except:
		html += "state=\"false\" "
		html += "errmsg=\"reset failed\" />"
		print "rset failed"
		return html

	resp = ret[1:2]
	if resp == "OK":
		html += "state=\"true\" />"
		return html
	else:
		html += "state=\"false\" />"
		return html
	
	html += "state=\"false\" />"
	return html

def getlen(se):

	global server
	global html
	html = "<state "
	
	if server == None:
		html += "state=\"false\" "
		html += "errmsg=\"server is null\" />"
		print "server is null"
		return html
	
	try:
		ret = len(server.list()[1])
	except:
		html += "state=\"false\" "
		html += "errmsg=\"get email number failed\" />"
		return html

	html += "state=\"true\" "
	html += "content=\"" + ret + "\" />"
	return html

	
def showmessage(mail):
	
	global html 
	global text
	global userpath
	
	if mail.is_multipart():
		for part in mail.get_payload():
			
			name = part.get_param("name")
			ctype = part.get_content_type() #text/plain
			subcode = part.get_content_charset()
			print "*******************************************************"
			print "ctype : ", ctype
			print "subcode : ", subcode

			tt = sys.getfilesystemencoding() #utf-8
			if tt != 'utf-8':
				reload(sys)
				sys.setdefaultencoding('utf-8')
				tt = 'utf-8'
		
			t = part.get_payload()
			if 'text/plain'==ctype:
				str = t.decode('base64').decode('gbk').encode(tt)
				print "plain/str : ", str
				html += "<text><CDATA[[" + str + "]]></text>"
					
			elif 'text/html'==ctype:
				str = t.decode('base64').decode('gbk').encode(tt)
				print "html/str : ", str
				html += "<content><CDATA[[" + str + "]]></content>"
			
			elif 'multipart/alternative' == ctype:
				showmessage(t)	
			
			elif 'application/octet-stream' == ctype:
				html += "<attach><CDATA[[" + name + "]]><attach>"
				userpath += name
				data = open(userpath, 'wb')
				data.write(base64.decodestring(t))
				data.close()
			
			elif 'image/jpeg' == ctype:
				html += "<image><CDATA[[" + name + "]]></image>"
				userpath += name
				data = open(userpath, 'wb')
				data.write(base64.decodestring(t))
				data.close()
			
			else:
				print "Undefined ctype : ", ctype

	else:
		type = mail.get_content_charset() #None or "utf-8" and so on
		
		if type==None:
			str =  base64.decodestring(mail.get_payload(decode=True))
		
		elif type == 'gb2312':
			str = mail.get_payload(decode=True).decode(type).encode('utf-8')
		
		else:
			try:
				str = mail.get_payload(decode=True) 
			except:
			    print "fail to get the content" 
		
		html += "<content><CDATA[[" + str + "]]></content>"

def rec(idx):
	
	global html 
	global text
	global server
	html = "<state "
	
	if server == None:
		html += "state=\"false\" "
		html += "errmsg=\"server is null,please login again\" />"
		print "server is null,please login again!"
		return html
	
	hdr,message,octet=server.retr(idx)
	mail=email.message_from_string(string.join(message,'\n'))
	subject=email.Header.decode_header(mail['subject'])[0][0]
	subcode=email.Header.decode_header(mail['subject'])[0][1]
	
	if subcode == None:
		strr = subject
	
	elif subcode == 'utf-8':
		strr = subject
	
	elif subcode == 'gb2312':
		strr = subject.decode(subcode).encode('utf-8')
	
	else:
		try:
			strr = unicode(subject, subcode)
		except:
			html += "state=\"false\" "
			html += "errmsg=\"subject decode error: " + subcode + "\" />"
			print "subject decode error : ", subcode
			return html
	
	html += "subject=\"" + strr +"\" "
	strr = email.utils.parseaddr(mail.get("from"))[1]
	html += "from=\"" + strr + "\" "
	strr = email.utils.parseaddr(mail.get("to"))[1]
	html += "to=\"" + strr + "\" />"
	showmessage(mail)
	html += "</rcv_email>"
	
	print "success!"
	print html
	return html 
