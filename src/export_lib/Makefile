#all directorys you want to make
SubDirs = common 

#add your directorys here
SubDirs += inetctrl 
SubDirs += firewall 
SubDirs += wan 
SubDirs += vlan
SubDirs += qos
SubDirs += router
SubDirs += mac
SubDirs += dmz
SubDirs += webwall
SubDirs += resmgr
SubDirs += dnsmasq
SubDirs += rosos
SubDirs += keyman
SubDirs += pptp
SubDirs += pppoe
SubDirs += systemMisc
SubDirs += systemtime

PWD:sh = /bin/pwd

all:
	@for i in $(SubDirs) ; do cd $(PWD)/$${i}; \
	make $@ || exit; cd $(PWD); done
	
clean:
	@for i in $(SubDirs) ; do cd $(PWD)/$${i}; \
	make $@; cd $(PWD); done

