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
#include "populate_shell_combo.h"

gboolean item_shell_exists(GtkComboBox *combo_box, const gchar *text) {
    GtkTreeIter iter;
    GtkListStore *store;
    
    store = GTK_LIST_STORE(gtk_combo_box_get_model(combo_box));

    if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter)) {
        do {
            gchar *entry_text;
            gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, 0, &entry_text, -1);
            if (g_strcmp0(entry_text, text) == 0) {
                g_free(entry_text);
                return TRUE; // Item exists
            }
            g_free(entry_text);
        } while (gtk_tree_model_iter_next(GTK_TREE_MODEL(store), &iter));
    }

    return FALSE; // Item does not exist
}


void populate_shell_combo(GtkWidget * shell_combo)
{
    FILE *fp;
    long conf_size;
    int shell_found = 0;
    char *line, *new_buf;
    gchar *utf8 = NULL;
    gchar *combo_text = NULL;
    gchar *info;

    /* Append a /dev/null shell first and select it */
    combo_text = g_strdup_printf("/dev/null");
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(shell_combo), utf8);
    gtk_combo_box_set_active(GTK_COMBO_BOX(shell_combo), 0);
    g_free(combo_text);

    if((fp = fopen(SHELLS_FILE, "r")) == NULL)
    {
        info = g_strdup_printf(_("Cant find the shells file here:\n%s\n"), SHELLS_FILE);
        show_info(info);
        g_free(info);
        return;
    }
    fseek(fp, 0, SEEK_END);
    conf_size = ftell(fp);
    rewind(fp);

    line = allocate(conf_size + 2);
    new_buf = allocate(conf_size + 2);

    if( conf_size > 1 )
        while(fgets(line, conf_size, fp) != NULL)
        {
            /* Append all other shells but not /dev/null */
            if( strlen(line) > 3 && !strstr(line, "#") && !strstr(line, "/dev/null") )
            {
                shell_found = 1;
                strcpy(new_buf, line);
                utf8 = g_locale_to_utf8(new_buf, strlen(new_buf) - 1, NULL, NULL, NULL);
                if(item_shell_exists(GTK_COMBO_BOX(shell_combo),utf8)==FALSE){
					gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(shell_combo), utf8);
				}
            }
        }
 
    fclose(fp);
    free(line);
    free(new_buf);

    if(utf8 != NULL)
        g_free(utf8);
}
