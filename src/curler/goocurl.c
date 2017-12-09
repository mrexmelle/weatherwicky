
#include <string.h>
#include <curl/curl.h>
#include "goocurl.h"

static pthread_mutex_t gMutex;
static CURL * gCurl=NULL;

#define KGoogleStaticMapLatLonUrl	\
	"http://maps.googleapis.com/maps/api/staticmap?center=%lf,%lf&zoom=%d&size=%dx%d&maptype=%s&format=%s&sensor=%s"

#define KGoogleStaticMapStrUrl	\
	"http://maps.googleapis.com/maps/api/staticmap?center=%s&zoom=%d&size=%dx%d&maptype=%s&format=%s&sensor=%s"

#define	KGooMarkerFormatStr	\
	"markers=size:%s|color:0x%.8X|label:%c|%s"

#define	KGooMarkerFormatLatLon	\
	"markers=size:%s|color:0x%.8X|label:%c|%lf,%lf"

#define KBmkgIndoWeatherUrl	\
	"http://data.bmkg.go.id/cuaca_indo_1.xml"

/*
size_t goocurl_writedata_callback(void * aBuffer, size_t aSize, size_t aNmemb, void * aFp)
{
	fwrite(aBuffer, aSize, aNmemb, aFp);
}
*/

int goocurl_init()
{
	// init curl
	gCurl=curl_easy_init();
	if(gCurl==NULL) { return -1; }

	// init mutex
	pthread_mutex_init(&gMutex, NULL);

	return 0;
}

int goocurl_request_static_map_by_lat_lon(double aLat, double aLon, char aZoom, short aWidth, short aHeight,
	TMapType aMapType, TFileFormat aFormat, char aSensor, const char * aMarkerStr, const char * aOutput)
{
	int backIdx;
	FILE * fp=NULL;
	char url[1024];
	char mapType[10];
	char format[13];
	CURLcode res;

	// trap error
	if(gCurl==NULL || aOutput==NULL) { return -1; }

	// validate zoom
	if(aZoom < 0 || aZoom > 22) { return -2; }

	// get mapType
	memset(mapType, 0x0, 10);
	if(aMapType==KTypeRoadmap) { strcpy(mapType, "roadmap"); }
	else if(aMapType==KTypeSatellite) { strcpy(mapType, "satellite"); }
	else if(aMapType==KTypeTerrain) { strcpy(mapType, "terrain"); }
	else if(aMapType==KTypeHybrid) { strcpy(mapType, "hybrid"); }
	else { return -3; } // parameter aMapType mismatch

	// get format
	memset(format, 0x0, 13);
	if(aFormat==KFormatPng8) { strcpy(format, "png8"); }
	else if(aFormat==KFormatPng32) { strcpy(format, "png32"); }
	else if(aFormat==KFormatGif) { strcpy(format, "gif"); }
	else if(aFormat==KFormatJpg) { strcpy(format, "jpg"); }
	else if(aFormat==KFormatJpgBaseline) { strcpy(format, "jpg-baseline"); }
	else { return -4; } // parameter aFormat mismatch

	// get url
	memset(url, 0x0, 1024);
	sprintf(url, KGoogleStaticMapLatLonUrl,
		aLat, aLon, (int)(aZoom), aWidth, aHeight, mapType, format, aSensor ? "true" : "false");

	// add marker if available
	if(aMarkerStr)
	{
		strcat(url, "&");
		strcat(url, aMarkerStr);
	}

//	printf("url: %s\n", url);

	// find path
	for(backIdx=strlen(aOutput)-1; backIdx>=0; backIdx--)
	{
		if(aOutput[backIdx]=='/') { break; }
	}

	// start locking process
	pthread_mutex_lock(&gMutex);

	// ensure dest dir is exist
	if(backIdx>=0)
	{
		char cmd[255];

		memset(cmd, 0x0, 255);
		strcpy(cmd, "mkdir -p ");
		strncpy(cmd+9, aOutput, backIdx+1);
		strcat(cmd, " > /dev/null 2>&1");
		system(cmd);
	}

	// open file
	fp=fopen(aOutput, "w");
	if(fp==NULL)
	{
		// unlock process
		pthread_mutex_unlock(&gMutex);
		return -5;
	}

	// set file pointer
	curl_easy_setopt(gCurl, CURLOPT_WRITEDATA, fp);

	// set file writer function
	curl_easy_setopt(gCurl, CURLOPT_WRITEFUNCTION, fwrite);

	// set url
	curl_easy_setopt(gCurl, CURLOPT_URL, url);

	// execute request
	res=curl_easy_perform(gCurl);

	// close file
	fclose(fp);

	// unlock process
	pthread_mutex_unlock(&gMutex);

	return 0;
}

int goocurl_request_static_map_by_string(const char * aStr, char aZoom, short aWidth, short aHeight,
	TMapType aMapType, TFileFormat aFormat, char aSensor, const char * aMarkerStr, const char * aOutput)
{
	int backIdx;
	FILE * fp=NULL;
	char url[1024];
	char mapType[10];
	char format[13];
	CURLcode res;

	// trap error
	if(gCurl==NULL || aOutput==NULL) { return -1; }

	// validate zoom
	if(aZoom < 0 || aZoom > 22) { return -2; }

	// get mapType
	memset(mapType, 0x0, 10);
	if(aMapType==KTypeRoadmap) { strcpy(mapType, "roadmap"); }
	else if(aMapType==KTypeSatellite) { strcpy(mapType, "satellite"); }
	else if(aMapType==KTypeTerrain) { strcpy(mapType, "terrain"); }
	else if(aMapType==KTypeHybrid) { strcpy(mapType, "hybrid"); }
	else { return -3; } // parameter aMapType mismatch

	// get format
	memset(format, 0x0, 13);
	if(aFormat==KFormatPng8) { strcpy(format, "png8"); }
	else if(aFormat==KFormatPng32) { strcpy(format, "png32"); }
	else if(aFormat==KFormatGif) { strcpy(format, "gif"); }
	else if(aFormat==KFormatJpg) { strcpy(format, "jpg"); }
	else if(aFormat==KFormatJpgBaseline) { strcpy(format, "jpg-baseline"); }
	else { return -4; } // parameter aFormat mismatch

	// get url
	memset(url, 0x0, 1024);
	sprintf(url, KGoogleStaticMapStrUrl,
		aStr, (int)(aZoom), aWidth, aHeight, mapType, format, aSensor ? "true" : "false");

	// add marker if available
	if(aMarkerStr)
	{
		strcat(url, "&");
		strcat(url, aMarkerStr);
	}

	// find path
	for(backIdx=strlen(aOutput)-1; backIdx>=0; backIdx--)
	{
		if(aOutput[backIdx]=='/') { break; }
	}

	// start locking process
	pthread_mutex_lock(&gMutex);

	// ensure dest dir is exist
	if(backIdx>=0)
	{
		char cmd[255];

		memset(cmd, 0x0, 255);
		strcpy(cmd, "mkdir -p ");
		strncpy(cmd+9, aOutput, backIdx+1);
		strcat(cmd, " > /dev/null 2>&1");
		system(cmd);
	}

	// open file
	fp=fopen(aOutput, "w");
	if(fp==NULL)
	{
		// unlock process
		pthread_mutex_unlock(&gMutex);
		return -5;
	}

	// set file pointer
	curl_easy_setopt(gCurl, CURLOPT_WRITEDATA, fp);

	// set file writer function
	curl_easy_setopt(gCurl, CURLOPT_WRITEFUNCTION, fwrite);

	// set url
	curl_easy_setopt(gCurl, CURLOPT_URL, url);

	// execute request
	res=curl_easy_perform(gCurl);

	// close file
	fclose(fp);

	// unlock process
	pthread_mutex_unlock(&gMutex);

	return 0;
}

int goocurl_get_marker_descriptor_by_lat_lon(double aLat, double aLon, char aSize, unsigned int aColor, char aLabel,
	char * aOutput, size_t aOutputSize)
{
	char size[6];

	// get size
	memset(size, 0x0, 6);
	if(aSize==KMarkerSizeTiny) { strcpy(size, "tiny"); }
	else if(aSize==KMarkerSizeMid) { strcpy(size, "mid"); }
	else if(aSize==KMarkerSizeSmall) { strcpy(size, "small"); }
	else { return -1; } // parameter aSize mismatch	

	// set descriptor
	memset(aOutput, 0x0, aOutputSize);
	sprintf(aOutput, KGooMarkerFormatLatLon,
		size, aColor, aLabel, aLat, aLon);

	return 0;
}

int goocurl_get_marker_descriptor_by_string(const char * aStr, char aSize, unsigned int aColor, char aLabel,
	char * aOutput, size_t aOutputSize)
{
	char size[6];

	// get size
	memset(size, 0x0, 6);
	if(aSize==KMarkerSizeTiny) { strcpy(size, "tiny"); }
	else if(aSize==KMarkerSizeMid) { strcpy(size, "mid"); }
	else if(aSize==KMarkerSizeSmall) { strcpy(size, "small"); }
	else { return -1; } // parameter aSize mismatch	

	// set descriptor
	memset(aOutput, 0x0, aOutputSize);
	sprintf(aOutput, KGooMarkerFormatStr,
		size, aColor, aLabel, aStr);

	return 0;
}

int bmkgcurl_get_indonesia_weather(const char * aOutput)
{
	int backIdx;
	FILE * fp=NULL;
	CURLcode res;

	// trap error
	if(gCurl==NULL || aOutput==NULL) { return -1; }

	// find path
	for(backIdx=strlen(aOutput)-1; backIdx>=0; backIdx--)
	{
		if(aOutput[backIdx]=='/') { break; }
	}

	// start locking process
	pthread_mutex_lock(&gMutex);

	// ensure dest dir is exist
	if(backIdx>=0)
	{
		char cmd[255];

		memset(cmd, 0x0, 255);
		strcpy(cmd, "mkdir -p ");
		strncpy(cmd+9, aOutput, backIdx+1);
		strcat(cmd, " > /dev/null 2>&1");
		system(cmd);
	}

	// open file
	fp=fopen(aOutput, "w");
	if(fp==NULL)
	{
		// unlock process
		pthread_mutex_unlock(&gMutex);
		return -5;
	}

	// set file pointer
	curl_easy_setopt(gCurl, CURLOPT_WRITEDATA, fp);

	// set file writer function
	curl_easy_setopt(gCurl, CURLOPT_WRITEFUNCTION, fwrite);

	// set url
	curl_easy_setopt(gCurl, CURLOPT_URL, KBmkgIndoWeatherUrl);

	// execute request
	res=curl_easy_perform(gCurl);

	// close file
	fclose(fp);

	// unlock process
	pthread_mutex_unlock(&gMutex);

	return 0;
}

void goocurl_close()
{
	// cleanup curl
	curl_easy_cleanup(gCurl);
	gCurl=NULL;

	// destroy mutex
	pthread_mutex_destroy(&gMutex);
}

