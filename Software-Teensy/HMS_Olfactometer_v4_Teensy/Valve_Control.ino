

// Array of all 7 potntial valve banks
Adafruit_MCP23X08 GPIO_Expander[7];

// Boolean array of which valve banks exist
bool ValveBanksFound[7];

void InitializeValves() {
	for (uint8_t bankNum=0; bankNum<7; bankNum++) {
		// attempt I2C communication to each valve potential bank
		bool success = GPIO_Expander[bankNum].begin_I2C(0x20+bankNum);  	// adresses are 0x20 - 0x27
		ValveBanksFound[bankNum] = success;
		if (success) {
			Serial.print("Found valve bank ");
			Serial.println(bankNum+1);
			// set all pins to output mode
			for (uint8_t chanNum=0; chanNum<8; chanNum++) {
				GPIO_Expander[bankNum].pinMode(chanNum, OUTPUT);
			}
		}
	}
}


bool activateOdorValve(uint8_t odorValveNum) { // odorValveNum is 1-based
	if ((odorValveNum == 0) || (odorValveNum > MAX_NUM_ODOR_VALVES)) {
		Serial.println("ERROR: Odor valve number must be in range 1-48");
		return true;
	}
	bool error = activateValve_core(odorValveNum-1);
	return error;
}

bool deactivateOdorValve(uint8_t odorValveNum) { // odorValveNum is 1-based
	if ((odorValveNum == 0) || (odorValveNum > MAX_NUM_ODOR_VALVES)) {
		Serial.println("ERROR: Odor valve number must be in range 1-48");
		return true;
	}
	bool error = deactivateValve_core(odorValveNum-1);
	return error;
}

bool activateAuxValve(uint8_t auxValveNum) { // auxValveNum is 1-based
	if ((auxValveNum == 0) || (auxValveNum > MAX_NUM_AUX_VALVES)) {
		Serial.println("ERROR: Aux valve number must be in range 1-8");
		return true;
	}
	bool error = activateValve_core(auxValveNum + MAX_NUM_ODOR_VALVES - 1);
	return error;
}

bool deactivateAuxValve(uint8_t auxValveNum) { // auxValveNum is 1-based
	if ((auxValveNum == 0) || (auxValveNum > MAX_NUM_AUX_VALVES)) {
		Serial.println("ERROR: Aux valve number must be in range 1-8");
		return true;
	}
	bool error = deactivateValve_core(auxValveNum + MAX_NUM_ODOR_VALVES - 1);
	return error;
}



// ====================================
// DO NOT CALL THESE FUNCTIONS DIRECTLY

bool activateValve_core(uint8_t rawValveNum) { // rawValveNum is 0-based
	bool error = false;
	// valves are grouped into banks of 8
	// within each bank, valves are in reverse order (e.g. rawValve-0 is [Bank-0, Channel-7])
	// below we compute the bankNum and chanNum given a rawValveNum
	uint8_t bankNum = rawValveNum/8;
	uint8_t chanNum = 7 - (rawValveNum - (8*bankNum));
	if (ValveBanksFound[bankNum]) {
		GPIO_Expander[bankNum].digitalWrite(chanNum, HIGH);
		if (DEBUG) {
			Serial.print("HIGH: Bank-");
			Serial.print(bankNum);
			Serial.print(" Chan-");
			Serial.println(chanNum);
		}
	} else {
		Serial.print("ERROR: Cannot find valve bank");
		Serial.println(bankNum);
		error = true;
	}
	return(error);
}

bool deactivateValve_core(uint8_t rawValveNum) { // rawValveNum is 0-based
	bool error = false;
	uint8_t bankNum = rawValveNum/8;
	uint8_t chanNum = 7 - (rawValveNum - (8*bankNum));
	if (ValveBanksFound[bankNum]) {
		GPIO_Expander[bankNum].digitalWrite(chanNum, LOW);
		if (DEBUG) {
			Serial.print("LOW: Bank-");
			Serial.print(bankNum);
			Serial.print(" Chan-");
			Serial.println(chanNum);
		}
	} else {
		Serial.print("ERROR: Cannot find valve bank");
		Serial.println(bankNum);
		error = true;
	}
	return(error);
}

// ====================================

