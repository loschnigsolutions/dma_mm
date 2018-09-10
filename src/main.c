/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include "Timer.h"
#include "BlinkLed.h"

// ----------------------------------------------------------------------------
//
// Standalone STM32F4 led blink sample (trace via DEBUG).
//
// In debug configurations, demonstrate how to print a greeting message
// on the trace device. In release configurations the message is
// simply discarded.
//
// Then demonstrates how to blink a led with 1 Hz, using a
// continuous loop and SysTick delays.
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- Timing definitions -------------------------------------------------

// Keep the LED on for 2/3 of a second.
#define BLINK_ON_TICKS  (TIMER_FREQUENCY_HZ * 3 / 4)
#define BLINK_OFF_TICKS (TIMER_FREQUENCY_HZ - BLINK_ON_TICKS)

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

#define getbit(n,b)		((n  &  (1 << b)) >> b)

int
main(int argc, char* argv[])
{
	volatile uint32_t a[4096];
	volatile uint32_t b[4096];

	volatile int i;

	DMA_Stream_TypeDef *d2 = DMA2_Stream0;

	RCC->AHB1ENR	|=	RCC_AHB1ENR_DMA2EN;

	for(i = 0;i < 6; i++)
	{
		trace_printf("Values in b[%d]: %d\n",i,b[i]);
	}

	for(i = 0;i < 4096; i++)
	{
		a[i] = i;	//4096 - i;
	}

	//Direction Mem to Mem

	d2->CR	|=	(1 << 7);


	//Mem_inc  Pher_inc

	d2->CR	|=	(1 << 9)|(1 << 10);


	d2->NDTR = 4096;
	//d2->NDTR = 10;


	//Source and Destination Addresses

	d2->PAR		= a;
	d2->M0AR	= b;

	trace_printf("FCR: %d%d%d\n",getbit(d2->FCR,5),getbit(d2->FCR,4),getbit(d2->FCR,3));

	d2->CR	|=	(1 << 0);

	trace_printf("FCR: %d%d%d\n",getbit(d2->FCR,5),getbit(d2->FCR,4),getbit(d2->FCR,3));

	while(getbit(d2->CR,0));

	trace_printf("FCR: %d%d%d\n",getbit(d2->FCR,5),getbit(d2->FCR,4),getbit(d2->FCR,3));

	for(i = 0;i < 6; i++)
	{
		trace_printf("b[%d]:  %d\n",i,b[i]);
	}

	while(1);

}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
