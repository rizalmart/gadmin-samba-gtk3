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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "gettext.h"
#include "widgets.h"
#include "allocate.h"
#include "chars_are_digits.h"
#include "populate_server_settings.h"
#include "commented.h"
#include "get_option_pos.h"
#include "show_info.h"
#include "functions.h"


void populate_server_settings(struct w *widgets)
{
    /* Lists the values in the server tab */
    FILE *fp;
    int val = 0, global_section_found = 0;
    int is_user = 0, is_bdc = 0;
    long conf_size, opt_pos;
    char *line, *new_buffer;
    gchar *info, *utf8 = NULL;

    if((fp = fopen(SAMBA_CONF, "r")) == NULL)
    {
        info = g_strdup_printf(_("Cant open smb.conf here:\n%s\n"), SAMBA_CONF);
        show_info(info);
        g_free(info);
        return;
    }
    fseek(fp, 0, SEEK_END);
    conf_size = ftell(fp);
    rewind(fp);

    line = allocate(conf_size + 1);
    new_buffer = allocate(conf_size + 1);

    /* First determine if security is "user", set as: is_user */
    if( conf_size > 1 )
        while(fgets(line, conf_size, fp) != NULL)
        {
            if( commented(line) )
                continue;

            /* Break at any share definition */
            if( strstr(line, "[") && !cmplowercase(line, "[global]") )
                break;

            /* Security is user */
            if( cmplowercase(line, "security = ") && cmplowercase(line, "user") )
                is_user = 1;
        }
    rewind(fp);

    /* We know that security is USER, now see if its a BDC, set as: is_bdc */
    if( is_user && conf_size > 1 )
        while(fgets(line, conf_size, fp) != NULL)
        {
            if( commented(line) )
                continue;

            /* Break at any share definition */
            if( strstr(line, "[") && !cmplowercase(line, "[global]") )
                break;

            /* If domain logons is specified its a BDC */
            if( cmplowercase(line, "domain logons = ") && cmplowercase(line, "yes") )
                is_bdc = 1;
        }
    rewind(fp);


    /* First scroll to the global section */
    if( conf_size > 1 )
        while(fgets(line, conf_size, fp) != NULL)
        {
            if( cmplowercase(line, "[global]") && !commented(line) )
            {
                global_section_found = 1;
                break;
            }
        }

    if( ! global_section_found )
    {
        info = g_strdup_printf(_("Can not find the [global] section in %s\nNo changes made.\n"),
            SAMBA_CONF);
        show_info(info);
        g_free(info);
        fclose(fp);
        free(line);
        free(new_buffer);
        return;
    }


    if( conf_size > 1 )
        while(fgets(line, conf_size, fp) != NULL)
        {
            /* Scroll past any comment */
            if( commented(line) )
                continue;

            /* Break at any share definition "[" */
            if( strstr(line, "[") )
                break;


            /* Entries... */

            /* Populate matching server directives */
            if( cmplowercase(line, "netbios name") )
            {
                /* Positions are at the first char located after "=" not including whitespace */
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if(new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r')
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[0]), utf8);
            }
            else if( cmplowercase(line, "server string") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if(new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r')
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[1]), utf8);
            }
            else if( cmplowercase(line, "workgroup") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if(new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r')
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[2]), utf8);
            }
            else if( cmplowercase(line, "realm") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if(new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r')
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[3]), utf8);
            }
            else if( cmplowercase(line, "hosts allow") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if(new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r')
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[4]), utf8);
            }
            else if( cmplowercase(line, "interfaces") && ! cmplowercase(line, "bind interfaces only") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[5]), utf8);
            }
            else if( cmplowercase(line, "remote announce") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[6]), utf8);
            }
            else if( cmplowercase(line, "remote browse sync") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[7]), utf8);
            }
            else if( cmplowercase(line, "printcap name") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[8]), utf8);
            }
            else if( cmplowercase(line, "guest account") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[9]), utf8);
            }
            else if( cmplowercase(line, "log file") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[10]), utf8);
            }
            else if( cmplowercase(line, "socket options") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[11]), utf8);
            }
            else if( cmplowercase(line, "logon drive") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[12]), utf8);
            }
            else if( cmplowercase(line, "logon home") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[13]), utf8);
            }
            else if( cmplowercase(line, "logon path") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[14]), utf8);
            }
            else if( cmplowercase(line, "logon script") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[15]), utf8);
            }
            else if( cmplowercase(line, "name resolve order") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[16]), utf8);
            }
            else if( cmplowercase(line, "wins server") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[17]), utf8);
            }
            else if( cmplowercase(line, "username map") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[18]), utf8);
            }
            else if( cmplowercase(line, "passwd program") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[20]), utf8);
            }
            else if( cmplowercase(line, "passwd chat") && ! cmplowercase(line, "passwd chat timeout") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[22]), utf8);
            }
            else if( cmplowercase(line, "add user script") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[23]), utf8);
            }
            else if( cmplowercase(line, "add user to group script") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[24]), utf8);
            }
            else if( cmplowercase(line, "add group script") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[25]), utf8);
            }
            else if( cmplowercase(line, "delete user script") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[26]), utf8);
            }
            else
             if( cmplowercase(line, "delete user from group script") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[27]), utf8);
            }
            else
             if( cmplowercase(line, "delete group script") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[28]), utf8);
            }
            else if( cmplowercase(line, "add machine script") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[29]), utf8);
            }
            else if( cmplowercase(line, "idmap uid") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[30]), utf8);
            }
            else if( cmplowercase(line, "idmap gid") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[31]), utf8);
            }
            else if( cmplowercase(line, "template shell") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[32]), utf8);
            }
            else if( cmplowercase(line, "winbind separator") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[33]), utf8);
            }
            else

                /* Combos... */

                /* Security level, also sets realm usability */
            if( cmplowercase(line, "security") )
            {
                /* User */
                if( ! is_bdc && cmplowercase(line, "user") )
                {
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[0]), 0);

                    /* Clear Realm entry, disable input and remove the option */
                    gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[3]), "");
                    gtk_widget_set_sensitive(GTK_WIDGET(widgets->server_set_entry[3]), 0);
                }
                else /* BDC */ if( is_bdc && cmplowercase(line, "user") )
                {
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[0]), 1);

                    /* Can realm be used here ? */
                    gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[3]), "");
                    gtk_widget_set_sensitive(GTK_WIDGET(widgets->server_set_entry[3]), 0);
                }
                else if( cmplowercase(line, "domain") )
                {
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[0]), 2);

                    /* Can realm be used here ? */
                    gtk_entry_set_text(GTK_ENTRY(widgets->server_set_entry[3]), "");
                    gtk_widget_set_sensitive(GTK_WIDGET(widgets->server_set_entry[3]), 0);
                }
                else            /* ADS */
                {
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[0]), 3);

                    /* Enable Kerberos Realm entry (Disabled in apply server settings) */
                    gtk_widget_set_sensitive(GTK_WIDGET(widgets->server_set_entry[3]), 1);
                }
            }
            else if( cmplowercase(line, "load printers") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[1]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[1]), 1);
            }
            else if( cmplowercase(line, "cups options") )
            {
                if( cmplowercase(line, "raw") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[2]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[2]), 1);
            }
            else
// printing = cups missing, dunno what extra options it has.

            if( cmplowercase(line, "null passwords") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[3]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[3]), 1);
            }
            else if( cmplowercase(line, "encrypt passwords") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[4]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[4]), 1);
            }
            else if( cmplowercase(line, "unix password sync") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[5]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[5]), 1);
            }
            else if( cmplowercase(line, "local master") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[6]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[6]), 1);
            }
            else if( cmplowercase(line, "domain master") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[7]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[7]), 1);
            }
            else if( cmplowercase(line, "preferred master") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[8]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[8]), 1);
            }
            else if( cmplowercase(line, "domain logons") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[9]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[9]), 1);
            }
            else if( cmplowercase(line, "time server") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[10]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[10]), 1);
            }
            else if( cmplowercase(line, "wins support") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[11]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[11]), 1);
            }
            else if( cmplowercase(line, "wins proxy") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[12]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[12]), 1);
            }
            else if( cmplowercase(line, "dns proxy") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[13]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[13]), 1);
            }
            else if( cmplowercase(line, "preserve case") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[14]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[14]), 1);
            }
            else if( cmplowercase(line, "client use spnego") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[15]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[15]), 1);
            }
            else if( cmplowercase(line, "client signing") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[16]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[16]), 1);
            }
            else if( cmplowercase(line, "client schannel") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[17]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[17]), 1);
            }
            else if( cmplowercase(line, "server signing") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[18]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[18]), 1);
            }
            else if( cmplowercase(line, "server schannel") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[19]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[19]), 1);
            }
            else if( cmplowercase(line, "nt pipe support") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[20]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[20]), 1);
            }
            else if( cmplowercase(line, "nt status support") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[21]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[21]), 1);
            }
            else if( cmplowercase(line, "allow trusted domains") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[22]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[22]), 1);
            }
            else if( cmplowercase(line, "obey pam restrictions") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[23]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[23]), 1);
            }
            else if( cmplowercase(line, "enable spoolss") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[24]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[24]), 1);
            }
            else if( cmplowercase(line, "client plaintext auth") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[25]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[25]), 1);
            }
            else if( cmplowercase(line, "disable netbios") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[26]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[26]), 1);
            }
            else if( cmplowercase(line, "follow symlinks") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[27]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[27]), 1);
            }
            else if( cmplowercase(line, "update encrypted") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[28]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[28]), 1);
            }
            else if( cmplowercase(line, "pam password change") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[29]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[29]), 1);
            }
            else if( cmplowercase(line, "hostname lookups") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[30]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[30]), 1);
            }
            else if( cmplowercase(line, "winbind use default domain") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[31]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[31]), 1);
            }
            else if( cmplowercase(line, "winbind trusted domains only") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[32]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[32]), 1);
            }
            else if( cmplowercase(line, "winbind nested groups") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[33]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[33]), 1);
            }
            else if( cmplowercase(line, "winbind nss info") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[34]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[34]), 1);
            }
            else if( cmplowercase(line, "winbind refresh tickets") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[35]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[35]), 1);
            }
            else if( cmplowercase(line, "winbind offline logon") )
            {
                if( cmplowercase(line, "yes") )
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[36]), 0);
                else
                    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->server_set_combo[36]), 1);
            }
            else

                /* Spinbuttons... */


            if( cmplowercase(line, "max log size") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                if( chars_are_digits(new_buffer) )
                {
                    val = atoi(new_buffer);
                    gtk_spin_button_set_value(GTK_SPIN_BUTTON(widgets->server_set_spinbutton[0]),
                        val);
                }
            }
            else if( cmplowercase(line, "username level") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                if( chars_are_digits(new_buffer) )
                {
                    val = atoi(new_buffer);
                    gtk_spin_button_set_value(GTK_SPIN_BUTTON(widgets->server_set_spinbutton[1]),
                        val);
                }
            }
            else if( cmplowercase(line, "password level") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                if( chars_are_digits(new_buffer) )
                {
                    val = atoi(new_buffer);
                    gtk_spin_button_set_value(GTK_SPIN_BUTTON(widgets->server_set_spinbutton[2]),
                        val);
                }
            }
            else if( cmplowercase(line, "os level") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                if( chars_are_digits(new_buffer) )
                {
                    val = atoi(new_buffer);
                    gtk_spin_button_set_value(GTK_SPIN_BUTTON(widgets->server_set_spinbutton[3]),
                        val);
                }
            }
            else if( cmplowercase(line, "passwd chat timeout") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                if( chars_are_digits(new_buffer) )
                {
                    val = atoi(new_buffer);
                    gtk_spin_button_set_value(GTK_SPIN_BUTTON(widgets->server_set_spinbutton[4]),
                        val);
                }
            }
            else if( cmplowercase(line, "machine password timeout") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                if( chars_are_digits(new_buffer) )
                {
                    val = atoi(new_buffer);
                    gtk_spin_button_set_value(GTK_SPIN_BUTTON(widgets->server_set_spinbutton[5]),
                        val);
                }
            }
            else if( cmplowercase(line, "winbind cache time") )
            {
                opt_pos = get_option_pos(line);
                sprintf(new_buffer, "%s", &line[opt_pos]);
                if( new_buffer[strlen(new_buffer) - 1] == '\n'
                    || new_buffer[strlen(new_buffer) - 1] == '\r' )
                    new_buffer[strlen(new_buffer) - 1] = '\0';

                if( chars_are_digits(new_buffer) )
                {
                    val = atoi(new_buffer);
                    gtk_spin_button_set_value(GTK_SPIN_BUTTON(widgets->server_set_spinbutton[6]),
                        val);
                }
            }
        }
    fclose(fp);
    free(line);
    free(new_buffer);

    if(utf8 != NULL)
        g_free(utf8);

    gtk_widget_show_all(widgets->main_window);
}
