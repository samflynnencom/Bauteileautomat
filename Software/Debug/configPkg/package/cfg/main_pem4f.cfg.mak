# invoke SourceDir generated makefile for main.pem4f
main.pem4f: .libraries,main.pem4f
.libraries,main.pem4f: package/cfg/main_pem4f.xdl
	$(MAKE) -f C:\Users\einball\workspace_v6_0\RTOS_UART/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\einball\workspace_v6_0\RTOS_UART/src/makefile.libs clean

