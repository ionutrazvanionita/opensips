/* *
 * Copyright (C) 2014 OpenSIPS Foundation
 *
 * This file is part of opensips, a free SIP server.
 *
 * opensips is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version
 *
 * opensips is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 * History
 * -------
 *  2016-07-29  initial version (Ionut Ionita)
 * */

#include "../../aaa/aaa.h"

#define AA_REQUEST_CODE 265
#define ACCT_REQUEST_CODE 271


typedef enum
{
AUTHENTICATE_ONLY = 1, AUTHORIZE_ONLY = 2, AUTHORIZE_AUTHENTICATE = 3
} auth_request;

aaa_conn* diam_init_prot(str* aaa_url);
aaa_message* diam_create_message(aaa_conn* rh, int flag);

