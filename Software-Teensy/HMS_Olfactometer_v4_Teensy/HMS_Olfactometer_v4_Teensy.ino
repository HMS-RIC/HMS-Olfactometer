
#include "Olfactometer_header.h"

// Define Carrier/Odor MFC numbers
// NOTE: The PCB connector (MFC-1..4) determines the MFC number
const int CARRIER_MFC = 1;
const int ODOR_MFC = 2;


void setup() {
  Serial.begin(115200);
  while (!Serial) {
      delay(100);
  }

  // Set up MFCs.
  // Run for each MFC connected to the board.
  // NOTE: The PCB connector (MFC-1..4) determines the MFC number
  setupMFC(CARRIER_MFC);
  setupMFC(ODOR_MFC);

  // Initialize valves.
  // This command will determine which IO-expander chips are wired up.
  InitializeValves();
}


void loop() {

  // Option 1) Act on USB commands:
  // readFromUSB();

  // Option 2) Or just put your main code directly here, to run repeatedly:
  // Typically used for troubleshooting

  delay(1000);
  Serial.println("Turning Odor-1 On/Off");
  activateOdorValve(1);
  delay(500);
  deactivateOdorValve(1);
  delay(500);
  Serial.println("Turning Aux-1 On/Off");
  activateAuxValve(1);
  delay(500);
  deactivateAuxValve(1);


  delay(1000);
  Serial.print("Flow: ");
  Serial.println(getMFCFlowRate(CARRIER_MFC));
  delay(1000);
  setMFCFlowRate(CARRIER_MFC, 1.5); delay(150);
  Serial.print("Flow: ");
  Serial.println(getMFCFlowRate(CARRIER_MFC));
  delay(1000);
  setMFCFlowRate(CARRIER_MFC, 2.5); delay(150);
  Serial.print("Flow: ");
  Serial.println(getMFCFlowRate(CARRIER_MFC));
  delay(1000);
  setMFCFlowRate(CARRIER_MFC, 0); delay(150);
  Serial.print("Flow: ");
  Serial.println(getMFCFlowRate(CARRIER_MFC));

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

  switch (command) {

    // D: changes o[D]or stream flow rate (in mLPM)
    case 'D':
    case 'd':
      setMFCFlowRate(ODOR_MFC, arg1/1000.0);
      delay(250);
      Serial.print("Setting ODOR flow to: ");
      Serial.print(arg1/1000.0);
      Serial.println(" lpm");
      Serial.print("Actual ODOR flow: ");
      Serial.print(getMFCFlowRate(ODOR_MFC));
      Serial.println(" lpm");
      break;

    // R: changes ca[R]rier stream flow rate (in mLPM)
    case 'R':
    case 'r':
      setMFCFlowRate(CARRIER_MFC, arg1/1000.0);
      delay(250);
      Serial.print("Setting CARRIER flow to: ");
      Serial.print(arg1/1000.0);
      Serial.println(" lpm");
      Serial.print("Actual CARRIER flow: ");
      Serial.print(getMFCFlowRate(CARRIER_MFC));
      Serial.println(" lpm");      break;

    // K: chec[K] carrier and odor flow
    case 'K':
    case 'k':
      Serial.print("CARRIER flow: ");
      Serial.print(getMFCFlowRate(CARRIER_MFC));
      Serial.println(" lpm");
      Serial.print("ODOR flow: ");
      Serial.print(getMFCFlowRate(ODOR_MFC));
      Serial.println(" lpm");
      break;


    // A: opens [A]ux valve
    case 'A':
    case 'a':
      activateAuxValve(arg1);
      break;

    // X: close au[X] valve
    case 'X':
    case 'x':
      deactivateAuxValve(arg1);
      break;

    // O: opens odor valve
    case 'O':
    case 'o':
      activateOdorValve(arg1);
      break;

    // C: closes odor valve
    case 'C':
    case 'c':
      deactivateOdorValve(arg1);
      break;



    // otherwise: error
    default:
      Serial.println("#"); // "#" means error
  }
}
