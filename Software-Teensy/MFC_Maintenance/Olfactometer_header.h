
#include "Arduino.h"

// The Teensy pin assignments differ between PCB versions 2.0 and 2.1
// By default this file will assign correct pins for v2.1.
// If you want the pins for v2.0, you must #define OLD_PCB_V2_0 in Olfactometer_Control_v3.ino



#ifndef OLD_PCB_V2_0
// if OLD_PCB_V2_0 is not defined:

	// =================================
	// Pin Assignments for PCB v2.1

	const uint8_t RTS_pin = 28;

	const uint8_t BNC1_pin = 18;
	const uint8_t BNC2_pin = 19;

	const uint8_t VAux1_pin = 37;
	const uint8_t VAux2_pin = 38;

	const uint8_t V1 = 2;
	const uint8_t V2 = 3;
	const uint8_t V3 = 4;
	const uint8_t V4 = 5;
	const uint8_t V5 = 6;
	const uint8_t V6 = 7;
	const uint8_t V7 = 8;
	const uint8_t V8 = 9;
	const uint8_t V9 = 10;
	const uint8_t V10 = 11;
	const uint8_t V11 = 12;
	const uint8_t V12 = 13;
	const uint8_t V13 = 14;
	const uint8_t V14 = 15;
	const uint8_t V15 = 16;
	const uint8_t V16 = 17;

	const uint8_t V17 = 20;
	const uint8_t V18 = 21;
	const uint8_t V19 = 22;
	const uint8_t V20 = 23;
	const uint8_t V21 = 24;
	const uint8_t V22 = 25;
	const uint8_t V23 = 26;
	const uint8_t V24 = 27;
	const uint8_t V25 = 29;
	const uint8_t V26 = 30;
	const uint8_t V27 = 31;
	const uint8_t V28 = 32;
	const uint8_t V29 = 33;
	const uint8_t V30 = 34;
	const uint8_t V31 = 35;
	const uint8_t V32 = 36;

	const uint8_t V33 = 39;
	const uint8_t V34 = 40;
	const uint8_t V35 = 41;
	const uint8_t V36 = 42;
	const uint8_t V37 = 43;
	const uint8_t V38 = 44;
	const uint8_t V39 = 45;
	const uint8_t V40 = 46;
	const uint8_t V41 = 47;
	const uint8_t V42 = 48;
	const uint8_t V43 = 49;
	const uint8_t V44 = 50;
	const uint8_t V45 = 51;
	const uint8_t V46 = 52;
	const uint8_t V47 = 53;
	const uint8_t V48 = 54;

	const uint8_t V49 = 55;
	const uint8_t V50 = 56;
	const uint8_t V51 = 57;

#else // else if OLD_PCB_V2_0 is defined:

	// =================================
	// Pin Assignments for PCB v 2.0

	const uint8_t RTS_pin = 38;	// NOTE: This is a hack to fix lack of RTS pin on PCB v2.0.
							//		To be fixed in PCB v2.1
	const uint8_t BNC1_pin = 18;
	const uint8_t BNC2_pin = 19;

	const uint8_t VAux1_pin = 36;
	const uint8_t VAux2_pin = 37;

	const uint8_t V1 = 2;
	const uint8_t V2 = 3;
	const uint8_t V3 = 4;
	const uint8_t V4 = 5;
	const uint8_t V5 = 6;
	const uint8_t V6 = 7;
	const uint8_t V7 = 8;
	const uint8_t V8 = 9;
	const uint8_t V9 = 10;
	const uint8_t V10 = 11;
	const uint8_t V11 = 12;
	const uint8_t V12 = 13;
	const uint8_t V13 = 14;
	const uint8_t V14 = 15;
	const uint8_t V15 = 16;
	const uint8_t V16 = 17;

	const uint8_t V17 = 20;
	const uint8_t V18 = 21;
	const uint8_t V19 = 22;
	const uint8_t V20 = 23;
	const uint8_t V21 = 24;
	const uint8_t V22 = 25;
	const uint8_t V23 = 26;
	const uint8_t V24 = 27;
	const uint8_t V25 = 28;
	const uint8_t V26 = 29;
	const uint8_t V27 = 30;
	const uint8_t V28 = 31;
	const uint8_t V29 = 32;
	const uint8_t V30 = 33;
	const uint8_t V31 = 34;
	const uint8_t V32 = 35;

	const uint8_t V33 = 38;
	const uint8_t V34 = 39;
	const uint8_t V35 = 40;
	const uint8_t V36 = 41;
	const uint8_t V37 = 42;
	const uint8_t V38 = 43;
	const uint8_t V39 = 44;
	const uint8_t V40 = 45;
	const uint8_t V41 = 46;
	const uint8_t V42 = 47;
	const uint8_t V43 = 48;
	const uint8_t V44 = 49;
	const uint8_t V45 = 50;
	const uint8_t V46 = 51;
	const uint8_t V47 = 52;
	const uint8_t V48 = 53;

	const uint8_t V49 = 54;
	const uint8_t V50 = 55;
	const uint8_t V51 = 56;

#endif // OLD_PCB_V2_0


const uint8_t MAX_NUM_VALVES = 51;
const uint8_t all_valves[] = {
	VAux1_pin, VAux2_pin, // Aux1 and Aux2 are indecies 0 and 1
	V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12, V13, V14, V15, V16,
 	V17, V18, V19, V20, V21, V22, V23, V24, V25, V26, V27, V28, V29, V30, V31, V32,
	V33, V34, V35, V36, V37, V38, V39, V40, V41, V42, V43, V44, V45, V46, V47, V48,
	V49, V50, V51
	};


// =======================================
//     MFC Addresses
// (DO NOT CHANGE THESE)
const char default_address[] = "11";
const char carrier_address[] = "02";
const char odor_address[] = "04";

