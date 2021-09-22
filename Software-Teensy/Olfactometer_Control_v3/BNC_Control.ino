
// =======================================
// BNC Triggering Functions
void WaitForTrigger1() {
 while (digitalRead(BNC1_pin) == LOW) {};
}

void WaitForTrigger2() {
 while (digitalRead(BNC2_pin) == LOW) {};
}


// =======================================
// BNC Pulse Generation Functions
void BNC_Pulse_On(int channelNum) {
    if ((channelNum == 1) && BNC1_OUTPUT) {
        digitalWrite(BNC1_pin, HIGH);
        if (DISPLAY_BNC_STATUS) {
            USB_BNCOn(channelNum);
        }
    } else if ((channelNum == 2) && BNC2_OUTPUT && (!TRIGGER_WHEN_OPEN)) {
        digitalWrite(BNC2_pin, HIGH);
        if (DISPLAY_BNC_STATUS) {
            USB_BNCOn(channelNum);
        }
    }
}

void BNC_Pulse_Off(int channelNum) {
    if ((channelNum == 1) && BNC1_OUTPUT) {
        digitalWrite(BNC1_pin, LOW);
        if (DISPLAY_BNC_STATUS) {
            USB_BNCOff(channelNum);
        }
    } else if ((channelNum == 2) && BNC2_OUTPUT && (!TRIGGER_WHEN_OPEN)) {
        digitalWrite(BNC2_pin, LOW);
        if (DISPLAY_BNC_STATUS) {
            USB_BNCOff(channelNum);
        }
    }
}

