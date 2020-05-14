## Old schoold demo effects for ESP32

```
$ git clone git@github.com:tuupola/esp_effects.git --recursive
$ cd esp_effects
$ cp sdkconfig.ttgo-t-display sdkconfig
$ make -j8 flash
```

If you have some other board or display run menuconfig yourself.

```
$ git clone git@github.com:tuupola/esp_effects.git --recursive
$ cd esp_effects
$ make menuconfig
$ make -j8 flash
```

