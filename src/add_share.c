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
#include "widgets.h"
#include "allocate.h"
#include "chars_are_digits.h"
#include "add_share.h"
#include "reread_conf.h"
#include "commented.h"
#include "functions.h"
#include "get_option_pos.h"
#include "show_info.h"
#include "populate_shares.h"
#include "populate_share_settings.h"
#include "populate_conf_tab.h"

#define MAX_USERS_PER_ROW 10

int get_data_size = 0;
int access_type = 0;
long num_users = 0;
long data_size = 0;
char *access_buffer = NULL;

extern char global_share_name[1024];


int add_access_to_buffer(GtkTreeModel * model, GtkTreePath * path,
                             GtkTreeIter * iter, struct w *widgets)
{
    gchar *conf_line = NULL;
    gchar *option1;

    gtk_tree_model_get(model, iter, 0, &option1, -1);

    /* No more users in this treeview. */
    if( option1 == NULL )
    {
        num_users = 0;

        if( option1 != NULL )
            g_free(option1);
        /* Return false to continue iteration in case something odd happens */
        return FALSE;
    }

    /* Get the byte size of all lines in this treeview */
    if( get_data_size )
    {
        /* +15 For "option =" and newlines (optimize later) */
        data_size = data_size + (strlen(option1) + 15);
        if( option1 != NULL )
            g_free(option1);
        return FALSE;
    }

    /* Access type "valid users = ..." */
    if( access_type == 0 )
    {
        if( num_users == 0 )
        {
            conf_line = g_strdup_printf("valid users = %s", option1);
        }
        else
        if( num_users == MAX_USERS_PER_ROW )
        {
            num_users = 0;
            /* Adds a continued line ('\') plus the user */
            conf_line = g_strdup_printf(" \\\n%s", option1);
        }
        else
        {
            conf_line = g_strdup_printf(" %s", option1);
        }
        /* Add to global buffer */
        strcat(access_buffer, conf_line);
    }

    /* Access type: "invalid users = ..." */
    if( access_type == 1 )
    {
        if( num_users == 0 )
        {
            conf_line = g_strdup_printf("invalid users = %s", option1);
        }
        else
        if( num_users == MAX_USERS_PER_ROW )
        {
            num_users = 0;
            conf_line = g_strdup_printf(" \\\n%s", option1);
        }
        else
        {
            conf_line = g_strdup_printf(" %s", option1);
        }
        /* Add to global buffer */
        strcat(access_buffer, conf_line);
    }

    /* Access type: "write list = ..." */
    if( access_type == 2 )
    {
        if( num_users == 0 )
        {
            conf_line = g_strdup_printf("write list = %s", option1);
        }
        else
        if( num_users == MAX_USERS_PER_ROW )
        {
            num_users = 0;
            conf_line = g_strdup_printf(" \\\n%s", option1);
        }
        else
        {
            conf_line = g_strdup_printf(" %s", option1);
        }
        /* Add to global buffer */
        strcat(access_buffer, conf_line);
    }

    /* Access type: "admin users = ..." */
    if( access_type == 3 )
    {
        if( num_users == 0 )
        {
            conf_line = g_strdup_printf("admin users = %s", option1);
        }
        else
        if( num_users == MAX_USERS_PER_ROW )
        {
            num_users = 0;
            conf_line = g_strdup_printf(" \\\n%s", option1);
        }
        else
        {
            conf_line = g_strdup_printf(" %s", option1);
        }
        /* Add to global buffer */
        strcat(access_buffer, conf_line);
    }

    num_users++;

    if( conf_line != NULL )
        g_free(conf_line);

    if( option1 != NULL )
        g_free(option1);

    /* Return false to continue iteration */
    return FALSE;
}


/* Adds a new share. Apply also uses this but first removes the share */
void add_share(struct w *widgets)
{
    FILE *fp;
    long conf_size;
    char *line, *new_conf;
    gchar *dir, *cmd, *opt, *info;
    gchar *cmp_global_share_name;
    gint active_index;
    GtkTreeModel *model;

    /* Entries, get all share settings */
    const gchar *share_name = gtk_entry_get_text(GTK_ENTRY(widgets->share_set_entry[0]));
    const gchar *directory = gtk_entry_get_text(GTK_ENTRY(widgets->share_set_entry[1]));
    const gchar *comment = gtk_entry_get_text(GTK_ENTRY(widgets->share_set_entry[2]));
    const gchar *dir_mask = gtk_entry_get_text(GTK_ENTRY(widgets->share_set_entry[3]));
    const gchar *create_mode = gtk_entry_get_text(GTK_ENTRY(widgets->share_set_entry[4]));
    const gchar *force_user = gtk_entry_get_text(GTK_ENTRY(widgets->share_set_entry[5]));
    const gchar *force_group = gtk_entry_get_text(GTK_ENTRY(widgets->share_set_entry[6]));

    if( share_name == NULL || strlen(share_name) < 1 )
    {
        info = g_strdup_printf(_("The share name is missing, the share was not added.\n"));
        show_info(info);
        g_free(info);
        return;
    }

    if( directory == NULL || strlen(directory) < 2 )
    {
        info = g_strdup_printf(_("The directory to share is missing, the share was not added.\n"));
        show_info(info);
        g_free(info);
        return;
    }

    /* Call foreach on all access treeviews with
       get_data_size=1 to get total data size */
    data_size = 0;
    get_data_size = 1;
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(widgets->valid_users_treeview));
    gtk_tree_model_foreach(GTK_TREE_MODEL(model),
        (GtkTreeModelForeachFunc) & add_access_to_buffer, widgets);

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(widgets->invalid_users_treeview));
    gtk_tree_model_foreach(GTK_TREE_MODEL(model),
        (GtkTreeModelForeachFunc) & add_access_to_buffer, widgets);

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(widgets->write_users_treeview));
    gtk_tree_model_foreach(GTK_TREE_MODEL(model),
        (GtkTreeModelForeachFunc) & add_access_to_buffer, widgets);

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(widgets->admin_users_treeview));
    gtk_tree_model_foreach(GTK_TREE_MODEL(model),
        (GtkTreeModelForeachFunc) & add_access_to_buffer, widgets);

    access_buffer = allocate(data_size + 10);

    get_data_size = 0;

    /* Call foreach on the treeview with access_type=0 to get all valid users */
    access_type = 0;
    num_users = 0;
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(widgets->valid_users_treeview));
    gtk_tree_model_foreach(GTK_TREE_MODEL(model),
        (GtkTreeModelForeachFunc) & add_access_to_buffer, widgets);
    if( access_buffer != NULL && strlen(access_buffer) > 2
        && access_buffer[strlen(access_buffer) - 1] != '\n')
        strcat(access_buffer, "\n");

    /* Call foreach on the treeview with access_type=1 to get all invalid users */
    access_type = 1;
    num_users = 0;
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(widgets->invalid_users_treeview));
    gtk_tree_model_foreach(GTK_TREE_MODEL(model),
        (GtkTreeModelForeachFunc) & add_access_to_buffer, widgets);
    if( access_buffer != NULL && strlen(access_buffer) > 2
        && access_buffer[strlen(access_buffer) - 1] != '\n' )
        strcat(access_buffer, "\n");

    /* Call foreach on the treeview with access_type=2 to get all write list users */
    access_type = 2;
    num_users = 0;
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(widgets->write_users_treeview));
    gtk_tree_model_foreach(GTK_TREE_MODEL(model),
        (GtkTreeModelForeachFunc) & add_access_to_buffer, widgets);
    if( access_buffer != NULL && strlen(access_buffer) > 2
        && access_buffer[strlen(access_buffer) - 1] != '\n' )
        strcat(access_buffer, "\n");

    /* Call foreach on the treeview with access_type=3 to get all admin users */
    access_type = 3;
    num_users = 0;
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(widgets->admin_users_treeview));
    gtk_tree_model_foreach(GTK_TREE_MODEL(model),
        (GtkTreeModelForeachFunc) & add_access_to_buffer, widgets);
    if( access_buffer != NULL && strlen(access_buffer) > 2
        && access_buffer[strlen(access_buffer) - 1] != '\n' )
        strcat(access_buffer, "\n");


    if((fp = fopen(SAMBA_CONF, "r")) == NULL)
    {
        info = g_strdup_printf(_("Cant open smb.conf here:\n%s\n"), SAMBA_CONF);
        show_info(info);
        g_free(info);
        free(access_buffer);
        return;
    }
    fseek(fp, 0, SEEK_END);
    conf_size = ftell(fp);
    rewind(fp);

    line = allocate(conf_size + 1);

    /* Plus for the share options and users */
    new_conf = allocate(conf_size + 2000 + data_size);

    /* First collect the entire conf, if the share exists then return */
    gchar *sharedef = g_strdup_printf("[%s]", share_name);

    if( conf_size > 1 )
    while(fgets(line, conf_size, fp) != NULL)
    {
        if( strstr(line, sharedef) )
        {
            free(line);
            free(new_conf);
            fclose(fp);
            info = g_strdup_printf(_("The share already exists.\n"));
            show_info(info);
            g_free(info);
            free(access_buffer);
            return;
        }
        strcat(new_conf, line);
    }
    fclose(fp);
    free(line);


    /* Entries... */

    /* Add the new share definition */
    opt = g_strdup_printf("\n[%s]\n", share_name);
    strcat(new_conf, opt);

    opt = g_strdup_printf("path = %s\n", directory);
    strcat(new_conf, opt);

    if( comment != NULL && strlen(comment) > 1 )
    {
        opt = g_strdup_printf("comment = %s\n", comment);
        strcat(new_conf, opt);
    }
    else
    {
        // Fixme: Make it translatable
        opt = g_strdup_printf("comment = No comment\n");
        strcat(new_conf, opt);
    }


    /* Add the access_buffer contents */
    if( access_buffer != NULL && strlen(access_buffer) > 5 )
    {
        strcat(new_conf, access_buffer);
    }

    free(access_buffer);


    /* If these 4 arent specified at share creation then dont use them */
    if( dir_mask != NULL && strlen(dir_mask) > 1 )
    {
        opt = g_strdup_printf("directory mask = %s\n", dir_mask);
        strcat(new_conf, opt);
    }

    if( create_mode != NULL && strlen(create_mode) > 1 )
    {
        opt = g_strdup_printf("create mode = %s\n", create_mode);
        strcat(new_conf, opt);
    }

    if( force_user != NULL && strlen(force_user) > 1 )
    {
        opt = g_strdup_printf("force user = %s\n", force_user);
        strcat(new_conf, opt);
    }

    if( force_group != NULL && strlen(force_group) > 1 )
    {
        opt = g_strdup_printf("force group = %s\n", force_group);
        strcat(new_conf, opt);
    }

    /* Combos... */

    active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->share_set_combo[0]));
    if( active_index == 0 )
        opt = g_strdup_printf("read only = yes\n");
    else
        opt = g_strdup_printf("read only = no\n");
    strcat(new_conf, opt);

    active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->share_set_combo[1]));
    if( active_index == 0 )
        opt = g_strdup_printf("available = yes\n");
    else
        opt = g_strdup_printf("available = no\n");
    strcat(new_conf, opt);

    active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->share_set_combo[2]));
    if( active_index == 0 )
        opt = g_strdup_printf("browseable = yes\n");
    else
        opt = g_strdup_printf("browseable = no\n");
    strcat(new_conf, opt);

    active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->share_set_combo[3]));
    if( active_index == 0 )
        opt = g_strdup_printf("writable = yes\n");
    else
        opt = g_strdup_printf("writable = no\n");
    strcat(new_conf, opt);

    active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->share_set_combo[4]));
    if( active_index == 0 ) /* Reverse no to yes */
        opt = g_strdup_printf("guest ok = no\n");
    else
        opt = g_strdup_printf("guest ok = yes\n");
    strcat(new_conf, opt);

    active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->share_set_combo[6]));
    if( active_index == 0 )
        opt = g_strdup_printf("printable = yes\n");
    else
        opt = g_strdup_printf("printable = no\n");
    strcat(new_conf, opt);

    active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->share_set_combo[7]));
    if( active_index == 0 )
        opt = g_strdup_printf("locking = yes\n");
    else
        opt = g_strdup_printf("locking = no\n");
    strcat(new_conf, opt);

    active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->share_set_combo[8]));
    if( active_index == 0 )
        opt = g_strdup_printf("strict locking = yes\n");
    else
        opt = g_strdup_printf("strict locking = no\n");
    strcat(new_conf, opt);

    g_free(opt);


    /* Make the directory if it doesnt exist */
    /* Fixme: Add dir if( ! sys_users_dir() ) Check /path/AllUsers.
       If /home/sysuser, dont add the directory and inform the user */
    dir = g_strdup_printf("%s", directory);
    if( ! file_exists(dir) )
    {
        cmd = g_strdup_printf("mkdir -p '%s'", dir);
        if((fp = popen(cmd, "w")) == NULL)
            perror("popen");
        pclose(fp);
        g_free(cmd);
        /* Chmod it to 777 otherwise "write list =" wont work unless
           chown is used. In that case "admin users =" is the only thing
           that works for 2 or more users if they arent in the same group. */
        cmd = g_strdup_printf("chmod 777 '%s'", dir);
        if((fp = popen(cmd, "w")) == NULL)
            perror("popen");
        pclose(fp);
        g_free(cmd);
    }
    g_free(dir);

    /* Write the new conf */
    if((fp = fopen(SAMBA_CONF, "w+")) == NULL)
    {
        info = g_strdup_printf(_("Can not write new samba configuration here:\n%s\n"), SAMBA_CONF);
        show_info(info);
        g_free(info);
        return;
    }
    fputs(new_conf, fp);
    fclose(fp);
    free(new_conf);


    cmp_global_share_name = g_strdup_printf("%s", (char *)global_share_name);
//    cmp_global_share_directory  = g_strdup_printf("%s", global_share_directory);
//    cmp_global_share_comment    = g_strdup_printf("%s", global_share_comment);

    // Fixme: Also check directory and comment (listed in the treeview)
    /* Dont repopulate if the sharename,directory or comment is unchanged */
    if( strcmp(cmp_global_share_name, share_name) )
    {
        populate_shares(widgets);
        populate_share_settings(widgets);
    }
    g_free(cmp_global_share_name);

    fix_newlines_in_conf();

    populate_conf_tab(widgets);

    reread_conf(widgets);
}
