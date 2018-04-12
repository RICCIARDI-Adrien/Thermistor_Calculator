/** @file Main.c
 * Compute the ADC lookup table (containing Celsius temperatures) corresponding to a specific thermistor voltage, taking into account the voltage divider the thermistor is connected to.
 * @author Adrien RICCIARDI
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//-------------------------------------------------------------------------------------------------
// Private constants
//-------------------------------------------------------------------------------------------------
/** Maximum allowed amount of ADC steps. */
#define MAIN_MAXIMUM_ADC_RESOLUTION 65536 // 16-bit ADC

//-------------------------------------------------------------------------------------------------
// Private types
//-------------------------------------------------------------------------------------------------
/** All computed values for a precise ADC value. */
typedef struct
{
	double Voltage_Divider_Output_Voltage; //!< The bridge output voltage (volts).
	double Thermistor_Resistance; //!< The thermistor resistance (ohms).
	double Thermistor_Temperature; //!< The thermistor temperature (Celsius).
} TMainComputedValues;

//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
/** All computed values for all requested ADC values. */
static TMainComputedValues Values[MAIN_MAXIMUM_ADC_RESOLUTION];

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
		"Usage : %s [-c circuit] [-B beta] [-R r25] [-r resistor] [-v Vcc] [-a resolution]\n"
		"  -c : circuit variant, it can be 1 or 2 (see above for circuit variants description). Default value is 1.\n"
		"  -B : thermistor Beta coefficient (kelvin), this is the B25/100 value of the datasheet. Default value is 4300.\n"
		"  -R : thermistor resistance at 25 Celsius degrees (ohm), floating numbers are allowed. Default value is 10000.\n"
		"  -r : voltage divider bridge other resistance value (ohm), floating numbers are allowed. Default value is 10000.\n"
		"  -v : Vcc voltage (volt), floating numbers are allowed. Default value is 3.3.\n"
		"  -a : ADC resolution (or how many values you want in the lookup table). Default value is 256.\n"
		"  -h : display this help.\n"
		, Pointer_String_Program_Name);
}

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

/** Determine the thermistor Celsius temperature for a given thermistor resistance.
 * @param Thermistor_Beta_Coefficient Beta coefficient value.
 * @param Thermistor_Reference_Resistance Thermistor R25 resistor value (ohms).
 * @param Thermistor_Resistance The resistance to get temperature from (ohms).
 * @return The corresponding temperature (Celsius).
 */
static double MainComputeThermistorTemperature(double Thermistor_Beta_Coefficient, double Thermistor_Reference_Resistance, double Thermistor_Resistance)
{
	double Kelvin_Result;
	
	Kelvin_Result = 1. / ((log(Thermistor_Resistance / Thermistor_Reference_Resistance) / Thermistor_Beta_Coefficient) + (1. / (273.15 + 25.)));
	return Kelvin_Result - 273.15;
}

//-------------------------------------------------------------------------------------------------
// Entry point
//-------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	unsigned int i, ADC_Resolution = 256;
	int Circuit_Variant = 1, Parameter;
	double Voltage_Divider_Bridge_Voltage = 3.3, Voltage_Divider_Resistor = 10000., Thermistor_Beta_Coefficient = 4300., Thermistor_Reference_Resistance = 10000.;
	
	// Display banner
	puts("+-------------------------------------------------+");
	puts("| Thermistor calculator (C) 2018 Adrien RICCIARDI |");
	puts("+-------------------------------------------------+");
	
	// Extract parameters
	while (1)
	{
		Parameter = getopt(argc, argv, "B:R:a:c:hr:v:");
		if (Parameter == -1) break;
		
		switch (Parameter)
		{
			case 'B':
				if (sscanf(optarg, "%lf", &Thermistor_Beta_Coefficient) != 1)
				{
					printf("Error : invalid thermistor beta coefficient value.\n\n");
					MainDisplayProgramUsage(argv[0]);
					return EXIT_FAILURE;
				}
				break;
				
			case 'R':
				if (sscanf(optarg, "%lf", &Thermistor_Reference_Resistance) != 1)
				{
					printf("Error : invalid thermistor reference resistance (R25) value.\n\n");
					MainDisplayProgramUsage(argv[0]);
					return EXIT_FAILURE;
				}
				break;
			
			case 'a':
				if (sscanf(optarg, "%u", &ADC_Resolution) != 1)
				{
					printf("Error : invalid ADC resolution value.\n\n");
					MainDisplayProgramUsage(argv[0]);
					return EXIT_FAILURE;
				}
				// Make sure the results array has enough room
				if (ADC_Resolution > MAIN_MAXIMUM_ADC_RESOLUTION)
				{
					printf("Error : maximum allowed ADC resolution is %u.\n", MAIN_MAXIMUM_ADC_RESOLUTION);
					return EXIT_FAILURE;
				}
				break;
				
			case 'c':
				if (sscanf(optarg, "%d", &Circuit_Variant) != 1)
				{
					printf("Error : invalid circuit variant value.\n\n");
					MainDisplayProgramUsage(argv[0]);
					return EXIT_FAILURE;
				}
				if ((Circuit_Variant < 1) || (Circuit_Variant > 2))
				{
					printf("Error : circuit variant value must be 1 or 2.\n\n");
					MainDisplayProgramUsage(argv[0]);
					return EXIT_FAILURE;
				}
				break;
				
			case 'h':
				MainDisplayProgramUsage(argv[0]);
				return EXIT_SUCCESS;
				
			case 'r':
				if (sscanf(optarg, "%lf", &Voltage_Divider_Resistor) != 1)
				{
					printf("Error : invalid voltage divider resistor value.\n\n");
					MainDisplayProgramUsage(argv[0]);
					return EXIT_FAILURE;
				}
				break;
				
			case 'v':
				if (sscanf(optarg, "%lf", &Voltage_Divider_Bridge_Voltage) != 1)
				{
					printf("Error : invalid voltage divider bridge voltage value.\n\n");
					MainDisplayProgramUsage(argv[0]);
					return EXIT_FAILURE;
				}
				break;
				
			case '?':
				putchar('\n');
				MainDisplayProgramUsage(argv[0]);
				return EXIT_FAILURE;
			
			default:
				break;
		}
	}
	
	// Compute values
	for (i = 0; i < ADC_Resolution; i++)
	{
		Values[i].Voltage_Divider_Output_Voltage = MainComputeVoltageDividerOutputVoltage(Voltage_Divider_Bridge_Voltage, ADC_Resolution, i);
		Values[i].Thermistor_Resistance = MainComputeThermistorResistance(Circuit_Variant, Voltage_Divider_Bridge_Voltage, Values[i].Voltage_Divider_Output_Voltage, Voltage_Divider_Resistor);
		Values[i].Thermistor_Temperature = MainComputeThermistorTemperature(Thermistor_Beta_Coefficient, Thermistor_Reference_Resistance, Values[i].Thermistor_Resistance);
	}
	
	// Display results
	printf("ADC value	Thermistor voltage (V)	Thermistor resistance (ohm)	Thermistor temperature (Celsius)\n");
	for (i = 0; i < ADC_Resolution; i++) printf("%d		%lf		%lf			%lf\n", i, Values[i].Voltage_Divider_Output_Voltage, Values[i].Thermistor_Resistance, Values[i].Thermistor_Temperature);
	
	return EXIT_SUCCESS;
}
