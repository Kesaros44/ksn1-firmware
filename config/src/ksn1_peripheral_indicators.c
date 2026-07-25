/*
 * Caps lock LED driver for the KSN1 peripheral (left) half.
 *
 * ZMK's official `zmk,indicator-leds` driver (app/src/indicators/indicator_leds.c)
 * calls zmk_hid_indicators_get_current_profile() / zmk_endpoint_is_connected(),
 * which are only compiled when `(NOT CONFIG_ZMK_SPLIT) OR CONFIG_ZMK_SPLIT_ROLE_CENTRAL`
 * (see app/CMakeLists.txt). The caps lock LED here is physically on the
 * peripheral (left) half, so that driver can't be used - it would link-fail
 * with "undefined reference" on a peripheral build.
 *
 * Instead, this listens for zmk_hid_indicators_changed, which IS safe on a
 * peripheral: with CONFIG_ZMK_SPLIT_PERIPHERAL_HID_INDICATORS=y, central
 * pushes the raw indicator bitmask to the peripheral over BLE and this event
 * fires locally with no central-only APIs involved.
 */

#if !defined(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/led.h>

#include <zmk/event_manager.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/hid_indicators_types.h>
#include <dt-bindings/zmk/hid_indicators.h>

#define LED_GPIO_NODE_ID DT_COMPAT_GET_ANY_STATUS_OKAY(gpio_leds)

static const struct device *led_dev = DEVICE_DT_GET(LED_GPIO_NODE_ID);

BUILD_ASSERT(DT_NODE_EXISTS(DT_ALIAS(caps_lock_led)),
             "caps-lock-led alias not found - check ksn_1_left.overlay");
static const uint8_t caps_lock_led_idx = DT_NODE_CHILD_IDX(DT_ALIAS(caps_lock_led));

static int caps_lock_led_listener(const zmk_event_t *eh) {
    const struct zmk_hid_indicators_changed *ev = as_zmk_hid_indicators_changed(eh);
    if (ev == NULL) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    if (ev->indicators & HID_INDICATOR_CAPS_LOCK) {
        led_on(led_dev, caps_lock_led_idx);
    } else {
        led_off(led_dev, caps_lock_led_idx);
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(caps_lock_led, caps_lock_led_listener);
ZMK_SUBSCRIPTION(caps_lock_led, zmk_hid_indicators_changed);

#endif /* !CONFIG_ZMK_SPLIT_ROLE_CENTRAL */
