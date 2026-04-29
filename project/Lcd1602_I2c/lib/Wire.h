#ifndef Wire_h
#define Wire_h

#include <stdint.h>
#include <stddef.h>

// I2C status codes
#define TWI_START   0x08
#define TWI_RSTART  0x10
#define TWI_SLAW_ACK    0x18
#define TWI_SLAW_NACK   0x20
#define TWI_DATA_ACK    0x28
#define TWI_DATA_NACK   0x30
#define TWI_ERROR   0x38

class TwoWire {
private:
    uint8_t txBuffer[32];
    uint8_t txBufferIndex;
    uint8_t rxBuffer[32];
    uint8_t rxBufferIndex;
    uint8_t txAddress;
    static const uint32_t TWBR_VAL = 72; // For 100kHz @ 16MHz
    
public:
    TwoWire();
    void begin();
    void end();
    void setClock(uint32_t clock);
    
    void beginTransmission(uint8_t address);
    void beginTransmission(int address);
    uint8_t endTransmission(uint8_t sendStop = 1);
    
    size_t write(uint8_t data);
    size_t write(const uint8_t *data, size_t quantity);
    
    uint8_t read(void);
    int available(void);
    void flush(void);
    
    uint8_t requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop = 1);
    
private:
    void twi_init(void);
    uint8_t twi_writeTo(uint8_t addr, uint8_t *data, uint8_t length, uint8_t sendStop);
    void twi_stop(void);
};

extern TwoWire Wire;

#endif
