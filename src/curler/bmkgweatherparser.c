
#include <string.h>
#include "bmkgweatherparser.h"

xmlDoc * bmkgweatherparser_open(const char * aFileName)
{
	return xmlReadFile(aFileName, NULL, 0);
}

int bmkgweatherparser_query_valid_date(xmlDoc * aXml, char * aMulai, char * aSampai)
{
	// error trapper
	if(aXml==NULL) { return -1; }

	char found=0;
	xmlNode * root=xmlDocGetRootElement(aXml);
	while(root)
	{
		if(root->type==XML_ELEMENT_NODE && strcasecmp(root->name, "cuaca")==0)
		{
			// we are now in cuaca tag
			xmlNode * cuaca=root->children;
			while(cuaca)
			{
				if(cuaca->type==XML_ELEMENT_NODE && strcasecmp(cuaca->name, "tanggal")==0)
				{
					// we are now in tanggal tag
					xmlNode * tanggal=cuaca->children;
					while(tanggal)
					{
						if(tanggal->type==XML_ELEMENT_NODE)
						{
							const char * tagName=tanggal->name;
							if(strcasecmp(tagName, "mulai")==0)
							{
								char * tagContent=xmlNodeGetContent(tanggal);
								memset(aMulai, 0x0, strlen(tagContent)+1);
								strcpy(aMulai, tagContent);
								found=found|(1<<4);
							}
							else if(strcasecmp(tagName, "sampai")==0)
							{
								char * tagContent=xmlNodeGetContent(tanggal);
								memset(aSampai, 0x0, strlen(tagContent)+1);
								strcpy(aSampai, tagContent);
								found=found|1;
							}
						}
						if((found & 0xf0) && (found & 0x0f)) { break; }
						tanggal=tanggal->next;
					}
				}
				if((found & 0xf0) && (found & 0x0f)) { break; }
				cuaca=cuaca->next;
			}
		}
		if((found & 0xf0) && (found & 0x0f)) { break; }
		root=root->next;
	}
	
	// if both mulai and sampai are not found
	if(!(found & 0xf0) && !(found & 0x0f)) { return -2; }

	// if unable to find mulai
	if(!(found & 0xf0) && (found & 0x0f)) { return -3; }

	//if unable to find sampai
	if((found & 0xf0) && !(found & 0x0f)) { return -4; }

	return 0;
}

int bmkgweatherparser_query_cities(xmlDoc * aXml, SBmkgCity * aCityList)
{
	// error trapper
	if(aXml==NULL) { return -1; }

	char cityFound=0;
	int cityIndex=0;
	char infoFetched=0;
	xmlNode * root=xmlDocGetRootElement(aXml);
	while(root)
	{
		if(root->type==XML_ELEMENT_NODE && strcasecmp(root->name, "cuaca")==0)
		{
			// we are now in cuaca tag
			xmlNode * cuaca=root->children;
			while(cuaca)
			{
				if(cuaca->type==XML_ELEMENT_NODE && strcasecmp(cuaca->name, "isi")==0)
				{
					// we are now in tanggal tag
					xmlNode * isi=cuaca->children;
					cityIndex=0;
					while(isi)
					{
						infoFetched=0;
						if(isi->type==XML_ELEMENT_NODE && strcasecmp(isi->name, "row")==0)
						{
							xmlNode * row=isi->children;
							while(row)
							{
								if(row->type==XML_ELEMENT_NODE)
								{
									const char * tagName=row->name;
									if(strcasecmp(tagName, "kota")==0)
									{
										memset(aCityList[cityIndex].iKota, 0x0, 32);
										strcpy(aCityList[cityIndex].iKota, xmlNodeGetContent(row));
										infoFetched++;
									}
									else if(strcasecmp(tagName, "cuaca")==0)
									{
										memset(aCityList[cityIndex].iCuaca, 0x0, 32);
										strcpy(aCityList[cityIndex].iCuaca, xmlNodeGetContent(row));
										infoFetched++;
									}
									else if(strcasecmp(tagName, "suhumin")==0)
									{
										char * tagContent=xmlNodeGetContent(row);
										int suhuMin=atoi(tagContent);
										aCityList[cityIndex].iSuhuMin=(char)(suhuMin);
										infoFetched++;
									}
									else if(strcasecmp(tagName, "suhumax")==0)
									{
										char * tagContent=xmlNodeGetContent(row);
										int suhuMax=atoi(tagContent);
										aCityList[cityIndex].iSuhuMax=(char)(suhuMax);
										infoFetched++;
									}
									else if(strcasecmp(tagName, "kelembapanmin")==0)
									{
										char * tagContent=xmlNodeGetContent(row);
										int kelembapanMin=atoi(tagContent);
										aCityList[cityIndex].iKelembapanMin=(char)(kelembapanMin);
										infoFetched++;
									}
									else if(strcasecmp(tagName, "kelembapanmax")==0)
									{
										char * tagContent=xmlNodeGetContent(row);
										int kelembapanMax=atoi(tagContent);
										aCityList[cityIndex].iKelembapanMax=(char)(kelembapanMax);
										infoFetched++;
									}
								}
								row=row->next;
							}
						}
						if(infoFetched>0) { cityIndex++; }
						isi=isi->next;
					}
				}
				cuaca=cuaca->next;
			}
		}
		root=root->next;
	}
	
	// return city count
	return cityIndex;
}

void bmkgweatherparser_close(xmlDoc ** aXml)
{
	if(*aXml) { xmlFreeDoc(*aXml); *aXml=NULL; }
}

