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
#include "allocate.h"
#include "widgets.h"
#include "populate_gadmin_samba.h"
#include "populate_server_settings.h"
#include "populate_users.h"
#include "select_first_user.h"
#include "populate_user_settings.h"
#include "populate_shares.h"
#include "populate_share_settings.h"
#include "populate_maccts.h"
#include "select_first_macct.h"
#include "populate_macct_settings.h"

#include "populate_disc_tab.h"
#include "populate_security_tab.h"
#include "populate_conf_tab.h"



void populate_gadmin_samba(struct w *widgets)
{
    /* Populate the server settings */
    populate_server_settings(widgets);

    /* Populate the user tab */
    populate_users(widgets);
    /* Set the first user as selected */
    select_first_user(widgets);
    /* Populate the user settings */
    populate_user_settings(widgets);


    /* Populate the share tab */
    populate_shares(widgets);
    /* Populate the share settings */
    populate_share_settings(widgets);


    /* Populate the machine account tab */
    populate_maccts(widgets);
    /* Set the first machine account as selected */
    select_first_macct(widgets);
    /* Populate the machine account settings */
    populate_macct_settings(widgets);


    /* Populate the disc tab */
    populate_discs(widgets);

    /* Populate the security tab */
    populate_security(widgets);

    /* Populate the conf tab */
    populate_conf_tab(widgets);
}
