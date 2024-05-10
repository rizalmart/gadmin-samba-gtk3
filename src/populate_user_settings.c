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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "gettext.h"
#include "allocate.h"
#include "widgets.h"
#include "functions.h"
#include "populate_user_settings.h"
#include "chars_are_digits.h"
#include "show_info.h"

extern char global_user_name[1024];
extern gulong account_disabled_toggled_signal;


void populate_user_settings(struct w *widgets)
{
    gchar *info, *utf8, *atext;
    int i = 0, locked = 0;
    gchar *what, *val;

    info = g_strdup_printf("%s", global_user_name);
    if( info == NULL || strlen(info) < 1 )
    {
        /* User not specified, clear all widgets */
        for(i = 0; i < 5; i++)
            gtk_entry_set_text(GTK_ENTRY(widgets->user_set_entry[i]), "");
    }
    g_free(info);

    /* Get and insert the users settings */
    gchar *username = g_strdup_printf("%s", global_user_name);
    gtk_entry_set_text(GTK_ENTRY(widgets->user_set_entry[0]), username);

    /* Locked account checkbox and label modifications, see if the account is locked. */
    locked = smbuser_locked(username);

    /* Block the togglebuttons signal so it doesnt emit the changed signal. */
    g_signal_handler_block(GTK_WIDGET(widgets->user_set_checkbutton[0]),
        account_disabled_toggled_signal);
    if( locked && !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets->user_set_checkbutton[0])) )
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->user_set_checkbutton[0]), TRUE);
    if( ! locked && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets->user_set_checkbutton[0])) )
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->user_set_checkbutton[0]), FALSE);
    /* Unblock the togglebuttons signal */
    g_signal_handler_unblock(GTK_WIDGET(widgets->user_set_checkbutton[0]),
        account_disabled_toggled_signal);

    if( locked == 1 )
    {
        /* Samba user account is locked but has a previous password. */
        atext = g_strdup_printf(_("Account locked:"));
        utf8 = g_locale_to_utf8(atext, strlen(atext), NULL, NULL, NULL);
        gtk_label_set_text(GTK_LABEL(widgets->acclock_label), utf8);
        if(atext != NULL)
            g_free(atext);
        if(utf8 != NULL)
            g_free(utf8);
    }
    else if( locked == 2 )
    {
        /* Samba user account is locked but has no previous password. */
        atext = g_strdup_printf(_("Missing password:"));
        utf8 = g_locale_to_utf8(atext, strlen(atext), NULL, NULL, NULL);
        gtk_label_set_text(GTK_LABEL(widgets->acclock_label), utf8);
        if(atext != NULL)
            g_free(atext);
        if(utf8 != NULL)
            g_free(utf8);
    }
    else if( locked == 3 )
    {
        /* A samba user with a Null password is considered locked but
           show that it has an  empty password */
        atext = g_strdup_printf(_("Empty password:"));
        utf8 = g_locale_to_utf8(atext, strlen(atext), NULL, NULL, NULL);
        gtk_label_set_text(GTK_LABEL(widgets->acclock_label), utf8);
        if(atext != NULL)
            g_free(atext);
        if(utf8 != NULL)
            g_free(utf8);
    }
    else
    {
        /* Regular samba user is not locked (locked == 0 ) */
        atext = g_strdup_printf(_("Account locked:"));
        utf8 = g_locale_to_utf8(atext, strlen(atext), NULL, NULL, NULL);
        gtk_label_set_text(GTK_LABEL(widgets->acclock_label), utf8);
        if(atext != NULL)
            g_free(atext);
        if(utf8 != NULL)
            g_free(utf8);
    }

    what = g_strdup_printf("group");
    val = get_user_setting(username, what);
    gchar *groupname = get_group_name(val);
    gtk_entry_set_text(GTK_ENTRY(widgets->user_set_entry[2]), groupname);
    g_free(what);
    g_free(val);
    g_free(groupname);

    what = g_strdup_printf("comment");
    val = get_user_setting(username, what);
    gtk_entry_set_text(GTK_ENTRY(widgets->user_set_entry[3]), val);
    g_free(what);
    g_free(val);

    what = g_strdup_printf("homedir");
    val = get_user_setting(username, what);
    gtk_entry_set_text(GTK_ENTRY(widgets->user_set_entry[4]), val);
    g_free(what);
    g_free(val);

    what = g_strdup_printf("shell");
    val = get_user_setting(username, what);
    g_free(what);
    g_free(username);

    /* Select the shell we just got in the shell combo box */
    GtkListStore *shell_store;
    shell_store =
        GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(widgets->user_set_combo[0])));

    if( shell_store )
    {
        GtkTreeIter shell_iter;
        gchar *shell;

        if( gtk_tree_model_get_iter_first(GTK_TREE_MODEL(shell_store), &shell_iter) )
        {
            do
            {
                gtk_tree_model_get(GTK_TREE_MODEL(shell_store), &shell_iter, 0, &shell, -1);
                if( shell )
                {
                    if( ! strcmp(val, shell) )
                    {
                        gtk_combo_box_set_active_iter(GTK_COMBO_BOX(widgets->user_set_combo[0]),
                            &shell_iter);
                        break;
                    }
                }
                if(shell != NULL)
                    g_free(shell);

            }
            while(gtk_tree_model_iter_next(GTK_TREE_MODEL(shell_store), &shell_iter));
        }
    }
    g_free(val);
}
