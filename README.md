# Thermistor Calculator

## Building

Use `make` to build the program.

## Usage

Simply execute `./thermistor-calculator -h` to display program full help.  
Here is what the help looks like :
```
ar@ar-i7:~/Documents/Thermistor_Calculator$ ./thermistor-calculator -h
+-------------------------------------------------+
| Thermistor calculator (C) 2018 Adrien RICCIARDI |
+-------------------------------------------------+
Compute the ADC lookup table (containing Celsius temperatures) corresponding to a specific thermistor voltage, taking into account the voltage divider the thermistor is connected to.
For now, only Negative Temperature Coefficient thermistors are supported.

Here are the two voltage divider circuits that are supported by the program :

Circuit variant 1        Circuit variant 2
-----------------        -----------------

      Vcc                      Vcc
       |                        |
      +-+                      +-+
      | | Resistor             | | NTC
      +-+                      +-+
       |                        |
       |--- Vntc                |--- Vntc
       |                        |
      +-+                      +-+
      | | NTC                  | | Resistor
      +-+                      +-+
       |                        |
      GND                      GND

Usage : ./thermistor-calculator [-c circuit] [-B beta] [-R r25] [-r resistor] [-v Vcc] [-a resolution]
  -c : circuit variant, it can be 1 or 2 (see above for circuit variants description). Default value is 1.
  -B : thermistor Beta coefficient (kelvin), this is the B25/100 value of the datasheet. Default value is 4300.
  -R : thermistor resistance at 25 Celsius degrees (ohm), floating numbers are allowed. Default value is 10000.
  -r : voltage divider bridge other resistance value (ohm), floating numbers are allowed. Default value is 10000.
  -v : Vcc voltage (volt), floating numbers are allowed. Default value is 3.3.
  -a : ADC resolution (or how many values you want in the lookup table). Default value is 256.
  -h : display this help.
```
