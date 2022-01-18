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

/*********************************************************************
*		 	Typedefs
**********************************************************************/

	
/*********************************************************************
*			Function Prototypes
**********************************************************************/

A_INT32 pcdm3180_init( void ); 
void display_init( void );
void button_init( void );


#ifdef __cplusplus
}

#endif


#endif

