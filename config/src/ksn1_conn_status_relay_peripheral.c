/*
 * Drives status_led (physically on the peripheral/left half, see
 * ksn_1_left.overlay) to show whether the CENTRAL half (currently
 * ksn_1_right) is connected to a PC/host - NOT whether the two keyboard
 * halves are linked to each other (that was the previous behavior, via
 * zmk-poor-mans-led-indicator's CONFIG_INDICATOR_LED_SHOW_PERIPHERAL_BLE,
 * now disabled in ksn_1_left.conf).
 *
 * Solid on  = central has an active host connection (USB or BLE).
 * Blinking  = no active host connection.
 *
 * WHY THIS NEEDS ITS OWN GATT SERVICE
 * ------------------------------------
 * ZMK's only built-in central->peripheral data channel is HID indicators
 * (CONFIG_ZMK_SPLIT_PERIPHERAL_HID_INDICATORS, already used by
 * ksn1_peripheral_indicators.c for the caps lock LED on this same board).
 * That channel is populated exclusively from real host Set_Report(Output)
 * data deep inside ZMK core (app/src/hid_indicators.c) and has no public
 * setter we could call from an out-of-tree file to inject a synthetic
 * "host connected" bit - so it can't be reused here. zmk-poor-mans-
 * led-indicator's SHOW_PERIPHERAL_BLE option only ever had access to the
 * peripheral<->central link state for the same underlying reason: from
 * the peripheral, central-only functions like zmk_endpoint_is_connected()
 * simply aren't linkable (see the comment at the top of
 * ksn1_peripheral_indicators.c).
 *
 * So instead, this file registers one small extra GATT service (its own
 * private UUIDs, see ksn1_conn_status_relay.h) directly on top of the SAME
 * BLE connection the two halves already use for ZMK's split protocol.
 * ksn1_conn_status_relay_central.c (built only on ksn_1_right) writes a
 * single byte to this characteristic every time zmk_endpoint_is_connected()
 * changes. This file just receives that byte and drives the LED.
 *
 * Only builds on the peripheral (mirrors the guard used in
 * ksn1_peripheral_indicators.c) and only if the status_led alias exists.
 */

#include <zephyr/devicetree.h>

#if !defined(CONFIG_ZMK_SPLIT_ROLE_CENTRAL) && DT_NODE_EXISTS(DT_ALIAS(status_led))

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/drivers/led.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/logging/log.h>

#include "ksn1_conn_status_relay.h"

LOG_MODULE_REGISTER(ksn1_conn_status_relay_peripheral, CONFIG_ZMK_LOG_LEVEL);

/* How fast status_led blinks while there's no host connection. */
#define KSN1_CONN_STATUS_BLINK_MS 300

#define LED_GPIO_NODE_ID DT_COMPAT_GET_ANY_STATUS_OKAY(gpio_leds)

static const struct device *led_dev = DEVICE_DT_GET(LED_GPIO_NODE_ID);
static const uint8_t status_led_idx = DT_NODE_CHILD_IDX(DT_ALIAS(status_led));

static bool host_connected;
static bool led_phys_on;
static struct k_work_delayable blink_work;

static void set_led(bool on) {
    led_phys_on = on;
    if (on) {
        led_on(led_dev, status_led_idx);
    } else {
        led_off(led_dev, status_led_idx);
    }
}

static void blink_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    if (host_connected) {
        /* Solid on - stop rescheduling, nothing left to toggle. */
        set_led(true);
        return;
    }

    set_led(!led_phys_on);
    k_work_reschedule(&blink_work, K_MSEC(KSN1_CONN_STATUS_BLINK_MS));
}

static void apply_state(bool connected) {
    if (host_connected == connected) {
        return;
    }
    host_connected = connected;

    if (connected) {
        k_work_cancel_delayable(&blink_work);
        set_led(true);
    } else {
        set_led(true); /* start each blink cycle from "on" */
        k_work_reschedule(&blink_work, K_MSEC(KSN1_CONN_STATUS_BLINK_MS));
    }
}

static ssize_t on_write(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf,
                        uint16_t len, uint16_t offset, uint8_t flags) {
    ARG_UNUSED(conn);
    ARG_UNUSED(attr);
    ARG_UNUSED(offset);
    ARG_UNUSED(flags);

    if (len < 1) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }

    apply_state(((const uint8_t *)buf)[0] != 0);
    return len;
}

BT_GATT_SERVICE_DEFINE(ksn1_conn_status_svc, BT_GATT_PRIMARY_SERVICE(KSN1_CONN_STATUS_SERVICE_UUID),
                        BT_GATT_CHARACTERISTIC(KSN1_CONN_STATUS_CHAR_UUID,
                                                BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                                                BT_GATT_PERM_WRITE_ENCRYPT, NULL, on_write, NULL)));

static int ksn1_conn_status_relay_peripheral_init(void) {
    k_work_init_delayable(&blink_work, blink_work_handler);

    /* Assume "not connected" (blinking) until the central actually tells
     * us otherwise - matches reality on cold boot, before the split link
     * and the host link are both up. */
    host_connected = false;
    set_led(true);
    k_work_reschedule(&blink_work, K_MSEC(KSN1_CONN_STATUS_BLINK_MS));

    return 0;
}

SYS_INIT(ksn1_conn_status_relay_peripheral_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#endif /* !CONFIG_ZMK_SPLIT_ROLE_CENTRAL && status_led alias exists */
