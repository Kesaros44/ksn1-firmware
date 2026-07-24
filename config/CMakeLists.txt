# KSN1 config module. See config/src/ksn1_peripheral_indicators.c for why this
# exists: ZMK's core zmk,indicator-leds driver can't drive an LED that is
# physically wired to a split peripheral (the left half here), because it
# links against central-only endpoint/HID-indicator symbols.
target_sources(app PRIVATE src/ksn1_peripheral_indicators.c)
