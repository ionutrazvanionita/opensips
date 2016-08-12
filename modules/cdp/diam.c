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
#include "diam.h"
#include "diameter_peer.h"

#include "../../ut.h"

/*
 * FIXME we should do here what we do in mod init
 * The problem: how to start processes?? the whole module
 * uses globally declared config file handler; how to use
 * our own config???
 *
 * FIX??
 *
 */
aaa_conn* diam_init_prot(str* aaa_url)
{

	return NULL;
}


/**
 * FIXME testing purposes only
 *
 */
int createAndAddAVPToMsg( AAA_AVPCode code, char* data, int length,
		AAAMessage* m, AAA_AVP* position)
{

	AAA_AVP* avp;

	avp=AAACreateAVP( code, 0, 0, data, length, AVP_DUPLICATE_DATA );

	AAAAddAVPToMessage(m, avp, position);

	return 0;
}

aaa_message* diam_create_message(aaa_conn* dh, int flag)
{
	aaa_message* message;

	unsigned int id;

	static str alice_s = str_init("alice");
	static str realm_s = str_init("open_ims.test");

	if (!dh) {
		LM_ERR("invalid aaa connection argument\n");
		return NULL;
	}

	if (flag != AAA_AUTH && flag != AAA_ACCT) {
		LM_ERR("invalid flag\n");
		return NULL;
	}

	message = (aaa_message*) pkg_malloc (sizeof(aaa_message));

	if (!message) {
		LM_ERR("no pkg memory left \n");
		return NULL;
	}

	message->session = AAACreateSession(0);
	if (message->session == NULL) {
		LM_ERR("failed to create diameter session!\n");
		return NULL;
	}

	message->type = flag;
	message->avpair = message->last_found = NULL;

	if( flag == AAA_ACCT) {
		message->proto_message=AAACreateRequest(0, ACCT_REQUEST_CODE,
				Flag_Request|Flag_Proxyable, message->session);

		if (message->proto_message == NULL) {
			LM_ERR("failed to create diameter request!\n");
			return NULL;
		}

		AAASessionsUnlock(((AAASession *)message->session)->hash);

		AAASendMessageToPeer(message->proto_message, &alice_s, 0, 0);
	} else {
		message->proto_message=AAACreateRequest(1, AA_REQUEST_CODE,
				Flag_Request, message->session);

		if (message->proto_message == NULL) {
			LM_ERR("failed to create diameter request!\n");
			return NULL;
		}

		AAASessionsUnlock(((AAASession *)message->session)->hash);

		id=1;
		createAndAddAVPToMsg(AVP_Auth_Application_Id, (char *)&id, sizeof(id),
				message->proto_message,
				((AAAMessage *)message->proto_message)->avpList.tail);

		createAndAddAVPToMsg( AVP_Destination_Realm, realm_s.s, realm_s.len,
				message->proto_message,
				((AAAMessage *)message->proto_message)->avpList.tail);

		createAndAddAVPToMsg( AVP_Destination_Host, alice_s.s, alice_s.len,
				message->proto_message,
				((AAAMessage *)message->proto_message)->avpList.tail);


		id = AUTHENTICATE_ONLY;
		createAndAddAVPToMsg(AVP_Auth_Request_Type, (char *)&id, sizeof(id),
				message->proto_message,
				((AAAMessage *)message->proto_message)->avpList.tail);




		AAASendMessageToPeer(message->proto_message, &alice_s, 0, 0);

	}

	return NULL;
}
