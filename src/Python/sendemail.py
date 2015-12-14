#!/usr/bin env python
# -*- encoding:utf-8 -*-

import email,smtplib,time,string

def test2(a,b,loginpass,sub,content,se,count):
	toAdd =a
	formAdd=b
	msg=email.Message.Message()
	msg['to']=toAdd
	msg['from']=formAdd
	msg['date']=time.ctime()
	msg['subject']=email.Header.Header(sub,'utf-8')

	body=email.MIMEText.MIMEText(content,_subtype='plain',_charset='utf-8')
	try:
		server=smtplib.SMTP(se)
		#server=smtplib.SMTP('smtp.sohu.com')
		if se == "smtp.gmail.com" or se == "smtp.live.com":
			#server.ehlo()
			server.docmd("EHLO server")
			server.starttls()
			#server.esmtp_features["auth"] = "LOGIN PLAIN"
			#server.ehlo
		try:
			server.login(b,loginpass)
		except:
			print "SMTP server login failed, please check the user name and password"
			return 2
		addrs = toAdd
		f = addrs.split(',') 
		for i in f:
			print ("addr: %s\n" % i)
			server.sendmail(formAdd, i, msg.as_string()[:-1] +  body.as_string())
			if count == "usesleep":
				time.sleep(5)
		server.quit()
		print "success!"
	except Exception, e:
		print str(e)
		return 1 
	return 




