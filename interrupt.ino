/*
 A Timer Interrupt Example
 */
#include "Energia.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"

void SamplerInterrupt(void)
{
  // clear the timer interrupt
  ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

  count++;
}

void setup()
{
  // init serial port
  Serial.begin(9600);

  // initialise the sampler interrupt
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
  TimerIntRegister(TIMER0_BASE, TIMER_A, SamplerInterrupt);
  ROM_TimerEnable(TIMER0_BASE, TIMER_A);
  ROM_IntEnable(INT_TIMER0A);
  ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}

void loop()
{
  // start the timer then loop forever
  ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, 2498); // 2498 =~ 48000Hz
  while (1)
  {
    Serial.println(count);
    count = 0;
    delay(1000);
  }
}
