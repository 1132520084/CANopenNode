/*
 * CAN module object for Microchip dsPIC33 or PIC24 microcontroller.
 *
 * @file        CO_driver.h
 * @author      Janez Paternoster
 * @author      P�ter R�zsahegyi
 * @copyright   2004 - 2013 Janez Paternoster
 *
 * This file is part of CANopenNode, an opensource CANopen Stack.
 * Project home page is <http://canopennode.sourceforge.net>.
 * For more information on CANopen see <http://www.can-cia.org/>.
 *
 * CANopenNode is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef CO_DRIVER_H
#define CO_DRIVER_H


/* For documentation see file drvTemplate/CO_driver.h */


#if defined(__dsPIC33F__) || defined(__PIC24H__)
#include <p33fxxxx.h>       /* processor header file */
#elif defined(__dsPIC33E__) || defined(__PIC24E__)
#include <p33exxxx.h>       /* processor header file */
#endif
#include <stddef.h>         /* for 'NULL' */
#include <stdint.h>         /* for 'int8_t' to 'uint64_t' */

#ifdef __HAS_EDS__
    #define EDS_PTR __eds__
#else
    #define EDS_PTR
#endif


/* CAN module base address */
    #define ADDR_CAN1               ((uint16_t)&C1CTRL1)
    #define ADDR_CAN2               ((uint16_t)&C2CTRL1)

    #define ADDR_DMA0               ((uint16_t)&DMA0CON)
    #define ADDR_DMA1               ((uint16_t)&DMA1CON)
    #define ADDR_DMA2               ((uint16_t)&DMA2CON)
    #define ADDR_DMA3               ((uint16_t)&DMA3CON)
    #define ADDR_DMA4               ((uint16_t)&DMA4CON)
    #define ADDR_DMA5               ((uint16_t)&DMA5CON)
    #define ADDR_DMA6               ((uint16_t)&DMA6CON)
    #define ADDR_DMA7               ((uint16_t)&DMA7CON)


/* Critical sections */
    #define CO_LOCK_CAN_SEND()      asm volatile ("disi #0x3FFF")
    #define CO_UNLOCK_CAN_SEND()    asm volatile ("disi #0x0000")

    #define CO_LOCK_EMCY()          asm volatile ("disi #0x3FFF")
    #define CO_UNLOCK_EMCY()        asm volatile ("disi #0x0000")

    #define CO_LOCK_OD()            asm volatile ("disi #0x3FFF")
    #define CO_UNLOCK_OD()          asm volatile ("disi #0x0000")


/* Data types */
    /* int8_t to uint64_t are defined in stdint.h */
    typedef unsigned char           bool_t;
    typedef float                   float32_t;
    typedef long double             float64_t;
    typedef char                    char_t;
    typedef unsigned char           oChar_t;
    typedef unsigned char           domain_t;


/* CAN bit rates
 *
 * CAN bit rates are initializers for array of eight CO_CANbitRateData_t
 * objects.
 *
 * Macros are not used by driver itself, they may be used by application with
 * combination with object CO_CANbitRateData_t.
 * Application must declare following global variable depending on CO_FCY used:
 * const CO_CANbitRateData_t  CO_CANbitRateData[8] = {CO_CANbitRateDataInitializers};
 *
 * There are initializers for eight objects, which corresponds to following
 * CAN bit rates (in kbps): 10, 20, 50, 125, 250, 500, 800, 1000.
 *
 * CO_FCY is internal instruction cycle clock frequency in kHz units. See
 * dsPIC33F documentation for more information on FCY.
 *
 * Possible values for FCY are (in three groups):
 *     - Optimal CAN bit timing on all Baud Rates: 8000, 12000, 16000, 24000.
 *     - Not so optimal CAN bit timing on all Baud Rates: 4000, 32000.
 *     - not all CANopen Baud Rates possible: 2000, 3000, 5000, 6000, 10000,
 *       20000, 40000.
 *
 * IMPORTANT: For FCY<=12000 there is unresolved bug; CANCKS configuration
 *               bit on ECAN does not work, so some baudrates are not possible.
 */
#ifdef CO_FCY
    /* Macros, which divides K into (SJW + PROP + PhSeg1 + PhSeg2) */
    #define TQ_x_4    1, 1, 1, 1
    #define TQ_x_5    1, 1, 2, 1
    #define TQ_x_6    1, 1, 3, 1
    #define TQ_x_8    1, 2, 3, 2
    #define TQ_x_9    1, 2, 4, 2
    #define TQ_x_10   1, 3, 4, 2
    #define TQ_x_12   1, 3, 6, 2
    #define TQ_x_14   1, 4, 7, 2
    #define TQ_x_15   1, 4, 8, 2  /* good timing */
    #define TQ_x_16   1, 5, 8, 2  /* good timing */
    #define TQ_x_17   1, 6, 8, 2  /* good timing */
    #define TQ_x_18   1, 7, 8, 2  /* good timing */
    #define TQ_x_19   1, 8, 8, 2  /* good timing */
    #define TQ_x_20   1, 8, 8, 3  /* good timing */
    #define TQ_x_21   1, 8, 8, 4
    #define TQ_x_25   1, 8, 8, 8

    #if CO_FCY == 2000
        #define CO_CANbitRateDataInitializers  \
        {1, 5,   TQ_x_20},   /*CAN=10kbps*/    \
        {2, 5,   TQ_x_20},   /*CAN=20kbps*/    \
        {1, 1,   TQ_x_20},   /*CAN=50kbps*/    \
        {2, 1,   TQ_x_16},   /*CAN=125kbps*/   \
        {2, 1,   TQ_x_8 },   /*CAN=250kbps*/   \
        {2, 1,   TQ_x_4 },   /*CAN=500kbps*/   \
        {2, 1,   TQ_x_4 },   /*Not possible*/  \
        {2, 1,   TQ_x_4 }    /*Not possible*/
    #elif CO_FCY == 3000
        #define CO_CANbitRateDataInitializers  \
        {2, 15,  TQ_x_20},   /*CAN=10kbps*/    \
        {1, 5,   TQ_x_15},   /*CAN=20kbps*/    \
        {1, 2,   TQ_x_15},   /*CAN=50kbps*/    \
        {1, 1,   TQ_x_12},   /*CAN=125kbps*/   \
        {2, 1,   TQ_x_12},   /*CAN=250kbps*/   \
        {2, 1,   TQ_x_6 },   /*CAN=500kbps*/   \
        {2, 1,   TQ_x_6 },   /*Not possible*/  \
        {2, 1,   TQ_x_6 }    /*Not possible*/
    #elif CO_FCY == 4000
        #define CO_CANbitRateDataInitializers  \
        {2, 25,  TQ_x_16},   /*CAN=10kbps*/    \
        {1, 5,   TQ_x_20},   /*CAN=20kbps*/    \
        {2, 5,   TQ_x_16},   /*CAN=50kbps*/    \
        {1, 1,   TQ_x_16},   /*CAN=125kbps*/   \
        {2, 1,   TQ_x_16},   /*CAN=250kbps*/   \
        {2, 1,   TQ_x_8 },   /*CAN=500kbps*/   \
        {2, 1,   TQ_x_5 },   /*CAN=800kbps*/   \
        {2, 1,   TQ_x_4 }    /*CAN=1000kbps*/
    #elif CO_FCY == 5000
        #define CO_CANbitRateDataInitializers  \
        {2, 25,  TQ_x_20},   /*CAN=10kbps*/    \
        {1, 5,   TQ_x_25},   /*CAN=20kbps*/    \
        {2, 5,   TQ_x_20},   /*CAN=50kbps*/    \
        {1, 1,   TQ_x_20},   /*CAN=125kbps*/   \
        {2, 1,   TQ_x_20},   /*CAN=250kbps*/   \
        {2, 1,   TQ_x_10},   /*CAN=500kbps*/   \
        {2, 1,   TQ_x_10},   /*Not possible*/  \
        {2, 1,   TQ_x_5 }    /*CAN=1000kbps*/
    #elif CO_FCY == 6000
        #define CO_CANbitRateDataInitializers  \
        {1, 20,  TQ_x_15},   /*CAN=10kbps*/    \
        {1, 10,  TQ_x_15},   /*CAN=20kbps*/    \
        {1, 4,   TQ_x_15},   /*CAN=50kbps*/    \
        {2, 3,   TQ_x_16},   /*CAN=125kbps*/   \
        {1, 1,   TQ_x_12},   /*CAN=250kbps*/   \
        {2, 1,   TQ_x_12},   /*CAN=500kbps*/   \
        {2, 1,   TQ_x_12},   /*Not possible*/  \
        {2, 1,   TQ_x_6 }    /*CAN=1000kbps*/
    #elif CO_FCY == 8000
        #define CO_CANbitRateDataInitializers  \
        {1, 25,  TQ_x_16},   /*CAN=10kbps*/    \
        {2, 25,  TQ_x_16},   /*CAN=20kbps*/    \
        {1, 5,   TQ_x_16},   /*CAN=50kbps*/    \
        {1, 2,   TQ_x_16},   /*CAN=125kbps*/   \
        {1, 1,   TQ_x_16},   /*CAN=250kbps*/   \
        {2, 1,   TQ_x_16},   /*CAN=500kbps*/   \
        {2, 1,   TQ_x_10},   /*CAN=800kbps*/   \
        {2, 1,   TQ_x_8 }    /*CAN=1000kbps*/
    #elif CO_FCY == 10000
        #define CO_CANbitRateDataInitializers  \
        {1, 25,  TQ_x_20},   /*CAN=10kbps*/    \
        {2, 25,  TQ_x_20},   /*CAN=20kbps*/    \
        {1, 5,   TQ_x_20},   /*CAN=50kbps*/    \
        {2, 5,   TQ_x_16},   /*CAN=125kbps*/   \
        {1, 1,   TQ_x_20},   /*CAN=250kbps*/   \
        {2, 1,   TQ_x_20},   /*CAN=500kbps*/   \
        {2, 1,   TQ_x_20},   /*Not possible*/  \
        {2, 1,   TQ_x_10}    /*CAN=1000kbps*/
    #elif CO_FCY == 12000
        #define CO_CANbitRateDataInitializers  \
        {2, 63,  TQ_x_19},   /*CAN=10kbps*/    \
        {1, 20,  TQ_x_15},   /*CAN=20kbps*/    \
        {2, 15,  TQ_x_16},   /*CAN=50kbps*/    \
        {1, 3,   TQ_x_16},   /*CAN=125kbps*/   \
        {2, 3,   TQ_x_16},   /*CAN=250kbps*/   \
        {1, 1,   TQ_x_12},   /*CAN=500kbps*/   \
        {2, 1,   TQ_x_15},   /*CAN=800kbps*/   \
        {2, 1,   TQ_x_12}    /*CAN=1000kbps*/
    #elif CO_FCY == 16000
        #define CO_CANbitRateDataInitializers  \
        {1, 50,  TQ_x_16},   /*CAN=10kbps*/    \
        {1, 25,  TQ_x_16},   /*CAN=20kbps*/    \
        {1, 10,  TQ_x_16},   /*CAN=50kbps*/    \
        {1, 4,   TQ_x_16},   /*CAN=125kbps*/   \
        {1, 2,   TQ_x_16},   /*CAN=250kbps*/   \
        {1, 1,   TQ_x_16},   /*CAN=500kbps*/   \
        {1, 1,   TQ_x_10},   /*CAN=800kbps*/   \
        {1, 1,   TQ_x_8 }    /*CAN=1000kbps*/
    #elif CO_FCY == 20000
        #define CO_CANbitRateDataInitializers  \
        {1, 50,  TQ_x_20},   /*CAN=10kbps*/    \
        {1, 25,  TQ_x_20},   /*CAN=20kbps*/    \
        {1, 10,  TQ_x_20},   /*CAN=50kbps*/    \
        {1, 5,   TQ_x_16},   /*CAN=125kbps*/   \
        {1, 2,   TQ_x_20},   /*CAN=250kbps*/   \
        {1, 1,   TQ_x_20},   /*CAN=500kbps*/   \
        {1, 1,   TQ_x_20},   /*Not possible*/  \
        {1, 1,   TQ_x_10}    /*CAN=1000kbps*/
    #elif CO_FCY == 24000
        #define CO_CANbitRateDataInitializers  \
        {1, 63,  TQ_x_19},   /*CAN=10kbps*/    \
        {1, 40,  TQ_x_15},   /*CAN=20kbps*/    \
        {1, 15,  TQ_x_16},   /*CAN=50kbps*/    \
        {1, 6,   TQ_x_16},   /*CAN=125kbps*/   \
        {1, 3,   TQ_x_16},   /*CAN=250kbps*/   \
        {1, 2,   TQ_x_12},   /*CAN=500kbps*/   \
        {1, 1,   TQ_x_15},   /*CAN=800kbps*/   \
        {1, 1,   TQ_x_12}    /*CAN=1000kbps*/
    #elif CO_FCY == 32000
        #define CO_CANbitRateDataInitializers  \
        {1, 64,  TQ_x_25},   /*CAN=10kbps*/    \
        {1, 50,  TQ_x_16},   /*CAN=20kbps*/    \
        {1, 20,  TQ_x_16},   /*CAN=50kbps*/    \
        {1, 8,   TQ_x_16},   /*CAN=125kbps*/   \
        {1, 4,   TQ_x_16},   /*CAN=250kbps*/   \
        {1, 2,   TQ_x_16},   /*CAN=500kbps*/   \
        {1, 2,   TQ_x_10},   /*CAN=800kbps*/   \
        {1, 1,   TQ_x_16}    /*CAN=1000kbps*/
    #elif CO_FCY == 40000
        #define CO_CANbitRateDataInitializers  \
        {1, 50,  TQ_x_20},   /*Not possible*/  \
        {1, 50,  TQ_x_20},   /*CAN=20kbps*/    \
        {1, 25,  TQ_x_16},   /*CAN=50kbps*/    \
        {1, 10,  TQ_x_16},   /*CAN=125kbps*/   \
        {1, 5,   TQ_x_16},   /*CAN=250kbps*/   \
        {1, 2,   TQ_x_20},   /*CAN=500kbps*/   \
        {1, 1,   TQ_x_25},   /*CAN=800kbps*/   \
        {1, 1,   TQ_x_20}    /*CAN=1000kbps*/
    #else
        #error define_CO_FCY CO_FCY not supported
    #endif
#endif


/* Structure contains timing coefficients for CAN module.
 *
 * CAN baud rate is calculated from following equations:
 * FCAN = FCY * Scale                  - Input frequency to CAN module (MAX 40MHz for dsPIC33F and PIC24H)
 * TQ = 2 * BRP / FCAN                 - Time Quanta
 * BaudRate = 1 / (TQ * K)             - Can bus Baud Rate
 * K = SJW + PROP + PhSeg1 + PhSeg2    - Number of Time Quantas
 */
typedef struct{
    uint8_t   scale;    /* (1 or 2) Scales FCY clock - dsPIC33F and PIC24H specific */
    uint8_t   BRP;      /* (1...64) Baud Rate Prescaler */
    uint8_t   SJW;      /* (1...4) SJW time */
    uint8_t   PROP;     /* (1...8) PROP time */
    uint8_t   phSeg1;   /* (1...8) Phase Segment 1 time */
    uint8_t   phSeg2;   /* (1...8) Phase Segment 2 time */
}CO_CANbitRateData_t;


/* Return values */
typedef enum{
    CO_ERROR_NO                 = 0,
    CO_ERROR_ILLEGAL_ARGUMENT   = -1,
    CO_ERROR_OUT_OF_MEMORY      = -2,
    CO_ERROR_TIMEOUT            = -3,
    CO_ERROR_ILLEGAL_BAUDRATE   = -4,
    CO_ERROR_RX_OVERFLOW        = -5,
    CO_ERROR_RX_PDO_OVERFLOW    = -6,
    CO_ERROR_RX_MSG_LENGTH      = -7,
    CO_ERROR_RX_PDO_LENGTH      = -8,
    CO_ERROR_TX_OVERFLOW        = -9,
    CO_ERROR_TX_PDO_WINDOW      = -10,
    CO_ERROR_TX_UNCONFIGURED    = -11,
    CO_ERROR_PARAMETERS         = -12,
    CO_ERROR_DATA_CORRUPT       = -13,
    CO_ERROR_CRC                = -14
}CO_ReturnError_t;


/* CAN receive message structure as aligned in CAN module.
 * In dsPIC33F and PIC24H this structure is used for both: transmitting and
 * receiving to and from CAN module. (Object is ownded by CAN module).
 */
typedef struct{
    uint16_t    ident;          /* Standard Identifier as aligned in CAN module. 16 bits:
                                   'UUUSSSSS SSSSSSRE' (U: unused; S: SID; R=SRR; E=IDE). */
    uint16_t    extIdent;       /* Extended identifier, not used here */
    uint16_t    DLC      :4;    /* Data length code (bits 0...3) */
    uint16_t    DLCrest  :12;   /* Not used here (bits 4..15) */
    uint8_t     data[8];        /* 8 data bytes */
    uint8_t     dummy;          /* Not used */
    uint8_t     FILHIT;         /* Filter hit */
}CO_CANrxMsg_t;


/* Received message object */
typedef struct{
    uint16_t            ident;
    uint16_t            mask;
    void               *object;
    void              (*pFunct)(void *object, const CO_CANrxMsg_t *message);
}CO_CANrx_t;


/* Transmit message object. */
typedef struct{
    uint16_t            ident;      /* Standard Identifier as aligned in CAN module. 16 bits:
                                      'SSSSSUUU SSSSSSRE' (U: unused; S: SID; R=SRR; E=IDE). */
    uint8_t             DLC;
    uint8_t             data[8];
    volatile bool_t     bufferFull;
    volatile bool_t     syncFlag;
}CO_CANtx_t;


/* CAN module object. */
typedef struct{
    uint16_t            CANbaseAddress;
    EDS_PTR CO_CANrxMsg_t *CANmsgBuff;  /* dsPIC33F specific: CAN message buffer for CAN module */
    uint8_t             CANmsgBuffSize; /* dsPIC33F specific: Size of the above buffer */
    CO_CANrx_t         *rxArray;
    uint16_t            rxSize;
    CO_CANtx_t         *txArray;
    uint16_t            txSize;
    volatile bool_t     useCANrxFilters;
    volatile bool_t     bufferInhibitFlag;
    volatile bool_t     firstCANtxMessage;
    volatile uint16_t   CANtxCount;
    uint32_t            errOld;
    void               *em;
}CO_CANmodule_t;


/* Endianes */
#define CO_LITTLE_ENDIAN


/* Request CAN configuration or normal mode */
void CO_CANsetConfigurationMode(uint16_t CANbaseAddress);
void CO_CANsetNormalMode(uint16_t CANbaseAddress);


/* Initialize CAN module object.
 *
 * @param DMArxBaseAddress dsPIC33F specific: Base address for registers for
 * DMA reception. See Peripheral addresses.
 * @param DMAtxBaseAddress dsPIC33F specific: Base address for registers for
 * DMA transmission. See Peripheral addresses.
 * @param CANmsgBuff dsPIC33F specific: Pointer to CAN message buffer defined in DMA RAM.
 * @param CANmsgBuffSize dsPIC33F specific: Size of above buffer.
 * @param CANmsgBuffDMAoffset dsPIC33F specific: DMA offset of the above buffer.
 */
CO_ReturnError_t CO_CANmodule_init(
        CO_CANmodule_t         *CANmodule,
        uint16_t                CANbaseAddress,
        uint16_t                DMArxBaseAddress,
        uint16_t                DMAtxBaseAddress,
        EDS_PTR CO_CANrxMsg_t  *CANmsgBuff,
        uint8_t                 CANmsgBuffSize,
        uint16_t                CANmsgBuffDMAoffset,
#ifdef __HAS_EDS__
        uint16_t                CANmsgBuffDMApage,
#endif
        CO_CANrx_t              rxArray[],
        uint16_t                rxSize,
        CO_CANtx_t              txArray[],
        uint16_t                txSize,
        uint16_t                CANbitRate);


/* Switch off CANmodule. */
void CO_CANmodule_disable(CO_CANmodule_t *CANmodule);


/* Read CAN identifier */
uint16_t CO_CANrxMsg_readIdent(const CO_CANrxMsg_t *rxMsg);


/* Configure CAN message receive buffer. */
CO_ReturnError_t CO_CANrxBufferInit(
        CO_CANmodule_t         *CANmodule,
        uint16_t                index,
        uint16_t                ident,
        uint16_t                mask,
        bool_t                  rtr,
        void                   *object,
        void                  (*pFunct)(void *object, const CO_CANrxMsg_t *message));


/* Configure CAN message transmit buffer. */
CO_CANtx_t *CO_CANtxBufferInit(
        CO_CANmodule_t         *CANmodule,
        uint16_t                index,
        uint16_t                ident,
        bool_t                  rtr,
        uint8_t                 noOfBytes,
        bool_t                  syncFlag);


/* Send CAN message. */
CO_ReturnError_t CO_CANsend(CO_CANmodule_t *CANmodule, CO_CANtx_t *buffer);


/* Clear all synchronous TPDOs from CAN module transmit buffers. */
void CO_CANclearPendingSyncPDOs(CO_CANmodule_t *CANmodule);


/* Verify all errors of CAN module. */
void CO_CANverifyErrors(CO_CANmodule_t *CANmodule);


/* CAN interrupt receives and transmits CAN messages.
 *
 * Function must be called directly from _C1Interrupt or _C2Interrupt with
 * high priority.
 */
void CO_CANinterrupt(CO_CANmodule_t *CANmodule);


#endif
