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
#include "../config.h"
#include "support.h"
#include "gettext.h"
#include "widgets.h"



void show_help()
{
    gchar *help_text;
    GtkWidget *help_window, *vbox15, *scrolledwindow16;
    GtkWidget *help_textview, *close_help_button;
    GtkWidget *alignment19, *hbox52, *image19, *label109;

    help_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_name(help_window, "help_window");
    gtk_widget_set_size_request(help_window, 650, 350);
    gtk_window_set_title(GTK_WINDOW(help_window), _("GADMIN-SAMBA Help"));
    gtk_window_set_position(GTK_WINDOW(help_window), GTK_WIN_POS_CENTER);

    vbox15 = gtk_vbox_new(FALSE, 0);
    gtk_widget_set_name(vbox15, "vbox15");
    gtk_widget_show(vbox15);
    gtk_container_add(GTK_CONTAINER(help_window), vbox15);

    scrolledwindow16 = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_name(scrolledwindow16, "scrolledwindow16");
    gtk_widget_show(scrolledwindow16);
    gtk_box_pack_start(GTK_BOX(vbox15), scrolledwindow16, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow16), GTK_POLICY_AUTOMATIC,
        GTK_POLICY_ALWAYS);

    help_textview = gtk_text_view_new();
    gtk_widget_set_name(help_textview, "help_textview");
    gtk_widget_show(help_textview);
    gtk_container_add(GTK_CONTAINER(scrolledwindow16), help_textview);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(help_textview), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(help_textview), FALSE);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(help_textview), 30);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(help_textview), 10);

    help_text = g_strconcat(_("\nServer settings:\n"),
        _("If all you want to do is share some directories you only need to change\n"),
        _("the first visible part in the server settings tab and then press apply.\n"),
        _("When you scroll down more advanced options will be shown.\n"),
        _("\nUsers:\n"),
        _("Adding a user can be done by specifying its username, group, password\n"),
        _("and other settings, then clicking apply.\n"),
        _("After one user has been added this user can serve as a profile for other users.\n"),
        _("Select a user in the user list, change its username and password then click apply.\n"),
        _("Both users will then have the same settings except for the username and password.\n"),
        _("The new user button can be used to clear and insert default user settings, making it\n"),
        _("ready for adding new users.\n\n"),
        _("Deleting a user can be done by selecting it in the list and then clicking delete.\n"),
        _("The deleted user will also be deleted from any shared resource.\n\n"),
        _("Changing a users settings is done by changing the settings and clicking apply.\n"),
        _("When you add a user you can randomize its name and password or use your own.\n"),
        _
        ("You can also select its login shell and where this user should have its home directory.\n"),
        _("If the selected shell is /dev/null or /sbin/nologin the user can only login to this\n"),
        _
        ("server, otherwise the user can login via ssh etc if the server allows this user to do so.\n\n"),
        _("Shares:\n"),
        _
        ("Shares can be added by selecting a directory to share, setting its share permissions and clicking add.\n"),
        _("Deleting a share is done by selecting the share to delete and clicking delete.\n"),
        _
        ("Changing share settings can be done by changing its settings and then clicking the apply button.\n\n"),
        _("Adding user and group access to shares:\n"),
        _("Share access for users and groups can be added in several ways. Those ways are:\n"),
        _
        ("local users and groups, remote domain users and groups and each of these can have 4 different\n"),
        _
        ("permission classes. Multiple users and groups can be imported at once and duplicate additions are ignored.\n\n"),
        _
        ("The new share button can be used to clear and insert default share settings, making it ready\n"),
        _("for adding new shares.\n\n"),
        _("Joining a samba domain with other types of clients as shown below:\n"),
        _("First disconnect all current connections on the client with: net use * /d\n"),
        _
        ("As administrator, select properties for \"this computer\" and write example.org in the domain\n"),
        _
        ("field then press join domain. Use root as username and the samba root password in the dialog.\n\n"),
        _("Joining the domain with Linux or other open source clients:\n"),
        _("net rpc join example.org -S samba24 -U root then supply the samba root password\n\n"),
        _("Mounting shares:\n"),
        _
        ("If you want to add directories thats not under the users root directory you can do this:\n\n"),
        _("Linux (as of kernel 2.4.0):\n"),
        _("mount --bind /some/directory/to/share /home/bob/make_this_directory_first\n\n"),
        _("Alternatively:\nmount -o bind /var/data /home/bob/mounted_data\n\n"),
        _("*BSD (as of 4.4BSD):\nmount_null /var/data /home/bob/mounted_data\n\n"),
        _("Solaris:\nmount -F lofs /var/data /home/bob/mounted_data\n\n"),
        _
        ("For more detailed information about the server and its configuration directives visit:\n"),
        _("http://www.samba.org\n"), NULL);


    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(help_textview)), help_text, -1);

    if(help_text != NULL)
        g_free(help_text);


    close_help_button = gtk_button_new();
    gtk_widget_set_name(close_help_button, "close_help_button");
    gtk_widget_show(close_help_button);
    gtk_box_pack_start(GTK_BOX(vbox15), close_help_button, FALSE, FALSE, 0);

    alignment19 = gtk_alignment_new(0.5, 0.5, 0, 0);
    gtk_widget_set_name(alignment19, "alignment19");
    gtk_widget_show(alignment19);
    gtk_container_add(GTK_CONTAINER(close_help_button), alignment19);

    hbox52 = gtk_hbox_new(FALSE, 2);
    gtk_widget_set_name(hbox52, "hbox52");
    gtk_widget_show(hbox52);
    gtk_container_add(GTK_CONTAINER(alignment19), hbox52);

    image19 = gtk_image_new_from_stock("gtk-close", GTK_ICON_SIZE_BUTTON);
    gtk_widget_set_name(image19, "image19");
    gtk_widget_show(image19);
    gtk_box_pack_start(GTK_BOX(hbox52), image19, FALSE, FALSE, 0);

    label109 = gtk_label_new_with_mnemonic(_("Close"));
    gtk_widget_set_name(label109, "label109");
    gtk_widget_show(label109);
    gtk_box_pack_start(GTK_BOX(hbox52), label109, FALSE, FALSE, 0);
    gtk_label_set_justify(GTK_LABEL(label109), GTK_JUSTIFY_LEFT);

    g_signal_connect_swapped(close_help_button, "clicked",
    G_CALLBACK(gtk_widget_destroy), G_OBJECT(help_window));

    gtk_widget_show_all(help_window);
}
