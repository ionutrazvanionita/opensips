/**
 * $Id: mod.c 920 2010-03-05 19:07:50Z vingarzan $
 *
 * Copyright (C) 2004-2006 FhG Fokus
 *
 * This file is part of Open IMS Core - an open source IMS CSCFs & HSS
 * implementation
 *
 * Open IMS Core is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * For a license to use the Open IMS Core software under conditions
 * other than those described here, or to purchase support for this
 * software, please contact Fraunhofer FOKUS by e-mail at the following
 * addresses:
 *     info@open-ims.org
 *
 * Open IMS Core is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * It has to be noted that this Open Source IMS Core System is not
 * intended to become or act as a product in a commercial context! Its
 * sole purpose is to provide an IMS core reference implementation for
 * IMS technology testing and IMS application prototyping for research
 * purposes, typically performed in IMS test-beds.
 *
 * Users of the Open Source IMS Core System have to be aware that IMS
 * technology may be subject of patents and licence terms, as being
 * specified within the various IMS-related IETF, ITU-T, ETSI, and 3GPP
 * standards. Thus all Open IMS Core users have to take notice of this
 * fact and have to agree to check out carefully before installing,
 * using and extending the Open Source IMS Core System, if related
 * patents and licences may become applicable to the intended usage
 * context.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/**
 * \file
 *
 * CDiameterPeer SER module interface and definitions.
 *
 *
 *  \author Dragos Vingarzan vingarzan -at- fokus dot fraunhofer dot de
 *
 */

#include "mod.h"
#include "diam.h"

#include "diameter_peer.h"
#include "config.h"
#include "cdp_load.h"
#include "receiver.h"
#include "acceptor.h"
#include "timer.h"

#include "../../ut.h"

#define CDP_WORKER_NAME "cdp_worker"
#define CDP_WORKER_NAME_LEN (sizeof(CDP_WORKER_NAME) - 1)


#define CDP_RECEIVER_NAME "cdp_receiver"
#define CDP_RECEIVER_NAME_LEN (sizeof(CDP_WORKER_NAME) - 1)

#define CDP_ACCEPTOR_NAME "cdp_acceptor"

#define CDP_TIMER_NAME "cdp_timer"

char* config_file="DiameterPeer.xml"; 	/**< default DiameterPeer configuration filename */

extern dp_config *config; 				/**< DiameterPeer configuration structure */
static int w_diameter_auth(struct sip_msg* msg);


#define EXP_FUNC(NAME) \
		{#NAME, (cmd_function)NAME, NO_SCRIPT, 0, 0},
/**
 * Exported functions. This is the API available for use from other SER modules.
 * If you require more, please add them here.
 * <p>
 * - load_cdp() - find and load the CDiameterPeer function bindings
 * <p>
 * - AAACreateRequest() - create a diameter request #AAAMessage
 * - AAACreateResponse() - create a diameter response #AAAMessage
 * - AAAFreeMessage() - free up the memory used in a Diameter message
 * <p>
 * - AAASendMessage() - asynchronously send a message
 * - AAASendMessageToPeer() - asynchronously send a message to a forced peer
 * - AAASendRecvMessage() - synchronously send a message and get the response
 * - AAASendRecvMessageToPeer() - synchronously send a message and get the response to a forced peer
 * <p>
 * - AAACreateSession() - create a diameter #AAASessionId
 * - AAADropSession() - drop a diameter #AAASessionId
 * <p>
 * - AAACreateTransaction() - create a diameter #AAATransaction
 * - AAADropTransaction() - drop a diameter #AAATransaction
 * <p>
 * - AAACreateAVP() - create an #AAA_AVP
 * - AAAAddAVPToMessage() - add an #AAA_AVP to a #AAAMessage
 * - AAAFindMatchingAVP() - find an #AAA_AVP inside a #AAAMessage
 * - AAAGetNextAVP() - get the next #AAA_AVP from the #AAAMessage
 * - AAAFreeAVP() - free the memory taken by the #AAA_AVP
 * - AAAGroupAVPS() - group a #AAA_AVP_LIST of #AAA_AVP into a grouped #AAA_AVP
 * - AAAUngroupAVPS() - ungroup a grouped #AAA_AVP into a #AAA_AVP_LIST of #AAA_AVP
 * - AAAFindMatchingAVPList() - find an #AAA_AVP inside a #AAA_AVP_LIST
 * - AAAFreeAVPList() - free the memory taken by the all members of #AAA_AVP_LIST
 * <p>
 * - AAAAddRequestHandler() - add a #AAARequestHandler_f callback to request being received
 * - AAAAddResponseHandler() - add a #AAAResponseHandler_f callback to responses being received
 */

static cmd_export_t cdp_cmds[] = {
	{"diameter_auth", (cmd_function)w_diameter_auth, 0, 0, 0,
		REQUEST_ROUTE|FAILURE_ROUTE|BRANCH_ROUTE|LOCAL_ROUTE},
#if 0
	{"load_cdp",					(cmd_function)load_cdp, 				NO_SCRIPT, 0, 0},

	EXP_FUNC(AAACreateRequest)
	EXP_FUNC(AAACreateResponse)
	EXP_FUNC(AAAFreeMessage)


	EXP_FUNC(AAACreateAVP)
	EXP_FUNC(AAAAddAVPToMessage)
	EXP_FUNC(AAAAddAVPToList)
	EXP_FUNC(AAAFindMatchingAVP)
	EXP_FUNC(AAAFindMatchingAVPList)
	EXP_FUNC(AAAGetNextAVP)
	EXP_FUNC(AAAFreeAVP)
	EXP_FUNC(AAAFreeAVPList)
	EXP_FUNC(AAAGroupAVPS)
	EXP_FUNC(AAAUngroupAVPS)

	EXP_FUNC(AAASendMessage)
	EXP_FUNC(AAASendMessageToPeer)
	EXP_FUNC(AAASendRecvMessage)
	EXP_FUNC(AAASendRecvMessageToPeer)

	EXP_FUNC(AAAAddRequestHandler)
	EXP_FUNC(AAAAddResponseHandler)


	EXP_FUNC(AAACreateTransaction)
	EXP_FUNC(AAADropTransaction)


	EXP_FUNC(AAACreateSession)
	EXP_FUNC(AAAMakeSession)
	EXP_FUNC(AAAGetSession)
	EXP_FUNC(AAADropSession)
	EXP_FUNC(AAASessionsLock)
	EXP_FUNC(AAASessionsUnlock)

	EXP_FUNC(AAACreateClientAuthSession)
	EXP_FUNC(AAACreateServerAuthSession)
	EXP_FUNC(AAAGetAuthSession)
	EXP_FUNC(AAADropAuthSession)
	EXP_FUNC(AAATerminateAuthSession)
#endif
	{ 0, 0, 0, 0, 0, 0 }
};

/**
 * Exported parameters.
 * - config_file - Configuration filename. See configdtd.h for the structure and ConfigExample.xml.
 */
static param_export_t cdp_params[] = {
	{ "config_file",STR_PARAM,&config_file}, /**< configuration filename */
	{ 0, 0, 0 }
};



static proc_export_t cdp_procs[] = {
	{CDP_WORKER_NAME, 0, 0, worker_process, 1, 0},
	{CDP_RECEIVER_NAME, diameter_receiver_init, 0, receiver_process_w, 1, 0},
	{CDP_ACCEPTOR_NAME, 0, 0, acceptor_process_w, 1, 0},
	{CDP_TIMER_NAME, 0, 0, timer_process, 1, 0},
	{NULL, 0, 0, NULL, 0, 0}
};
/**
 * Exported module interface
 */
struct module_exports exports = {
	"cdp",
	MOD_TYPE_DEFAULT,/* class of this module */
	MODULE_VERSION,  /* module version */
	DEFAULT_DLFLAGS, /* dlopen flags */
	0,               /* OpenSIPS module dependencies */
	cdp_cmds,        /**< Exported functions */
	0,               /* exported async functions */
	cdp_params,      /* exported params */
	0,               /* exported statistics */
	0,               /* exported MI functions */
	0,               /* exported pseudo-variables */
	cdp_procs,       /* extra processes */
	cdp_init,        /* initialization module */
	0,               /* response function */
	cdp_exit,        /* destroy function */
	0                /* per-child init function */
};




/**
 * Module init function.
 *
 * - Initializes the diameter peer using the provided configuration file.
 * - Registers with pt the required number of processes.
 */
static int cdp_init( void )
{
	int i;

	LM_INFO("CDiameterPeer initializing\n");
	if (!diameter_peer_init(config_file)){
		LM_CRIT("error initializing the diameter peer\n");
		return 1;
	}

	for (i=0; cdp_procs[i].name != NULL; i++) {
		/* instruct opensips to start the request number of worker processes
		 * that was read from config file */
		if (!strncmp(cdp_procs[i].name, CDP_WORKER_NAME, CDP_WORKER_NAME_LEN)) {
			cdp_procs[i].no = config->workers;
		}

		if (!strncmp(cdp_procs[i].name, CDP_RECEIVER_NAME, CDP_RECEIVER_NAME_LEN)) {
			cdp_procs[i].no = config->peers_cnt + 1/*unknown peer receiver process*/;
		}
	}



#if 0
	register_procs(2+config->workers + 2 * config->peers_cnt);
#endif
	return 0;
}

/**
 *	Module termination function.
 * - stop the DiameterPeer processes in a civilized manner
 */
static void cdp_exit( void )
{
	LM_INFO("CDiameterPeer stoping ...\n");
	diameter_peer_destroy();
	LM_INFO("... CDiameterPeer stoped\n");
}

void aaaSesCb(int event, void* session)
{
	switch (event) {
		case AUTH_EV_SESSION_TIMEOUT:

			LM_INFO("sess to!\n");
			break;
		case AUTH_EV_SESSION_GRACE_TIMEOUT:

			LM_INFO("sess grace to!\n");
			break;
		case AUTH_EV_SERVICE_TERMINATED:

			LM_INFO("sess terminated!\n");
			break;
		case AUTH_EV_SESSION_DROP:

			LM_INFO("sess drop!\n");
			break;
		case AUTH_EV_SESSION_CREATED:

			LM_INFO("auth session successfully created!\n");
			break;
		default:

			LM_INFO("unknown %d!\n", 23);
			break;
	}
}

void tranCb(int is_timeout, void* param, AAAMessage *m)
{
	LM_INFO("SHIT!\n");
}


static int w_diameter_auth(struct sip_msg* msg)
{
	aaa_message* message;

	diam_create_message(config, AAA_ACCT);

	return 1;
}

