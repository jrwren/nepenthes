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

/* $Id$ */

#include <stdlib.h>
#include <string.h>

#include "UploadResult.hpp"


using namespace std;
using namespace nepenthes;

#ifdef STDTAGS 
#undef STDTAGS 
#endif
#define STDTAGS l_dl | l_mgr



/**
 * UploadResult constructor
 * 
 * @param result the result payload
 * @param size   payload size
 * @param obj    additional data (taken from UploadQuery for the upload)
 */
UploadResult::UploadResult(char *result, uint32_t size, void *obj)
{
	m_Data = (char *)malloc(size);
	memcpy(m_Data,result,size);
	m_Size = size;
	m_Object = obj;
}

/**
 * UploadResult destructor
 * 
 * free the memory
 */
UploadResult::~UploadResult()
{
	free(m_Data);
}

/**
 * get the additional Data
 * 
 * @return returns pointer to the additional data
 */
void *UploadResult::getObject()
{
	return m_Object;
}

/**
 * get a pointer to the data we received in return to uploading something
 * 
 * @return returns pointer to the data
 */
char *UploadResult::getData()
{
	return m_Data;
}

/**
 * get size of the data we received in return to the data we uploaded
 * 
 * @return returns the returns size
 */
uint32_t UploadResult::getSize()
{
	return m_Size;
}
