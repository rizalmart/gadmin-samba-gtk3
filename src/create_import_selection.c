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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "gettext.h"
#include "widgets.h"
#include "allocate.h"
#include "functions.h"
#include "show_info.h"
#include "import_functions.h"
#include "create_import_selection.h"

// Use global decl
#define NOBODY "nobody"

extern int global_import_local;
extern int global_import_remote;

extern int global_import_users;
extern int global_import_groups;

extern int global_import_valid;
extern int global_import_invalid;
extern int global_import_write;
extern int global_import_admin;


/* Destroy both import windows */
void import_cancel_button_clicked(GtkButton * button, struct w *widgets)
{
    if( ! GTK_IS_WIDGET(widgets->import_question_window) )
    {
        gtk_widget_destroy(widgets->import_window);
        return;
    }

    gtk_widget_destroy(widgets->import_question_window);
}


/* User and group import selector */
void create_import_selection(struct w *widgets)
{
    /* Create and populate the import selector */
    FILE *fp;
    long conf_size;
    char *old_buffer, *new_buffer, *groupnr, *group_name;
    int i;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    gchar *import_msg = NULL, *utf8 = NULL;
    GtkTreeViewColumn *user_col, *group_col;
    GtkCellRenderer *import_cell_renderer;
    GtkWidget *import_vbox;
    GtkWidget *import_label0;
    GtkWidget *import_scrolledwindow;
    GtkWidget *import_hbuttonbox;
    GtkWidget *import_button;
    GtkWidget *alignment1;
    GtkWidget *hbox2;
    GtkWidget *image1;
    GtkWidget *label2;
    GtkWidget *import_cancel_button;
    GtkWidget *alignment2;
    GtkWidget *hbox3;
    GtkWidget *image2;
    GtkWidget *label3;
    GtkTooltip *tooltips;

    /* Destroy the first question window */
    gtk_widget_destroy(widgets->import_question_window);

    //tooltips = gtk_tooltip_new();

    widgets->import_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(widgets->import_window, 560, 350);

    /* Set some descriptive texts in the window based on what was selected */
    if( global_import_local && global_import_users )
        import_msg = g_strdup_printf(_("Select any local users you want to import"));

    if( global_import_local && global_import_groups )
        import_msg = g_strdup_printf(_("Select any local groups you want to import"));

    if( global_import_remote && global_import_users )
        import_msg = g_strdup_printf(_("Select any remote users you want to import"));

    if( global_import_remote && global_import_groups )
        import_msg = g_strdup_printf(_("Select any remote groups you want to import"));

    utf8 = g_locale_to_utf8(import_msg, strlen(import_msg), NULL, NULL, NULL);
    g_free(import_msg);

    /* Set a variable window title */
    gtk_window_set_title(GTK_WINDOW(widgets->import_window), utf8);
    gtk_window_set_position(GTK_WINDOW(widgets->import_window), GTK_WIN_POS_CENTER);
    import_vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(widgets->import_window), import_vbox);

    import_label0 = gtk_label_new(utf8);


    gtk_box_pack_start(GTK_BOX(import_vbox), import_label0, FALSE, FALSE, 0);
    gtk_label_set_justify(GTK_LABEL(import_label0), GTK_JUSTIFY_LEFT);
    gtk_misc_set_padding(GTK_MISC(import_label0), 0, 5);

    import_scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(import_vbox), import_scrolledwindow, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(import_scrolledwindow), GTK_POLICY_AUTOMATIC,
        GTK_POLICY_AUTOMATIC);

    widgets->import_treeview = gtk_tree_view_new();
    gtk_container_add(GTK_CONTAINER(import_scrolledwindow), widgets->import_treeview);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(widgets->import_treeview), TRUE);

    widgets->import_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(widgets->import_treeview),
        GTK_TREE_MODEL(widgets->import_store));

    /* Set the column labels in the treeview */
    import_cell_renderer = gtk_cell_renderer_text_new();

    user_col = gtk_tree_view_column_new_with_attributes(_("Username"), import_cell_renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets->import_treeview),
        GTK_TREE_VIEW_COLUMN(user_col));

    group_col = gtk_tree_view_column_new_with_attributes(_("Group"), import_cell_renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets->import_treeview),
        GTK_TREE_VIEW_COLUMN(group_col));


    /* Selection is multiple */
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widgets->import_treeview));
    gtk_tree_selection_set_mode(GTK_TREE_SELECTION(selection), GTK_SELECTION_MULTIPLE);


    import_hbuttonbox = gtk_hbutton_box_new();
    gtk_box_pack_start(GTK_BOX(import_vbox), import_hbuttonbox, FALSE, FALSE, 0);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(import_hbuttonbox), GTK_BUTTONBOX_SPREAD);

    /* Import cancel button */
    import_cancel_button = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(import_hbuttonbox), import_cancel_button);
    //GTK_WIDGET_SET_FLAGS(import_cancel_button, GTK_CAN_DEFAULT);
    gtk_widget_set_can_default(import_cancel_button, TRUE);

    alignment2 = gtk_alignment_new(0.5, 0.5, 0, 0);
    gtk_container_add(GTK_CONTAINER(import_cancel_button), alignment2);

    hbox3 = gtk_hbox_new(FALSE, 2);
    gtk_container_add(GTK_CONTAINER(alignment2), hbox3);

    image2 = gtk_image_new_from_stock("gtk-cancel", GTK_ICON_SIZE_BUTTON);
    gtk_box_pack_start(GTK_BOX(hbox3), image2, FALSE, FALSE, 0);

    label3 = gtk_label_new_with_mnemonic(_("Cancel"));
    gtk_box_pack_start(GTK_BOX(hbox3), label3, FALSE, FALSE, 0);
    gtk_label_set_justify(GTK_LABEL(label3), GTK_JUSTIFY_LEFT);


    /* The import button */
    import_button = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(import_hbuttonbox), import_button);
    //GTK_WIDGET_SET_FLAGS(import_button, GTK_CAN_DEFAULT);
    gtk_widget_set_can_default(import_button, TRUE);

    alignment1 = gtk_alignment_new(0.5, 0.5, 0, 0);
    gtk_container_add(GTK_CONTAINER(import_button), alignment1);

    hbox2 = gtk_hbox_new(FALSE, 2);
    gtk_container_add(GTK_CONTAINER(alignment1), hbox2);

    image1 = gtk_image_new_from_stock("gtk-yes", GTK_ICON_SIZE_BUTTON);
    gtk_box_pack_start(GTK_BOX(hbox2), image1, FALSE, FALSE, 0);


    /* Variable import button text */
    if( global_import_users )
        import_msg = g_strdup_printf("Import users");
    else
        import_msg = g_strdup_printf("Import groups");

    utf8 = g_locale_to_utf8(import_msg, strlen(import_msg), NULL, NULL, NULL);
    g_free(import_msg);

    label2 = gtk_label_new_with_mnemonic(utf8);

    gtk_box_pack_start(GTK_BOX(hbox2), label2, FALSE, FALSE, 0);
    gtk_label_set_justify(GTK_LABEL(label2), GTK_JUSTIFY_LEFT);

    /* Callback is located in import_functions.c */
    g_signal_connect((gpointer) import_button, "clicked",
        G_CALLBACK(import_button_clicked), widgets);

    /* Callback is located in this file */
    g_signal_connect((gpointer) import_cancel_button, "clicked",
        G_CALLBACK(import_cancel_button_clicked), widgets);

    gtk_widget_show_all(widgets->import_window);



    /* Populate the import treeview with LOCAL users and groups except machine accounts */
    if( global_import_local )
    {
        /* First add macros for all users and groups.
           FIX: Make translatable after modifying the recieving functions. */
        gtk_list_store_append(GTK_LIST_STORE(widgets->import_store), &iter);
        import_msg = g_strdup_printf("All users");
        utf8 = g_locale_to_utf8(import_msg, strlen(import_msg), NULL, NULL, NULL);
        gtk_list_store_set(GTK_LIST_STORE(widgets->import_store), &iter, 0, utf8, -1);
        g_free(import_msg);
        import_msg = g_strdup_printf("All groups");
        utf8 = g_locale_to_utf8(import_msg, strlen(import_msg), NULL, NULL, NULL);
        gtk_list_store_set(GTK_LIST_STORE(widgets->import_store), &iter, 1, utf8, -1);
        g_free(import_msg);


        if((fp = fopen(PASSWD_FILE, "r")) == NULL)
        {
            printf("Cant open passwd here:\n%s\n", PASSWD_FILE);
            return;
        }

        fseek(fp, 0, SEEK_END);
        conf_size = ftell(fp);
        rewind(fp);

        old_buffer = allocate(conf_size);
        new_buffer = allocate(8192);

        if( conf_size > 1 )
            while(fgets(old_buffer, conf_size, fp) != NULL)
            {
                if( strlen(old_buffer) > 10 && strlen(old_buffer) < 4000 )
                {
                    for(i = 0; old_buffer[i] != '\0'; i++)
                        if(old_buffer[i] == ':')
                            break;

                    strcpy(new_buffer, old_buffer);
                    new_buffer[i] = '\0';

                    /* Dont insert root or the user the server runs as */
                    if( ! strcmp(new_buffer, "root") || ! strcmp(new_buffer, NOBODY) )
                        continue;

                    /* Dont insert any machine account groups */
                    if( strstr(new_buffer, "$") )
                        continue;

                    gtk_list_store_append(GTK_LIST_STORE(widgets->import_store), &iter);
                    utf8 = g_locale_to_utf8(new_buffer, strlen(new_buffer), NULL, NULL, NULL);
                    gtk_list_store_set(GTK_LIST_STORE(widgets->import_store), &iter, 0, utf8, -1);

                    groupnr = get_user_setting(new_buffer, "group");
                    group_name = get_group_name(groupnr);
                    free(groupnr);

                    utf8 = g_locale_to_utf8(group_name, strlen(group_name), NULL, NULL, NULL);
                    gtk_list_store_set(GTK_LIST_STORE(widgets->import_store), &iter, 1, utf8, -1);

                    free(group_name);
                }
            }
        fclose(fp);
        free(old_buffer);
        free(new_buffer);
    }


    /* Populate the import treeview with REMOTE users and groups */
    if( global_import_remote )
    {
        import_msg = g_strdup_printf("Remote population is incomplete.\n");
        show_info(import_msg);
        g_free(import_msg);
    }


    if( utf8 != NULL )
        g_free(utf8);
}
