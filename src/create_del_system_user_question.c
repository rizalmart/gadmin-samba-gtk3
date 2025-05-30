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
#include "gettext.h"
#include "widgets.h"
#include "create_del_system_user_question.h"
#include "delete_user.h"


void create_del_system_user_question(struct w *widgets)
{
    GtkWidget *vbox31, *label297, *hbox225;
    GtkWidget *label298, *label299;
    GtkWidget *userdel_question_entry;
    GtkWidget *label300, *hbuttonbox13;
    GtkWidget *question_userdel_cancel_button;
    GtkWidget *alignment49, *hbox229;
    GtkWidget *image49, *label304;
    GtkWidget *question_userdel_delete_button;
    GtkWidget *alignment50, *hbox230, *image50, *label305;

    const gchar *username;
    username = gtk_entry_get_text(GTK_ENTRY(widgets->user_set_entry[0]));

    widgets->del_system_user_question_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(widgets->del_system_user_question_window),
        _("Delete this system user ?"));
    gtk_window_set_position(GTK_WINDOW(widgets->del_system_user_question_window),
        GTK_WIN_POS_CENTER);

    vbox31 = gtk_vbox_new(TRUE, 0);
    gtk_container_add(GTK_CONTAINER(widgets->del_system_user_question_window), vbox31);

    label297 = gtk_label_new(_("Do you also want to delete the system user ?"));
    gtk_box_pack_start(GTK_BOX(vbox31), label297, FALSE, FALSE, 0);
    gtk_label_set_justify(GTK_LABEL(label297), GTK_JUSTIFY_LEFT);

    hbox225 = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox31), hbox225, FALSE, FALSE, 0);

    label298 = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(hbox225), label298, FALSE, FALSE, 0);
    gtk_widget_set_size_request(label298, 30, -1);
    gtk_label_set_justify(GTK_LABEL(label298), GTK_JUSTIFY_LEFT);

    label299 = gtk_label_new(_("Username: "));
    gtk_box_pack_start(GTK_BOX(hbox225), label299, FALSE, FALSE, 0);
    gtk_label_set_justify(GTK_LABEL(label299), GTK_JUSTIFY_LEFT);

    userdel_question_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hbox225), userdel_question_entry, TRUE, TRUE, 0);
    gtk_widget_set_sensitive(userdel_question_entry, FALSE);
    gtk_editable_set_editable(GTK_EDITABLE(userdel_question_entry), FALSE);

    /* Set the username in the entry */
    gtk_entry_set_text(GTK_ENTRY(userdel_question_entry), username);


    label300 = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(hbox225), label300, FALSE, FALSE, 0);
    gtk_widget_set_size_request(label300, 90, -1);
    gtk_label_set_justify(GTK_LABEL(label300), GTK_JUSTIFY_LEFT);

    hbuttonbox13 = gtk_hbutton_box_new();
    gtk_box_pack_start(GTK_BOX(vbox31), hbuttonbox13, FALSE, FALSE, 0);
    gtk_widget_set_size_request(hbuttonbox13, -1, 40);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(hbuttonbox13), GTK_BUTTONBOX_SPREAD);

    question_userdel_cancel_button = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(hbuttonbox13), question_userdel_cancel_button);
    
    //GTK_WIDGET_SET_FLAGS(question_userdel_cancel_button, GTK_CAN_DEFAULT);
    gtk_widget_set_can_default(question_userdel_cancel_button, TRUE);

    alignment49 = gtk_alignment_new(0.5, 0.5, 0, 0);
    gtk_container_add(GTK_CONTAINER(question_userdel_cancel_button), alignment49);

    hbox229 = gtk_hbox_new(FALSE, 2);
    gtk_container_add(GTK_CONTAINER(alignment49), hbox229);

    image49 = gtk_image_new_from_stock("gtk-cancel", GTK_ICON_SIZE_BUTTON);
    gtk_box_pack_start(GTK_BOX(hbox229), image49, FALSE, FALSE, 0);

    label304 = gtk_label_new_with_mnemonic(_("No"));
    gtk_box_pack_start(GTK_BOX(hbox229), label304, FALSE, FALSE, 0);
    gtk_label_set_justify(GTK_LABEL(label304), GTK_JUSTIFY_LEFT);

    question_userdel_delete_button = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(hbuttonbox13), question_userdel_delete_button);
    
    //GTK_WIDGET_SET_FLAGS(question_userdel_delete_button, GTK_CAN_DEFAULT);
    gtk_widget_set_can_default(question_userdel_delete_button, TRUE);

    alignment50 = gtk_alignment_new(0.5, 0.5, 0, 0);
    gtk_container_add(GTK_CONTAINER(question_userdel_delete_button), alignment50);

    hbox230 = gtk_hbox_new(FALSE, 2);
    gtk_container_add(GTK_CONTAINER(alignment50), hbox230);

    image50 = gtk_image_new_from_stock("gtk-remove", GTK_ICON_SIZE_BUTTON);
    gtk_box_pack_start(GTK_BOX(hbox230), image50, FALSE, FALSE, 0);

    label305 = gtk_label_new_with_mnemonic(_("Delete"));
    gtk_box_pack_start(GTK_BOX(hbox230), label305, FALSE, FALSE, 0);
    gtk_label_set_justify(GTK_LABEL(label305), GTK_JUSTIFY_LEFT);


    g_signal_connect_swapped(G_OBJECT(question_userdel_cancel_button), "clicked",
        G_CALLBACK(dont_delete_system_user), widgets);

    g_signal_connect(G_OBJECT(question_userdel_delete_button), "clicked",
        G_CALLBACK(do_delete_system_user), widgets);
}
