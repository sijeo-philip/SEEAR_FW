/*****************************************************************
* Title			:	seear_sys_init
* Filename		:	seear_sys_init.h
* Author		:	Sijeo Philip
* Origin Date		:	26th Oct 2021
* Version		:	1.0.0
* Compiler		:	gcc
* Target		: 	QCA4010 (SX-ULGPN, Silex)
* Notes			:	This Module is used to Initialize the
*				System
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
*  26/10/2021   1.0.0	     Sijeo Philip	Interface Created
*********************************************************************/

/********************************************************************
* @file seear_sys_init.h
* @brief This module is used to initialize the system 
*	 
*********************************************************************/

#ifndef _SEEAR_SYS_INIT_H_
#define _SEEAR_SYS_INIT_H_

#ifdef __cplusplus
{
#endif

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
#include "select_api.h"
#include "qcom_i2s.h"
#include "qcom_i2c_master.h"
#include "qcom_gpio.h"

/*********************************************************************
*		        Preprocessor Constants
***********************************************************************/
#define I2S_BUFF_SIZE 		1024
#define I2S_BUFF_MAX_SIZE	1440
#define I2S_BUFF_CNT		10
#define AUDIO_BUFF_SIZE		1024*10
#define AUDIO_CAPTURE_INTERVAL	2000



#define I2S1			1
#define I2S0			0

/**
* This MACRO is used to use the qcom application timer or ThreadX timer
* 1 ==> To use the qcom_timer API for Millisecond Delay
* 2 ==> To use the ThreadX ticks timer.
*
***/
#ifndef AUDIO_CAPTURE_TIMER
#define AUDIO_CAPTURE_TIMER	1
#endif


/*********************************************************************
*		 	Typedefs
**********************************************************************/
typedef struct audio_buff_ctrl
{
	A_UINT8 currBuffFlag;  /** << stores the state of the current buffer
				      0 for Buffer1 as current Buffer 1 as
				      for Buffer2 as current Buffer */ 
	A_UINT8 *p_currentBuffer;
	A_UINT8 *p_captureBuffer;
	A_UINT32 audioDataCount;   /**<< Number of 1K bytes of Data Recvd*/
	A_UINT32 audioTransmitCount; /**<< Number of 10K transmit over Wifi*/
}audio_buff_ctrl_t;



	
/*********************************************************************
*			Function Prototypes
**********************************************************************/

A_INT32 pcdm3180_init( void ); 
void display_init( void );
void button_init( void );
void audio_buffer_init(audio_buff_ctrl_t *audio_buff_ctrl);
void swap_audio_buff(audio_buff_ctrl_t *audio_buff_ctrl);


#ifdef __cplusplus
}

#endif


#endif

