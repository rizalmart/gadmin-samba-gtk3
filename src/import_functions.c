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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "support.h"
#include "allocate.h"
#include "widgets.h"
#include "functions.h"
#include "show_info.h"
#include "import_functions.h"
#include "get_option_pos.h"
#include "commands.h"
#include "commented.h"
#include "apply_share_settings.h"
#include "populate_users.h"
#include "select_first_user.h"
#include "populate_user_settings.h"
#include "populate_shares.h"
#include "populate_share_settings.h"
#include "populate_conf_tab.h"


extern int activated;

extern char global_share_name[1024];

extern int global_import_local;
extern int global_import_remote;

extern int global_import_users;
extern int global_import_groups;

extern int global_import_valid;
extern int global_import_invalid;
extern int global_import_write;
extern int global_import_admin;

long num_imported = 0;
int new_smbuser_added = 0;



/* Checks if the user or group name is listed in a treeview */
int in_access_list(gchar * name, GtkListStore * list_store)
{
    int retval = 0;
    gchar *temp_name;
    GtkTreeIter iter;

    if( list_store )
    {
        if( gtk_tree_model_get_iter_first(GTK_TREE_MODEL(list_store), &iter) )
        {
            do
            {
                gtk_tree_model_get(GTK_TREE_MODEL(list_store), &iter, 0, &temp_name, -1);
                if( temp_name )
                {
                    if( ! strcmp(name, temp_name) )
                    {
                        retval = 1;
                        break;
                    }
                }
            }
            while(gtk_tree_model_iter_next(GTK_TREE_MODEL(list_store), &iter));
        }
    }

    return retval;
}


/* The selected names in the import list are added to the
   share access treeviews with the selected permissions */
void do_import(GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, struct w *widgets)
{
    FILE *fp;
    int imported = 0;
    gchar *utf8, *cmd, *name, *mod_name;

    /* Get users or groups depending on what is selected */
    if( global_import_users )
        gtk_tree_model_get(model, iter, 0, &name, -1);
    else 
    if( global_import_groups )
        gtk_tree_model_get(model, iter, 1, &name, -1);

    /* Add an @ char infront of group names */
    if( global_import_users && global_import_local )
    {
        /* The all users macro is used. FIX: Check if its the first row. */
        if( strstr(name, "All users") )
            mod_name = g_strdup_printf("%%U");
        else
            mod_name = g_strdup_printf("%s", name);
    }
    else
    if( global_import_groups && global_import_local )
    {
        /* The all groups macro is used. FIX: Check if its the first row. */
        if( strstr(name, "All groups") )
            mod_name = g_strdup_printf("%%G");
        else
            mod_name = g_strdup_printf("@%s", name);
    }
    else
    {
        /* FIX: Add global_import_remote */
        mod_name = g_strdup_printf("Avoid compile warning\n");
        g_free(name);
        g_free(mod_name);
        return;
    }


    /* If the name is empty and its a local user import. */
    if( strlen(mod_name) == 0 && global_import_users && global_import_local )
    {
        printf("The import name was empty\n");
        g_free(name);
        g_free(mod_name);
        return;
    }
    /* If the name only contains an '@' or is empty and its a local group import. */
    if( strlen(mod_name) < 2 && global_import_groups && global_import_local )
    {
        printf("The import name was empty\n");
        g_free(name);
        g_free(mod_name);
        return;
    }

// Fixme: Add remote defs..

    /* Machine accounts arent allowed */
    if( strstr((char *)name, "$") )
    {
        printf("An import name containing $ is not allowed as it is a machine account.\n");
        g_free(name);
        g_free(mod_name);
        return;
    }


    /* Add the name to the valid treeview if not there already */
    if( ! in_access_list(mod_name, widgets->valid_users_store) && global_import_valid )
    {
        utf8 = g_locale_to_utf8(mod_name, strlen(mod_name), NULL, NULL, NULL);
        gtk_list_store_append(GTK_LIST_STORE(widgets->valid_users_store), iter);
        gtk_list_store_set(GTK_LIST_STORE(widgets->valid_users_store), iter, 0, utf8, -1);
        if(utf8 != NULL)
            g_free(utf8);

        imported = 1;
    }
    /* Add the name to the invalid treeview if not there already */
    if( ! in_access_list(mod_name, widgets->invalid_users_store) && global_import_invalid )
    {
        utf8 = g_locale_to_utf8(mod_name, strlen(mod_name), NULL, NULL, NULL);
        gtk_list_store_append(GTK_LIST_STORE(widgets->invalid_users_store), iter);
        gtk_list_store_set(GTK_LIST_STORE(widgets->invalid_users_store), iter, 0, utf8, -1);
        if(utf8 != NULL)
            g_free(utf8);

        imported = 1;
    }
    /* Add the name to the write treeview if not there already */
    if( ! in_access_list(mod_name, widgets->write_users_store) && global_import_write )
    {
        utf8 = g_locale_to_utf8(mod_name, strlen(mod_name), NULL, NULL, NULL);
        gtk_list_store_append(GTK_LIST_STORE(widgets->write_users_store), iter);
        gtk_list_store_set(GTK_LIST_STORE(widgets->write_users_store), iter, 0, utf8, -1);
        if(utf8 != NULL)
            g_free(utf8);

        imported = 1;
    }
    /* Add the name to the admin treeview if not there already */
    if( ! in_access_list(mod_name, widgets->admin_users_store) && global_import_admin )
    {
        utf8 = g_locale_to_utf8(mod_name, strlen(mod_name), NULL, NULL, NULL);
        gtk_list_store_append(GTK_LIST_STORE(widgets->admin_users_store), iter);
        gtk_list_store_set(GTK_LIST_STORE(widgets->admin_users_store), iter, 0, utf8, -1);
        if(utf8 != NULL)
            g_free(utf8);

        imported = 1;
    }

    if( imported )
        num_imported++;

    /* Return if a macro such as %U etc is used */
    if( strstr((char *)name, "All users") || strstr((char *)name, "All groups") )
    {
        /* No popup */
        g_free(name);
        g_free(mod_name);
        return;
    }

    /* If its user import and this samba user doesnt exist, add it with an empty password */
    if( ! smbuser_exists(name) && global_import_local && global_import_users )
    {
        new_smbuser_added = 1;

        if( add_null_password_samba_user(name) )
            new_smbuser_added = 1;

        /* Also create its /var/lib/samba/profiles/UserName directory */
        cmd = g_strdup_printf("mkdir -p '%s%s/%s'", LOCALSTATEDIR, "/lib/samba/profiles", name);
        if((fp = popen(cmd, "w")) == NULL)
            perror("popen");
        else
            pclose(fp);

        g_free(cmd);

        /* Chown this users directory to user:user so it can write the profile contents there */
        cmd = g_strdup_printf("chown %s:%s %s%s/%s", name, name, LOCALSTATEDIR,
            "/lib/samba/profiles", name);
        if((fp = popen(cmd, "w")) == NULL)
            perror("popen");
        else
            pclose(fp);

        g_free(cmd);
    }

    g_free(name);
    g_free(mod_name);
}



/* Call foreach on the group or user names in the import treeview */
void import_button_clicked(GtkButton * button, struct w *widgets)
{
    GtkTreeSelection *selection;
    gchar *info = NULL;

    if(global_import_remote)
    {
        info = g_strdup_printf(_("Remote operations are not yet supported.\n"));
        show_info(info);
        g_free(info);
        return;
    }


    /* Global counter */
    num_imported = 0;
    new_smbuser_added = 0;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widgets->import_treeview));

    gtk_tree_selection_selected_foreach(GTK_TREE_SELECTION(selection),
        (GtkTreeSelectionForeachFunc) & do_import, widgets);

    /* Destroy the import window */
    gtk_widget_destroy(widgets->import_window);


    if( new_smbuser_added )
    {
        info = g_strdup_printf(_
            ("New samba users where added with empty passwords.\nSee server settings and empty passwords.\n"));
        show_info(info);
        g_free(info);
    }

    if( global_import_users )
        info = g_strdup_printf(_("Number of added users: %ld\n"), num_imported);
    else
        info = g_strdup_printf(_("Number of added groups: %ld\n"), num_imported);

    show_info(info);
    g_free(info);


    /* If anything was imported, update relevant parts of the gui */
    if( num_imported )
    {
        /* Apply the share so new access is written */
        apply_share_settings(widgets);

        /* Populate the user tab */
        populate_users(widgets);
        /* Set the first user as selected */
        select_first_user(widgets);

        /* Populate the user settings */
        populate_user_settings(widgets);

        // Done in add_share(); populate_shares(widgets);
        // Done in add_share(); populate_share_settings(widgets);

        populate_conf_tab(widgets);
    }
}
