#include "qcom_common.h"
#include "threadx/tx_api.h"
#include "threadxdmn_api.h"
#include "qcom_cli.h"
#include "socket_api.h"
#include "qcom_event.h"
#include <ctype.h>

#include "seear_wifi_http.h"
#include "seear_sys_init.h"

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

#ifndef USER_APP_SLICE_WR
#define USER_APP_SLICE_WR 100
#endif

#ifndef USER_APP_SLICE_RD
#define USER_APP_SLICE_RD 100
#endif

#define AUDIO_BUFF_SIZE	1024*10
#define I2S_QUEUE_SIZE	4

TX_THREAD writer_thread;
TX_THREAD reader_thread;
TX_QUEUE i2s_queue;

A_UINT8 write_audio_stack[USER_APP_STACK_SIZE];
A_UINT8 read_audio_stack[USER_APP_STACK_SIZE];

A_INT16 m_currentAudioBuffer[AUDIO_BUFF_SIZE];
A_INT16 m_capturedAudioBuffer[AUDIO_BUFF_SIZE];

A_UINT32 I2S_Audio_Queue[I2S_QUEUE_SIZE];


/*********************************************************************************************
* Function : process_audio_samples()
*\b Description:
*
* This function is used to check if the stored i2s samples have crossed the set Threshold
*
* PRE-CONDITION: The system is initalized for all the peripherals and IOs
*
* POST-CONDITION: The string provided is converted to SockAddr
* @param[in]		threshold	Threshold value to be checked for.
* @param[in]		data		i2s buffer data
*
* @return	 		 	1-If threshold crossed/0-otherwise
*
*\b Example:
* @code 
*
*
* @endcode
*
* @see 
*
*
*<br><b>- HISTORY OF CHANGES -</b>
*
* <table align="left" style="width:800px">
* <tr><td> Date </td><td> Software Version</td><td> Initials</td><td>Description</td><tr>
* <tr><td> 16/11/2021 </td> 0.5.0	  </td><td> SP	    </td><td>Interface Created</td></tr>
* </table><br><br>
* <hr>
*
************************************************************************************************/

static A_BOOL process_audio_samples(void* data, A_UINT16 threshold)
{
	//TO DO: Check if the Data is passed the Threshold for Recording

}
/*********************************************************************************************
* Function : user_app_entry()
*\b Description:
*
* Threadx thread for processing the I2S data and initiating data for HTTP transfer to SErver
*
* PRE-CONDITION: The system is initalized for all the peripherals and IOs
*
* POST-CONDITION: The string provided is converted to SockAddr
* @param[in]		param		data to the Thread 
*
* @return	 		 	None
*
*\b Example:
* @code 
*
*
* @endcode
*
* @see 
*
*
*<br><b>- HISTORY OF CHANGES -</b>
*
* <table align="left" style="width:800px">
* <tr><td> Date </td><td> Software Version</td><td> Initials</td><td>Description</td><tr>
* <tr><td> 16/11/2021 </td> 0.5.0	  </td><td> SP	    </td><td>Interface Created</td></tr>
* </table><br><br>
* <hr>
*
************************************************************************************************/

void audio_writer_thread(ULONG param)
{
	A_STATUS qret;
	extern void user_pre_init(void);

	user_pre_init();
	qcom_enable_print(1);
	pcdm3180_init();
	while(1)
	{

	//TO DO: wait for message queue
	}

}


void audio_reader_thread(ULONG param)
{

	while(1)
	{


	}


}

void user_main(void)
{

/***
*  Thread creation parameters
*  UINT tx_thread_create(TX_THREAD *thread_ptr, CHAR *name_ptr, VOID (*entry_function)(ULONG),
*			 ULONG entry_input, VOID *stack_start, ULONG stack_size, UINT priority,
*			 UINT preempt_threshold, ULONG time_slice, UINT auto_start)
* <*thread_ptr>-- Pointer to the Thread Dresciptor,
* <*name_ptr> -- Name of the thread for user description, <*entry_function> -- Entry Function
* < entry_input> -- Value passed to the entry function, <*stack_start>-- Pointer to the Thread stack
* < stack_size>-- size of the Thread Stack, <priority> --Priority of thread, 0 being the highest
* < preempt_threshold> -- threads with priority higher than threshold can preempt the thread
* <time_slice> -- Number of ticks the thread should run before preemption,
* <auto_start> -- Start the thread immediately or wait for thread_resume to be called.
*/
       tx_thread_create(&writer_thread, "audio_writer_thread", audio_writer_thread, 0, write_audio_stack, USER_APP_STACK_SIZE, USER_APP_PRIORITY, USER_APP_PRIORITY, USER_APP_SLICE_WR, TX_AUTO_START);

	tx_thread_create(&reader_thread, "audio_reader_thread", audio_reader_thread, 0, read_audio_stack,
USER_APP_STACK_SIZE, USER_APP_PRIORITY, USER_APP_PRIORITY, USER_APP_SLICE_RD, TX_AUTO_START);
/****
*Message Queue Creation Parameters
*UINT tx_queue_create(TX_QUEUE *queue_ptr, CHAR *name_ptr, UINT message_size,
*		      VOID *queue_start, ULONG queue_size);
*<*queue_ptr> -- Address of the Descriptor of the Queue Created
*<*name_ptr> -- String representing the name of the Queue, 
*<message_size> -- Size of each message on queue, <*queue_start> -- Address of the Queue Memory
*<queue_size> -- Size of the Queue in bytes i.e. it should match the number of 
* ULONG messages it can hold
*/

	tx_queue_create(&i2s_queue, "audio_queue", TX_1_ULONG, I2S_Audio_Queue, 16);
     //TO DO: Initialize Message queue.
	
      	cdr_threadx_thread_init();
}

