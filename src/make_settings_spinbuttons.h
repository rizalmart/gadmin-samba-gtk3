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



#ifndef make_settings_spinbuttons_H
#   define make_settings_spinbuttons_H

GtkWidget *make_spinbutton_with_label(GtkTable *table, 
                                 const gchar *label_text, 
				 gint left_attach, 
				 gint right_attach, 
				 gint top_attach, 
                                 gint bottom_attach, 
				 gint entry_length);

GtkWidget *make_padded_spinbutton_with_label(GtkTable *table, 
                                 const gchar *label_text, 
				 gint left_attach, 
				 gint right_attach, 
				 gint top_attach, 
                                 gint bottom_attach, 
				 gint entry_length);

GtkWidget *make_short_spinbutton_with_label(GtkTable *table, 
                                 const gchar *label_text, 
				 gint left_attach, 
				 gint right_attach, 
				 gint top_attach, 
                                 gint bottom_attach, 
				 gint entry_length);

GtkWidget *make_spinbutton(GtkTable *table, 
				 gint left_attach, 
				 gint right_attach, 
				 gint top_attach, 
                                 gint bottom_attach, 
				 gint entry_length);
#endif
