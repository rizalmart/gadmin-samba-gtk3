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
#include "support.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "allocate.h"
#include "widgets.h"
#include "functions.h"
#include "populate_shares.h"
#include "populate_share_settings.h"
#include "show_info.h"
#include "commands.h"
#include "commented.h"
#include "get_option_pos.h"

extern char global_share_name[1024];


void populate_share_settings(struct w *widgets)
{
    /* Lists all share settings */
    FILE *fp;
    char *line, *shareopt;
    long conf_size, opt_pos, i, x, z;
    int found_share = 0;
    GtkTreeIter iter;
    gchar *utf8 = NULL;

    /* Clear all share entries */
    for(i = 0; i < 7; i++)
        gtk_entry_set_text(GTK_ENTRY(widgets->share_set_entry[i]), "");

    /* Set all share combos to the most restrictive setting ("off", "no", etc) */
    for(i = 0; i < 9; i++)
    {
        /* Set combo 4, guest ok/require password to Yes.*/
        if( i == 4 )
        {
           gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->share_set_combo[i]), 0);
           continue;
        }
    
        /* Removed combo 5 */
        if( i == 5 )
           continue;
        
        gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->share_set_combo[i]), 1);
    }

    /* Clear the user access treeviews */
    gtk_list_store_clear(widgets->valid_users_store);
    gtk_list_store_clear(widgets->invalid_users_store);
    gtk_list_store_clear(widgets->write_users_store);
    gtk_list_store_clear(widgets->admin_users_store);

    /* Populate the share settings */
    if((fp = fopen(SAMBA_CONF, "r")) == NULL)
    {
        /* Dont show a popup */
        printf("Error opening: [%s] for listing shares\n", SAMBA_CONF);
        return;
    }
    fseek(fp, 0, SEEK_END);
    conf_size = ftell(fp);
    rewind(fp);

    line = allocate(conf_size + 1);
    shareopt = allocate(conf_size + 10);

    gchar *share_name = g_strdup_printf("[%s]", global_share_name);

    if( conf_size > 1 )
        while(fgets(line, conf_size, fp) != NULL)
        {
            /* Skip commented lines */
            if( commented(line) )
                continue;

            /* The selected share is found */
            if( ! strstr(line, share_name) )
                continue;

            found_share = 1;
            snprintf(shareopt, conf_size, "%s", &line[1]);
            utf8 = g_locale_to_utf8(shareopt, strlen(shareopt) - 2, NULL, NULL, NULL);
            gtk_entry_set_text(GTK_ENTRY(widgets->share_set_entry[0]), utf8);

            /* Append the share settings */
            while(fgets(line, conf_size, fp) != NULL)
            {
                /* Skip commented lines */
                if( commented(line) )
                    continue;

                /* If a new share is found, scroll back and break */
                if( strstr(line, "[") && strstr(line, "]") )
                {
                    fseek(fp, 0, SEEK_END);
                    break;
                }

                /* Only handle options */
                if( ! strstr(line, "=") )
                    continue;

                /* Path */
                if( cmplowercase(line, "path") )
                {
                    opt_pos = get_option_pos(line);
                    snprintf(shareopt, conf_size, "%s", &line[opt_pos]);
                    utf8 = g_locale_to_utf8(shareopt, strlen(shareopt) - 1, NULL, NULL, NULL);
                    gtk_entry_set_text(GTK_ENTRY(widgets->share_set_entry[1]), utf8);
                }

                /* Comment */
                if( cmplowercase(line, "comment") )
                {
                    opt_pos = get_option_pos(line);
                    snprintf(shareopt, conf_size, "%s", &line[opt_pos]);
                    utf8 = g_locale_to_utf8(shareopt, strlen(shareopt) - 1, NULL, NULL, NULL);
                    gtk_entry_set_text(GTK_ENTRY(widgets->share_set_entry[2]), utf8);
                }

                /* Valid users */
                if( cmplowercase(line, "valid users") && !cmplowercase(line, "invalid users") )
                {
                    while( 1 )
                    {
                        i = 0;
                        x = 0;
                        z = 0;
                        /* Scroll to the first username, take '\' continued lines into account */
                        if( strstr(line, "=") )
                        {
                            for(i = 0; line[i] != '\0'; i++)
                                if( line[i] == '=' )
                                    break;
                        }
                        for(i = i; line[i] != '\0'; i++)
                            if( line[i + 1] != ' ' )
                                break;

                        /* Begin at the first username */
                        for(x = i; line[x] != '\0'; x++)
                        {
                            if( (line[x] == ' ' && line[x + 1] != ' ') || (x == 0 && line[x] != ' '
                                    && ! strstr(line, "=")) )
                                for(z = x; line[z] != '\0'; z++)
                                {
                                    if( z <= x )
                                        continue;
                                    /* Get the end of the usernames */
                                    if( line[z] != ' ' && (line[z + 1] == ' '
                                            || line[z + 1] == '\0') )
                                    {
                                        if( x == 0 )
                                            snprintf(shareopt, (z + 2) - x, "%s", &line[x]);
                                        else
                                            snprintf(shareopt, (z + 1) - x, "%s", &line[x + 1]);

                                        if( shareopt[strlen(shareopt) - 1] == '\n' )
                                            shareopt[strlen(shareopt) - 1] = '\0';

                                        if( strlen(shareopt) > 0 && !strstr(shareopt, "\\") )
                                        {
                                            utf8 =  g_locale_to_utf8(shareopt, strlen(shareopt),
                                                NULL, NULL, NULL);
                                            gtk_list_store_append(GTK_LIST_STORE(widgets->
                                                    valid_users_store), &iter);
                                            gtk_list_store_set(GTK_LIST_STORE(widgets->
                                                    valid_users_store), &iter, 0, utf8, -1);
                                        }
                                        break;
                                    }
                                }
                        }
                        /* Handle continued lines "\" or break */
                        if( ! strstr(line, "\\") )
                            break;
                        else
                            while(fgets(line, conf_size, fp) != NULL)
                            {
                                if( commented(line) )
                                    continue;

                                break;
                            }
                    }
                }

                /* Invalid users */
                if( cmplowercase(line, "invalid users") )
                {
                    while( 1 )
                    {
                        i = 0;
                        x = 0;
                        z = 0;
                        /* Scroll to the first username, take '\' continued lines into account */
                        if( strstr(line, "=") )
                        {
                            for(i = 0; line[i] != '\0'; i++)
                                if( line[i] == '=' )
                                    break;
                        }
                        for(i = i; line[i] != '\0'; i++)
                            if( line[i + 1] != ' ' )
                                break;

                        /* Begin at the first username */
                        for(x = i; line[x] != '\0'; x++)
                        {
                            if( (line[x] == ' ' && line[x + 1] != ' ') || (x == 0 && line[x] != ' '
                                    && ! strstr(line, "=")) )
                                for(z = x; line[z] != '\0'; z++)
                                {
                                    if( z <= x )
                                        continue;
                                    /* Get the end of the usernames */
                                    if( line[z] != ' ' && (line[z + 1] == ' '
                                            || line[z + 1] == '\0') )
                                    {
                                        if( x == 0 )
                                            snprintf(shareopt, (z + 2) - x, "%s", &line[x]);
                                        else
                                            snprintf(shareopt, (z + 1) - x, "%s", &line[x + 1]);

                                        if( shareopt[strlen(shareopt) - 1] == '\n' )
                                            shareopt[strlen(shareopt) - 1] = '\0';

                                        if( strlen(shareopt) > 0 && !strstr(shareopt, "\\") )
                                        {
                                            utf8 = g_locale_to_utf8(shareopt, strlen(shareopt),
                                                NULL, NULL, NULL);
                                            gtk_list_store_append(GTK_LIST_STORE(widgets->
                                                    invalid_users_store), &iter);
                                            gtk_list_store_set(GTK_LIST_STORE(widgets->
                                                    invalid_users_store), &iter, 0, utf8, -1);
                                        }
                                        break;
                                    }
                                }
                        }
                        /* Handle continued lines "\" or break */
                        if( ! strstr(line, "\\") )
                            break;
                        else
                            while(fgets(line, conf_size, fp) != NULL)
                            {
                                if( commented(line) )
                                    continue;

                                break;
                            }
                    }
                }

                /* Write list */
                if( cmplowercase(line, "write list") )
                {
                    while( 1 )
                    {
                        i = 0;
                        x = 0;
                        z = 0;
                        /* Scroll to the first username, take '\' continued lines into account */
                        if( strstr(line, "=") )
                        {
                            for(i = 0; line[i] != '\0'; i++)
                                if( line[i] == '=' )
                                    break;
                        }
                        for(i = i; line[i] != '\0'; i++)
                            if( line[i + 1] != ' ' )
                                break;

                        /* Begin at the first username */
                        for(x = i; line[x] != '\0'; x++)
                        {
                            if( (line[x] == ' ' && line[x + 1] != ' ') || (x == 0 && line[x] != ' '
                                    && ! strstr(line, "=")) )
                                for(z = x; line[z] != '\0'; z++)
                                {
                                    if( z <= x )
                                        continue;
                                    /* Get the end of the usernames */
                                    if( line[z] != ' ' && (line[z + 1] == ' '
                                            || line[z + 1] == '\0') )
                                    {
                                        if( x == 0 )
                                            snprintf(shareopt, (z + 2) - x, "%s", &line[x]);
                                        else
                                            snprintf(shareopt, (z + 1) - x, "%s", &line[x + 1]);

                                        if( shareopt[strlen(shareopt) - 1] == '\n' )
                                            shareopt[strlen(shareopt) - 1] = '\0';

                                        if( strlen(shareopt) > 0 && !strstr(shareopt, "\\") )
                                        {
                                            utf8 =  g_locale_to_utf8(shareopt, strlen(shareopt),
                                                NULL, NULL, NULL);
                                            gtk_list_store_append(GTK_LIST_STORE(widgets->
                                                    write_users_store), &iter);
                                            gtk_list_store_set(GTK_LIST_STORE(widgets->
                                                    write_users_store), &iter, 0, utf8, -1);
                                        }
                                        break;
                                    }
                                }
                        }
                        /* Handle continued lines "\" or break */
                        if( ! strstr(line, "\\") )
                            break;
                        else
                            while(fgets(line, conf_size, fp) != NULL)
                            {
                                if( commented(line) )
                                    continue;

                                break;
                            }
                    }
                }

                /* Admin users */
                if( cmplowercase(line, "admin users") )
                {
                    while( 1 )
                    {
                        i = 0;
                        x = 0;
                        z = 0;
                        /* Scroll to the first username, take '\' continued lines into account */
                        if( strstr(line, "=") )
                        {
                            for(i = 0; line[i] != '\0'; i++)
                                if( line[i] == '=' )
                                    break;
                        }
                        for(i = i; line[i] != '\0'; i++)
                            if( line[i + 1] != ' ' )
                                break;

                        /* Begin at the first username */
                        for(x = i; line[x] != '\0'; x++)
                        {
                            if( (line[x] == ' ' && line[x + 1] != ' ') || (x == 0 && line[x] != ' '
                                    && ! strstr(line, "=")) )
                                for(z = x; line[z] != '\0'; z++)
                                {
                                    if( z <= x )
                                        continue;
                                    /* Get the end of the usernames */
                                    if( line[z] != ' ' && (line[z + 1] == ' '
                                            || line[z + 1] == '\0') )
                                    {
                                        if( x == 0 )
                                            snprintf(shareopt, (z + 2) - x, "%s", &line[x]);
                                        else
                                            snprintf(shareopt, (z + 1) - x, "%s", &line[x + 1]);

                                        if( shareopt[strlen(shareopt) - 1] == '\n' )
                                            shareopt[strlen(shareopt) - 1] = '\0';

                                        if( strlen(shareopt) > 0 && !strstr(shareopt, "\\") )
                                        {
                                            utf8 =  g_locale_to_utf8(shareopt, strlen(shareopt),
                                                NULL, NULL, NULL);
                                            gtk_list_store_append(GTK_LIST_STORE(widgets->
                                                    admin_users_store), &iter);
                                            gtk_list_store_set(GTK_LIST_STORE(widgets->
                                                    admin_users_store), &iter, 0, utf8, -1);
                                        }
                                        break;
                                    }
                                }
                        }
                        /* Handle continued lines "\" or break */
                        if( ! strstr(line, "\\") )
                            break;
                        else
                            while(fgets(line, conf_size, fp) != NULL)
                            {
                                if( commented(line) )
                                    continue;

                                break;
                            }
                    }
                }

                /* Directory mask */
                if( cmplowercase(line, "directory mask") )
                {
                    opt_pos = get_option_pos(line);
                    snprintf(shareopt, conf_size, "%s", &line[opt_pos]);
                    utf8 = g_locale_to_utf8(shareopt, strlen(shareopt) - 1, NULL, NULL, NULL);
                    gtk_entry_set_text(GTK_ENTRY(widgets->share_set_entry[3]), utf8);
                }

                /* Create mode */
                if( cmplowercase(line, "create mode") )
                {
                    opt_pos = get_option_pos(line);
                    snprintf(shareopt, conf_size, "%s", &line[opt_pos]);
                    utf8 = g_locale_to_utf8(shareopt, strlen(shareopt) - 1, NULL, NULL, NULL);
                    gtk_entry_set_text(GTK_ENTRY(widgets->share_set_entry[4]), utf8);
                }

                /* Read only */
                if( cmplowercase(line, "read only") )
                {
                    opt_pos = get_option_pos(line);
                    snprintf(shareopt, conf_size, "%s", &line[opt_pos]);
                    if( strstr(line, "yes") )
                        gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->share_set_combo[0]), 0);
                    else
                        gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->share_set_combo[0]), 1);
                }

                /* Available */
                if( cmplowercase(line, "available") )
                {
                    opt_pos = get_option_pos(line);
                    snprintf(shareopt, conf_size, "%s", &line[opt_pos]);
                    if( strstr(line, "yes") )
                        gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->share_set_combo[1]), 0);
                    else
                        gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->share_set_combo[1]), 1);
                }

                /* Browseable */
                if( cmplowercase(line, "browseable") )
                {
                    opt_pos = get_option_pos(line);
                    snprintf(shareopt, conf_size, "%s", &line[opt_pos]);
                    if( strstr(line, "yes") )
                        gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->share_set_combo[2]), 0);
                    else
                        gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->share_set_combo[2]), 1);
                }

                /* Writable */
                if( cmplowercase(line, "writable") )
                {
                    opt_pos = get_option_pos(line);
                    snprintf(shareopt, conf_size, "%s", &line[opt_pos]);
                    if( strstr(line, "yes") )
                        gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->share_set_combo[3]), 0);
                    else
                        gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->share_set_combo[3]), 1);
                }

                /* Guest ok */
                if( cmplowercase(line, "guest ok") )
                {
                    opt_pos = get_option_pos(line);
                    snprintf(shareopt, conf_size, "%s", &line[opt_pos]);
                    if( strstr(line, "yes") ) /* Reverse order, no to yes */
                        gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->share_set_combo[4]), 1);
                    else
                        gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->share_set_combo[4]), 0);
                }

                /* Printable */
                if( cmplowercase(line, "printable") )
                {
                    opt_pos = get_option_pos(line);
                    snprintf(shareopt, conf_size, "%s", &line[opt_pos]);
                    if( strstr(line, "yes") )
                        gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->share_set_combo[6]), 0);
                    else
                        gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->share_set_combo[6]), 1);
                }

                /* File locking */
                if( cmplowercase(line, "locking") && !cmplowercase(line, "strict") )
                {
                    opt_pos = get_option_pos(line);
                    snprintf(shareopt, conf_size, "%s", &line[opt_pos]);
                    if( strstr(line, "yes") )
                        gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->share_set_combo[7]), 0);
                    else
                        gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->share_set_combo[7]), 1);
                }

                /* Strict file locking. Was ~the same as the deprecated "share modes" */
                if( cmplowercase(line, "strict locking") || cmplowercase(line, "share modes") )
                {
                    opt_pos = get_option_pos(line);
                    snprintf(shareopt, conf_size, "%s", &line[opt_pos]);
                    if( strstr(line, "yes") )
                        gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->share_set_combo[8]), 0);
                    else
                        gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->share_set_combo[8]), 1);
                }
            }
        }
    fclose(fp);
    free(line);
    free(shareopt);
    g_free(share_name);

    if(utf8 != NULL)
        g_free(utf8);
}
