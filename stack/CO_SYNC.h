/**
 * CANopen SYNC object protocol.
 *
 * @file        CO_SYNC.h
 * @ingroup     CO_SYNC
 * @author      Janez Paternoster
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


#ifndef CO_SYNC_H
#define CO_SYNC_H


/**
 * @defgroup CO_SYNC SYNC
 * @ingroup CO_CANopen
 * @{
 *
 * CANopen SYNC object protocol.
 *
 * For CAN identifier see #CO_Default_CAN_ID_t
 *
 * SYNC message is used for synchronization of the nodes on network. One node
 * can be SYNC producer, others can be SYNC consumers. Synchronous TPDOs are
 * transmitted after the CANopen SYNC message. Synchronous received PDOs are
 * accepted(copied to OD) immediatelly after the reception of the next SYNC
 * message.
 *
 * ####Contents of SYNC message
 * By default SYNC message has no data. If _Synchronous counter overflow value_
 * from Object dictionary (index 0x1019) is different than 0, SYNC message has
 * one data byte: _counter_ incremented by 1 with every SYNC transmission.
 *
 * ####SYNC in CANopenNode
 * If callback is configured by CO_SYNC_initCallback(), then it is called from
 * CO_SYNC_receive thread, after SYNC message is received. Or, if node is SYNC
 * producer, function is called just before SYNC message is transmitted.
 * Usage of callback is microcontroller specific. It may, for example, temporary
 * disable CAN message reception. Reception is then reenabled after all RPDOs
 * are processed. This is necessary because reception of PDOs must be disabled
 * after sync message, before old RPDOs are processed completelly.
 *
 * Besides callback, information about SYNC message is returned also from
 * CO_SYNC_process() function.
 */


/**
 * SYNC producer and consumer object.
 */
typedef struct{
    CO_EM_t            *em;             /**< From CO_SYNC_init() */
    uint8_t            *operatingState; /**< From CO_SYNC_init() */
    /** True, if device is SYNC producer. Calculated from _COB ID SYNC Message_
    variable from Object dictionary (index 0x1005). */
    bool_t              isProducer;
    /** COB_ID of SYNC message. Calculated from _COB ID SYNC Message_
    variable from Object dictionary (index 0x1005). */
    uint16_t            COB_ID;
    /** Sync period time in [microseconds]. Calculated from _Communication cycle period_
    variable from Object dictionary (index 0x1006). */
    uint32_t            periodTime;
    /** Sync period timeout time in [microseconds].
    (periodTimeoutTime = periodTime * 1,5) */
    uint32_t            periodTimeoutTime;
    /** Value from _Synchronous counter overflow value_ variable from Object
    dictionary (index 0x1019) */
    uint8_t             counterOverflowValue;
    /** True, if current time is inside synchronous window.
    In this case synchronous PDO may be sent. */
    bool_t              curentSyncTimeIsInsideWindow;
    /** Variable indicates, if new SYNC message received from CAN bus */
    bool_t              CANrxNew;
    /** Counter of the SYNC message if counterOverflowValue is different than zero */
    uint8_t             counter;
    /** Timer for the SYNC message in [microseconds].
    Set to zero after received or transmitted SYNC message */
    uint32_t            timer;
    /** Set to nonzero value, if SYNC with wrong data length is received from CAN */
    uint16_t            receiveError;
    void              (*cbSync)(bool_t syncReceived);/**< From CO_SYNC_initCallback() or NULL */
    CO_CANmodule_t     *CANdevRx;       /**< From CO_SYNC_init() */
    uint16_t            CANdevRxIdx;    /**< From CO_SYNC_init() */
    CO_CANmodule_t     *CANdevTx;       /**< From CO_SYNC_init() */
    CO_CANtx_t         *CANtxBuff;      /**< CAN transmit buffer inside CANdevTx */
    uint16_t            CANdevTxIdx;    /**< From CO_SYNC_init() */
}CO_SYNC_t;


/**
 * Initialize SYNC object.
 *
 * Function must be called in the communication reset section.
 *
 * @param SYNC This object will be initialized.
 * @param em Emergency object.
 * @param SDO SDO server object.
 * @param operatingState Pointer to variable indicating CANopen device NMT internal state.
 * @param COB_ID_SYNCMessage From Object dictionary (index 0x1005).
 * @param communicationCyclePeriod From Object dictionary (index 0x1006).
 * @param synchronousCounterOverflowValue From Object dictionary (index 0x1019).
 * @param CANdevRx CAN device for SYNC reception.
 * @param CANdevRxIdx Index of receive buffer in the above CAN device.
 * @param CANdevTx CAN device for SYNC transmission.
 * @param CANdevTxIdx Index of transmit buffer in the above CAN device.
 *
 * @return #CO_ReturnError_t: CO_ERROR_NO or CO_ERROR_ILLEGAL_ARGUMENT.
 */
CO_ReturnError_t CO_SYNC_init(
        CO_SYNC_t              *SYNC,
        CO_EM_t                *em,
        CO_SDO_t               *SDO,
        uint8_t                *operatingState,
        uint32_t                COB_ID_SYNCMessage,
        uint32_t                communicationCyclePeriod,
        uint8_t                 synchronousCounterOverflowValue,
        CO_CANmodule_t         *CANdevRx,
        uint16_t                CANdevRxIdx,
        CO_CANmodule_t         *CANdevTx,
        uint16_t                CANdevTxIdx);


/**
 * Initialize SYNC callback function.
 *
 * Function initializes optional callback function, which executes after the SYNC.
 *
 * @param SYNC This object.
 * @param cbSync Callback function, which will be called just after the
 * presence of CANopen SYNC message on the bus. If SYNC was received,
 * syncReceived will be true, if SYNC will be transmitted, syncReceived
 * will be false.
 */
void CO_SYNC_initCallback(
        CO_SYNC_t              *SYNC,
        void                  (*cbSync)(bool_t syncReceived));


/**
 * Process SYNC communication.
 *
 * Function must be called cyclically.
 *
 * @param SYNC This object.
 * @param timeDifference_us Time difference from previous function call in [microseconds].
 * @param ObjDict_synchronousWindowLength _Synchronous window length_ variable from
 * Object dictionary (index 0x1007).
 *
 * @return 0: No special meaning.
 * @return 1: New SYNC message recently received or was just transmitted.
 * @return 2: SYNC time was just passed out of window.
 */
uint8_t CO_SYNC_process(
        CO_SYNC_t              *SYNC,
        uint32_t                timeDifference_us,
        uint32_t                ObjDict_synchronousWindowLength);


/** @} */
#endif
