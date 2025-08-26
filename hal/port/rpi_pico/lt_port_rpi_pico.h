#ifndef LT_PORT_RPI_PICO_H
#define LT_PORT_RPI_PICO_H


#include "libtropic_port.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

/**
 * @brief Device structure for Raspberry Pi Pico port.
 *
 * El usuario completa los parámetros antes de pasar el handle a libtropic.
 */
typedef struct lt_dev_pico {
    /** @brief @public Instancia de SPI (ej: spi0 o spi1). */
    spi_inst_t *spi_instance;

    /** @brief @public Frecuencia SPI en Hz (ej: 1 MHz, 4 MHz, etc.). */
    uint32_t spi_baudrate;

    /** @brief @public Pin usado para chip select (GPIO). */
    uint cs_pin;

    uint pin_miso;
    uint pin_mosi;
    uint pin_sck;

#ifdef LT_USE_INT_PIN
    /** @brief @public Pin usado para interrupciones (opcional). */
    uint int_pin;
#endif

    /** @brief @private flag para inicialización */
    bool initialized;
} lt_dev_pico;

#endif // LT_PORT_PICO_H
