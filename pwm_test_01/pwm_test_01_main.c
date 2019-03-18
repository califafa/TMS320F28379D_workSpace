
//
// Included Files
//
#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"


void initEPWMGPIO(void)
{
    //
    // Disable pull up on GPIO 0 and GPIO 2 and configure them as PWM1A and
    // PWM2A output respectively.
    //
    GPIO_setPadConfig(0, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(GPIO_0_EPWM1A);

    GPIO_setPadConfig(2, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(GPIO_2_EPWM2A);
}

void initEPWM1()
{

    //
    // Set-up TBCLK
    //
    EPWM_setTimeBasePeriod(EPWM1_BASE, 10000U);
    EPWM_setPhaseShift(EPWM1_BASE, 0U);
    EPWM_setTimeBaseCounter(EPWM1_BASE, 0U);
    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_UP_DOWN);
    EPWM_disablePhaseShiftLoad(EPWM1_BASE);

    //
    // Set ePWM clock pre-scaler
    //
    EPWM_setClockPrescaler(EPWM1_BASE,
                           EPWM_CLOCK_DIVIDER_4,
                           EPWM_HSCLOCK_DIVIDER_4);

    //
    // Set up shadowing
    //
    EPWM_setCounterCompareShadowLoadMode(EPWM1_BASE,
                                         EPWM_COUNTER_COMPARE_A,
                                         EPWM_COMP_LOAD_ON_CNTR_ZERO);

    //
    // Set-up compare
    //
    EPWM_setCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_A, 1000U);

    //
    // Set actions
    //
    EPWM_setActionQualifierAction(EPWM1_BASE,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
    EPWM_setActionQualifierAction(EPWM1_BASE,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);
}


void initEPWM2()
{

    //
    // Set-up TBCLK
    //
    EPWM_setTimeBasePeriod(EPWM2_BASE, 5000U);
    EPWM_setPhaseShift(EPWM2_BASE, 0U);
    EPWM_setTimeBaseCounter(EPWM2_BASE, 0U);
    EPWM_setTimeBaseCounterMode(EPWM2_BASE, EPWM_COUNTER_MODE_UP_DOWN);
    EPWM_disablePhaseShiftLoad(EPWM2_BASE);

    //
    // Set ePWM clock pre-scaler
    //
    EPWM_setClockPrescaler(EPWM2_BASE,
                           EPWM_CLOCK_DIVIDER_4,
                           EPWM_HSCLOCK_DIVIDER_4);

    //
    // Set-up compare
    //
    EPWM_setCounterCompareValue(EPWM2_BASE, EPWM_COUNTER_COMPARE_A, 500U);

    //
    // Set actions
    //
    EPWM_setActionQualifierAction(EPWM2_BASE,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
    EPWM_setActionQualifierAction(EPWM2_BASE,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);
}


//
// Main
//
void main(void)
{
    Device_init();
    Device_initGPIO();

    initEPWMGPIO();

    //
    // Disable sync(Freeze clock to PWM as well)
    //
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
    //
    // Initialize ePWM1 and ePWM2
    //
    initEPWM1();
    initEPWM2();
    //
    // Enable sync and clock to PWM
    //
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    while(1){
        NOP;
    }
}

//
// End of File
//
