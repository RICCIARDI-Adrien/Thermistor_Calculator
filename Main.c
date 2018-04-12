/** @file Main.c
 * Compute the ADC lookup table (containing Celsius temperatures) corresponding to a specific thermistor voltage, taking into account the voltage divider the thermistor is connected to.
 * @author Adrien RICCIARDI
 */
#include <stdio.h>

//-------------------------------------------------------------------------------------------------
// Private constants
//-------------------------------------------------------------------------------------------------
/** Maximum allowed amount of ADC steps. */
#define MAIN_MAXIMUM_ADC_STEPS_COUNT 65536 // 16-bit ADC

//-------------------------------------------------------------------------------------------------
// Private types
//-------------------------------------------------------------------------------------------------
/** All computed values for a precise ADC value. */
typedef struct
{
	double Voltage_Divider_Output_Voltage; //!< The bridge output voltage (volts).
	double Thermistor_Resistance; //!< The thermistor resistance (ohms).
} TMainComputedValues;

//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
/** All computed values for all requested ADC values. */
static TMainComputedValues Values[MAIN_MAXIMUM_ADC_STEPS_COUNT];

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
/** Display the program usage help.
 * @param Pointer_String_Program_Name The name the program has been executed with.
 */
static void MainDisplayProgramUsage(char *Pointer_String_Program_Name)
{
	printf("Compute the ADC lookup table (containing Celsius temperatures) corresponding to a specific thermistor voltage, taking into account the voltage divider the thermistor is connected to.\n"
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

/** Compute the thermistor voltage corresponding to an ADC value.
 * @param Circuit_Variant The selected circuit variant (must be 1 or 2).
 * @param Voltage_Divider_Bridge_Voltage Vcc voltage in volts.
 * @param Voltage_Divider_Resistor Resistor value in ohms.
 * @param ADC_Value The ADC value (in range [0; ADC_resolution-1].
 * @return The corresponding voltage in volts.
 */
//static MainComputeThermistorVoltage(int Circuit_Variant, double Voltage_Divider_Bridge_Voltage, double Voltage_Divider_Resistor, unsigned int ADC_Value)

/** Compute the voltage divider output voltage corresponding to an ADC value.
 * @param Voltage_Divider_Bridge_Voltage Vcc voltage in volts.
 * @param ADC_Resolution The ADC resolution (corresponding to the amount of ADC steps). For instance, set 256 for a 8-bit ADC.
 * @param ADC_Value The ADC value (in range [0; ADC_resolution-1]).
 * @return The corresponding voltage in volts.
 */
static double MainComputeVoltageDividerOutputVoltage(double Voltage_Divider_Bridge_Voltage, unsigned int ADC_Resolution, unsigned int ADC_Value)
{
	return Voltage_Divider_Bridge_Voltage * ADC_Value / (ADC_Resolution - 1); // Subtract 1 to resolution because the maximum reachable ADC value is resolution-1
}

/** Compute the thermistor resistance corresponding to a specific voltage divider output voltage.
 * @param Circuit_Variant The selected circuit variant (must be 1 or 2).
 * @param Voltage_Divider_Bridge_Voltage Vcc voltage in volts.
 * @param Voltage_Divider_Output_Voltage The voltage divider output voltage to get corresponding thermistor resistance (volts).
 * @param Voltage_Divider_Resistor The bridge resistor value in ohms.
 * @return The corresponding thermistor resistance (ohms).
 */
static double MainComputeThermistorResistance(int Circuit_Variant, double Voltage_Divider_Bridge_Voltage, double Voltage_Divider_Output_Voltage, double Voltage_Divider_Resistor)
{
	double Result;
	
	// Compute voltage divider second resistance value
	if (Circuit_Variant == 1) Result = Voltage_Divider_Output_Voltage * Voltage_Divider_Resistor / (Voltage_Divider_Bridge_Voltage - Voltage_Divider_Output_Voltage);
	// Compute voltage divider first resistance value
	else Result = (Voltage_Divider_Bridge_Voltage * Voltage_Divider_Resistor / Voltage_Divider_Output_Voltage) - Voltage_Divider_Resistor;
	
	return Result;
}

//-------------------------------------------------------------------------------------------------
// Entry point
//-------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	unsigned int i, ADC_Resolution = 256;
	
	// Display banner
	puts("+-------------------------------------------------+");
	puts("| Thermistor calculator (C) 2018 Adrien RICCIARDI |");
	puts("+-------------------------------------------------+");
	
	// Check parameters
	// TODO
	//MainDisplayProgramUsage(argv[0]);
	
	// TEST
	for (i = 0; i < 256; i++)
	{
		Values[i].Voltage_Divider_Output_Voltage = MainComputeVoltageDividerOutputVoltage(3.3, 256, i);
		Values[i].Thermistor_Resistance = MainComputeThermistorResistance(1, 3.3, Values[i].Voltage_Divider_Output_Voltage, 10000);
	}
	
	// Display results
	printf("ADC value	Thermistor voltage (V)	Thermistor resistance (ohm)\n");
	for (i = 0; i < ADC_Resolution; i++) printf("%d		%f		%f\n", i, Values[i].Voltage_Divider_Output_Voltage, Values[i].Thermistor_Resistance);
	
	return 5;
}
