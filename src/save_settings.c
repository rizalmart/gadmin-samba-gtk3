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
#include "commands.h"
#include "allocate.h"
#include "save_settings.h"
#include "load_settings.h"
#include "standard_conf.h"



void save_settings(struct w *widgets)
{
    FILE *fp;
    long conf_size;
    char *line, *new_conf;
    gchar *settings_file, *setting;
    const gchar *set_smb_conf;

    /* APPCONFDIR is defined at compiletime */
    settings_file = g_strdup_printf("%s/settings.conf", APPCONFDIR);
    if((fp = fopen(settings_file, "r")) == NULL)
    {
        printf("Could not open the settings file: %s\n", settings_file);
        g_free(settings_file);
        return;
    }
    fseek(fp, 0, SEEK_END);
    conf_size = ftell(fp);
    rewind(fp);

    line = allocate(conf_size + 1);
    new_conf = allocate(conf_size + 1);

    /* Remove the old settings */
    if( conf_size > 1 )
        while(fgets(line, conf_size, fp) != NULL)
        {
            if( strlen(line) > 1000 )
                continue;

            if( strstr(line, "show_conf_question:") )
                continue;

            if( strstr(line, "start_winbindd:") )
                continue;

            if( strstr(line, "start_nmbd:") )
                continue;

            if( strstr(line, "smb_conf:") )
                continue;

            strcat(new_conf, line);
        }
    fclose(fp);
    free(line);


    set_smb_conf = gtk_entry_get_text(GTK_ENTRY(widgets->settings_entry[0]));

    /* Update global paths */


    /* Write the new settings */
    if((fp = fopen(settings_file, "w+")) == NULL)
    {
        printf(_("Couldnt open the settings file here:\n%s\n"), settings_file);
        g_free(settings_file);
        return;
    }
    fputs(new_conf, fp);

    if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets->settings_checkbutton[0])) )
        setting = g_strdup_printf("\nshow_conf_question: yes\n");
    else
        setting = g_strdup_printf("\nshow_conf_question: no\n");
    fputs(setting, fp);
    g_free(setting);

    if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets->settings_checkbutton[1])) )
        setting = g_strdup_printf("\nstart_winbindd: yes\n");
    else
        setting = g_strdup_printf("\nstart_winbindd: no\n");
    fputs(setting, fp);
    g_free(setting);

    if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets->settings_checkbutton[2])) )
        setting = g_strdup_printf("\nstart_nmbd: yes\n");
    else
        setting = g_strdup_printf("\nstart_nmbd: no\n");
    fputs(setting, fp);
    g_free(setting);

    if( strlen(set_smb_conf) > 5 )
    {
        setting = g_strdup_printf("smb_conf: %s\n", set_smb_conf);
        fputs(setting, fp);
        g_free(setting);
    }

    fclose(fp);
    g_free(settings_file);
    free(new_conf);

    /* Load the new settings */
    load_settings(widgets);

    /* If new configuration is selected, show standard conf question */
    if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets->settings_checkbutton[0])) )
    {
        create_standard_conf_question(widgets);
    }
}
