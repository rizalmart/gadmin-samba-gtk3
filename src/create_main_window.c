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
#include "credits_window.h"
#include "activate_button_clicked.h"
#include "deactivate_button_clicked.h"
#include "apply_button_clicked.h"
#include "show_help.h"
#include "settings_window.h"



void create_main_window(struct w *widgets)
{
    gchar *info, *pixmap_directory;
    GtkCellRenderer *pixbuf_cell_renderer;
    GdkPixbuf *pixbuf;

    /* Create the main window */
    widgets->main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(widgets->main_window), GTK_WIN_POS_CENTER);
    gtk_widget_set_size_request(widgets->main_window, -1, 490);

    /* Set window information */
    info = g_strdup_printf("GADMIN-SAMBA %s", VERSION);
    gtk_window_set_title(GTK_WINDOW(widgets->main_window), info);
    g_free(info);

    /* Set the main window icon */
    pixmap_directory = g_strdup_printf("%s/pixmaps/gadmin-samba", PACKAGE_DATA_DIR);
    add_pixmap_directory(pixmap_directory);
    g_free(pixmap_directory);

    pixbuf_cell_renderer = gtk_cell_renderer_pixbuf_new();
    pixbuf = create_pixbuf("gadmin-samba.png");
    g_object_set(pixbuf_cell_renderer, "pixbuf", pixbuf, NULL);
    gtk_window_set_icon(GTK_WINDOW(widgets->main_window), pixbuf);
    g_object_unref(pixbuf);

    //widgets->main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    widgets->main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    
    gtk_container_add(GTK_CONTAINER(widgets->main_window), widgets->main_vbox);

    /* Hboxes (down) */
    //GtkWidget *toolbar_hbox = gtk_hbox_new(FALSE, 0);
    GtkWidget *toolbar_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    
    gtk_box_pack_start(GTK_BOX(widgets->main_vbox), toolbar_hbox, FALSE, FALSE, 0);

    //GtkWidget *status_hbox = gtk_hbox_new(FALSE, 0);
    GtkWidget *status_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    
    gtk_box_pack_start(GTK_BOX(widgets->main_vbox), status_hbox, FALSE, FALSE, 0);

    //GtkWidget *status_hsep_hbox = gtk_hbox_new(TRUE, 0);
    GtkWidget *status_hsep_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(status_hsep_hbox),TRUE);
    
    gtk_box_pack_start(GTK_BOX(widgets->main_vbox), status_hsep_hbox, FALSE, TRUE, 0);

    //GtkWidget *notebook_hbox = gtk_hbox_new(TRUE, 0);
    GtkWidget *notebook_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(notebook_hbox),TRUE);
    
    gtk_box_pack_start(GTK_BOX(widgets->main_vbox), notebook_hbox, TRUE, TRUE, 0);


    /* Create the main toolbar */
    GtkWidget *main_toolbar = gtk_toolbar_new();
    gtk_box_pack_start(GTK_BOX(toolbar_hbox), main_toolbar, TRUE, TRUE, 0);

    gtk_toolbar_set_style(GTK_TOOLBAR(main_toolbar), GTK_TOOLBAR_BOTH);

    
    GtkWidget *activate_icon = gtk_image_new_from_icon_name("gtk-yes", GTK_ICON_SIZE_SMALL_TOOLBAR);
    GtkWidget *deactivate_icon = gtk_image_new_from_icon_name("gtk-no", GTK_ICON_SIZE_SMALL_TOOLBAR);
    GtkWidget *apply_icon = gtk_image_new_from_icon_name("gtk-refresh", GTK_ICON_SIZE_SMALL_TOOLBAR);
    GtkWidget *settings_icon = gtk_image_new_from_icon_name("gtk-index", GTK_ICON_SIZE_SMALL_TOOLBAR);
    GtkWidget *help_icon = gtk_image_new_from_icon_name("gtk-help", GTK_ICON_SIZE_SMALL_TOOLBAR);
    GtkWidget *about_icon = gtk_image_new_from_icon_name("gtk-about", GTK_ICON_SIZE_SMALL_TOOLBAR);
    GtkWidget *quit_icon = gtk_image_new_from_icon_name("gtk-quit", GTK_ICON_SIZE_SMALL_TOOLBAR);

    
    GtkToolItem *activate_button = gtk_tool_button_new(activate_icon, _("Activate"));
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), activate_button, -1);
	g_signal_connect_swapped(G_OBJECT(activate_button), "clicked",
                         G_CALLBACK(activate_button_clicked), widgets);

    
	GtkToolItem *deactivate_button = gtk_tool_button_new(deactivate_icon, _("Deactivate"));
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), deactivate_button, -1);
	g_signal_connect_swapped(G_OBJECT(deactivate_button), "clicked",
                            G_CALLBACK(deactivate_button_clicked), widgets);
 

	GtkToolItem *apply_button = gtk_tool_button_new(apply_icon, _("Apply"));
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), apply_button, -1);
	g_signal_connect_swapped(G_OBJECT(apply_button), "clicked",
							 G_CALLBACK(apply_button_clicked), widgets);
	
	
	GtkToolItem *settings_button = gtk_tool_button_new(settings_icon, _("Settings"));
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), settings_button, -1);
	g_signal_connect_swapped(G_OBJECT(settings_button), "clicked",
							 G_CALLBACK(show_settings_window), widgets);
	

	GtkToolItem *help_button = gtk_tool_button_new(help_icon, _("Help"));
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), help_button, -1);
	g_signal_connect_swapped(G_OBJECT(help_button), "clicked",
							 G_CALLBACK(show_help), widgets);
		
 
	GtkToolItem *about_button = gtk_tool_button_new(about_icon, _("About"));
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), about_button, -1);
	g_signal_connect_swapped(G_OBJECT(about_button), "clicked",
							 G_CALLBACK(show_credits), widgets);
   
	
 	GtkToolItem *quit_button = gtk_tool_button_new(quit_icon, _("Quit"));
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), quit_button, -1);
	g_signal_connect_swapped(G_OBJECT(quit_button), "clicked",
							 G_CALLBACK(gtk_main_quit), widgets);	
	
	
    /* The notebook */
    //GtkWidget *notebook_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *notebook_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(notebook_hbox), notebook_vbox, TRUE, TRUE, 0);

    widgets->notebook_vbox1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    widgets->notebook_vbox2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    widgets->notebook_vbox3 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    widgets->notebook_vbox4 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    widgets->notebook_vbox5 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    widgets->notebook_vbox6 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    widgets->notebook_vbox7 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    widgets->notebook_vbox8 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    widgets->notebook_vbox9 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    GtkWidget *main_notebook = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(notebook_vbox), main_notebook, TRUE, TRUE, 0);
    gtk_notebook_set_show_border(GTK_NOTEBOOK(main_notebook), TRUE);


//    gtk_notebook_set_tab_label_packing(GTK_NOTEBOOK(main_notebook), notebook_vbox,
//                                      TRUE, TRUE, GTK_PACK_START);


    GtkWidget *server_label = gtk_label_new(_("Server settings"));
    GtkWidget *user_label = gtk_label_new(_("Users"));
    GtkWidget *resource_label = gtk_label_new(_("Shares"));

    GtkWidget *maccts_label = gtk_label_new(_("Machines"));
    //GtkWidget *dom_mgmt_label = gtk_label_new(_("Domains"));

    GtkWidget *conn_label = gtk_label_new(_("Connections"));

    GtkWidget *disc_label = gtk_label_new(_("Disk space"));
    GtkWidget *security_label = gtk_label_new(_("Security"));
    GtkWidget *conf_label = gtk_label_new(_("Configuration"));

    gtk_notebook_insert_page(GTK_NOTEBOOK(main_notebook), widgets->notebook_vbox1, server_label, 0);
    gtk_notebook_insert_page(GTK_NOTEBOOK(main_notebook), widgets->notebook_vbox2, user_label, 1);
    gtk_notebook_insert_page(GTK_NOTEBOOK(main_notebook), widgets->notebook_vbox3, resource_label, 2);
    gtk_notebook_insert_page(GTK_NOTEBOOK(main_notebook), widgets->notebook_vbox4, maccts_label, 3);
    //gtk_notebook_insert_page(GTK_NOTEBOOK(main_notebook), widgets->notebook_vbox5, dom_mgmt_label, 4);
    gtk_notebook_insert_page(GTK_NOTEBOOK(main_notebook), widgets->notebook_vbox6, conn_label, 5);
    gtk_notebook_insert_page(GTK_NOTEBOOK(main_notebook), widgets->notebook_vbox7, disc_label, 6);
    gtk_notebook_insert_page(GTK_NOTEBOOK(main_notebook), widgets->notebook_vbox8, security_label, 7);
    gtk_notebook_insert_page(GTK_NOTEBOOK(main_notebook), widgets->notebook_vbox9, conf_label, 8);

// Do we want to have the notebook labels expanded ???
//    gtk_widget_set_size_request(GTK_WIDGET(server_label), -1, -1);
//    gtk_label_set_justify (GTK_LABEL (server_label), GTK_JUSTIFY_LEFT);

    /* Set version and status labels */
    widgets->version_label = gtk_label_new(_("Information: cant read version"));
    gtk_box_pack_start(GTK_BOX(status_hbox), widgets->version_label, FALSE, FALSE, 0);
    //gtk_misc_set_alignment(GTK_MISC(widgets->version_label), 0, 0);
	gtk_widget_set_halign(widgets->version_label, GTK_ALIGN_START);
	gtk_widget_set_valign(widgets->version_label, GTK_ALIGN_START);

    GtkWidget *status_spacer_label = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(status_hbox), status_spacer_label, TRUE, TRUE, 0);
    //gtk_misc_set_alignment(GTK_MISC(status_spacer_label), 0, 0);
	gtk_widget_set_halign(status_spacer_label, GTK_ALIGN_START);
	gtk_widget_set_valign(status_spacer_label, GTK_ALIGN_START);   

    widgets->status_label = gtk_label_new(_("Status: unknown"));
    gtk_box_pack_start(GTK_BOX(status_hbox), widgets->status_label, FALSE, FALSE, 0);
    //gtk_misc_set_alignment(GTK_MISC(widgets->status_label), 0, 0);
	gtk_widget_set_halign(widgets->status_label, GTK_ALIGN_START);
	gtk_widget_set_valign(widgets->status_label, GTK_ALIGN_START);


    GtkWidget *status_hseparator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(status_hsep_hbox), status_hseparator, TRUE, TRUE, 0);
    gtk_widget_set_size_request(status_hseparator, 10, 10);


    gtk_widget_show_all(widgets->main_window);
}
