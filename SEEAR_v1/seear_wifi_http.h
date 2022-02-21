/*****************************************************************
* Title			:	seear_wifi_http
* Filename		:	seear_wifi_http.h
* Author		:	Sijeo Philip
* Origin Date		:	1st Oct 2021
* Version		:	1.0.0
* Compiler		:	gcc
* Target		: 	QCA4010 (SX-ULGPN, Silex)
* Notes			:	This Module is used to do the HTTP/
*				HTTPS Transactions
* THIS SOFTWARE IS PROVIDED BY UNISEM ELECTRONICS "AS IS" AND 
* ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
* TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL UNISEM
* ELECTRONICS OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
* USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
* AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE. 
*********************************************************************/

/*******************INTERFACE CHANGE LIST****************************
*
*   DATE	VERSION		AUTHOR		DESCRIPTION
*  01/10/2021   1.0.0	     Sijeo Philip	Interface Created
*********************************************************************/

/********************************************************************
* @file seear_http.h
* @brief This module is used as interface to do http/https transaction
*	 of data over wifi connection 
*********************************************************************/

#ifndef _SEEAR_WIFI_HTTP_H_
#define _SEEAR_WIFI_HTTP_H_



/*********************************************************************
*			Includes
**********************************************************************/
#include "qcom_common.h"
#include "socket_api.h"
#include "qcom_event.h"
#include "threadx/tx_api.h"
#include "threadxdmn_api.h"
#include <ctype.h>
#include "malloc_api.h"
#include "qcom_ssl.h"
#include "qcom_network.h"

/*********************************************************************
*		        Preprocessor Constants
***********************************************************************/
#define SX_WIFI_STATE_DISCONNECTED	0
#define SX_WIFI_STATE_CONNECTING	1
#define SX_WIFI_STATE_CONNECTED		2
#define SX_WIFI_STATE_CONNECT_FAILED	3
#define SX_WIFI_STATE_RECONNECTING	4
#define SX_WIFI_STATE_DISCONNECTING	5

/*********************************************************************
*		 	Typedefs
**********************************************************************/
/*typedef struct httpc_response{
	A_UINT32 length;
	A_UINT32 resp_code;
	A_UINT32 more;
	A_UINT8 data[1];
}HTTPC_RESPONSE_t;*/

typedef struct http_rsp_cont {
	struct http_rsp_cont * next;
	A_UINT32 flag;
	A_UINT32 totalLen;
	A_UINT32 length;
	A_UINT8 data[1];
}HTTP_RSP_CONT;
	
/*********************************************************************
*			Function Prototypes
**********************************************************************/

int connect_device(char* passphrase, char* ssid, A_UINT8 wpa_wpa2, A_UINT8 aes_tkip);
int disconnect_device(void);
int ipconfig_device(int mode, char *pAddress, char *pNetmask, char *pGateway);
int ping_connection(char* address, int ping_size);
void show_wifi_status(A_UINT8 device_id);
int create_https_context_client(void);
int connect_http(char* pHostname, char* pPort);
int connect_https(char* pHostname, char* pPort);
A_STATUS disconnect_http(void);

int client_http_get(char* pMessage, const char *url_ptr);
int client_http_post(char* pMessage, const char *url_ptr);



#endif

