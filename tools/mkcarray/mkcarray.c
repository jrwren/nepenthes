/*
 * $Id$
 *
 * mkcarray - create c-style array from binary file.
 *
 * gcc -Wall -Werror mkcarray.c -o mkcarray
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define uint unsigned int
#define byte unsigned char

int main(int argc, char **argv)
{
	FILE *f = stdin;
	byte buffer[16];
	uint bytesRead, totalBytes = 0, i = 1, exploitCode = 0;
	uint startOffset = 0, addInt3 = 0;
	
	while( (argc - i) > 0 )
	{
		if( !strcmp("-e", argv[i]) || !strcmp("--exploit", argv[i]) )
			exploitCode = 1;
		else if( !strcmp("-i", argv[i]) || !strcmp("--int3", argv[i]) )
			addInt3 = 1;
		else if( !strcmp("-o", argv[i]) || !strcmp("--offset", argv[i]) )
		{
			if( !(argc - i > 1) )
			{
				printf("missing argument for -o / --offset\n");
				exit(-1);
			}
			else
			{
				startOffset = atoi(argv[i + 1]);
				i++;
			}
		}
		else
		{
			printf("unknown argument %s\n", argv[i]);
			exit(-1);
		}

		i++;
	}

	if( exploitCode )
	{
		printf("/* compile with gcc -lws2_32 FILE */\n");
		printf("#include <winsock2.h>\n");
		printf("#pragma comment (lib, \"ws2_32\")\n\n");
	}
	
	printf("unsigned char data[] = {\n");

	if( addInt3 )
		printf("0xcc, // debugger trap\n");

	if( startOffset && !feof(f) )
		fseek(f, startOffset, SEEK_SET);
	
	while( !feof(f) )
	{
		bytesRead = fread((void *)buffer, 1, sizeof(buffer), f);

		if( bytesRead > 0 )
		{

			for( i = 0; i < 16; i++ )
			{
				if ( i == 8 )
					printf("  ");
					
				if( i < bytesRead )
					printf("0x%02x, ", buffer[i]);
				else
					printf("      ");
			}

			printf("   // 0x%04x  ", totalBytes);
			
			for( i = 0; i < bytesRead; i++ )
			{
				if ( i == 8 )
				printf("  ");
				
				if( isprint(buffer[i]) )
					printf("%c", buffer[i]);
				else
					printf(".");
			}

			totalBytes += bytesRead;

			printf("\n");
		}
	}
	
	printf("};\n");
	
	if( exploitCode )
	{
		printf(
			"\nvoid fixWSA()\n"
			"{\n"
			"	WSADATA wsa;\n"
			"	WSAStartup(MAKEWORD(2, 0), &wsa);\n"
			"}\n"
			"\n"
			"int main(int argc, char **argv)\n"
			"{\n"
			"	int *ret;\n"
			"\n"
			"	fixWSA();\n"
			"\n"
			"	ret = (int *)&ret + 2;\n"
			"	(*ret) = (int)shellcode;\n"
			"\n"
			"	return 0;\n"
			"}\n"
		);
	}

	return 0;
}
