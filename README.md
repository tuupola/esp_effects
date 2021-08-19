## Old schoold demo effects for ESP32

![ESP effects](https://appelsiini.net/img/2020/esp-effects.jpg)

Created to test the [HAGL graphics library](https://github.com/tuupola/hagl). For quick reference see the [recording on Vimeo](https://vimeo.com/419551395).

Ready made config files for M5Stack, M5Stick C, M5Stick CPlus, TTGO T-Display, TTGO T4 V13 and TTGO T-Watch 2020. For example to compile and flash for M5Stack run the following.

```
$ git clone https://github.com/tuupola/esp_effects.git --recursive
$ cd esp_effects
$ cp sdkconfig.m5stack sdkconfig
$ make -j8 flash
```

If you have some other board or display run menuconfig yourself. For smaller screens triple buffering offers the smoothest animations.

```
$ git clone https://github.com/tuupola/esp_effects.git --recursive
$ cd esp_effects
$ make menuconfig
$ make -j8 flash
```

Or if you are using the new build system.

```
$ git clone https://github.com/tuupola/esp_effects.git --recursive
$ cd esp_effects
$ idf.py menuconfig
$ idf.py build flash
```

## Run on computer

HAGL is hardware agnostic. You can run the demos also [on your computer](https://github.com/tuupola/sdl2_effects).

