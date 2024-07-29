#include "serial_hack.h"

uint8_t get_parity(uint8_t command) {
    return 1;
}

uint8_t listen_for_response(Pin& rx_pin) {
    return 0;
}

void dispatch_byte(Pin& triggerPin, Pin& bitPin, uint8_t byte) {
  triggerPin.synchronousWrite(false);
  delayMicroseconds(100);
  for (uint8_t i = 0; i < 8; i++) {
      bitPin.synchronousWrite(bitRead(byte, i));
      triggerPin.synchronousWrite(true);
      delayMicroseconds(100);
      triggerPin.synchronousWrite(false);
      delayMicroseconds(100);
    }
}

uint8_t receive_byte(Pin& triggerPin, Pin& bitPin) {
    char debug[50];
    uint8_t dataIn = 0x00;
    uint8_t bitIn = 0;
    uint64_t timeout = millis() + 500;
    bool timedOut = false;
    bool dataReceived = false;
    bool wasTriggered = false;

    uint64_t attempts = 0;

    while (!timedOut && !dataReceived) {
        attempts++;
        bool isTriggered = triggerPin.read();
        bool bitState = bitPin.read();
        if (isTriggered && !wasTriggered) {
            sprintf(debug, "Bit %i, Value %i", bitIn, bitState);
            log_info(debug);
            if (bitState) {
                bitSet(dataIn, bitIn);
            }
            if (bitIn == 7) {
                dataReceived = true;
            }
            bitIn++;
            timeout = millis() + 500;
            sprintf(debug, "Timeout %i", timeout);
            log_info(debug);
            sprintf(debug, "Time %i", millis());
            log_info(debug);
        }
        wasTriggered = isTriggered;
        if (timeout < millis()) {
            timedOut = true;
        }
        
    }

    if (timedOut) {
        log_info("Timed out");
        sprintf(debug, "Attempts %i", attempts);
        log_info(debug);
        return 0xff;
    }

    return dataIn;
}


uint8_t dispatch_to_atc(uint8_t data) {
    RapidChange::RapidChange* rapidChange = config->_rapidChange;
    dispatch_byte(rapidChange->_trigger_out, rapidChange->_signal_out, data);
    byte responseData = receive_byte(rapidChange->_trigger_in, rapidChange->_signal_in);

    
    return responseData;
}

void send_message_to_atc(const char* message) {
    RapidChange::RapidChange* rapidChange = config->_rapidChange;
    int length = strlen(message);
    for (int i = 0; i < length - 1; i++) {
        dispatch_byte(rapidChange->_trigger_out, rapidChange->_signal_out, message[i]);
    }
    char lastChar = message[length - 1];
    bitSet(lastChar, 7);
    dispatch_byte(rapidChange->_trigger_out, rapidChange->_signal_out, lastChar);
    rapidChange = nullptr;
}
