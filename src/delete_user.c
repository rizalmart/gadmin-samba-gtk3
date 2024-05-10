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
#include <stdlib.h>
#include <string.h>
#include "widgets.h"
#include "gettext.h"
#include "allocate.h"
#include "commands.h"
#include "show_info.h"
#include "delete_user.h"
#include "functions.h"
#include "commented.h"
#include "get_option_pos.h"
#include "system_defines.h"
#include "create_del_system_user_question.h"
#include "populate_users.h"
#include "select_first_user.h"
#include "populate_user_settings.h"
#include "populate_shares.h"
#include "select_first_share.h"
#include "populate_share_settings.h"
#include "populate_conf_tab.h"
#include "reread_conf.h"

#ifdef USE_DARWIN
#include "osx_functions.h"
#endif


extern char global_user_name[1024];

extern int activated;



void delete_user(struct w *widgets)
{
    /* Deletes the samba user, deletes its settings from 
       smb.conf and asks to delete the system user as well */
    FILE *fp;
    long conf_size, opt_pos;
    char *line, *user_line, *new_line, *new_conf, *temp;
    int root_user = 0, i = 0, length = 0, user_deleted = 0;
    gchar *info, *cmd;

    G_CONST_RETURN gchar *username;


    username = gtk_entry_get_text(GTK_ENTRY(widgets->user_set_entry[0]));

    /* If the namefield in username is empty inform that we refuse to do anything */
    length = strlen(username);
    if( length == 0 )
    {
        info = g_strdup_printf(_("No user specified.\n"));
        show_info(info);
        g_free(info);
        return;
    }


    /* Delete all traces of this username in smb.conf's shares */
    if((fp = fopen(SAMBA_CONF, "r")) == NULL)
    {
        info =
            g_strdup_printf(_("Cant open smb.conf here: %s run gadmin-samba as root.\n"),
            SAMBA_CONF);
        show_info(info);
        g_free(info);
        return;
    }
    fseek(fp, 0, SEEK_END);
    conf_size = ftell(fp);
    rewind(fp);

    line = allocate(conf_size + 1);
    new_conf = allocate(conf_size + 1);
    new_line = allocate(conf_size + 1);
    user_line = allocate(conf_size + 1);

    if( conf_size > 1 )
        while(fgets(line, conf_size, fp) != NULL)
        {
            if(commented(line))
                continue;

            /* Lines where this user could be listed */
            if( strstr(line, "valid users =")
                || strstr(line, "invalid users =")
                || strstr(line, "write list =") || strstr(line, "admin users =") )
            {
                user_deleted = 0;
                new_line[0] = '\0';

                /* Only handle the users and groups */
                opt_pos = get_option_pos(line);

                snprintf(user_line, conf_size, "%s", &line[opt_pos]);

                for(i = strlen(user_line) - 1; user_line[i] != '\0'; i--)
                    if(user_line[i] != '\r' || user_line[i] != '\n')
                    {
                        user_line[i] = '\0';
                        break;
                    }

                /* There must be an extra whitespace at the end for strtok */
                strcat(user_line, " ");

                temp = strtok(user_line, " ");
                while(temp != NULL)
                {
                    if(!strcmp(temp, username))
                    {
                        /* The user is deleted */
                        user_deleted = 1;
                    }
                    else
                    {
                        /* Contents: username username username etc */
                        strcat(new_line, " ");  /* Add whitespace */
                        strcat(new_line, temp); /* Add the user   */
                    }

                    /* We pass NULL to strtok to get the next token in the string */
                    temp = strtok(NULL, " ");
                }

                if(user_deleted)
                {
                    if(strstr(line, "valid users ="))
                        strcat(new_conf, "valid users =");  /* Whitespace added above */

                    if(strstr(line, "write list ="))
                        strcat(new_conf, "write list =");   /* Whitespace added above */

                    if(strstr(line, "admin users ="))
                        strcat(new_conf, "admin users =");  /* Whitespace added above */

                    strcat(new_conf, new_line); /* Add the users that wherent deleted */
                    strcat(new_conf, "\n");
                }
                else            /* No changes where made */
                    strcat(new_conf, line);
            }
            else                /* Copy everything else */
                strcat(new_conf, line);
        }
    fclose(fp);
    free(line);
    free(new_line);
    free(user_line);


    /* Write the new config without the deleted users settings */
    if((fp = fopen(SAMBA_CONF, "w+")) == NULL)
    {
        printf("Error Writing configuration here: %s\n", SAMBA_CONF);
        return;
    }
    fputs(new_conf, fp);
    fclose(fp);
    free(new_conf);

    /* Dont ever ask to delete the root system user */
    if( username[0] == 'r' && username[1] == 'o'
        && username[2] == 'o' && username[3] == 't' && strlen(username) == 4 )
    {
        info = g_strdup_printf(_
            ("The samba root user was deleted.\nThe root system user can not be deleted.\n"));
        show_info(info);
        g_free(info);
        root_user = 1;
    }


    /* Delete the samba user */
    if( ! root_user )
    {
        cmd = g_strdup_printf("%s -x -u '%s'", PDBEDIT_BINARY, username);
        if( ! run_command(cmd) )
        {
            info = g_strdup_printf(_("Could not remove the samba user: %s\n"), username);
            show_info(info);
            g_free(info);
            /* Dont return.. just tell the user */
        }
        g_free(cmd);
    }


    /* Ask to delete the system user if its not "root" */
    if( user_exists(username) && ! root_user )
    {
        /* Repopulation is done after the question */
        create_del_system_user_question(widgets);
        gtk_widget_show_all(widgets->del_system_user_question_window);
    }
    else
    {
        /* Repopulate affected things */
        populate_users(widgets);
        select_first_user(widgets);
        populate_user_settings(widgets);

        populate_shares(widgets);
        select_first_share(widgets);
        populate_share_settings(widgets);

        populate_conf_tab(widgets);

        /* Update the server */
        reread_conf(widgets);
    }
}



void dont_delete_system_user(struct w *widgets)
{
    gtk_widget_destroy(widgets->del_system_user_question_window);

    /* Update the user list and the user settings */
    populate_users(widgets);
    select_first_user(widgets);
    populate_user_settings(widgets);

    populate_shares(widgets);
    select_first_share(widgets);
    populate_share_settings(widgets);

    populate_conf_tab(widgets);

    /* Update the server */
    reread_conf(widgets);
}



void do_delete_system_user(GtkButton * button, struct w *widgets)
{
    /* Deletes the system user */

#ifndef USE_DARWIN
    char *command;
#endif

    int deleted = 0;
    int length = 0;
    gchar *utf8 = NULL;
    gchar *info;
    gchar *username;

    gtk_widget_destroy(widgets->del_system_user_question_window);

    username = g_strdup_printf("%s", global_user_name);

    if( username == NULL )
    {
        info = g_strdup_printf(_("No username specified.\n"));
        show_info(info);
        g_free(info);

        return;
    }


    /* If the username field is empty inform that this cant be done. */
    length = strlen(username);
    if( length == 0 )
    {
        info = g_strdup_printf(_("No username specified.\n"));
        show_info(info);
        g_free(info);
        return;
    }

    if( username[0] == 'r' && username[1] == 'o' && username[2] == 'o' && username[3] == 't'
        && strlen(username) == 4 )
    {
        info = g_strdup_printf(_("The root user can not be deleted.\n"));
        show_info(info);
        g_free(info);
        return;
    }


#ifndef USE_DARWIN
    if( user_exists(username) )
    {
        command = allocate(4096);
        snprintf(command, 4000, "%s %s", DELUSER, username);
        if(run_command(command))
            deleted = 1;

        free(command);
    }
#elif USE_DARWIN
    if( niutil_user_exists(username) )
    {
        if( niutil_userdel(username) )
            deleted = 1;
    }
#endif


    if( ! deleted )
    {
        info = g_strdup_printf(_("Failed to delete system user: %s\n"), username);
        show_info(info);
        g_free(info);
    }

    if( utf8 != NULL )
        g_free(utf8);


    /* Update the user list and the user settings */
    populate_users(widgets);
    select_first_user(widgets);
    populate_user_settings(widgets);

    populate_shares(widgets);
    select_first_share(widgets);
    populate_share_settings(widgets);

    populate_conf_tab(widgets);

    /* Update the server */
    reread_conf(widgets);
}
