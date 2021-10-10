#include "qcom_common.h"
#include "threadx/tx_api.h"
#include "threadxdmn_api.h"

#ifndef USER_APP_STACK_SIZE
#ifdef REV74_TEST_ENV4
/* default stack size for QCA400x */
#define USER_APP_STACK_SIZE (2 * 1024 )
#else
/* default stack size for QCA401x */
#define USER_APP_STACK_SIZE (4 * 1024 )
#endif
#endif

#ifndef USER_APP_PRIORITY
#define USER_APP_PRIORITY 16
#endif

#ifndef USER_APP_SLICE
#define USER_APP_SLICE 4
#endif

TX_THREAD host_thread;
A_UINT8 user_app_stack[USER_APP_STACK_SIZE];

extern void user_app_entry(ULONG param);

void user_main(void)
{
    tx_thread_create(&host_thread, "user_app thread", user_app_entry, 0, user_app_stack, USER_APP_STACK_SIZE, USER_APP_PRIORITY, USER_APP_PRIORITY, USER_APP_SLICE, TX_AUTO_START);

    cdr_threadx_thread_init();
}

