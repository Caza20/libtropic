#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "libtropic_macros.h"
#include "libtropic_port.h"

#include "lt_port_arduino.h"
#include <SPI.h>
#include <Arduino.h>

lt_ret_t lt_port_random_bytes(lt_l2_state_t *s2, void *buff, size_t count) {
    // lt_dev_arduino *device = (lt_dev_arduino *)(s2->device);
    size_t bytes_left = count;
    uint8_t *buff_ptr = (uint8_t *)buff;

    while (bytes_left) {
        uint32_t random_data = random(0, UINT32_MAX);
        size_t cpy_cnt = bytes_left < sizeof(random_data) ? bytes_left : sizeof(random_data);
        memcpy(buff_ptr, &random_data, cpy_cnt);
        bytes_left -= cpy_cnt;
        buff_ptr += cpy_cnt;
    }

    return LT_OK;
}

lt_ret_t lt_port_spi_csn_low(lt_l2_state_t *s2) {
    lt_dev_arduino *device = (lt_dev_arduino *)(s2->device);
    digitalWrite(device->spi_cs_pin, LOW);
    while (digitalRead(device->spi_cs_pin) == HIGH);
    return LT_OK;
}

lt_ret_t lt_port_spi_csn_high(lt_l2_state_t *s2) {
    lt_dev_arduino *device = (lt_dev_arduino *)(s2->device);
    digitalWrite(device->spi_cs_pin, HIGH);
    while (digitalRead(device->spi_cs_pin) == LOW);
    return LT_OK;
}

lt_ret_t lt_port_init(lt_l2_state_t *s2) {
    lt_dev_arduino *device = (lt_dev_arduino *)(s2->device);
    SPI.begin();
    pinMode(device->spi_cs_pin, OUTPUT);
    digitalWrite(device->spi_cs_pin, HIGH);
    return LT_OK;
}

lt_ret_t lt_port_deinit(lt_l2_state_t *s2) {
    lt_dev_arduino *device = (lt_dev_arduino *)(s2->device);
    SPI.end();
    return LT_OK;
}

lt_ret_t lt_port_spi_transfer(lt_l2_state_t *s2, uint8_t offset, uint16_t tx_data_length, uint32_t timeout_ms) {
    lt_dev_arduino *device = (lt_dev_arduino *)(s2->device);
    if (offset + tx_data_length > LT_L1_LEN_MAX) {
        return LT_L1_DATA_LEN_ERROR;
    }
    SPI.beginTransaction(SPISettings(device->baudrate, MSBFIRST, SPI_MODE0));
    SPI.transfer(s2->buff + offset, tx_data_length);
    SPI.endTransaction();
    return LT_OK;
}

lt_ret_t lt_port_delay(lt_l2_state_t *s2, uint32_t ms) {
    UNUSED(s2);
    delay(ms);
    return LT_OK;
}

#if LT_USE_INT_PIN
lt_ret_t lt_port_delay_on_int(lt_l2_state_t *s2, uint32_t ms) {
    lt_dev_arduino *device = (lt_dev_arduino *)(s2->device);
    uint32_t time_initial = millis();
    while (digitalRead(device->int_pin) == LOW) {
        if ((millis() - time_initial) > ms) {
            return LT_L1_INT_TIMEOUT;
        }
    }
    return LT_OK;
}
#endif