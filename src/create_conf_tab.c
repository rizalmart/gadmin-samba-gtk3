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
#include "create_conf_tab.h"
#include "save_conf_tab.h"


/* Wether or not to let the conf section expand */
#define EXPAND_CONF_SECTION TRUE



void create_conf_tab(struct w *widgets)
{
    GtkWidget *conf_textview_hbox;
    GtkWidget *conf_scrolled_window;
    gchar *utf8 = NULL;


    /* Create the conf textview in a scrolled window */
    conf_textview_hbox = gtk_hbox_new(TRUE, 0);
    gtk_box_pack_start(GTK_BOX(widgets->notebook_vbox9), conf_textview_hbox, EXPAND_CONF_SECTION, TRUE, 0);

    conf_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(conf_textview_hbox), conf_scrolled_window, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(conf_scrolled_window),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

    widgets->conf_textview = gtk_text_view_new();
    gtk_container_add(GTK_CONTAINER(conf_scrolled_window), widgets->conf_textview);

    /* The save conf button */
    GtkWidget *conf_button_box = gtk_hbutton_box_new();
    gtk_box_pack_start(GTK_BOX(widgets->notebook_vbox9), conf_button_box, FALSE, FALSE, 0);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(conf_button_box), GTK_BUTTONBOX_SPREAD);

    GtkWidget *save_conf_button = gtk_button_new_from_stock(GTK_STOCK_SAVE);
    gtk_box_pack_start(GTK_BOX(conf_button_box), save_conf_button, FALSE, FALSE, 0);
    g_signal_connect_swapped(G_OBJECT(save_conf_button), "clicked",
        G_CALLBACK(save_conf_tab), widgets);

    gtk_widget_show_all(widgets->main_window);

    if( utf8 != NULL )
        g_free(utf8);
}
