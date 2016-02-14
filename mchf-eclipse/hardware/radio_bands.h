/************************************************************************************
**                                                                                 **
**                               mcHF QRP Transceiver                              **
**                             K Atanassov - M0NKA 2014                            **
**                                                                                 **
**---------------------------------------------------------------------------------**
**                                                                                 **
**  File name:                                                                     **
**  Description:                                                                   **
**  Last Modified:                                                                 **
**  Licence:		For radio amateurs experimentation, non-commercial use only!   **
************************************************************************************/

#ifndef __RADIO_BANDS_H
#define __RADIO_BANDS_H

#include "stm32f4xx_gpio.h"
#include "mchf_types.h"

// Total bands supported
//
#define	MIN_BANDS			0		// lowest band number
#define	MAX_BANDS			17		// Highest band number:  17 = General coverage (RX only) band

#define	KHZ_MULT			4000	// multiplier to convert oscillator frequency or band size to display kHz, used below
//
// Bands definition
// - ID
// - SI570 startup freq
// - size in Hz
//
#define	BAND_MODE_80			0
#define	BAND_FREQ_80			3500*KHZ_MULT		// 3500 kHz
#define	BAND_SIZE_80			500*KHZ_MULT		// 500 kHz in size (Region 2)
//
#define	BAND_MODE_60			1
#define	BAND_FREQ_60			5250*KHZ_MULT		// 5250 kHz
#define	BAND_SIZE_60			200*KHZ_MULT		// 200 kHz in size to allow different allocations
//
#define	BAND_MODE_40			2
#define	BAND_FREQ_40			7000*KHZ_MULT		// 7000 kHz
#define	BAND_SIZE_40			300*KHZ_MULT		// 300 kHz in size (Region 2)
//
#define	BAND_MODE_30			3
#define	BAND_FREQ_30			10100*KHZ_MULT		// 10100 kHz
#define	BAND_SIZE_30			50*KHZ_MULT		// 50 kHz in size
//
#define	BAND_MODE_20			4
#define	BAND_FREQ_20			14000*KHZ_MULT		// 14000 kHz
#define	BAND_SIZE_20			350*KHZ_MULT		// 350 kHz in size
//
#define	BAND_MODE_17			5
#define	BAND_FREQ_17			18068*KHZ_MULT		// 18068 kHz
#define	BAND_SIZE_17			100*KHZ_MULT		// 100 kHz in size
//
#define	BAND_MODE_15			6
#define	BAND_FREQ_15			21000*KHZ_MULT		// 21000 kHz
#define	BAND_SIZE_15			450*KHZ_MULT		// 450 kHz in size
//
#define	BAND_MODE_12			7
#define	BAND_FREQ_12			24890*KHZ_MULT		// 24890 kHz
#define	BAND_SIZE_12			100*KHZ_MULT		// 100 kHz in size
//
#define	BAND_MODE_10			8
#define	BAND_FREQ_10			28000*KHZ_MULT		// 28000 kHz
#define	BAND_SIZE_10			1700*KHZ_MULT		// 1700 kHz in size
//
#define	BAND_MODE_6			9
#define	BAND_FREQ_6			50000*KHZ_MULT		// 50000 kHz
#define	BAND_SIZE_6			2000*KHZ_MULT		// 2000 kHz in size (Region 2)
//
#define	BAND_MODE_4			10
#define	BAND_FREQ_4			70000*KHZ_MULT		// 70000 kHz
#define	BAND_SIZE_4			500*KHZ_MULT		// 500 kHz in size (Region 2)
//
#define	BAND_MODE_2			11
#define	BAND_FREQ_2			144000*KHZ_MULT		// 144000 kHz
#define	BAND_SIZE_2			2000*KHZ_MULT		// 2000 kHz in size (Region 1)
//
#define	BAND_MODE_70			12
#define	BAND_FREQ_70			430000*KHZ_MULT		// 430000 kHz
#define	BAND_SIZE_70			10000*KHZ_MULT		// 10000 kHz in size (Region 1)
//
#define	BAND_MODE_23			13
#define	BAND_FREQ_23			450000*KHZ_MULT		// 1240000 kHz
#define	BAND_SIZE_23			10000*KHZ_MULT		// 60000 kHz in size (Region 1)
//
#define	BAND_MODE_2200			14
#define	BAND_FREQ_2200			135.7*KHZ_MULT		// 135.7 kHz
#define	BAND_SIZE_2200			2.1*KHZ_MULT		// 2.1 kHz in size (Region 1)
//
#define	BAND_MODE_630			15
#define	BAND_FREQ_630			472*KHZ_MULT		// 472 kHz
#define	BAND_SIZE_630			7*KHZ_MULT		// 7 kHz in size (Region 1)
//
#define	BAND_MODE_160			16
#define	BAND_FREQ_160			1800*KHZ_MULT		// 1810 kHz
#define	BAND_SIZE_160			190*KHZ_MULT		// 190 kHz in size (Region 1)
//
#define	BAND_MODE_GEN			17			// General Coverage
#define	BAND_FREQ_GEN			10000*KHZ_MULT		// 10000 kHz
#define	BAND_SIZE_GEN			1*KHZ_MULT		// Dummy variable

// Band definitions - band frequency size
const ulong size_bands[MAX_BANDS];

// Band definitions - band base frequency value
const ulong tune_bands[MAX_BANDS];

#endif