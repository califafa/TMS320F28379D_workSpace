
//GPIO104 -> 1Hz    timer0
//GPIO105 -> 1Hz    timer1
//GPIO22 -> 1Hz    timer2


//
// Included Files
//
#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"


//
// Globals
//
uint16_t cpuTimer0IntCount;
uint16_t cpuTimer1IntCount;
uint16_t cpuTimer2IntCount;

//
// Function Prototypes
//
__interrupt void cpuTimer0ISR(void);
__interrupt void cpuTimer1ISR(void);
__interrupt void cpuTimer2ISR(void);
void initCPUTimers(void);
void configCPUTimer(uint32_t, float, float);

void gpioInit(){
    GPIO_setDirectionMode(104,GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(105,GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(22,GPIO_DIR_MODE_OUT);
    GPIO_writePin(22,1);
}
//
// Main
//
void main(void)
{
    //
    // Initializes device clock and peripherals
    //
    Device_init();

    //
    // Configures the GPIO pin as a push-pull output
    //
    Device_initGPIO();
    GPIO_setPinConfig(DEVICE_GPIO_CFG_LED1);
    GPIO_setMasterCore(DEVICE_GPIO_PIN_LED1, GPIO_CORE_CPU1);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_LED1, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_LED1, GPIO_DIR_MODE_OUT);

    //
    // Initializes PIE and clears PIE registers. Disables CPU interrupts.
    //
    Interrupt_initModule();

    //
    // Initializes the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();

    //
    // ISRs for each CPU Timer interrupt
    //
    Interrupt_register(INT_TIMER0, &cpuTimer0ISR);
    Interrupt_register(INT_TIMER1, &cpuTimer1ISR);
    Interrupt_register(INT_TIMER2, &cpuTimer2ISR);

    //
    // Initializes the Device Peripheral. For this example, only initialize the
    // Cpu Timers.
    //
    initCPUTimers();

    //
    // Configure CPU-Timer 0, 1, and 2 to interrupt every second:
    // 1 second Period (in uSeconds)
    //
    configCPUTimer(CPUTIMER0_BASE, DEVICE_SYSCLK_FREQ, 1000000);    //1sec == 1000000 microSec
    configCPUTimer(CPUTIMER1_BASE, DEVICE_SYSCLK_FREQ, 1000000);    //1sec
    configCPUTimer(CPUTIMER2_BASE, DEVICE_SYSCLK_FREQ, 1000000);    //1sec

    //
    // To ensure precise timing, use write-only instructions to write to the
    // entire register. Therefore, if any of the configuration bits are changed
    // in configCPUTimer and initCPUTimers, the below settings must also
    // be updated.
    //
    CPUTimer_enableInterrupt(CPUTIMER0_BASE);
    CPUTimer_enableInterrupt(CPUTIMER1_BASE);
    CPUTimer_enableInterrupt(CPUTIMER2_BASE);

    //
    // Enables CPU int1, int13, and int14 which are connected to CPU-Timer 0,
    // CPU-Timer 1, and CPU-Timer 2 respectively.
    // Enable TINT0 in the PIE: Group 1 interrupt 7
    //
    Interrupt_enable(INT_TIMER0);
    Interrupt_enable(INT_TIMER1);
    Interrupt_enable(INT_TIMER2);

    //
    // Starts CPU-Timer 0, CPU-Timer 1, and CPU-Timer 2.
    //
    CPUTimer_startTimer(CPUTIMER0_BASE);
    CPUTimer_startTimer(CPUTIMER1_BASE);
    CPUTimer_startTimer(CPUTIMER2_BASE);

    //
    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    //
    EINT;
    ERTM;

    gpioInit();

    //
    // IDLE loop. Just sit and loop forever (optional)
    //
    while(1)
    {
    }
}

//
// initCPUTimers - This function initializes all three CPU timers
// to a known state.
//
void
initCPUTimers(void)
{
    //
    // Initialize timer period to maximum
    //
    CPUTimer_setPeriod(CPUTIMER0_BASE, 0xFFFFFFFF);
    CPUTimer_setPeriod(CPUTIMER1_BASE, 0xFFFFFFFF);
    CPUTimer_setPeriod(CPUTIMER2_BASE, 0xFFFFFFFF);

    //
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT)
    //
    CPUTimer_setPreScaler(CPUTIMER0_BASE, 0);
    CPUTimer_setPreScaler(CPUTIMER1_BASE, 0);
    CPUTimer_setPreScaler(CPUTIMER2_BASE, 0);

    //
    // Make sure timer is stopped
    //
    CPUTimer_stopTimer(CPUTIMER0_BASE);
    CPUTimer_stopTimer(CPUTIMER1_BASE);
    CPUTimer_stopTimer(CPUTIMER2_BASE);

    //
    // Reload all counter register with period value
    //
    CPUTimer_reloadTimerCounter(CPUTIMER0_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER1_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER2_BASE);

    //
    // Reset interrupt counter
    //
    cpuTimer0IntCount = 0;
    cpuTimer1IntCount = 0;
    cpuTimer2IntCount = 0;
}

//
// configCPUTimer - This function initializes the selected timer to the
// period specified by the "freq" and "period" parameters. The "freq" is
// entered as Hz and the period in uSeconds. The timer is held in the stopped
// state after configuration.
//
void
configCPUTimer(uint32_t cpuTimer, float freq, float period)
{
    uint32_t temp;

    //
    // Initialize timer period:
    //
    temp = (uint32_t)(freq / 1000000 * period);
    CPUTimer_setPeriod(cpuTimer, temp);

    //
    // Set pre-scale counter to divide by 1 (SYSCLKOUT):
    //
    CPUTimer_setPreScaler(cpuTimer, 0);

    //
    // Initializes timer control register. The timer is stopped, reloaded,
    // free run disabled, and interrupt enabled.
    // Additionally, the free and soft bits are set
    //
    CPUTimer_stopTimer(cpuTimer);
    CPUTimer_reloadTimerCounter(cpuTimer);
    CPUTimer_setEmulationMode(cpuTimer,
                              CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
    CPUTimer_enableInterrupt(cpuTimer);

    //
    // Resets interrupt counters for the three cpuTimers
    //
    if (cpuTimer == CPUTIMER0_BASE)
    {
        cpuTimer0IntCount = 0;
    }
    else if(cpuTimer == CPUTIMER1_BASE)
    {
        cpuTimer1IntCount = 0;
    }
    else if(cpuTimer == CPUTIMER2_BASE)
    {
        cpuTimer2IntCount = 0;
    }
}

//
// cpuTimer0ISR - Counter for CpuTimer0
//
__interrupt void
cpuTimer0ISR(void)
{
    cpuTimer0IntCount++;
    GPIO_togglePin(104);
    //
    // Acknowledge this interrupt to receive more interrupts from group 1
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

//
// cpuTimer1ISR - Counter for CpuTimer1
//
__interrupt void
cpuTimer1ISR(void)
{
    //
    //
    //
    cpuTimer1IntCount++;
    GPIO_togglePin(105);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

//
// cpuTimer2ISR - Counter for CpuTimer2
//
__interrupt void
cpuTimer2ISR(void)
{
    //
    //
    //
    cpuTimer2IntCount++;
    GPIO_togglePin(22);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

//
// End of File
//
