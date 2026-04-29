#include "Wire.h"
#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>

TwoWire Wire;

TwoWire::TwoWire() : txBufferIndex(0), rxBufferIndex(0), txAddress(0) {
    memset(txBuffer, 0, sizeof(txBuffer));
    memset(rxBuffer, 0, sizeof(rxBuffer));
}

void TwoWire::begin() {
    twi_init();
}

void TwoWire::end() {
    // Disable TWI
    TWCR = 0;
}

void TwoWire::setClock(uint32_t clock) {
    // For 100kHz with 16MHz clock, TWBR = 72
}

void TwoWire::twi_init(void) {
    // Enable TWI, set prescaler to 1, set bit rate
    TWSR = 0;  // prescaler = 1
    TWBR = TWBR_VAL;  // bit rate for 100kHz @ 16MHz
    TWCR = (1 << TWEN);  // Enable TWI
}

void TwoWire::beginTransmission(uint8_t address) {
    txAddress = address;
    txBufferIndex = 0;
}

void TwoWire::beginTransmission(int address) {
    beginTransmission((uint8_t)address);
}

size_t TwoWire::write(uint8_t data) {
    if (txBufferIndex >= sizeof(txBuffer)) return 0;
    txBuffer[txBufferIndex++] = data;
    return 1;
}

size_t TwoWire::write(const uint8_t *data, size_t quantity) {
    size_t i;
    for (i = 0; i < quantity; i++) {
        if (!write(data[i])) break;
    }
    return i;
}

uint8_t TwoWire::endTransmission(uint8_t sendStop) {
    return twi_writeTo(txAddress, txBuffer, txBufferIndex, sendStop);
}

uint8_t TwoWire::twi_writeTo(uint8_t addr, uint8_t *data, uint8_t length, uint8_t sendStop) {
    uint8_t i, status;
    
    // Send START
    TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTA);
    while (!(TWCR & (1 << TWINT)));
    status = TW_STATUS;
    
    if (status != TW_START && status != TW_REP_START) {
        return 1;  // Error
    }
    
    // Send address + W
    TWDR = TW_WRITE | (addr << 1);
    TWCR = (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)));
    status = TW_STATUS;
    
    if (status != TW_MT_SLA_ACK && status != TW_MT_SLA_NACK) {
        return 2;  // Error
    }
    
    // Send data
    for (i = 0; i < length; i++) {
        TWDR = data[i];
        TWCR = (1 << TWEN) | (1 << TWINT);
        while (!(TWCR & (1 << TWINT)));
        status = TW_STATUS;
        
        if (status != TW_MT_DATA_ACK && status != TW_MT_DATA_NACK) {
            break;
        }
    }
    
    // Send STOP if requested
    if (sendStop) {
        TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTO);
    }
    
    return 0;  // Success
}

void TwoWire::twi_stop(void) {
    TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTO);
}

uint8_t TwoWire::read(void) {
    return 0;
}

int TwoWire::available(void) {
    return 0;
}

void TwoWire::flush(void) {
    // Flush is not needed for master mode
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop) {
    return 0;
}
