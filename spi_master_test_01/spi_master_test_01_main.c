
//GPIO59 -> MISO
//GPIO58 -> MOSI
//GPIO60 -> CLK
//GPIO61 -> STE
//sData is 16bits , if need 8bits data , it takes the high 8bits , not the low 8bits.
//this test project sends 8bits-width-data from 0x00 to 0xff looply.
//in this test , the average spi master mosi rate is 6Mb/s (it is almost the max rate that can ensure 1 Byte with one STE(use GPIO61) signal , and the single spi master mosi rate is 8Mb/s.
//if use other GPIO as SPI_STE , the rate should be lower.


//
// Included Files
//
#include "driverlib.h"
#include "device.h"


#define DEVICE_DELAY_NS(x) SysCtl_delay(((((long double)(x)) / (1000000000.0L /  \
                              (long double)DEVICE_SYSCLK_FREQ)) - 9.0L) / 5.0L)


void cfgGPIOs(){
    //
    // GPIO59 is the SPISOMIA.
    //
    GPIO_setMasterCore(59, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_59_SPISOMIA);
    GPIO_setPadConfig(59, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(59, GPIO_QUAL_ASYNC);

    //
    // GPIO58 is the SPISIMOA clock pin.
    //
    GPIO_setMasterCore(58, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_58_SPISIMOA);
    GPIO_setPadConfig(58, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(58, GPIO_QUAL_ASYNC);

    //
    // GPIO60 is the SPICLKA.
    //
    GPIO_setMasterCore(60, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_60_SPICLKA);
    GPIO_setPadConfig(60, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(60, GPIO_QUAL_ASYNC);

    //
    // GPIO61 is the SPISTEA.
    //
    GPIO_setMasterCore(61, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_61_SPISTEA);
    GPIO_setPadConfig(61, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(61, GPIO_QUAL_ASYNC);

}

void initSPIMaster(){
    SPI_disableModule(SPIA_BASE);

    SPI_setConfig(SPIA_BASE,DEVICE_LSPCLK_FREQ,SPI_PROT_POL0PHA0,SPI_MODE_MASTER,8000000,8);
    SPI_disableLoopback(SPIA_BASE);
    SPI_setEmulationMode(SPIA_BASE,SPI_EMULATION_FREE_RUN);

    //SPI_enableFIFO(SPIA_BASE);
    //SPI_clearInterruptStatus(SPIA_BASE,SPI_INT_TXFF);
    //SPI_setFIFOInterruptLevel(SPIA_BASE,SPI_FIFO_TX2,SPI_FIFO_RX2);
    //SPI_enableInterrupt(SPIA_BASE,SPI_INT_TXFF);

    SPI_enableModule(SPIA_BASE);
}

uint16_t rData;
unsigned char sData;

//
// Main
//
void main(void)
{
    uint16_t i;
    Device_init();
    Device_initGPIO();
    Interrupt_initModule();
    Interrupt_initVectorTable();
    //Interrupt_register(INT_SPIA_TX,&tx_func);
    //Interrupt_register(INT_SPIA_RX,&rx_func);
    cfgGPIOs();
    initSPIMaster();
    //initSPIASlave();


    sData=0;
    rData=0;

    //GPIO_setDirectionMode(104,GPIO_DIR_MODE_OUT);
    GPIO_writePin(104,1);

    EINT;
    ERTM;


    sData=1;
    while(1){
        GPIO_writePin(104,0);
        SPI_writeDataNonBlocking(SPIA_BASE, sData<<8);  //send sData(8bits)
        //DEVICE_DELAY_US(1);
        //GPIO_writePin(104,1);
        DEVICE_DELAY_NS(200);   //this delay makes sure that transmit completed.
        GPIO_writePin(104,1);   //if use GPIO as SPI_STE , need delay more.

        rData = SPI_readDataNonBlocking(SPIA_BASE);


        if((rData != 0xff)&&(rData!=0x00)){
            ESTOP0;
        }



        sData+=1;
    }
}

//
// End of File
//
