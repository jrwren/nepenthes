/* $Id$
 *
 * bdiffm - display binary diff matrix for n files.
 *
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#define uint unsigned int
#define byte unsigned char

#define max(a, b) ((a > b) ? (a) : (b))

#define CL_RESET 0
#define CL_RED 31
#define CL_GREEN 32
#define CL_YELLOW 33
#define CL_WHITE 37

typedef struct _FileInfo
{
	uint		m_number;
	const char	*m_filename;
	uint		m_size;
	void		*m_data;
} FileInfo;

void pcolor(uint c)
{
	if( c == CL_RESET )
		printf("\033[0m");
	else
		printf("\033[%d;1m", c);
}

int percent(float p)
{
	return (int)(p * 100);
}

uint getRevision()
{
	char revision[] = "$Rev$";
	char *p = revision;
	
	while( *p && !isdigit(*p) )
		p++;
	
	if( !(*p) )
		return 0; // unknown
	else
		return atoi(p);
}

float getSimilarity(FileInfo *a, FileInfo *b)
{
	uint diffBytes, i;
	
	if( a->m_size > b->m_size )
		diffBytes = a->m_size - b->m_size;
	else
		diffBytes = b->m_size - a->m_size;
	
	if( max(a->m_size, b->m_size) == 0 )
		return 0;
	
	for( i = 0; i < a->m_size && i < b->m_size; i++ )
		if( *((byte *)a->m_data + i) != *((byte *)b->m_data + i) )
			diffBytes++;
	
	return (1.0 - ((float)diffBytes / max(a->m_size, b->m_size)));
}

void displayMatrix(uint count, FileInfo *files)
{

	uint i, j;
	float similarity;
	
	printf("    |");
	for( i = 0; i < count; i++ )
		printf("    %02d", i + 1);
	printf("\n");
	
	printf("----+");
	for( i = 0; i < count; i++ )
		printf("------");
	printf("\n");

	
	for( i = 0; i < count; i++ )
	{
		printf(" %02d |", i + 1);
		for( j = 0; j < i; j++ )
			printf("      ");
		for( j = i; j < count; j++ )
		{
			if( i == j )
			{
				similarity = 1.0;
				pcolor(CL_WHITE);
			}
			else
			{
				similarity = getSimilarity(&files[i], &files[j]);
				if( similarity >= 0.75 )
					pcolor(CL_GREEN);
				else if( similarity >= 0.50 )
					pcolor(CL_YELLOW);
				else
					pcolor(CL_RED);
			}

			printf("  %3d%%", percent(similarity));
			
			pcolor(CL_RESET);
		}
		
		printf("\n");
	}
	
	printf("\n");
}

int main(int argc, char **argv)
{
	uint		fileCount = 0, i;
	int		fd;
	FileInfo	*files;
	struct stat	statInfo;
	const char	*currentFile;
	
	printf("bdiffm version %d built %s %s\n", getRevision(), __DATE__, __TIME__);
	
	if( argc < 2 )
	{
		printf("usage: %s FILE_1 FILE_2 ... FILE_n\n", argv[0]);
		return -1;
	}

	files = (FileInfo *)malloc(sizeof(FileInfo) * (argc - 1));

	for( i = 0; i < (argc - 1); i++ )
	{
		currentFile = argv[i + 1];
		
		fd = open(currentFile, 0);
		
		if( fd == -1 )
		{
			printf("%s: Unable to open %s: %s\n", argv[0], currentFile, strerror(errno));
			return -1;
		}

		stat(currentFile, &statInfo);

		files[i].m_number = i + 1;
		files[i].m_filename = currentFile;
		files[i].m_size = (uint)statInfo.st_size;
		files[i].m_data = mmap(0, files[i].m_size, PROT_READ, MAP_PRIVATE, fd, 0);
		
		if( (int)files[i].m_data == -1 )
		{
			printf("%s: Unable to mmap %s: %s\n", argv[0], currentFile, strerror(errno));
			return -1;
		}
		
		printf("%02d  %-50s  0x%08x  0x%08x\n", files[i].m_number, files[i].m_filename, files[i].m_size, (uint)files[i].m_data);
		
		fileCount++;

		close(fd);
	}
	
	printf("\nComparing %d files.\n\n", fileCount);
	
	displayMatrix(fileCount, files);

	for( i = 0; i < (argc - 1); i++ )
		munmap(files[i].m_data, files[i].m_size);
	

	free(files);

	return 0;
}
