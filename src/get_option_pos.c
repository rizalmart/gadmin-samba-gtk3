/* GADMIN-SAMBA, an easy to use GTK+ frontend for the SAMBA file and print server.
 * Copyright (C) 2006 - 2012 Magnus Loef <magnus-swe@telia.com> 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
*/



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "get_option_pos.h"



int get_option_pos(char *line)
{
    long i = 0;

    /* Locate the position at "=" + 1 */
    if( line != NULL && strlen(line) > 0 )
    {
        for(i = 0; line[i] != '\0'; i++)
        {
            if( line[i] == '=' )
            {
                i++;
                break;
            }
        }
    }

    /* Locate the position of the first char after "=" */
    if( line != NULL && strlen(line) > 0 )
    {
        for(i = i; line[i] != '\0'; i++)
        {
            if( line[i] != ' ' )
                break;
        }
    }

    return i;
}
