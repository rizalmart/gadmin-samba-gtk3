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

#ifndef create_import_question_H
#	define create_import_question_H

extern struct w *widgets;

void create_import_question(struct w *widgets);

void import_local_checkbutton_toggled(GtkButton *button, struct w *widgets);
void import_remote_checkbutton_toggled(GtkButton *button, struct w *widgets);

void import_users_checkbutton_toggled(GtkButton *button, struct w *widgets);
void import_groups_checkbutton_toggled(GtkButton *button, struct w *widgets);

void import_valid_checkbutton_toggled(GtkButton *button, struct w *widgets);
void import_write_checkbutton_toggled(GtkButton *button, struct w *widgets);
void import_admin_checkbutton_toggled(GtkButton *button, struct w *widgets);


#endif
