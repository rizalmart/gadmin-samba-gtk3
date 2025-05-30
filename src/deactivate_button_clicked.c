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



#include "../config.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "widgets.h"
#include "gettext.h"
#include "commands.h"
#include "show_info.h"
#include "commands.h"

extern int activated;
extern int global_smbd_activated;
extern int global_nmbd_activated;
extern int global_winbindd_activated;



void deactivate_button_clicked(struct w *widgets)
{
    gchar *stop, *info;

    /* We dont want it started by init at boot */
    init_stop(widgets);

    if( ! activated )
        return;

    /* Killall -15 makes a graceful shutdown but leaves
       the users logged on. This is not the desired
       result for the admin here, hence -9 */
    if( global_winbindd_activated )
    {
        if(strstr(SYSINIT_START_CMD, "systemctl")){	
			stop = g_strdup_printf("systemctl stop winbind");		
		}
		else{	
			stop = g_strdup_printf("killall -9 %s", WINBINDD_BINARY);
        }
        
        if( ! run_command(stop) )
        {
            info = g_strdup_printf("Stopping samba (winbindd) failed.\n");
            show_info(info);
            g_free(info);
        }
        
        g_free(stop);
    
    }

	if(strstr(SYSINIT_START_CMD, "systemctl")){	
	    stop = g_strdup_printf("systemctl stop samba");		
	}
    else{
		stop = g_strdup_printf("killall -9 %s", SMBD_BINARY);
	}
    
    if( ! run_command(stop) )
    {
        info = g_strdup_printf("Stopping samba (smbd) failed.\n");
        show_info(info);
        g_free(info);
    }
    g_free(stop);

    if( global_nmbd_activated )
    {
        
		if(strstr(SYSINIT_START_CMD, "systemctl")){	
			stop = g_strdup_printf("systemctl stop nmb");		
		}
		else{        
			stop = g_strdup_printf("killall -9 %s", NMBD_BINARY);
		}
        
        if( ! run_command(stop) )
        {
            info = g_strdup_printf("Stopping samba (nmbd) failed.\n");
            show_info(info);
            g_free(info);
        }
        
        g_free(stop);
        
    }
}
