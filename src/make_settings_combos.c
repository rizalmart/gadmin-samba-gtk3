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
#include "make_settings_combos.h"


GtkWidget * make_combo_with_label(GtkTable * table,
    const gchar * label_text,
    gint left_attach, gint right_attach, gint top_attach, gint bottom_attach, gint combo_length)
{
    GtkWidget *combo;
    GtkWidget *label;
    label = gtk_label_new(label_text);
    combo = gtk_combo_box_text_new();

    gtk_table_attach(table, label, 0, 0 + 1, top_attach, bottom_attach, GTK_FILL, GTK_EXPAND, 2, 2);
    gtk_table_attach(table, combo, left_attach + 1, right_attach + 1, top_attach, bottom_attach,
        GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 20, 2);

    //gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
	gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
    
    //gtk_misc_set_padding(GTK_MISC(label), 2, 0);

	gtk_widget_set_margin_top(label, 2);
	gtk_widget_set_margin_bottom(label, 2);
	gtk_widget_set_margin_start(label, 2);
	gtk_widget_set_margin_end(label, 2);


    gtk_widget_show(combo);
    gtk_widget_show(label);

    return combo;
}

GtkWidget * make_short_combo_with_label(GtkTable * table,
    const gchar * label_text,
    gint left_attach, gint right_attach, gint top_attach, gint bottom_attach, gint combo_length)
{
    GtkWidget *combo;
    GtkWidget *label;
    label = gtk_label_new(label_text);
    combo = gtk_combo_box_text_new();

    gtk_table_attach(table, label, 0, 0 + 1, top_attach, bottom_attach, GTK_FILL, GTK_EXPAND, 2, 2);
    gtk_table_attach(table, combo, left_attach + 2, right_attach + 2, top_attach, bottom_attach,
        GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 20, 2);

    //gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
    
	gtk_widget_set_halign(label, GTK_ALIGN_START);
	gtk_widget_set_valign(label, GTK_ALIGN_CENTER);   
    
    //gtk_misc_set_padding(GTK_MISC(label), 2, 0);

	gtk_widget_set_margin_top(label, 2);
	gtk_widget_set_margin_bottom(label, 2);
	gtk_widget_set_margin_start(label, 2);
	gtk_widget_set_margin_end(label, 2);

    gtk_widget_set_size_request(combo, combo_length, -1);

    gtk_widget_show(combo);
    gtk_widget_show(label);

    return combo;
}


GtkWidget * make_label_combo_label(GtkTable * table,
    const gchar * label_text,
    gint left_attach, gint right_attach, gint top_attach, gint bottom_attach, gint combo_length)
{
    GtkWidget *label, *pad_label;
    GtkWidget *combo;
    GtkWidget *hbox;

    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    label = gtk_label_new(label_text);
    combo = gtk_combo_box_text_new();
    pad_label = gtk_label_new("");

    gtk_box_pack_start(GTK_BOX(hbox), combo, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), pad_label, FALSE, FALSE, 0);

    gtk_table_attach(table, label, 0, 1, top_attach, bottom_attach, GTK_FILL, GTK_EXPAND, 2, 2);
    gtk_table_attach(table, hbox, 1, 2, top_attach, bottom_attach, GTK_FILL, GTK_EXPAND, 20, 2);

//    gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
	gtk_widget_set_halign(label, GTK_ALIGN_START);
	gtk_widget_set_valign(label, GTK_ALIGN_CENTER);  
  
    
//    gtk_misc_set_padding(GTK_MISC(label), 2, 0);
	gtk_widget_set_margin_top(label, 2);
	gtk_widget_set_margin_bottom(label, 2);
	gtk_widget_set_margin_start(label, 2);
	gtk_widget_set_margin_end(label, 2);


    gtk_widget_show(label);
    gtk_widget_show(combo);
    gtk_widget_show(pad_label);

    return combo;
}


/* A clone of the above but with a text entry combo for shells etc */
GtkWidget * make_label_textcombo_label(GtkTable * table,
    const gchar * label_text,
    gint left_attach, gint right_attach, gint top_attach, gint bottom_attach, gint combo_length)
{
    GtkWidget *label, *pad_label;
    GtkWidget *combo;
    GtkWidget *hbox;

    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    label = gtk_label_new(label_text);
    //combo = gtk_combo_box_new_with_entry();
    combo = gtk_combo_box_text_new();
    pad_label = gtk_label_new("");

    gtk_box_pack_start(GTK_BOX(hbox), combo, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), pad_label, FALSE, FALSE, 0);

    gtk_table_attach(table, label, 0, 1, top_attach, bottom_attach, GTK_FILL, GTK_EXPAND, 2, 2);
    gtk_table_attach(table, hbox, 1, 2, top_attach, bottom_attach, GTK_FILL, GTK_EXPAND, 20, 2);

    //gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
	gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
    
    //gtk_misc_set_padding(GTK_MISC(label), 2, 0);

	gtk_widget_set_margin_top(label, 2);
	gtk_widget_set_margin_bottom(label, 2);
	gtk_widget_set_margin_start(label, 2);
	gtk_widget_set_margin_end(label, 2);

    gtk_widget_show(label);
    gtk_widget_show(combo);
    gtk_widget_show(pad_label);

    return combo;
}


GtkWidget * make_combo(GtkTable * table,
    gint left_attach, gint right_attach, gint top_attach, gint bottom_attach, gint combo_length)
{
    GtkWidget *combo;
    combo = gtk_combo_box_text_new();

    gtk_table_attach(table, combo, left_attach + 1, right_attach + 1, top_attach, bottom_attach,
        GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 20, 2);

    gtk_widget_set_size_request(combo, combo_length, -1);

    gtk_widget_show(combo);

    return combo;
}
