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


#define I2S_QUEUE_SIZE	10

TX_THREAD writer_thread;
TX_THREAD reader_thread;
TX_QUEUE i2s_queue;
TX_MUTEX memory_mutex;
#if AUDIO_CAPTURE_TIMER == 2
TX_TIMER audio_capture_timer;
#elif AUDIO_CAPTURE_TIMER == 1
qcom_timer_t audio_capture_timer;
#endif 

A_UINT8 write_audio_stack[USER_APP_STACK_SIZE];
A_UINT8 read_audio_stack[USER_APP_STACK_SIZE];


A_UINT32 I2S_Audio_Queue[I2S_QUEUE_SIZE];

audio_buff_ctrl_t  audioBuffCtrl;

/**********************************************************************************
*			CALLBACKS
*
***********************************************************************************/
#if AUDIO_CAPTURE_TIMER == 2
void audio_capture_timer_callback( ULONG params)
{

	//TO DO: resume the reader thread
	//TO DO: Activate the I2S Interface
	//TO DO: deactivate the Timer 
	tx_thread_resume(&reader_thread);
	qcom_i2s_rcv_control(I2S1, I2S_RCV_START);
	tx_timer_deactivate(&audio_capture_timer);

	

}
#elif AUDIO_CAPTURE_TIMER == 1
void audio_capture_timer_callback( unsigned int params, void* data )
{

	//TO DO: resume the reader thread
	//TO DO: Activate the I2S Interface
	//TO DO: stop the timer.
	tx_thread_resume(&reader_thread);
	qcom_i2s_rcv_control(I2S1, I2S_RCV_START);
	qcom_timer_stop(&audio_capture_timer);

}

#endif 
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
#if 0
static A_BOOL process_audio_samples(void* data, A_UINT16 threshold)
{
	//TO DO: Check if the Data is passed the Threshold for Recording

}
#endif 
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

void audio_reader_thread(ULONG param)
{
	//A_STATUS qret;
	A_UINT32 status;
	A_UINT32 message;
	A_INT32 ret = -1;
	A_UINT32 rcvd_size=0;
	extern void user_pre_init(void);

	user_pre_init();
	qcom_enable_print(1);
	pcdm3180_init();
	
	while(1)
	{
	  status = tx_queue_receive(&i2s_queue, &message, TX_WAIT_FOREVER);
	  if ( TX_SUCCESS == status )
	  {
		
	     do {
		 ret = qcom_i2s_rcv_data(I2S1, audioBuffCtrl.p_currentBuffer+(message*I2S_BUFF_SIZE), I2S_BUFF_SIZE, &rcvd_size);	
		 if (rcvd_size < I2S_BUFF_SIZE && rcvd_size != 0)
		    qcom_printf("[%d]", rcvd_size);
		}while(ret==0); /* ret=0 means I2S DMA own normally*/
	     audioBuffCtrl.audioDataCount++;	
	     if (( 9 == audioBuffCtrl.audioDataCount ) && (rcvd_size > 0))
		{
		     //TO DO: Stop the I2S interface 
		    //TO DO : Get Mutex for the Memory Buffer
		    //2. Swap the Memory Buffer
		    //3. Release the Mutex
		    //4. Resume the writer thread
		    //5. Reset the audioDataCount to Zero
		    //6. Start the I2S interface

		    qcom_i2s_rcv_control(I2S1, I2S_RCV_STOP);
		    if(	TX_SUCCESS == tx_mutex_get(&memory_mutex, TX_WAIT_FOREVER) )
		    {
			swap_audio_buff(&audioBuffCtrl);
		       	tx_mutex_put(&memory_mutex);
			tx_thread_resume(&writer_thread);
		        audioBuffCtrl.audioDataCount = 0;
			qcom_i2s_rcv_control(I2S1, I2S_RCV_START);
		    }	    
		}
	   }
	}
}


void audio_writer_thread(ULONG param)
{

	while(1)
	{
		//TO DO: Wait for the Mutex
		//2. On reciept of Mutex
		//3. Send the p_captureBuffer Data over HTTP over WiFi
		//3a. If successfully sent, increment the dataTransmitCount
		//3b. If Transmit count reaches above 50, suspend the readerThread
		//3b. Stop the I2S interface
		//3b. with tx_thread_suspend, clear the queue, activate the Timer
		//3b  based on AUDIO_CAPTURE_TIMER Configuration MACRO
		//4. Release the Mutex
		//5. Suspend the Thread.
		if (TX_SUCCESS == tx_mutex_get(&memory_mutex, TX_WAIT_FOREVER) )
		{
		   /*SEND HTTP REQUEST OVER WIFI*/
		   /*IF SUCCESSFULLY SENT*/
		   audioBuffCtrl.audioTransmitCount = audioBuffCtrl.audioTransmitCount + 1;
		   if (audioBuffCtrl.audioTransmitCount >= 50 )
		   {
			qcom_i2s_rcv_control(I2S1, I2S_RCV_STOP);
			audioBuffCtrl.audioTransmitCount = 0;
			tx_queue_flush(&i2s_queue);
			tx_thread_suspend(&reader_thread);			
			#if AUDIO_CAPTURE_TIMER == 1
			qcom_timer_start(&audio_capture_timer);
			#elif AUDIO_CAPTURE_TIMER == 2
			tx_timer_activate(&audio_capture_timer);
			#endif
			tx_mutex_put(&memory_mutex);
			tx_thread_suspend(&writer_thread);
                   }

		}

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
       tx_thread_create(&writer_thread, "audio_writer_thread", audio_writer_thread, 0, write_audio_stack, USER_APP_STACK_SIZE, USER_APP_PRIORITY, USER_APP_PRIORITY, USER_APP_SLICE_WR, TX_DONT_START); /**<< This thread is used to write to Socket */

	tx_thread_create(&reader_thread, "audio_reader_thread", audio_reader_thread, 0, read_audio_stack,
USER_APP_STACK_SIZE, USER_APP_PRIORITY, USER_APP_PRIORITY, USER_APP_SLICE_RD, TX_AUTO_START); /**<< This Thread is used to capture the I2S*/

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

	tx_queue_create(&i2s_queue, "audio_queue", TX_1_ULONG, I2S_Audio_Queue, 40);
/******
*Mutex Creation Parameters
*UINT tx_mutex_create(TX_MUTEX *mutex_ptr, CHAR *name_ptr, UINT priority_inherit)
*<*mutex_ptr> -- Pointer to Mutex Control Block
*<*name_ptr> -- Pointer to name of the mutex
*<priority_inherit> -- Specifies whether or not this mutex supports priority inheritance
*If this value is TX_INHERIT, then priority inheritance is supported. However, if TX_NO_INHERIT
*is specified, priority inheritance is not supported by this mutex
*******/

	tx_mutex_create(&memory_mutex, "muter_i2s_memory", TX_NO_INHERIT);

/********
*Application Timer Create API
*UINT tx_timer_create(TX_TIMER *timer_ptr, CHAR *name_ptr, VOID (*expiration_function)
*                     (ULONG), ULONG expiration_input, ULONG initial_ticks,
*		      ULONG reschedule_ticks, UINT auto_activate)
*<*timer_ptr> -- Pointer to a timer Control Block
*<*expiration_function(ULONG)> -- Application function to call when the timer expires
*<expiration_input> -- Input to pass to Expiration function when the timer expires
*<initial_ticks> -- Specifies the initial number of ticks for timer expiration
*<reschedule_ticks> -- Specifies the number for timer_ticks for all timer expiration
*	 	       after the first
*<auto_activate> -- Determines whether the timer is automatically activated during 
*		    creation i.e. TX_AUTO_ACTIVATE, else TX_NO_ACTIVATE
*********/
#if AUDIO_CAPTURE_TIMER == 2
	tx_timer_create( &audio_capture_timer, "audio_capture", audio_capture_timer_callback, 0, AUDIO_CAPTURE_INTERVAL, AUDIO_CAPTURE_INTERVAL, TX_NO_ACTIVATE);
#elif AUDIO_CAPTURE_TIMER == 1 
	qcom_timer_init(&audio_capture_timer, audio_capture_timer_callback, NULL, 
	                2000, PERIODIC);

#endif
	
      	cdr_threadx_thread_init();
}

