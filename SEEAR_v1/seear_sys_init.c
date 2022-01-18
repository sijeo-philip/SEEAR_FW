/*****************************************************************
* Title			:	seear_sys_init
* Filename		:	seear_sys_init.c
* Author		:	Sijeo Philip
* Origin Date		:	26th Oct 2021
* Version		:	1.0.0
* Compiler		:	gcc
* Target		: 	QCA4010 (SX-ULGPN, Silex)
* Notes			:	This Module is used to initialize the
*				system
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

/**********************SOURCE REVISION LOG **************************
*
*	Date	Version		Author		Description
*    26/10/2021  1.0.0	      Sijeo Philip	Initial Release
*
**********************************************************************/

/**********************************************************************
* @file: seear_sys_init.c
* @brief : This is the source file for initializing the system
* 
***********************************************************************/

/**********************************************************************
*		INCLUDES
***********************************************************************/
#include "seear_sys_init.h"

/**********************************************************************
*		MODULE PREPROCESSOR MACROS
***********************************************************************/
#define I2S_BUFF_SIZE 		1024
#define I2S_BUFF_MAX_SIZE	1440
#define I2S_BUFFER_CNT		8

#define I2S1_SCK		27
#define I2S1_SDI		30
#define I2S1_SDO		31
#define I2S1_WS			32
#define I2S1_MCLK		33

#define I2S1			1
#define I2S0			0
/**********************************************************************
*		MODULE TYPEDEFS	
***********************************************************************/

/**********************************************************************
*		MODULE VARIABLE DEFINITIONS
***********************************************************************/
static i2s_api_config_s i2sConfig {
			.port = I2S1, /**<< PORT 1 of I2S*/
			.freq = I2S_FREQ_44_1K, /**<< I2S Frequency of Sampling */
			.dsize = 16, /**<< 16 bit Data Size of each sample */
			.i2s_buf_size = I2S_BUFF_SIZE, /**<< I2S data buffer size */
			.num_rx_desc = I2S_BUFF_CNT, /**<< Number of recieve Buffer */
			.num_tx_desc = 0, /**<< Number for Transmit Buffer */
			.i2s_mode =1,     /**<< Master Mode for the i2s peripheral */
};
/**********************************************************************
*		FUNCTION PROTOTYPES	
***********************************************************************/


/*************************************************************************
*		CALLBACKS
**************************************************************************/
static void i2s_rxcomp_intr_callback( unsigned char* data , unsigned int size)
{



}

/*************************************************************************
*		FUNCTION DEFINITIONS
*************************************************************************/

/*********************************************************************************************
* Function : pcdm3180_init( void )
*\b Description:
*
* This function is used to initialize the I2S interface to PCDM
* TDM to I2S converter
*
*  PRE-CONDITION: The system is initalized for all the peripheral
*  POST-CONDITION: The PCDM3180 TDM to I2S converter is initialized 
*		  for further operations  
* @param[in, out]	None
*
* @return	  	Success/Fail for initializing
*
*\b Example:
* @code 
*
*
* @endcode
*
* @see seear_sys_init.h
*
*
*<br><b>- HISTORY OF CHANGES -</b>
*
* <table align="left" style="width:800px">
* <tr><td> Date </td><td>Software Version</td><td>Initials<td><td>Description</td><tr>
* <tr><td> 04/10/2021</td>0.5</td><td>SP</td><td>Interface Created</td></tr>
* </table><br><br>
* <hr>
*
************************************************************************************************/

A_INT32 pcdm3180_init( void )
{
	A_INT32 ret;
	qcom_gpio_pin_dir(I2S1_SDI, 1); /*<< Input */
	qcom_gpio_pin_dir(I2S1_SCK, 0);
	qcom_gpio_pin_dir(I2S1_SDO, 0);
	qcom_gpio_pin_dir(I2S1_WS, 0);
	qcom_gpio_pin_dir(I2S1_MCLK, 0);

	//TO DO: Initialize I2C and Initialize the pcdm3180 device over I2C

	/* Initialize the I2S Interface for the MEMS MIC */
	ret = qcom_i2s_init( &i2sConfig, NULL,  i2s_rxcomp_intr_callback);
	/*Begin Transfer in I2S Interface */
	qcom_i2s_rcv_control(I2S1, I2S_RCV_START);
	return ret;
	
}
