#include "Energia.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"

#define ARM_MATH_CM4
#ifndef __FPU_PRESENT
#define __FPU_PRESENT 1
#endif
 
#include <arm_math.h>
#include <arm_common_tables.h>
 
#define USE_STATIC_INIT

#define TEST_LENGTH_SAMPLES 128 

static float32_t testInput_f32_10khz[TEST_LENGTH_SAMPLES]; 
static float32_t testOutput[TEST_LENGTH_SAMPLES/2]; 
int count = 0;
int position = 0;
uint32_t fftSize = 64; 
uint32_t ifftFlag = 0; 
uint32_t doBitReverse = 1; 
 
/* Reference index at which max energy of bin ocuurs */ 
uint32_t refIndex = 213, testIndex = 0; 
arm_status status; 
arm_cfft_radix4_instance_f32 S; 
float32_t maxValue;
int sample_rate = 0;

void SamplerInterrupt(void)
{
  // clear the timer interrupt
  ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

  testInput_f32_10khz[position] = analogRead(A0) - 2050;
  position++;
  
  if (position == TEST_LENGTH_SAMPLES)
  {
    arm_cfft_radix4_f32(&S, testInput_f32_10khz); 
    arm_cmplx_mag_f32(testInput_f32_10khz, testOutput, fftSize); 
    arm_max_f32(testOutput, fftSize, &maxValue, &testIndex);  
    position = 0;
  }
  
  count++;
}

void setup()
{
  // init serial port
  Serial.begin(9600);
  SysCtlADCSpeedSet(SYSCTL_ADCSPEED_1MSPS);
  
  // initialise the sampler interrupt
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
  TimerIntRegister(TIMER0_BASE, TIMER_A, SamplerInterrupt);
  ROM_TimerEnable(TIMER0_BASE, TIMER_A);
  ROM_IntEnable(INT_TIMER0A);
  ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

  status = ARM_MATH_SUCCESS; 
   
  /* Initialize the CFFT/CIFFT module */  
  status = arm_cfft_radix4_init_f32(&S, fftSize, ifftFlag, doBitReverse); 
}

void loop()
{
  // start the timer then loop forever
  ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, 3072); // 2498 =~ 48000Hz
  while (1)
  {
    Serial.print(testIndex, DEC);
    Serial.print(" -- ");
    Serial.print(maxValue, DEC);
    Serial.print(" -- ");
    Serial.println(count);
    
    sample_rate = count;
    count = 0;
    
    delay(1000);
  }
}
