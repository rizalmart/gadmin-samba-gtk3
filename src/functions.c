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
#include "support.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include "widgets.h"
#include "chars_are_digits.h"
#include "allocate.h"
#include "functions.h"
#include "show_info.h"
#include "system_defines.h"
#include "commands.h"
#include "commented.h"
#include "get_option_pos.h"

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif

#if ! defined USE_DARWIN
#ifndef _CRYPT_H
#include <crypt.h>
#endif
#endif


/* Compare strings in lower case */
int cmplowercase(char *buf, char *cmpstring)
{
    char *p, *newbuf;
    int retval = 0;

    if( buf == NULL )
        return retval;

    /* Ignore single newline chars */
    if( strlen(buf) < 2 )
        return retval;

    newbuf = allocate(strlen(buf) + 1);
    /* Exists if it cant allocate */
    sprintf(newbuf, "%s", buf);

    for(p = newbuf; *p != '\0'; ++p)
        *p = tolower(*p);

    if( strstr(newbuf, cmpstring) )
        retval = 1;

    free(newbuf);

    return retval;
}


void add_global_option(char *option)
{
    /* Adds a global option if it doesnt exist */
    FILE *fp;
    char *line;
    char *new_buffer;
    long conf_size;
    int option_existed = 0;
    gchar *option1, *option2;

    /* To ways to identify an option and avoid mixups */
    option1 = g_strdup_printf("%s=", option);
    option2 = g_strdup_printf("%s =", option);

    if( option == NULL || strlen(option) < 3 )
    {
        printf("Add global option: Option length too short.\n");
        return;
    }

    if((fp = fopen(SAMBA_CONF, "r")) == NULL)
    {
        printf("Cant open: %s\n", SAMBA_CONF);
        return;
    }

    fseek(fp, 0, SEEK_END);
    conf_size = ftell(fp);
    rewind(fp);

    new_buffer = allocate(conf_size + 4096);
    line = allocate(conf_size + 1);

    /* Dont add a new option if it already exists */
    while(fgets(line, conf_size, fp) != NULL)
    {
        if( (cmplowercase(line, option1) || cmplowercase(line, option2)) && !commented(line) )
        {
            option_existed = 1;
            break;
        }
    }

    g_free(option1);
    g_free(option2);

    if( option_existed )
    {
        fclose(fp);
        free(new_buffer);
        free(line);
        return;
    }


    rewind(fp);

    /* Get everything up to and including "[global]" or the first option "=" */
    while(fgets(line, conf_size, fp) != NULL)
    {
        strcat(new_buffer, line);

        if( cmplowercase(line, "[global]") && !commented(line) )
            break;

        if( cmplowercase(line, "=") && !commented(line) )
            break;
    }

    /* Add the new option */
    strcat(new_buffer, option);
    strcat(new_buffer, " =\n");

    while(fgets(line, conf_size, fp) != NULL)
        strcat(new_buffer, line);

    fclose(fp);

    /* Write the new conf */
    if((fp = fopen(SAMBA_CONF, "w+")) == NULL)
    {
        printf("Cant write to: %s\n", SAMBA_CONF);
        return;
    }
    fputs(new_buffer, fp);
    fclose(fp);

    free(line);
    free(new_buffer);
}


void remove_global_option(char *option)
{
    FILE *fp;
    char *line;
    char *new_buffer;
    long conf_size;
    gchar *option1, *option2;

    /* To ways to identify an option and avoid mixups */
    option1 = g_strdup_printf("%s=", option);
    option2 = g_strdup_printf("%s =", option);


    if( option == NULL || strlen(option) < 3 )
    {
        printf("Remove global option: Option length too short.\n");
        g_free(option1);
        g_free(option2);
        return;
    }

    if((fp = fopen(SAMBA_CONF, "r")) == NULL)
    {
        printf("Cant open: %s\n", SAMBA_CONF);
        g_free(option1);
        g_free(option2);
        return;
    }
    fseek(fp, 0, SEEK_END);
    conf_size = ftell(fp);
    rewind(fp);

    new_buffer = allocate(conf_size + 4096);
    line = allocate(conf_size + 1);

    /* Get everything up to and including the first
       share definition "[" and skips the option */
    while(fgets(line, conf_size, fp) != NULL)
    {
        /* Break at any share definition */
        if( strstr(line, "[") && !cmplowercase(line, "[global]") && ! commented(line) )
        {
            strcat(new_buffer, line);
            break;
        }

        /* Skip the specified option. */
        if( (cmplowercase(line, option1) || cmplowercase(line, option2)) && ! commented(line) )
        {
            printf("Removed option: %s\n", option);
            continue;
        }
        else
            strcat(new_buffer, line);
    }

    g_free(option1);
    g_free(option2);

    /* Gather the rest */
    while(fgets(line, conf_size, fp) != NULL)
        strcat(new_buffer, line);

    fclose(fp);

    /* Write the new conf */
    if((fp = fopen(SAMBA_CONF, "w+")) == NULL)
    {
        printf("Cant write to: %s\n", SAMBA_CONF);
        return;
    }
    fputs(new_buffer, fp);
    fclose(fp);

    free(line);
    free(new_buffer);
}


int smbuser_exists(const gchar * username)
{
    /* Checks if the system user exists */
    FILE *fp;
    char *line;
    long conf_size;
    int x, user_exists = 0;
    char tempname[4096] = "";

    /* Puts the pdbedit userlist as:
       /etc/gadmin-samba/gadmin_samba_users */
    get_gadmin_samba_users();

    /* Checks if the user exists */
    if((fp = fopen(GADMIN_SAMBA_USERS, "r")) == NULL)
    {
        /* Dont show anything */
    }
    else
    {
        fseek(fp, 0, SEEK_END);
        conf_size = ftell(fp);
        rewind(fp);
        line = allocate(conf_size);

        if( conf_size > 1 )
        while(fgets(line, conf_size, fp) != NULL)
        {
            for(x = 0; line[x]; x++)
            {
                if(line[x] == ':')
                {
                    snprintf(tempname, 4000, "%s", line);
                    tempname[x] = '\0';
                    if( ! strcmp(username, tempname) )
                    {
                        user_exists = 1;
                        break;
                    }
                }
            }
            if( user_exists )
                break;
        }
        free(line);
        fclose(fp);
    }

    return user_exists;
}


int smbuser_locked(const gchar * username)
{
    /* Checks if the samba user is locked */
    /* Return values: 0="Not locked" 1="Locked" 2="NULL password" */
    FILE *fp;
    char *line;
    long conf_size;
    int x, user_exists = 0;
    int prop_found = 0, account_locked = 0;
    char tempname[4096] = "";

    get_gadmin_samba_users();

    if((fp = fopen(GADMIN_SAMBA_USERS, "r")) == NULL)
    {
        /* Dont show anything */
    }
    fseek(fp, 0, SEEK_END);
    conf_size = ftell(fp);
    rewind(fp);
    line = allocate(conf_size + 1);

    if( conf_size > 1 )
    while(fgets(line, conf_size, fp) != NULL)
    {
        for(x = 0; line[x] != '\0'; x++)
        {
            if( line[x] == ':' )
            {
                snprintf(tempname, 4000, "%s", line);
                tempname[x] = '\0';
                if( ! strcmp(username, tempname) )
                {
                    user_exists = 1;
                    break;
                }
            }
        }
        if( user_exists )
            break;
    }

    /* The samba user exists, see if its locked */
    if( user_exists )
        for(x = 0; line[x] != '\0'; x++)
        {
            /* Account properties are located between "[" and "]" */
            if( line[x] == '[' )
            {
                prop_found = 1;
                break;
            }
        }
    if( user_exists && prop_found )
        for(x = x; line[x] != '\0'; x++)
        {
            /* Read to the end of account properties "]" */
            if( line[x] == ']' )
                break;

            /* Disabled accounts with previously set passwords
               can be enabled without setting a new one */
            if( line[x] == 'D' )
            {
                account_locked = 1;
                break;
            }

            /* Enabling accounts with no previous passwords requires new passwords */
            if( strstr(line, "XXXXXXXXXXXX:[") )
            {
                account_locked = 2;
                break;
            }

            /* Enabling accounts with NULL passwords requires new passwords */
            if(line[x] == 'N')
                account_locked = 3;
        }

    free(line);
    fclose(fp);

    return account_locked;
}


int user_exists(const gchar * username)
{
    /* Checks if the system user exists */
    FILE *fp;
    char *line;
    long conf_size;
    int x, user_exists = 0;
    char tempname[4096] = "";

    /* Checks if the user exists in passwd */
    if((fp = fopen(PASSWD_FILE, "r")) == NULL)
    {
        printf("Unable to read file: [%s]\n", PASSWD_FILE);
        return 0;
    }
    else
    {
        fseek(fp, 0, SEEK_END);
        conf_size = ftell(fp);
        rewind(fp);
        line = allocate(conf_size);

        if( conf_size > 1 )
            while(fgets(line, conf_size, fp) != NULL)
            {
                for(x = 0; line[x]; x++)
                {
                    if(line[x] == ':')
                    {
                        snprintf(tempname, 4000, "%s", line);
                        tempname[x] = '\0';
                        if(strcmp(username, tempname) == 0)
                        {
                            user_exists = 1;
                            break;
                        }
                    }
                }
                if( user_exists )
                    break;
            }
        free(line);
        fclose(fp);
    }

    /* Checks if the user exists in shadow */
    if((fp = fopen(SHADOW_FILE, "r")) == NULL)
    {
        /* Dont show anything */
    }
    else
    {
        fseek(fp, 0, SEEK_END);
        conf_size = ftell(fp);
        rewind(fp);
        line = allocate(conf_size);

        if( conf_size > 1 )
            while(fgets(line, conf_size, fp) != NULL)
            {
                for(x = 0; line[x] != '\0'; x++)
                {
                    if(line[x] == ':')
                    {
                        snprintf(tempname, 4000, "%s", line);
                        tempname[x] = '\0';
                        if( strcmp(username, tempname) == 0 )
                        {
                            user_exists = 1;
                            break;
                        }
                    }
                    if( user_exists )
                        break;
                }
            }
        free(line);
        fclose(fp);
    }

    return user_exists;
}


int group_exists(const gchar * groupname)
{
    /* Checks if the system group already exists */
    FILE *fp;
    char *line;
    long conf_size;
    int x, group_exists = 0;
    char tempname[4096] = "";

    /* Checks if the group exists in group */
    if((fp = fopen(GROUP_FILE, "r")) == NULL)
    {
        /* Dont show anything  */
    }
    else
    {
        fseek(fp, 0, SEEK_END);
        conf_size = ftell(fp);
        rewind(fp);
        line = allocate(conf_size);

        if(conf_size > 1)
            while(fgets(line, conf_size, fp) != NULL)
            {
                for(x = 0; line[x] != '\0'; x++)
                {
                    if(line[x] == ':')
                    {
                        snprintf(tempname, 4000, "%s", line);
                        tempname[x] = '\0';
                        if(strcmp(groupname, tempname) == 0)
                        {
                            group_exists = 1;
                            break;
                        }
                    }
                }
                if(group_exists)
                    break;
            }
        free(line);
        fclose(fp);
    }

    /* Checks if the group exists in gshadow */
    if((fp = fopen(GSHADOW_FILE, "r")) == NULL)
    {
        /* Dont show anything */
    }
    else
    {
        fseek(fp, 0, SEEK_END);
        conf_size = ftell(fp);
        rewind(fp);
        line = allocate(conf_size);

        if(conf_size > 1)
            while(fgets(line, conf_size, fp) != NULL)
            {
                for(x = 0; line[x] != '\0'; x++)
                {
                    if(line[x] == ':')
                    {
                        snprintf(tempname, 4000, "%s", line);
                        tempname[x] = '\0';
                        if(strcmp(groupname, tempname) == 0)
//          if( strstr(groupname, tempname) 
//          && strlen(groupname) == strlen(tempname) )
                        {
                            group_exists = 1;
                            break;
                        }
                    }
                }
                if(group_exists)
                    break;
            }
        free(line);
        fclose(fp);
    }

    return group_exists;
}


gchar * get_group_name(gchar * groupnr)
{
    /* Gets the system group name from the system group number */
    FILE *fp;
    long conf_size;
    int x = 0, y = 0, found_group_number = 0;
    int count = 0, found_group_name = 0;
    char *line, *temp;
    gchar *retval = NULL;
    gint val_pos = 0;

    /* position of the group number (name is always first) */
    val_pos = 2;

    temp = allocate(4096);

    if((fp = fopen(GROUP_FILE, "r")) == NULL)
    {
        retval = g_strdup_printf(_("Group file not found"));
        free(temp);
        return retval;
    }
    fseek(fp, 0, SEEK_END);
    conf_size = ftell(fp);
    rewind(fp);

    line = allocate(conf_size + 1);

    if( conf_size > 1 )
        while(fgets(line, conf_size, fp) != NULL)
        {
            count = 0;

            for(x = 0; line[x] != '\0'; x++)
            {
                if(line[x] == ':')
                    count++;

                if(count == val_pos)
                    break;
            }

            snprintf(temp, 4000, "%s", &line[x + 1]);

            /* Snip an extra ":" at the end */
            for(y = strlen(temp) - 1; temp[y]; y--)
            {
                if(temp[y] == ':')
                {
                    temp[y] = '\0';
                    break;
                }
            }

            if(!strcmp(groupnr, temp))
            {
                found_group_number = 1;
                break;
            }

        }
    fclose(fp);


    count = 0;

    /* Get the name of this group */
    if( found_group_number )
        for(y = strlen(line) - 1; line[y] != '\0'; y--)
        {
            if(line[y] == ':' && x < 4000)
                count++;

            if( count == 3 )
            {
                found_group_name = 1;
                snprintf(temp, y + 1, "%s", line);
                break;
            }
        }

    free(line);

    /* g_free'd after the return */
    if( found_group_name )
    {
        retval = g_strdup_printf("%s", temp);
        free(temp);
        return retval;
    }

    free(temp);

    /* New default samba group if not found. */
    retval = g_strdup_printf("sambausers");

    return retval;
}


gchar * get_user_setting(gchar * user, gchar * what)
{
    /* Gets the users comment from passwd */
    FILE *fp;
    char *line;
    long conf_size;
    int x = 0, y = 0, found_user = 0, found_val = 0, count = 0;
    char *tempname;
    gchar *retval = NULL;
    gint val_pos = 0;

    if( strstr(what, "group") )
        val_pos = 3;

    if( strstr(what, "comment") )
        val_pos = 4;

    if( strstr(what, "homedir") )
        val_pos = 5;

    if( strstr(what, "shell") )
        val_pos = 6;


    tempname = allocate(4096);

    if((fp = fopen(PASSWD_FILE, "r")) == NULL)
    {
        retval = g_strdup_printf(_("User setting not found"));
        free(tempname);
        return retval;
    }
    else
    {
        fseek(fp, 0, SEEK_END);
        conf_size = ftell(fp);
        rewind(fp);
        line = allocate(conf_size);

        if( conf_size > 1 )
            while(fgets(line, conf_size, fp) != NULL)
            {
                for(x = 0; line[x] != '\0'; x++)
                {
                    if(line[x] == ':')
                    {
                        snprintf(tempname, 4000, "%s", line);
                        tempname[x] = '\0';
                        if(!strcmp(user, tempname))
                        {
                            found_user = 1;
                            break;
                        }
                    }
                }
                if( found_user )
                    break;
            }
    }

    fclose(fp);


    /* If the user is found, get the specified section */
    if( found_user )
        for(x = 0; line[x] != '\0'; x++)
        {
            if(line[x] == ':')
            {
                count++;
                /* The val_pos colon is found, beginning of the value to get */
                if(count == val_pos)
                {
                    snprintf(tempname, 4000, "%s", &line[x + 1]);
                    break;
                }
            }
        }

    free(line);

    /* Snip at value end, shell comes last and should be newline terminated */
    if( found_user )
        for(y = 0; tempname[y]; y++)
        {
            if((tempname[y] == ':' && y > 0) || (tempname[y] == '\n' && y > 0))
            {
                tempname[y] = '\0';

                /* Atleast one char in the value and max 350 */
                if(strlen(tempname) < 351 && !strstr(tempname, ":"))
                {
                    retval = g_strdup_printf("%s", tempname);
                    found_val = 1;
                    break;
                }
            }
        }

    free(tempname);

    /* g_free'd after the return */
    if(found_val)
        return retval;

    /* Return values if not found. Same as in
       add_change_user.c's clear_user_profile() */
    if( strstr(what, "group") )
        retval = g_strdup_printf("group1");
    else
    if( strstr(what, "comment") )
        retval = g_strdup_printf(_("Comment"));
    else
    if( strstr(what, "homedir") )
        retval = g_strdup_printf("/home/user1");
    else
    if( strstr(what, "shell") )
        retval = g_strdup_printf("/dev/null");
    else
        retval = g_strdup_printf(_("No value has been set"));

    return retval;
}



void password_user(const gchar * username, const gchar * password)
{
    FILE *fp;
    char *user_pass, *encrypted_pass;

    if( ! user_exists(username) )
        return;

    user_pass = allocate(8192);

    /* Crypt returns a pointer to a static buffer do not free it */
    encrypted_pass = encrypt_password(password);

    if( encrypted_pass == NULL )
    {
        /* Using unencrypted passwords */
        printf("Crypt error: using the chpass(wd) program instead\n");

#if defined USE_LINUX
        snprintf(user_pass, 8000, "echo %s:%s | chpasswd", username, password);
#endif


#if defined USE_OPENBSD || USE_NETBSD || USE_FREEBSD
        snprintf(user_pass, 8000, "echo %s:%s | chpass", username, password);
#endif


#if defined USE_DARWIN
        /* Someone who knows what can be used .. niutil lines ? */
        printf("Darwin functions are defined in osx_functions, this is a bug\n");
        snprintf(user_pass, 8000, "echo passwd/username %s:%s", username, password);
#endif

    }
    else
    {
        /* Using encrypted passwords */

#if defined USE_LINUX
        snprintf(user_pass, 8000, "usermod -p '%s' %s", encrypted_pass, username);
#endif

#if defined USE_OPENSBD || USE_NETBSD || USE_FREEBSD || USE_AIX || USE_HPUX
        snprintf(user_pass, 8000, "pw usermod -p '%s' %s", encrypted_pass, username);
#endif

#if defined USE_DARWIN
        snprintf(user_pass, 8000, "echo \"manually change the password: passwd/username '%s' %s\"",
            encrypted_pass, username);
#endif

    }

    /* Now we have the correct password changing string, change it */
    if((fp = popen(user_pass, "w")) == NULL)
    {
        perror("popen");
//  info = g_strdup_printf(_("Error changing password for user: %s\n"), username);
//      show_info(info);
//  g_free(info);
    }
    else
        pclose(fp);

#if defined USE_DARWIN
    printf("Send me a mail describing your systems way of changing a users password\n");
    printf("Then i can fix it.\n");
#endif

    free(user_pass);
}



char * encrypt_password(const gchar * password)
{
    /* Make an encrypted password using the MD5 algoritm */
    int i = 0, where = 0, randlen = 8;  /* The max MD5 random stringsize is 8 */
    char *rnd_string;
    char salt[13] = "";         /* The total salt length is 12 */

    /* 64 chars */
    char *arr[] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K",
        "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V",
        "W", "X", "Y", "Z", "a", "b", "c", "d", "e", "f", "g",
        "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r",
        "s", "t", "u", "v", "w", "x", "y", "z", "0", "1", "2",
        "3", "4", "5", "6", "7", "8", "9", ".", "/"
    };

    srand((unsigned)time(NULL));

    rnd_string = allocate(9);

    while(i < randlen)
    {
        where = rand() % 64;
        strcat(rnd_string, arr[where]);
        i++;
    }

#if defined HAVE_CRYPT
    /* For this to work it must have linked with -lcrypt */
    sprintf(salt, "$1$%s$", rnd_string);
#else
    /* We cant use MD5 salting and the salt can only be 2 chars */
    sprintf(salt, "%s", &rnd_string[6]);
#endif

    free(rnd_string);

    return crypt(password, salt);
}


void make_dir_chmod(gchar * directory, char perm[128])
{
    gchar *command;

    command = g_strdup_printf("mkdir -p '%s' && chmod %s '%s'", directory, perm, directory);
    if( ! run_command(command) )
        printf("Error running command: %s\n", command);

    g_free(command);
}


void make_file_chmod(gchar * file, char perm[128])
{
    gchar *command;

    command = g_strdup_printf("touch '%s' && chmod %s '%s'", file, perm, file);
    if( ! run_command(command) )
        printf("Error running command: %s\n", command);

    g_free(command);
}


int fix_newlines_in_conf()
{
    /* Strip all newlines between directives.
       Add newlines before any share "[" "]" */
    FILE *fp;
    char *old_buffer, *new_buffer;
    long conf_size, allocate_extra = 0;

    /* First remove any newlines first on all lines in the config.
       Check how many new chars we will be adding */
    if((fp = fopen(SAMBA_CONF, "r")) == NULL)
    {
        printf("Error reading samba configuration: %s\n", SAMBA_CONF);
        return 0;
    }
    fseek(fp, 0, SEEK_END);
    conf_size = ftell(fp);
    rewind(fp);

    old_buffer = allocate(conf_size + 1024);
    new_buffer = allocate(conf_size + 1024);

    if( conf_size > 1 )
        while(fgets(old_buffer, conf_size, fp) != NULL)
        {
            if(old_buffer[0] == '\n' || old_buffer[0] == '\r')
            {
                /* Dont collect these lines */
            }
            else
                strcat(new_buffer, old_buffer);

            if(strstr(old_buffer, "[") && strstr(old_buffer, "]"))
                allocate_extra++;
        }
    fclose(fp);
    free(old_buffer);
    strcat(new_buffer, "\n");

    /* Write the newline stripped conf */
    if((fp = fopen(SAMBA_CONF, "w+")) == NULL)
    {
        printf("Error writing newline stripped configuration: %s\n", SAMBA_CONF);
        free(new_buffer);
        return 0;
    }
    fputs(new_buffer, fp);
    fclose(fp);
    free(new_buffer);

    /* Add the newlines */
    if((fp = fopen(SAMBA_CONF, "r")) == NULL)
    {
        printf("Error reading newline stipped configuration: %s\n", SAMBA_CONF);
        return 0;
    }
    fseek(fp, 0, SEEK_END);
    conf_size = ftell(fp);
    rewind(fp);

    old_buffer = allocate(conf_size + 1024);
    new_buffer = allocate(conf_size + 1024 + allocate_extra);

    if( conf_size > 1 )
        while(fgets(old_buffer, conf_size, fp) != NULL)
        {
            if(strstr(old_buffer, "[") && strstr(old_buffer, "]"))
            {
                strcat(new_buffer, "\n");
                strcat(new_buffer, old_buffer);
            }
            else
                strcat(new_buffer, old_buffer);
        }
    fclose(fp);
    free(old_buffer);

    /* Write the newline added conf */
    if((fp = fopen(SAMBA_CONF, "w+")) == NULL)
    {
        printf("Error writing newline added configuration: %s\n", SAMBA_CONF);
        free(new_buffer);
        return 0;
    }
    fputs(new_buffer, fp);
    fclose(fp);
    free(new_buffer);

    return 1;
}


void fix_path(char *path)
{
    char *temp;
    temp = allocate(4096);

    /* Take away an unwanted star at the end */
    if( path[strlen(path) - 1] == '*' )
        path[strlen(path) - 1] = '\0';
    /* Take away an extra slash at the end */
    if( path[strlen(path) - 1] == '/' )
        path[strlen(path) - 1] = '\0';

    /* Make sure we have a slash at the beginning of the path */
    if( path[0] != '/' )
    {
        snprintf(temp, 1024, "/%s", path);
        snprintf(path, 1024, "%s", temp);
    }
    if( temp != NULL )
        free(temp);
}


void randomize_username(struct w *widgets)
{
    char *rnd_string;
    char *homedir_username;
    int len;
    gchar *info;
    gchar *utf8 = NULL;
//    const gchar *default_username_length;
    char *default_username_length;
    char *default_homedir;
//    const gchar *default_homedir;
//    default_username_length = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_spinbutton[7]));

// Fixme, get from settings
    default_username_length = allocate(100);
    snprintf(default_username_length, 10, "%s", MIN_PASS_LEN);


    if( chars_are_digits((char *)default_username_length) )
    {
        if( strlen((char *)default_username_length) < 4 )
            len = atoi((char *)default_username_length);
        else
        {
            info = g_strdup_printf(_("Max randomize username length is 999\n"));
            show_info(info);
            g_free(info);
            free(default_username_length);
            return;
        }
    }
    else
    {
        info = g_strdup_printf(_("Randomize username length can only contain digits\n"));
        show_info(info);
        g_free(info);
        free(default_username_length);
        return;
    }

    free(default_username_length);

    rnd_string = random_string(len);

    /* Insert the randomized username in the username entry */
    utf8 = g_locale_to_utf8(rnd_string, strlen(rnd_string), NULL, NULL, NULL);
    gtk_entry_set_text(GTK_ENTRY(widgets->user_set_entry[0]), utf8);
    if(utf8 != NULL)
        g_free(utf8);


    /* Insert the randomized username after the path in the homedir entry */
    homedir_username = allocate(16384);
    default_homedir = allocate(16384);

//    default_homedir = gtk_entry_get_text(GTK_ENTRY(widgets->user_set_entry[4]));
// Fixme.. get from gadmin-sambas settings
    snprintf(default_homedir, 8190, "%s", "/home");
    if( strlen((char *)default_homedir) > 8192 )
    {
        info = g_strdup_printf(_("Max home directory length is 8192 chars.\n"));
        show_info(info);
        g_free(info);
        free(homedir_username);
        return;
    }

    snprintf(homedir_username, 8192, "%s", default_homedir);

    free(default_homedir);

    /* Strip some slashes and things we dont want */
    if( homedir_username[strlen(homedir_username) - 1] == '/' )
        homedir_username[strlen(homedir_username) - 1] = '\0';

    if( homedir_username[strlen(homedir_username) - 1] == '/' )
        homedir_username[strlen(homedir_username) - 1] = '\0';

    if( homedir_username[strlen(homedir_username) - 1] == '*' )
        homedir_username[strlen(homedir_username) - 1] = '\0';

    /* Add a slash to the end then the username */
    strcat(homedir_username, "/");
    strcat(homedir_username, rnd_string);

    utf8 = g_locale_to_utf8(homedir_username, strlen(homedir_username), NULL, NULL, NULL);
    gtk_entry_set_text(GTK_ENTRY(widgets->user_set_entry[4]), utf8);
    if(utf8 != NULL)
        g_free(utf8);

    free(rnd_string);
    free(homedir_username);
}


void randomize_password(struct w *widgets)
{
    char *rnd_string;
    int len;
    gchar *info;
    gchar *utf8 = NULL;
    char *default_password_length;
//    const gchar *default_password_length;
//    default_password_length = gtk_entry_get_text(GTK_ENTRY(widgets->server_set_spinbutton[8]));

    // Should be user configurable in the settings dlg
    default_password_length = allocate(100);

    snprintf(default_password_length, 10, "%s", MIN_PASS_LEN);

    if( chars_are_digits(default_password_length) )
    {
        if( strlen(default_password_length) < 4 )
            len = atoi(default_password_length);
        else
        {
            info = g_strdup_printf(_("Max randomize password length is 999\n"));
            show_info(info);
            g_free(info);
            free(default_password_length);
            return;
        }
    }
    else
    {
        info = g_strdup_printf(_("Randomize password length can only contain digits\n"));
        show_info(info);
        g_free(info);
        free(default_password_length);
        return;
    }

    rnd_string = random_string(len);

    free(default_password_length);

    /* Insert the randomized username in the username entry */
    utf8 = g_locale_to_utf8(rnd_string, strlen(rnd_string), NULL, NULL, NULL);
    gtk_entry_set_text(GTK_ENTRY(widgets->user_set_entry[1]), utf8);
    if(utf8 != NULL)
        g_free(utf8);

    free(rnd_string);
}


char * random_string(int len)
{
    /* Returns a randomized string in upper or lower case 
     * depending on what is selected */
    char *rnd_string;
    gchar *info;
    int upper_case = 0;
    int where = 0;
    int i = 0;

    if( len > 256 )
    {
        info = g_strdup_printf(_("Randomize length too long, changed it to 256.\n"));
        show_info(info);
        g_free(info);
        len = 256;
    }

    /* 256 is the max allowed */
    rnd_string = allocate(300);

    /* Uppercase array 106 chars */
    char *u_arr[] = { "A", "B", "C", "D", "E", "F", "G", "H", "I",
        "J", "K", "L", "M", "N", "O", "P", "Q", "R",
        "S", "T", "U", "V", "W", "X", "Y", "Z",
        "a", "b", "c", "d", "e", "f", "g", "h", "i",
        "j", "k", "l", "m", "n", "o", "p", "q", "r",
        "s", "t", "u", "v", "w", "x", "y", "z",
        "1", "2", "3", "4", "5", "6", "7", "8", "9",
        "1", "2", "3", "4", "5", "6", "7", "8", "9",
        "1", "2", "3", "4", "5", "6", "7", "8", "9",
        "1", "2", "3", "4", "5", "6", "7", "8", "9",
        "1", "2", "3", "4", "5", "6", "7", "8", "9",
        "1", "2", "3", "4", "5", "6", "7", "8", "9"
    };

    /* Lowercase array 53 chars */
    char *l_arr[] = { "a", "b", "c", "d", "e", "f", "g", "h", "i",
        "j", "k", "l", "m", "n", "o", "p", "q", "r",
        "s", "t", "u", "v", "w", "x", "y", "z",
        "1", "2", "3", "4", "5", "6", "7", "8", "9",
        "1", "2", "3", "4", "5", "6", "7", "8", "9",
        "1", "2", "3", "4", "5", "6", "7", "8", "9"
    };


    /* Give "deep thought" some time to come up with the number "42" :) */
    usleep(300000);

    /* Make the first random nondigit */
    srand((unsigned)time(NULL));
    while( 1 )
    {
        if( upper_case )
        {
            where = rand() % 106;

            if( ! chars_are_digits(u_arr[where]) )
            {
                strcpy(rnd_string, u_arr[where]);
                break;
            }
        }

        if( ! upper_case )
        {
            where = rand() % 53;

            if( ! chars_are_digits(l_arr[where]) )
            {
                strcpy(rnd_string, l_arr[where]);
                break;
            }
        }
    }

    where = 0;
    i = 1;

    /* Give "deep thought" some time to come up with the number "42" :) */
    usleep(300000);
    srand((unsigned)time(NULL));

    while( i < len )
    {
        if( upper_case )
        {
            where = rand() % 106;
            strcat(rnd_string, u_arr[where]);
        }
        if( ! upper_case )
        {
            where = rand() % 53;
            strcat(rnd_string, l_arr[where]);
        }
        i++;
    }

    return rnd_string;
}
