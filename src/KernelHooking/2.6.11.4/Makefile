SRCDIR = $(AOSHOME)/src
HOOKDIR = $(SRCDIR)/KernelHooking/2.6.11.4
KERNELHOME = /usr/src/linux

install:
	ln -s -f $(HOOKDIR)/tcp_ipv4.c $(KERNELHOME)/net/ipv4/tcp_ipv4.c
	ln -s -f $(HOOKDIR)/tcp_minisocks.c $(KERNELHOME)/net/ipv4/tcp_minisocks.c
	ln -s -f $(HOOKDIR)/asm-i386-socket.h $(KERNELHOME)/include/asm/socket.h
	ln -s -f $(HOOKDIR)/sock.h $(KERNELHOME)/include/net/sock.h
	ln -s -f $(HOOKDIR)/syncookies.c $(KERNELHOME)/net/ipv4/syncookies.c
	ln -s -f $(HOOKDIR)/tcp_input.c $(KERNELHOME)/net/ipv4/tcp_input.c
	ln -s -f $(HOOKDIR)/sock.c $(KERNELHOME)/net/core/sock.c
	ln -s -f $(HOOKDIR)/dev.c $(KERNELHOME)/net/core/dev.c
	ln -s -f $(HOOKDIR)/arp.c $(KERNELHOME)/net/ipv4/arp.c

	ln -s -f $(SRCDIR)/aos_core/TcpSockBridge.cpp $(KERNELHOME)/net/core/TcpSockBridge.c
	ln -s -f $(SRCDIR)/KernelUtil/aosKernelAlarm.cpp $(KERNELHOME)/net/core/aosKernelAlarm.c
	ln -s -f $(SRCDIR)/aos_core/aosBridge.cpp $(KERNELHOME)/net/core/aosBridge.c
	ln -s -f $(SRCDIR)/aos_core/aosBuiltinRules.cpp $(KERNELHOME)/net/core/aosBuiltinRules.c
	ln -s -f $(SRCDIR)/aos_core/aosConfig.cpp $(KERNELHOME)/net/core/aosConfig.c
	ln -s -f $(SRCDIR)/aos_core/aosCoreComm.cpp $(KERNELHOME)/net/core/aosCoreComm.c
	ln -s -f $(SRCDIR)/aos_core/aosCorePcp.cpp $(KERNELHOME)/net/core/aosCorePcp.c
	ln -s -f $(SRCDIR)/aos_core/aosDev.cpp $(KERNELHOME)/net/core/aosDev.c
	ln -s -f $(SRCDIR)/aos_core/aosFwdDev.cpp $(KERNELHOME)/net/core/aosFwdDev.c
	ln -s -f $(SRCDIR)/aos_core/aosFwder.cpp $(KERNELHOME)/net/core/aosFwder.c
	ln -s -f $(SRCDIR)/aos_core/aosKernelApi.cpp $(KERNELHOME)/net/core/aosKernelApi.c
	ln -s -f $(SRCDIR)/aos_core/aosMacCtlr.cpp $(KERNELHOME)/net/core/aosMacCtlr.c
	ln -s -f $(SRCDIR)/aos_core/aosMacRule.cpp $(KERNELHOME)/net/core/aosMacRule.c
	ln -s -f $(SRCDIR)/aos_core/aosMultiCaster.cpp $(KERNELHOME)/net/core/aosMultiCaster.c
	ln -s -f $(SRCDIR)/aos_core/aosPktStat.cpp $(KERNELHOME)/net/core/aosPktStat.c
	ln -s -f $(SRCDIR)/aos_core/aosRule.cpp $(KERNELHOME)/net/core/aosRule.c
	ln -s -f $(SRCDIR)/aos_core/aosRuleHeadSwapper.cpp $(KERNELHOME)/net/core/aosRuleHeadSwapper.c
	ln -s -f $(SRCDIR)/aos_core/aosRuleNetInput.cpp $(KERNELHOME)/net/core/aosRuleNetInput.c
	ln -s -f $(SRCDIR)/aos_core/aosSkbRecv.cpp $(KERNELHOME)/net/core/aosSkbRecv.c
	ln -s -f $(SRCDIR)/aos_core/aosSockBridge.cpp $(KERNELHOME)/net/core/aosSockBridge.c
	ln -s -f $(SRCDIR)/aos_core/aosSock.cpp $(KERNELHOME)/net/core/aosSock.c
	ln -s -f $(SRCDIR)/aos_core/aosSocket.cpp $(KERNELHOME)/net/core/aosSocket.c
	ln -s -f $(SRCDIR)/aos_core/aosSockSSL.cpp $(KERNELHOME)/net/core/aosSockSSL.c
	ln -s -f $(SRCDIR)/aos_core/aosTimer.cpp $(KERNELHOME)/net/core/aosTimer.c
	ln -s -f $(SRCDIR)/aos_core/TcpSockBridge.cpp $(KERNELHOME)/net/core/TcpSockBridge.c
	ln -s -f $(SRCDIR)/aos_core/SockFirewall.cpp $(KERNELHOME)/net/core/SockFirewall.c
	ln -s -f $(SRCDIR)/aos_ipv4/aosArp.cpp $(KERNELHOME)/net/ipv4/aosArp.c
	ln -s -f $(SRCDIR)/AppProxy/AppProxy.cpp $(KERNELHOME)/net/core/AppProxy.c
	ln -s -f $(SRCDIR)/KernelUtil/CharPtree.cpp $(KERNELHOME)/net/core/CharPtree.c
	ln -s -f $(SRCDIR)/KernelUtil/KernelLock.cpp $(KERNELHOME)/net/core/KernelLock.c
	ln -s -f $(SRCDIR)/KernelUtil/KernelStr.cpp $(KERNELHOME)/net/core/KernelStr.c
	ln -s -f $(SRCDIR)/KernelUtil/Slab.cpp $(KERNELHOME)/net/core/Slab.c
	ln -s -f $(SRCDIR)/KernelUtil/TArray.cpp $(KERNELHOME)/net/core/TArray.c
	ln -s -f $(SRCDIR)/Servers/AppServer.cpp $(KERNELHOME)/net/core/AppServer.c
	ln -s -f $(SRCDIR)/Servers/ServerGroup.cpp $(KERNELHOME)/net/core/ServerGroup.c
	ln -s -f $(SRCDIR)/AAA/AaaServer.cpp $(KERNELHOME)/net/core/AaaServer.c


