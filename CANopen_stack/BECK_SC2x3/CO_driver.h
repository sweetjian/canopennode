/*
 * CAN module object for BECK SC243 computer.
 *
 * @file        CO_driver.h
 * @version     SVN: \$Id$
 * @author      Janez Paternoster
 * @copyright   2004 - 2013 Janez Paternoster
 *
 * This file is part of CANopenNode, an opensource CANopen Stack.
 * Project home page is <http://canopennode.sourceforge.net>.
 * For more information on CANopen see <http://www.can-cia.org/>.
 *
 * CANopenNode is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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


#ifndef _CO_DRIVER_H
#define _CO_DRIVER_H


#include <clib.h>       /* processor header file */


/* Peripheral addresses */
#define ADDR_CAN1    CAN_PORT_CAN1
#define ADDR_CAN2    CAN_PORT_CAN2


/* Disabling interrupts */
#define DISABLE_INTERRUPTS()     MaskInterrupts()
#define ENABLE_INTERRUPTS()      EnableInterrupts()


/* Other configuration */
#define CO_LOG_CAN_MESSAGES                 /* Call external function for each received
                                               or transmitted CAN message. */
#define CO_SDO_BUFFER_SIZE           889    /* Override default SDO buffer size. */


/* Data types *
    typedef unsigned char           uint8_t;
    typedef unsigned short int      uint16_t;
    typedef unsigned int            uint32_t;
    typedef unsigned long long int  uint64_t;
    typedef signed char             int8_t;
    typedef signed short int        int16_t;
    typedef signed int              int32_t;
    typedef signed long long int    int64_t;
    typedef float                   float32_t;
    typedef double                  float64_t;
    typedef char                    char_t;
    typedef unsigned char           oChar_t;
    typedef unsigned char           domain_t;
*/
   #define UNSIGNED8       unsigned char
   #define UNSIGNED16      unsigned short int
   #define UNSIGNED32      unsigned int
   #define UNSIGNED64      unsigned long long int
   #define INTEGER8        signed char
   #define INTEGER16       signed short int
   #define INTEGER32       signed int
   #define INTEGER64       signed long long int
   #define REAL32          float
   #define REAL64          double
   #define VISIBLE_STRING  char
   #define OCTET_STRING    unsigned char
   #define DOMAIN          unsigned char
   #define uint8_t       unsigned char
    #define uint16_t      unsigned short int
    #define uint32_t      unsigned int
    #define uint64_t      unsigned long long int
    #define int8_t        signed char
    #define int16_t       signed short int
    #define int32_t       signed int
    #define int64_t       signed long long int
    #define float32_t          float
    #define float64_t          double
    #define char_t  char
    #define oChar_t    unsigned char
    #define domain_t          unsigned char


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


/* CAN receive message structure as aligned in CAN module. In SC2x3 this
 * structure has the same alignment as in the _CanMsg_ structure from canAPI.h */
typedef struct{
    uint32_t        ident;
    uint8_t         DLC;
    uint8_t         data[8];
}CO_CANrxMsg_t;


/* Received message object */
typedef struct{
    uint16_t            ident;
    uint16_t            mask;
    void               *object;
    int16_t           (*pFunct)(void *object, CO_CANrxMsg_t *message);
}CO_CANrx_t;


/* Transmit message object. In SC2x3 this structure has the same alignment
 * of first three members as in the _CanMsg_ structure from canAPI.h */
typedef struct{
    uint32_t            ident;
    uint8_t             DLC;
    uint8_t             data[8];
    volatile uint8_t    bufferFull;
    volatile uint8_t    syncFlag;
}CO_CANtx_t;


/* CAN module object. */
typedef struct{
    uint16_t            CANbaseAddress;
    CO_CANrx_t         *rxArray;
    uint16_t            rxSize;
    CO_CANtx_t         *txArray;
    uint16_t            txSize;
    volatile uint8_t   *curentSyncTimeIsInsideWindow;
    volatile uint8_t    bufferInhibitFlag;
    volatile uint8_t    firstCANtxMessage;
    volatile uint8_t    error;
    volatile uint16_t   CANtxCount;
    uint32_t            errOld;
    void               *EM;
}CO_CANmodule_t;


/* Endianes */
#define BIG_ENDIAN
void memcpySwap2(uint8_t* dest, uint8_t* src);
void memcpySwap4(uint8_t* dest, uint8_t* src);


/* Request CAN configuration or normal mode */
void CO_CANsetConfigurationMode(uint16_t CANbaseAddress);
void CO_CANsetNormalMode(uint16_t CANbaseAddress);


/* Initialize CAN module object. */
int16_t CO_CANmodule_init(
        CO_CANmodule_t        **CANmodule,
        uint16_t                CANbaseAddress,
        uint16_t                rxSize,
        uint16_t                txSize,
        uint16_t                CANbitRate);


/**
 * Delete CANmodule object and free memory.
 *
 * @param ppCANmodule Pointer to pointer to CAN module object <CO_CANmodule_t>.
 * Pointer to CAN module object is set to 0.
 */
void CO_CANmodule_delete(CO_CANmodule_t **ppCANmodule);


/* Read CAN identifier */
uint16_t CO_CANrxMsg_readIdent(CO_CANrxMsg_t *rxMsg);


/* Configure CAN message receive buffer. */
int16_t CO_CANrxBufferInit(
        CO_CANmodule_t         *CANmodule,
        uint16_t                index,
        uint16_t                ident,
        uint16_t                mask,
        uint8_t                 rtr,
        void                   *object,
        int16_t               (*pFunct)(void *object, CO_CANrxMsg_t *message));


/* Configure CAN message transmit buffer. */
CO_CANtx_t *CO_CANtxBufferInit(
        CO_CANmodule_t         *CANmodule,
        uint16_t                index,
        uint16_t                ident,
        uint8_t                 rtr,
        uint8_t                 noOfBytes,
        uint8_t                 syncFlag);


/* Send CAN message. */
int16_t CO_CANsend(CO_CANmodule_t *CANmodule, CO_CANtx_t *buffer);


/* Clear all synchronous TPDOs from CAN module transmit buffers. */
void CO_CANclearPendingSyncPDOs(CO_CANmodule_t *CANmodule);


/* Verify all errors of CAN module. */
void CO_CANverifyErrors(CO_CANmodule_t *CANmodule);


/* CAN interrupt receives and transmits CAN messages.
 *
 * @param event From _CanCallback_ function from _CAN API_ for SC243.
 * @param msg From _CanCallback_ function from _CAN API_ for SC243.
 *
 * @return For _CanCallback_ function from _CAN API_ for SC243.
 */
int CO_CANinterrupt(CO_CANmodule_t *CANmodule, CanEvent event, const CanMsg *msg);


#endif
