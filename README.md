# srp

Konfigi do budowania:
- BBB
    ```
    --config=bbb-platform
    ```

ostatnie zbudowane obrazy dostępne do pobrania pod adresem:
- [EC](https://simbaj.toadres.pl/job/srp-master-build/lastSuccessfulBuild/artifact/bazel-bin/deployment/cpu/ec/pkg.tar)
- [FC](https://simbaj.toadres.pl/job/srp-master-build/lastSuccessfulBuild/artifact/bazel-bin/deployment/cpu/fc/pkg.tar)

## Potrzebne wersje:
cpplint==1.6.0



## DevBoard Tutorial


### PCA9685
C69 - PWM4 - CH3
C68 - PWM3 - CH2
C67 - PWM1 - CH0
C70 - PWM2 - CH1

### I2C
C18 - I2C_SDA - I2C_SDL

### UART
C71 - GND - TX - RX
C72 - GND - TX - RX
H6 - TX - RX

### 1Wire
C55, C56, C64, C65, C38, C63
GND - 3.3V - 1-Wire

### ADC
R4 - CH0
R5 - CH1
R6 - CH2
R7 - CH3


## Checklista działania

### Serva
- [x]  4 kanały PWM
- [x]  4 przetwornice:
    - układ z wyjściem C69 (VENT VALVE) po rozłączeniu zasilania powoli się rozładowywuje ( z 1.2V do ~0V)? czy nie powinien mieć jakiegoś rezystora który rozładuje napięcie po rozłączeniu zasilania czy to tylko efekt braku wpiętego serva
    - układ z wyjściem C68 idk
    - układ z wyjściem C67 po rozłączeniu zasilania utrzymuje 4V !!!!!!!! 
    - układ z wyjściem C70 (MAIN VALVE) 
- [x] ADC - średni błąd w przedziale 0.4V - 2.1V 0.5% -> nie wymaga korekty
- [x] wzmacniacze operacyjne...
    - na wzmacniaczach jest 2.12V maksymalna wartość?
- [ ] INA219
- [x] Spłonki
- [ ] Różnicówka: -> unused so not tested
    - Po RS485
    - Po ADC
- [x] EEPROM
- [x] Linecutter
- [ ] BME280 -> brak
- [ ] LSM6DSOXTR -> brak
