/*
 *  Copyright (C) 2014 -2016  Espressif System
 *
 */
#include "espressif/esp_common.h"
#include "portmacro.h"

#include "c_types.h"
#include "gpio.h"

void ICACHE_FLASH_ATTR gpio_config(GPIO_ConfigTypeDef  *pGPIOConfig)
{
    uint16_t gpio_pin_mask = pGPIOConfig->GPIO_Pin;
    uint32_t io_reg;
    uint8_t io_num = 0;
    uint32_t pin_reg;

    if (pGPIOConfig->GPIO_Mode == GPIO_Mode_Input) {
        GPIO_AS_INPUT(gpio_pin_mask);
    } else if (pGPIOConfig->GPIO_Mode == GPIO_Mode_Output) {
        GPIO_AS_OUTPUT(gpio_pin_mask);
    }

    do {
        if ((gpio_pin_mask >> io_num) & 0x1) {
            io_reg = GPIO_PIN_REG(io_num);

            if ((0x1 << io_num) & (GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5)) {
                PIN_FUNC_SELECT(io_reg, 0);
            } else {
                PIN_FUNC_SELECT(io_reg, 3);
            }

            if (pGPIOConfig->GPIO_Pullup) {
                PIN_PULLUP_EN(io_reg);
            } else {
                PIN_PULLUP_DIS(io_reg);
            }

            if (pGPIOConfig->GPIO_Mode == GPIO_Mode_Out_OD) {
                portENTER_CRITICAL();

                pin_reg = GPIO_REG_READ(GPIO_PIN_ADDR(io_num));
                pin_reg &= (~GPIO_PIN_DRIVER_MASK);
                pin_reg |= (GPIO_PAD_DRIVER_ENABLE << GPIO_PIN_DRIVER_LSB);
                GPIO_REG_WRITE(GPIO_PIN_ADDR(io_num), pin_reg);

                portEXIT_CRITICAL();
            } else if (pGPIOConfig->GPIO_Mode == GPIO_Mode_Sigma_Delta) {
                portENTER_CRITICAL();

                pin_reg = GPIO_REG_READ(GPIO_PIN_ADDR(io_num));
                pin_reg &= (~GPIO_PIN_SOURCE_MASK);
                pin_reg |= (0x1 << GPIO_PIN_SOURCE_LSB);
                GPIO_REG_WRITE(GPIO_PIN_ADDR(io_num), pin_reg);
                GPIO_REG_WRITE(GPIO_SIGMA_DELTA_ADDRESS, SIGMA_DELTA_ENABLE);

                portEXIT_CRITICAL();
            }

            gpio_pin_intr_state_set(io_num, pGPIOConfig->GPIO_IntrType);
        }

        io_num++;
    } while (io_num < 16);
}


/*
 * Change GPIO pin output by setting, clearing, or disabling pins.
 * In general, it is expected that a bit will be set in at most one
 * of these masks.  If a bit is clear in all masks, the output state
 * remains unchanged.
 *
 * There is no particular ordering guaranteed; so if the order of
 * writes is significant, calling code should divide a single call
 * into multiple calls.
 */
void ICACHE_FLASH_ATTR gpio_output_conf(uint32_t set_mask, uint32_t clear_mask, uint32_t enable_mask, uint32_t disable_mask)
{
    GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, set_mask);
    GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, clear_mask);
    GPIO_REG_WRITE(GPIO_ENABLE_W1TS_ADDRESS, enable_mask);
    GPIO_REG_WRITE(GPIO_ENABLE_W1TC_ADDRESS, disable_mask);
}

/*
 * Sample the value of GPIO input pins and returns a bitmask.
 */
uint32_t ICACHE_FLASH_ATTR gpio_input_get(void)
{
    return GPIO_REG_READ(GPIO_IN_ADDRESS);
}

/*
 * Register an application-specific interrupt handler for GPIO pin
 * interrupts.  Once the interrupt handler is called, it will not
 * be called again until after a call to gpio_intr_ack.  Any GPIO
 * interrupts that occur during the interim are masked.
 *
 * The application-specific handler is called with a mask of
 * pending GPIO interrupts.  After processing pin interrupts, the
 * application-specific handler may wish to use gpio_intr_pending
 * to check for any additional pending interrupts before it returns.
 */
void ICACHE_FLASH_ATTR gpio_intr_handler_register(void *fn)
{
    _xt_isr_attach(ETS_GPIO_INUM, fn, NULL);
}

/*
  only highlevel and lowlevel intr can use for wakeup
*/
void ICACHE_FLASH_ATTR gpio_pin_wakeup_enable(uint32_t i, GPIO_INT_TYPE intr_state)
{
    uint32_t pin_reg;

    if ((intr_state == GPIO_PIN_INTR_LOLEVEL) || (intr_state == GPIO_PIN_INTR_HILEVEL)) {
        portENTER_CRITICAL();

        pin_reg = GPIO_REG_READ(GPIO_PIN_ADDR(i));
        pin_reg &= (~GPIO_PIN_INT_TYPE_MASK);
        pin_reg |= (intr_state << GPIO_PIN_INT_TYPE_LSB);
        pin_reg |= GPIO_PIN_WAKEUP_ENABLE_SET(GPIO_WAKEUP_ENABLE);
        GPIO_REG_WRITE(GPIO_PIN_ADDR(i), pin_reg);

        portEXIT_CRITICAL();
    }
}

void ICACHE_FLASH_ATTR gpio_pin_wakeup_disable(void)
{
    uint8_t  i;
    uint32_t pin_reg;

    for (i = 0; i < GPIO_PIN_COUNT; i++) {
        pin_reg = GPIO_REG_READ(GPIO_PIN_ADDR(i));

        if (pin_reg & GPIO_PIN_WAKEUP_ENABLE_MASK) {
            pin_reg &= (~GPIO_PIN_INT_TYPE_MASK);
            pin_reg |= (GPIO_PIN_INTR_DISABLE << GPIO_PIN_INT_TYPE_LSB);
            pin_reg &= ~(GPIO_PIN_WAKEUP_ENABLE_SET(GPIO_WAKEUP_ENABLE));
            GPIO_REG_WRITE(GPIO_PIN_ADDR(i), pin_reg);
        }
    }
}

void ICACHE_FLASH_ATTR gpio_pin_intr_state_set(uint32_t i, GPIO_INT_TYPE intr_state)
{
    uint32_t pin_reg;

    portENTER_CRITICAL();

    pin_reg = GPIO_REG_READ(GPIO_PIN_ADDR(i));
    pin_reg &= (~GPIO_PIN_INT_TYPE_MASK);
    pin_reg |= (intr_state << GPIO_PIN_INT_TYPE_LSB);
    GPIO_REG_WRITE(GPIO_PIN_ADDR(i), pin_reg);

    portEXIT_CRITICAL();
}

void ICACHE_FLASH_ATTR gpio16_output_conf(void)
{
    WRITE_PERI_REG(PAD_XPD_DCDC_CONF,
                   (READ_PERI_REG(PAD_XPD_DCDC_CONF) & 0xffffffbc) | (uint32_t)0x1); 	// mux configuration for XPD_DCDC to output rtc_gpio0

    WRITE_PERI_REG(RTC_GPIO_CONF,
                   (READ_PERI_REG(RTC_GPIO_CONF) & (uint32_t)0xfffffffe) | (uint32_t)0x0);	//mux configuration for out enable

    WRITE_PERI_REG(RTC_GPIO_ENABLE,
                   (READ_PERI_REG(RTC_GPIO_ENABLE) & (uint32_t)0xfffffffe) | (uint32_t)0x1);	//out enable
}

void ICACHE_FLASH_ATTR gpio16_output_set(uint8_t value)
{
    WRITE_PERI_REG(RTC_GPIO_OUT,
                   (READ_PERI_REG(RTC_GPIO_OUT) & (uint32_t)0xfffffffe) | (uint32_t)(value & 1));
}

void ICACHE_FLASH_ATTR gpio16_input_conf(void)
{
    WRITE_PERI_REG(PAD_XPD_DCDC_CONF,
                   (READ_PERI_REG(PAD_XPD_DCDC_CONF) & 0xffffffbc) | (uint32_t)0x1); 	// mux configuration for XPD_DCDC and rtc_gpio0 connection

    WRITE_PERI_REG(RTC_GPIO_CONF,
                   (READ_PERI_REG(RTC_GPIO_CONF) & (uint32_t)0xfffffffe) | (uint32_t)0x0);	//mux configuration for out enable

    WRITE_PERI_REG(RTC_GPIO_ENABLE,
                   READ_PERI_REG(RTC_GPIO_ENABLE) & (uint32_t)0xfffffffe);	//out disable
}

uint8_t ICACHE_FLASH_ATTR gpio16_input_get(void)
{
    return (uint8_t)(READ_PERI_REG(RTC_GPIO_IN_DATA) & 1);
}

