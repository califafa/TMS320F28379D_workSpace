
//
// Included Files
//
#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"

uint32_t epwm1TZIntCount;
uint32_t epwm2TZIntCount;



void initEPWM1(){

    EPWM_enableTripZoneSignals(EPWM1_BASE,EPWM_TZ_SIGNAL_OSHT1);
    EPWM_setTripZoneAction(EPWM1_BASE,EPWM_TZ_ACTION_EVENT_TZA,EPWM_TZ_ACTION_HIGH);
    EPWM_enableTripZoneInterrupt(EPWM1_BASE,EPWM_TZ_INTERRUPT_OST);

    EPWM_setTimeBasePeriod(EPWM1_BASE,12000U);
    EPWM_setPhaseShift(EPWM1_BASE,0);
    EPWM_setTimeBaseCounter(EPWM1_BASE,0);
    EPWM_setTimeBaseCounterMode(EPWM1_BASE,EPWM_COUNTER_MODE_UP_DOWN);
    EPWM_disablePhaseShiftLoad(EPWM1_BASE);

    EPWM_setClockPrescaler(EPWM1_BASE,EPWM_CLOCK_DIVIDER_4,EPWM_HSCLOCK_DIVIDER_4);

    EPWM_setCounterCompareShadowLoadMode(EPWM1_BASE,EPWM_COUNTER_COMPARE_A,EPWM_COMP_LOAD_ON_CNTR_ZERO);

    EPWM_setCounterCompareValue(EPWM1_BASE,EPWM_COUNTER_COMPARE_A,600U);

    EPWM_setActionQualifierAction(EPWM1_BASE,EPWM_AQ_OUTPUT_A,EPWM_AQ_OUTPUT_LOW,EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
    EPWM_setActionQualifierAction(EPWM1_BASE,EPWM_AQ_OUTPUT_A,EPWM_AQ_OUTPUT_HIGH,EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);

}

void initEPWM2(){

    //
    // Enable TZ1 as one cycle-by-cycle trip sources
    //
    EPWM_enableTripZoneSignals(EPWM2_BASE, EPWM_TZ_SIGNAL_CBC1);

    //
    // Action on TZ1
    //
    EPWM_setTripZoneAction(EPWM2_BASE,
                           EPWM_TZ_ACTION_EVENT_TZA,
                           EPWM_TZ_ACTION_HIGH);

    //
    // Enable TZ interrupt
    //
    EPWM_enableTripZoneInterrupt(EPWM2_BASE,
                                 EPWM_TZ_INTERRUPT_CBC);

    //
    // Set-up TBCLK
    //
    EPWM_setTimeBasePeriod(EPWM2_BASE, 6000U);
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
    EPWM_setCounterCompareValue(EPWM2_BASE, EPWM_COUNTER_COMPARE_A, 3000U);

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

void initTZGPIO(){
    //
    // Set GPIO 12 as as Asynchronous input with pull up enabled
    //
    GPIO_setPadConfig(12, GPIO_PIN_TYPE_PULLUP);
    GPIO_setPinConfig(GPIO_12_GPIO12);
    GPIO_setDirectionMode(12, GPIO_DIR_MODE_IN);
    GPIO_setQualificationMode(12, GPIO_QUAL_ASYNC);

    //
    // Set GPIO 12 as TZ1 input
    //
    XBAR_setInputPin(XBAR_INPUT1, 12);

    //
    // Configure GPIO 11 as general purpose GPIO for monitoring when the TZ
    // Interrupt has been entered
    //
    GPIO_setPadConfig(11, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(GPIO_11_GPIO11);
    GPIO_setDirectionMode(11, GPIO_DIR_MODE_OUT);
}

void initEPWMGPIO(){
    //
    // Disable pull up on GPIO 0 and GPIO 2 and configure them as PWM1A and
    // PWM2A output respectively.
    //
    GPIO_setPadConfig(0, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(GPIO_0_EPWM1A);

    GPIO_setPadConfig(2, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(GPIO_2_EPWM2A);
}

interrupt void epwm1TZISR(){
    epwm1TZIntCount++;
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP2);
}

interrupt void epwm2TZISR(){
    epwm2TZIntCount++;
    GPIO_togglePin(11);

    EPWM_clearTripZoneFlag(EPWM2_BASE,(EPWM_TZ_INTERRUPT | EPWM_TZ_FLAG_CBC));

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP2);
}

//
// Main
//
void main(void)
{

    epwm1TZIntCount=0;
    epwm2TZIntCount=0;

    Device_init();
    Device_initGPIO();
    Interrupt_initModule();
    Interrupt_initVectorTable();
    Interrupt_register(INT_EPWM1_TZ,&epwm1TZISR);
    Interrupt_register(INT_EPWM2_TZ,&epwm2TZISR);

    initEPWMGPIO();
    initTZGPIO();

    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
    initEPWM1();
    initEPWM2();
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    Interrupt_enable(INT_EPWM1_TZ);
    Interrupt_enable(INT_EPWM2_TZ);

    EINT;
    ERTM;

    while(1){
        NOP;
    }

}

//
// End of File
//
