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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "gettext.h"
#include "widgets.h"
#include "allocate.h"
#include "functions.h"
#include "import_functions.h"
#include "make_settings_checkbuttons.h"
#include "create_import_question.h"
#include "create_import_selection.h"

extern int global_import_local;
extern int global_import_remote;

extern int global_import_users;
extern int global_import_groups;

extern int global_import_valid;
extern int global_import_invalid;
extern int global_import_write;
extern int global_import_admin;



/* Local checkbutton callback */
void import_local_checkbutton_toggled(GtkButton * button, struct w *widgets)
{
    /* The local checkbutton is active */
    if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets->import_chkbtn_local)) )
    {
        global_import_local = 1;
        global_import_remote = 0;
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->import_chkbtn_remote), FALSE);
    }
}

/* Remote checkbutton callback */
void import_remote_checkbutton_toggled(GtkButton * button, struct w *widgets)
{
    /* The remote checkbutton is active */
    if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets->import_chkbtn_remote)) )
    {
        global_import_local = 0;
        global_import_remote = 1;
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->import_chkbtn_local), FALSE);
    }
}

/* Users checkbutton callback */
void import_users_checkbutton_toggled(GtkButton * button, struct w *widgets)
{
    /* The users checkbutton is active */
    if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets->import_chkbtn_users)) )
    {
        global_import_users = 1;
        global_import_groups = 0;
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->import_chkbtn_groups), FALSE);
    }
}

/* Groups checkbutton callback */
void import_groups_checkbutton_toggled(GtkButton * button, struct w *widgets)
{
    /* The groups checkbutton is active */
    if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets->import_chkbtn_groups)) )
    {
        global_import_users = 0;
        global_import_groups = 1;
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->import_chkbtn_users), FALSE);
    }
}


/* Valid checkbutton callback */
void import_valid_checkbutton_toggled(GtkButton * button, struct w *widgets)
{
    /* The valid checkbutton is active */
    if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets->import_chkbtn_valid)) )
        global_import_valid = 1;
    else
        global_import_valid = 0;
}

/* Invalid checkbutton callback */
void import_invalid_checkbutton_toggled(GtkButton * button, struct w *widgets)
{
    /* The invalid checkbutton is active */
    if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets->import_chkbtn_invalid)) )
        global_import_invalid = 1;
    else
        global_import_invalid = 0;
}

/* Write checkbutton callback */
void import_write_checkbutton_toggled(GtkButton * button, struct w *widgets)
{
    /* The write checkbutton is active */
    if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets->import_chkbtn_write)) )
        global_import_write = 1;
    else
        global_import_write = 0;
}

/* Admin checkbutton callback */
void import_admin_checkbutton_toggled(GtkButton * button, struct w *widgets)
{
    /* The admin checkbutton is active */
    if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets->import_chkbtn_admin)) )
        global_import_admin = 1;
    else
        global_import_admin = 0;
}


/* A question for how to import users or groups */
void create_import_question(struct w *widgets)
{
    GtkWidget *frame1, *frame2, *frame3;
    GtkWidget *table1, *table2, *table3;
    GtkWidget *question_vbox;
    GtkTooltip *tooltips;
    gchar *utf8 = NULL;
    gchar *info;
    int a = 0, b = 1;

    widgets->import_question_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(widgets->import_question_window), GTK_WIN_POS_CENTER);
    gtk_widget_set_size_request(widgets->import_question_window, 450, -1);

    /* Set window information */
    info = g_strdup_printf(_("GADMIN-SAMBA %s access permissions"), VERSION);
    gtk_window_set_title(GTK_WINDOW(widgets->import_question_window), info);
    g_free(info);

    g_signal_connect(GTK_WINDOW(widgets->import_question_window), "delete_event",
        G_CALLBACK(gtk_widget_destroy), NULL);

    question_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(widgets->import_question_window), question_vbox);

    //tooltips = gtk_tooltip_new();

    /* 3 tables with some settings and 2 columns */
    table1 = gtk_table_new(2, 2, FALSE);
    table2 = gtk_table_new(2, 2, FALSE);
    table3 = gtk_table_new(3, 2, FALSE);

    frame1 = gtk_frame_new("Add users or groups from:");
    gtk_box_pack_start(GTK_BOX(question_vbox), frame1, TRUE, TRUE, 1);
    gtk_container_add(GTK_CONTAINER(frame1), table1);

    frame2 = gtk_frame_new("Add users or groups:");
    gtk_box_pack_start(GTK_BOX(question_vbox), frame2, TRUE, TRUE, 1);
    gtk_container_add(GTK_CONTAINER(frame2), table2);

    frame3 = gtk_frame_new("Permissions:");
    gtk_box_pack_start(GTK_BOX(question_vbox), frame3, TRUE, TRUE, 1);
    gtk_container_add(GTK_CONTAINER(frame3), table3);


    /* Settings */

    /* Import from */
    widgets->import_chkbtn_local =
        make_checkbutton_with_label(GTK_TABLE(table1), _("Local computer:"), 0, 1, a, b);
    a++;
    b++;

    widgets->import_chkbtn_remote =
        make_checkbutton_with_label(GTK_TABLE(table1), _("Remote master domain:"), 0, 1, a, b);

    a = 0;
    b = 1;


    /* Import users or groups */
    widgets->import_chkbtn_users =
        make_checkbutton_with_label(GTK_TABLE(table2), _("Users:"), 0, 1, a, b);
    a++;
    b++;

    widgets->import_chkbtn_groups =
        make_checkbutton_with_label(GTK_TABLE(table2), _("Groups:"), 0, 1, a, b);


    a = 0;
    b = 1;

    /* Import permissions (valid users, invalid users, write list and admin users) */
    widgets->import_chkbtn_valid =
        make_checkbutton_with_label(GTK_TABLE(table3), _("Access allowed:"), 0, 1, a, b);
    a++;
    b++;
    widgets->import_chkbtn_invalid =
        make_checkbutton_with_label(GTK_TABLE(table3), _("Access denied:"), 0, 1, a, b);
    a++;
    b++;
    widgets->import_chkbtn_write =
        make_checkbutton_with_label(GTK_TABLE(table3), _("Write access:"), 0, 1, a, b);
    a++;
    b++;
    widgets->import_chkbtn_admin =
        make_checkbutton_with_label(GTK_TABLE(table3), _("Administrators:"), 0, 1, a, b);

    /* Buttons, "Abort", "Next" */
    GtkWidget *hbutton_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(hbutton_box), GTK_BUTTONBOX_SPREAD);

    GtkWidget *cancel_button, *forward_button;
    
	cancel_button = gtk_button_new_with_mnemonic("_Cancel");
	GtkWidget *cancel_icon = gtk_image_new_from_icon_name("gtk-cancel", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(cancel_button), cancel_icon);
	gtk_button_set_always_show_image(GTK_BUTTON(cancel_button), TRUE);
  
	forward_button = gtk_button_new_with_mnemonic("_Forward");
	GtkWidget *forward_icon = gtk_image_new_from_icon_name("gtk-go-forward", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(forward_button), forward_icon);
	gtk_button_set_always_show_image(GTK_BUTTON(forward_button), TRUE);    
    

    gtk_box_pack_start(GTK_BOX(hbutton_box), cancel_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbutton_box), forward_button, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(question_vbox), hbutton_box);


    /* Set default import states.. same as in gadmin_samba.c (globals origin) */
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->import_chkbtn_local), TRUE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->import_chkbtn_users), TRUE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->import_chkbtn_valid), TRUE);

    /* We need to reset global_* options so that it matches the default settings */
    global_import_local = 1;
    global_import_remote = 0;
    global_import_users = 1;
    global_import_groups = 0;

    global_import_valid = 1;
    global_import_invalid = 0;
    global_import_write = 0;
    global_import_admin = 0;


    /* Signal hookups for the checkbuttons, all uses the same function to set globals */
    g_signal_connect((gpointer) widgets->import_chkbtn_local, "toggled",
        G_CALLBACK(import_local_checkbutton_toggled), widgets);

    g_signal_connect((gpointer) widgets->import_chkbtn_remote, "toggled",
        G_CALLBACK(import_remote_checkbutton_toggled), widgets);

    g_signal_connect((gpointer) widgets->import_chkbtn_users, "toggled",
        G_CALLBACK(import_users_checkbutton_toggled), widgets);

    g_signal_connect((gpointer) widgets->import_chkbtn_groups, "toggled",
        G_CALLBACK(import_groups_checkbutton_toggled), widgets);

    g_signal_connect((gpointer) widgets->import_chkbtn_valid, "toggled",
        G_CALLBACK(import_valid_checkbutton_toggled), widgets);

    g_signal_connect((gpointer) widgets->import_chkbtn_invalid, "toggled",
        G_CALLBACK(import_invalid_checkbutton_toggled), widgets);

    g_signal_connect((gpointer) widgets->import_chkbtn_write, "toggled",
        G_CALLBACK(import_write_checkbutton_toggled), widgets);

    g_signal_connect((gpointer) widgets->import_chkbtn_admin, "toggled",
        G_CALLBACK(import_admin_checkbutton_toggled), widgets);


    /* Quit button */
    g_signal_connect_swapped((gpointer) cancel_button, "clicked",
        G_CALLBACK(gtk_widget_destroy), G_OBJECT(widgets->import_question_window));

    /* Forward button */
    g_signal_connect_swapped((gpointer) forward_button, "clicked",
        G_CALLBACK(create_import_selection), widgets);


    gtk_widget_show_all(widgets->import_question_window);

    if( utf8 != NULL )
        g_free(utf8);
}
