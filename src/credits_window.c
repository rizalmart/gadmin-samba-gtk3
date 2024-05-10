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
#include "gettext.h"
#include "credits_window.h"


void show_credits()
{
    gchar *credits;
    GtkWidget *credits_window;
    GtkWidget *credits_vbox;
    GtkWidget *credits_scrolledwindow;
    GtkWidget *credits_textview;
    GtkWidget *close_credits_button;
    GtkWidget *credits_alignment, *credits_hbox;
    GtkWidget *credits_image, *credits_label;

    credits_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(credits_window, 550, 350);
    gtk_window_set_title(GTK_WINDOW(credits_window), _("Credits"));
    gtk_window_set_position(GTK_WINDOW(credits_window), GTK_WIN_POS_CENTER);

    credits_vbox = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(credits_vbox);
    gtk_container_add(GTK_CONTAINER(credits_window), credits_vbox);

    credits_scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_show(credits_scrolledwindow);
    gtk_box_pack_start(GTK_BOX(credits_vbox), credits_scrolledwindow, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(credits_scrolledwindow),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(credits_scrolledwindow),
        GTK_SHADOW_ETCHED_OUT);

    credits_textview = gtk_text_view_new();
    gtk_widget_set_name(credits_textview, "credits_textview");
    gtk_widget_show(credits_textview);
    gtk_container_add(GTK_CONTAINER(credits_scrolledwindow), credits_textview);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(credits_textview), FALSE);
    gtk_text_view_set_justification(GTK_TEXT_VIEW(credits_textview), GTK_JUSTIFY_CENTER);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(credits_textview), FALSE);
    gtk_text_view_set_pixels_above_lines(GTK_TEXT_VIEW(credits_textview), 4);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(credits_textview), 29);

    credits = g_strconcat("\n",
        _("Author: "), "Magnus Loef <magnus-swe@telia.com>\n",
        "\n",
        _("I want to thank the following for their contributions:"),
        "\n-------------------------------------------------------------------------\n",
        _("The SAMBA developers.\n\n"),
        "Bjoern Lindner ", _("for the program icons.\n"),
        "Rudolf Kastl <che666@uni.de> ", _("for the RPM spec file."),
        "\n\n",
        _("And all translators:"),
        "\n-------------------------------------------------------------------------\n",
        _("Afrikaans"), " (af) NAME <EMAIL>\n",
        _("Amharic"), " (am) NAME <EMAIL>\n",
        _("Arabic"), " (ar) NAME <EMAIL>\n",
        _("Azerbaijani"), " (az) NAME <EMAIL>\n",
        _("Basque"), " (eu) NAME <EMAIL>\n",
        _("Bulgarian"), " (bg) NAME <EMAIL>\n",
        _("Breton"), " (br) NAME <EMAIL>\n",
        _("Brittish-English"), " (en_GB) NAME <EMAIL>\n",
        _("Bosnian"), " (bs) NAME <EMAIL>\n",
        _("Byelorussian"), " (be) NAME <EMAIL>\n",
        _("Catalan"), " (ca) NAME <EMAIL>\n",
        _("Chinese"), " (zh_CN) <EMAIL>\n",
        _("Chinese-Taiwan"), " (zh_TW) NAME <EMAIL>\n",
        _("Croatian"), " (hr) NAME <EMAIL>\n",
        _("Czech"), " (cs) NAME <EMAIL>\n",
        _("Danish"), " (dk) NAME <EMAIL>\n",
        _("Dutch"), " (nl) NAME <EMAIL>\n",
        _("Esperanto"), " (eo) NAME <EMAIL>\n",
        _("Estonian"), " (et) NAME <EMAIL>\n",
        _("Finnish"), " (fi) NAME <EMAIL>\n",
        _("French"), " (fr) Martial Lillo <HiddenEmail>\n",
        _("Galician"), " (gl) NAME <EMAIL>\n",
        _("German"), " (de) M.Mueller aka Quax <mmueller@live-modules.org>\n",
        _("Greek"), " (el) NAME <EMAIL>\n",
        _("Hebrew"), " (he) NAME <EMAIL>\n",
        _("Hini"), " (hi) NAME <EMAIL>\n",
        _("Hungarian"), " (hu) NAME <EMAIL>\n",
        _("Icelandic"), " (is) NAME <EMAIL>\n",
        _("Indonesian"), " (id) NAME <EMAIL>\n",
        _("Irish"), " (ga) NAME <EMAIL>\n",
        _("Italian"), " (it) NAME <EMAIL>\n",
        _("Japanese"), " (ja) NAME <EMAIL>\n",
        _("Korean"), " (ko) NAME <EMAIL>\n",
        _("Kurdish"), " (ku) NAME <EMAIL>\n",
        _("Latvian-Lettish"), " (lv) NAME <EMAIL>\n",
        _("Lithuanian"), " (lt) NAME <EMAIL>\n",
        _("Macedonian"), " (mk) NAME <EMAIL>\n",
        _("Maori"), " (mi) NAME <EMAIL>\n",
        _("Macedonian"), " (mk) NAME <EMAIL>\n",
        _("Maltese"), " (mt) NAME <EMAIL>\n",
        _("Norwegian"), " (no) NAME <EMAIL>\n",
        _("Polish"), " (pl) NAME <EMAIL>\n",
        _("Portuguese"), " (pt) (pt_BR) NAME <EMAIL>\n",
        _("Portuguese-Brazil"), " (pt_BR) NAME <EMAIL>\n",
        _("Romanian"), " (ro) NAME <EMAIL>\n",
        _("Russian"), " (ru) NAME <EMAIL>\n",
        _("Serbian"), " (sr) NAME <EMAIL>\n",
        _("Slovak"), " (sk) NAME <EMAIL>\n",
        _("Slovenian"), " (sl) NAME <EMAIL>\n",
        _("Spanish"), " (es) NAME <EMAIL>\n",
        _("Swedish"), " (sv) Magnus Loef <magnus-swe@telia.com>\n",
        _("Tamil"), " (ta) NAME <EMAIL>\n",
        _("Thai"), " (th) NAME <EMAIL>\n",
        _("Turkish"), " (tr) NAME <EMAIL>\n",
        _("Traditional Chinese"), " (zh_TW) NAME <EMAIL>\n",
        _("Ukrainian"), " (uk) NAME <EMAIL>\n",
        _("Vietnamese"), " (vi) NAME <EMAIL>\n",
        _("Wallon"), " (wa) NAME <EMAIL>\n", _("Welch"), " (cy) NAME <EMAIL>\n", "\n", NULL);

    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(credits_textview)), credits, -1);

    if( credits != NULL )
        g_free(credits);

    close_credits_button = gtk_button_new();
    gtk_widget_show(close_credits_button);
    gtk_box_pack_start(GTK_BOX(credits_vbox), close_credits_button, FALSE, FALSE, 0);

    credits_alignment = gtk_alignment_new(0.5, 0.5, 0, 0);
    gtk_widget_show(credits_alignment);
    gtk_container_add(GTK_CONTAINER(close_credits_button), credits_alignment);

    credits_hbox = gtk_hbox_new(FALSE, 2);
    gtk_widget_show(credits_hbox);
    gtk_container_add(GTK_CONTAINER(credits_alignment), credits_hbox);

    credits_image = gtk_image_new_from_stock("gtk-close", GTK_ICON_SIZE_BUTTON);
    gtk_widget_show(credits_image);
    gtk_box_pack_start(GTK_BOX(credits_hbox), credits_image, FALSE, FALSE, 0);

    credits_label = gtk_label_new_with_mnemonic(_("Close"));
    gtk_widget_show(credits_label);
    gtk_box_pack_start(GTK_BOX(credits_hbox), credits_label, FALSE, FALSE, 0);
    gtk_label_set_justify(GTK_LABEL(credits_label), GTK_JUSTIFY_LEFT);

    g_signal_connect_swapped((gpointer) close_credits_button, "clicked",
        G_CALLBACK(gtk_widget_destroy), G_OBJECT(credits_window));

    gtk_widget_show(credits_window);
}
