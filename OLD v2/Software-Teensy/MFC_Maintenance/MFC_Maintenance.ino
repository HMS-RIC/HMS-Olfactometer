

//#define OLD_PCB_V2_0

#include "Olfactometer_header.h"




// === SETUP ===
// the setup function runs once when you press reset or power the board
void setup() {

	// start connections with the MFCs (Serial1 / RS485)
	pinMode(RTS_pin, OUTPUT);
	digitalWrite(RTS_pin, LOW); // RTS low
	Serial1.begin(9600);

	// start USB connection with the computer
	Serial.begin(9600);

  // Broadcast version number
  delay(1000);
  Serial.println("MFC Maintenance Utilities");
  Serial.println("Version 3.2");
#ifndef OLD_PCB_V2_0
  Serial.println("Compiled for PCB v2.1");
#else
  Serial.println("Compiled for PCB v2.0");
#endif
  Serial.println("");


  setupMFC(odor_address, 0);
  setupMFC(carrier_address, 0);

	printCommands();

}


// =================
// === MAIN LOOP ===
// =================

// the loop function runs over and over again forever
void loop() {

	readFromUSB();
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


void printMFCStatus() {
	printCarrierMFCStatus();
	printOdorMFCStatus();
}

void printCarrierMFCStatus() {
	Serial.println(F("CARRIER Flow Rate: "));
	Serial.print(getMFCFlowRate(carrier_address));
	Serial.println(F(" LPM"));
	Serial.println("");
}

void printOdorMFCStatus() {
	Serial.println(F("ODOR Flow Rate: "));
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
	long arg1 = -1;
	if (intString.length() > 0){
		arg1 = intString.toInt();
	}

	parameters.trim();
	intString = "";
	while ((parameters.length() > 0) && (isDigit(parameters[0]))) {
		intString += parameters[0];
		parameters.remove(0,1);
	}
	long arg2 = -1;
	if (intString.length() > 0){
		arg2 = intString.toInt();
	}



	switch (command) {
		// @: Reset MFCs
		case '@':
			setupMFC(odor_address, 0);
			setupMFC(carrier_address, 0);
			break;

		// O: Force MFC valve open
		case 'O':
		case 'o':
			if (arg1==1) {
					Serial.println("Odor MFC: Valve set to Open");
					setValveOpen(odor_address);
				} else if (arg1==2) {
					Serial.println("Carrier MFC: Valve set to Open");
					setValveOpen(carrier_address);
				}
				break;

		// C: Force MFC valve closed
		case 'C':
		case 'c':
			if (arg1==1) {
					Serial.println("Odor MFC: Valve set to Closed");
					setValveClosed(odor_address);
				} else if (arg1==2) {
					Serial.println("Carrier MFC: Valve set to Closed");
					setValveClosed(carrier_address);
				}
				break;

		// A: Put MFC valve in automatic mode (default)
		case 'A':
		case 'a':
			if (arg1==1) {
					Serial.println("Odor MFC: Valve set to Auto (default)");
					setValveAuto(odor_address);
				} else if (arg1==2) {
					Serial.println("Carrier MFC: Valve set to Auto (default)");
					setValveAuto(carrier_address);
				}
				break;

		// T: Run MFC autotune
		case 'T':
		case 't':
			if (arg2<=0.0) {
				Serial.println("Error: Autotune requires a flow rate. (Ideally 80-90% of rated flow)");
				break;
			}
			if (arg1==1) {
				Serial.print("Odor MFC: Run Autotune at flow ");
				Serial.print(arg2);
				Serial.println(" mLPM");
				autoTuneMFC(odor_address, arg2/1000.0);
			} else if (arg1==2) {
				Serial.print("Carrier MFC: Run Autotune at flow ");
				Serial.print(arg2);
				Serial.println(" mLPM");
				autoTuneMFC(carrier_address, arg2/1000.0);
			}
			break;

		// S: Report autotune status
		case 'S':
		case 's':
			if (arg1==1) {
				checkAutoTuneStatus(odor_address);
			} else if (arg1==2) {
				checkAutoTuneStatus(carrier_address);
			}
			break;

		case 'V':
		case 'v':
			if (arg1==1) {
				printPIDValues(odor_address);
			} else if (arg1==2) {
				printPIDValues(carrier_address);
			}
			break;

		// D: changes o[D]or stream flow rate (in mLPM)
		case 'D':
		case 'd':
			if (arg1 > -1) {
				changeMFCFlowRate(odor_address, arg1/1000.0);
				delay(5000);
			}
			printOdorMFCStatus();
			break;

		// R: changes ca[R]rier stream flow rate (in mLPM)
		case 'R':
		case 'r':
			if (arg1 > -1) {
				changeMFCFlowRate(carrier_address, arg1/1000.0);
	      delay(5000);
	    }
			printCarrierMFCStatus();
			break;

    // P: [P]rint full carrier and odor MFC Status
    case 'P':
    case 'p':
      printMFCStatus();
      break;

    // ?:
    case '?':
      printCommands();
      break;


    // ### Address Change Commands ###
    case 'W':
    case 'w':
    	initialize_MFC_as_Odor();
    	break;

    case 'X':
    case 'x':
    	initialize_MFC_as_Carrier();
    	break;

    case 'Y':
    case 'y':
    	change_MFC_from_Carrier_to_Odor();
    	break;

    case 'Z':
    case 'z':
    	change_MFC_from_Odor_to_Carrier();
    	break;



		// otherwise: error
		default:
			Serial.println("#"); // "#" means error
	}
}


void printCommands() {
	Serial.println(" COMMAND                DESCRIPTION");
	Serial.println(" O <1/2>                Force MFC valve [O]pen");
	Serial.println(" C <1/2>                Force MFC valve [C]losed");
	Serial.println(" A <1/2>                Put MFC valve in [A]utomatic mode (default)");
	Serial.println(" T <1/2> <flow rate>    Run MFC auto[T]une");
	Serial.println(" S <1/2>                Report autotune [S]tatus");
	Serial.println(" V <1/2>                Print PID [V]alues");
  Serial.println(" D <flow rate>          changes o[D]or stream flow rate (in mLPM)");
	Serial.println(" R <flow rate>          changes ca[R]rier stream flow rate (in mLPM)");
	Serial.println(" P                      Print flow status for both MFCs");
	Serial.println(" @                      Reset MFCs");
	Serial.println("");
	Serial.println(" ** For [O,C,A,T,S] use 1 for Odor-MFC and 2 for Carrier-MFC **");
	Serial.println("");
	Serial.println(" ADDRESS CHANGES");
	Serial.println(" W                      Initialize MFC as Odor");
	Serial.println(" X                      Initialize MFC as Carrier");
	Serial.println(" Y                      Change MFC from Carrier to Odor");
	Serial.println(" Z                      Change MFC from Odor to Carrier");
	Serial.println("");
}


