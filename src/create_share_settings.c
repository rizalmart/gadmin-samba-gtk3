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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "gettext.h"
#include "widgets.h"
#include "make_settings_entries.h"
#include "make_settings_buttons.h"
#include "make_settings_combos.h"
#include "make_settings_spinbuttons.h"
#include "make_settings_checkbuttons.h"
#include "delete_share.h"
#include "add_share.h"
#include "create_import_question.h"
#include "apply_share_settings.h"
#include "file_chooser.h"
#include "show_info.h"

int new_share_info_shown = 0;

extern int global_share_access_treeview_num;
GtkTreeIter global_access_iter;


void share_access_treeview_row_clicked(GtkTreeView * treeview, GdkEventButton * event, gpointer data)
{
    GtkTreePath *path;
    GtkTreeModel *list_store;
    GtkTreeSelection *selection;

    /* The left button is pressed */
    if( ! event->type == GDK_BUTTON_PRESS || !event->button == 1 )
        return;

    /* Get the number for the treeview (0-3) */
    global_share_access_treeview_num = (int)g_object_get_data(G_OBJECT(treeview), "column");

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

    if( gtk_tree_selection_count_selected_rows(selection) <= 1 )
    {
        /* Get the treepath for the row that was clicked */
        if( gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview), event->x, event->y,
                &path, NULL, NULL, NULL) )
        {

            gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview), event->x, event->y,
                &path, NULL, NULL, NULL);

            gtk_tree_selection_unselect_all(selection);
            gtk_tree_selection_select_path(selection, path);

            list_store = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
            gtk_tree_model_get_iter(list_store, &global_access_iter, path);
            gtk_tree_path_free(path);
        }
    }
}


void show_share_selection_info()
{
    gchar *info;
    info = g_strdup_printf("Select a user or group to remove access for first.");
    show_info(info);
    g_free(info);
}


void new_share_clicked(GtkButton * button, struct w *widgets)
{
    gchar *info;
    int i = 0;

    /* Clear all share entries */
    for(i = 0; i < 7; i++)
        gtk_entry_set_text(GTK_ENTRY(widgets->share_set_entry[i]), "");

    /* Set all share combos to commonly used settings. */
    for(i = 0; i < 8; i++)
    {
        /* Set guest ok/require password to yes */
        if( i == 4 )
        {
            gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->share_set_combo[i]), 0);
            continue;
        }
    
        /* Skip removed combo 5 */
        if( i == 5 )
           continue;

        /* Set all others to No */
        gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->share_set_combo[i]), 1);
    }

    /* Set combo 1, 2 and 5 to yes. (Available, browseable and guest ok/require password) */
    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->share_set_combo[1]), 0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->share_set_combo[2]), 0);

    /* Clear the user access treeviews */
    gtk_list_store_clear(widgets->valid_users_store);
    gtk_list_store_clear(widgets->invalid_users_store);
    gtk_list_store_clear(widgets->write_users_store);
    gtk_list_store_clear(widgets->admin_users_store);

    /* Only show profiling info once */
    if( new_share_info_shown )
        return;
    else
        new_share_info_shown = 1;

    info =
        g_strconcat(_("Write the new share name and other settings in the share settings below.\n"),
        _("Press add to add this new share with the selected settings.\n\n"),
        _("You can also skip this step and directly alter an existing shares name,\n"),
        _("settings and then press add. This can save administration time.\n"), NULL);
    show_info(info);
    g_free(info);
}


void del_share_access_clicked(GtkButton * button, struct w *widgets)
{
    GtkTreeSelection *selection;
    gboolean edited = 0;

    if( global_share_access_treeview_num == 0 )
    {
        selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widgets->valid_users_treeview));
        if( gtk_tree_selection_count_selected_rows(selection) < 1 )
        {
            show_share_selection_info();
            return;
        }
        edited = gtk_list_store_remove(GTK_LIST_STORE(widgets->valid_users_store), &global_access_iter);
        selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widgets->valid_users_treeview));
        gtk_tree_selection_unselect_all(selection);
    }

    if( global_share_access_treeview_num == 1 )
    {
        selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widgets->invalid_users_treeview));
        if( gtk_tree_selection_count_selected_rows(selection) < 1 )
        {
            show_share_selection_info();
            return;
        }
        edited = gtk_list_store_remove(GTK_LIST_STORE(widgets->invalid_users_store), &global_access_iter);
        selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widgets->invalid_users_treeview));
        gtk_tree_selection_unselect_all(selection);
    }

    if( global_share_access_treeview_num == 2 )
    {
        selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widgets->write_users_treeview));
        if( gtk_tree_selection_count_selected_rows(selection) < 1 )
        {
            show_share_selection_info();
            return;
        }
        edited = gtk_list_store_remove(GTK_LIST_STORE(widgets->write_users_store), &global_access_iter);
        selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widgets->write_users_treeview));
        gtk_tree_selection_unselect_all(selection);
    }

    if( global_share_access_treeview_num == 3 )
    {
        selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widgets->admin_users_treeview));
        if( gtk_tree_selection_count_selected_rows(selection) < 1 )
        {
            show_share_selection_info();
            return;
        }
        edited = gtk_list_store_remove(GTK_LIST_STORE(widgets->admin_users_store), &global_access_iter);
        selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widgets->admin_users_treeview));
        gtk_tree_selection_unselect_all(selection);
    }

    apply_share_settings(widgets);
}


void select_share_path_clicked(struct w *widgets)
{
    gchar *path = NULL, *sharesdir = NULL;

    sharesdir = g_strdup_printf("%s/lib/samba", LOCALSTATEDIR);

    path = get_dialog_path_selection("DIR", sharesdir, "None");
    if( path != NULL )
    {
        printf("Selected directory: %s\n", path);
        gtk_entry_set_text(GTK_ENTRY(widgets->share_set_entry[1]), path);
        g_free(path);
    }

    g_free(sharesdir);
}


void create_share_settings(struct w *widgets)
{
    GtkTooltip *tooltips;
    gchar *combo_text = NULL;
    gchar *utf8 = NULL;

    GtkWidget *hbox;
    GtkWidget *delete_share_button;
    GtkWidget *add_share_button;
    GtkWidget *apply_share_button;

    GtkWidget *add_access_button;
    GtkWidget *add_access_hbox;
    GtkWidget *add_access_image;
    GtkWidget *add_access_label;
    GtkWidget *add_access_alignment;

    GtkWidget *del_access_button;
    GtkWidget *del_access_hbox;
    GtkWidget *del_access_image;
    GtkWidget *del_access_label;
    GtkWidget *del_access_alignment;

    GtkWidget *new_share_button;
    GtkWidget *new_share_hbox;
    GtkWidget *new_share_image;
    GtkWidget *new_share_label;
    GtkWidget *new_share_alignment;

    int a = 0;
    int b = 1;
    int ent = 0;
    int comb = 0;

    //tooltips = gtk_tooltip_new();
    hbox = gtk_hbox_new(FALSE, 3);

    /* The delete add share and apply share settings buttons */
    delete_share_button = gtk_button_new_from_stock(GTK_STOCK_DELETE);
    add_share_button = gtk_button_new_from_stock(GTK_STOCK_ADD);
    apply_share_button = gtk_button_new_from_stock(GTK_STOCK_APPLY);
    /* Custom "New share" button */
    new_share_button = gtk_button_new();
    new_share_alignment = gtk_alignment_new(0.5, 0.5, 0, 0);
    gtk_container_add(GTK_CONTAINER(new_share_button), new_share_alignment);
    new_share_hbox = gtk_hbox_new(FALSE, 2);
    gtk_container_add(GTK_CONTAINER(new_share_alignment), new_share_hbox);
    new_share_image = gtk_image_new_from_stock("gtk-add", GTK_ICON_SIZE_BUTTON);
    gtk_box_pack_start(GTK_BOX(new_share_hbox), new_share_image, FALSE, FALSE, 0);
    new_share_label = gtk_label_new_with_mnemonic(_("New share"));
    gtk_box_pack_start(GTK_BOX(new_share_hbox), new_share_label, FALSE, FALSE, 0);
    gtk_label_set_justify(GTK_LABEL(new_share_label), GTK_JUSTIFY_LEFT);

    /* Add the buttons to the hbox */
    gtk_box_pack_start(GTK_BOX(hbox), delete_share_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), new_share_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), add_share_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), apply_share_button, FALSE, FALSE, 0);

    /* table, left, right, top, bottom, fill, expand, length, xpad, ypad */
    gtk_table_attach(GTK_TABLE(widgets->share_set_table), hbox, 1, 2, 0, 1, 0, 0, 5, 5);
    a++;
    b++;

    g_signal_connect_swapped(G_OBJECT(delete_share_button), "clicked",
        G_CALLBACK(delete_share), widgets);

    g_signal_connect((gpointer) new_share_button, "clicked",
        G_CALLBACK(new_share_clicked), widgets);

    g_signal_connect_swapped(G_OBJECT(add_share_button), "clicked", G_CALLBACK(add_share), widgets);

    g_signal_connect_swapped(G_OBJECT(apply_share_button), "clicked",
        G_CALLBACK(apply_share_settings), widgets);



    /* Sharename *//* Max length and input 350 chars */
    widgets->share_set_entry[ent] =
        make_entry_with_label(GTK_TABLE(widgets->share_set_table), _(" Share name: "), 0, 1, a, b, 350);
    //gtk_tooltips_set_tip(tooltips, widgets->share_set_entry[ent], _("The name of the Share"), NULL);
    gtk_widget_set_tooltip_text(widgets->share_set_entry[ent], _("The name of the Share"));
    
    a++;
    b++;
    ent++;

    /* Share path */
    GtkWidget *sharepath_button;
    widgets->share_set_entry[ent] = gtk_entry_new();
    sharepath_button = make_button_with_entry(GTK_TABLE(widgets->share_set_table),
        widgets->share_set_entry[ent],
        "gtk-refresh", _("Shared directory:"), _("Select a directory to share"), 0, 1, a, b, 350);
    a++;
    b++;
    ent++;

    g_signal_connect_swapped(G_OBJECT(sharepath_button), "clicked",
        G_CALLBACK(select_share_path_clicked), widgets);


    /* A comment */
    widgets->share_set_entry[ent] =
        make_entry_with_label(GTK_TABLE(widgets->share_set_table), _(" Comment: "), 0, 1, a, b, 350);
    //gtk_tooltips_set_tip(tooltips, widgets->share_set_entry[ent], "A comment for this share", NULL);
    gtk_widget_set_tooltip_text(widgets->share_set_entry[ent], "A comment for this share");
    a++;
    b++;
    ent++;


    /* Add a description label for the treeviews */
    GtkWidget *access_label1, *access_label2;
    access_label1 = gtk_label_new(NULL);
    access_label2 = gtk_label_new(NULL);
    /* Add a spacer label to the table */
    gtk_label_set_text(GTK_LABEL(access_label1), "");
    gtk_table_attach(GTK_TABLE(widgets->share_set_table), access_label1, 0, 2, a, b, -1, 1, 1, 0);
    a++;
    b++;
    gtk_label_set_text(GTK_LABEL(access_label2),
        _("Access permissions for users and groups in the selected share:"));
    gtk_table_attach(GTK_TABLE(widgets->share_set_table), access_label2, 0, 2, a, b, -1, 1, 1, 0);
    a++;
    b++;

    GtkWidget *access_hseparator = gtk_hseparator_new();
    gtk_table_attach(GTK_TABLE(widgets->share_set_table), access_hseparator, 0, 2, a, b, -1, 1, 1, 0);
    a++;
    b++;

    /* Create a hbox that holds the user access scrolled windows and treeviews */
    GtkWidget *user_access_hbox1, *user_access_hbox2;

    /* Valid users treeview (Access allowed) */
    GtkCellRenderer *valid_users_cell_renderer;
    GtkWidget *valid_users_scrolled_window;

    /* Invalid users treeview (Access denied) */
    GtkCellRenderer *invalid_users_cell_renderer;
    GtkWidget *invalid_users_scrolled_window;

    /* Write list treeview (Write access) */
    GtkCellRenderer *write_users_cell_renderer;
    GtkWidget *write_users_scrolled_window;

    /* Admin users treeview (Administrators) */
    GtkCellRenderer *admin_users_cell_renderer;
    GtkWidget *admin_users_scrolled_window;

    /* Create and add 2 hboxes to the table */
    user_access_hbox1 = gtk_hbox_new(TRUE, 0);
    user_access_hbox2 = gtk_hbox_new(TRUE, 0);
    /* table,    left, right, top, bottom, fill, expand, length, xpad, ypad */
    gtk_table_attach(GTK_TABLE(widgets->share_set_table), user_access_hbox1, 0, 2, a, b, -1, 1, 20, 10);
    a++;
    b++;
    gtk_table_attach(GTK_TABLE(widgets->share_set_table), user_access_hbox2, 0, 2, a, b, -1, 1, 20, 10);
    a++;
    b++;


    /* Create and add the valid users treeview to hbox1 */
    valid_users_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(user_access_hbox1), valid_users_scrolled_window, TRUE, TRUE, 0);

    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(valid_users_scrolled_window),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    /* Must set a larger size or it wont scroll */
    gtk_widget_set_size_request(valid_users_scrolled_window, -1, 120);

    widgets->valid_users_store = gtk_list_store_new(1, G_TYPE_STRING);
    widgets->valid_users_treeview = gtk_tree_view_new();
    gtk_tree_view_set_model(GTK_TREE_VIEW(widgets->valid_users_treeview),
        GTK_TREE_MODEL(widgets->valid_users_store));
    gtk_container_add(GTK_CONTAINER(valid_users_scrolled_window), widgets->valid_users_treeview);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(widgets->valid_users_treeview), TRUE);
    /* Set the column labels in the treeview */
    valid_users_cell_renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *valid_users_col =
        gtk_tree_view_column_new_with_attributes(_("Access allowed"), valid_users_cell_renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets->valid_users_treeview), GTK_TREE_VIEW_COLUMN(valid_users_col));

    /* Set a number reference for the treeview to indicate what treeview was clicked */
    g_object_set_data(G_OBJECT(widgets->valid_users_treeview), "column", (void *)(0));

    g_signal_connect((gpointer) widgets->valid_users_treeview, "button_press_event",
        G_CALLBACK(share_access_treeview_row_clicked), widgets);


    /* Create and add invalid users treeview to hbox1 */
    invalid_users_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(user_access_hbox1), invalid_users_scrolled_window, TRUE, TRUE, 0);

    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(invalid_users_scrolled_window),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    /* Must set a larger size or it wont scroll */
    gtk_widget_set_size_request(invalid_users_scrolled_window, -1, 120);

    widgets->invalid_users_store = gtk_list_store_new(1, G_TYPE_STRING);
    widgets->invalid_users_treeview = gtk_tree_view_new();
    gtk_tree_view_set_model(GTK_TREE_VIEW(widgets->invalid_users_treeview),
        GTK_TREE_MODEL(widgets->invalid_users_store));
    gtk_container_add(GTK_CONTAINER(invalid_users_scrolled_window),
        widgets->invalid_users_treeview);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(widgets->invalid_users_treeview), TRUE);
    /* Set the column labels in the treeview */
    invalid_users_cell_renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *invalid_users_col =
        gtk_tree_view_column_new_with_attributes(_("Access denied"), invalid_users_cell_renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets->invalid_users_treeview),
        GTK_TREE_VIEW_COLUMN(invalid_users_col));

    //gtk_tooltips_set_tip(tooltips, widgets->invalid_users_treeview,
    //    _("If a group has been allowed access you can deny access for some of the users in this group here"),
    //    NULL);
    
    gtk_widget_set_tooltip_text(widgets->invalid_users_treeview, _("If a group has been allowed access you can deny access for some of the users in this group here"));    

    /* Set a number reference for the treeview to indicate what treeview was clicked */
    g_object_set_data(G_OBJECT(widgets->invalid_users_treeview), "column", (void *)(1));

    g_signal_connect((gpointer) widgets->invalid_users_treeview, "button_press_event",
        G_CALLBACK(share_access_treeview_row_clicked), widgets);



    /* Create and add write list users treeview to hbox2 */
    write_users_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(user_access_hbox2), write_users_scrolled_window, TRUE, TRUE, 0);

    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(write_users_scrolled_window),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    /* Must set a larger size or it wont scroll */
    gtk_widget_set_size_request(write_users_scrolled_window, -1, 120);

    widgets->write_users_store = gtk_list_store_new(1, G_TYPE_STRING);
    widgets->write_users_treeview = gtk_tree_view_new();
    gtk_tree_view_set_model(GTK_TREE_VIEW(widgets->write_users_treeview),
        GTK_TREE_MODEL(widgets->write_users_store));
    gtk_container_add(GTK_CONTAINER(write_users_scrolled_window), widgets->write_users_treeview);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(widgets->write_users_treeview), TRUE);
    /* Set the column labels in the treeview */
    write_users_cell_renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *write_users_col =
        gtk_tree_view_column_new_with_attributes(_("Write access"), write_users_cell_renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets->write_users_treeview),
        GTK_TREE_VIEW_COLUMN(write_users_col));

    /* Set a number reference for the treeview to indicate what treeview was clicked */
    g_object_set_data(G_OBJECT(widgets->write_users_treeview), "column", (void *)(2));

    g_signal_connect((gpointer) widgets->write_users_treeview, "button_press_event",
        G_CALLBACK(share_access_treeview_row_clicked), widgets);

    /* Create and add admin users treeview to hbox2 */
    admin_users_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(user_access_hbox2), admin_users_scrolled_window, TRUE, TRUE, 0);

    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(admin_users_scrolled_window),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    /* Must set a larger size or it wont scroll */
    gtk_widget_set_size_request(admin_users_scrolled_window, -1, 120);

    widgets->admin_users_store = gtk_list_store_new(1, G_TYPE_STRING);
    widgets->admin_users_treeview = gtk_tree_view_new();
    gtk_tree_view_set_model(GTK_TREE_VIEW(widgets->admin_users_treeview),
        GTK_TREE_MODEL(widgets->admin_users_store));
    gtk_container_add(GTK_CONTAINER(admin_users_scrolled_window), widgets->admin_users_treeview);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(widgets->admin_users_treeview), TRUE);
    /* Set the column labels in the treeview */
    admin_users_cell_renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *admin_users_col =
        gtk_tree_view_column_new_with_attributes(_("Administrators"), admin_users_cell_renderer,
        "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets->admin_users_treeview),
        GTK_TREE_VIEW_COLUMN(admin_users_col));

    /* Set a number reference for the treeview to indicate what treeview was clicked */
    g_object_set_data(G_OBJECT(widgets->admin_users_treeview), "column", (void *)(3));

    g_signal_connect((gpointer) widgets->admin_users_treeview, "button_press_event",
        G_CALLBACK(share_access_treeview_row_clicked), widgets);

    a++;
    b++;


    GtkWidget *access_hbuttonbox = gtk_hbutton_box_new();

    /* Custom "Delete access for users and groups" button */
    del_access_button = gtk_button_new();
    del_access_alignment = gtk_alignment_new(0.5, 0.5, 0, 0);
    gtk_container_add(GTK_CONTAINER(del_access_button), del_access_alignment);

    del_access_hbox = gtk_hbox_new(FALSE, 2);
    gtk_container_add(GTK_CONTAINER(del_access_alignment), del_access_hbox);

    del_access_image = gtk_image_new_from_stock("gtk-remove", GTK_ICON_SIZE_BUTTON);
    gtk_box_pack_start(GTK_BOX(del_access_hbox), del_access_image, FALSE, FALSE, 0);

    del_access_label = gtk_label_new_with_mnemonic(_("Delete access permission"));
    gtk_box_pack_start(GTK_BOX(del_access_hbox), del_access_label, FALSE, FALSE, 0);
    gtk_label_set_justify(GTK_LABEL(del_access_label), GTK_JUSTIFY_LEFT);

    /* Custom "Add access for users and groups" button */
    add_access_button = gtk_button_new();
    add_access_alignment = gtk_alignment_new(0.5, 0.5, 0, 0);
    gtk_container_add(GTK_CONTAINER(add_access_button), add_access_alignment);

    add_access_hbox = gtk_hbox_new(FALSE, 2);
    gtk_container_add(GTK_CONTAINER(add_access_alignment), add_access_hbox);

    add_access_image = gtk_image_new_from_stock("gtk-add", GTK_ICON_SIZE_BUTTON);
    gtk_box_pack_start(GTK_BOX(add_access_hbox), add_access_image, FALSE, FALSE, 0);

    add_access_label = gtk_label_new_with_mnemonic(_("Add access permissions"));
    gtk_box_pack_start(GTK_BOX(add_access_hbox), add_access_label, FALSE, FALSE, 0);
    gtk_label_set_justify(GTK_LABEL(add_access_label), GTK_JUSTIFY_LEFT);


    /* Add the buttons to the table */
    gtk_box_pack_start(GTK_BOX(access_hbuttonbox), del_access_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(access_hbuttonbox), add_access_button, FALSE, FALSE, 0);
    gtk_widget_set_size_request(access_hbuttonbox, -1, 40);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(access_hbuttonbox), GTK_BUTTONBOX_SPREAD);
    gtk_table_attach(GTK_TABLE(widgets->share_set_table), access_hbuttonbox, 0, 2, a, b, -1, 1, 20, 10);

    g_signal_connect((gpointer) del_access_button, "clicked",
        G_CALLBACK(del_share_access_clicked), widgets);

    g_signal_connect_swapped(G_OBJECT(add_access_button), "clicked",
        G_CALLBACK(create_import_question), widgets);
    a++;
    b++;

    /* Directory mask */
    widgets->share_set_entry[ent] =
        make_entry_with_label(GTK_TABLE(widgets->share_set_table), _(" Directory mask: "), 0, 1, a, b, 350);
    //gtk_tooltips_set_tip(tooltips, widgets->share_set_entry[ent],
    //    "A Directory mask like 0644 or 0755", NULL);
    gtk_widget_set_tooltip_text(widgets->share_set_entry[ent], "A Directory mask like 0644 or 0755");    
        
    a++;
    b++;
    ent++;

    /* Create mode */
    widgets->share_set_entry[ent] =
        make_entry_with_label(GTK_TABLE(widgets->share_set_table), _(" Create mode: "), 0, 1, a, b, 350);
    //gtk_tooltips_set_tip(tooltips, widgets->share_set_entry[ent],
    //    "A creation mode like: 0644 or 0777", NULL);
    gtk_widget_set_tooltip_text(widgets->share_set_entry[ent], "A creation mode like: 0644 or 0777");    
    
    a++;
    b++;
    ent++;

    /* Force user */
    widgets->share_set_entry[ent] =
        make_entry_with_label(GTK_TABLE(widgets->share_set_table), _(" Force user: "), 0, 1, a, b, 350);
    //gtk_tooltips_set_tip(tooltips, widgets->share_set_entry[ent],
    //    "All files and directories written to this share are owned by this user", NULL);
    gtk_widget_set_tooltip_text(widgets->share_set_entry[ent], "All files and directories written to this share are owned by this user");       
    a++;
    b++;
    ent++;

    /* Force group */
    widgets->share_set_entry[ent] =
        make_entry_with_label(GTK_TABLE(widgets->share_set_table), _(" Force group: "), 0, 1, a, b, 350);
    //gtk_tooltips_set_tip(tooltips, widgets->share_set_entry[ent],
    //    "All files and directories written to this share are owned by this group", NULL);
    gtk_widget_set_tooltip_text(widgets->share_set_entry[ent], "All files and directories written to this share are owned by this group");       
    
    a++;
    b++;
    ent++;

    /* read only combo */
    widgets->share_set_combo[comb] =
        make_label_combo_label(GTK_TABLE(widgets->share_set_table), _(" Read only: "), 0, 1, a, b, 80);
        
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX(widgets->share_set_combo[comb]), utf8);
    
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX(widgets->share_set_combo[comb]), utf8);
    
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* available combo */
    widgets->share_set_combo[comb] =
        make_label_combo_label(GTK_TABLE(widgets->share_set_table), _(" Available: "), 0, 1, a, b, 80);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX(widgets->share_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX(widgets->share_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* browseable combo */
    widgets->share_set_combo[comb] =
        make_label_combo_label(GTK_TABLE(widgets->share_set_table), _(" Browseable: "), 0, 1, a, b, 80);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX(widgets->share_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX(widgets->share_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* writable combo */
    widgets->share_set_combo[comb] =
        make_label_combo_label(GTK_TABLE(widgets->share_set_table), _(" Writable: "), 0, 1, a, b, 80);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX(widgets->share_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX(widgets->share_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* guest ok / require password combo */
    widgets->share_set_combo[comb] =
        make_label_combo_label(GTK_TABLE(widgets->share_set_table), _(" Require password: "), 0, 1, a, b, 80);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX(widgets->share_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX(widgets->share_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;


    comb++; // Let this be for now.


    /* printable combo */
    widgets->share_set_combo[comb] =
        make_label_combo_label(GTK_TABLE(widgets->share_set_table), _(" Printable: "), 0, 1, a, b, 80);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX(widgets->share_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX(widgets->share_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* File locking combo */
    widgets->share_set_combo[comb] =
        make_label_combo_label(GTK_TABLE(widgets->share_set_table), _(" File locking: "), 0, 1, a, b, 80);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX(widgets->share_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX(widgets->share_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Strict file locking */
    widgets->share_set_combo[comb] =
        make_label_combo_label(GTK_TABLE(widgets->share_set_table), _(" Strict locking: "), 0, 1, a, b, 80);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX(widgets->share_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX(widgets->share_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    gtk_widget_show_all(widgets->main_window);
}
