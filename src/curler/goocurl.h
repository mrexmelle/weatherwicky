
#ifndef __GOOCURL_H__
#define __GOOCURL_H__

typedef char		TMapType;
#define	KTypeRoadmap	1
#define	KTypeSatellite	2
#define	KTypeTerrain	3
#define	KTypeHybrid	4
#define	KTypeDefault	KTypeRoadmap

typedef	char			TFileFormat;
#define	KFormatPng8		1
#define	KFormatPng32		2
#define	KFormatGif		3
#define	KFormatJpg		4
#define	KFormatJpgBaseline	5
#define	KFormatDefault		KFormatPng8

typedef char			TMarkerSize;
#define KMarkerSizeTiny		1
#define KMarkerSizeMid		2
#define KMarkerSizeSmall	3

typedef struct
{


	unsigned int iColor;

	char iLabel;

} SGooMarker;

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

extern int goocurl_init();

extern int goocurl_request_static_map_by_lat_lon(double aLat, double aLon, char aZoom, short aWidth, short aHeight,
	TMapType aMapType, TFileFormat aFormat, char aSensor, const char * aMarkerStr, const char * aOutput);

extern int goocurl_request_static_map_by_string(const char * aStr, char aZoom, short aWidth, short aHeight,
	TMapType aMapType, TFileFormat aFormat, char aSensor, const char * aMarkerStr, const char * aOutput);

extern int goocurl_get_marker_descriptor_by_lat_lon(double aLat, double aLon, char aSize, unsigned int aColor, char aLabel,
	char * aOutput, size_t aOutputSize);

extern int goocurl_get_marker_descriptor_by_string(const char * aStr, char aSize, unsigned int aColor, char aLabel,
	char * aOutput, size_t aOutputSize);

extern int bmkgcurl_get_indonesia_weather(const char * aOutput);

extern void goocurl_close();

#ifdef __cplusplus
}
#endif // __cplusplus


#endif // __GOOCURL_H__

