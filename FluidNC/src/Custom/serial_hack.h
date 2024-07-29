#pragma once

#include "../Machine/MachineConfig.h"
#include "../NutsBolts.h"
#include <Arduino.h>

const uint8_t request_tool_change = 0x0;


uint8_t get_parity(uint8_t command);
uint8_t dispatch_to_atc(uint8_t data);
uint8_t listen_for_response(Pin &rx_pin);
uint8_t receive_byte(Pin& triggerPin, Pin& bitPin);
void receive_message_from_atc(char* buffer, Pin& trigger_in, Pin& signal_in);
void send_message_to_atc(const char* message, char* buffer);
