
// =======================================
// BNC Triggering Functions
void WaitForTrigger1() {
 while (digitalRead(BNC1_pin) == LOW) {};
}

void WaitForTrigger2() {
 while (digitalRead(BNC2_pin) == LOW) {};
}

