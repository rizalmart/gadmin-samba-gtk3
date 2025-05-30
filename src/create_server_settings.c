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
#include "make_settings_entries.h"
#include "make_settings_combos.h"
#include "make_settings_spinbuttons.h"
#include "make_settings_progressbars.h"
#include "make_settings_labels.h"
#include "make_settings_hseparators.h"
#include "create_server_settings.h"
#include "apply_server_settings.h"



void create_server_settings(struct w *widgets)
{
    GtkTooltip *tooltips;
    gchar *utf8 = NULL;
    gchar *combo_text;

    /* Counters for the widgets and positions in the table */
    int a = 0;
    int b = 1;
    int ent = 0;
    int spin = 0;
    int comb = 0;

    /* Max lengths and input */
    int entry_size = 250;

    //tooltips = gtk_tooltip_new();

    /* The servers hostname */
    widgets->server_set_entry[ent] =
        make_long_entry_with_label(GTK_TABLE(widgets->srv_set_table), _(" The servers host name: "), 0, 1, a, b, entry_size);
    gtk_widget_set_tooltip_text(widgets->server_set_entry[ent],
        _("IE: Samba24. Use this name to connect to this server from the clients"));
    a++;
    b++;
    ent++;

    /* The server comment */
    widgets->server_set_entry[ent] =
        make_long_entry_with_label(GTK_TABLE(widgets->srv_set_table), _(" Comment: "), 0, 1, a, b, entry_size);
    gtk_widget_set_tooltip_text(widgets->server_set_entry[ent],
        _("A comment for this server, IE: SAMBA Server"));
    a++;
    b++;
    ent++;

    /* Workgroup or domain name */
    widgets->server_set_entry[ent] =
        make_entry_with_label(GTK_TABLE(widgets->srv_set_table), _(" Workgroup or domain name: "), 0, 1, a, b, entry_size);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _
        ("IE: work-group-name or sambaserver2.example.org (spaces arent allowed). If SAMBA is set up as a domain then use this domain name to connect to from the clients"));
    a++;
    b++;
    ent++;;



    /* Security level combo */
    widgets->server_set_combo[comb] =
        make_combo_with_label(GTK_TABLE(widgets->srv_set_table), _(" Security level: "), 0, 1, a, b, 50);

    /* Default SAMBA security level */
    combo_text = g_strdup_printf(_("User"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);

    /* Backup Domain security level = Domain */
    combo_text = g_strdup_printf(_("Backup Domain"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);

    /* Primary Domain security level = Domain */
    combo_text = g_strdup_printf(_("Primary Domain"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);

    /* Active directory security level = ADS */
    combo_text = g_strdup_printf(_("Active directory"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;;



    /* Kerberos Realm for ADS */
    widgets->server_set_entry[ent] =
        make_long_entry_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Active directory kerberos realm: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("ADS Kerberos realm, IE: domain.example.org"));
    a++;
    b++;
    ent++;

    /* Allowed hosts and networks */
    widgets->server_set_entry[ent] =
        make_long_entry_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Allowed hosts and networks: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("Hosts and networks allowed to connect to this server, IE: 127. 192.168.0."));
    a++;
    b++;
    ent++;

    /* Interfaces to listen on */
    widgets->server_set_entry[ent] =
        make_long_entry_with_label(GTK_TABLE(widgets->srv_set_table), _(" Handle connections on: "),
        0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("Network interfaces to listen on, IE: eth0 127.0.0.1/8 192.168.0.120/24"));
    a++;
    b++;
    ent++;

    /* Remote announce */
    widgets->server_set_entry[ent] =
        make_long_entry_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Announce this server to: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("Announce this servers precence to the specified networks, IE: 192.168.0.255 192.168.2.44"));
    a++;
    b++;
    ent++;

    /* Retrieve announcements from (Remote browse sync) */
    widgets->server_set_entry[ent] =
        make_long_entry_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Retrieve announcements from: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("Retrieve announcements from the specified networks, IE: 192.168.0.255 192.168.2.44"));
    a++;
    b++;
    ent++;

    /* Printcap file name */
    widgets->server_set_entry[ent] =
        make_long_entry_with_label(GTK_TABLE(widgets->srv_set_table), _(" Printcap filepath: "), 0,
        1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("Path to the printcap file, IE: /etc/printcap"));
    a++;
    b++;
    ent++;

    /* Automatically load printer list */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Automatically load printer list: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* The options above are the only options simple sharing requires */
    make_3columns_hseparator(GTK_TABLE(widgets->srv_set_table), 0, 1, a, b);
    a++;
    b++;
    make_3columns_label(GTK_TABLE(widgets->srv_set_table),
        _("Server settings for advanced administrators:"), 0, 1, a, b);
    a++;
    b++;

    /* CUPS Options */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table), _(" CUPS Options: "), 0, 1,
        a, b, 50);
    combo_text = g_strdup_printf(_("Raw"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Guest user */
    widgets->server_set_entry[ent] =
        make_entry_with_label(GTK_TABLE(widgets->srv_set_table), _(" Guest user: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("The name of the guest user, IE: guest"));
    a++;
    b++;
    ent++;

    /* Logfile */
    widgets->server_set_entry[ent] =
        make_long_entry_with_label(GTK_TABLE(widgets->srv_set_table), _(" SAMBA log file: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("Path to the log file, IE: /var/log/samba/samba.log or /var/log/samba/%m.log for individual computer logs"));
    a++;
    b++;
    ent++;

    /* Max logfile size */
    widgets->server_set_spinbutton[spin] =
        make_short_spinbutton_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Max logfile size: "), 0, 1, a, b, 50);
    gtk_widget_set_tooltip_text( widgets->server_set_spinbutton[spin],
        _("The maximum logfile size in Kb."));
    a++;
    b++;
    spin++;

    /* Null Passwords */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Allow empty passwords: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Username level */
    widgets->server_set_spinbutton[spin] =
        make_short_spinbutton_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Username match level: "), 0, 1, a, b, 50);
    gtk_widget_set_tooltip_text( widgets->server_set_spinbutton[spin],
        _("Username match level allows matching of _n_ characters of the username for all combinations of upper and lower case."));
    a++;
    b++;
    spin++;

    /* Password level */
    widgets->server_set_spinbutton[spin] =
        make_short_spinbutton_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Password match level: "), 0, 1, a, b, 50);
    gtk_widget_set_tooltip_text( widgets->server_set_spinbutton[spin],
        _("Password match level allows matching of _n_ characters of the password for all combinations of upper and lower case."));
    a++;
    b++;
    spin++;

    /* Encrypt passwords */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table), _(" Encrypt passwords: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Password syncronization */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table), _(" Syncronize passwords: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Socket options */
    widgets->server_set_entry[ent] =
        make_long_entry_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Socket transfer options: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("The Socket transfer options, IE: TCP_NODELAY SO_RCVBUF=8192 SO_SNDBUF=8192"));
    a++;
    b++;
    ent++;

    /* Local Master Browser */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table), _(" Local master browser: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Domain master browser */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Domain master browser: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Preferred Master */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table), _(" Preferred master: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Master browser election level (OS level) */
    widgets->server_set_spinbutton[spin] =
        make_short_spinbutton_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Master browser election level: "), 0, 1, a, b, 50);
    gtk_widget_set_tooltip_text( widgets->server_set_spinbutton[spin],
        _("Determines the precedence of this server in master browser elections."));
    a++;
    b++;
    spin++;

    /* Domain logons */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table), _(" Domain logons: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Logon drive */
    widgets->server_set_entry[ent] =
        make_entry_with_label(GTK_TABLE(widgets->srv_set_table), _(" Logon drive: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("The drive letter of the home directory, IE: h: or m:"));
    a++;
    b++;
    ent++;

    /* Logon home */
    widgets->server_set_entry[ent] =
        make_entry_with_label(GTK_TABLE(widgets->srv_set_table), _(" Logon home: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("The path to the users home, \\\\%L\\homes\\%u"));
    a++;
    b++;
    ent++;

    /* Logon path */
    widgets->server_set_entry[ent] =
        make_entry_with_label(GTK_TABLE(widgets->srv_set_table), _(" Logon profile path: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("The logon profile path, IE: \\\\%L\\profiles\\%u"));
    a++;
    b++;
    ent++;

    /* Logon script */
    widgets->server_set_entry[ent] =
        make_entry_with_label(GTK_TABLE(widgets->srv_set_table), _(" Logon script: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("The logon script. IE: %U.bat (user_name.bat),%G.bat (group_name.bat), %m.bat (machine_name.bat) or just login.bat. Place the scripts in the netlogon directory"));
    a++;
    b++;
    ent++;

    /* Time server */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table), _(" Time server: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* NetBIOS name resolve order */
    widgets->server_set_entry[ent] =
        make_entry_with_label(GTK_TABLE(widgets->srv_set_table), _(" NetBIOS name resolve order: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("The NetBIOS name resolve order, IE: wins lmhosts bcast"));
    a++;
    b++;
    ent++;

    /* WINS support */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table), _(" WINS support: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Wins servers */
    widgets->server_set_entry[ent] =
        make_entry_with_label(GTK_TABLE(widgets->srv_set_table), _(" Wins servers: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("The Wins servers, IE: 192.168.0.130"));
    a++;
    b++;
    ent++;

    /* Act as a WINS proxy */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table), _(" Act as a WINS proxy: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Act as a DNS proxy */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table), _(" Act as a DNS proxy: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Case sensitive filenames */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Case sensitive filenames: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Client use spnego */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table), _(" Client use spnego: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Client signing */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table), _(" Client signing: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Client schannel */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table), _(" Client schannel: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Server signing */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table), _(" Server signing: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Server schannel */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table), _(" Server schannel: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* NT pipe support */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table), _(" Pipe support: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* NT status support */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table), _(" Status support: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Allow trusted domains */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Allow trusted domains: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Obey PAM restrictions */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Obey PAM restrictions: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Enable spoolss */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table), _(" Enable print spooler: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Client plain text auth */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Allow plain text authentication: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Disable netbios */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table), _(" Disable netbios: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Follow symlinks */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table), _(" Follow symlinks: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Update encrypted */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table), _(" Update encrypted: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* PAM password change */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table), _(" PAM password change: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Password chat timeout */
    widgets->server_set_spinbutton[spin] =
        make_short_spinbutton_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Password chat timeout: "), 0, 1, a, b, 50);
    gtk_widget_set_tooltip_text( widgets->server_set_spinbutton[spin],
        _("Time until password chats time out."));
    a++;
    b++;
    spin++;

    /* Hostname lookups */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table), _(" Hostname lookups: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;


    /* Username map file */
    widgets->server_set_entry[ent] =
        make_long_entry_with_label(GTK_TABLE(widgets->srv_set_table), _(" SAMBA username file: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("The path to SAMBA's username file, IE: /etc/samba/smbusers"));
    a++;
    b++;
    ent++;

    /* DEPRECATED: Samba password file, leave these... */
    a++;
    b++;
    ent++;

    /* The systems passwd program */
    widgets->server_set_entry[ent] =
        make_long_entry_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" The systems password program: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("The path to the systems passwd program, IE: /usr/bin/passwd"));
    a++;
    b++;
    ent++;

    /*  Leave these... */
    a++;
    b++;
    ent++;

    /* Password chat phrase */
    widgets->server_set_entry[ent] =
        make_long_entry_with_label(GTK_TABLE(widgets->srv_set_table), _(" New password phraze: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("The new password phrase, IE: *new*password* \%n\\n *ReType*new*password* \%n\\n *password:*changed*sucessfully*"));
    a++;
    b++;
    ent++;

    /* Add user script */
    widgets->server_set_entry[ent] =
        make_long_entry_with_label(GTK_TABLE(widgets->srv_set_table), _(" Add user script: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("Add user script, IE: /usr/sbin/useradd -d /dev/null -c 'Samba User Account' -s /dev/null"));
    a++;
    b++;
    ent++;

    /* Add user to group script */
    widgets->server_set_entry[ent] =
        make_long_entry_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Add user to group script: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("Add user to group script, IE: /usr/sbin/useradd -d /dev/null -c 'Samba User Account' -s /dev/null -g '%g'"));
    a++;
    b++;
    ent++;

    /* Add group script */
    widgets->server_set_entry[ent] =
        make_long_entry_with_label(GTK_TABLE(widgets->srv_set_table), _(" Add group script: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("Add group script, IE: /usr/sbin/groupadd '%g'"));
    a++;
    b++;
    ent++;

    /* Delete user script */
    widgets->server_set_entry[ent] =
        make_long_entry_with_label(GTK_TABLE(widgets->srv_set_table), _(" Delete user script: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("Delete user script, IE: /usr/sbin/userdel '%u'"));
    a++;
    b++;
    ent++;

    /* Delete user from group script */
    widgets->server_set_entry[ent] =
        make_long_entry_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Delete user from group script: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("Delete user from group script, IE: /usr/sbin/userdel '%u' '%g'"));
    a++;
    b++;
    ent++;

    /* Delete group script */
    widgets->server_set_entry[ent] =
        make_long_entry_with_label(GTK_TABLE(widgets->srv_set_table), _(" Delete group script: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("Delete group script, IE: /usr/sbin/groupdel '%g'"));
    a++;
    b++;
    ent++;

    /* Add machine account script */
    widgets->server_set_entry[ent] =
        make_long_entry_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Add machine account script: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("IE: /usr/sbin/useradd -d /dev/null -g sambamachines -c 'Samba Machine Account' -s /dev/null -M %u"));
    a++;
    b++;
    ent++;

    /* Machine password timeout */
    widgets->server_set_spinbutton[spin] =
        make_short_spinbutton_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Machine password timeout: "), 0, 1, a, b, 50);
    gtk_widget_set_tooltip_text( widgets->server_set_spinbutton[spin],
        _("Machine password timeout, IE: 120 (seconds)"));
    a++;
    b++;
    spin++;

    /* idmap_uid */
    widgets->server_set_entry[ent] =
        make_entry_with_label(GTK_TABLE(widgets->srv_set_table), _(" Username id range: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("The users uids, IE: 16777216-33554431"));
    a++;
    b++;
    ent++;

    /* idmap_gid */
    widgets->server_set_entry[ent] =
        make_entry_with_label(GTK_TABLE(widgets->srv_set_table), _(" Group id range: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("The users gids, IE: 16777216-33554431"));
    a++;
    b++;
    ent++;

    /* Template shell */
    widgets->server_set_entry[ent] =
        make_entry_with_label(GTK_TABLE(widgets->srv_set_table), _(" Template shell: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("The template shell, IE: /dev/null"));
    a++;
    b++;
    ent++;


    /* Winbind use default domain */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Winbind use default domain: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Winbind separator */
    widgets->server_set_entry[ent] =
        make_entry_with_label(GTK_TABLE(widgets->srv_set_table), _(" Winbind separator: "), 0, 1, a, b, 200);
    gtk_widget_set_tooltip_text( widgets->server_set_entry[ent],
        _("Winbind separator, IE: @ for user@server.org"));
    a++;
    b++;
    ent++;

    /* Winbind cache time */
    widgets->server_set_spinbutton[spin] =
        make_short_spinbutton_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Winbind cache timeout: "), 0, 1, a, b, 50);
    gtk_widget_set_tooltip_text( widgets->server_set_spinbutton[spin],
        _("Time until winbind cache times out."));
    a++;
    b++;
    spin++;

    /* Winbind trusted domains only */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Winbind trusted domains only: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Winbind nested groups */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Winbind nested groups: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Winbind nss info */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table), _(" Winbind nss info: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Winbind refresh tickets */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Winbind refresh tickets: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    /* Winbind offline logon */
    widgets->server_set_combo[comb] =
        make_short_combo_with_label(GTK_TABLE(widgets->srv_set_table),
        _(" Winbind offline logon: "), 0, 1, a, b, 50);
    combo_text = g_strdup_printf(_("Yes"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    combo_text = g_strdup_printf(_("No"));
    utf8 = g_locale_to_utf8(combo_text, strlen(combo_text), NULL, NULL, NULL);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets->server_set_combo[comb]), utf8);
    g_free(combo_text);
    g_free(utf8);
    a++;
    b++;
    comb++;

    gtk_widget_show_all(widgets->main_window);
}
