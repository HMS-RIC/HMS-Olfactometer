

void USB_ValveOpen(int valve) {
	if (VERBOSE_OUTPUT) {
		Serial.print(F("Valve "));
		Serial.print(valve);
		Serial.println(F(" OPEN."));
	} else {
		Serial.print(F("O "));
		Serial.println(valve);
	}
}

void USB_ValveClosed(int valve) {
	if (VERBOSE_OUTPUT) {
		Serial.print(F("Valve "));
		Serial.print(valve);
		Serial.println(F(" CLOSED."));
	} else {
		Serial.print(F("C "));
		Serial.println(valve);
	}
}

void USB_BNCOn(int channelNum) {
	if (VERBOSE_OUTPUT) {
		Serial.print(F("BNC "));
		Serial.print(channelNum);
		Serial.println(F(" pulse START."));
	} else {
		Serial.print(F("B "));
		Serial.println(channelNum);
	}
}

void USB_BNCOff(int channelNum) {
	if (VERBOSE_OUTPUT) {
		Serial.print(F("BNC "));
		Serial.print(channelNum);
		Serial.println(F(" pulse END."));
	} else {
		Serial.print(F("E "));
		Serial.println(channelNum);
	}
}

void USB_CarrierFlowRate(float flow_lpm) {
	if (VERBOSE_OUTPUT) {
		Serial.print(F("Carrier flow rate "));
		Serial.print(flow_lpm);
		Serial.println(F(" LPM."));
	} else {
		Serial.print(F("R "));
		Serial.println(round(flow_lpm*1000));
	}
}

void USB_OdorFlowRate(float flow_lpm) {
	if (VERBOSE_OUTPUT) {
		Serial.print(F("Odor flow rate "));
		Serial.print(flow_lpm);
		Serial.println(F(" LPM."));
	} else {
		Serial.print(F("D "));
		Serial.println(round(flow_lpm*1000));
	}
}

void USB_PrintProg() {
	if (VERBOSE_OUTPUT) {
		if (prog_len == 0) {
			Serial.println(F("-- Empty Program --"));
		} else {
			Serial.println(F("-- Current program: --"));
			String progLine = "";
			for (int i=0; i<prog_len; i++) {
				progLine = String(i+1) + ": \t"; // use 1-based line numbers
				if (cmd_list[i] == CMD_OPEN) {
					progLine += "OPEN      \t";
				} else if (cmd_list[i] == CMD_CLOSE) {
					progLine += "CLOSE     \t";
				} else if (cmd_list[i] == CMD_BNC_ON) {
					progLine += "PULSE-ON  \t";
				} else if (cmd_list[i] == CMD_BNC_OFF) {
					progLine += "PULSE-OFF \t";
				} else {
					progLine += "????      \t";
				}
				if ((cmd_list[i] == CMD_BNC_ON) || (cmd_list[i] == CMD_BNC_OFF)) {
					progLine += "B";
				} else {
					progLine += "V";
				}
				progLine += valve_list[i];
				progLine += " \t";
				progLine += duration_ms_list[i];
				progLine += " ms";
				Serial.println(progLine.c_str());
			}
			Serial.println(F("-- Current program: --"));
		}
	} else {
		Serial.print(F("L "));
		Serial.println(prog_len);
		for (int i=0; i<prog_len; i++) {
			Serial.print(F("P "));
			Serial.print(i);
			Serial.print(F(" "));
			Serial.print(cmd_list[i]);
			Serial.print(F(" "));
			Serial.print(valve_list[i]);
			Serial.print(F(" "));
			Serial.println(duration_ms_list[i]);
		}
	}
}

void USB_BeginProgram() {
	if (VERBOSE_OUTPUT) {
		Serial.println(F("# Program Started"));
	} else {
		Serial.println(F("B"));
	}
}

void USB_EndProgram() {
	if (VERBOSE_OUTPUT) {
		Serial.println(F("# Program Ended"));
	} else {
		Serial.println(F("E"));
	}
}


// ERROR #1
void USB_Error_ValveOneAccess() {
	if (VERBOSE_OUTPUT) {
		Serial.println(F("Warning: Valve 1 cannot be directly closed in 'ONE_VALVE_OPEN' mode."));
	} else {
		Serial.println(F("# 1"));
	}
}

// ERROR #2
void USB_Error_ValveAlreadyOpen(int valve) {
	if (VERBOSE_OUTPUT) {
		Serial.print(F("Valve "));
		Serial.print(valve);
		Serial.println(F(" already open."));
	} else {
		Serial.print(F("# 2 "));
		Serial.println(valve);
	}
}

// ERROR #3
void USB_Error_ValveCannotClose(int valve) {
	if (VERBOSE_OUTPUT) {
		Serial.print(F("Cannot close valve "));
		Serial.print(valve);
		Serial.println(F(", it's not open."));
	} else {
		Serial.print(F("# 3 "));
		Serial.println(valve);
	}
}

// ERROR #4
void USB_Error_BadMFCResponse() {
	if (VERBOSE_OUTPUT) {
		Serial.println(F("ERROR: bad MFC reponse."));
	} else {
		Serial.println(F("# 4"));
	}
}

// ERROR #5
void USB_Error_BadValveNumber() {
	if (VERBOSE_OUTPUT) {
		Serial.println(F("ERROR: Bad valve number."));
	} else {
		Serial.println(F("# 5"));
	}
}

// ERROR #6
void USB_Error_CannotActivateValve() {
	if (VERBOSE_OUTPUT) {
		Serial.println(F("ERROR: Cannot switch active valves when an odor valve is open."));
	} else {
		Serial.println(F("# 6"));
	}
}

// ERROR #7
void USB_Error_MaxProgLen() {
	if (VERBOSE_OUTPUT) {
		Serial.print(F("ERROR: Program has reached maximum length ("));
		Serial.print(MAX_PROG_LEN);
		Serial.println(F(")."));
	} else {
		Serial.println(F("# 7"));
	}
}
