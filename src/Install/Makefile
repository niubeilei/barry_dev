# File: makefile
#
# This is a makefile for use with g++
#

INSTARGET = install-keyou.sh install-rhc.sh make-mystro-install-package.sh install-aos.sh install-mystro.sh make-aos-install-package.sh make-keyou-install-package.sh make-rhc-install-package.sh
INSDIR = ../../install

AOSSRC = $(shell pwd)/..
LINUXAOS = $(LINUXHOME)/net/aos
LINUXINC = $(LINUXHOME)/include

LINUX_IPV4 = $(LINUXHOME)/net/ipv4
LINUX_CORE = $(LINUXHOME)/net/core
LINUX_FS = $(LINUXHOME)/fs
HOOKDIR_2_6_7 = $(AOSSRC)/KernelHooking/2.6.7

nothing:
	echo "Please use parameters"
check:
	@echo "Source directory=$(AOSSRC)"
	@echo "LINUXHOME=$(LINUXHOME)"
	@if [ a$(LINUXHOME) == a ]; then false; fi	
	@echo "Check Success"

KernelHooking_2_6_7:
	ln -s -f $(HOOKDIR_2_6_7)/tcp_timer.c 		$(LINUX_IPV4)/tcp_timer.c
	ln -s -f $(HOOKDIR_2_6_7)/tcp_output.c 		$(LINUX_IPV4)/tcp_output.c
	ln -s -f $(HOOKDIR_2_6_7)/tcp_ipv4.c 		$(LINUX_IPV4)/tcp_ipv4.c
	ln -s -f $(HOOKDIR_2_6_7)/tcp_minisocks.c 	$(LINUX_IPV4)/tcp_minisocks.c
	ln -s -f $(HOOKDIR_2_6_7)/syncookies.c 		$(LINUX_IPV4)/syncookies.c
	ln -s -f $(HOOKDIR_2_6_7)/tcp_input.c 		$(LINUX_IPV4)/tcp_input.c
	ln -s -f $(HOOKDIR_2_6_7)/tcp.c 		$(LINUX_IPV4)/tcp.c
	ln -s -f $(HOOKDIR_2_6_7)/af_inet.c 		$(LINUX_IPV4)/af_inet.c
	ln -s -f $(HOOKDIR_2_6_7)/arp.c 			$(LINUX_IPV4)/arp.c
	ln -s -f $(HOOKDIR_2_6_7)/sock.h 			$(LINUXINC)/net/sock.h
	ln -s -f $(HOOKDIR_2_6_7)/sock.c 			$(LINUX_CORE)/sock.c
	ln -s -f $(HOOKDIR_2_6_7)/dev.c 			$(LINUX_CORE)/dev.c
	ln -s -f $(HOOKDIR_2_6_7)/main.c 			$(LINUXHOME)/init/main.c

HOOKDIR_2_6_15_5 = $(AOSSRC)/KernelHooking/2.6.15.5
KernelHooking_2_6_15_5:
#	ln -s -f $(HOOKDIR_2_6_15_5)/tcp_timer.c 		$(LINUX_IPV4)/tcp_timer.c
#	ln -s -f $(HOOKDIR_2_6_15_5)/tcp_output.c 		$(LINUX_IPV4)/tcp_output.c
#	ln -s -f $(HOOKDIR_2_6_15_5)/tcp_ipv4.c 		$(LINUX_IPV4)/tcp_ipv4.c
#	ln -s -f $(HOOKDIR_2_6_15_5)/tcp_minisocks.c 	$(LINUX_IPV4)/tcp_minisocks.c
#	ln -s -f $(HOOKDIR_2_6_15_5)/syncookies.c 		$(LINUX_IPV4)/syncookies.c
#	ln -s -f $(HOOKDIR_2_6_15_5)/tcp_input.c 		$(LINUX_IPV4)/tcp_input.c
#	ln -s -f $(HOOKDIR_2_6_15_5)/tcp.c 		$(LINUX_IPV4)/tcp.c
#	ln -s -f $(HOOKDIR_2_6_15_5)/af_inet.c 		$(LINUX_IPV4)/af_inet.c
#	ln -s -f $(HOOKDIR_2_6_15_5)/arp.c 			$(LINUX_IPV4)/arp.c
	ln -s -f $(HOOKDIR_2_6_15_5)/skbuff.h 		$(LINUXINC)/linux/skbuff.h
	ln -s -f $(HOOKDIR_2_6_15_5)/sock.c 		$(LINUX_CORE)/sock.c
	ln -s -f $(HOOKDIR_2_6_15_5)/skbuff.c 		$(LINUX_CORE)/skbuff.c
	ln -s -f $(HOOKDIR_2_6_15_5)/dev.c 			$(LINUX_CORE)/dev.c
	ln -s -f $(HOOKDIR_2_6_15_5)/exec.c 		$(LINUX_FS)/exec.c
	ln -s -f $(HOOKDIR_2_6_15_5)/main.c 		$(LINUXHOME)/init/main.c
	ln -s -f $(HOOKDIR_2_6_15_5)/udp.c 			$(LINUX_IPV4)/udp.c
	ln -s -f $(HOOKDIR_2_6_15_5)/tcp_output.c 	$(LINUX_IPV4)/tcp_output.c
	ln -s -f $(HOOKDIR_2_6_15_5)/tcp_ipv4.c 	$(LINUX_IPV4)/tcp_ipv4.c
	ln -s -f $(HOOKDIR_2_6_15_5)/icmp.c 		$(LINUX_IPV4)/icmp.c
	ln -s -f $(HOOKDIR_2_6_15_5)/snmp.h 		$(LINUXINC)/net/snmp.h
	ln -s -f $(HOOKDIR_2_6_15_5)/vlan_dev.c 	$(LINUXHOME)/net/8021q/vlan_dev.c
	

systemcall:
	ln -s -f $(HOOKDIR_2_6_15_5)/unistd.h 			$(LINUXHOME)/include/asm-i386/unistd.h
	ln -s -f $(HOOKDIR_2_6_15_5)/syscall_table.S 	$(LINUXHOME)/arch/i386/kernel/syscall_table.S

installStarvation: systemcall
	if [ -e $(LINUXAOS)/starvation ]; then rm -rf $(LINUXAOS)/starvation; fi; 
	mkdir $(LINUXAOS)/starvation 
	ln -sf $(AOSSRC)/starvation/kernel-Makefile	    $(LINUXAOS)/starvation/Makefile
	ln -sf $(AOSSRC)/starvation/Kconfig 		$(LINUXAOS)/starvation/Kconfig
	ln -sf $(AOSSRC)/starvation/starvation.c 		$(LINUXAOS)/starvation/starvation.c

installPanicNotifier: 
	if [ -e $(LINUXAOS)/PanicNotifier ]; then rm -rf $(LINUXAOS)/PanicNotifier; fi; 
	mkdir $(LINUXAOS)/PanicNotifier
	ln -sf $(AOSSRC)/PanicNotifier/kernel-Makefile	    $(LINUXAOS)/PanicNotifier/Makefile
	ln -sf $(AOSSRC)/PanicNotifier/Kconfig 		$(LINUXAOS)/PanicNotifier/Kconfig
	ln -sf $(AOSSRC)/PanicNotifier/panicnotifier.c 		$(LINUXAOS)/PanicNotifier/panicnotifier.c

installRosOS:
	if [ -e $(LINUXAOS)/RosOS ]; then rm -rf $(LINUXAOS)/RosOS; fi; 
	mkdir $(LINUXAOS)/RosOS
	ln -sf $(AOSSRC)/RosOS/kernel-Makefile	    $(LINUXAOS)/RosOS/Makefile
	ln -sf $(AOSSRC)/RosOS/Kconfig 				$(LINUXAOS)/RosOS/Kconfig
	ln -sf $(AOSSRC)/RosOS/ros_os_cli.c 		$(LINUXAOS)/RosOS/ros_os_cli.c


binlog: systemcall
	ln -fs $(AOSSRC)/binlog							$(LINUXAOS)

AOSCORE = $(LINUXAOS)/AosCore
CREATE_AOSCORE = $(shell if [ -d $(AOSCORE) ]; then echo ""; else mkdir $(AOSCORE); fi)

AOS_CORE = $(AOSSRC)/aos_core
installAosCore:
	$(CREATE_AOSCORE)
	rm -f $(AOSCORE)/*
	ln -s -f $(AOS_CORE)/Makefile-AosCore 		$(AOSCORE)/Makefile
	ln -s -f $(AOS_CORE)/TcpSockBridge.cpp 		$(AOSCORE)/TcpSockBridge.c
	ln -s -f $(AOS_CORE)/aosBridge.cpp 			$(AOSCORE)/aosBridge.c
	ln -s -f $(AOS_CORE)/aosBuiltinRules.cpp 	$(AOSCORE)/aosBuiltinRules.c
	ln -s -f $(AOS_CORE)/aosConfig.cpp 			$(AOSCORE)/aosConfig.c
	ln -s -f $(AOS_CORE)/aosCoreComm.cpp 		$(AOSCORE)/aosCoreComm.c
	ln -s -f $(AOS_CORE)/aosCorePcp.cpp 		$(AOSCORE)/aosCorePcp.c
	ln -s -f $(AOS_CORE)/aosDev.cpp 			$(AOSCORE)/aosDev.c
	ln -s -f $(AOS_CORE)/aosSockCtl.cpp 		$(AOSCORE)/aosSockCtl.c
	ln -s -f $(AOS_CORE)/aosFwdDev.cpp 			$(AOSCORE)/aosFwdDev.c
	ln -s -f $(AOS_CORE)/aosFwder.cpp 			$(AOSCORE)/aosFwder.c
	ln -s -f $(AOS_CORE)/aosKernelApi.cpp 		$(AOSCORE)/aosKernelApi.c
	ln -s -f $(AOS_CORE)/aosMacCtlr.cpp 		$(AOSCORE)/aosMacCtlr.c
	ln -s -f $(AOS_CORE)/aosMacRule.cpp 		$(AOSCORE)/aosMacRule.c
	ln -s -f $(AOS_CORE)/aosMultiCaster.cpp 	$(AOSCORE)/aosMultiCaster.c
	ln -s -f $(AOS_CORE)/aosPktStat.cpp 		$(AOSCORE)/aosPktStat.c
	ln -s -f $(AOS_CORE)/aosRule.cpp 			$(AOSCORE)/aosRule.c
	ln -s -f $(AOS_CORE)/aosRuleNetInput.cpp 	$(AOSCORE)/aosRuleNetInput.c
	ln -s -f $(AOS_CORE)/aosSkbRecv.cpp 		$(AOSCORE)/aosSkbRecv.c
	ln -s -f $(AOS_CORE)/aosSockBridge.cpp 		$(AOSCORE)/aosSockBridge.c
	ln -s -f $(AOS_CORE)/aosSock.cpp 			$(AOSCORE)/aosSock.c
	ln -s -f $(AOS_CORE)/aosSocket.cpp 			$(AOSCORE)/aosSocket.c
	ln -s -f $(AOS_CORE)/aosSockSSL.cpp 		$(AOSCORE)/aosSockSSL.c
	ln -s -f $(AOS_CORE)/TcpSockBridge.cpp 		$(AOSCORE)/TcpSockBridge.c
	ln -s -f $(AOS_CORE)/SockFirewall.cpp 		$(AOSCORE)/SockFirewall.c
	ln -s -f $(AOS_CORE)/aosRuleHeadSwapper.cpp $(AOSCORE)/aosRuleHeadSwapper.c
	ln -s -f $(AOS_CORE)/aosArp.cpp 			$(AOSCORE)/aosArp.c

K_UTIL = $(AOSSRC)/KernelUtil
installKernelUtil:
	ln -s -f $(K_UTIL)/aosKernelAlarm.cpp 		$(AOSCORE)/aosKernelAlarm.c
	ln -s -f $(K_UTIL)/MgdObj.cpp 				$(AOSCORE)/MgdObj.c
	ln -s -f $(K_UTIL)/KernelLock.cpp 			$(AOSCORE)/KernelLock.c
	ln -s -f $(K_UTIL)/TArray.cpp 				$(AOSCORE)/TArray.c
	ln -s -f $(K_UTIL)/aostcpapi.c 				$(AOSCORE)/aostcpapi.c
	ln -s -f $(K_UTIL)/KernelStat.cpp			$(AOSCORE)/KernelStat.c


KAPI = $(AOSSRC)/KernelAPI
installKernelAPI:
	ln -s -f $(KAPI)/KernelWrapper/TestAosApi.c $(AOSCORE)/TestAosApi.c


AOSUTIL = $(AOSSRC)/aosUtil
installAosUtil:
	ln -s -f $(AOSUTIL)/Alarm.cpp 			$(AOSCORE)/AosAlarm.c
	ln -s -f $(AOSUTIL)/Modules.cpp 		$(AOSCORE)/Modules.c
	ln -s -f $(AOSUTIL)/Tracer.cpp 			$(AOSCORE)/Tracer.c
	ln -s -f $(AOSUTIL)/CharPtree.cpp 		$(AOSCORE)/CharPtree.c
	ln -s -f $(AOSUTIL)/UrlPtree.cpp 		$(AOSCORE)/UrlPtree.c
	ln -s -f $(AOSUTIL)/StringUtil.cpp 		$(AOSCORE)/StringUtil.c
	ln -s -f $(AOSUTIL)/Slab.cpp 			$(AOSCORE)/Slab.c
	ln -s -f $(AOSUTIL)/Random.cpp 			$(AOSCORE)/Random.c
	ln -s -f $(AOSUTIL)/Debug.cpp 			$(AOSCORE)/Debug.c
	ln -s -f $(AOSUTIL)/HashBin.cpp 		$(AOSCORE)/HashBin.c
	ln -s -f $(AOSUTIL)/Timer.cpp 			$(AOSCORE)/aosTimer.c
	ln -s -f $(AOSUTIL)/TimerPack.cpp 		$(AOSCORE)/TimerPack.c
	ln -s -f $(AOSUTIL)/BusyLoop.cpp 		$(AOSCORE)/BusyLoop.c
	ln -s -f $(AOSUTIL)/aosArpProxy.cpp 	$(AOSCORE)/aosArpProxy.c
	ln -s -f $(AOSUTIL)/aos_hashtab.c 		$(AOSCORE)/aos_hashtab.c
	
SECURE_SRC = $(AOSSRC)/aos_secure
SECURE_DST = $(LINUXAOS)/aos_secure
CREATE_SECURE_DIR = (if [ -e $(SECURE_DST) ]; then rm -rf $(SECURE_DST); fi; mkdir $(SECURE_DST) )
installSecure:
	$(CREATE_SECURE_DIR)
	ln -sf $(SECURE_SRC)/Makefile	            $(SECURE_DST)/Makefile
	ln -sf $(SECURE_SRC)/SecureAuthCmd.c	            $(SECURE_DST)/SecureAuthCmd.c
	
TRAFFIC_SRC = $(AOSSRC)/aos_traffic
TRAFFIC_DST = $(LINUXAOS)/aos_traffic
CREATE_TRAFFIC_DIR = (if [ -e $(TRAFFIC_DST) ]; then rm -rf $(TRAFFIC_DST); fi; mkdir $(TRAFFIC_DST) )
installTraffic:
	$(CREATE_TRAFFIC_DIR)
	ln -sf $(TRAFFIC_SRC)/Makefile	            $(TRAFFIC_DST)/Makefile
	ln -sf $(TRAFFIC_SRC)/TrafficStat.c	            $(TRAFFIC_DST)/TrafficStat.c


SERVERS = $(AOSSRC)/Servers
installServers:
	ln -s -f $(SERVERS)/AppServer.cpp 			$(AOSCORE)/AppServer.c
	ln -s -f $(SERVERS)/ServerGroup.cpp 		$(AOSCORE)/ServerGroup.c


AAA_DIR = $(AOSSRC)/AAA
installAAA:
	ln -s -f $(AAA_DIR)/AaaServer.cpp 			$(AOSCORE)/AaaServer.c


MAKEFILE_DIR = $(AOSSRC)/KernelMakefiles
install2_6_7Makefiles:
	ln -sf  $(MAKEFILE_DIR)/Makefile-2.6.7 			$(LINUXHOME)/Makefile
	ln -sf $(MAKEFILE_DIR)/Kconfig-arch-2.6.7 		$(LINUXHOME)/arch/i386/Kconfig
	ln -sf $(MAKEFILE_DIR)/aosKconfig-2.6.7 		$(LINUXHOME)/net/aosKconfig
	ln -sf $(MAKEFILE_DIR)/Makefile-net-2.6.7 		$(LINUXHOME)/net/Makefile
	ln -sf $(MAKEFILE_DIR)/Makefile-aos-2.6.7 		$(LINUXAOS)/Makefile
	ln -sf $(MAKEFILE_DIR)/Kconfig-aos-2.6.7 		$(LINUXAOS)/Kconfig
	ln -sf $(MAKEFILE_DIR)/Kconfig-char-2.6.7		$(LINUXHOME)/drivers/char/Kconfig
	ln -sf $(MAKEFILE_DIR)/Makefile-char-2.6.7		$(LINUXHOME)/drivers/char/Makefile

install2_6_15_5Makefiles:
	ln -sf  $(MAKEFILE_DIR)/Makefile-2.6.15.5 			$(LINUXHOME)/Makefile
	ln -sf $(MAKEFILE_DIR)/Kconfig-arch-2.6.15.5 		$(LINUXHOME)/arch/i386/Kconfig
	ln -sf $(MAKEFILE_DIR)/aosKconfig-2.6.15.5 		$(LINUXHOME)/net/aosKconfig
	ln -sf $(MAKEFILE_DIR)/Makefile-net-2.6.15.5 		$(LINUXHOME)/net/Makefile
	ln -sf $(MAKEFILE_DIR)/Makefile-aos-2.6.15.5 		$(LINUXAOS)/Makefile
	ln -sf $(MAKEFILE_DIR)/Kconfig-aos-2.6.15.5 		$(LINUXAOS)/Kconfig
	ln -sf $(MAKEFILE_DIR)/Kconfig-char-2.6.15.5		$(LINUXHOME)/drivers/char/Kconfig
#	ln -sf $(MAKEFILE_DIR)/Makefile-char-2.6.15.5		$(LINUXHOME)/drivers/char/Makefile

PKCS_SRC = $(AOSSRC)/PKCS
CERT_VERIFY_SRC = $(AOSSRC)/CertVerify
ASN1_SRC = $(AOSSRC)/ASN1
CERT_VERIFY_DST = $(LINUXAOS)/CertVerify
CREATE_CERT_DIR = (if [ -d $(CERT_VERIFY_DST) ]; then echo ""; else mkdir $(CERT_VERIFY_DST); fi)
installCertVerify:
	$(CREATE_CERT_DIR)
	rm -f $(CERT_VERIFY_DST)/*
	ln -s -f $(CERT_VERIFY_SRC)/Makefile-CertVerify $(CERT_VERIFY_DST)/Makefile
	ln -s -f $(CERT_VERIFY_SRC)/aosAccessManage.h	$(CERT_VERIFY_DST)/aosAccessManage.h
	ln -s -f $(CERT_VERIFY_SRC)/aosBase64.h			$(CERT_VERIFY_DST)/aosBase64.h
	ln -s -f $(CERT_VERIFY_SRC)/aosCertVerify.h		$(CERT_VERIFY_DST)/aosCertVerify.h
	ln -s -f $(CERT_VERIFY_SRC)/aostcpapi.h			$(CERT_VERIFY_DST)/aostcpapi.h
	ln -s -f $(PKCS_SRC)/asn1.h						$(CERT_VERIFY_DST)/asn1.h
	ln -s -f $(CERT_VERIFY_SRC)/cert_verify.h		$(CERT_VERIFY_DST)/cert_verify.h
	ln -s -f $(CERT_VERIFY_SRC)/common.h			$(CERT_VERIFY_DST)/common.h
	ln -s -f $(CERT_VERIFY_SRC)/crypt_hash.h		$(CERT_VERIFY_DST)/crypt_hash.h
	ln -s -f $(CERT_VERIFY_SRC)/_list.h				$(CERT_VERIFY_DST)/_list.h
	ln -s -f $(PKCS_SRC)/x509.h						$(CERT_VERIFY_DST)/x509.h
	ln -s -f $(PKCS_SRC)/x509_object.h				$(CERT_VERIFY_DST)/x509_object.h
	ln -s -f $(CERT_VERIFY_SRC)/x509_ocsp.h			$(CERT_VERIFY_DST)/x509_ocsp.h
	ln -s -f $(PKCS_SRC)/x509_util.h				$(CERT_VERIFY_DST)/x509_util.h
	ln -s -f $(CERT_VERIFY_SRC)/amm_priv.h			$(CERT_VERIFY_DST)/amm_priv.h

	ln -s -f $(CERT_VERIFY_SRC)/access_manage.c		$(CERT_VERIFY_DST)/access_manage.c
	ln -s -f $(PKCS_SRC)/asn1_decode.cpp		$(CERT_VERIFY_DST)/asn1_decode.c
	ln -s -f $(PKCS_SRC)/asn1_encode.cpp		$(CERT_VERIFY_DST)/asn1_encode.c
	ln -s -f $(PKCS_SRC)/Base64.cpp					$(CERT_VERIFY_DST)/Base64.c
	ln -s -f $(CERT_VERIFY_SRC)/cert_verify.c		$(CERT_VERIFY_DST)/cert_verify.c
	ln -s -f $(CERT_VERIFY_SRC)/md5.c				$(CERT_VERIFY_DST)/md5.c
	ln -s -f $(PKCS_SRC)/CRL.cpp					$(CERT_VERIFY_DST)/CRL.c
	ln -s -f $(PKCS_SRC)/CertMgr.cpp				$(CERT_VERIFY_DST)/CertMgr.c
	ln -s -f $(PKCS_SRC)/CertChain.cpp				$(CERT_VERIFY_DST)/CertChain.c
	ln -s -f $(PKCS_SRC)/sha1.cpp				$(CERT_VERIFY_DST)/sha1.c
	ln -s -f $(PKCS_SRC)/x509.cpp				$(CERT_VERIFY_DST)/x509.c
	ln -s -f $(PKCS_SRC)/x509_object.cpp		$(CERT_VERIFY_DST)/x509_object.c
	ln -s -f $(CERT_VERIFY_SRC)/x509_ocsp.c			$(CERT_VERIFY_DST)/x509_ocsp.c
	ln -s -f $(PKCS_SRC)/x509_util.cpp			$(CERT_VERIFY_DST)/x509_util.c
	ln -s -f $(CERT_VERIFY_SRC)/jns_ocsp.c			$(CERT_VERIFY_DST)/jns_ocsp.c
	ln -s -f $(ASN1_SRC)/AsnCodec.cpp				$(CERT_VERIFY_DST)/AsnCodec.c

HTTP_SRC = $(AOSSRC)/AppProc/http_proc
APP_DST = $(LINUXAOS)/AppProc
CREATE_APP_DIR = (if [ -d $(APP_DST) ]; then echo ""; else mkdir $(APP_DST); fi)
installAppProc:
	$(CREATE_APP_DIR)
	rm -f $(APP_DST)/*
	ln -s -f $(HTTP_SRC)/Makefile-HTTP  $(APP_DST)/Makefile
	ln -s -f $(HTTP_SRC)/AppProcReq.h	$(APP_DST)/AppProcReq.h
	ln -s -f $(HTTP_SRC)/config.h		$(APP_DST)/config.h
	ln -s -f $(HTTP_SRC)/HTTP.h			$(APP_DST)/HTTP.h
	ln -s -f $(HTTP_SRC)/HttpParser.h	$(APP_DST)/HttpParser.h
	ln -s -f $(HTTP_SRC)/SmtpParser.h	$(APP_DST)/SmtpParser.h
	
	ln -s -f $(HTTP_SRC)/AppProcReq.c	$(APP_DST)/AppProcReq.c
	ln -s -f $(HTTP_SRC)/HTTP.c			$(APP_DST)/HTTP.c
	ln -s -f $(HTTP_SRC)/HttpParser.c	$(APP_DST)/HttpParser.c
	ln -s -f $(HTTP_SRC)/SmtpParser.c	$(APP_DST)/SmtpParser.c
	ln -s -f $(HTTP_SRC)/ForwardTable.c	$(APP_DST)/ForwardTable.c
	ln -s -f $(HTTP_SRC)/AutoLogin.c	$(APP_DST)/AutoLogin.c
	ln -s -f $(HTTP_SRC)/HttpProcSet.c  	$(APP_DST)/HttpProcSet.c
	ln -s -f $(HTTP_SRC)/HttpProcSet.h  	$(APP_DST)/HttpProcSet.h
	ln -s -f $(HTTP_SRC)/url_decode.h  	$(APP_DST)/url_decode.h
	ln -s -f $(HTTP_SRC)/url_decode.c  	$(APP_DST)/url_decode.c
	
	
installProxy:
	ln -sf $(AOSSRC)/Proxy/proxy_hook.h			$(LINUXINC)/linux/proxy_hook.h
	ln -fs $(AOSSRC)/Proxy						$(LINUXAOS)



APPPROXY = $(AOSSRC)/AppProxy
TCPVS_SRC = $(AOSSRC)/Ktcpvs
TCPVS_DST = $(LINUXAOS)/AosTcpvs
CREATE_KTCPVS_DIR = $(shell if [ -d $(TCPVS_DST) ]; then echo ""; else mkdir $(TCPVS_DST); fi)
installKtcpvs:
	$(CREATE_KTCPVS_DIR)
	rm -f $(TCPVS_DST)/*
	ln -s -f $(TCPVS_SRC)/Kconfig-tcpvs-2.6.7 	$(TCPVS_DST)/Kconfig
	ln -s -f $(TCPVS_SRC)/Makefile-Ktcpvs 		$(TCPVS_DST)/Makefile
	ln -s -f $(TCPVS_SRC)/aosProxyConn.c 		$(TCPVS_DST)/aosProxyConn.c
	ln -s -f $(TCPVS_SRC)/misc.c 				$(TCPVS_DST)/misc.c
	ln -s -f $(TCPVS_SRC)/redirect.c 			$(TCPVS_DST)/redirect.c
	ln -s -f $(TCPVS_SRC)/tcp_vs.c 			$(TCPVS_DST)/tcp_vs.c
	ln -s -f $(TCPVS_SRC)/aosKernelFile.c 			$(TCPVS_DST)/aosKernelFile.c
	ln -s -f $(TCPVS_SRC)/aosLogFunc.c 			$(TCPVS_DST)/aosLogFunc.c
	ln -s -f $(TCPVS_SRC)/aosCompress.c 			$(TCPVS_DST)/aosCompress.c
	ln -s -f $(TCPVS_SRC)/tcp_vs_ctl.c 		$(TCPVS_DST)/tcp_vs_ctl.c
	ln -s -f $(TCPVS_SRC)/tcp_vs_http.c 		$(TCPVS_DST)/tcp_vs_http.c
	ln -s -f $(TCPVS_SRC)/tcp_vs_http_parser.c 	$(TCPVS_DST)/tcp_vs_http_parser.c
	ln -s -f $(TCPVS_SRC)/tcp_vs_jns.c 		$(TCPVS_DST)/tcp_vs_jns.c
	ln -s -f $(TCPVS_SRC)/tcp_vs_ftp.c 		$(TCPVS_DST)/tcp_vs_ftp.c
	ln -s -f $(TCPVS_SRC)/tcp_vs_ssh.c 		$(TCPVS_DST)/tcp_vs_ssh.c
	ln -s -f $(TCPVS_SRC)/tcp_vs_tcp.c 		$(TCPVS_DST)/tcp_vs_tcp.c
	ln -s -f $(TCPVS_SRC)/tcp_vs_telnet.c 		$(TCPVS_DST)/tcp_vs_telnet.c
	ln -s -f $(TCPVS_SRC)/tcp_vs_sched.c 		$(TCPVS_DST)/tcp_vs_sched.c
	ln -s -f $(TCPVS_SRC)/tcp_vs_timer.c 		$(TCPVS_DST)/tcp_vs_timer.c
	ln -s -f $(TCPVS_SRC)/regex.c 				$(TCPVS_DST)/regex.c
	ln -s -f $(TCPVS_SRC)/aosTcpVsCntl.c 		$(TCPVS_DST)/aosTcpVsCntl.c
	ln -s -f $(TCPVS_SRC)/interface.c 			$(TCPVS_DST)/interface.c
	ln -s -f $(TCPVS_SRC)/aosMemCheck.c 		$(TCPVS_DST)/aosMemCheck.c
	ln -s -f $(APPPROXY)/AppProxy.cpp 			$(TCPVS_DST)/AppProxy.c
	ln -s -f $(APPPROXY)/DenyPage.cpp 			$(TCPVS_DST)/DenyPage.c



CERT_SSL_SRC = $(AOSSRC)/ssl
CERT_SSL_DST = $(LINUXAOS)/AosSsl
CREATE_SSL_DIR = (if [ -d $(CERT_SSL_DST) ]; then echo ""; else mkdir $(CERT_SSL_DST); fi)
installSsl:
	$(CREATE_SSL_DIR)
	rm -f $(CERT_SSL_DST)/*
	ln -s -f $(CERT_SSL_SRC)/Makefile-SSL 	$(CERT_SSL_DST)/Makefile
	ln -s -f $(CERT_SSL_SRC)/aosSslProc.h	$(CERT_SSL_DST)/aosSslProc.h
	ln -s -f $(CERT_SSL_SRC)/base64.h		$(CERT_SSL_DST)/base64.h
	ln -s -f $(CERT_SSL_SRC)/des.h			$(CERT_SSL_DST)/des.h
	ln -s -f $(CERT_SSL_SRC)/md5.h			$(CERT_SSL_DST)/md5.h
	ln -s -f $(CERT_SSL_SRC)/random.h		$(CERT_SSL_DST)/random.h
	ln -s -f $(CERT_SSL_SRC)/ReturnCode.h	$(CERT_SSL_DST)/ReturnCode.h
	ln -s -f $(CERT_SSL_SRC)/rc4.c			$(CERT_SSL_DST)/rc4.c
	ln -s -f $(CERT_SSL_SRC)/rsa_arth.h		$(CERT_SSL_DST)/rsa_arth.h
	ln -s -f $(CERT_SSL_SRC)/rsa.h			$(CERT_SSL_DST)/rsa.h
	ln -s -f $(CERT_SSL_SRC)/sha1.h			$(CERT_SSL_DST)/sha1.h
	ln -s -f $(CERT_SSL_SRC)/Ssl.h			$(CERT_SSL_DST)/Ssl.h
	ln -s -f $(CERT_SSL_SRC)/SslApi.h		$(CERT_SSL_DST)/SslApi.h
	ln -s -f $(CERT_SSL_SRC)/SslClient.h	$(CERT_SSL_DST)/SslClient.h
	ln -s -f $(CERT_SSL_SRC)/SslMisc.h		$(CERT_SSL_DST)/SslMisc.h
	ln -s -f $(CERT_SSL_SRC)/SslRecord.h	$(CERT_SSL_DST)/SslRecord.h
	ln -s -f $(CERT_SSL_SRC)/SslServer.h	$(CERT_SSL_DST)/SslServer.h
	ln -s -f $(CERT_SSL_SRC)/SslSessionMgr.h	$(CERT_SSL_DST)/SslSessionMgr.h
	ln -s -f $(CERT_SSL_SRC)/SslStatemachine.h	$(CERT_SSL_DST)/SslStatemachine.h
	
	ln -s -f $(CERT_SSL_SRC)/aosSslProc.c	$(CERT_SSL_DST)/aosSslProc.c
	ln -s -f $(CERT_SSL_SRC)/des.c			$(CERT_SSL_DST)/des.c
	ln -s -f $(CERT_SSL_SRC)/md5.c			$(CERT_SSL_DST)/md5.c
	ln -s -f $(CERT_SSL_SRC)/random.c		$(CERT_SSL_DST)/random.c
	ln -s -f $(CERT_SSL_SRC)/rsa.c			$(CERT_SSL_DST)/rsa.c
	ln -s -f $(CERT_SSL_SRC)/sha1.c			$(CERT_SSL_DST)/sha1.c
	ln -s -f $(CERT_SSL_SRC)/Ssl.c			$(CERT_SSL_DST)/Ssl.c
	ln -s -f $(CERT_SSL_SRC)/SslClient.c	$(CERT_SSL_DST)/SslClient.c
	ln -s -f $(CERT_SSL_SRC)/SslMisc.c		$(CERT_SSL_DST)/SslMisc.c
	ln -s -f $(CERT_SSL_SRC)/SslRecord.c	$(CERT_SSL_DST)/SslRecord.c
	ln -s -f $(CERT_SSL_SRC)/SslServer.c	$(CERT_SSL_DST)/SslServer.c
	ln -s -f $(CERT_SSL_SRC)/SslSessionMgr.c	$(CERT_SSL_DST)/SslSessionMgr.c
	ln -s -f $(CERT_SSL_SRC)/SslStatemachine.c	$(CERT_SSL_DST)/SslStatemachine.c
	ln -s -f $(CERT_SSL_SRC)/SslCipher.c	$(CERT_SSL_DST)/SslCipher.c
	ln -s -f $(CERT_SSL_SRC)/cavium.c		$(CERT_SSL_DST)/cavium.c

CARD_SRC = $(AOSSRC)/JNS9054Card
CARD_DST = $(LINUXAOS)/JNS9054Card
CREATE_CARD_DIR = (if [ -d $(CARD_DST) ]; then echo ""; else mkdir $(CARD_DST); fi)
installEncryptCard:
	$(CREATE_CARD_DIR)
	rm -f $(CARD_DST)/*
	ln -s -f $(CARD_SRC)/Makefile 			$(CARD_DST)/Makefile
	ln -s -f $(CARD_SRC)/j9054.c 			$(CARD_DST)/j9054.c
	ln -s -f $(CARD_SRC)/jmk_lib_internal.c	$(CARD_DST)/jmk_lib_internal.c
	ln -s -f $(CARD_SRC)/lib_9054jmk.c 		$(CARD_DST)/lib_9054jmk.c

CAVIUM_CARD_SRC = $(AOSSRC)/CaviumDriver/cavium_driver
CAVIUM_CARD_DST = $(LINUXAOS)/cavium_driver
CREATE_CAVIUM_CARD_DIR = (if [ -d $(CAVIUM_CARD_DST) ]; then echo ""; else mkdir $(CAVIUM_CARD_DST); fi)
installCaviumCard:
	$(CREATE_CAVIUM_CARD_DIR)
	rm -f $(CAVIUM_CARD_DST)/*
	ln -s -f $(CAVIUM_CARD_SRC)/Makefile 			$(CAVIUM_CARD_DST)/Makefile
	ln -s -f $(CAVIUM_CARD_SRC)/admin_command_que.c 			$(CAVIUM_CARD_DST)/admin_command_que.c
	ln -s -f $(CAVIUM_CARD_SRC)/admin_command_que.h 			$(CAVIUM_CARD_DST)/admin_command_que.h
	ln -s -f $(CAVIUM_CARD_SRC)/api_test.c 			$(CAVIUM_CARD_DST)/api_test.c
	ln -s -f $(CAVIUM_CARD_SRC)/api_test.h 			$(CAVIUM_CARD_DST)/api_test.h
	ln -s -f $(CAVIUM_CARD_SRC)/bl_nbl_list.c 			$(CAVIUM_CARD_DST)/bl_nbl_list.c
	ln -s -f $(CAVIUM_CARD_SRC)/bl_nbl_list.h 			$(CAVIUM_CARD_DST)/bl_nbl_list.h
	ln -s -f $(CAVIUM_CARD_SRC)/buffer_pool.c 			$(CAVIUM_CARD_DST)/buffer_pool.c
	ln -s -f $(CAVIUM_CARD_SRC)/buffer_pool.h 			$(CAVIUM_CARD_DST)/buffer_pool.h
	ln -s -f $(CAVIUM_CARD_SRC)/cavium.c 			$(CAVIUM_CARD_DST)/cavium.c
	ln -s -f $(CAVIUM_CARD_SRC)/cavium.h 			$(CAVIUM_CARD_DST)/cavium.h
	ln -s -f $(CAVIUM_CARD_SRC)/cavium_be.h 			$(CAVIUM_CARD_DST)/cavium_be.h
	ln -s -f $(CAVIUM_CARD_SRC)/cavium_cn1010.mod.c 			$(CAVIUM_CARD_DST)/cavium_cn1010.mod.c
	ln -s -f $(CAVIUM_CARD_SRC)/cavium_common.h 			$(CAVIUM_CARD_DST)/cavium_common.h
	ln -s -f $(CAVIUM_CARD_SRC)/cavium_endian.h 			$(CAVIUM_CARD_DST)/cavium_endian.h
	ln -s -f $(CAVIUM_CARD_SRC)/cavium_ioctl.h 			$(CAVIUM_CARD_DST)/cavium_ioctl.h
	ln -s -f $(CAVIUM_CARD_SRC)/cavium_kernel_common.c 			$(CAVIUM_CARD_DST)/cavium_kernel_common.c
	ln -s -f $(CAVIUM_CARD_SRC)/cavium_kernel_common.h 			$(CAVIUM_CARD_DST)/cavium_kernel_common.h
	ln -s -f $(CAVIUM_CARD_SRC)/cavium_le.h 			$(CAVIUM_CARD_DST)/cavium_le.h
	ln -s -f $(CAVIUM_CARD_SRC)/cavium_list.h 			$(CAVIUM_CARD_DST)/cavium_list.h
	ln -s -f $(CAVIUM_CARD_SRC)/cavium_proc.c 			$(CAVIUM_CARD_DST)/cavium_proc.c
	ln -s -f $(CAVIUM_CARD_SRC)/cavium_proc.h 			$(CAVIUM_CARD_DST)/cavium_proc.h
	ln -s -f $(CAVIUM_CARD_SRC)/cavium_random.c 			$(CAVIUM_CARD_DST)/cavium_random.c
	ln -s -f $(CAVIUM_CARD_SRC)/cavium_random.h 			$(CAVIUM_CARD_DST)/cavium_random.h
	ln -s -f $(CAVIUM_CARD_SRC)/cavium_ssl.h 			$(CAVIUM_CARD_DST)/cavium_ssl.h
	ln -s -f $(CAVIUM_CARD_SRC)/cavium_sysdep.h 			$(CAVIUM_CARD_DST)/cavium_sysdep.h
	ln -s -f $(CAVIUM_CARD_SRC)/command_que.c 			$(CAVIUM_CARD_DST)/command_que.c
	ln -s -f $(CAVIUM_CARD_SRC)/command_que.h 			$(CAVIUM_CARD_DST)/command_que.h
	ln -s -f $(CAVIUM_CARD_SRC)/completion_dma_free_list.c 			$(CAVIUM_CARD_DST)/completion_dma_free_list.c
	ln -s -f $(CAVIUM_CARD_SRC)/completion_dma_free_list.h 			$(CAVIUM_CARD_DST)/completion_dma_free_list.h
	ln -s -f $(CAVIUM_CARD_SRC)/context_memory.c 			$(CAVIUM_CARD_DST)/context_memory.c
	ln -s -f $(CAVIUM_CARD_SRC)/context_memory.h 			$(CAVIUM_CARD_DST)/context_memory.h
	ln -s -f $(CAVIUM_CARD_SRC)/custom_ioctl.h 			$(CAVIUM_CARD_DST)/custom_ioctl.h
	ln -s -f $(CAVIUM_CARD_SRC)/direct_free_list.c 			$(CAVIUM_CARD_DST)/direct_free_list.c
	ln -s -f $(CAVIUM_CARD_SRC)/direct_free_list.h 			$(CAVIUM_CARD_DST)/direct_free_list.h
	ln -s -f $(CAVIUM_CARD_SRC)/error_handler.c 			$(CAVIUM_CARD_DST)/error_handler.c
	ln -s -f $(CAVIUM_CARD_SRC)/error_handler.h 			$(CAVIUM_CARD_DST)/error_handler.h
	ln -s -f $(CAVIUM_CARD_SRC)/hw_lib.c 			$(CAVIUM_CARD_DST)/hw_lib.c
	ln -s -f $(CAVIUM_CARD_SRC)/hw_lib.h 			$(CAVIUM_CARD_DST)/hw_lib.h
	ln -s -f $(CAVIUM_CARD_SRC)/init_cfg.c 			$(CAVIUM_CARD_DST)/init_cfg.c
	ln -s -f $(CAVIUM_CARD_SRC)/init_cfg.h 			$(CAVIUM_CARD_DST)/init_cfg.h
	ln -s -f $(CAVIUM_CARD_SRC)/interrupt.c 			$(CAVIUM_CARD_DST)/interrupt.c
	ln -s -f $(CAVIUM_CARD_SRC)/interrupt.h 			$(CAVIUM_CARD_DST)/interrupt.h
	ln -s -f $(CAVIUM_CARD_SRC)/kernel_ssl_api.c 			$(CAVIUM_CARD_DST)/kernel_ssl_api.c
	ln -s -f $(CAVIUM_CARD_SRC)/key_memory.c 			$(CAVIUM_CARD_DST)/key_memory.c
	ln -s -f $(CAVIUM_CARD_SRC)/key_memory.h 			$(CAVIUM_CARD_DST)/key_memory.h
	ln -s -f $(CAVIUM_CARD_SRC)/linux_ioctl.h 			$(CAVIUM_CARD_DST)/linux_ioctl.h
	ln -s -f $(CAVIUM_CARD_SRC)/linux_main.c 			$(CAVIUM_CARD_DST)/linux_main.c
	ln -s -f $(CAVIUM_CARD_SRC)/linux_main.h 			$(CAVIUM_CARD_DST)/linux_main.h
	ln -s -f $(CAVIUM_CARD_SRC)/linux_sysdep.h 			$(CAVIUM_CARD_DST)/linux_sysdep.h
	ln -s -f $(CAVIUM_CARD_SRC)/microcode.h 			$(CAVIUM_CARD_DST)/microcode.h
	ln -s -f $(CAVIUM_CARD_SRC)/pending_free_list.c 			$(CAVIUM_CARD_DST)/pending_free_list.c
	ln -s -f $(CAVIUM_CARD_SRC)/pending_free_list.h 			$(CAVIUM_CARD_DST)/pending_free_list.h
	ln -s -f $(CAVIUM_CARD_SRC)/pending_list.c 			$(CAVIUM_CARD_DST)/pending_list.c
	ln -s -f $(CAVIUM_CARD_SRC)/pending_list.h 			$(CAVIUM_CARD_DST)/pending_list.h
	ln -s -f $(CAVIUM_CARD_SRC)/poll_thread.c 			$(CAVIUM_CARD_DST)/poll_thread.c
	ln -s -f $(CAVIUM_CARD_SRC)/poll_thread.h 			$(CAVIUM_CARD_DST)/poll_thread.h
	ln -s -f $(CAVIUM_CARD_SRC)/aos_poll_timer.c 			$(CAVIUM_CARD_DST)/aos_poll_timer.c
	ln -s -f $(CAVIUM_CARD_SRC)/aos_poll_timer.h 			$(CAVIUM_CARD_DST)/aos_poll_timer.h
	ln -s -f $(CAVIUM_CARD_SRC)/request_manager.c 			$(CAVIUM_CARD_DST)/request_manager.c
	ln -s -f $(CAVIUM_CARD_SRC)/request_manager.h 			$(CAVIUM_CARD_DST)/request_manager.h
	ln -s -f $(CAVIUM_CARD_SRC)/sg_dma_free_list.c 			$(CAVIUM_CARD_DST)/sg_dma_free_list.c
	ln -s -f $(CAVIUM_CARD_SRC)/sg_dma_free_list.h 			$(CAVIUM_CARD_DST)/sg_dma_free_list.h
	ln -s -f $(CAVIUM_CARD_SRC)/sg_free_list.c 			$(CAVIUM_CARD_DST)/sg_free_list.c
	ln -s -f $(CAVIUM_CARD_SRC)/sg_free_list.h 			$(CAVIUM_CARD_DST)/sg_free_list.h
	ln -s -f $(CAVIUM_CARD_SRC)/soft_req_queue.c 			$(CAVIUM_CARD_DST)/soft_req_queue.c
	ln -s -f $(CAVIUM_CARD_SRC)/soft_req_queue.h 			$(CAVIUM_CARD_DST)/soft_req_queue.h
	ln -s -f $(CAVIUM_CARD_SRC)/aos_request.c 				$(CAVIUM_CARD_DST)/aos_request.c
	ln -s -f $(CAVIUM_CARD_SRC)/aos_request.h 				$(CAVIUM_CARD_DST)/aos_request.h

WEB_WALL_SRC = $(AOSSRC)/WebWall
WEB_WALL_DST = $(LINUXAOS)/WebWall
CREATE_WEB_WALL_DIR = (if [ -d $(WEB_WALL_DST) ]; then echo ""; else mkdir $(WEB_WALL_DST); fi)
installWebWall:
	$(CREATE_WEB_WALL_DIR)
	rm -f $(WEB_WALL_DST)/*
	ln -s -f $(WEB_WALL_SRC)/Makefile 				$(WEB_WALL_DST)/Makefile
	ln -s -f $(WEB_WALL_SRC)/WebWall.c 				$(WEB_WALL_DST)/WebWall.c
	ln -s -f $(WEB_WALL_SRC)/WebWallCli.c 			$(WEB_WALL_DST)/WebWallCli.c
	
	
QOS_SRC = $(AOSSRC)/QoS
QOS_DST = $(LINUXAOS)/QoS
CREATE_QOS_DIR = (if [ -e $(QOS_DST) ]; then rm -rf $(QOS_DST); fi; mkdir $(QOS_DST) )
installQoS:
	$(CREATE_QOS_DIR)
	ln -sf $(QOS_SRC)/kernel-Makefile	            $(QOS_DST)/Makefile
	#ln -sf $(QOS_SRC)/qos_module.c 		            $(QOS_DST)/qos_module.c
	ln -sf $(QOS_SRC)/qos_traffic.c 	            $(QOS_DST)/qos_traffic.c
	ln -sf $(QOS_SRC)/qos_traffic_cli.c 	        $(QOS_DST)/qos_traffic_cli.c
	#ln -sf $(QOS_SRC)/qos_traffic_rule.c 	        $(QOS_DST)/qos_traffic_rule.c
	#ln -sf $(QOS_SRC)/qos_traffic_channel_record.c $(QOS_DST)/qos_traffic_channel_record.c
	ln -sf $(QOS_SRC)/qos_global.c 	                $(QOS_DST)/qos_global.c
	ln -sf $(QOS_SRC)/qos_rule_cli.c                $(QOS_DST)/qos_rule_cli.c
	ln -sf $(QOS_SRC)/qos_rule.c                	$(QOS_DST)/qos_rule.c
	ln -sf $(QOS_SRC)/hashtab.c                		$(QOS_DST)/hashtab.c
	#ln -sf $(QOS_SRC)/qos_test.c                	$(QOS_DST)/qos_test.c
	ln -sf $(QOS_SRC)/qos_main.c                	$(QOS_DST)/qos_main.c
	ln -sf $(QOS_SRC)/qos_nf.c                		$(QOS_DST)/qos_nf.c
	ln -sf $(QOS_SRC)/qos_bw_man.c                	$(QOS_DST)/qos_bw_man.c
	ln -sf $(QOS_SRC)/qos_config.c                	$(QOS_DST)/qos_config.c
	ln -sf $(QOS_SRC)/qos_shaper.c                	$(QOS_DST)/qos_shaper.c
	#ln -sf $(QOS_SRC)/qos_test.c                	$(QOS_DST)/qos_test.c
	ln -sf $(QOS_SRC)/qos_proc.c                	$(QOS_DST)/qos_proc.c
	ln -sf $(QOS_SRC)/qos_cli_check_parms.c         $(QOS_DST)/qos_cli_check_parms.c
	ln -sf $(QOS_SRC)/qos_logstat.c                 $(QOS_DST)/qos_logstat.c

USER_IDENTIFY_SRC = $(AOSSRC)/UserIdentify
USER_IDENTIFY_DST = $(LINUXAOS)/UserIdentify
CREATE_USER_IDENTIFY_DIR = (if [ -e $(USER_IDENTIFY_DST) ]; then rm -rf $(USER_IDENTIFY_DST); fi; mkdir $(USER_IDENTIFY_DST))
installUserIdentify:
	$(CREATE_USER_IDENTIFY_DIR)
	ln -sf $(USER_IDENTIFY_SRC)/kernel-Makefile			$(USER_IDENTIFY_DST)/Makefile
	ln -sf $(USER_IDENTIFY_SRC)/user_identify.c			$(USER_IDENTIFY_DST)/user_identify.c
	ln -sf $(USER_IDENTIFY_SRC)/user_identify.h			$(USER_IDENTIFY_DST)/user_identify.h
	ln -sf $(USER_IDENTIFY_SRC)/user_identify_cli.c		$(USER_IDENTIFY_DST)/user_identify_cli.c
	ln -sf $(USER_IDENTIFY_SRC)/user_identify_cli.h		$(USER_IDENTIFY_DST)/user_identify_cli.h
	ln -sf $(USER_IDENTIFY_SRC)/user_identify_module.c	$(USER_IDENTIFY_DST)/user_identify_module.c

INETCTRL_SRC = $(AOSSRC)/InetCtrl
INETCTRL_DST = $(LINUXAOS)/inet_ctrl
CREATE_INETCTRL_DIR = (if [ -e $(INETCTRL_DST) ]; then rm -rf $(INETCTRL_DST); fi; mkdir $(INETCTRL_DST))
installInetCtrl:
	$(CREATE_INETCTRL_DIR)
	ln -sf $(INETCTRL_SRC)/kernel-Makefile			$(INETCTRL_DST)/Makefile
	ln -sf $(INETCTRL_SRC)/inet_ctrl.c				$(INETCTRL_DST)/inet_ctrl.c
	ln -sf $(INETCTRL_SRC)/inet_ctrl.h				$(INETCTRL_DST)/inet_ctrl.h
	ln -sf $(INETCTRL_SRC)/inet_ctrl_cli.c			$(INETCTRL_DST)/inet_ctrl_cli.c
	ln -sf $(INETCTRL_SRC)/inet_ctrl_cli.h			$(INETCTRL_DST)/inet_ctrl_cli.h
	ln -sf $(INETCTRL_SRC)/inet_ctrl_hash.c			$(INETCTRL_DST)/inet_ctrl_hash.c
	ln -sf $(INETCTRL_SRC)/inet_ctrl_hash.h			$(INETCTRL_DST)/inet_ctrl_hash.h
	ln -sf $(INETCTRL_SRC)/inet_ctrl_printf.h		$(INETCTRL_DST)/inet_ctrl_printf.h
	ln -sf $(INETCTRL_SRC)/inet_ctrl_printf.c		$(INETCTRL_DST)/inet_ctrl_printf.c
	ln -sf $(INETCTRL_SRC)/inet_ctrl_timer.h		$(INETCTRL_DST)/inet_ctrl_timer.h
	ln -sf $(INETCTRL_SRC)/inet_ctrl_timer.c		$(INETCTRL_DST)/inet_ctrl_timer.c
	ln -sf $(INETCTRL_SRC)/inet_ctrl_file.c			$(INETCTRL_DST)/inet_ctrl_file.c
	ln -sf $(INETCTRL_SRC)/inet_ctrl_file.h			$(INETCTRL_DST)/inet_ctrl_file.h
	ln -sf $(INETCTRL_SRC)/inet_ctrl_core.c			$(INETCTRL_DST)/inet_ctrl_core.c

KERNEL_WRAPPER = $(AOSSRC)/KernelAPI/KernelWrapper
KNLMAKEFILES_DIR = $(AOSSRC)/KernelMakefiles
AOSTEST = $(LINUXAOS)/AosTest
AOSUTILTEST = $(AOSSRC)/aosUtil/Tester/KernelFiles
CREATE_AOSTEST = $(shell if [ -d $(AOSTEST) ]; then echo ""; else mkdir $(AOSTEST); fi)
installTester:
	$(CREATE_AOSTEST)
	rm -f $(AOSTEST)/*
	ln -s -f $(KNLMAKEFILES_DIR)/Makefile-Test 	$(AOSTEST)/Makefile
	ln -s -f $(KERNEL_WRAPPER)/MutexTesterKnl.c $(AOSTEST)/MutexTesterKnl.c
	ln -s -f $(KERNEL_WRAPPER)/RwlockTesterKnl.c $(AOSTEST)/RwlockTesterKnl.c
	ln -s -f $(CERT_VERIFY_SRC)/Tester/OcspTestDriver.c $(AOSTEST)/OcspTestDriver.c
	ln -s -f $(CERT_VERIFY_SRC)/Tester/OcspTesterKernelApi.c $(AOSTEST)/OcspTesterKernelApi.c
	ln -s -f $(AOSUTIL)/Memory.cpp 		$(AOSTEST)/Memory.c
	ln -s -f $(AOSUTIL)/HashBinRaw.cpp 	$(AOSTEST)/HashBinRaw.c
	ln -s -f $(AOSUTIL)/SlabRaw.cpp 	$(AOSTEST)/SlabRaw.c
	ln -s -f $(KERNEL_WRAPPER)/LogSetLinesKernelApi.c $(AOSTEST)/LogSetLinesKernelApi.c

create_dir:
	mkdir $(LINUXAOS) || true

installBasic: \
	check \
	create_dir \
	installAosCore \
	installKernelUtil \
	installKernelAPI \
	installAosUtil \
	installServers \
	installTester \
	installAAA \
	installCertVerify \
	installAppProc \
	installKtcpvs \
	installSsl \
	installEncryptCard	\
	installCaviumCard \
	installProxy \
	installQoS \
	installSecure \
	installTraffic \
	installUserIdentify \
	installWebWall \
	installInetCtrl
		
createDir:
	$(CREATE_AOSCORE)

install2_6_7: \
	installBasic \
	KernelHooking_2_6_7 \
	install2_6_7Makefiles

installMystro2_6_7: install2_6_7
	cp -f  $(MAKEFILE_DIR)/dotconfig-2.6.7-mystro 			$(LINUXHOME)/.config

install2_6_15_5: \
	installBasic \
	binlog \
	installStarvation \
	installPanicNotifier \
	installRosOS \
	install2_6_15_5Makefiles \
	KernelHooking_2_6_15_5

installMystro: install2_6_15_5 
	cp -f  $(MAKEFILE_DIR)/dotconfig-2.6.15.5-mystro 			$(LINUXHOME)/.config

installMystro_jeff: install2_6_15_5 
	cp -f  $(MAKEFILE_DIR)/dotconfig-2.6.15.5-mystro-jeff 		$(LINUXHOME)/.config

installMystro_zzh: install2_6_15_5 
	cp -f  $(MAKEFILE_DIR)/dotconfig-2.6.15.5-mystro-zzh 			$(LINUXHOME)/.config

installKeyou: install2_6_15_5 
	cp -f  $(MAKEFILE_DIR)/dotconfig-2.6.15.5-keyou-zql 			$(LINUXHOME)/.config
	
installMystro2_6_15_5-ytao: install2_6_15_5 
	cp -f  $(MAKEFILE_DIR)/dotconfig-2.6.15.5-mystro-ytao 		$(LINUXHOME)/.config

installMystroModule: install2_6_15_5 
	cp -f  $(MAKEFILE_DIR)/dotconfig-2.6.15.5-mystro-module 	$(LINUXHOME)/.config

installMystro_wangping: install2_6_15_5 
	cp -f  $(MAKEFILE_DIR)/dotconfig-2.6.15.5-mystro-wangping 	$(LINUXHOME)/.config
installMystro_bill: install2_6_15_5
	cp -f  $(MAKEFILE_DIR)/dotconfig-2.6.15.5-mystro-bill		$(LINUXHOME)/.config
installMystro_wangping_kgdb: install2_6_15_5 
	cp -f  $(MAKEFILE_DIR)/dotconfig-2.6.15.5-mystro-wangping-kgdb 	$(LINUXHOME)/.config
	
installRhc: install2_6_15_5 
	cp -f  $(MAKEFILE_DIR)/dotconfig-2.6.15.5-rhc 		$(LINUXHOME)/.config

install: 
	@for i in $(INSTARGET) ;do cp -rf $$i $(INSDIR)/.; done
