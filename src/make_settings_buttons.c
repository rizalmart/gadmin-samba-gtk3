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
#include "gettext.h"
#include "widgets.h"
#include "make_settings_buttons.h"


GtkWidget * make_button_with_entry(GtkTable * table,
    GtkWidget * entry,
    char icon_name[100],
    gchar * utf8_btn_txt,
    gchar * utf8_tip_txt,
    gint left_attach, gint right_attach, gint top_attach, gint bottom_attach, gint button_length)
{
    GtkWidget *button = gtk_button_new();
    //GtkTooltip *tooltips = gtk_tooltip_new();

    GtkWidget *alignment = gtk_alignment_new(0.5, 0.5, 0, 0);
    gtk_container_add(GTK_CONTAINER(button), alignment);

    GtkWidget *hbox = gtk_hbox_new(FALSE, 2);
    gtk_container_add(GTK_CONTAINER(alignment), hbox);
    GtkWidget *image = gtk_image_new_from_stock(icon_name, GTK_ICON_SIZE_BUTTON);
    gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);

    GtkWidget *label = gtk_label_new_with_mnemonic(utf8_btn_txt);
    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
    //gtk_tooltips_set_tip(tooltips, button, utf8_tip_txt, NULL);
    gtk_widget_set_tooltip_text(button, utf8_tip_txt);

    gtk_table_attach(table, button, left_attach, right_attach, top_attach, bottom_attach, GTK_FILL,
        GTK_EXPAND, 2, 2);
    gtk_table_attach(table, entry, 1, 2, top_attach, bottom_attach, GTK_FILL | GTK_EXPAND,
        GTK_FILL | GTK_EXPAND, 20, 2);

    gtk_widget_show(button);
    gtk_widget_show(entry);

    return button;
}
