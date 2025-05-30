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
#include <stdio.h>
#include <string.h>
#include "widgets.h"
#include "gettext.h"
#include "create_security_tab.h"
#include "populate_security_tab.h"
#include "clear_security_tab.h"


/* Wether or not to let the security section expand */
#define EXPAND_SECURITY_SECTION TRUE



void create_security_tab(struct w *widgets)
{
    GtkCellRenderer *security_cell_renderer;
    GtkWidget *security_treeview_hbox;
    GtkWidget *security_scrolled_window;
    gchar *utf8 = NULL;

    GtkTooltip *tooltips;
    //tooltips = gtk_tooltip_new();

    /* Create the security treeview in a scrolled window */
    //security_treeview_hbox = gtk_hbox_new(TRUE, 0);
    security_treeview_hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(security_treeview_hbox),TRUE);
    
    gtk_box_pack_start(GTK_BOX(widgets->notebook_vbox8), security_treeview_hbox,
        EXPAND_SECURITY_SECTION, TRUE, 0);

    security_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(security_treeview_hbox), security_scrolled_window, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(security_scrolled_window),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

    widgets->security_store = gtk_list_store_new(6, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
        G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    widgets->security_treeview = gtk_tree_view_new();
    gtk_tree_view_set_model(GTK_TREE_VIEW(widgets->security_treeview),
        GTK_TREE_MODEL(widgets->security_store));

    gtk_container_add(GTK_CONTAINER(security_scrolled_window), widgets->security_treeview);
    //gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(widgets->security_treeview), TRUE);

    /* Set the column labels in the treeview */
    security_cell_renderer = gtk_cell_renderer_text_new();

    GtkTreeViewColumn *username_col =
        gtk_tree_view_column_new_with_attributes(_("Security information"), security_cell_renderer,
        "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets->security_treeview),
        GTK_TREE_VIEW_COLUMN(username_col));


    /* The update file button */
    GtkWidget *security_button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(widgets->notebook_vbox8), security_button_box, FALSE, FALSE, 0);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(security_button_box), GTK_BUTTONBOX_SPREAD);

    //GtkWidget *clear_security_button = gtk_button_new_from_stock(GTK_STOCK_DELETE);
    
	GtkWidget *clear_security_button = gtk_button_new_with_mnemonic("_Delete");
	GtkWidget *clear_icon = gtk_image_new_from_icon_name("gtk-delete", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(clear_security_button), clear_icon);
	gtk_button_set_always_show_image(GTK_BUTTON(clear_security_button), TRUE);    
    
    
    gtk_box_pack_start(GTK_BOX(security_button_box), clear_security_button, FALSE, FALSE, 0);
    g_signal_connect_swapped(G_OBJECT(clear_security_button), "clicked",
        G_CALLBACK(clear_security_tab), widgets);

    //GtkWidget *update_security_button = gtk_button_new_from_stock(GTK_STOCK_REFRESH);

	GtkWidget *update_security_button = gtk_button_new_with_mnemonic("_Refresh");
	GtkWidget *refresh_icon = gtk_image_new_from_icon_name("gtk-refresh", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(update_security_button), refresh_icon);
	gtk_button_set_always_show_image(GTK_BUTTON(update_security_button), TRUE);        
    
    
    gtk_box_pack_start(GTK_BOX(security_button_box), update_security_button, FALSE, FALSE, 0);
    g_signal_connect_swapped(G_OBJECT(update_security_button), "clicked",
        G_CALLBACK(populate_security), widgets);

    gtk_widget_show_all(widgets->main_window);

    if( utf8 != NULL )
        g_free(utf8);
}
