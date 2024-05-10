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
#include <string.h>


gchar * get_dialog_path_selection(gchar * type, gchar * initial_dir, gchar * initial_file)
{
    GtkWidget *dialog;
    gboolean ret = 0;
    int dlg_ret = 0;
    gchar *path = NULL;

    if( strstr(type, "DIR") )
    {
        dialog = gtk_file_chooser_dialog_new("Select directory", NULL,
            GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);

        /* Set initial directory. If it doesnt exist "/" is used. */
        ret = gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), initial_dir);
        if( ! ret )
            ret = gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), "/");
    }
    else 
    if( strstr(type, "FILE") )
    {
        dialog = gtk_file_chooser_dialog_new("Select file", NULL, GTK_FILE_CHOOSER_ACTION_OPEN,
//  GTK_FILE_CHOOSER_ACTION_SAVE,
            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);

        /* Set initial directory. If it doesnt exist "/" is used. */
        ret = gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), initial_dir);
        if( ! ret )
            ret = gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), "/");

        /* It complains if the file doesnt exist, ignore it. */
        gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), initial_file);
    }
    else
    {
        printf("Path selection: Input TYPE error.\n");
        return NULL;
    }

    dlg_ret = gtk_dialog_run(GTK_DIALOG(dialog));

    if( dlg_ret == GTK_RESPONSE_ACCEPT )
    {
        path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    }

    if( dlg_ret == GTK_RESPONSE_CANCEL )
    {
        printf("Path selection: Canceled.\n");
        path = NULL;
    }

    gtk_widget_destroy(dialog);

    return path;
}
