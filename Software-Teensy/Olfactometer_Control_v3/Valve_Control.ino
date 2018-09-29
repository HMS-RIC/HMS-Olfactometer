// =======================================
// Valve control functions

int current_valve = 1;
void OpenValve(int valveNum) {
	// error checking
	if (digitalRead(all_valves[valveNum-1]) == HIGH) {
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
		valve_status_list[current_valve-1] = VALVE_CLOSED;

//    	if (current_valve==1) {
//            Serial.println("-- V1 Closed --");
//    }
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
	if (ONE_VALVE_OPEN && (valveNum == 0)) {
		if (current_valve > 1) {
			CloseValve(current_valve);
		}
		return;
	}
	// if (digitalRead(all_valves[valveNum-1]) == LOW) {
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

void updateValves() {
	for (int valve = 0; valve < NUM_VALVES; valve++) {
		// test for VALVE_CLOSED first, since it's most likely
		if (valve_status_list[valve] > VALVE_CLOSED) {
			if (valve_status_list[valve] == VALVE_OPEN_2) {
				valve_status_list[valve] = VALVE_OPEN_1;
			} else if (valve_status_list[valve] == VALVE_OPEN_1) {
				if (digitalPinHasPWM(all_valves[valve])) {
					valve_status_list[valve] = VALVE_PWM;
					analogWrite(all_valves[valve], 127);
				} else {
					valve_status_list[valve] = VALVE_NO_PWM;
				}
			}
		}
	}
}
