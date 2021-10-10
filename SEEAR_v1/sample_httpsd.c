#include "qcom_common.h"
#include "socket_api.h"
#include "qcom_ssl.h"
#include "threadx/tx_api.h"
#include "threadxdmn_api.h"

A_UINT8 wifi_ap_device_id = 0;
A_UINT8 wifi_ap_state = 0;

void wifi_connect_callback(A_UINT8 device_id, A_INT32 value)
{
    printf("WiFi callback stat=%d event=%d\n", wifi_ap_state, value);
    if (value == 1 || value == 0x10) {
        wifi_ap_state = 1;
    }
}

void user_app_entry(ULONG param)
{
    A_STATUS qret;
    A_INT32 ret;
    SSL_CTX* sslCtx;
    extern const unsigned char server_cert[];
    extern const int server_cert_size;

    extern void user_pre_init(void);
    user_pre_init();

    qcom_enable_print(1);

    qret = qcom_set_connect_callback(wifi_ap_device_id, wifi_connect_callback); 
    printf("set callback ret=%d\n", qret);
    qret = qcom_op_set_mode(wifi_ap_device_id, 1); 
    printf("set AP mode ret=%d\n", qret);
    qret = qcom_power_set_mode(wifi_ap_device_id, MAX_PERF_POWER);
    printf("set MAXPERF mode ret=%d\n", qret);
    qret = qcom_set_channel(wifi_ap_device_id, 6);
    printf("set channel ret=%d\n", qret);
    qret = qcom_set_ssid(wifi_ap_device_id, "SX-ULPGN-AP");
    printf("set ssid ret=%d\n", qret);
    qret = qcom_commit(wifi_ap_device_id);
    printf("commit ret=%d\n", qret);

    printf("Waiting for connect...");
    while (wifi_ap_state == 0) {
        tx_thread_sleep(100);
        printf(".");
    }
    printf("connected\n");

    {
        A_UINT32 ipaddr, netmask, gateway;
        ipaddr  = 0x0a000001; /* 10.0.0.1 */
        netmask = 0xff000000; /* 255.0.0.0 */
        gateway = 0x00000000; /* 0.0.0.0 */
        qcom_ipconfig(wifi_ap_device_id, 1, &ipaddr, &netmask, &gateway);
    }

    {
        A_UINT32 startip, endip, leasetime;
        startip = 0x0a00000a; /* 10.0.0.10 */
        endip   = 0x0a00ffff; /* 10.0.255.255 */
        leasetime = 3600; /* 3600 sec = 1 hour */
        qret = qcom_dhcps_set_pool(wifi_ap_device_id, startip, endip, leasetime);
        printf("dhcps_set_pool ret=%d\n", qret);
    }

    sslCtx = qcom_SSL_ctx_new(SSL_SERVER, 6000, 3500, 0);
    printf("SSL_ctx_new ptr=%x\n", (unsigned int)sslCtx);
    if (sslCtx != NULL) {
        ret = qcom_SSL_addCert(sslCtx, (A_UINT8*)server_cert, server_cert_size);
        printf("addCert ret=%d\n", ret);

        qret = qcom_http_server(3, sslCtx); /* 3=Start HTTPS server */
        printf("http_server ret=%d\n", qret);
    }

    /* Exit from user thread */
}
