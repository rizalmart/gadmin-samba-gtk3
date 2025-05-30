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


#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include "gettext.h"
#include "widgets.h"
#include "allocate.h"
#include "show_info.h"

extern int MAX_READ_POPEN;
extern int global_start_winbindd;



void run_command_show_err(gchar * command)
{
    FILE *fp;
    char *line, *info;

    if((fp = popen(command, "r")) == NULL)
    {
        perror("popen");
        return;
    }
    else
    {
        line = allocate(MAX_READ_POPEN + 2);
        info = allocate(MAX_READ_POPEN + 2);
        while(fgets(line, MAX_READ_POPEN, fp) != NULL)
            strcat(info, line);

        pclose(fp);
        show_info(info);
        free(info);
        free(line);
    }
}


int run_command(gchar * command)
{
    FILE *fp;
    int status = 0, exit_status = 0;

    if((fp = popen(command, "w")) == NULL)
    {
        perror("popen");
        return 0;
    }
    status = pclose(fp);

    exit_status = WEXITSTATUS(status);

    if( exit_status > 0 )         // Was == 1
        exit_status = 0;
    else
        exit_status = 1;

    return exit_status;
}


void init_start(struct w *widgets)
{
    gchar *cmd;

    /* RH/Fedora has a separate winbindd service */
    if( global_start_winbindd && strstr(SYSINIT_START_CMD, "chkconfig") ){
        cmd = g_strdup_printf("%s && chkconfig winbind on", SYSINIT_START_CMD);
    }
    else if( global_start_winbindd && strstr(SYSINIT_START_CMD, "systemctl") ){
        cmd = g_strdup_printf("%s && systemctl enable winbind", SYSINIT_START_CMD);
    }
    else{
        cmd = g_strdup_printf("%s", SYSINIT_START_CMD);
	}
	
    if( strlen(cmd) > 4 )
    {
        if( ! run_command(cmd) )
        {
            run_command_show_err(cmd);
        }
    }
    if( cmd != NULL )
        g_free(cmd);
}


void init_stop(struct w *widgets)
{
    gchar *cmd;

    /* RH/Fedora has a separate winbindd service */
    if( global_start_winbindd && strstr(SYSINIT_STOP_CMD, "chkconfig") ){
        cmd = g_strdup_printf("%s && chkconfig winbind off", SYSINIT_STOP_CMD);
    }
    else if( global_start_winbindd && strstr(SYSINIT_STOP_CMD, "systemctl") ){
        cmd = g_strdup_printf("%s && systemctl disable winbind", SYSINIT_STOP_CMD);
    }
    else
    {
        cmd = g_strdup_printf("%s", SYSINIT_STOP_CMD);
	}
	
    if( strlen(cmd) > 4 )
    {
        if( ! run_command(cmd) )
        {
            run_command_show_err(cmd);
        }
    }
    if( cmd != NULL )
        g_free(cmd);
}


int file_exists(char *infile)
{
    FILE *fp;
    if((fp = fopen(infile, "r")) == NULL)
        return 0;

    fclose(fp);
    return 1;
}


void get_gadmin_samba_users()
{
    gchar *cmd;

    /* Secure the gadmin_samba_users file. */
    cmd = g_strdup_printf("touch %s && chmod 600 %s\n", GADMIN_SAMBA_USERS, GADMIN_SAMBA_USERS);
    if( ! run_command(cmd) )
    {
        printf("Show info here: Creating an initial gadmin_samba_users file failed.\n");
    }
    g_free(cmd);

    /* List users like the smbpasswd file. (Use -v instead of -w to get more info) */
    cmd = g_strdup_printf("%s -L -w > %s\n", PDBEDIT_BINARY, GADMIN_SAMBA_USERS);
    if( ! run_command(cmd) )
    {
        printf("Show info here: pbedit -L -w -v failed.\n");
    }
    g_free(cmd);
}


int add_locked_samba_user(gchar * username)
{
    FILE *fp;
    gchar *info, *cmd;

    /* Add the user */
    cmd = g_strdup_printf("%s -t -a -u '%s'", PDBEDIT_BINARY, username);
    if((fp = popen(cmd, "w")) == NULL)
    {
        perror("popen");
        info = g_strdup_printf(_("Failed adding locked samba user: %s\n"), username);
        show_info(info);
        g_free(info);
        g_free(cmd);
        return 0;
    }
    fputs("\n", fp);
    fputs("\n", fp);
    pclose(fp);
    g_free(cmd);

    /* Disable it */
    cmd = g_strdup_printf("%s -c \"[D ]\" -u '%s'", PDBEDIT_BINARY, username);
    if((fp = popen(cmd, "w")) == NULL)
    {
        perror("popen");
        info = g_strdup_printf(_("Failed disabling/locking samba user: %s\n"), username);
        show_info(info);
        g_free(info);
        g_free(cmd);
        return 0;
    }
    pclose(fp);
    g_free(cmd);

    return 1;
}


int add_null_password_samba_user(gchar * username)
{
    FILE *fp;
    gchar *info, *cmd;

    /* Add the user */
    cmd = g_strdup_printf("%s -t -a -u '%s'", PDBEDIT_BINARY, username);
    if((fp = popen(cmd, "w")) == NULL)
    {
        perror("popen");
        info = g_strdup_printf(_("Failed adding null password samba user: %s\n"), username);
        show_info(info);
        g_free(info);
        g_free(cmd);
        return 0;
    }
    fputs("\n", fp);
    fputs("\n", fp);
    pclose(fp);
    g_free(cmd);

    /* Set null password / require no password if NULL passwords are allowed */
    cmd = g_strdup_printf("%s -c \"[N ]\" -u '%s'", PDBEDIT_BINARY, username);
    if((fp = popen(cmd, "w")) == NULL)
    {
        perror("popen");
        info = g_strdup_printf(_("Failed locking null password samba user: %s\n"), username);
        show_info(info);
        g_free(info);
        g_free(cmd);
        return 0;
    }

    return 1;
}
