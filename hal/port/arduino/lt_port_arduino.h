#ifndef LT_PORT_ARDUINO_H
#define LT_PORT_ARDUINO_H

#include "libtropic_port.h"
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>


typedef struct {
    SPIClass *spi_instance;
    uint8_t spi_cs_pin;
    uint32_t baudrate_prescaler;

    /** @brief @public Optional interrupt pin. */
#ifdef LT_USE_INT_PIN
    uint8_t int_pin;
#endif
    // Add other device-specific configurations as needed
} lt_dev_arduino;


#endif // LT_PORT_ARDUINO_H