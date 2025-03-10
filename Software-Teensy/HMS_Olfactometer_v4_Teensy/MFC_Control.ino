//  MFC_Control.ino
//  =================
//
//  This file handles the low-level details of communication between
//  the HMS Olfactometer PCB and up to four Sensirion mass flow
//  controllers (MFCs).
//
//  This file isn't meant to be modified by end-users.
//  Instead, you can/should call the following functions in the main body of your code:
//
//  setupMFC(MFCNum)  --  Call this once for each MFC, in the Setup() function
//  float getMFCFlowRate(MFCNum)  --  returns measured flow in LPM
//  setMFCFlowRate(MFCNum, targetFlowRate_LPM)  --  sets target flow rate (in LPM)


//  ==================================================================
//  NOTE: Do not edit this file unless you konw what you are doing.



// make sure that we use the proper definition of NO_ERROR
#ifdef NO_ERROR
#undef NO_ERROR
#endif
#define NO_ERROR 0

SensirionI2cSfx6xxx MFC[4];
bool MFCFound[4];

static char errorMessage[64];
static int16_t error;

void PrintUint64(uint64_t& value) {
    Serial.print("0x");
    Serial.print((uint32_t)(value >> 32), HEX);
    Serial.print((uint32_t)(value & 0xFFFFFFFF), HEX);
}

void setupMFC(uint8_t MFCNum) { // MFCNum is 1-based
	static bool firstTime = true;
	if (firstTime) {
	    Wire1.begin();
	    firstTime = false;

        // Perform SOFT-RESET on *all* MFCs
        // (This seems to help in situations where Teensy was reset after already communicating with MFCs.)
        Wire1.beginTransmission(0x00);
        Wire1.write(0x06);
        Wire1.endTransmission();
        delay(500);
	}

	uint8_t addr;
	if (MFCNum==1) {
		addr = SFC6000_I2C_ADDR_24;
	} else if (MFCNum==2) {
		addr = SFC6000_I2C_ADDR_23;
	} else if (MFCNum==3) {
		addr = SFC6000_I2C_ADDR_22;
	} else if (MFCNum==4) {
		addr = SFC6000_I2C_ADDR_21;
	} else {
		Serial.println("ERROR: Bad MFC number");
		return;
	}

    MFC[MFCNum-1].begin(Wire1, addr);

    uint32_t productIdentifier = 0;
    uint64_t serialNumber = 0;
    error = MFC[MFCNum-1].readProductIdentifier(productIdentifier, serialNumber);
    if (error != NO_ERROR) {
        MFCFound[MFCNum-1] = false;
        Serial.print("Cannot connect to MFC ");
        Serial.print(MFCNum);
        Serial.print("   ### ");
        // Serial.print("Error trying to execute readProductIdentifier(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.print(errorMessage);
        Serial.println(" ###");
        return;
    }
    MFCFound[MFCNum-1] = true;
    Serial.print("Found MFC ");
    Serial.print(MFCNum);
    Serial.print("    (");
    Serial.print("productIdentifier: ");
    Serial.print(productIdentifier);
    Serial.print("\t");
    Serial.print("serialNumber: ");
    PrintUint64(serialNumber);
    Serial.println(")");

    startContinuousMeasurement(MFCNum);
    return;
}

bool startContinuousMeasurement(uint8_t MFCNum) {
    error = MFC[MFCNum-1].startAirContinuousMeasurement();
    if (error != NO_ERROR) {
        Serial.print("MFC ");
        Serial.print(MFCNum);
        Serial.print(": Error starting continuous measurement:  ### ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.print(errorMessage);
        Serial.println(" ###");
        return true;
    }
    return false;
}

float getMFCFlowRate(uint8_t MFCNum) {
    if (!MFCFound[MFCNum-1]) {
        Serial.print("MFC ");
        Serial.print(MFCNum);
        Serial.println(" is not connected/initialized.");
        return -1;
    }
    float aFlow = 0.0;
    error = MFC[MFCNum-1].readFlow(aFlow);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute readFlow(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return -1;
    }
    return(aFlow);
}

bool setMFCFlowRate(uint8_t MFCNum, float targetFlowRate_LPM) {
    if (!MFCFound[MFCNum-1]) {
        Serial.print("MFC ");
        Serial.print(MFCNum);
        Serial.println(" is not connected/initialized.");
        return true;
    }
    if (targetFlowRate_LPM<0) {
        Serial.println("Error: Flow rate must be 0 or greater");
        return true;
    }
    Serial.print("Set flow rate to ");
    Serial.print(targetFlowRate_LPM);
    Serial.println(" LPM");
    error = MFC[MFCNum-1].updateSetpoint(targetFlowRate_LPM);
    if (error != NO_ERROR) {
        Serial.print("Error: ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return true;
    }
    return false;
}


// OTHER MFC COMMANDS....

        //     error = sensor.forceOpenValve();
        //     if (error != NO_ERROR) {
        //         Serial.print("Error: ");
        //         errorToString(error, errorMessage, sizeof errorMessage);
        //         Serial.println(errorMessage);
        //     }
        //     error = sensor.forceCloseValve();
        //     if (error != NO_ERROR) {
        //         Serial.print("Error: ");
        //         errorToString(error, errorMessage, sizeof errorMessage);
        //         Serial.println(errorMessage);
        //     }

        // // A: Put MFC valve in automatic mode (default)
        //     error = sensor.resetForceOpenValve();
        //     if (error != NO_ERROR) {
        //         Serial.print("Error: ");
        //         errorToString(error, errorMessage, sizeof errorMessage);
        //         Serial.println(errorMessage);
        //     }
        //     error = sensor.resetForceCloseValve();
        //     if (error != NO_ERROR) {
        //         Serial.print("Error: ");
        //         errorToString(error, errorMessage, sizeof errorMessage);
        //         Serial.println(errorMessage);
        //     }


