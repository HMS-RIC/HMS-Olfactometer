// =======================================
// Valve control functions

// For valve PWM
uint8_t valve_status_list[MAX_NUM_VALVES];
#define VALVE_CLOSED (0)
#define VALVE_NO_PWM (1)
#define VALVE_PWM    (2)
#define VALVE_OPEN_1 (3)
#define VALVE_OPEN_2 (4)


// FOR DEBUGGING
// #define digitalWrite(pin, val) digitalWrite((pin), (val)); Serial.print("DWrite "); Serial.print((pin)); Serial.print(" "); Serial.println((val))
// #define analogWrite(pin, val) analogWrite((pin), (val)); Serial.print("AWrite "); Serial.print((pin)); Serial.print(" "); Serial.println((val))

void setupValves() {
	// Set up pins for valve control
	for (int i=0; i<MAX_NUM_VALVES; i++) {
		pinMode(all_valves[i], OUTPUT);
		digitalWrite(all_valves[i], LOW);
		valve_status_list[i] = VALVE_CLOSED;
	}
	pinMode(VAux1_pin, OUTPUT);
	digitalWrite(VAux1_pin, LOW);
	pinMode(VAux2_pin, OUTPUT);
	digitalWrite(VAux2_pin, LOW);
	if (ONE_VALVE_OPEN) {
		digitalWrite(V1, HIGH);
		valve_status_list[0] = VALVE_OPEN_2;
	}
	// set PWM frequency
	// (many pins share the same clock, so we only
	//  need to set for pins 2,3,5,29 on Teensy3.5.)
	// (Ref: https://www.pjrc.com/teensy/td_pulse.html)
	analogWriteFrequency(2, 234375);
	analogWriteFrequency(3, 234375);
	analogWriteFrequency(5, 234375);
	analogWriteFrequency(29, 234375);
}

int current_valve = 1;
void OpenValve(int valveNum) {
	// error checking
	if ((valveNum > NUM_VALVES) || (valveNum == 0)) {
		Serial.print("Error: Bad valve number ("); Serial.print(valveNum); Serial.println(").");
		return;
	}
	if (valve_status_list[valveNum-1] > VALVE_CLOSED) {
		Serial.print("Valve "); Serial.print(valveNum); Serial.println("already open.");
		return;
	}
	if ((ONE_VALVE_OPEN) && (valveNum==1)) {
	 	Serial.println("Warning: Valve 1 cannot be directly opened in 'ONE_VALVE_OPEN' mode.");
	 	return;
	}

	// open the valve
	digitalWrite(all_valves[valveNum-1], HIGH);
	valve_status_list[valveNum-1] = VALVE_OPEN_2;
	// close the currenly open valve, if necessary
	if (ONE_VALVE_OPEN) {
		digitalWrite(all_valves[current_valve-1], LOW);
		if (valve_status_list[current_valve-1] == VALVE_PWM) {
			analogWrite(all_valves[current_valve-1], 0);
		}
		valve_status_list[current_valve-1] = VALVE_CLOSED;

		if (DISPLAY_VALVE_STATUS && (current_valve>1)) {
			Serial.print("Valve ");
			Serial.print(current_valve);
			Serial.println(" CLOSED.");
		}
		if (TRIGGER_WHEN_OPEN && BNC2_OUTPUT) {
			digitalWrite(BNC2_pin, HIGH);
		}
	}
	if (ONE_VALVE_OPEN) {
		current_valve = valveNum;
	}
	if (DISPLAY_VALVE_STATUS) {
		Serial.print("Valve ");
		Serial.print(valveNum);
		Serial.println(" OPEN.");
	}
}

void CloseValve(int valveNum) {
	// error checking
	if ((valveNum > NUM_VALVES)) {
		Serial.print("Error: Bad valve number ("); Serial.print(valveNum); Serial.println(").");
		return;
	}
	if (ONE_VALVE_OPEN && (valveNum == 0)) {
		if (current_valve > 1) {
			CloseValve(current_valve);
		}
		return;
	}
	if (valve_status_list[valveNum-1] == VALVE_CLOSED) {
		Serial.print("Cannot close valve "); Serial.print(valveNum); Serial.println(", it's not open.");
		return;
	}
	if (ONE_VALVE_OPEN && (valveNum==1)) {
	 	Serial.println("Warning: Valve 1 cannot be directly closed in 'ONE_VALVE_OPEN' mode.");
	 	return;
	}

	// close valve
	digitalWrite(all_valves[valveNum-1], LOW);
	if (valve_status_list[valveNum-1] == VALVE_PWM) {
		analogWrite(all_valves[valveNum-1], 0);
	}
	valve_status_list[valveNum-1] = VALVE_CLOSED;

	// open valve 1 if necessary
	if (ONE_VALVE_OPEN) {
		current_valve = 1;
		digitalWrite(V1, HIGH);
		valve_status_list[0] = VALVE_OPEN_2;

//    Serial.println("-- V1 Open --");
		if (TRIGGER_WHEN_OPEN && BNC2_OUTPUT) {
			digitalWrite(BNC2_pin, LOW);
		}
	}
	if (DISPLAY_VALVE_STATUS) {
		Serial.print("Valve ");
		Serial.print(valveNum);
		Serial.println(" CLOSED.");
	}
}

static elapsedMillis previousUpdateInterval = 0;
void updateValves() {
	if (previousUpdateInterval > 500) {
		previousUpdateInterval = 0;
		for (int valve = 0; valve < NUM_VALVES; valve++) {
			// test for VALVE_CLOSED first, since it's most likely
			if (valve_status_list[valve] > VALVE_CLOSED) {
				if (valve_status_list[valve] == VALVE_OPEN_2) {
					valve_status_list[valve] = VALVE_OPEN_1;
				} else if (valve_status_list[valve] == VALVE_OPEN_1) {
					if (digitalPinHasPWM(all_valves[valve])) {
						valve_status_list[valve] = VALVE_PWM;
						analogWrite(all_valves[valve], 127);
						// if (DISPLAY_VALVE_STATUS) {
						// 	Serial.print("Valve ");
						// 	Serial.print(valve+1);
						// 	Serial.println(" PWM (low power).");
						// }
					} else {
						valve_status_list[valve] = VALVE_NO_PWM;
					}
				}
			}
		}
	}
}
