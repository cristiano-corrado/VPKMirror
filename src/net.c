#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include <psp2/io/fcntl.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/net/http.h>
#include <psp2/sysmodule.h>

void netInit() {
	sceSysmoduleLoadModule(SCE_SYSMODULE_NET);

	SceNetInitParam netInitParam;
	int size = 1*1024*1024;
	netInitParam.memory = malloc(size);
	netInitParam.size = size;
	netInitParam.flags = 0;
	sceNetInit(&netInitParam);

	sceNetCtlInit();
}

void netTerm() {
	sceNetCtlTerm();
	sceNetTerm();
	sceSysmoduleUnloadModule(SCE_SYSMODULE_NET);
}

void httpInit() {
	sceSysmoduleLoadModule(SCE_SYSMODULE_HTTP);
	sceHttpInit(1*1024*1024);
}

void httpTerm() {
	sceHttpTerm();
	sceSysmoduleUnloadModule(SCE_SYSMODULE_HTTP);
}

void download(const char *url, const char *dest) {
	// Create template with user agend "PS Vita Sample App"
	int tpl = sceHttpCreateTemplate("VPKMirror", 1, 1);

	// set url on the template
	int conn = sceHttpCreateConnectionWithURL(tpl, url, 0);

	// create the request with the correct method
	int request = sceHttpCreateRequestWithURL(conn, SCE_HTTP_METHOD_GET, url, 0);

	// send the actual request. Second parameter would be POST data, third would be length of it.
	int handle = sceHttpSendRequest(request, NULL, 0);

	// open destination file
	int fh = sceIoOpen(dest, SCE_O_WRONLY | SCE_O_CREAT, 0777);

	// create buffer and counter for read bytes.
	unsigned char data[16*1024];
	int read = 0;

	// read data until finished
	while ((read = sceHttpReadData(request, &data, sizeof(data))) > 0) {
		// writing the count of read bytes from the data buffer to the file
		int write = sceIoWrite(fh, data, read);
	}

	// close file
	sceIoClose(fh);
}