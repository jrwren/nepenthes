
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "Ip2Location.h"



uint8_t COUNTRY_POSITION[9]		= {0, 2, 2, 2, 2, 2, 2, 2, 2};
uint8_t REGION_POSITION[9]		= {0, 0, 0, 3, 3, 3, 3, 3, 3};
uint8_t CITY_POSITION[9]		= {0, 0, 0, 4, 4, 4, 4, 4, 4};
uint8_t ISP_POSITION[9]			= {0, 0, 3, 0, 5, 0, 7, 5, 7};
uint8_t LATITUDE_POSITION[9] 	= {0, 0, 0, 0, 0, 5, 5, 0, 5};
uint8_t LONGITUDE_POSITION[9]	= {0, 0, 0, 0, 0, 6, 6, 0, 6};
uint8_t DOMAIN_POSITION[9]		= {0, 0, 0, 0, 0, 0, 0, 6, 8};


Ip2Location *ip2location_open(char *db)
{
	FILE *f;

	if ((f=fopen(db,"rb"))==NULL)
	{
		printf("Could not open %s\n",db);
		return NULL;
	}

	Ip2Location *loc = (Ip2Location *)malloc(sizeof(Ip2Location));
	memset(loc,0,sizeof(Ip2Location));

	loc->filehandle = f;

	return loc;
}

uint32_t ip2location_close(Ip2Location *loc)
{
	fclose(loc->filehandle);
	free(loc);
	return 0;
}

int ip2location_initialize(Ip2Location *loc)
{
	loc->databasetype   = read8(loc->filehandle, 1);
	loc->databasecolumn = read8(loc->filehandle, 2);
	loc->databaseday    = read8(loc->filehandle, 3);
	loc->databasemonth  = read8(loc->filehandle, 4);
	loc->databaseyear   = read8(loc->filehandle, 5);
	loc->databasecount  = read32(loc->filehandle, 6);
	loc->databaseaddr   = read32(loc->filehandle, 10);

/*
	printf("Debug\n"        
        "uint8_t databasetype	%i\n"
        "uint8_t databasecolumn  %i\n"
        "uint8_t databaseday     %i\n"
        "uint8_t databasemonth   %i\n"
        "uint8_t databaseyear    %i\n"
        "uint32_t databasecount  %i\n"
        "uint32_t databaseaddr   %i\n",
		   loc->databasetype,
		   loc->databasecolumn,
		   loc->databaseday,
		   loc->databasemonth,
		   loc->databaseyear,
		   loc->databasecount,
		   loc->databaseaddr);
*/



	return 0;
}

Ip2LocationRecord *get_country_short(Ip2Location *loc, char *ip)
{
	return get_record(loc,ip,COUNTRYSHORT);
}

Ip2LocationRecord *get_country_long(Ip2Location *loc, char *ip)
{
	return get_record(loc,ip,COUNTRYLONG);
}

Ip2LocationRecord *get_region(Ip2Location *loc, char *ip)
{
	return get_record(loc,ip,REGION);
}

Ip2LocationRecord *get_city (Ip2Location *loc, char *ip)
{
	return get_record(loc,ip,CITY);
}

Ip2LocationRecord *get_isp(Ip2Location *loc, char *ip)
{
	return get_record(loc,ip,ISP);
}

Ip2LocationRecord *get_latitude(Ip2Location *loc, char *ip)
{
	return get_record(loc,ip,LATITUDE);
}

Ip2LocationRecord *get_longitude(Ip2Location *loc, char *ip)
{
	return get_record(loc,ip,LONGITUDE);
}

Ip2LocationRecord *get_domain(Ip2Location *loc, char *ip)
{
	return get_record(loc,ip,DOMAIN);
}

Ip2LocationRecord *get_all(Ip2Location *loc, char *ip)
{
	return get_record(loc,ip,ALL);
}

Ip2LocationRecord *get_record(Ip2Location *loc, char *ipstring, uint8_t mode)
{
	uint8_t dbtype = loc->databasetype;

	uint32_t ipno=ip2no(inet_addr(ipstring));
    FILE *handle = loc->filehandle;
    uint32_t baseaddr = loc->databaseaddr;
    uint32_t dbcount = loc->databasecount;
    uint32_t dbcolumn = loc->databasecolumn;

    uint32_t low = 0;
    uint32_t high = dbcount;
    uint32_t mid = 0;
    uint32_t ipfrom = 0;
    uint32_t ipto = 0;

	Ip2LocationRecord *record = new_record();

	if ((int)ipno == (int)MAX_IP_RANGE)
	{

		if (mode & COUNTRYSHORT && COUNTRY_POSITION[dbtype] != 0 )
			record->country_short = readStr(handle, read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (COUNTRY_POSITION[dbtype]-1)));
		
		if( mode & COUNTRYLONG && COUNTRY_POSITION[dbtype] != 0 )
			record->country_long = readStr(handle, read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (COUNTRY_POSITION[dbtype]-1))+3);
			
		if( mode & REGION && REGION_POSITION[dbtype] != 0 )
			record->region = readStr(handle, read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (REGION_POSITION[dbtype]-1)));
			
		if( mode & CITY && CITY_POSITION[dbtype] != 0 )
			record->city = readStr(handle, read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (CITY_POSITION[dbtype]-1)));
			
		if( mode & ISP && ISP_POSITION[dbtype] != 0 )
			record->isp = readStr(handle, read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (ISP_POSITION[dbtype]-1)));
			
		if( mode & LATITUDE && LATITUDE_POSITION[dbtype] != 0 )
			record->latitude = readFloat(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (LATITUDE_POSITION[dbtype]-1));
			
		if( mode & LONGITUDE && LONGITUDE_POSITION[dbtype] != 0 )
			record->longitude = readFloat(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (LONGITUDE_POSITION[dbtype]-1));
			
		if( mode & DOMAIN && DOMAIN_POSITION[dbtype] != 0 )
			record->domain = readStr(handle, read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (DOMAIN_POSITION[dbtype]-1)));
			
		return record;

	}else
	{
		while (low <= high) 
		{
			mid = (uint32_t)((low + high)/2);
			ipfrom 	= read32(handle, baseaddr + mid       * dbcolumn * 4);
			ipto 	= read32(handle, baseaddr + (mid + 1) * dbcolumn * 4);

			if ((ipno >= ipfrom) && (ipno < ipto)) 
			{
				if (mode & COUNTRYSHORT && COUNTRY_POSITION[dbtype] != 0 )
					record->country_short = readStr(handle, read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (COUNTRY_POSITION[dbtype]-1)));

				if( mode & COUNTRYLONG && COUNTRY_POSITION[dbtype] != 0 )
					record->country_long = readStr(handle, read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (COUNTRY_POSITION[dbtype]-1))+3);

				if( mode & REGION && REGION_POSITION[dbtype] != 0 )
					record->region = readStr(handle, read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (REGION_POSITION[dbtype]-1)));

				if( mode & CITY && CITY_POSITION[dbtype] != 0 )
					record->city = readStr(handle, read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (CITY_POSITION[dbtype]-1)));

				if( mode & ISP && ISP_POSITION[dbtype] != 0 )
					record->isp = readStr(handle, read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (ISP_POSITION[dbtype]-1)));

				if( mode & LATITUDE && LATITUDE_POSITION[dbtype] != 0 )
					record->latitude = readFloat(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (LATITUDE_POSITION[dbtype]-1));

				if( mode & LONGITUDE && LONGITUDE_POSITION[dbtype] != 0 )
					record->longitude = readFloat(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (LONGITUDE_POSITION[dbtype]-1));

				if( mode & DOMAIN && DOMAIN_POSITION[dbtype] != 0 )
					record->domain = readStr(handle, read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (DOMAIN_POSITION[dbtype]-1)));

				return record;

			} else
			{
				if ( ipno < ipfrom )
				{
					high = mid - 1;
				} else
				{
					low = mid + 1;
				}
			}

		}
	}
	free_record(record);
	return NULL;
}


Ip2LocationRecord *new_record()
{
	Ip2LocationRecord *record = (Ip2LocationRecord *)malloc(sizeof(Ip2LocationRecord));
	memset(record,0,sizeof(Ip2LocationRecord));
	return record;
}


void free_record(Ip2LocationRecord *record)
{
	if (record->city != NULL)
    	free(record->city);

	if (record->country_long != NULL)
		free(record->country_long);

	if (record->country_short != NULL)
		free(record->country_short);

	if (record->domain != NULL)
		free(record->domain);

	if (record->isp != NULL)
		free(record->isp);

	if (record->region != NULL)
		free(record->region);

	free(record);
}

uint32_t    read32(FILE *handle, uint32_t position)
{
	uint32_t ret=0;
	fseek(handle,position-1,0);
	fread(&ret,1,4,handle);
	return ret;
}

uint8_t read8(FILE *handle, uint32_t position)
{	
	uint8_t ret=0;
	fseek(handle,position-1,0);
	fread(&ret,1,1,handle);
	return ret;
}

char *readStr(FILE *handle, uint32_t position)
{
	uint8_t size=0;
	char *str=0;
	fseek(handle,position,0);
	fread(&size,1,1,handle);
	str=(char *)malloc(size+1);
	memset(str,0,size+1);
	fread(str,size,1,handle);
	return str;
}

float readFloat(FILE *handle, uint32_t position)
{
	float ret=0.0;
	fseek(handle,position-1,0);
	fread(&ret,4,1,handle);
	return ret;
}

uint32_t ip2no(uint32_t ip)
{
	uint8_t *ptr = (uint8_t *)&ip;
	uint32_t a;
	a=  (uint8_t)(ptr[3]);
	a+= (uint8_t)(ptr[2])*256;
	a+= (uint8_t)(ptr[1])*256*256;
	a+= (uint8_t)(ptr[0])*256*256*256;
	return a;
}


