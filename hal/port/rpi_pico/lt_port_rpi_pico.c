#include "lt_port_rpi_pico.h"
#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "libtropic_macros.h"
#include "libtropic_port.h"
#include "libtropic_examples.h"
#include "libtropic_functional_tests.h"

#include "pico/rand.h"
#include "pico/stdlib.h"

#include <string.h>


// #include "pico_stdlib/stdlib.c"
#include "hardware/spi.h"
// #include "pico_rand/rand.c"   // para get_rand_32()

lt_ret_t lt_port_random_bytes(lt_l2_state_t *s2, void *buff, size_t count)
{
    lt_dev_pico *device = (lt_dev_pico *)(s2->device);
    uint8_t *buff_ptr = (uint8_t *)buff;
    size_t bytes_left = count;

    while (bytes_left > 0) {
        uint32_t rnd = get_rand_32(); // pseudo-random del SDK
        size_t cpy_cnt = bytes_left < sizeof(rnd) ? bytes_left : sizeof(rnd);
        memcpy(buff_ptr, &rnd, cpy_cnt);
        buff_ptr += cpy_cnt;
        bytes_left -= cpy_cnt;
    }

    return LT_OK;
}

lt_ret_t lt_port_spi_csn_low(lt_l2_state_t *s2)
{
    lt_dev_pico *device = (lt_dev_pico *)(s2->device);
    gpio_put(device->cs_pin, 0);
    while(gpio_get(device->cs_pin));
    return LT_OK;
}

lt_ret_t lt_port_spi_csn_high(lt_l2_state_t *s2)
{
    lt_dev_pico *device = (lt_dev_pico *)(s2->device);
    gpio_put(device->cs_pin, 1);
    while(!gpio_get(device->cs_pin));
    return LT_OK;
}

lt_ret_t lt_port_init(lt_l2_state_t *s2)
{
    lt_dev_pico *device = (lt_dev_pico *)(s2->device);

    // int ret;

    // Inicializar SPI
    spi_init(device->spi_instance, device->spi_baudrate);

    // while (spi_get_hw(device->spi_instance)->sr & SPI_SSPSR_BSY_BITS) {
    //     LT_LOG_ERROR("Wait to init SPI, ret=%d", (int)1);
    // }
    // if ((uint32_t)ret != device->spi_baudrate) {
    //     LT_LOG_ERROR("Failed to init SPI, ret=%d", ret);
    //     LT_LOG_ERROR("Failed to init SPI, baud=%d", (int)device->spi_baudrate);
    //     return LT_L1_SPI_ERROR;
    // }
    gpio_set_function(device->pin_miso, GPIO_FUNC_SPI);
    gpio_set_function(device->pin_mosi, GPIO_FUNC_SPI);
    gpio_set_function(device->pin_sck,  GPIO_FUNC_SPI);

    gpio_set_function(device->cs_pin,   GPIO_FUNC_SIO);

    // CS como salida
    gpio_init(device->cs_pin);
    // LT_SPI_CS_CLK_ENABLE();
    gpio_set_dir(device->cs_pin, GPIO_OUT);
    gpio_put(device->cs_pin, 1);

    

#ifdef LT_USE_INT_PIN
    gpio_init(device->int_pin);
    gpio_set_dir(device->int_pin, GPIO_IN);
#endif

    device->initialized = true;
    return LT_OK;
}

lt_ret_t lt_port_deinit(lt_l2_state_t *s2)
{
    lt_dev_pico *device = (lt_dev_pico *)(s2->device);

    spi_deinit(device->spi_instance);
    device->initialized = false;
    return LT_OK;
}

lt_ret_t lt_port_spi_transfer(lt_l2_state_t *s2, uint8_t offset, uint16_t tx_data_length, uint32_t timeout_ms)
{
    lt_dev_pico *device = (lt_dev_pico *)(s2->device);

    if (offset + tx_data_length > LT_L1_LEN_MAX) {
        LT_LOG_ERROR("Invalid data length!");
        return LT_L1_DATA_LEN_ERROR;
    }

    // gpio_put(device->cs_pin, 0);
    spi_write_read_blocking(device->spi_instance, s2->buff + offset, s2->buff + offset, tx_data_length);
    // gpio_put(device->cs_pin, 1);

    return LT_OK;
}

lt_ret_t lt_port_delay(lt_l2_state_t *s2, uint32_t ms)
{
    (void)s2;
    sleep_ms(ms);
    return LT_OK;
}

#if LT_USE_INT_PIN
lt_ret_t lt_port_delay_on_int(lt_l2_state_t *s2, uint32_t ms)
{
    lt_dev_pico *device = (lt_dev_pico *)(s2->device);

    absolute_time_t start = get_absolute_time();
    while (gpio_get(device->int_pin) == 0) {
        if (absolute_time_diff_us(start, get_absolute_time()) / 1000 > ms) {
            return LT_L1_INT_TIMEOUT;
        }
        sleep_ms(1);
    }
    return LT_OK;
}
#endif
