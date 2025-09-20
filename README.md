# TI LAUNCHXL-F28379D Peripheral Applications

This repository contains example applications developed for the TI LAUNCHXL-F28379D board, demonstrating the use of key peripherals including **Timer, ePWM, and ADC.**
Each application highlights how to configure and integrate these peripherals into practical embedded system scenarios.

## ⚙️ Hardware Setup

- Hardware: TI LAUNCHXL-F28379D development board

- Inverter: External MOSFET driver board (IRF9540N / IRF540N)

## Timer Application – Microwave Simulation

Press the Start button to **begin countdown and turn on the LED.**

A periodic interrupt checks the remaining time every second.

When the timer reaches 0, the microwave stops and the LED is turned off.

https://github.com/user-attachments/assets/1c48e1b9-1479-4134-a2be-74d0d86a2792

## ePWM Application – Frequency & Duty Cycle Control

Button 1 cycles through different PWM frequencies:
**5 kHz → 10 kHz → 20 kHz → repeat**

Button 2 cycles through different duty cycles:
**20% → 50% → 80% → repeat**

https://github.com/user-attachments/assets/0d836f4f-ffe4-4069-9010-1a05c7431ae2

## ADC Application – Potentiometer Calibration

**currentADC**: raw ADC value from the potentiometer (before calibration).

**mappedADC**: calibrated ADC value.

During calibration, the system records minimum, maximum, and midpoint values.

After calibration, mappedADC reflects the corrected and scaled ADC result.

https://github.com/user-attachments/assets/c42280ab-26f3-422a-bdee-d71017f52a85


