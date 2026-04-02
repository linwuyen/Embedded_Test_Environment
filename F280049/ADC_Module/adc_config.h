/*
 * adc_config.h
 *
 * ADC Configuration Parameters
 */

#ifndef ADC_MODULE_ADC_CONFIG_H_
#define ADC_MODULE_ADC_CONFIG_H_

// ADC Configuration
#define ADC_SAMPLE_WINDOW       15      // Sample window cycles
#define ADC_RESOLUTION          4096    // 12-bit ADC
#define ADC_VREF                3.3f    // Reference voltage

// ADC Channel Mapping
#define ADC_CH_CURRENT_A        ADC_SOC_NUMBER0
#define ADC_CH_CURRENT_B        ADC_SOC_NUMBER1
#define ADC_CH_CURRENT_C        ADC_SOC_NUMBER2
#define ADC_CH_VOLTAGE_DC       ADC_SOC_NUMBER3

// Current Sensing Parameters
#define CURRENT_SENSOR_GAIN     0.1f    // V/A
#define CURRENT_OFFSET          1.65f   // V (mid-rail)
#define CURRENT_MAX             10.0f   // A

// Voltage Sensing Parameters
#define VOLTAGE_DIVIDER_RATIO   0.1f    // Voltage divider ratio
#define VOLTAGE_MAX             330.0f  // V

// Protection Thresholds
#define OVERCURRENT_THRESHOLD   8.0f    // A
#define OVERVOLTAGE_THRESHOLD   350.0f  // V
#define UNDERVOLTAGE_THRESHOLD  100.0f  // V

// Filter Parameters
#define ADC_FILTER_ALPHA        0.1f    // Low-pass filter coefficient (0-1)

#endif /* ADC_MODULE_ADC_CONFIG_H_ */
