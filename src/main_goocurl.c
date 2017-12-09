
#include <stdio.h>
#include "goocurl.h"

int main(int argc, char * argv[])
{

int status; // var to catch return value

// init goocurl
status=goocurl_init();
if(status!=0) { printf("main - fail to init goocurl - error: %d\n", status); return -1; }

// request map
status=goocurl_request_static_map(40.714728, -73.998672, 20, 320, 320,
	KTypeSatellite, KFormatDefault, 0, "data/test.png");
if(status!=0) { printf("main - fail to request static map - error: %d\n", status); return -2; }

// close goocurl
goocurl_close();

return 0;
}

