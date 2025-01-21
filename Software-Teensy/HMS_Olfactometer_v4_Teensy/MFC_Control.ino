
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
        Serial.print("Error trying to execute readProductIdentifier(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }
    Serial.print("productIdentifier: ");
    Serial.print(productIdentifier);
    Serial.print("\t");
    Serial.print("serialNumber: ");
    PrintUint64(serialNumber);
    Serial.println();
    error = MFC[MFCNum-1].startAirContinuousMeasurement();
    if (error != NO_ERROR) {
        Serial.print(
            "Error trying to execute startAirContinuousMeasurement(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }

}

float getMFCFlowRate(uint8_t MFCNum) {
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


