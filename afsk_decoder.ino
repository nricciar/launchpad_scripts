/*
 A start of a AFSK decoder for the Tiva C Series EK-TM4C1294XL
 
 This code is not complete, and can only do some basic tone detection
 currently. Will not decode AFSK signals yet.
 
 */
#include "Energia.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"

#define FREQ_MARK 1200
#define FREQ_SPACE 2200
#define FREQ_SAMP 48016
#define BAUD 1200

int     sk,mk,i;
float   floatnumSamples;
float   somega,ssine,scosine,scoeff,momega,msine,mcosine,mcoeff,q0,q1,q2,i0,i1,i2,magnitude,real,imag;

float   scalingFactor = 40 / 2.0;
int     count = 0;
int     sample_position = 0;

void SamplerInterrupt(void)
{
  // clear the timer interrupt
  ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

  float tmp = analogRead(A0);

  // mark
  q0 = mcoeff * q1 - q2 + tmp;
  q2 = q1;
  q1 = q0;

  // space
  i0 = scoeff * i1 - i2 + tmp;
  i2 = i1;
  i1 = i0;
  sample_position++;

  if (sample_position == 40) {
    real = (q1 - q2 * mcosine) / scalingFactor;
    imag = (q2 * msine) / scalingFactor;

    // check if signal is mark
    magnitude = sqrtf(real*real + imag*imag);
    if (magnitude > 500) {
      Serial.print("1");
    } 
    else {
      real = (i1 - i2 * scosine) / scalingFactor;
      imag = (i2 * ssine) / scalingFactor;

      // check if signal is space
      magnitude = sqrtf(real*real + imag*imag);
      if (magnitude > 500) {
        Serial.print("0");
      }
    }

    // reset
    q0=0;
    q1=0;
    q2=0;
    i0=0;
    i1=0;
    i2=0;
    sample_position = 0;
  }

  // for debug
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

  // setup goertzel variables for mark detection
  floatnumSamples = (float) 128;
  mk = (int) (0.5 + ((floatnumSamples * FREQ_MARK) / FREQ_SAMP));
  momega = (2.0 * M_PI * mk) / floatnumSamples;
  msine = sin(momega);
  mcosine = cos(momega);
  mcoeff = 2.0 * mcosine;
  q0=0;
  q1=0;
  q2=0;

  // setup goertzel variables for space detection
  sk = (int) (0.5 + ((floatnumSamples * FREQ_SPACE) / FREQ_SAMP));
  somega = (2.0 * M_PI * sk) / floatnumSamples;
  ssine = sin(somega);
  scosine = cos(somega);
  scoeff = 2.0 * scosine;
  i0=0;
  i1=0;
  i2=0;
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
