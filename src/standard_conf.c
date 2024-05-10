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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "allocate.h"
#include "widgets.h"
#include "functions.h"
#include "commands.h"
#include "commented.h"
#include "standard_conf.h"
#include "system_defines.h"
#include "populate_gadmin_samba.h"
#include "show_info.h"


int conf_ok(gchar * file_path)
{
    FILE *fp;
    long conf_size;
    char *line;
    int retval = 0;
    int have_security = 0;

    /* The configuration is ok if gadmin-sambas settings
       contains "show_conf_question: (no OR false)" */
    if((fp = fopen(file_path, "r")) == NULL)
    {
        /* Return false */
        return retval;
    }
    fseek(fp, 0, SEEK_END);
    conf_size = ftell(fp);
    rewind(fp);
    line = allocate(conf_size + 1);

    if( conf_size > 1 )
        while(fgets(line, conf_size, fp) != NULL)
        {
            /* Skip commented options */
            if( commented(line) )
                continue;

            if( strlen(line) > 5 )
            {
                if( strstr(line, "show_conf_question:" )
                    && (strstr(line, "false") || strstr(line, "no")) )
                {
                    retval = 1;
                    break;
                }
            }
        }
    fclose(fp);
    free(line);


    /* If the security option is missing the conf is still bad */
    if((fp = fopen(SAMBA_CONF, "r")) == NULL)
    {
        return retval;
    }
    fseek(fp, 0, SEEK_END);
    conf_size = ftell(fp);
    rewind(fp);
    line = allocate(conf_size + 1);

    if( conf_size > 1 )
        while(fgets(line, conf_size, fp) != NULL)
        {
            /* Skip commented options */
            if( commented(line) )
                continue;

            if( strlen(line) > 5 )
            {
                if( strstr(line, "security=") || strstr(line, "security =") )
                {
                    have_security = 1;
                    break;
                }
            }
        }

    if( ! have_security )
        retval = 0;

    fclose(fp);
    free(line);

    return retval;
}


void backup_samba_conf(struct w *widgets)
{
    gchar *cmd, *info;

    if( ! file_exists(SAMBA_CONF_BACKUP) && file_exists(SAMBA_CONF) )
    {
        cmd = g_strdup_printf("cp %s %s", SAMBA_CONF, SAMBA_CONF_BACKUP);
        if( ! run_command(cmd) )
        {
            info = g_strdup_printf(_("Error: Could not backup the current configuration:\n%s\n"),
                SAMBA_CONF);
            show_info(info);
        }
        else
        {
            info =  g_strdup_printf(_("The samba configuration was backed up here for safety:\n%s\n"),
                SAMBA_CONF_BACKUP);
            show_info(info);
        }

        g_free(cmd);
        g_free(info);
    }
}


/* This backs up the old configuration and adds the default configuration */
void add_standard_conf(struct w *widgets)
{
    FILE *fp;
    char *buffer;
    gchar *cmd, *info;

    /* First take a backup of the old configuration
       as SAMBA_CONF_BACKUP if it doesnt exist */
    backup_samba_conf(widgets);

    /* Write the default configuration */
    if((fp = fopen(SAMBA_CONF, "w+")) == NULL)
    {
        info = g_strdup_printf(_("Cant write a new smb.conf here:\n%s\n"), SAMBA_CONF);
        show_info(info);
        return;
    }

    buffer = allocate(100000);

    strcpy(buffer, "[global]\n");
    strcat(buffer, "netbios name = Samba24\n");
    strcat(buffer, "server string = Samba file and print server\n");
    strcat(buffer, "workgroup = Workgroup\n");
    strcat(buffer, "security = user\n");
    /* Automatically adds ADS security level option: realm = domain.example.org */
    strcat(buffer, "hosts allow = 127. 192.168.0.\n");
    strcat(buffer, "interfaces = 127.0.0.1/8 192.168.0.0/24\n");
    strcat(buffer, "bind interfaces only = yes\n");
    strcat(buffer, "remote announce = 192.168.0.255\n");
    strcat(buffer, "remote browse sync = 192.168.0.255\n");

    strcat(buffer, "printcap name = cups\n");
    strcat(buffer, "load printers = yes\n");
    strcat(buffer, "cups options = raw\n");
    strcat(buffer, "printing = cups\n");    /* Fixme: Combo: cups, bsd etc */

    strcat(buffer, "guest account = smbguest\n");

    info = g_strdup_printf("log file = %s/log/samba/samba.log\n", LOCALSTATEDIR);
    strcat(buffer, info);
    g_free(info);

    strcat(buffer, "max log size = 1000\n");
    strcat(buffer, "null passwords = no\n");
    strcat(buffer, "username level = 6\n");
    strcat(buffer, "password level = 6\n");
    strcat(buffer, "encrypt passwords = yes\n");
    strcat(buffer, "unix password sync = yes\n");
    strcat(buffer, "socket options = TCP_NODELAY SO_RCVBUF=8192 SO_SNDBUF=8192\n");
    strcat(buffer, "local master = no\n");  /* Only one per subnet. */
    strcat(buffer, "domain master = no\n");
    strcat(buffer, "preferred master = no\n");
    strcat(buffer, "domain logons = no\n");
    strcat(buffer, "os level = 33\n");
    strcat(buffer, "logon drive = m:\n");
    strcat(buffer, "logon home = \\\\%L\\homes\\%u\n");
    strcat(buffer, "logon path = \\\\%L\\profiles\\%u\n");
    strcat(buffer, "logon script = %G.bat\n");
    strcat(buffer, "time server = no\n");
    strcat(buffer, "name resolve order = wins lmhosts bcast\n");
    strcat(buffer, "wins support = no\n");
/*    strcat(buffer, "wins server =\n"); This option is added if IP or DNS name is given. */
    strcat(buffer, "wins proxy = no\n");
    strcat(buffer, "dns proxy = no\n");
    strcat(buffer, "preserve case = yes\n");
    strcat(buffer, "short preserve case = yes\n");
    strcat(buffer, "client use spnego = yes\n");
    strcat(buffer, "client signing = yes\n");
    strcat(buffer, "client schannel = yes\n");
    strcat(buffer, "server signing = yes\n");
    strcat(buffer, "server schannel = yes\n");
    strcat(buffer, "nt pipe support = yes\n");
    strcat(buffer, "nt status support = yes\n");
    strcat(buffer, "allow trusted domains = no\n");
    strcat(buffer, "obey pam restrictions = yes\n");
    strcat(buffer, "enable spoolss = yes\n");
    strcat(buffer, "client plaintext auth = no\n");
    strcat(buffer, "disable netbios = no\n");
    strcat(buffer, "follow symlinks = no\n");
    strcat(buffer, "update encrypted = yes\n");
    strcat(buffer, "pam password change = no\n");
    strcat(buffer, "passwd chat timeout = 120\n");
    strcat(buffer, "hostname lookups = no\n");

    info = g_strdup_printf("username map = %s/samba/smbusers\n", SYSCONFDIR);
    strcat(buffer, info);
    g_free(info);

    /* Specify that tdbsam is used. */
    strcat(buffer, "passdb backend = tdbsam\n");


    strcat(buffer, "passwd program = /usr/bin/passwd '%u'\n");

    strcat(buffer,
        "passwd chat = *New*password* %n\\n *ReType*new*password* %n\\n *passwd*changed*\\n\n");
    strcat(buffer,
        "add user script = /usr/sbin/useradd -d /dev/null -c 'Samba User Account' -s /dev/null '%u'\n");
    strcat(buffer,
        "add user to group script=/usr/sbin/useradd -d /dev/null -c 'Samba User Account' -s /dev/null -g '%g' '%u'\n");
    strcat(buffer, "add group script = /usr/sbin/groupadd '%g'\n");
    strcat(buffer, "delete user script = /usr/sbin/userdel '%u'\n");
    strcat(buffer, "delete user from group script = /usr/sbin/userdel '%u' '%g'\n");
    strcat(buffer, "delete group script = /usr/sbin/groupdel '%g'\n");
    strcat(buffer,
        "add machine script = /usr/sbin/useradd -d /dev/null -g sambamachines -c 'Samba Machine Account' -s /dev/null -M '%u'\n");
    strcat(buffer, "machine password timeout = 120\n");
    strcat(buffer, "idmap uid = 16777216-33554431\n");
    strcat(buffer, "idmap gid = 16777216-33554431\n");
    strcat(buffer, "template shell = /dev/null\n");

    /* Winbindd stuff */
    strcat(buffer, "winbind use default domain = yes\n");
    strcat(buffer, "winbind separator = @\n");
    strcat(buffer, "winbind cache time = 360\n");
    strcat(buffer, "winbind trusted domains only = yes\n");
    strcat(buffer, "winbind nested groups = no\n");
    strcat(buffer, "winbind nss info = no\n");
    strcat(buffer, "winbind refresh tickets = no\n");
    strcat(buffer, "winbind offline logon = no\n");

    /* Shared home directories */
    strcat(buffer, "\n");
    strcat(buffer, "[homes]\n");
    strcat(buffer, "comment = Home Directories\n");
    strcat(buffer, "path = /home\n");
    strcat(buffer, "valid users = %U\n");
    strcat(buffer, "read only = no\n");
    strcat(buffer, "available = yes\n");
    strcat(buffer, "browseable = yes\n");
    strcat(buffer, "writable = yes\n");
    strcat(buffer, "guest ok = no\n");
    strcat(buffer, "printable = no\n");
    strcat(buffer, "locking = no\n");
    strcat(buffer, "strict locking = no\n");

    strcat(buffer, "\n");
    strcat(buffer, "[netlogon]\n");
    strcat(buffer, "comment = Network Logon Service\n");
    strcat(buffer, "path = ");
    strcat(buffer, LOCALSTATEDIR);
    strcat(buffer, "/lib/samba/netlogon\n");
    strcat(buffer, "read only = no\n");
    strcat(buffer, "available = yes\n");
    strcat(buffer, "browseable = yes\n");
    strcat(buffer, "writable = no\n");
    strcat(buffer, "guest ok = no\n");
    strcat(buffer, "printable = no\n");
    strcat(buffer, "locking = no\n");
    strcat(buffer, "strict locking = no\n");


    /* Profiles are created automatically for each user, clients adds the files */
    strcat(buffer, "\n");
    strcat(buffer, "[profiles]\n"); /* This must be writable */
    strcat(buffer, "comment = User Profiles\n");
    strcat(buffer, "path = ");
    strcat(buffer, LOCALSTATEDIR);
    strcat(buffer, "/lib/samba/profiles\n");
    strcat(buffer, "read only = no\n");
    strcat(buffer, "available = yes\n");
    strcat(buffer, "browseable = yes\n");
    strcat(buffer, "writable = yes\n");
    strcat(buffer, "guest ok = no\n");
    strcat(buffer, "printable = no\n");
    strcat(buffer, "create mode = 0600\n");
    strcat(buffer, "directory mask = 0700\n");
    strcat(buffer, "locking = no\n");
    strcat(buffer, "strict locking = no\n");

    strcat(buffer, "\n");
    strcat(buffer, "[printers]\n");
    strcat(buffer, "comment = All Printers\n");
    info = g_strdup_printf("path = %s/spool/samba\n", LOCALSTATEDIR);
    strcat(buffer, info);
    g_free(info);
    strcat(buffer, "browseable = yes\n");
    strcat(buffer, "writable = no\n");
    strcat(buffer, "guest ok = no\n");
    strcat(buffer, "printable = yes\n");
    strcat(buffer, "locking = no\n");
    strcat(buffer, "strict locking = no\n");


    /* We want to be able to print anything as pdf documents to a directory */
    strcat(buffer, "\n");
    strcat(buffer, "[pdf-documents]\n");
    strcat(buffer, "path = ");
    strcat(buffer, LOCALSTATEDIR);
    strcat(buffer, "/lib/samba/pdf-documents\n");
    strcat(buffer, "comment = Converted PDF Documents\n");
    strcat(buffer, "admin users = %U\n");
    strcat(buffer, "available = yes\n");
    strcat(buffer, "browseable = yes\n");
    strcat(buffer, "writeable = yes\n");
    strcat(buffer, "guest ok = yes\n");
    strcat(buffer, "locking = no\n");
    strcat(buffer, "strict locking = no\n");

    /* The pdf printer service (converter) */
    strcat(buffer, "\n");
    strcat(buffer, "[pdf-printer]\n");
    strcat(buffer, "path = /tmp\n");
    strcat(buffer, "comment = PDF Printer Service\n");
    strcat(buffer, "printable = yes\n");
    strcat(buffer, "guest ok = yes\n");
    strcat(buffer, "use client driver = yes\n");
    strcat(buffer, "printing = bsd\n");
    strcat(buffer, "print command = /usr/bin/gadmin-samba-pdf %s %u\n");
    strcat(buffer, "lpq command =\n");
    strcat(buffer, "lprm command =\n");

    fputs(buffer, fp);
    fclose(fp);
    free(buffer);


    if( widgets->default_conf_question_window != NULL )
    {
        gtk_widget_destroy(widgets->default_conf_question_window);
        widgets->default_conf_question_window = NULL;
    }
    populate_gadmin_samba(widgets);

    /* Create default directories */
    cmd = g_strdup_printf
        ("mkdir -p '%s/lib/samba/netlogon' '%s/lib/samba/pdf-documents' '%s/lib/samba/profiles/root' '%s/log/samba'\n",
        LOCALSTATEDIR, LOCALSTATEDIR, LOCALSTATEDIR, LOCALSTATEDIR);
    if((fp = popen(cmd, "w")) == NULL)
    {
        perror("popen");
        info = g_strconcat(_("Failed to create directories:\n%s\n"),
            LOCALSTATEDIR, "/lib/samba/netlogon\n",
            LOCALSTATEDIR, "/lib/samba/pdf-documents\n",
            LOCALSTATEDIR, "/lib/samba/profiles/root\n", LOCALSTATEDIR, "/log/samba\n", NULL);
        show_info(info);
        g_free(info);
        g_free(cmd);
        return;
    }
    pclose(fp);
    g_free(cmd);

    /* Chmod the profiles directory */
    cmd = g_strdup_printf("chmod 1755 %s/lib/samba/profiles\n", LOCALSTATEDIR);
    if((fp = popen(cmd, "w")) == NULL)
    {
        perror("popen");
        info = g_strdup_printf(_("Failed chmodding directory: %s/lib/samba/profiles\n"),
            LOCALSTATEDIR);
        show_info(info);
        g_free(info);
        g_free(cmd);
        return;
    }
    pclose(fp);
    g_free(cmd);

    /* Create the samba root users home directory */
    cmd = g_strdup_printf("mkdir -p '%s/lib/samba/root'\n", LOCALSTATEDIR);
    if((fp = popen(cmd, "w")) == NULL)
    {
        perror("popen");
        info = g_strdup_printf(_("Failed to create directory: %s/%s\n"), LOCALSTATEDIR,
            "/lib/samba/root");
        show_info(info);
        g_free(info);
        g_free(cmd);
        return;
    }
    pclose(fp);
    g_free(cmd);

    /* Fixme: add username mappings to /etc/samba/smbusers */
    // root = administrator
    // nobody = guest

    // net groupmap modify ntgroup="Domain Admins" unixgroup=root
    // net groupmap modify ntgroup="Administrators" unixgroup=root
    // net groupmap modify ntgroup="Power Users" unixgroup=sys
    // net groupmap modify ntgroup="Domain Users" unixgroup=users
    // net groupmap modify ntgroup="Domain Guests" unixgroup=nobody

    /* net groupmap list (lists the group mappings) */
}


void dont_add_standard_conf(struct w *widgets)
{
    gtk_widget_destroy(widgets->default_conf_question_window);
    widgets->default_conf_question_window = NULL;
    populate_gadmin_samba(widgets);
}


void create_standard_conf_question(struct w *widgets)
{
    FILE *fp;
    char *line, *conf;
    long conf_size;
    int found_conf_question = 0;
    GtkWidget *vbox18, *label182;
    GtkWidget *scrolledwindow18;
    GtkWidget *default_question_textview;
    GtkWidget *hbuttonbox11;
    GtkWidget *yes_default_question_button;
    GtkWidget *alignment44, *hbox98;
    GtkWidget *image44, *label184;
    GtkWidget *no_default_question_button;
    GtkWidget *alignment45, *hbox99;
    GtkWidget *image45, *label185;
    gchar *cmd, *info, *text, *utf8;

    widgets->default_conf_question_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(widgets->default_conf_question_window, 550, 340);
    gtk_window_set_title(GTK_WINDOW(widgets->default_conf_question_window),
        _("GADMIN-SAMBA Question"));
    gtk_window_set_position(GTK_WINDOW(widgets->default_conf_question_window), GTK_WIN_POS_CENTER);

    vbox18 = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(widgets->default_conf_question_window), vbox18);

    label182 = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(vbox18), label182, FALSE, FALSE, 0);
    gtk_widget_set_size_request(label182, -1, 20);
    gtk_label_set_justify(GTK_LABEL(label182), GTK_JUSTIFY_LEFT);

    scrolledwindow18 = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(vbox18), scrolledwindow18, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow18), GTK_POLICY_AUTOMATIC,
        GTK_POLICY_AUTOMATIC);

    default_question_textview = gtk_text_view_new();
    gtk_container_add(GTK_CONTAINER(scrolledwindow18), default_question_textview);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(default_question_textview), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(default_question_textview), FALSE);
    gtk_text_view_set_pixels_above_lines(GTK_TEXT_VIEW(default_question_textview), 3);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(default_question_textview), 30);


    text = g_strconcat(_
        ("GADMIN-SAMBA could not find SAMBAs configuration (smb.conf) or the current\n"),
        _("configuration doesnt have all the features that GADMIN-SAMBA requires.\n\n"),
        _("Do you want to overwrite the current samba configuration ?\n"),
        _("\t\t\t(If you dont know then press yes)\n"), NULL);

    utf8 = g_locale_to_utf8(text, strlen(text), NULL, NULL, NULL);
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(default_question_textview)),
        utf8, -1);
    g_free(text);


    hbuttonbox11 = gtk_hbutton_box_new();
    gtk_box_pack_start(GTK_BOX(vbox18), hbuttonbox11, FALSE, FALSE, 0);
    gtk_widget_set_size_request(hbuttonbox11, -1, 40);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(hbuttonbox11), GTK_BUTTONBOX_SPREAD);

    yes_default_question_button = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(hbuttonbox11), yes_default_question_button);
    //GTK_WIDGET_SET_FLAGS(yes_default_question_button, GTK_CAN_DEFAULT);
    gtk_widget_set_can_default(yes_default_question_button, TRUE);

    alignment44 = gtk_alignment_new(0.5, 0.5, 0, 0);
    gtk_container_add(GTK_CONTAINER(yes_default_question_button), alignment44);

    hbox98 = gtk_hbox_new(FALSE, 2);
    gtk_container_add(GTK_CONTAINER(alignment44), hbox98);

    image44 = gtk_image_new_from_stock("gtk-yes", GTK_ICON_SIZE_BUTTON);
    gtk_box_pack_start(GTK_BOX(hbox98), image44, FALSE, FALSE, 0);

    label184 = gtk_label_new_with_mnemonic(_("Yes"));
    gtk_box_pack_start(GTK_BOX(hbox98), label184, FALSE, FALSE, 0);
    gtk_label_set_justify(GTK_LABEL(label184), GTK_JUSTIFY_LEFT);

    no_default_question_button = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(hbuttonbox11), no_default_question_button);
    //GTK_WIDGET_SET_FLAGS(no_default_question_button, GTK_CAN_DEFAULT);
    gtk_widget_set_can_default(no_default_question_button, TRUE);

    alignment45 = gtk_alignment_new(0.5, 0.5, 0, 0);
    gtk_container_add(GTK_CONTAINER(no_default_question_button), alignment45);

    hbox99 = gtk_hbox_new(FALSE, 2);
    gtk_container_add(GTK_CONTAINER(alignment45), hbox99);

    image45 = gtk_image_new_from_stock("gtk-no", GTK_ICON_SIZE_BUTTON);
    gtk_box_pack_start(GTK_BOX(hbox99), image45, FALSE, FALSE, 0);

    label185 = gtk_label_new_with_mnemonic(_("No"));
    gtk_box_pack_start(GTK_BOX(hbox99), label185, FALSE, FALSE, 0);
    gtk_label_set_justify(GTK_LABEL(label185), GTK_JUSTIFY_LEFT);

    if(utf8 != NULL)
        g_free(utf8);


    g_signal_connect_swapped(G_OBJECT(yes_default_question_button), "clicked",
        G_CALLBACK(add_standard_conf), widgets);

    g_signal_connect_swapped(G_OBJECT(no_default_question_button), "clicked",
        G_CALLBACK(dont_add_standard_conf), widgets);

    gtk_widget_show_all(widgets->default_conf_question_window);


    /* Create the gadmin-samba settings directory if it doesnt exist. */
    if( ! file_exists(APPCONFDIR) )
    {
        cmd = g_strdup_printf("mkdir -p '%s'", APPCONFDIR);
        if( ! run_command(cmd) )
        {
            info = g_strdup_printf(_("Error: cant create the settings directory here:\n%s\n"),
                APPCONFDIR);
            show_info(info);
            g_free(info);
            g_free(cmd);
            return;
        }
        g_free(cmd);
    }

    /* The settings file doesnt exist, create and write the setting to it */
    if( ! file_exists(SETTINGS_FILE) )
    {
        if((fp = fopen(SETTINGS_FILE, "w+")) == NULL)
        {
            info =  g_strdup_printf(_("Error: cant write the settings file here:\n%s\n"),
                SETTINGS_FILE);
            show_info(info);
            g_free(info);
            return;
        }
        fputs("show_conf_question: false\n", fp);
        fclose(fp);
        return;
    }


    /* Gather old values and re/write 'show_conf_question: false' to gadmin-sambas settings */
    if((fp = fopen(SETTINGS_FILE, "r")) == NULL)
    {
        info = g_strdup_printf(_("Error: cant write the settings file here:\n%s\n"), SETTINGS_FILE);
        show_info(info);
        g_free(info);
        return;
    }

    fseek(fp, 0, SEEK_END);
    conf_size = ftell(fp);
    rewind(fp);

    line = allocate(conf_size + 1);
    conf = allocate(conf_size + 1024);

    if( conf_size > 1 )
        while(fgets(line, conf_size, fp) != NULL)
        {
            if( strlen(line) > 5 )
            {
                if( strstr(line, "show_conf_question:") )
                {
                    found_conf_question = 1;
                    strcat(conf, "show_conf_question: false\n");
                    break;
                }
                else
                    strcat(conf, line);
            }
        }
    fclose(fp);
    free(line);

    /* If theres no 'show_conf_question: false' line, add one */
    if( ! found_conf_question )
        strcat(conf, "show_conf_question: false\n");

    /* Write the altered conf */
    if((fp = fopen(SETTINGS_FILE, "w+")) == NULL)
    {
        info = g_strdup_printf(_("Error: cant write the settings file here:\n%s\n"), SETTINGS_FILE);
        show_info(info);
        g_free(info);
        return;
    }
    fputs(conf, fp);
    fclose(fp);
    free(conf);
}
