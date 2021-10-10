/*****************************************************************
* Title			:	seear_wifi_http
* Filename		:	seear_wifi_http.c
* Author		:	Sijeo Philip
* Origin Date		:	4th Oct 2021
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

/***********************SOURCE REVISION LOG **************************
*
*	Date	Version		Author		Description
*    04/10/2021  1.0.0	      Sijeo Philip	Initial Release
*
**********************************************************************/

/**********************************************************************
* @file: seear_wifi_http.c
* @brief : This is the source file for connecting to the WiFi network
* and executing GET, POST over HTTP, HTTPS
***********************************************************************/

/**********************************************************************
*		INCLUDES
***********************************************************************/
#include "seear_wifi_http.h"

/**********************************************************************
*		MODULE PREPROCESSOR MACROS
***********************************************************************/

/**********************************************************************
*		MODULE TYPEDEFS	
***********************************************************************/

/**********************************************************************
*		MODULE VARIABLE DEFINITIONS
***********************************************************************/
SSL_CTX *httpc_sslctx = NULL;
A_UINT8 wifi_stat_state = SX_WIFI_STATE_DISCONNECTED;
A_UINT8 wifi_sta_device_id = 0;
/**********************************************************************
*		FUNCTION PROTOTYPES	
***********************************************************************/
static int gf_SockaddrToString(const struct sockaddr_storage *sa, char *str, int *p_strlen);
static const char *gf_StringToSockaddr(const char *str, struct sockaddr_storage *sa, int *p_salen);
static int client_http_action(A_UINT32 http_cmd, const char *url_ptr);


/*************************************************************************
*		CALLBACKS
**************************************************************************/
void wifi_sta_connect_callback(A_UINT8 device_id, A_INT32 value)
{
	QCOM_WIFI_CALLBACK_CODE cbValue = value;
	if (device_id != wifi_sta_device_id ) {
	   return;
	}

	printf("STA Callback stat =%d, event=%d\n", wif_sta_state, value);
	switch (wifi_sta_state) {
	case SX_WIFI_STATE_CONNECTING:
	   if ( cbValue == QCOM_NOT_CONNECTED || cbValue == QCOM_RSNA_AUTH_FAIL) {
		wifi_sta_state = SX_WIFI_CONNECT_FAILED;	
		break;
	    }
	case SX_WIFI_STATE_RECONNECTING:
	   if ( ((wifi_sta_auth_mode == WLAN_AUTH_NONE) && 
		 (cbValue == QCOM_CONNECTED_OPEN_WEP_AUTH)) ||
		( cbValue == QCOM_RSNA_AUTH_SUCCESS )) {
	        wifi_sta_state = SX_WIFI_STATE_CONNECTED;
	    }
	    break;
        case SX_WIFI_STATE_CONNECTED:
	   if ( cbValue == QCOM_NOT_CONNECTED ) {
		wifi_sta_state = SX_WIFI_STATE_RECONNECTING;
	   }
	   break;
        case SX_WIFI_STATE_DISCONNECTING:
	   if ( cbValue == QCOM_NOT_CONNECTED ) {
	      wifi_sta_state = SX_WIFI_STATE_DISCONNECTED;
	      /* Stop callback when disconnect is completed */
	      qcom_set_connect_callback(wifi_sta_device_id, NULL);
            }
	    break;
 

	}


}

/*************************************************************************
*		FUNCTION DEFINITIONS
*************************************************************************/

/*********************************************************************************************
* Function : gf_SockaddrToString()
*\b Description:
*
* This function is used to convert Socket IP Address to string for further
* Processing or Storage
*
* PRE-CONDITION: The system is initalized for all the peripherals and IOs
*
* POST-CONDITION: The socket address provided is converted to string 
* @param[in]		sa		sockadd_in or sockaddr_in6
* @param[in, out]	str		pointer to store converted string	
* @param[in, out]	p_strlen	size of str (unlimited if NULL)
*
* @return	 		 	length stored in str
*
*\b Example:
* @code 
*
*
* @endcode
*
* @see seear_wifi_http.h
*
*
*<br><b>- HISTORY OF CHANGES -</b>
*
* <table align="left" style="width:800px">
* <tr><td> Date </td><td> Software Version</td><td> Initials</td><td>Description</td><tr>
* <tr><td> 04/10/2021 </td> 0.5.0	  </td><td> SP	    </td><td>Interface Created</td></tr>
* </table><br><br>
* <hr>
*
************************************************************************************************/
static int gf_SockaddrToString(const struct sockaddr_storage *sa, char *str, int *p_strlen)
{
	int ret = 0;
	int len = (p_strlen != NULL) ? *p_strlen : 65535;
	unsigned long inaddr;

	*str = 0;
	inaddr = (u_long)(((struct sockaddr_in*)sa)->sin_addr.s_addr);
	inaddr = ntohl(inaddr);

	if (len >= 16)
	{
		ret = sprintf(str, "%d.%d.%d.%d",
		      (int)(inaddr >> 24) & 0xff,
                      (int)(inaddr >> 16) & 0xff,
		      (int)(inaddr >> 8) & 0xff, 
                      (int)(inaddr     ) & 0xff);
                str[ret] = 0;
    	}
	ret = strlen(str);
	if ( NULL != p_strlen ){
	    *p_strlen = len;
	}

	return ret;
}

/*********************************************************************************************
* Function : gf_StringToSockaddr()
*\b Description:
*
* This function is used to convert String to SockAddr IP fromat for further
* Processing or Storage
*
* PRE-CONDITION: The system is initalized for all the peripherals and IOs
*
* POST-CONDITION: The string provided is converted to SockAddr
* @param[in]		str		Pointer to string
* @param[in, out]	sa		sockaddr_in or sockaddr_in6
* @param[in, out]	p_salen		size of sa
*
* @return	 		 	end of parsed string
*
*\b Example:
* @code 
*
*
* @endcode
*
* @see seear_wifi_http.h
*
*
*<br><b>- HISTORY OF CHANGES -</b>
*
* <table align="left" style="width:800px">
* <tr><td> Date </td><td> Software Version</td><td> Initials</td><td>Description</td><tr>
* <tr><td> 04/10/2021 </td> 0.5.0	  </td><td> SP	    </td><td>Interface Created</td></tr>
* </table><br><br>
* <hr>
*
************************************************************************************************/
static const char *gf_StringToSockaddr(const char *str, struct sockaddr_storage *sa, int *p_salen)
{

	unsigned long inaddr = 0;
	int i;
	const char	*p = str;

	memset(sa, 0, *p_salen);

	if( *p_salen >= sizeof(struct sockaddr_in) ){
	    *p_salen = sizeof(struct sockaddr_in);
	    (((struct sockaddr*)(sa))->sa_family) = AF_INET;

	    for (i = 0; i < 4; i++)
            {
		inaddr <<= 8;
		inaddr |= atoi(p);
		while ( (((int)*p) >= '0') && (((int)*p) <= '9') ) {
		 p++;
		}
		if (*p == '.'){
		   p++;
		}
		else {
		   break;
		}
            }/*end of for loop */
	   ((struct sockaddr_in*)sa)->sin_addr.s_addr = htonl(inaddr);
	} /*end of main if */

return p;
} /*end of function */

/*********************************************************************************************
* Function : client_http_action()
*\b Description:
*
* This function is used to execute the http action such as POST/GET
*
* PRE-CONDITION: The system is initalized for all the peripherals and IOs
*	         and connected to the Network
*
* POST-CONDITION: HTTP POST/GET action is initiated
* @param[in]		http_cmd	POST OR GET Command is provided
* @param[in]		*url_ptr	Address of the URL is provided for HTTP
*
* @return	 	0 for success	
*
*\b Example:
* @code 
*
*
* @endcode
*
* @see seear_wifi_http.h
*
*
*<br><b>- HISTORY OF CHANGES -</b>
*
* <table align="left" style="width:800px">
* <tr><td> Date </td><td> Software Version</td><td> Initials</td><td>Description</td><tr>
* <tr><td> 04/10/2021 </td> 0.5.0	  </td><td> SP	    </td><td>Interface Created</td></tr>
* </table><br><br>
* <hr>
*
************************************************************************************************/
static int client_http_action(A_UINT32 http_cmd, const char *url_ptr)
{
	HTTPC_RESPONSE_t *httpc_response;
	A_UINT8 *httpc_respbuff;
	HTTP_RSP_CONT_t* httpc_rspcnt;
	HTTP_RSP_CONT_t* httpc_rsphead;
	A_UINT32 httpc_content_size;
        A_STATUS qret;
	int num_buffers;
	int len;
	char *httpc_tmpbuff;
	
	httpc_respbuff = qcom_mem_alloc(1500);
	if ( NULL == httpc_respbuff ){
	    return -1;
	}
	
	httpc_tmpbuff = qcom_mem_alloc(1500);
	if ( NULL == httpc_tmpbuff ){
           qcom_mem_free(respbuff);
	   return -1;
	}

	memset( httpc_respbuff, 0, sizeof(httpc_respbuff));
	httpc_response = (HTTPC_RESPONSE_t*) httpc_respbuff;

	qret = qcom_http_client_method(
	       http_cmd,
	       (A_UINT8*)url_ptr, 
	       (A_UINT8*)NULL,
	       (A_UINT8*)httpc_response,
		httpc_sslctx);
	printf("HTTP action cmd = %d ret=%d\n", http_cmd, qret);
	if ( qret != A_OK ){
	    qcom_mem_free(httc_respbuff);
	    qcom_mem_free(httpc_tmpbuff);
	    return 0;
         }

	printf("HTTP response code=%u len=%d more=%d buff=0x%x\n",
	        httpc_response->resp_code,
                httpc_response->length,
	        httpc_response->more,
		*( (A_UINT32*)(&httpc_response->data[0]) ) );
       httpc_rsphead = (HTTP_RSP_CONT**)*((A_UINT32*)(&httpc_response->data[0]) );

	/* Calculate total length of the returned content */
	 num_buffers = 0;
	 if ( httpc_response->more == 0 ){
 	    httpc_content_size = httpc_response->length;
  	 }
	 else {
	      httpc_content_size = 0;
	      httpc_rspcnt = *httpc_rsphead;
              while ( httpc_rspcnt != NULL ) {
		num_buffers++;
	 	if ( httpc_rspcnt->data ) {
		   httpc_content_size += httpc_rspcnt->length;
		}
	      }
	  }

	  printf("HTTP response size=%u(%d buffers)\n", httpc_content_size, num_buffers);
	  /*Display returned conten */

	 if( httpc_response->more == 0 ){
    	    len = httpc_response->length;
	    if ( len >= 1500 ){
	     	len = (1500-1);
	    }
	    memcpy(httpc_tmpbuff, httpc_response->data, len);
	    httpc_tmpbuff[len] = 0;
	    printf("%s", httpc_tmpbuff);
	    tx_thread_relinquish();
          }
	  else {
	       httpc_rspcnt = *httpc_rsphead;
	       while( httpc_rspcnt != NULL ) {
	        HTTP_RSP_CONT_t *prev;
	 	prev = httpc_rspcnt;
		if ( httpc_rspcnt->data ) {
		   len = httpc_rspcnt->length;
		   if ( len >= 1500 ) {
			len = ( 1500 -1 );
		   }
		   memcpy(httpc_tmpbuff, httpc_rspcnt->data, len);
		   httpc_tmpbuff[len] = 0;
	   	   printf("%s", httpc_tmpbuff );
		   tx_thread_relinquish();
                 }
		 httpc_rspcnt = httpc_rspcnt->next;
	    	 free(prev);
                }
                *httpc_rsphead = NULL;
              }
	     qcom_mem_free(httpc_respbuff);
	     qcom_mem_free(httpc_tmpbuff);
	     return 0;
}

/*********************************************************************************************
* Function : ipconfig_device()
*\b Description:
*
* This function is used to configure the ip of the wifi device in DHCP or 
* Manual mode.
*
* PRE-CONDITION: The system is initalized for all the peripherals and IOs
*		 and should be connected to WiFi
*
* POST-CONDITION: The system will be configured with IP for further use with the 
*	          Network
* @param[in]		mode		Mode of the IP config, 
*					0 - IP_CONFIG_QUERY
*					1 - IP_CONFIG_SET (STATIC IP)
*					2 - IP_CONFIG_DHCP (DHCP)
* @param[in, out]	address		IP address of the device
* @param[in, out]	netmask		NetMask of the device
* @param[in, out]	gateway		Gateway IP of the device
*
* @return	 		 	0
*
*\b Example:
* @code 
*
*
* @endcode
*
* @see seear_wifi_http.h
*
*
*<br><b>- HISTORY OF CHANGES -</b>
*
* <table align="left" style="width:800px">
* <tr><td> Date </td><td> Software Version</td><td> Initials</td><td>Description</td><tr>
* <tr><td> 08/10/2021 </td> 0.5.0	  </td><td> SP	    </td><td>Interface Created</td></tr>
* </table><br><br>
* <hr>
*
************************************************************************************************/

int ipconfig_device(int mode, char *pAddress, char *pNetmask, char *pGateway)
{
	A_UINT32 address, netmask, gateway;
	struct sockaddr_storage	ss;
	char buff[20];
	int len;
	int mode;


	/*Query for the IP Data */
	qcom_ipconfig( wifi_sta_device_id, 0, &address, &netmask, &gateway);
	if ((mode != 1) || (mode !=2) )
	{
	   ((struct sockaddr_in*)&ss)->sin_addr.s_addr = htonl(address);
	   len = sizeof(buff);
	   gf_SockaddrToString(&ss, buff, &len);
	   printf("IP:%s\n", buff);
	   ((struct sockaddr_in*)&ss)->sin_addr.s_addr = htonl(netmask);
	   len = sizeof(buff);
	   gf_SockaddrToString(&ss, buff, &len);
	   printf("MASK:%s\n", buff);
	   ((struct sockaddr_in*)&ss)->sin_addr.s_addr = htonl(gateway);
	   len = sizeof(buff);
	   gf_SockaddrToString(&ss, buff, &len);
	   printf("GATEWAY:%s\n", buff);
	   return 0;
	}

	if (mode != 2) /*Set Fixed IP*/
	{
	  len = sizeof(ss);
	  gf_StringToSockaddr(pAddress, &ss, &len);
	  address = ntohl(((struct sockaddr_in*)&ss)->sin_addr.s_addr);
	  printf("Assigned fixed address\n);
	}

	if (pNetmask != NULL)
	{
	  len = sizeof(ss);
	  gf_StringToSockaddr(pNetmask, &ss, &len);
	  netmask = ntohl(((struct sockaddr_in*)&ss)->sin_addr.s_addr);
	}
	
	if (pGateway != NULL )
	{
	  len = sizeof(ss);
	  gf_StringToSockaddr(pGateway, &ss, &len);
	  gateway = ntohl(((struct sockaddr_in*)
	}
	
	qcom_ipconfig(wifi_sta_device_id, mode, &address, &netmask, &gateway);
	return 0;

}


/*********************************************************************************************
* Function : connect_device();
*\b Description:
*
* This function is used to connect the device to the AP (with SSID /Passphrase)
*
* PRE-CONDITION: The system is initalized for all the peripherals and IOs
*
* POST-CONDITION: The system will be connected to wifi
* @param[in]		passphrase	Password of the AP/SSID to be connected
* @param[in]		ssid		ssid of the connection to be connected
* @param[in]		wpa_wpa2	Authentication mode WPA/WPA2 (Refer Notes)
* @param[in]		aes_tkip	Encryption mode AES/TKIP (Refer Note)
* @return	 		 	0
*
*\b Example:
* @code 
*
*
* @endcode
*
* @see seear_wifi_http.h
*
*
*<br><b>- HISTORY OF CHANGES -</b>
*
* <table align="left" style="width:800px">
* <tr><td> Date </td><td> Software Version</td><td> Initials</td><td>Description</td><tr>
* <tr><td> 08/10/2021 </td> 0.5.0	  </td><td> SP	    </td><td>Interface Created</td></tr>
* </table><br><br>
* <hr>
*
* Note : 
*	<wpa_wpa2> : 0-> WLAN_AUTH_NONE, 1-> WLAN_AUTH_WPA, 2-> WLAN_AUTH_WPA2
*		     3-> WLAN_AUTH_WPA_PSK 4-> WLAN_AUTH_WPA2_PSK, 5-> WLAN_AUTH_WPA_CCKM
*	   	     6-> WLAN_AUTH_WPA2_CCKM, 7-> WLAN_AUTH_WPA2_PSK_SHA256,
*		     OTHERS-> WLAN_AUTH_WPA2_PSK
*	<aes_tkip> : 0-> WLAN_CRYPT_NONE, 1-> WLAN_CRYPT_WEP_CRYPT, 2-> WLAN_CRYPT_TKIP_CRYPT
*		     3-> WLAN_CRYPT_AES_CRYPT, 4-> WLAN_CRYPT_BIP_CRYPT, 5-> WLAN_CRYPT_KTK_CRYPT
*		     OTHERS-> WLAN_CRYPT_AES_CRYPT
*****************************************************************************************/

int connect_device(char* passphrase, char* ssid, A_UINT8 wpa_wpa2, A_UINT8 aes_tkip)
{
	
	A_STATUS qret;
	int timeout;

	A_UINT32 param;

	switch(wpa_wpa2)
	{
	    case 0: 
		param = WLAN_AUTH_NONE;
	        break;
 	    case 1:
	        param = WLAN_AUTH_WPA;
	        break;
	    case 2:
		param = WLAN_AUTH_WPA2;
	        break;
	    case 3:
	        param = WLAN_AUTH_WPA_PSK;
	        break;
	    case 4:
	        param = WLAN_AUTH_WPA2_PSK;
		break;
	    case 5:
	        param = WLAN_AUTH_WPA_CCKM;
	        break;
	    case 6:
		param = WLAN_AUTH_WPA2_CCKM;
	        break;
	    case 7:
		param = WLAN_AUTH_WPA2_PSK_SHA256;
	        break;
	    default:
		param = WLAN_AUTH_WPA2_PSK;
	        break;
        }
	qret = qcom_sec_set_auth_mode(wifi_sta_device_id, param);
	printf("set auth mode=%d ret=%d\n",(int)param, qret);
        switch(aes_tkip)
	{
	    case 0: 
		param = WLAN_CRYPT_NONE;
	        break;
            case 1:
		param = WLAN_CRYPT_WEP_CRYPT;
	        break;
	    case 2:
	        param = WLAN_CRYPT_TKIP_CRYPT;
	        break;
	    case 3:
		param = WLAN_CRYPT_AES_CRYPT;
	        break;
	    case 4:
		param = WLAN_CRYPT_BIP_CRYPT;
	        break;
	    case 5:
	        param = WLAN_CRYPT_KTK_CRYPT;
		break;
	    default:
		param = WLAN_CRYPT_AES_CRYPT;
	        break;
        }
	qret = qcom_sec_set_encrypt_mode(wifi_sta_device_id, param);
	printf("set enc mode=%d ret=%d\n",(int)param, qret);
	
	qret = qcom_sec_set_passphrase(wifi_sta_device_id, passphrase);
	printf("set passphrase psk=%s ret=%d\n", passphrase, qret);

	qret = qcom_set_ssid(wifi_sta_device_id, ssid);
	printf("set ssid =%s qret=%d\n", ssid, qret);

	qret = qcom_set_connect_callback(device_id, wifi_sta_connect_callback);
	printf("set callback ret=%d\n", qret);

	qret = qcom_commit(device_id);
	printf("commit ret=%d\n", qret);

	printf("Waiting for STA Connect...");
	wifi_sta_state = SX_WIFI_STATE_CONNECTING;
	timeout = 300;
	while (( timeout > 0) && ( wifi_sta_state != SX_WIFI_STATE_CONNECTED )) {
	      tx_thread_sleep(100);
	      printf(".");
	      timeout--;
         }

	if ( wifi_sta_state == SX_WIFI_STATE_CONNECTED ) {
	    printf("Connected\n");
	 }
	else{
	   printf("Connect timeout\n");
	   disconnect_device();
	}
        return 0;
}



/*********************************************************************************************
* Function : disconnect_device();
*\b Description:
*
* This function is used to disconnect the device from the WIFI AP
*
* PRE-CONDITION: The system is initalized for all the peripherals and IOs
*
* POST-CONDITION: The device will be disconnected from WiFi
* 
* @return	 		 	0
*
*\b Example:
* @code 
*
*
* @endcode
*
* @see seear_wifi_http.h
*
*
*<br><b>- HISTORY OF CHANGES -</b>
*
* <table align="left" style="width:800px">
* <tr><td> Date </td><td> Software Version</td><td> Initials</td><td>Description</td><tr>
* <tr><td> 09/10/2021 </td> 0.5.0	  </td><td> SP	    </td><td>Interface Created</td></tr>
* </table><br><br>
* <hr>
*
*****************************************************************************************/
int disconnect_device(void)
{
	A_STATUS qret;
	int timeout;
	
	if ( wifi_sta_state != SX_WIFI_STATE_DISCONNECTED ) {
	   wifi_sta_state = SX_WIFI_STATE_DISCONNECTING;
	   qret = qcom_disconnect(wifi_sta_device_id);
	   
	   while ( (timeout > 0) && (wifi_sta_state != SX_WIFI_STATE_DISCONNECTED)) {
	         tx_thread_sleep(100);
	         printf(".");
	         timeout--;
	   }
	   qret = qcom_set_ssid(wifi_sta_device_id, "");
       	 }
	 printf("Disconnected\n");
	 return 0;
}	

/*********************************************************************************************
* Function : ping_connection();
*\b Description:
*
* This function is used to ping the address with the ping size 
*
* PRE-CONDITION: The system is initalized for all the peripherals and IOs
*		 and connected to WiFi successfully
*
* POST-CONDITION: The system if connected will ping to the address mentioned
* @param[in]	pAddress	The address of the site to be pinged
* @param[in] 	ping_size	Size of the Ping to be conducted
*
* @return	 		 	0
*
*\b Example:
* @code 
*
*
* @endcode
*
* @see seear_wifi_http.h
*
*
*<br><b>- HISTORY OF CHANGES -</b>
*
* <table align="left" style="width:800px">
* <tr><td> Date </td><td> Software Version</td><td> Initials</td><td>Description</td><tr>
* <tr><td> 09/10/2021 </td> 0.5.0	  </td><td> SP	    </td><td>Interface Created</td></tr>
* </table><br><br>
* <hr>
* Note : Ping size is not greater than 32
*****************************************************************************************/
int ping_connection(char* pAddress, int ping_size)
{

	A_STATUS qret;
	A_UINT32 address;
	struct sockaddr_storage ss;
	char buff[20];
	int len;	
	int def_ping_size = 32;

	len = sizeof(ss);
	gf_StringToSockaddr(pAddress, &ss, &len);
	address = ntohl(((struct sockaddr_in*)&ss)->sin_addr.s_addr);
	len = sizeof(buff);
	gf_SockaddrToString(&ss, buff, &len);
	if ( ping_size > 32 || ping_size <= 0 ) {
	   ping_size = def_ping_size;
	}
	qret = qcom_ping(address, ping_size);
	printf("ping addr=%s, size=%d, ret=%d\n", buff, ping_size, qret);

	return 0;
}


/*********************************************************************************************
* Function : show_wifi_status();
*\b Description:
*
* This function is used to show the wifi status of the device id passed 
*
* PRE-CONDITION: The system is initalized for all the peripherals and IOs
*		 
*
* POST-CONDITION: The system if connected will show the wifi status such as the ssid, mac, 
*		  power mode, operating mode of the device
* @param[in]	device_id	The device id of the wifi device
* 
* @return	 		 	None
*
*\b Example:
* @code 
*
*
* @endcode
*
* @see seear_wifi_http.h
*
*
*<br><b>- HISTORY OF CHANGES -</b>
*
* <table align="left" style="width:800px">
* <tr><td> Date </td><td> Software Version</td><td> Initials</td><td>Description</td><tr>
* <tr><td> 09/10/2021 </td> 0.5.0	  </td><td> SP	    </td><td>Interface Created</td></tr>
* </table><br><br>
* <hr>
* Note : Ping size is not greater than 32
*****************************************************************************************/
void show_wifi_status(A_UINT8 device_id)
{

	char ssid[64+1];
	A_STATUS qret;
	A_UINT32 wpm;
	A_UINT8 mac[6];
	A_UINT32 mode;
	A_UINT16 channel = 0;
	
	qret = qcom_get_ssid(device_id, ssid);
	printf("SSID:%s\n",ssid);

	qret = qcom_mac_get(device_id, mac);
	printf("MAC:%x:%x:%x:%x:%x:%x\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

	qret = qcom_power_get_mode(device_id, &wpm);
	printf("Power:%s\n",(wpm == MAX_PERF_POWER) ? "Max Perf" : "Power Save");

	qret = qcom_op_get_mode(device_id, &mode);
	printf("Mode:");
	switch(mode) {
	case QCOM_WLAN_DEV_MODE_STATION:
	     printf("STATION\n");
	     break;
	case QCOM_WLAN_DEV_MODE_AP:
	     printf("AP\n");
	     break;
	case QCOM_WLAN_DEV_MODE_ADHOC:
	     printf("ADHOC\n");
	     break;
	default:
	     printf("Invalid(%d)\n", mode);
	     break;
	 }
	 qret = qcom_get_channel(device_id, &channel);
	 printf("Channel:%d\n", channel);
}

/*********************************************************************************************
* Function : create_https_context_client()
*\b Description:
*
* This function is used to create a new SSL context before HTTPS transactions 
*
* PRE-CONDITION: The system is initalized for all the peripherals and IOs and HTTPS 
*		 transaction to be done next
*
* POST-CONDITION: SSL context is created for the client  
* 
* @return	 		 	0
*
*\b Example:
* @code 
*
*
* @endcode
*
* @see seear_wifi_http.h
*
*
*<br><b>- HISTORY OF CHANGES -</b>
*
* <table align="left" style="width:800px">
* <tr><td> Date </td><td> Software Version</td><td> Initials</td><td>Description</td><tr>
* <tr><td> 09/10/2021 </td> 0.5.0	  </td><td> SP	    </td><td>Interface Created</td></tr>
* </table><br><br>
* <hr>
*
*****************************************************************************************/
int create_https_context_client(void)
{
	if ( httpc_sslctx != NULL ) {
	   qcom_SSL_ctx_free( httpc_sslctx );
	   httpc_sslctx = NULL;
	 }
	 httpc_sslctx = qcom_SSL_ctx_new(SSL_CLIENT, 6000, 3500, 0);
	 printf("SSL ctx=0x%x\n",(unsigned int)httpc_sslctx);

	 return 0;
}

/*********************************************************************************************
* Function : connect_http();
*\b Description:
*
* This function is used to make a HTTP connection  
*
* PRE-CONDITION: The system is initalized for all the peripherals and IOs and WIFI should be 
*		 be connected
*
* POST-CONDITION: HTTP connnection is established to the Hostname and port passed
* @param[in]		pHostname	Hostname to be connected 
* @param[in]		pPort		Port to which HTTP should be established  
* 
* @return	 		 	0
*
*\b Example:
* @code 
*
*
* @endcode
*
* @see seear_wifi_http.h
*
*
*<br><b>- HISTORY OF CHANGES -</b>
*
* <table align="left" style="width:800px">
* <tr><td> Date </td><td> Software Version</td><td> Initials</td><td>Description</td><tr>
* <tr><td> 09/10/2021 </td> 0.5.0	  </td><td> SP	    </td><td>Interface Created</td></tr>
* </table><br><br>
* <hr>
* Note : if NULL is passed to pPort the port is defaulted to 80
*****************************************************************************************/
int connect_http(char* pHostname, char* pPort)
{
	char *httpc_hostname = NULL;
	char *httpc_port = "80";
	A_STATUS qret;

	httpc_hostname = pHostname;

	if ( pPort != NULL ) {
	   httpc_port = pPort;
	}
	qret = qcom_http_client_method(
	       HTTPC_CONNECT_CMD,
	       (A_UINT8*)httpc_hostname,
	       (A_UINT8*)httpc_port,
	       (A_UINT8*)NULL,
	        NULL);

	printf("HTTP connect ret=%d\n", qret);
	return 0;
}


/*********************************************************************************************
* Function : connect_https();
*\b Description:
*
* This function is used to make a HTTPS connection  
*
* PRE-CONDITION: The system is initalized for all the peripherals and IOs and WIFI should be 
*		 be connected
*
* POST-CONDITION: HTTP connnection is established to the Hostname and port passed
* @param[in]		pHostname	Hostname to be connected 
* @param[in]		pPort		Port to which HTTP should be established  
* 
* @return	 		 	0
*
*\b Example:
* @code 
*
*
* @endcode
*
* @see seear_wifi_http.h
*
*
*<br><b>- HISTORY OF CHANGES -</b>
*
* <table align="left" style="width:800px">
* <tr><td> Date </td><td> Software Version</td><td> Initials</td><td>Description</td><tr>
* <tr><td> 09/10/2021 </td> 0.5.0	  </td><td> SP	    </td><td>Interface Created</td></tr>
* </table><br><br>
* <hr>
* Note : if NULL is passed to pPort the port is defaulted to 443
*****************************************************************************************/

int connect_https(char* pHostname, char* pPort)
{

	char *httpc_hostname = NULL;
	char *httpc_port = "443";
	A_STATUS qret;

	if ( httpc_sslctx == NULL ) {
	   printf("SSL context is not ready\n");
	   return -1;
	}
	if ( *pHostname == NULL ) {
	   return -1;
  	}
	httpc_hostname = pHostname;
	if ( pPort != NULL ) {
	  httpc_port = pPort;
	}
	qret = qcom_http_client_method(
		HTTPC_CONNECT_SSL_CMD,
	 	(A_UINT8*)httpc_hostname,
		(A_UINT8*)httpc_port,
		(A_UINT8*)NULL,
		httpc_sslctx);
	
	printf("HTTPS connect ret=%d\n", qret);
	
	return 0;
}

/*********************************************************************************************
* Function : disconnect_http();
*\b Description:
*
* This function is used to disconnect HTTP/HTTPS connection   
*
* PRE-CONDITION: The system is initalized for all the peripherals and IOs and WIFI should be 
*		 be connected also HTTP/HTTPS should be connected
*
* POST-CONDITION: HTTP/HTTPS connnection established is disconnected. 
*
* 
* @return	 		 	0
*
*\b Example:
* @code 
*
*
* @endcode
*
* @see seear_wifi_http.h
*
*
*<br><b>- HISTORY OF CHANGES -</b>
*
* <table align="left" style="width:800px">
* <tr><td> Date </td><td> Software Version</td><td> Initials</td><td>Description</td><tr>
* <tr><td> 09/10/2021 </td> 0.5.0	  </td><td> SP	    </td><td>Interface Created</td></tr>
* </table><br><br>
* <hr>
* 
*****************************************************************************************/

int disconnect_http(void)
{
	A_STATUS qret;

	qret = qcom_http_client_method(
		HTTPC_DISCONNECT_CMD,
	 	(A_UINT8*)NULL,
		(A_UINT8*)NULL,
		(A_UINT8*)NULL,
		NULL);

	printf("HTTP disconnect ret=%d\n", qret);
	
	return 0;
}

/*********************************************************************************************
* Function : client_http_get();
*\b Description:
*
* This function is used to disconnect HTTP/HTTPS connection   
*
* PRE-CONDITION: The system is initalized for all the peripherals and IOs and WIFI should be 
*		 be connected also HTTP/HTTPS should be connected
*
* POST-CONDITION: HTTP/HTTPS GET Method is executed 
* @param[in, out]	pMessage 	The Message to be GET is returned 
* @param[in]		url_ptr		pointer to the URL to execute GET method
* 
* @return	 		 	0
*
*\b Example:
* @code 
*
*
* @endcode
*
* @see seear_wifi_http.h
*
*
*<br><b>- HISTORY OF CHANGES -</b>
*
* <table align="left" style="width:800px">
* <tr><td> Date </td><td> Software Version</td><td> Initials</td><td>Description</td><tr>
* <tr><td> 09/10/2021 </td> 0.5.0	  </td><td> SP	    </td><td>Interface Created</td></tr>
* </table><br><br>
* <hr>
* 
*****************************************************************************************/
int client_http_get(char* pMessage, const char *url_ptr)
{
	A_STATUS qret;
	if (NULL == url_ptr ) {
	   return -1;
	}
	qret = qcom_http_client_body(
	  	HTTPC_BODY_CMD,
		(A_UINT8*)pMessage,
		strlen(pMessage) );
	httpc_action(HTTPC_GET_CMD, url_ptr);
	return 0;

}
int client_http_post(char* pMessage, const char *url_ptr);
