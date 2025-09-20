#include "F28x_Project.h"

typedef unsigned char Uint8;

Uint16 minADC = 0;
Uint16 maxADC = 4095;
Uint16 midADC = 2048;
bool isCalibrated = false;

Uint16 calibrateMin = 0;
Uint16 calibrateMax = 0;
Uint16 calibrateMid = 0;

Uint16 currentADC = 0;
Uint16 mappedADC = 0;

void ConfigureADC(void);
void ConfigureSOC(void);
interrupt void adca1_isr(void);

#define RESULTS_BUFFER_SIZE 20
Uint16 AdcaResults[RESULTS_BUFFER_SIZE];
Uint16 resultsIndex = 0;

void main(void)
{
    InitSysCtrl();
    ConfigureADC();
    ConfigureSOC();

    DINT;
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();
    EALLOW;
    PieVectTable.ADCA1_INT = &adca1_isr;
    EDIS;
    IER |= M_INT1;
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;
    EINT;
    ERTM;

    while (1)
    {
        if (calibrateMin == 1 && minADC == 0)  // Set calibrateMin to 1 in Expressions to record currentADC as minimum
        {
            minADC = currentADC;
            mappedADC = 0;
        }
        if (calibrateMax == 1 && maxADC == 4095)  // Set calibrateMax to 1 in Expressions to record currentADC as maximum
        {
            maxADC = currentADC;
            mappedADC = 4095;
        }
        if (calibrateMid == 1 && midADC == 2048)  // Set calibrateMid to 1 in Expressions to record currentADC as midpoint
        {
            midADC = currentADC;
            mappedADC = 2048;
        }

        // ��T�Ӯե��ܼƳ���1�ɡA�]�m�ե�����
        if (calibrateMin == 1 && calibrateMax == 1 && calibrateMid == 1 && !isCalibrated)
        {
            isCalibrated = true;
        }
    }
}

void ConfigureADC(void)
{
    EALLOW;
    AdcaRegs.ADCCTL2.bit.PRESCALE = 6;
    AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    DELAY_US(1000);
    EDIS;
}

void ConfigureSOC(void)
{
    EALLOW;
    EPwm1Regs.ETSEL.bit.SOCAEN = 1;
    EPwm1Regs.ETSEL.bit.SOCASEL = 2;
    EPwm1Regs.ETPS.bit.SOCAPRD = 1;
    EPwm1Regs.TBPRD = 5000;
    EPwm1Regs.TBCTL.bit.CTRMODE = 0;
    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 0;
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = 99;
    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 5;
    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0;
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
    EDIS;
}

interrupt void adca1_isr(void)
{
    Uint16 i;
    AdcaResults[resultsIndex++] = AdcaResultRegs.ADCRESULT0;

    if (RESULTS_BUFFER_SIZE <= resultsIndex)
    {
        Uint32 sum = 0;
        for (i = 0; i < RESULTS_BUFFER_SIZE; i++)
        {
            sum += AdcaResults[i];
        }
        currentADC = sum / RESULTS_BUFFER_SIZE;

        // Apply calibration if complete
        if (isCalibrated)
        {
            if (currentADC <= minADC)
            {
                mappedADC = 0;
            }
            else if (currentADC >= maxADC)
            {
                mappedADC = 4095;
            }
            else if (currentADC == midADC)
            {
                mappedADC = 2048;
            }
            else if (currentADC > minADC && currentADC < midADC)
            {
                // �p�G currentADC �b minADC �M midADC �����A�u�ʴ��Ȩ� mappedADC �� 0 �� 2048 �϶�
                mappedADC = (Uint32)(currentADC - minADC) * 2048 / (midADC - minADC);
            }
            else if (currentADC > midADC && currentADC < maxADC)
            {
                // �p�G currentADC �b midADC �M maxADC �����A�u�ʴ��Ȩ� mappedADC �� 2048 �� 4095 �϶�
                mappedADC = 2048 + (Uint32)(currentADC - midADC) * (4095 - 2048) / (maxADC - midADC);
            }
        }
        else
        {
            mappedADC = currentADC;  // �b�ե��e�AmappedADC �� currentADC �O���@�P
        }


        resultsIndex = 0;
    }

    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

