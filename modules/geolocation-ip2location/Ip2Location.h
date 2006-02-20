#ifndef HAVE_IP2LOCATION_H
#define HAVE_IP2LOCATION_H

#include <stdint.h>
#include <stdio.h>


#define MAX_IP_RANGE 4294967295

#define  COUNTRYSHORT 	0x001
#define  COUNTRYLONG 	0x002
#define  REGION 		0x004
#define  CITY 			0x008
#define  ISP 			0x010
#define  LATITUDE 		0x020
#define  LONGITUDE 		0x040
#define  DOMAIN 		0x080
#define  ALL 			COUNTRYSHORT | COUNTRYLONG | REGION | CITY | ISP | LATITUDE | LONGITUDE | DOMAIN

typedef struct
{
        FILE *filehandle;
        uint8_t databasetype;
        uint8_t databasecolumn;
        uint8_t databaseday;
        uint8_t databasemonth;
        uint8_t databaseyear;
        uint32_t databasecount;
        uint32_t databaseaddr;
}Ip2Location;

typedef struct
{
	char *country_short;
	char *country_long;
	char *region;
	char *city;
	char *isp;
	float latitude;
	float longitude;
	char *domain;
}Ip2LocationRecord;

Ip2Location *ip2location_open(char *db);
int ip2location_initialize(Ip2Location *loc);
uint32_t ip2location_close(Ip2Location *loc);

Ip2LocationRecord *get_country_short(Ip2Location *loc, char *ip);
Ip2LocationRecord *get_country_long(Ip2Location *loc, char *ip);
Ip2LocationRecord *get_region(Ip2Location *loc, char *ip);
Ip2LocationRecord *get_city (Ip2Location *loc, char *ip);
Ip2LocationRecord *get_isp(Ip2Location *loc, char *ip);
Ip2LocationRecord *get_latitude(Ip2Location *loc, char *ip);
Ip2LocationRecord *get_longitude(Ip2Location *loc, char *ip);
Ip2LocationRecord *get_domain(Ip2Location *loc, char *ip);
Ip2LocationRecord *get_all(Ip2Location *loc, char *ip);
Ip2LocationRecord *get_record(Ip2Location *loc, char *ip, uint8_t mode);

Ip2LocationRecord *new_record();
void free_record(Ip2LocationRecord *record);

uint32_t 	read32(FILE *handle, uint32_t position);
uint8_t 	read8(FILE *handle, uint32_t position);
char 		*readStr(FILE *handle, uint32_t position);
float 		readFloat(FILE *handle, uint32_t position);

uint32_t ip2no(uint32_t ip);

#endif


