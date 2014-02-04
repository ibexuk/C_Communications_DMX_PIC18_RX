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



#include "main.h"					//Global data type definitions (see https://github.com/ibexuk/C_Generic_Header_File )
#define	DMX_C
#include "dmx-rx.h"






//**********************************
//**********************************
//********** DMX RECEIVE ***********
//**********************************
//**********************************
void dmx_receive (void)					//<<If you get problems with your PIC not being fast enough to receive DMX (its very fast for some PIC18's) then taking this functions code and putting it directly in the interrupt handler will avoid the call to here and the time this adds for the C compiler related tasks when calling a function
{
	static BYTE rx_status;			//static as it can help to avoid the irq use the general ram stack
	static BYTE rx_data;			//static as it can help to avoid the irq use the general ram stack
	static WORD dmx_rx_channel_number;
	static BYTE waiting_for_header = 1;
	static BYTE waiting_for_break = 1;

	while (PIE3bits.RC2IE && PIR3bits.RC2IF)		//Keep doing if there is more RX (helps avoid overflow of the buffer)
	{
		//----- GET RECEIVED BYTE -----
		rx_status = DMX_RX_STATUS_REGISTER;
		rx_data = DMX_RX_DATA_REGISTER;

		//----- PROCESS RX -----
		if (rx_status & DMX_RX_OERR_BIT)
		{
			//-------------------------------
			//----- OVERRUN HAS OCCURED -----
			//-------------------------------
			dmx_rx_reset_port();
			waiting_for_break = 1;
		}
		else if (rx_status & DMX_RX_FERR_BIT)
		{
			//FRAMING ERROR - IF DATA IS ZERO THEN ERROR IS A BREAK
			if (rx_data == 0)
			{
				//--------------------------
				//----- BREAK RECEIVED -----
				//--------------------------
				waiting_for_break = 0;
				waiting_for_header = 1;				//Flag waiting for header

				dmx_rx_channel_number = 1;			//Reset the channel counter
			}
			else
			{
				//-------------------------------------
				//----- FRAMING ERROR HAS OCCURED -----
				//-------------------------------------
				dmx_rx_reset_port();
				waiting_for_break = 1;
			}
		}
		else if (waiting_for_header)
		{
			//---------------------------
			//----- HEADER RECEIVED -----
			//---------------------------
			if (rx_data == DMX_HEADER_STANDARD)
			{
				//------------------------------------
				//----- STANDARD HEADER RECEIVED -----
				//------------------------------------
				waiting_for_header = 0;
			}
			
			//<<<<< PUT ANY OTHER HEADER TYPES HERE

			else
			{
				//-----------------------------------
				//----- INVALID HEADER RECEIVED -----
				//-----------------------------------
				waiting_for_break = 1;			//Non recognised header - ignore packet
			}
		}
		else if (waiting_for_break)
		{
			//------------------------------------
			//----- WE ARE WAITING FOR BREAK -----
			//------------------------------------

			//Ignore rx until break is received

		}
		else
		{
			//-------------------------
			//----- DATA RECEIVED -----
			//-------------------------

			//RESET THE TIMEOUT TIMER
			dmx_rx_10mS_timeout_timer = DMX_TIMEOUT_10MS_TIME;

			//GET THE DATA BYTE IF IN OUR CHANNEL RANGE
			if (
				(dmx_rx_channel_number >= dmx_start_address) &&
				(dmx_rx_channel_number < (dmx_start_address + DMX_NO_OF_CHANS_TO_RX))
				)
			{
				//THIS CHANNEL IS IN OUR RX WINDOW
				dmx_rx[dmx_rx_channel_number - dmx_start_address] = rx_data;
			}

			//FLAG NEW FRAME RECEVIED IF WE HAVE RECEIVED ALL THE CHANNELS WE WANT TO RECEIVE
			if (dmx_rx_channel_number == (dmx_start_address + DMX_NO_OF_CHANS_TO_RX - 1))
				dmx_new_frame_received = 1;

			//INCREMENT THE CHANNEL NUMBER READY FOR THE NEXT BYTE
			dmx_rx_channel_number++;
			if (dmx_rx_channel_number > 512)
			{
				waiting_for_break = 1;
			}

		}
	} //while (PIE3bits.RC2IE && PIR3bits.RC2IF)

}



//***************************************
//***************************************
//********** DMX RX RESET PORT **********
//***************************************
//***************************************
void dmx_rx_reset_port (void)
{
	BYTE temp;


	//Clear the RX FIFO
	temp = DMX_RX_DATA_REGISTER;
	temp = DMX_RX_DATA_REGISTER;

	//Reset the receiver
	DMX_RX_CREN_BIT = 0;
	DMX_RX_CREN_BIT = 1;


}













