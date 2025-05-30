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
#include <string.h>
#include "widgets.h"
#include "gettext.h"
#include "functions.h"
#include "show_info.h"
#include "allocate.h"
#include "add_change_user.h"
#include "populate_users.h"
#include "select_first_user.h"
#include "populate_user_settings.h"
#include "populate_shell_combo.h"
#include "commands.h"

int user_profiling_info_shown = 0;

extern int activated;


/* The add button clears the user profile and informs the user about user profiling */
void clear_user_profile(struct w *widgets)
{
    gchar *info;

    /* Same default values as in functions.c's get_user_setting() */
    gtk_entry_set_text(GTK_ENTRY(widgets->user_set_entry[0]), "user1");
    gtk_entry_set_text(GTK_ENTRY(widgets->user_set_entry[1]), "");
    gtk_entry_set_text(GTK_ENTRY(widgets->user_set_entry[2]), "group1");
    gtk_entry_set_text(GTK_ENTRY(widgets->user_set_entry[3]), _("Comment"));
    gtk_entry_set_text(GTK_ENTRY(widgets->user_set_entry[4]), "/dev/null");

    /* Set account locked to the unlocked state */
    //gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->user_set_checkbutton[0]), FALSE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->user_set_checkbutton[0]), FALSE);

    /* Repopulate shells */
    populate_shell_combo(widgets->user_set_combo[0]);

    /* Only show profiling info once */
    if( user_profiling_info_shown )
        return;
    else
        user_profiling_info_shown = 1;

    info = g_strconcat(_("Write the new users name and other settings in the user profile below.\n"),
                       _("Press apply to add this new user with the selected settings.\n\n"),
                       _("You can also skip this step and directly alter an existing users name and\n"),
                       _("settings and then press apply. This can save administration time.\n"), NULL);
    show_info(info);
    g_free(info);
}


void account_locked_checkbutton_clicked(struct w *widgets)
{
    FILE *fp;
    int locked = 0;
    int have_password = 0;
    gchar *info, *cmd;
    const gchar *username, *password;

    username = gtk_entry_get_text(GTK_ENTRY(widgets->user_set_entry[0]));
    password = gtk_entry_get_text(GTK_ENTRY(widgets->user_set_entry[1]));

    if( password != NULL && strlen(password) >= atoi(MIN_PASS_LEN) )
        have_password = 1;

    if( ! smbuser_exists(username) )
    {
/*	Dont show this popup. 
	info = g_strdup_printf(_("Can not lock or unlock a samba user that does not exist.\n"));
	show_info(info);
	g_free(info);
*/
        return;
    }

    /* Get the samba accounts lock state */
    locked = smbuser_locked(username);

    /* Sanitycheck. Not required anymore */
    if( username == NULL || strlen(username) < 1 )
    {
        info = g_strdup_printf(_("A username must be specified.\n"));
        show_info(info);
        g_free(info);

        /* Set account locked checkbutton [x] if its still locked */
        // FIXME: treats users with NULL passwords as locked.
        if( locked )
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->user_set_checkbutton[0]), TRUE);
        else
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->user_set_checkbutton[0]), FALSE);

        return;
    }

    /* Disable an existing samba account if selected */
    if( ! locked && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets->user_set_checkbutton[0])) )
    {
        cmd = g_strdup_printf("%s -c \"[D ]\" -u '%s'", PDBEDIT_BINARY, username);
        if((fp = popen(cmd, "w")) == NULL)
        {
            perror("popen");
            info = g_strdup_printf(_("Failed to lock samba account: %s\n"), username);
            show_info(info);
            g_free(info);
            g_free(cmd);

            /* Set account locked checkbutton [x] if its still locked */
            if( locked )
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->user_set_checkbutton[0]), TRUE);
            else
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->user_set_checkbutton[0]), FALSE);

            return;
        }
        g_free(cmd);
        fclose(fp);
    }

    /* Enable an existing samba account if thats selected */
    if( locked && ! gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets->user_set_checkbutton[0])) )
    {
        /* No password given and the user already had a password before
           beeing disabled so just enable the account */
        if( ! have_password && locked == 1 )
        {
            cmd = g_strdup_printf("%s -c \"[ ]\" -u '%s'", PDBEDIT_BINARY, username);
            if((fp = popen(cmd, "w")) == NULL)
            {
                perror("popen");
                info = g_strdup_printf(_("Failed to unlock samba account: %s using no password\n"), username);
                show_info(info);
                g_free(info);
                g_free(cmd);
                /* Set account locked checkbutton [x] if its still locked */
                if( locked )
                    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->user_set_checkbutton[0]), TRUE);
                else
                    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->user_set_checkbutton[0]), FALSE);

                return;
            }
        }

        /* The user had no password before beeing locked or has a NULL password so require a new password */
        if( (have_password) && (locked == 1 || locked == 2 || locked == 3) )
        {
            cmd = g_strdup_printf("%s -t -a -u '%s'", PDBEDIT_BINARY, username);
            if((fp = popen(cmd, "w")) == NULL)
            {
                perror("popen");
                info = g_strdup_printf(_("Failed to unlock samba account: %s using a password\n"), username);
                show_info(info);
                g_free(info);
                g_free(cmd);

                /* Set account locked checkbutton [x] if its still locked */
                if( locked )
                    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->user_set_checkbutton[0]), TRUE);
                else
                    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->user_set_checkbutton[0]), FALSE);

                return;
            }
            fputs(password, fp);
            fputs("\n", fp);
            fputs(password, fp);
            fputs("\n", fp);
            pclose(fp);
            g_free(cmd);
        }

        if( (! have_password) && (locked == 2 || locked == 3) )
        {
            info = g_strconcat(_("Failed to unlock samba account: "), username, "\n",
                    _("A new password is required to unlock users that have no previous\n"),
                    _("passwords or empty passwords. The provided password is too short.\n"), NULL);
            show_info(info);
            g_free(info);

            /* Set account locked checkbutton [x] if its still locked */
            if( locked )
                //gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->user_set_checkbutton[0]), TRUE);
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->user_set_checkbutton[0]), TRUE);
            else
                //gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->user_set_checkbutton[0]), FALSE);
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets->user_set_checkbutton[0]), FALSE);
        }
    }

    /* Examples where a password is required for unlocking [ND ...] (NULL/Disabled):
       pommes:534:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX:[DU ]:LCT-00000000:
       frittes:517:NO PASSWORDXXXXXXXXXXXXXXXXXXXXX:NO PASSWORDXXXXXXXXXXXXXXXXXXXXX:[NU ]:LCT-00000000:
     */
}


/* Add or change the system and samba user */
void add_change_user(struct w *widgets)
{
    FILE *fp;
    gchar *info, *path, *cmd;

    const gchar *username;
    const gchar *password;
    const gchar *groupname;
    const gchar *comment;
    const gchar *homedir;
    const gchar *shell;

    /* Get the users settings */
    username = gtk_entry_get_text(GTK_ENTRY(widgets->user_set_entry[0]));
    password = gtk_entry_get_text(GTK_ENTRY(widgets->user_set_entry[1]));
    groupname = gtk_entry_get_text(GTK_ENTRY(widgets->user_set_entry[2]));
    comment = gtk_entry_get_text(GTK_ENTRY(widgets->user_set_entry[3]));
    homedir = gtk_entry_get_text(GTK_ENTRY(widgets->user_set_entry[4]));
    /* Its a gtk combo box entry new text */
    GtkTreeIter iter;
    GtkListStore *store;
    
    store = GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(widgets->user_set_combo[0])));
    gtk_combo_box_get_active_iter(GTK_COMBO_BOX(widgets->user_set_combo[0]), &iter);
    gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, 0, &shell, -1);
        
    //shell = gtk_entry_get_text(GTK_ENTRY(gtk_container_get_children(GTK_BIN(widgets->user_set_combo[0]))));


    /* Puts the pdbedit userlist as:
       /etc/gadmin-samba/gadmin_samba_users */
    get_gadmin_samba_users();


    if( ! smbuser_exists(username) && (password == NULL || strlen(password) < atoi(MIN_PASS_LEN)) )
    {
        info = g_strdup_printf(_("You must specify a valid password for this new user.\n"));
        show_info(info);
        g_free(info);
        return;
    }

    path = g_strdup_printf("%s", GADMIN_SAMBA_USERS);
    if( ! file_exists(path) )
        make_file_chmod(path, "0600");

    if( ! file_exists(path) )
    {
        info = g_strdup_printf(_("Failed to create missing samba password file:\n%s\n"), path);
        show_info(info);
        g_free(info);
        g_free(path);
        return;
    }
    g_free(path);


    /* Sanitychecks... */
    if( username == NULL || strlen(username) < 1 )
    {
        info = g_strdup_printf(_("A username must be specified.\n"));
        show_info(info);
        g_free(info);
        return;
    }
    if( groupname == NULL || strlen(groupname) < 1 )
    {
        info = g_strdup_printf(_("A groupname must be specified.\n"));
        show_info(info);
        g_free(info);
        return;
    }
    if( comment == NULL || strlen(comment) < 1 )
    {
        info = g_strdup_printf(_("A comment must be specified.\n"));
        show_info(info);
        g_free(info);
        return;
    }
    if( homedir == NULL || strlen(homedir) < 2 )
    {
        info = g_strdup_printf(_("A home directory must be specified.\n"));
        show_info(info);
        g_free(info);
        return;
    }
    if( shell == NULL || strlen(shell) < 2 )
    {
        info = g_strdup_printf(_("A shell must be specified.\n"));
        show_info(info);
        g_free(info);
        return;
    }

    /* Root user specified, only change its samba settings */
    if( strcmp(username, "root") == 0 )
    {
        /* If the root system user doesnt exist, tell the user and return. */
        if( ! user_exists(username) )
        {
            info = g_strdup_printf(_("The root system user doesnt exist, nothing was done.\n"));
            show_info(info);
            g_free(info);
            return;
        }

        if( password != NULL && strlen(password) >= atoi(MIN_PASS_LEN) )
        {
            cmd = g_strdup_printf("%s -t -a -u '%s'", PDBEDIT_BINARY, username);
            if((fp = popen(cmd, "w")) == NULL)
            {
                perror("popen");
                info = g_strdup_printf(_("Failed changing samba password or adding samba user: %s\n"), username);
                show_info(info);
                g_free(info);
                g_free(cmd);
                return;
            }
            fputs(password, fp);
            fputs("\n", fp);
            fputs(password, fp);
            fputs("\n", fp);
            pclose(fp);
            g_free(cmd);

            info = g_strdup_printf(_("The samba password for root was changed.\nNothing else was changed for this user.\n"));
            show_info(info);
            g_free(info);
            return;
        }
        else
        {
            info = g_strdup_printf(_("The password is too short, adding or changing samba user root failed.\n"));
            show_info(info);
            g_free(info);
            return;
        }

        /* End of modifications for samba user root */
        return;
    }


    /* Non root user modification follows... */


    /* The system user doesnt exist, add it */
    if( ! user_exists(username) )
    {
        /* Add the system group if it doesnt exist */
        if( ! group_exists(groupname) )
        {
            cmd = g_strdup_printf("%s %s", "groupadd", groupname);
            if( ! run_command(cmd) )
            {
                printf("Error adding system group: %s\n", groupname);
                printf("The command was: %s\n", cmd);
                g_free(cmd);
                return;
            }
            g_free(cmd);
        }

        /* Add the user to the specified group */
        cmd = g_strdup_printf("%s '%s' -d '%s' -c '%s' -s '%s' -g '%s'", "useradd",
                              username, homedir, comment, shell, groupname);
        if( ! run_command(cmd) )
        {
            info = g_strdup_printf(_("Error adding system user: %s\nThe command was: %s\n"), username, cmd);
            show_info(info);
            g_free(info);
            g_free(cmd);
            return;
        }
        g_free(cmd);
    }


    /* Make sure that the system user exists */
    if( ! user_exists(username) )
    {
        info = g_strdup_printf(_("Error adding system user: %s\n"), username);
        show_info(info);
        g_free(info);
        return;
    }


    /* The system user exists, modify its system and samba settings... */


    /* Add a new samba user with this password or change the old password. */
    if( password != NULL && strlen(password) >= atoi(MIN_PASS_LEN) )
    {
        cmd = g_strdup_printf("%s -t -a -u '%s'", PDBEDIT_BINARY, username);
        if((fp = popen(cmd, "w")) == NULL)
        {
            perror("popen");
            info = g_strdup_printf(_("Failed changing password or adding samba user: %s\n"), username);
            show_info(info);
            g_free(info);
            g_free(cmd);
            return;
        }
        fputs(password, fp);
        fputs("\n", fp);
        fputs(password, fp);
        fputs("\n", fp);
        pclose(fp);
        g_free(cmd);

        /* Also change the system accounts password. */
        password_user(username, password);
    }


    /* Change this users system comment */
    cmd = g_strdup_printf("%s -c '%s' '%s'\n", "usermod", comment, username);
    if((fp = popen(cmd, "w")) == NULL)
    {
        perror("popen");
        info = g_strdup_printf(_("Failed changing system comment for samba user: %s\n"), username);
        show_info(info);
        g_free(info);
        g_free(cmd);
        return;
    }
    pclose(fp);
    g_free(cmd);

    /* Maybe fix: Change other settings as well */
    //    printf("Selected group : %s\n", groupname);
    //    printf("Selected home: %s\n", homedir);
    //    printf("Selected Shell: %s\n", shell);


    /* Create directory: /var/lib/samba/profiles/UserName */
    cmd = g_strdup_printf("mkdir -p '%s/lib/samba/profiles/%s'", LOCALSTATEDIR, username);
    if( ! run_command(cmd) )
    {
        info = g_strdup_printf(_("Error creating directory: %s/lib/samba/profiles/%s\n"),
                                 LOCALSTATEDIR, username);
        show_info(info);
        g_free(info);
        g_free(cmd);
        return;
    }
    g_free(cmd);
    /* Chown directory: /var/lib/samba/profiles/UserName to this user:group */
    cmd = g_strdup_printf("chown %s:%s %s/lib/samba/profiles/%s",
                          username, groupname, LOCALSTATEDIR, username);
    if( ! run_command(cmd) )
    {
        info = g_strdup_printf(_("Error chowning directory: %s/lib/samba/profiles/%s\n"),
                               LOCALSTATEDIR, username);
        show_info(info);
        g_free(info);
        g_free(cmd);
        return;
    }
    g_free(cmd);


    /* Clear the password field */
    gtk_entry_set_text(GTK_ENTRY(widgets->user_set_entry[1]), "");

    /* Update the user tab */
    populate_users(widgets);
    select_first_user(widgets); /* Fixme: Should be scroll to and select this user */
    populate_user_settings(widgets);
}
