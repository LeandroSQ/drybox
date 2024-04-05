# DryBox

Aiming to dry my 3D filament spools, I have built a DryBox, this is the firmware running on the MCU inside the DryBox.

## Features

- [x] PID control of the temperature
- [x] IO/Serial
  - Readings to Serial every X seconds
  - Commands to control the DryBox such as:
    - `SETPOINT <value>` - Set the temperature setpoint
    - `PID <Kp> <Ki> <Kd>` - Tune the PID values
    - `FAN <value>` - Override the FAN speed
    - `HEATER <value>` - Override the Heater output
- [x] Home assistant integration
- [x] Silent FAN PWM control
  - Initially I was controlling the FAN using PID, but that is simply not needed anymore :p
- [x] Safety features
  - [x] Over temperature protection
  - [x] DHT sensor error detection
- [x] Calculates absolute humidity and the water vapor mass inside the box

## Integrations

### Python

Under src/tools you can find a python script that enables realtime plotting and control of the DryBox.
<p align=center>
    <img src=".github/screenshot01.png" width=650>
</p>
<p align=center>
    <small>Example while I was developing this</small>
</p>


### Home Assistant

Using template sensors you can integrate via USB/Serial the DryBox with Home Assistant. (Only if your board supports Serial communication, or you can use a TTL converter too)

```yaml
sensor:
  - platform: serial
    name: drybox
    serial_port: /dev/ttyACM1 # Replace with your port
    baudrate: 9600 # Replace with your baudrate
template:
  - binary_sensor:
    - name: "drybox_status"
      template: "{{ has_value('sensor.drybox') }}"
  - sensor:
      - name: "Fan Speed (PWM)"
        unit_of_measurement: "PWM"
        state: "{{ states('sensor.drybox').split(',')[0] | float(default=0) }}"
      - name: "Fan Speed"
        unit_of_measurement: "%"
        state: "{{ states('sensor.drybox').split(',')[1] | float(default=0) }}"
      - name: "Hotend temperature"
        unit_of_measurement: "ºC"
        state: "{{ states('sensor.drybox').split(',')[2] | float(default=0) }}"
      - name: "Heater setpoint"
        unit_of_measurement: "ºC"
        state: "{{ states('sensor.drybox').split(',')[3] | float(default=0) }}"
      - name: "Heater PID output"
        unit_of_measurement: "%"
        state: "{{ states('sensor.drybox').split(',')[4] | float(default=0) }}"
      - name: "Box temperature"
        unit_of_measurement: "ºC"
        state: "{{ states('sensor.drybox').split(',')[5] | float(default=0) }}"
      - name: "Box humidity"
        unit_of_measurement: "rh%"
        state: "{{ states('sensor.drybox').split(',')[6] | float(default=0) }}"
      - name: "Box absolute humidity"
        unit_of_measurement: "g/m³"
        state: "{{ states('sensor.drybox').split(',')[7] | float(default=0) }}"
```
<p align=center>
    <small>Into your <strong>configuration.yaml</strong></small>
</p>


## Hardware

| Component | Alternatives | Description |
| --- | --- | --- |
| MCU | Arduino Nano or ESP8266 | Controls everything |
| Display | LCD 2x24 or OLED 128x64 | Display the current temperature and humidity |
| Temperature Sensor | DHT11, DHT22 or DS18B20 | Reads the temperature and humidity inside the box |
| FAN | 12V 80mm | Moves the air inside the box |
| Heatsink | 80x80x10mm | Used to increase the surface area and heat more air in contact |
| 3d printer hotend | - | Used to heat the air inside the box |
| 100K NTC Thermistor | - | Used to read the temperature of the hotend |
| 12V 5A~ PSU | I'm using a PSU with 10A here just to be sure | Provides power |
| 5V to 3.3V Buck Converter | Not really needed if your board already has a tension regulator embedded | Found that DHT sensors work better at 3.3v |
| IRLZ44N | - | Used to control the heater output voltage using PWM |
| 2N2222 | - | Used to control the FAN output voltage using PWM |
| Resistors, capacitors, connectors, etc | - | - |
| 3/16" screws, washers and nuts | - | Used to hold parts onto the box |
| 60L Foam box | - | Used to isolate the air inside the box |
| 3D printed parts | - | Used to hold the parts together |

Some other stuff that helps:
- IR Thermal gun
- Multimeter
- Soldering iron
- 3D printer (duh)

And some other stuff I forgot to mention here.

## Schematic

I have a KiCad schematic somewhere, someday I will upload it here :p

## Resources:

### Inspiration

- Marlon Nardi's [video](https://www.youtube.com/watch?v=Z5tCAvwi9p4&t=370s) DryBox project which I took heavy inspiration from
- Mighty studio's [video](https://www.youtube.com/watch?v=R93u8XLAhz8) another DryBox project which I took heavy inspiration from
- 3DMaker's [Commercial DryBox](https://www.youtube.com/watch?v=S7JMmGDJ5gU)
- Ricky Impley's [amazing essay on Dryers](https://www.youtube.com/watch?v=8ET9EJrA0v0)
- METER Group [video](https://www.youtube.com/watch?v=EXjbjIgTgsA) about absolute humidity

### OSS Projects

- Sam Knight's [PWM.h](https://forum.arduino.cc/t/pwm-frequency-library/114988)
- Max Mayfield's Thermistor2. Which I can't find the original source anymore.
- Adafruit's [DHT sensor library](https://github.com/adafruit/DHT-sensor-library)