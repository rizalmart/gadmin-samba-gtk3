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
#include "create_user_settings.h"
#include "populate_shell_combo.h"
#include "add_change_user.h"
#include "delete_user.h"
#include "functions.h"
#include "file_chooser.h"

gulong account_disabled_toggled_signal;



void select_home_path_clicked(struct w *widgets)
{
    gchar *path = NULL;

    path = get_dialog_path_selection("DIR", "/home", "None");
    if( path != NULL )
    {
        gtk_entry_set_text(GTK_ENTRY(widgets->user_set_entry[4]), path);
        g_free(path);
    }
}


void create_user_settings(struct w *widgets)
{
    GtkTooltip *tooltips;
    GtkWidget *hbuttonbox;
    GtkWidget *delete_user_button;
    GtkWidget *apply_user_button;

    /* For the custom new user button */
    GtkWidget *new_user_button;
    GtkWidget *new_user_hbox;
    GtkWidget *new_user_image;
    GtkWidget *new_user_label;
    GtkWidget *new_user_alignment;

    int a = 0;
    int b = 1;
    int ent = 0;
    int comb = 0;
    int check = 0;

    //tooltips = gtk_tooltip_new();

    /* Account locked checkbutton and label */
    widgets->user_set_checkbutton[check] = gtk_check_button_new();
    widgets->acclock_label = gtk_label_new(_("Account locked:"));

    /* The delete, add and apply user buttons */
    //delete_user_button = gtk_button_new_from_stock(GTK_STOCK_DELETE);
    
	delete_user_button = gtk_button_new_with_mnemonic("_Delete");
	GtkWidget *delete_user_icon = gtk_image_new_from_icon_name("gtk-delete", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(delete_user_button), delete_user_icon);
	gtk_button_set_always_show_image(GTK_BUTTON(delete_user_button), TRUE);
    
    
    /* Custom new user button begin */
    new_user_button = gtk_button_new();
    //new_user_alignment = gtk_alignment_new(0.5, 0.5, 0, 0);
	new_user_alignment=gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	gtk_widget_set_halign(new_user_alignment, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(new_user_alignment, GTK_ALIGN_CENTER);
    
    gtk_container_add(GTK_CONTAINER(new_user_button), new_user_alignment);
    new_user_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_container_add(GTK_CONTAINER(new_user_alignment), new_user_hbox);
    new_user_image = gtk_image_new_from_icon_name("gtk-add", GTK_ICON_SIZE_BUTTON);
    
    gtk_box_pack_start(GTK_BOX(new_user_hbox), new_user_image, TRUE, TRUE, 0);
    new_user_label = gtk_label_new_with_mnemonic(_("New user"));

    gtk_box_pack_start(GTK_BOX(new_user_hbox), new_user_label, TRUE, TRUE, 0);
    gtk_label_set_justify(GTK_LABEL(new_user_label), GTK_JUSTIFY_LEFT);
    /* Custom new user button end */

    /* The apply button */
    //apply_user_button = gtk_button_new_from_stock(GTK_STOCK_APPLY);
 	apply_user_button = gtk_button_new_with_mnemonic("_Apply");
	GtkWidget *apply_user_icon = gtk_image_new_from_icon_name("gtk-apply", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(apply_user_button), apply_user_icon);
	gtk_button_set_always_show_image(GTK_BUTTON(apply_user_button), TRUE);   
    

    hbuttonbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);

    gtk_box_pack_start(GTK_BOX(hbuttonbox), widgets->acclock_label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbuttonbox), widgets->user_set_checkbutton[check], TRUE, TRUE, 0);
    check++;

    gtk_box_pack_start(GTK_BOX(hbuttonbox), delete_user_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbuttonbox), new_user_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbuttonbox), apply_user_button, TRUE, TRUE, 0);

    gtk_table_attach(GTK_TABLE(widgets->user_set_table), hbuttonbox, 0, 3, a, b, 0, 0, 5, 5);
    a++;
    b++;


    g_signal_connect_swapped(G_OBJECT(delete_user_button), "clicked",
        G_CALLBACK(delete_user), widgets);

    g_signal_connect_swapped(G_OBJECT(apply_user_button), "clicked",
        G_CALLBACK(add_change_user), widgets);

    g_signal_connect_swapped(G_OBJECT(new_user_button), "clicked",
        G_CALLBACK(clear_user_profile), widgets);


    /* Fetch the gulong as well as hooking up the connect signal. */
    account_disabled_toggled_signal =
        g_signal_connect_swapped(G_OBJECT(widgets->user_set_checkbutton[0]), "clicked",
        G_CALLBACK(account_locked_checkbutton_clicked), widgets);


    /* Username - Max length and input 350 chars */
    GtkWidget *username_button;
    widgets->user_set_entry[ent] = gtk_entry_new();
    username_button = make_button_with_entry(GTK_TABLE(widgets->user_set_table), widgets->user_set_entry[ent], "gtk-refresh",
        _("Username:"), _("Create a randomized username"), 0, 1, a, b, 350);

    //gtk_tooltips_set_tip(tooltips, widgets->user_set_entry[ent], _("The name of the user."), NULL);
    gtk_widget_set_tooltip_text(widgets->user_set_entry[ent], _("The name of the user."));
    a++;
    b++;
    ent++;

    g_signal_connect_swapped(G_OBJECT(username_button), "clicked",
        G_CALLBACK(randomize_username), widgets);

    /* Password  */
    GtkWidget *password_button;
    widgets->user_set_entry[ent] = gtk_entry_new();
    password_button = make_button_with_entry(GTK_TABLE(widgets->user_set_table), widgets->user_set_entry[ent], "gtk-refresh",
        _("Password:"), _("Create a randomized password"), 0, 1, a, b, 350);
    //gtk_tooltips_set_tip(tooltips, widgets->user_set_entry[ent], _("The users password."), NULL);
    gtk_widget_set_tooltip_text(widgets->user_set_entry[ent], _("The users password."));
    a++;
    b++;
    ent++;

    g_signal_connect_swapped(G_OBJECT(password_button), "clicked",
        G_CALLBACK(randomize_password), widgets);


    /* Group */
    widgets->user_set_entry[ent] =
        make_entry_with_label(GTK_TABLE(widgets->user_set_table), _(" Group: "), 0, 1, a, b, 350);
    //gtk_tooltips_set_tip(tooltips, widgets->user_set_entry[ent], _("The group the user belongs to"), NULL);
    gtk_widget_set_tooltip_text(widgets->user_set_entry[ent], _("The group the user belongs to"));
    a++;
    b++;
    ent++;


    /* Comment */
    widgets->user_set_entry[ent] =
        make_entry_with_label(GTK_TABLE(widgets->user_set_table), _(" Comment: "), 0, 1, a, b, 350);
    //gtk_tooltips_set_tip(tooltips, widgets->user_set_entry[ent], _("A comment about the user"),
    //    NULL);
    gtk_widget_set_tooltip_text(widgets->user_set_entry[ent], _("A comment about the user"));    
    a++;
    b++;
    ent++;


    /* Home directory */
    GtkWidget *homedir_button;
    widgets->user_set_entry[ent] = gtk_entry_new();
    homedir_button = make_button_with_entry(GTK_TABLE(widgets->user_set_table), widgets->user_set_entry[ent], "gtk-refresh",
        _("Home directory:"), _("Choose a home directory for this user"), 0, 1, a, b, 350);
    //gtk_tooltips_set_tip(tooltips, widgets->user_set_entry[ent],
    //    _("The users home directory, IE: /home/UserName or /dev/null to skip making a homedirectory."), NULL);
    gtk_widget_set_tooltip_text(widgets->user_set_entry[ent], _("The users home directory, IE: /home/UserName or /dev/null to skip making a home directory."));
    
    a++;
    b++;
    ent++;
    g_signal_connect_swapped(G_OBJECT(homedir_button), "clicked",
        G_CALLBACK(select_home_path_clicked), widgets);


    widgets->user_set_combo[comb] =
        make_label_textcombo_label(GTK_TABLE(widgets->user_set_table), _(" Shell: "), 0, 1, a, b, 80);
    populate_shell_combo(widgets->user_set_combo[comb]);
    a++;
    b++;
    comb++;


    gtk_widget_show_all(widgets->main_window);
}
