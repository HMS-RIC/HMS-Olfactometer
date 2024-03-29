
// NOTE: The Teensy pin assignments differ between PCB versions 2.0 and 2.1
	// For PCB v2.1, comment out the following #define line
	// For PCB v2.0, leave the #define uncommented
// #define OLD_PCB_V2_0 // when uncommented, there should be no spaces before the "#define"

#include "Olfactometer_header.h"

// User-modifiable global settings:
// =================================

const bool BNC1_OUTPUT = false; // set to true/false for BNC1 to be output/input
								// Must be false (input) when used to trigger a pre-programmed valve sequence.

const bool BNC2_OUTPUT = true; // set to true/false for BNC2 to be output/input

const bool ONE_VALVE_OPEN = true; // When true, exactly one valve is open at a time. Value 1 is open by default.
								  // When false, every value openeing and closing is explicitly controlled by user.

const bool TRIGGER_WHEN_OPEN = true; // Outputs a TTL pulse on BNC2 every time a valve is open (besides valve 1).
									 // Will only work if BNC2_OUTPUT and ONE_VALVE_OPEN are also true.
									 // This disables the control of BNC2 over USB commands.

const bool DISPLAY_VALVE_STATUS = true; // Write all valve openings/closing to USB
const bool DISPLAY_BNC_STATUS = true; // Write all BNC pulses to USB

const bool VERBOSE_OUTPUT = true;   // if true:  send user readable output over USB                   (e.g., "Valve 3 CLOSED")
									// if false: send coded output, e.g., for Matlab to interpret     (e.g., "C 3" )

const int NUM_VALVES = 32; // actual number of valves wired up

float CARRIER_FLOW_RATE = 1.0; // in LPM
float ODOR_FLOW_RATE    = 0.1; // in LPM


// ======== Do not modify these =======
volatile bool BNC1_flag = false;

bool isRunning = false;
const int MAX_PROG_LEN = 2048;
int prog_index = 0;   // index to next command
int prog_len = 0;     // num commands in program
elapsedMicros timeSinceLastCmd_us = 0;
unsigned long currentCmdDuration_us = 0;
// Program contents:
uint8_t  cmd_list         [MAX_PROG_LEN]; // command (open, close, etc)
uint8_t  valve_list       [MAX_PROG_LEN]; // valve num
uint32_t duration_ms_list [MAX_PROG_LEN]; // duration in ms

#define CMD_NOOP  (0)
#define CMD_OPEN  (1)
#define CMD_CLOSE (2)
#define CMD_BNC_ON (3)
#define CMD_BNC_OFF (4)


// =================================

void DEBUG(String message) {
	//Serial.println(message.c_str()); // comment this out when not debugging !!!!!!!
}


// === SETUP ===
// the setup function runs once when you press reset or power the board
void setup() {

	// initialize valves
	setupValves();

	// Set up pins for BNC input/output
	if (BNC1_OUTPUT) {
		pinMode(BNC1_pin, OUTPUT);
		digitalWrite(BNC1_pin, LOW); // BNC1 low
	} else {
		pinMode(BNC1_pin, INPUT);
		attachInterrupt(digitalPinToInterrupt(BNC1_pin), BNC1Trigger, RISING);
	}
	if (BNC2_OUTPUT) {
		pinMode(BNC2_pin, OUTPUT);
		digitalWrite(BNC2_pin, LOW); // BNC2 low
	} else {
		pinMode(BNC2_pin, INPUT);
	}

	// start connections with the MFCs (Serial1 / RS485)
	pinMode(RTS_pin, OUTPUT);
	digitalWrite(RTS_pin, LOW); // RTS low
	Serial1.begin(9600);

	// start USB connection with the computer
	Serial.begin(9600);

	// Broadcast version number
	delay(1000);
	Serial.println("HMS_Olfactometer_Controller");
	Serial.println("Version 3.2");
#ifndef OLD_PCB_V2_0
	Serial.println("Compiled for PCB v2.1");
#else
	Serial.println("Compiled for PCB v2.0");
#endif
	Serial.println("");

	// if communicating w/Matlab
	if (!VERBOSE_OUTPUT) {
		delay(100);
		Serial.println('S');
	}

	// Setup MFCs
	delay(3000); // wait until MFCs are active and ready for input...
	setupMFC(carrier_address, CARRIER_FLOW_RATE); // set carrier stream to 1 LPM
	setupMFC(odor_address, ODOR_FLOW_RATE);   // set odor stream to 0.1 LPM
	delay(3000);

	if (VERBOSE_OUTPUT) {
		printMFCStatus();
	}
}


// =================
// === MAIN LOOP ===
// =================

// the loop function runs over and over again forever
void loop() {

	readFromUSB();

	if (BNC1_flag) {
		BNC1_flag = false;
		if (!isRunning) {
			startProgram();     // trigger program
		}
	}

	if (isRunning) {
		updateProgram();
	}

	updateValves();
}

void startProgram() {
	isRunning = true;
	prog_index = 0;
	USB_BeginProgram();
	runCommand();
}

void updateProgram() {
	if (timeSinceLastCmd_us >= currentCmdDuration_us) {
		runCommand();
	}
}

void runCommand() {
	if (prog_index == prog_len) {
		isRunning = false;
		prog_index = 0;
		USB_EndProgram();
		return;
	}
	timeSinceLastCmd_us = 0;
	currentCmdDuration_us = duration_ms_list[prog_index]*1000;
	if (cmd_list[prog_index] == CMD_OPEN) {
		OpenValve(valve_list[prog_index]);

	} else if (cmd_list[prog_index] == CMD_CLOSE) {
		CloseValve(valve_list[prog_index]);

	} else if (cmd_list[prog_index] == CMD_BNC_ON) {
		BNC_Pulse_On(valve_list[prog_index]);

	} else if (cmd_list[prog_index] == CMD_BNC_OFF) {
		BNC_Pulse_Off(valve_list[prog_index]);
	}
	prog_index += 1;
}

void readFromUSB() {
	// read from USB, if available
	static String usbMessage = ""; // initialize usbMessage to empty string,
																 // happens once at start of program
	if (Serial.available() > 0) {
		// read next char if available
		char inByte = Serial.read();
		if ((inByte == '\n') || (inByte == ';')){
			// the new-line character ('\n') or the ';' character
			// indicate a complete message.
			// so interprete the message and then clear buffer
			interpretUSBMessage(usbMessage);
			usbMessage = ""; // clear message buffer
		} else {
			// append character to message buffer
			usbMessage = usbMessage + inByte;
		}
	}
}

void BNC1Trigger() {
	BNC1_flag = true;
}

void printMFCStatus() {
	Serial.println(F("CARRIER Flow Rate: "));
	Serial.print(F(" Set to: "));
	Serial.print(CARRIER_FLOW_RATE);
	Serial.println(F(" LPM"));
	Serial.print(F(" Actual: "));
	Serial.print(getMFCFlowRate(carrier_address));
	Serial.println(F(" LPM"));
	Serial.println("");

	Serial.println(F("ODOR Flow Rate: "));
	Serial.print(F(" Set to: "));
	Serial.print(ODOR_FLOW_RATE);
	Serial.println(F(" LPM"));
	Serial.print(F(" Actual: "));
	Serial.print(getMFCFlowRate(odor_address));
	Serial.println(F(" LPM"));
	Serial.println("");
}


void interpretUSBMessage(String message) {
	message.trim(); // remove leading and trailing white space
	int len = message.length();
	if (len==0) {
		Serial.print("#"); // "#" means error
		return;
	}
	char command = message[0]; // the command is the first char of a message
	String parameters = message.substring(1);
	parameters.trim();

	String intString = "";
	while ((parameters.length() > 0) && (isDigit(parameters[0]))) {
		intString += parameters[0];
		parameters.remove(0,1);
	}
	long arg1 = intString.toInt();

	parameters.trim();
	intString = "";
	while ((parameters.length() > 0) && (isDigit(parameters[0]))) {
		intString += parameters[0];
		parameters.remove(0,1);
	}
	long arg2 = intString.toInt();


	DEBUG(String("Command: ")+command);
	DEBUG(String("Argument 1: ")+arg1);
	DEBUG(String("Argument 2: ")+arg2);

	switch (command) {

		// A: [A]bort program
		case 'A':
		case 'a':
			if (isRunning) {
				isRunning = false;
				prog_index = 0;
				if (ONE_VALVE_OPEN) {
					CloseValve(0);
				}
			}
			break;

		// X: erase program
		case 'X':
		case 'x':
			if (!isRunning) {
				prog_index = 0;
				prog_len = 0;
			}
			break;

		// P: [P]rint program
		case 'P':
		case 'p':
			USB_PrintProg();
			break;

		// T: [T]rigger program
		case 'T':
		case 't':
			if (!isRunning) {
				startProgram();     // trigger program
			}
			break;

		// O: add valve [O]pen command
		case 'O':
		case 'o':
			if (!isRunning) {
				// check we don't exceed max commands
				if (prog_len == MAX_PROG_LEN) {
					USB_Error_MaxProgLen();
					break;
				}
				cmd_list[prog_len] = CMD_OPEN;
				valve_list[prog_len] = arg1;
				duration_ms_list[prog_len] = arg2;
				prog_len += 1;
			}
			break;

		// C: add valve [C]lose command
		case 'C':
		case 'c':
			if (!isRunning) {
				// check we don't exceed max commands
				if (prog_len == MAX_PROG_LEN) {
					USB_Error_MaxProgLen();
					break;
				}
				cmd_list[prog_len] = CMD_CLOSE;
				valve_list[prog_len] = arg1;
				duration_ms_list[prog_len] = arg2;
				prog_len += 1;
			}
			break;

		// B: initiate [B]NC pulse
		case 'B':
		case 'b':
			if (!isRunning) {
				// check we don't exceed max commands
				if (prog_len == MAX_PROG_LEN) {
					USB_Error_MaxProgLen();
					break;
				}
				cmd_list[prog_len] = CMD_BNC_ON;
				valve_list[prog_len] = arg1;
				duration_ms_list[prog_len] = arg2;
				prog_len += 1;
			}
			break;

		// E: [E]nd BNC pulse
		case 'E':
		case 'e':
			if (!isRunning) {
				// check we don't exceed max commands
				if (prog_len == MAX_PROG_LEN) {
					USB_Error_MaxProgLen();
					break;
				}
				cmd_list[prog_len] = CMD_BNC_OFF;
				valve_list[prog_len] = arg1;
				duration_ms_list[prog_len] = arg2;
				prog_len += 1;
			}
			break;


		// D: changes o[D]or stream flow rate (in mLPM)
		case 'D':
		case 'd':
			changeMFCFlowRate(odor_address, arg1/1000.0);
			delay(1000);
			USB_CarrierFlowRate(getMFCFlowRate(carrier_address));
			break;

		// R: changes ca[R]rier stream flow rate (in mLPM)
		case 'R':
		case 'r':
			changeMFCFlowRate(carrier_address, arg1/1000.0);
			USB_CarrierFlowRate(getMFCFlowRate(carrier_address));
			break;

		// S/s: start connection to Matlab
		case 'S':
		case 's':
			if (VERBOSE_OUTPUT) {
				printMFCStatus();
			} else {
				Serial.println('S');
			}
			break;

		// otherwise: error
		default:
			Serial.println("#"); // "#" means error
	}
}

// Commands in use: A, B, C, D, E, O, P, R, S, T, X

