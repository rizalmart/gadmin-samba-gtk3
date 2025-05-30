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



#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <gtk/gtk.h>

extern struct w *widgets;

void add_global_option(char *option);

void remove_global_option(char *option);

int cmplowercase(char *buf, char *cmpstring);

char *encrypt_password(const gchar *password);

int fix_newlines_in_conf();

void fix_path(char *path);

void randomize_username(struct w *widgets);

void randomize_password(struct w *widgets);

char *random_string(int len);

int smbuser_exists(const gchar*username);

int smbuser_locked(const gchar*username);

int user_exists(const gchar *username);

int group_exists(const gchar *username);

gchar *get_user_comment(gchar *user);

gchar *get_user_setting(gchar *user, gchar *what);

gchar *get_group_name(gchar *groupnr);

void password_user(const gchar *username, const gchar *password);

int is_banned(char *user);

void make_file_chmod(gchar *file, char perm[128]);
void make_dir_chmod(gchar *directory, char perm[128]);

int file_exists(char *entity);

#endif
