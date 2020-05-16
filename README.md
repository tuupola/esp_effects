## Old schoold demo effects for ESP32

Created to test the [HAGL graphics library](https://github.com/tuupola/hagl). Ready made config files for M5Stack, TTGO T-Display and TTGO T4 V13. For example to compile and flash for M5Stack run the following.

```
$ git clone git@github.com:tuupola/esp_effects.git --recursive
$ cd esp_effects
$ cp sdkconfig.ttgo-t-display sdkconfig
$ make -j8 flash
```

If you have some other board or display run menuconfig yourself. For smaller screens triple buffering offers the smoothest animations.

```
$ git clone git@github.com:tuupola/esp_effects.git --recursive
$ cd esp_effects
$ make menuconfig
$ make -j8 flash
```

## Run on computer

HAGL is hardware agnostic. You can run the demos also [on your computer](https://github.com/tuupola/sdl2_effects).

