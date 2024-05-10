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
#include "make_settings_checkbuttons.h"


GtkWidget * make_checkbutton_with_label(GtkTable * table,
    const gchar * label_text,
    gint left_attach, gint right_attach, gint top_attach, gint bottom_attach)
{
    GtkWidget *checkbutton;

    GtkWidget *label;
    label = gtk_label_new(label_text);

    checkbutton = gtk_check_button_new();

    gtk_table_attach(table, label, left_attach, right_attach, top_attach, bottom_attach, GTK_FILL,
        GTK_EXPAND, 2, 2);
    gtk_table_attach(table, checkbutton, left_attach + 1, right_attach + 1, top_attach,
        bottom_attach, GTK_FILL, GTK_EXPAND, 20, 2);

    gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
    gtk_misc_set_padding(GTK_MISC(label), 2, 2);
//    gtk_widget_set_size_request(spinbutton, spinbutton_length, -1);

    gtk_widget_show(checkbutton);
    gtk_widget_show(label);

    return checkbutton;
}
