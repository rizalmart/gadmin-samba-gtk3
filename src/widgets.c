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

/* Make sure we have enough of these or itll 
   borrow from the next widget type and fail */
#define NUM_SERVERTAB_ENTRIES 62
#define NUM_SERVERTAB_SPINBUTTONS 20
#define NUM_SERVERTAB_COMBOS 42

#define NUM_USERTAB_ENTRIES 7
#define NUM_USERTAB_SPINBUTTONS 1
#define NUM_USERTAB_CHECKBUTTONS 3
#define NUM_USERTAB_COMBOS 1

#define NUM_SHARETAB_ENTRIES 22
#define NUM_SHARETAB_SPINBUTTONS 10
#define NUM_SHARETAB_CHECKBUTTONS 10
#define NUM_SHARETAB_COMBOS 10

#define NUM_MACCTTAB_ENTRIES 2

#define NUM_SETTINGS_ENTRIES 20
#define NUM_SETTINGS_SPINBUTTONS 2


typedef struct w
{
    GtkWidget *main_window;
    GtkWidget *main_vbox;

    GtkWidget *notebook_vbox1;
    GtkWidget *notebook_vbox2;
    GtkWidget *notebook_vbox3;
    GtkWidget *notebook_vbox4;
    GtkWidget *notebook_vbox5;
    GtkWidget *notebook_vbox6;
    GtkWidget *notebook_vbox7;
    GtkWidget *notebook_vbox8;
    GtkWidget *notebook_vbox9;

    GtkWidget *status_label;
    GtkWidget *version_label;

    /* The server tab's widgets */
    GtkWidget *server_treeview;
    GtkListStore *server_store;
    GtkWidget *server_settings_vbox;
    GtkWidget *server_settings_scrolled_window;

    GtkWidget *server_set_entry[NUM_SERVERTAB_ENTRIES];
    GtkWidget *server_set_spinbutton[NUM_SERVERTAB_SPINBUTTONS];
    GtkWidget *server_set_combo[NUM_SERVERTAB_COMBOS];
    GtkWidget *srv_set_table;

    /* The user tab's widgets */
    GtkWidget *user_treeview;
    GtkListStore *user_store;
    GtkWidget *user_settings_vbox;
    GtkWidget *user_settings_scrolled_window;

    GtkWidget *user_set_entry[NUM_USERTAB_ENTRIES];
    GtkWidget *user_set_spinbutton[NUM_USERTAB_SPINBUTTONS];
    GtkWidget *user_set_checkbutton[NUM_USERTAB_CHECKBUTTONS];
    GtkWidget *user_set_combo[NUM_USERTAB_COMBOS];
    GtkWidget *gen_cert_progressbar;
    GtkWidget *user_set_table;
    GtkWidget *acclock_label;

    /* The add user widgets */
    GtkWidget *useradd_username_entry;
    GtkWidget *useradd_group_entry;
    GtkWidget *useradd_password_entry;
    GtkWidget *useradd_homedir_entry;
    GtkWidget *useradd_comment_entry;
    GtkWidget *useradd_shell_combo;

    /* The user tab's directory treeview */
    GtkWidget *directory_treeview;
    GtkListStore *directory_store;

    /* The share tab's widgets */
    GtkWidget *share_treeview;
    GtkListStore *share_store;
    GtkWidget *share_settings_vbox;
    GtkWidget *share_settings_scrolled_window;

    /* The user settings widgets */
    GtkWidget *share_set_entry[NUM_SHARETAB_ENTRIES];
    GtkWidget *share_set_spinbutton[NUM_SHARETAB_SPINBUTTONS];
    GtkWidget *share_set_checkbutton[NUM_SHARETAB_CHECKBUTTONS];
    GtkWidget *share_set_combo[NUM_SHARETAB_COMBOS];
    GtkWidget *share_set_table;
    /* User and group access (valid/invalid/write/admin) */
    GtkWidget *valid_users_treeview;
    GtkListStore *valid_users_store;
    GtkWidget *invalid_users_treeview;
    GtkListStore *invalid_users_store;
    GtkWidget *write_users_treeview;
    GtkListStore *write_users_store;
    GtkWidget *admin_users_treeview;
    GtkListStore *admin_users_store;

    /* The delete system user question window */
    GtkWidget *del_system_user_question_window;

    /* The add default configuration question window */
    GtkWidget *default_conf_question_window;

    /* The machine account tab's widgets */
    GtkWidget *macct_treeview;
    GtkListStore *macct_store;
    GtkWidget *macct_settings_vbox;
    GtkWidget *macct_settings_scrolled_window;
    GtkWidget *macct_set_table;
    GtkWidget *macct_set_entry[NUM_MACCTTAB_ENTRIES];

    /* The connection tabs widgets */
    GtkWidget *conns_treeview;
    GtkListStore *conns_store;

    /* The disc tabs widgets */
    GtkWidget *disc_treeview;
    GtkListStore *disc_store;

    /* The file treeview */
    GtkWidget *file_treeview;
    GtkListStore *file_store;

    /* The security tabs widgets */
    GtkWidget *security_treeview;
    GtkListStore *security_store;

    /* The conf tabs textview */
    GtkWidget *conf_textview;

    /* The import questions widgets */
    GtkWidget *import_question_window;

    GtkWidget *import_chkbtn_local;
    GtkWidget *import_chkbtn_remote;

    GtkWidget *import_chkbtn_users;
    GtkWidget *import_chkbtn_groups;

    GtkWidget *import_chkbtn_valid;
    GtkWidget *import_chkbtn_invalid;
    GtkWidget *import_chkbtn_write;
    GtkWidget *import_chkbtn_admin;

    /* The import selectors widgets */
    GtkWidget *import_window;
    GtkWidget *import_treeview;
    GtkListStore *import_store;

    /* The settings window */
    GtkWidget *settings_window;
    GtkWidget *settings_entry[NUM_SETTINGS_ENTRIES];
    GtkWidget *settings_checkbutton[NUM_SETTINGS_SPINBUTTONS];

    GtkWidget *add_configuration_window;

} wid;
