
#ifndef __BMKGWEATHERPARSER_H__
#define __BMKGWEATHERPARSER_H__

#include <libxml/tree.h>
#include <libxml/parser.h>

typedef struct
{
	char iKota[32];
	char iCuaca[32];
	char iSuhuMin;
	char iSuhuMax;
	char iKelembapanMin;
	char iKelembapanMax;
} SBmkgCity;

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

extern xmlDoc * bmkgweatherparser_open(const char * aFileName);

extern int bmkgweatherparser_query_valid_date(xmlDoc * aXml, char * aMulai, char * aSampai);

extern int bmkgweatherparser_query_cities(xmlDoc * aXml, SBmkgCity * aCityList);

extern void bmkgweatherparser_close(xmlDoc ** aXml);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __BMKGWEATHERPARSER_H__

