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
#include "make_settings_hseparators.h"


void make_3columns_hseparator(GtkTable * table,
    gint left_attach, gint right_attach, gint top_attach, gint bottom_attach)
{
    GtkWidget *hsep;

    hsep = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_table_attach(table, hsep, left_attach, right_attach + 3, top_attach, bottom_attach,
        GTK_FILL, GTK_EXPAND, 5, 5);
    gtk_widget_show(hsep);
}
