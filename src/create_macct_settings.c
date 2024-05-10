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
#include "make_settings_buttons.h"
#include "make_settings_entries.h"
#include "make_settings_combos.h"
#include "make_settings_spinbuttons.h"
#include "make_settings_checkbuttons.h"
#include "create_macct_settings.h"
#include "add_macct.h"
#include "delete_macct.h"
#include "change_macct.h"
#include "functions.h"



void create_macct_settings(struct w *widgets)
{
    GtkTooltip *tooltips;
//    gchar *utf8 = NULL;

    GtkWidget *hbuttonbox;
    GtkWidget *delete_macct_button;
    GtkWidget *add_macct_button;
    GtkWidget *apply_macct_button;

    int a = 0;
    int b = 1;
    int ent = 0;

    //tooltips = gtk_tooltip_new();

    /* The delete and apply machine account settings buttons */
    delete_macct_button = gtk_button_new_from_stock(GTK_STOCK_DELETE);
    add_macct_button = gtk_button_new_from_stock(GTK_STOCK_ADD);
    apply_macct_button = gtk_button_new_from_stock(GTK_STOCK_APPLY);

    hbuttonbox = gtk_hbutton_box_new();

    gtk_box_pack_start(GTK_BOX(hbuttonbox), delete_macct_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbuttonbox), add_macct_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbuttonbox), apply_macct_button, TRUE, TRUE, 0);

    gtk_table_attach(GTK_TABLE(widgets->macct_set_table), hbuttonbox, 0, 3, a, b, 0, 0, 5, 5);
    a++;
    b++;


    g_signal_connect_swapped(G_OBJECT(delete_macct_button), "clicked",
        G_CALLBACK(delete_macct), widgets);

    g_signal_connect_swapped(G_OBJECT(add_macct_button), "clicked", G_CALLBACK(add_macct), widgets);

    g_signal_connect_swapped(G_OBJECT(apply_macct_button), "clicked",
        G_CALLBACK(change_macct), widgets);


    /* Machine account name */
    widgets->macct_set_entry[ent] =
        make_entry_with_label(GTK_TABLE(widgets->macct_set_table), _(" Machine name: "), 0, 1, a, b, 350);
    //gtk_tooltips_set_tip(tooltips, widgets->macct_set_entry[ent], "IE: SomeMachine", NULL);
    gtk_widget_set_tooltip_text(widgets->macct_set_entry[ent], "IE: SomeMachine");    
    
    /* Must be able to type a new one to add gtk_widget_set_sensitive(widgets->macct_set_entry[ent], FALSE); */
    a++;
    b++;
    ent++;

    /* Comment */
    widgets->macct_set_entry[ent] =
        make_entry_with_label(GTK_TABLE(widgets->macct_set_table), _(" Comment: "), 0, 1, a, b, 350);
    //gtk_tooltips_set_tip(tooltips, widgets->macct_set_entry[ent],
    //    "IE: The name of the user on the client machine", NULL);
    gtk_widget_set_tooltip_text(widgets->macct_set_entry[ent], "IE: The name of the user on the client machine");  
    
    a++;
    b++;
    ent++;


    gtk_widget_show_all(widgets->main_window);
}
