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
#include "gettext.h"
#include "support.h"
#include "widgets.h"
/* For getuid */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include "create_main_window.h"
#include "gadmin_samba.h"
#include "commands.h"
#include "set_version.h"
#include "standard_conf.h"
#include "add_standard_users.h"
#include "create_server_tab.h"
#include "create_server_settings.h"
#include "create_user_tab.h"
#include "create_user_settings.h"
#include "create_share_tab.h"
#include "create_share_settings.h"
#include "create_macct_tab.h"
#include "create_macct_settings.h"
#include "create_conn_tab.h"
#include "create_disc_tab.h"
#include "create_security_tab.h"
#include "create_conf_tab.h"
#include "populate_gadmin_samba.h"
#include "show_info.h"
#include "load_settings.h"
#include "status_update.h"

int MAX_READ_POPEN = 16384;
int activated = 0;
int global_smbd_activated = 0;
int global_nmbd_activated = 0;
int global_winbindd_activated = 0;
int global_share_access_treeview_num = 0;

/* Wether or not to start winbindd */
int global_start_winbindd = 1;

int info_window_exit_main = 0;

char global_user_name[1024] = "";
char global_share_name[1024] = "";
char global_share_comment[1024] = "";
char global_share_directory[16384] = "";
char global_macct_name[1024] = "";

/* Default values for the importer */
int global_import_local = 1;
int global_import_remote = 0;

int global_import_users = 1;
int global_import_groups = 0;

int global_import_valid = 1;
int global_import_invalid = 0;
int global_import_write = 0;
int global_import_admin = 0;



int main(int argc, char *argv[])
{
    gchar *gadmin_samba_settings, *conf_path, *cmd, *info;
    int i = 0;

#ifdef ENABLE_NLS
    bindtextdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);
#endif

    //gtk_set_locale();
    gtk_init(&argc, &argv);

    wid *widgets = g_malloc(sizeof(wid));

    widgets->default_conf_question_window = NULL;

    /* Non root usage */
    if( ! getuid() == 0 )
    {
        /* For setting a different exit method in the info window */
        info_window_exit_main = 1;

        info = g_strdup_printf("You must be root to run: %s\nThis window will close in 10 seconds.\n",
            PACKAGE);
        show_info(info);
        g_free(info);

        /* Users can close the info window earlier then the timeout */
        for(i = 0; i < 10; i++)
        {
            while( gtk_events_pending() )
                gtk_main_iteration();

            /* Set when close info window is clicked */
            if( info_window_exit_main == 2 )
                break;

            usleep(100000 * 10);
        }

        g_free(widgets);
        return 0;
    }

    /* Add standard users (nobody etc) also makes sure
       that we have sambas configuration directory and required files */
    add_standard_users(widgets);


    /* Create the main window */
    create_main_window(widgets);

    /* Set SAMBA Version */
    set_version(widgets);

    /* Create the server tab */
    create_server_tab(widgets);

    /* Create the server settings */
    create_server_settings(widgets);

    /* Create the user tab */
    create_user_tab(widgets);

    /* Create the user settings */
    create_user_settings(widgets);

    /* Create the share tab */
    create_share_tab(widgets);
    create_share_settings(widgets);

    /* Create the machine account tab */
    create_macct_tab(widgets);
    create_macct_settings(widgets);

    /* Create the connection tab */
    create_conn_tab(widgets);

    /* Create the disc tab */
    create_disc_tab(widgets);

    /* Create the security tab */
    create_security_tab(widgets);

    /* Create the conf tab */
    create_conf_tab(widgets);

    /* Load settings from gadmin_sambas settings file */
    load_settings(widgets);


    /* Move the old settings directory as the new /etc/gadmin-samba directory.
       This is used for smoothly moving from gsambad to gadmin-samba */
    cmd = g_strdup_printf("test -e %s/gsambad && mv %s/gsambad %s/gadmin-samba || exit 0",
        SYSCONFDIR, SYSCONFDIR, SYSCONFDIR);
    if( ! run_command(cmd) )
    {
        printf("Error running transitional command: %s\n", cmd);
    }
    g_free(cmd);


    conf_path = g_strdup_printf("%s", SAMBA_CONF);
    gadmin_samba_settings = g_strdup_printf("%s", SETTINGS_FILE);

    /* If theres no smb.conf, add one and populate the entire gui */
    if( ! file_exists(conf_path) )
    {
        add_standard_conf(widgets);
        populate_gadmin_samba(widgets);
    }
    else
        /* Ask to add a new conf if we havnt done so before. 
           Depends on show_conf_question: yes/no in gadmin-sambas settings */
    if( ! conf_ok(gadmin_samba_settings) )
        create_standard_conf_question(widgets);
    else
        /* The conf is ok, populate the entire gui */
        populate_gadmin_samba(widgets);

    g_free(conf_path);
    g_free(gadmin_samba_settings);


    /* Always backup the samba configuration when new gadmin-samba versions are released */
    backup_samba_conf(widgets);


    /* Window close button (x) */
    g_signal_connect(G_OBJECT(widgets->main_window), "destroy",
        G_CALLBACK(gtk_main_quit), NULL);

    /* Activated/Deactivated status */
    g_timeout_add(1000, (GSourceFunc) status_update, widgets);

    gtk_main();

    g_free(widgets);

    return 0;
}
