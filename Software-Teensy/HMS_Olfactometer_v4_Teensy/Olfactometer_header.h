
#ifndef _OLFACTOMETER_HEADER_
#define _OLFACTOMETER_HEADER_

#include <Arduino.h>
#include <SensirionI2cSfx6xxx.h>
#include <Wire.h>
#include <Adafruit_MCP23X08.h>

const bool DEBUG = false;

// =================================
// Pin Assignments for PCB v 4.0
// =================================

const uint8_t BNC1_pin = 23;
const uint8_t BNC2_pin = 22;
const uint8_t BNC3_pin = 15;
const uint8_t BNC4_pin = 14;


const uint8_t MAX_NUM_ODOR_VALVES = 48;
const uint8_t MAX_NUM_AUX_VALVES = 8;

#endif // _OLFACTOMETER_HEADER_
