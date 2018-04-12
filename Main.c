/** @file Main.c
 * Compute the ADC lookup table (containing Celsius temperatures) corresponding to a specific thermistor voltage, taking into account the voltage divider the thermistor is connected to.
 * @author Adrien RICCIARDI
 */
#include <stdio.h>

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
/** Display the program usage help.
 * @param Pointer_String_Program_Name The name the program has been executed with.
 */
static void MainDisplayProgramUsage(char *Pointer_String_Program_Name)
{
	printf("+-------------------------------------------------+\n"
		"| Thermistor calculator (C) 2018 Adrien RICCIARDI |\n"
		"+-------------------------------------------------+\n"
		"Compute the ADC lookup table (containing Celsius temperatures) corresponding to a specific thermistor voltage, taking into account the voltage divider the thermistor is connected to.\n"
		"For now, only Negative Temperature Coefficient thermistors are supported.\n"
		"\n"
		"Here are the two voltage divider circuits that are supported by the program :\n"
		"\n"
		"Circuit variant 1        Circuit variant 2\n"
		"-----------------        -----------------\n"
		"\n"
		"      Vcc                      Vcc\n"
		"       |                        |\n"
		"      +-+                      +-+\n"
		"      | | Resistor             | | NTC\n"
		"      +-+                      +-+\n"
		"       |                        |\n"
		"       |--- Vntc                |--- Vntc\n"
		"       |                        |\n"
		"      +-+                      +-+\n"
		"      | | NTC                  | | Resistor\n"
		"      +-+                      +-+\n"
		"       |                        |\n"
		"      GND                      GND\n"
		"\n"
		"Usage : %s -c circuit -B beta -R r25 -r resistor -v Vcc -a resolution\n"
		"  -c : circuit variant, it can be 1 or 2 (see above for circuit variants description)\n"
		"  -B : thermistor Beta coefficient (kelvin), this is the B25/100 value of the datasheet\n"
		"  -R : thermistor resistance at 25 Celsius degrees (ohm), floating numbers are allowed\n"
		"  -r : voltage divider bridge other resistance value (ohm), floating numbers are allowed\n"
		"  -v : Vcc voltage (volt), floating numbers are allowed\n"
		"  -a : ADC resolution (or how many values you want in the lookup table)\n"
		, Pointer_String_Program_Name);
}

//-------------------------------------------------------------------------------------------------
// Entry point
//-------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	// Check parameters
	
	MainDisplayProgramUsage(argv[0]);
	return 5;
}
