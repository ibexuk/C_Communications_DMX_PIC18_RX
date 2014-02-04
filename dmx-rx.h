/*
Provided by IBEX UK LTD http://www.ibexuk.com
Electronic Product Design Specialists
RELEASED SOFTWARE

The MIT License (MIT)

Copyright (c) 2013, IBEX UK Ltd, http://ibexuk.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
//Visit http://www.embedded-code.com/source-code/communications/dmx512/pic18-dmx-receive-driver for more information
//
//Project Name: PIC18 DMX RX



//########################################
//########################################
//########## USING THIS DRIVER ###########
//########################################
//########################################

//######################
//##### INITIALISE #####
//######################
/*
	//-----------------------
	//----- SETUP USART -----
	//-----------------------
	//Setup to use for DMX

//>>>>> FOR PIC18 LATER MODELS e.g. 2520 etc
	//SETUP BAUD RATE
	TXSTAbits.BRGH = 0;					//Low speed mode
	BAUDCONbits.BRG16 = 1;				//16 bit baud rate generator
	SPBRGH = (((((DWORD)INSTRUCTION_CLOCK_FREQUENCY << 2) / 250000) / 16) - 1) >> 8;			//(((OSCILLATOR_FREQUENCY / DESIRED_BAUD) / DIVIDE_VALUE) - 1)
	SPBRG = (((((DWORD)INSTRUCTION_CLOCK_FREQUENCY << 2) / 250000) / 16) - 1) & 0x00ff;		//Note that different BRGH & BRG16 settings use different divide values - see device datasheet

	//SETUP TX
	TXSTAbits.TX9 = 1;					//9 bit tx
	TXSTAbits.TXEN = 0;					//Transmit disabled
	TXSTAbits.SYNC = 0;					//Asynchronous mode
	
	//SETUP RX
	RCSTAbits.RX9 = 1;					//9 bit rx
	RCSTAbits.ADDEN = 0;				//Address detect disabled
	RCSTAbits.CREN = 1;					//Enable receiver
	RCSTAbits.SPEN = 1;					//Serial port enabled

	INTCONbits.PEIE = 1;				//Enable peripheral IRQ's		<<<<< CHECK FOR THIS PIC
	PIE1bits.RCIE = 1;					//Enable the RX IRQ				<<<<< CHECK FOR THIS PIC
//<<<<<

//>>>>> FOR PIC18 EARLIER MODELS e.g. 252 etc
	TXSTA = 0x64;						//TX enabled, 9bit,  BRGH on (b=2 high)
	RCSTA = 0xd0;						//Serial port enabled, rx enabled, 9 bit, addr detect off
	SPBRG = 9;							//Set baud rate to 250K @ 40MHz BRGH=1

	INTCONbits.PEIE = 1;				//Enable peripheral IRQ's
	PIE1bits.RCIE = 1;					//Enable the RX IRQ
//<<<<<
*/

//################################
//##### HEARTBEAT 10mS TIMER #####
//################################
/*
	//----- DMX TIMEOUT TIMER -----
	if (dmx_rx_10mS_timeout_timer)
		dmx_rx_10mS_timeout_timer--;
*/

//#######################
//##### IRQ HANDLER #####
//#######################
/*
	//************************
	//***** USART RX IRQ *****
	//************************
	if(PIE1bits.RCIE && PIR1bits.RCIF)
		dmx_receive();
*/

//###############################
//##### APPLICATION GENERAL #####
//###############################
/*

	//----- SET DMX START ADDRESS -----
	dmx_start_address = 1;			//(1 - 512)

	//----- IS DMX RX ACTIVE? -----
	if (dmx_rx_10mS_timeout_timer)
	{
		//DMX RX IS ACTIVE
	}

	//----- GET RECEVIED DATA -----
	# = dmx_rx[#];

	//----- HAS A NEW FRAME BEEN RECEIVED -----
	if (dmx_new_frame_received)
	{
		dmx_new_frame_received = 0;
	}

*/




//*****************************
//*****************************
//********** DEFINES **********
//*****************************
//*****************************
#ifndef DMX_C_INIT		//Do only once the first time this file is used
#define	DMX_C_INIT


//----- DMX USER OPTIONS -----
#define	DMX_NO_OF_CHANS_TO_RX		20


//----- PROCESSOR SPECIFIC DEFINES -----
#define	DMX_RX_DATA_REGISTER		RCREG
#define	DMX_RX_STATUS_REGISTER		RCSTA
#define	DMX_RX_OERR_BIT				0x02
#define	DMX_RX_FERR_BIT				0x04
#define	DMX_RX_CREN_BIT				RCSTAbits.CREN

#define	DMX_TIMEOUT_10MS_TIME		120


#define	DMX_HEADER_STANDARD			0x00




#endif




//*******************************
//*******************************
//********** FUNCTIONS **********
//*******************************
//*******************************
#ifdef DMX_C
//-----------------------------------
//----- INTERNAL ONLY FUNCTIONS -----
//-----------------------------------
void dmx_rx_reset_port (void);


//-----------------------------------------
//----- INTERNAL & EXTERNAL FUNCTIONS -----
//-----------------------------------------
//(Also defined below as extern)
void dmx_receive (void);


#else
//------------------------------
//----- EXTERNAL FUNCTIONS -----
//------------------------------
extern void dmx_receive (void);


#endif




//****************************
//****************************
//********** MEMORY **********
//****************************
//****************************
#ifdef DMX_C
//--------------------------------------------
//----- INTERNAL ONLY MEMORY DEFINITIONS -----
//--------------------------------------------


//--------------------------------------------------
//----- INTERNAL & EXTERNAL MEMORY DEFINITIONS -----
//--------------------------------------------------
//(Also defined below as extern)
BYTE dmx_rx_10mS_timeout_timer;
WORD dmx_start_address;
BYTE dmx_rx[DMX_NO_OF_CHANS_TO_RX];
BYTE dmx_new_frame_received = 0;


#else
//---------------------------------------
//----- EXTERNAL MEMORY DEFINITIONS -----
//---------------------------------------
extern BYTE dmx_rx_10mS_timeout_timer;
extern WORD dmx_start_address;
extern BYTE dmx_rx[DMX_NO_OF_CHANS_TO_RX];
extern BYTE dmx_new_frame_received;

#endif






