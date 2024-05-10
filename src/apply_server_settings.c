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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "widgets.h"
#include "gettext.h"
#include "allocate.h"
#include "commented.h"
#include "reread_conf.h"
#include "show_info.h"
#include "functions.h"
#include "apply_server_settings.h"
#include "populate_server_settings.h"
#include "populate_conf_tab.h"

extern int activated;

/* Write the new file or just print the changes */
#define WRITE_CHANGES 1



void apply_server_settings(struct w *widgets)
{
    /* Change the selected servers configuration. */
    FILE *fp;
    long conf_size;
    gint active_index = 0, sec_index = 0;
    gchar *info;
    int global_section_found = 0;
    int sec_changed = 0;
    int dom_logons = 0;
    char *line, *conf_line, *new_conf;

    /* Entries */
    G_CONST_RETURN gchar *server_name;
    G_CONST_RETURN gchar *server_comment;
    G_CONST_RETURN gchar *domain_workgroup;
    G_CONST_RETURN gchar *realm;
    G_CONST_RETURN gchar *allowed_nets;
    G_CONST_RETURN gchar *listen_nets;
    G_CONST_RETURN gchar *announce_server_to;
    G_CONST_RETURN gchar *retrieve_announce_from;
    G_CONST_RETURN gchar *printcap_path;
    G_CONST_RETURN gchar *guest_users;
    G_CONST_RETURN gchar *samba_logfile;
    G_CONST_RETURN gchar *samba_password_program;
    G_CONST_RETURN gchar *new_pass_phrase;
    G_CONST_RETURN gchar *samba_username_mapfile;
    G_CONST_RETURN gchar *name_resolve_order;
    G_CONST_RETURN gchar *socket_options;
    G_CONST_RETURN gchar *wins_servers;
    G_CONST_RETURN gchar *logon_drive;
    G_CONST_RETURN gchar *logon_home;
    G_CONST_RETURN gchar *logon_path;
    G_CONST_RETURN gchar *logon_script;
//    G_CONST_RETURN gchar *samba_root_home;
    G_CONST_RETURN gchar *add_user_script;
    G_CONST_RETURN gchar *add_user_to_group_script;
    G_CONST_RETURN gchar *add_group_script;
    G_CONST_RETURN gchar *del_user_script;
    G_CONST_RETURN gchar *del_user_from_group_script;
    G_CONST_RETURN gchar *del_group_script;
    G_CONST_RETURN gchar *add_machine_script;
    G_CONST_RETURN gchar *winbind_separator;
    G_CONST_RETURN gchar *idmap_uid;
    G_CONST_RETURN gchar *idmap_gid;
    G_CONST_RETURN gchar *template_shell;


    /* Spinbuttons */
    G_CONST_RETURN gchar *max_log_file_size;
    G_CONST_RETURN gchar *username_match_level;
    G_CONST_RETURN gchar *password_match_level;
    G_CONST_RETURN gchar *election_level;
    G_CONST_RETURN gchar *password_chat_timeout;
    G_CONST_RETURN gchar *machine_password_timeout;
    G_CONST_RETURN gchar *winbind_cache_timeout;


    /* If the security level combo is changed, change all other relevant options. */
    sec_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[0]));

    /* Compare the current security level and domain logons option */
    if((fp = fopen(SAMBA_CONF, "r")) == NULL)
    {
        info = g_strdup_printf(_("Cant open smb.conf here: \n%s\n"), SAMBA_CONF);
        show_info(info);
        g_free(info);
        return;
    }
    fseek(fp, 0, SEEK_END);
    conf_size = ftell(fp);
    rewind(fp);

    line = allocate(conf_size + 1);

    /* Check if domain logons is set. Then its atleast a BDC */
    if( conf_size > 1 )
    while(fgets(line, conf_size, fp) != NULL)
    {
        if( strstr(line, "domain logons") && strstr(line, "yes") && ! commented(line) )
        {
            dom_logons = 1;
            break;
        }

        /* A share is found, break. */
        if( strstr(line, "[") && ! strstr(line, "[global]") && ! commented(line) )
            break;
    }

    rewind(fp);

    if( conf_size > 1 )
    while(fgets(line, conf_size, fp) != NULL)
    {
        if( strstr(line, "security =") && strstr(line, "user")
            && ! dom_logons && sec_index != 0 && ! commented(line) && ! sec_changed)
            sec_changed = 1;
        else
        if( strstr(line, "security =") && strstr(line, "user")
            && dom_logons && sec_index != 1 && ! commented(line) && ! sec_changed)
            sec_changed = 1;
        else
        if( strstr(line, "security =") && strstr(line, "domain")
            && sec_index != 2 && ! commented(line) && ! sec_changed)
            sec_changed = 1;
        else
        if( strstr(line, "security =") && strstr(line, "ads")
            && sec_index != 3 && ! commented(line) && ! sec_changed)
            sec_changed = 1;

        if( sec_changed )
            break;

        /* A share is found, break. */
        if( strstr(line, "[") && ! strstr(line, "[global]") && ! commented(line) )
            break;
    }
    fclose(fp);
    free(line);

    if( ! strstr(SAMBA_CONF, "smb.conf") )
    {
        info = g_strdup_printf(_
                    ("The define for SAMBA_CONF is bad, please reinstall the application.\n"));
        show_info(info);
        g_free(info);
        return;
    }



    /* Get settings from the entries */
    server_name = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[0]));
    server_comment = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[1]));
    domain_workgroup = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[2]));
    realm = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[3]));
    allowed_nets = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[4]));
    listen_nets = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[5]));
    announce_server_to = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[6]));
    retrieve_announce_from = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[7]));
    printcap_path = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[8]));
    guest_users = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[9]));
    samba_logfile = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[10]));
    socket_options = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[11]));
    logon_drive = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[12]));
    logon_home = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[13]));
    logon_path = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[14]));
    logon_script = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[15]));
    name_resolve_order = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[16]));
    wins_servers = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[17]));
    samba_username_mapfile = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[18]));
    samba_password_program = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[20]));
//    samba_root_home = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[21]));
    new_pass_phrase = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[22]));
    add_user_script = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[23]));
    add_user_to_group_script = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[24]));
    add_group_script = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[25]));
    del_user_script = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[26]));
    del_user_from_group_script = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[27]));
    del_group_script = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[28]));
    add_machine_script = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[29]));
    idmap_uid = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[30]));
    idmap_gid = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[31]));
    template_shell = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[32]));
    winbind_separator = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_entry[33]));


    /* Get settings from the spinbuttons */
    max_log_file_size = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_spinbutton[0]));
    username_match_level = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_spinbutton[1]));
    password_match_level = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_spinbutton[2]));
    election_level = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_spinbutton[3]));
    password_chat_timeout = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_spinbutton[4]));
    machine_password_timeout = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_spinbutton[5]));
    winbind_cache_timeout = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_spinbutton[6]));


    if( strlen(server_name) == 0 )
    {
        info = g_strdup_printf(_("Please specify the server name.\n"));
        show_info(info);
        g_free(info);
        return;
    }
    if( strlen(domain_workgroup) == 0 )
    {
        info = g_strdup_printf(_("Please specify a workgroup or domain name.\n"));
        show_info(info);
        g_free(info);
        return;
    }


    /* Change the server configuration. */
    if((fp = fopen(SAMBA_CONF, "r")) == NULL)
    {
        info = g_strdup_printf(_("Cant open smb.conf here: \n%s\n"), SAMBA_CONF);
        show_info(info);
        g_free(info);
        return;
    }
    fseek(fp, 0, SEEK_END);
    conf_size = ftell(fp);
    rewind(fp);

    line = allocate(conf_size + 1);
    conf_line = allocate(conf_size + 1024);
    /* Additional bytes for changes, 350 chars per 
       entry widget * 100 widgets + 20000 for the directives */
    new_conf = allocate(conf_size + (350 * 100) + 20000);

    /* First scroll to the global section */
    if( conf_size > 1 )
    while(fgets(line, conf_size, fp) != NULL)
    {
        strcat(new_conf, line);
        if( strstr(line, "[global]") && ! commented(line) )
        {
            global_section_found = 1;
            break;
        }
    }

    if( ! global_section_found )
    {
        info = g_strdup_printf(_("Can not find the [global] section in %s\nNo changes made.\n"),
                                 SAMBA_CONF);
        show_info(info);
        g_free(info);
        fclose(fp);
        free(line);
        free(conf_line);
        free(new_conf);
        return;
    }


    if( conf_size > 1 )
    while(fgets(line, conf_size, fp) != NULL)
    {
        if( commented(line) )
            continue;

        if( strlen(line) > 4000 )
        {
            info = g_strdup_printf(_("A line with over 4000 chars is not valid in: %s\n"),
                                     SAMBA_CONF);
            show_info(info);
            g_free(info);
            fclose(fp);
            free(line);
            free(conf_line);
            free(new_conf);
            return;
        }

        /* Break at any share definition "[" and collect them in the next while loop */
        if( strstr(line, "[") && strstr(line, "]") )
        {
            /* Add the "wins server =" option if an IP or DNS name is specified. */
            if( wins_servers != NULL && strlen(wins_servers) > 7 )
            {
                sprintf(conf_line, "wins server = %s\n", wins_servers);
                strcat(new_conf, conf_line);
            }

            strcat(new_conf, line);
            break;
        }

        /* Change matching server directives otherwise just gather them */
        if( strstr(line, "netbios name") )
        {
            sprintf(conf_line, "netbios name = %s\n", server_name);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "server string") )
        {
            sprintf(conf_line, "server string = %s\n", server_comment);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "workgroup") )
        {
            sprintf(conf_line, "workgroup = %s\n", domain_workgroup);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "hosts allow") )
        {
            sprintf(conf_line, "hosts allow = %s\n", allowed_nets);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "interfaces") && ! strstr(line, "bind interfaces only") )
        {
            sprintf(conf_line, "interfaces = %s\n", listen_nets);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "remote announce") )
        {
            sprintf(conf_line, "remote announce = %s\n", announce_server_to);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "remote browse sync") )
        {
            sprintf(conf_line, "remote browse sync = %s\n", retrieve_announce_from);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "printcap name") )
        {
            sprintf(conf_line, "printcap name = %s\n", printcap_path);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "guest account") )
        {
            sprintf(conf_line, "guest account = %s\n", guest_users);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "log file") )
        {
            sprintf(conf_line, "log file = %s\n", samba_logfile);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "username map") )
        {
            sprintf(conf_line, "username map = %s\n", samba_username_mapfile);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "add user script") )
        {
            sprintf(conf_line, "add user script = %s\n", add_user_script);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "passwd program") )
        {
            sprintf(conf_line, "passwd program = %s\n", samba_password_program);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "passwd chat") && ! strstr(line, "passwd chat timeout") )
        {
            sprintf(conf_line, "passwd chat = %s\n", new_pass_phrase);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "socket options") )
        {
            sprintf(conf_line, "socket options = %s\n", socket_options);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "name resolve order") )
        {
            sprintf(conf_line, "name resolve order = %s\n", name_resolve_order);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "wins server") )
        {
            /* Remove this line/option and add it last if an IP or DNS name is given. */
/*
	    sprintf(conf_line, "wins server = %s\n", wins_servers);
	    strcat(new_conf, conf_line);
*/
        }
        else if( strstr(line, "idmap uid") )
        {
            sprintf(conf_line, "idmap uid = %s\n", idmap_uid);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "idmap gid") )
        {
            sprintf(conf_line, "idmap gid = %s\n", idmap_gid);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "template shell") )
        {
            sprintf(conf_line, "template shell = %s\n", template_shell);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "logon drive") )
        {
            sprintf(conf_line, "logon drive = %s\n", logon_drive);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "logon home") )
        {
            sprintf(conf_line, "logon home = %s\n", logon_home);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "logon path") )
        {
            sprintf(conf_line, "logon path = %s\n", logon_path);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "logon script") )
        {
            sprintf(conf_line, "logon script = %s\n", logon_script);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "realm") )
        {
            sprintf(conf_line, "realm = %s\n", realm);
            strcat(new_conf, conf_line);
        }
        else
/* Doesnt seem to work	if( strstr(line, "root directory") )
{
	    sprintf(conf_line, "root directory = %s\n", samba_root_home);
	    strcat(new_conf, conf_line);
}
else
*/
        if( strstr(line, "add user script") )
        {
            sprintf(conf_line, "add user script = %s\n", add_user_script);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "add user to group script") )
        {
            sprintf(conf_line, "add user to group script = %s\n", add_user_to_group_script);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "add group script") )
        {
            sprintf(conf_line, "add group script = %s\n", add_group_script);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "del user script") )
        {
            sprintf(conf_line, "del user script = %s\n", del_user_script);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "del user from group script") )
        {
            sprintf(conf_line, "del user from group script = %s\n", del_user_from_group_script);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "del group script") )
        {
            sprintf(conf_line, "del group script = %s\n", del_group_script);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "add machine script") )
        {
            sprintf(conf_line, "add machine script = %s\n", add_machine_script);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "winbind separator") )
        {
            sprintf(conf_line, "winbind separator = %s\n", winbind_separator);
            strcat(new_conf, conf_line);
        }
        else


            /* Combos */


            /* Security level */
        if( strstr(line, "security =") || strstr(line, "security=") )
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[0]));
            if( active_index == 0 )
                sprintf(conf_line, "security = user\n");
            else if( active_index == 1 )  /* Backup domain */
                sprintf(conf_line, "security = user\n");
            else if( active_index == 2 )  /* Primary domain */
                sprintf(conf_line, "security = domain\n");
            else if( active_index == 3 )
                sprintf(conf_line, "security = ads\n");
            else
                sprintf(conf_line, "security = user\n");    /* Default to user sec level */

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "load printers") )
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[1]));
            if( active_index == 0 )
                sprintf(conf_line, "load printers = yes\n");
            else
                sprintf(conf_line, "load printers = no\n");

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "cups options") )
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[2]));
            if( active_index == 0 )
                sprintf(conf_line, "cups options = raw\n");
            else
                sprintf(conf_line, "cups options =\n");

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "null passwords") )
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[3]));
            if( active_index == 0 )
                sprintf(conf_line, "null passwords = yes\n");
            else
                sprintf(conf_line, "null passwords = no\n");

            strcat(new_conf, conf_line);
        }
        else /* Security level change dependant */ if(strstr(line, "encrypt passwords"))
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[4]));
            if( sec_changed && (sec_index == 1 || sec_index == 2 || sec_index == 3) )
                sprintf(conf_line, "encrypt passwords = yes\n");
            else if( sec_changed && sec_index == 0 )
                sprintf(conf_line, "encrypt passwords = yes\n");    /* We still want this for user sec */
            else if( active_index == 0 )
                sprintf(conf_line, "encrypt passwords = yes\n");
            else
                sprintf(conf_line, "encrypt passwords = no\n");

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "unix password sync") )
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[5]));
            if( sec_changed && (sec_index == 1 || sec_index == 2 || sec_index == 3) )
                sprintf(conf_line, "unix password sync = yes\n");
            else if( sec_changed && sec_index == 0 )
                sprintf(conf_line, "unix password sync = yes\n");   /* We still want this for user sec */
            else if( active_index == 0 )
                sprintf(conf_line, "unix password sync = yes\n");
            else
                sprintf(conf_line, "unix password sync = no\n");

            strcat(new_conf, conf_line);
        }
        else /* Security level change dependant */ if(strstr(line, "local master"))
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[6]));
            if( sec_changed && (sec_index == 1 || sec_index == 2 || sec_index == 3) )
                sprintf(conf_line, "local master = yes\n");
            else if( sec_changed && sec_index == 0 )
                sprintf(conf_line, "local master = no\n");
            else if( active_index == 0 )
                sprintf(conf_line, "local master = yes\n");
            else
                sprintf(conf_line, "local master = no\n");

            strcat(new_conf, conf_line);
        }
        else /* Security level change dependant (Auto: no for User and BDC) */ if(strstr(line,
            "domain master"))
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[7]));
            if( sec_changed && (sec_index == 2 || sec_index == 3) )
                sprintf(conf_line, "domain master = yes\n");
            else if( sec_changed && (sec_index == 0 || sec_index == 1) )
                sprintf(conf_line, "domain master = no\n");
            else if( active_index == 0 )
                sprintf(conf_line, "domain master = yes\n");
            else
                sprintf(conf_line, "domain master = no\n");

            strcat(new_conf, conf_line);
        }
        else /* Security level change dependant (Auto: no for User and BDC) */ if(strstr(line,
            "preferred master"))
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[8]));
            if( sec_changed && (sec_index == 2 || sec_index == 3) )
                sprintf(conf_line, "preferred master = yes\n");
            else if( sec_changed && sec_index == 0 )
                sprintf(conf_line, "preferred master = no\n");
            else if( active_index == 0 )
                sprintf(conf_line, "preferred master = yes\n");
            else
                sprintf(conf_line, "preferred master = no\n");

            strcat(new_conf, conf_line);
        }
        else /* Security level change dependant */ if(strstr(line, "time server"))
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[9]));
            if( sec_changed && (sec_index == 1 || sec_index == 2 || sec_index == 3) )
                sprintf(conf_line, "time server = yes\n");
            else if( sec_changed && sec_index == 0 )
                sprintf(conf_line, "time server = no\n");
            else if( active_index == 0 )
                sprintf(conf_line, "time server = yes\n");
            else
                sprintf(conf_line, "time server = no\n");

            strcat(new_conf, conf_line);
        }
        else /* Security level change dependant */ if(strstr(line, "domain logons"))
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[10]));
            if( sec_changed && (sec_index == 1 || sec_index == 2 || sec_index == 3) )
                sprintf(conf_line, "domain logons = yes\n");
            else if( sec_changed && sec_index == 0 )
                sprintf(conf_line, "domain logons = no\n");
            else if( active_index == 0 )
                sprintf(conf_line, "domain logons = yes\n");
            else if( active_index == 1 )
                sprintf(conf_line, "domain logons = no\n");

            strcat(new_conf, conf_line);
        }
        else /* Security level change dependant */ if(strstr(line, "wins support"))
        {
            active_index =
                gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[11]));
            if( sec_changed && (sec_index == 1 || sec_index == 2 || sec_index == 3) )
                sprintf(conf_line, "wins support = yes\n");
            else if( sec_changed && sec_index == 0 )
                sprintf(conf_line, "wins support = no\n");
            else if( active_index == 0 )
                sprintf(conf_line, "wins support = yes\n");
            else if( active_index == 1 )
                sprintf(conf_line, "wins support = no\n");

            strcat(new_conf, conf_line);
        }
        else /* Security level change dependant */ if(strstr(line, "wins proxy"))
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[12]));
            if( sec_changed && (sec_index == 1 || sec_index == 2 || sec_index == 3) )
                sprintf(conf_line, "wins proxy = yes\n");
            else if( sec_changed && sec_index == 0 )
                sprintf(conf_line, "wins proxy = no\n");
            else if( active_index == 0 )
                sprintf(conf_line, "wins proxy = yes\n");
            else if( active_index == 1 )
                sprintf(conf_line, "wins proxy = no\n");

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "dns proxy") )
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[13]));
            if( active_index == 0 )
                sprintf(conf_line, "dns proxy = yes\n");
            else
                sprintf(conf_line, "dns proxy = no\n");

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "short preserve case") )
        {
            /* Just remove the line */
        }
        else if( strstr(line, "preserve case") )
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[14]));
            if( active_index == 0 )
                sprintf(conf_line, "preserve case = yes\nshort preserve case = yes\n");
            else
                sprintf(conf_line, "preserve case = no\n");

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "client use spnego") )
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[15]));
            if( active_index == 0 )
                sprintf(conf_line, "client use spnego = yes\n");
            else
                sprintf(conf_line, "client use spnego = no\n");

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "client signing") )
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[16]));
            if( active_index == 0 )
                sprintf(conf_line, "client signing = yes\n");
            else
                sprintf(conf_line, "client signing = no\n");

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "client schannel") )
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[17]));
            if( active_index == 0 )
                sprintf(conf_line, "client schannel = yes\n");
            else
                sprintf(conf_line, "client schannel = no\n");

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "server signing") )
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[18]));
            if( active_index == 0 )
                sprintf(conf_line, "server signing = yes\n");
            else
                sprintf(conf_line, "server signing = no\n");

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "server schannel") )
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[19]));
            if( active_index == 0 )
                sprintf(conf_line, "server schannel = yes\n");
            else
                sprintf(conf_line, "server schannel = no\n");

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "nt pipe support") )
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[20]));
            if( active_index == 0 )
                sprintf(conf_line, "nt pipe support = yes\n");
            else
                sprintf(conf_line, "nt pipe support = no\n");

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "nt status support") )
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[21]));
            if( active_index == 0 )
                sprintf(conf_line, "nt status support = yes\n");
            else
                sprintf(conf_line, "nt status support = no\n");

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "allow trusted domains") )
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[22]));
            if( active_index == 0 )
                sprintf(conf_line, "allow trusted domains = yes\n");
            else
                sprintf(conf_line, "allow trusted domains = no\n");

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "obey pam restrictions") )
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[23]));
            if( active_index == 0 )
                sprintf(conf_line, "obey pam restrictions = yes\n");
            else
                sprintf(conf_line, "obey pam restrictions = no\n");

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "enable spoolss") )
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[24]));
            if( active_index == 0 )
                sprintf(conf_line, "enable spoolss = yes\n");
            else
                sprintf(conf_line, "enable spoolss = no\n");

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "client plaintext auth") )
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[25]));
            if( active_index == 0 )
                sprintf(conf_line, "client plaintext auth = yes\n");
            else
                sprintf(conf_line, "client plaintext auth = no\n");

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "disable netbios") )
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[26]));
            if( active_index == 0 )
                sprintf(conf_line, "disable netbios = yes\n");
            else
                sprintf(conf_line, "disable netbios = no\n");

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "follow symlinks") )
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[27]));
            if( active_index == 0 )
                sprintf(conf_line, "follow symlinks = yes\n");
            else
                sprintf(conf_line, "follow symlinks = no\n");

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "update encrypted") )
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[28]));
            if( active_index == 0 )
                sprintf(conf_line, "update encrypted = yes\n");
            else
                sprintf(conf_line, "update encrypted = no\n");

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "pam password change") )
        {
            active_index =
                gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[29]));
            if( active_index == 0 )
                sprintf(conf_line, "pam password change = yes\n");
            else
                sprintf(conf_line, "pam password change = no\n");

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "hostname lookups") )
        {
            active_index =
                gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[30]));
            if( active_index == 0 )
                sprintf(conf_line, "hostname lookups = yes\n");
            else
                sprintf(conf_line, "hostname lookups = no\n");

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "hostname lookups") )
        {
            active_index =
                gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[30]));
            if( active_index == 0 )
                sprintf(conf_line, "hostname lookups = yes\n");
            else
                sprintf(conf_line, "hostname lookups = no\n");

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "winbind use default domain") )
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[31]));
            if( active_index == 0 || (sec_changed && (sec_index == 1 || sec_index == 2 || sec_index == 3)) )
                sprintf(conf_line, "winbind use default domain = yes\n");
            else
                sprintf(conf_line, "winbind use default domain = no\n");

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "winbind trusted domains only") )
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[32]));
            if(active_index == 0)
                sprintf(conf_line, "winbind trusted domains only = yes\n");
            else
                sprintf(conf_line, "winbind trusted domains only = no\n");

            strcat(new_conf, conf_line);
        }
        else if(strstr(line, "winbind nested groups"))
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[33]));
            if(active_index == 0)
                sprintf(conf_line, "winbind nested groups = yes\n");
            else
                sprintf(conf_line, "winbind nested groups = no\n");

            strcat(new_conf, conf_line);
        }
        else if(strstr(line, "winbind nss info"))
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[34]));
            if(active_index == 0)
                sprintf(conf_line, "winbind nss info = yes\n");
            else
                sprintf(conf_line, "winbind nss info = no\n");

            strcat(new_conf, conf_line);
        }
        else if(strstr(line, "winbind refresh tickets"))
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[35]));
            if(active_index == 0)
                sprintf(conf_line, "winbind refresh tickets = yes\n");
            else
                sprintf(conf_line, "winbind refresh tickets = no\n");

            strcat(new_conf, conf_line);
        }
        else if(strstr(line, "winbind offline logon"))
        {
            active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->server_set_combo[36]));
            if(active_index == 0)
                sprintf(conf_line, "winbind offline logon = yes\n");
            else
                sprintf(conf_line, "winbind offline logon = no\n");

            strcat(new_conf, conf_line);
        }
        else


            /* Spinbuttons */

        if( strstr(line, "max log size") )
        {
            sprintf(conf_line, "max log size = %s\n", max_log_file_size);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "username level") )
        {
            sprintf(conf_line, "username level = %s\n", username_match_level);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "password level") )
        {
            sprintf(conf_line, "password level = %s\n", password_match_level);
            strcat(new_conf, conf_line);
        }
        else /* Security level change dependant */
        if( strstr(line, "os level") )
        {
            /* Set it to something high for Primary domains and active directory */
            if( sec_changed && (sec_index == 2 || sec_index == 3) )
                sprintf(conf_line, "os level = 80\n");  /* 35 is max but 80 doesnt hurt */
            else if( sec_changed && (sec_index == 0 || sec_index == 1) )
                sprintf(conf_line, "os level = 33\n");  /* 33 is default for user level security */
            else
                sprintf(conf_line, "os level = %s\n", election_level);

            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "passwd chat timeout") )
        {
            sprintf(conf_line, "passwd chat timeout = %s\n", password_chat_timeout);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "machine password timeout") )
        {
            sprintf(conf_line, "machine password timeout = %s\n", machine_password_timeout);
            strcat(new_conf, conf_line);
        }
        else if( strstr(line, "winbind cache time") )
        {
            sprintf(conf_line, "winbind cache time = %s\n", winbind_cache_timeout);
            strcat(new_conf, conf_line);
        }
        else
            strcat(new_conf, line); /* Gather all other values */
    }

    /* Collect the rest of the file, share definitions etc */
    if( conf_size > 1 )
        while(fgets(line, conf_size, fp) != NULL)
            strcat(new_conf, line);

    free(line);
    free(conf_line);
    fclose(fp);


    /* Write the new file if defined */
    if( WRITE_CHANGES )
    {
        if((fp = fopen(SAMBA_CONF, "w+")) == NULL)
        {
            info = g_strdup_printf(_("Cant write smb.conf here:\n%s\n"), SAMBA_CONF);
            show_info(info);
            g_free(info);
            return;
        }
        fputs(new_conf, fp);
        fclose(fp);
    }
    else
        printf("New configuration:\n%s\nThe configuration was not changed, only printed\n",
                new_conf);

    free(new_conf);

    if( sec_changed )
    {
        if( sec_index == 0 )
        {
            remove_global_option("realm");  /* Can realm be used here ? */

            info = g_strdup_printf(_
                     ("Auto configuration for \"user\" security level done.\nReview any changes.\n"));
        }
        else if( sec_index == 1 )
        {
            remove_global_option("realm");  /* Can realm be used here ? */

            info = g_strdup_printf(_
                     ("Auto configuration for (BDC) \"backup domain controller\" done.\nUnlock and supply a samba password for the samba root account if needed.\n"));
        }
        else if( sec_index == 2 )
        {
            remove_global_option("realm");  /* Can realm be used here ? */

            info = g_strdup_printf(_
                     ("Auto configuration for (PDC) \"primary domain controller\" done.\nConnect to this domain as root after giving it a samba password and unlocking it.\n"));
        }
        else if( sec_index == 3 )
        {
            /* The kerberos realm entry is enabled or disabled in populate_server_settings() depending on whats selected */
            add_global_option("realm");

            info = g_strdup_printf(_
                     ("Auto configuration for (ADS) \"Active directory\" security level done.\nSetup the kerberos realm and connect to this domain as root after giving it a samba password and unlocking it.\n"));
        }
        else
            info = g_strdup_printf(_
                     ("Error!!! Auto configuration for \"No\" security level done.\nPlease review any changes.\n"));

        show_info(info);
        g_free(info);
    }


    fix_newlines_in_conf();

    /* Populate the server settings */
    populate_server_settings(widgets);

    populate_conf_tab(widgets);

    reread_conf(widgets);
}
