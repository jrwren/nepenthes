/********************************************************************************
 *                              Nepenthes
 *                        - finest collection -
 *
 *
 *
 * Copyright (C) 2005  Paul Baecher & Markus Koetter
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 * 
 *             contact nepenthesdev@users.sourceforge.net  
 *
 *******************************************************************************/


#include <stdlib.h>
#include <errno.h>

#include "DownloadBuffer.hpp"
#include "Nepenthes.hpp"
#include "LogManager.hpp"

using namespace nepenthes;


/**
 * DownloadBuffer constructor
 */
DownloadBuffer::DownloadBuffer()
{

	m_Buffer = NULL;
	m_BufferOffset = 0;
	m_BufferSize = 0;
}


/**
 * DownloadBuffer destructor
 */

DownloadBuffer::~DownloadBuffer()
{
	logPF();
	free(m_Buffer);
}


/**
 * Init the DownloadBuffer
 * set the initial Buffers size
 * 
 * @param i      the initial size in bytes
 * 
 * @return true on success, else false
 */
bool DownloadBuffer::Init(uint32_t i)
{
	if(i <= 0 )
	{
		i = 65536;
	}
	m_Buffer = (char *) malloc(sizeof(char)*i);
	m_BufferOffset = 0;
	m_BufferSize = i;

	if(m_Buffer == NULL)
	{
		logCrit("ERROR allocating buffer %s \n",strerror(errno));
		return false;
	}

	return true;
}


/**
 * add data to the DownloadBuffer
 * will resize the buffer if buffersize is to small
 * 
 * @param pszData  the new data
 * @param iDataLen the new datas size
 * 
 * @return true on success
 */
bool DownloadBuffer::addData(char *pszData, uint32_t iDataLen)
{
	if(m_BufferSize == 0 && Init(65536) == false )
	{
		logCrit("Could not write %i to buffer \n", iDataLen);
		return false;
	}
//		printf("Adding %i Bytes data alloc %i ofset %i\n", iDataLen, m_uiBufferSize, m_uiBufferOffset );
	bool bNewBuffer = false;
	while(m_BufferOffset + iDataLen > m_BufferSize )
	{
		m_BufferSize += m_BufferSize; 
		bNewBuffer = true;
	}
	if(bNewBuffer == true)
	{
//			printf("reallocating Buffer to %i so i can carry %i Bytes \n",m_uiBufferSize, m_uiBufferOffset + iDataLen );
		char *pszNewBuffer = (char *)malloc(sizeof(char)*m_BufferSize);
		if( m_Buffer == NULL )
		{
			// malloc error
			return false;
		}

		memset(pszNewBuffer,0,m_BufferSize);
		memcpy(pszNewBuffer, m_Buffer,m_BufferOffset);
		free(m_Buffer);
		m_Buffer = pszNewBuffer;

	}

	memcpy(m_Buffer+m_BufferOffset,pszData,iDataLen);
	m_BufferOffset += iDataLen;
	return true;
}

/**
 * cur some bytes from DownloadBuffer 's front
 * 
 * @param len    the size  of bytes to cut
 * 
 * @return true on success, 
 *         else false
 */
bool DownloadBuffer::cutFront(uint32_t len)
{
	if (len > m_BufferOffset)
	{
		return false;
	}else
	{
		m_BufferOffset -= len;
		memmove(m_Buffer,m_Buffer+len,m_BufferOffset);
		return true;
	}


}

/**
 * get the DownloadBuffer 's Buffer
 * 
 * @return returns pointer to the internal Buffer
 */
char *DownloadBuffer::getData()
{
	return m_Buffer;
}

/**
 * get the internal buffers size
 * 
 * @return returns Buffers size
 */
uint32_t DownloadBuffer::getSize()
{
	return m_BufferOffset;
}

