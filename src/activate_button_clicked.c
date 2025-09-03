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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "widgets.h"
#include "commands.h"
#include "activate_button_clicked.h"
#include "add_standard_users.h"
#include "functions.h"
#include "show_info.h"
#include "commands.h"

extern int activated;
extern int global_start_winbindd;



void activate_button_clicked(struct w *widgets)
{
    gchar *start, *test;
    int retval;

    if(activated)
        return;

    /* If theres no file, nothing is reported so show info if thats the case */
    if( ! file_exists(SAMBA_CONF) )
    {
        test = g_strdup_printf("Cant start the server, missing configuration file:\n%s\n", SAMBA_CONF);
        show_info(test);
        g_free(test);
        return;
    }


    // FIXME: Use conf paths from settings.

    /* Start SMBD */
    if(strstr(SYSINIT_START_CMD, "systemctl")){	
		start = g_strdup_printf("systemctl start smb");
    }
    else{
		start = g_strdup_printf("%s -D -s %s", SMBD_BINARY, SAMBA_CONF);	
	}	
	
	retval=run_command(start);
	
	//debian used smbd instead of smb
	if( ! retval && strstr(start, "systemctl")){
	  	start = g_strdup_printf("systemctl start smbd");
	  	retval=run_command(start);
	}
	

    if( ! retval )
    {
        printf("Starting samba (smbd) failed using this command: %s\n", start);
        
        if(strstr(SYSINIT_START_CMD, "systemctl")){	
			test = g_strdup_printf("%s 2>&1", start);
		}
        else {
			test = g_strdup_printf("%s -D -s %s 2>&1", SMBD_BINARY, SAMBA_CONF);
		}
				
        run_command_show_err(test);
        
        g_free(test);
        g_free(start);

        /* We dont want it started by init if its not working */
        init_stop(widgets);

        return;
    }
    
    g_free(start);


    /* Start NMBD */
    if(strstr(SYSINIT_START_CMD, "systemctl")){			
	  start = g_strdup_printf("systemctl start nmb");
	}
	else{
	  start = g_strdup_printf("%s -D -s %s", NMBD_BINARY, SAMBA_CONF);	
	}	

	retval=run_command(start);
	
	//debian used nmbd instead of nmb
	if( ! retval && strstr(start, "systemctl")){
	  	start = g_strdup_printf("systemctl start nmbd");
	  	retval=run_command(start);
	}
    
    
    if( ! retval )
    {
        printf("Starting samba (nmbd) failed using this command: %s\n", start);
        
        if(strstr(SYSINIT_START_CMD, "systemctl")){	
           test = g_strdup_printf("%s 2>&1", start);			
		}
		else{	
           test = g_strdup_printf("%s -D -s %s 2>&1", NMBD_BINARY, SAMBA_CONF);
	    }
	
        run_command_show_err(test);
        g_free(test);
        g_free(start);
        
        
        return;
    }
    
    g_free(start);
       

    /* We want it started by init at boot */
    init_start(widgets);

    /* Start WinBindD if the settings sais so */
    if( global_start_winbindd )
    {
        if(strstr(SYSINIT_START_CMD, "systemctl")){	
			start = g_strdup_printf("systemctl start winbind");		
		}		
		else{		
			start = g_strdup_printf("%s -D -s %s", WINBINDD_BINARY, SAMBA_CONF);
	    }
        
        if( ! run_command(start) )
        {
            printf("Starting winbindd failed using this command: %s\nThe main SAMBA servers are working ok.\n", start);
            
            if(strstr(SYSINIT_START_CMD, "systemctl")){	
				test = g_strdup_printf("%s 2>&1",start);				
			}
			else{	
				test = g_strdup_printf("%s -D -s %s 2>&1", WINBINDD_BINARY, SAMBA_CONF);
		    }
            
            run_command_show_err(test);
            g_free(test);
            g_free(start);
            return;
        }
        
        g_free(start);
    }
}
