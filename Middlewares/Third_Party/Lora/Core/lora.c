/**
  ******************************************************************************
  * @file    lora.c
  * @author  MCD Application Team
  * @brief   lora API to drive the lora state Machine
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"
#include "timeServer.h"
#include "LoRaMac.h"
#include "lora.h"
#include "lora-test.h"

#include "ketCube_cfg.h"
#include "ketCube_modules.h"
#include "ketCube_terminal.h"
#include "ketCube_lora.h"

 /**
   * Lora Configuration
   */
typedef struct
{
   uint8_t static_dev_eui;
   uint8_t static_dev_addr;
   uint32_t NwkID;
   LoraState_t otaa;              /*< ENABLE if over the air activation, DISABLE otherwise */
   LoraState_t duty_cycle;        /*< ENABLE if dutycyle is on, DISABLE otherwise */
   uint8_t DevEui[8];           /*< Device EUI */
   uint8_t JoinEui[8];          /*< Join Eui */
   uint8_t AppKey[16];          /*< Application Key */
   uint8_t NwkKey[16];          /*< Application Key */
   uint8_t NwkSEncKey[16];      /*< Network Session Key */
   uint8_t AppSKey[16];         /*< Application Session Key */
   uint8_t FNwkSIntKey[16];     /*< Application Session Key */
   uint8_t SNwkSIntKey[16];     /*< Application Session Key */   
   int16_t Rssi;                /*< Rssi of the received packet */
   uint8_t Snr;                 /*< Snr of the received packet */
   uint8_t application_port;    /*< Application port we will receive to */
   LoraConfirm_t ReqAck;         /*< ENABLE if acknowledge is requested */
   McpsConfirm_t *McpsConfirm;  /*< pointer to the confirm structure */
} lora_configuration_t;
 
 
static lora_configuration_t lora_config = 
{
  .static_dev_eui = STATIC_DEVICE_EUI,
  .static_dev_addr = STATIC_DEVICE_ADDRESS,
  .NwkID = LORAWAN_NETWORK_ID,
  .otaa = OVER_THE_AIR_ACTIVATION,
#if defined( REGION_EU868 )
  .duty_cycle = LORA_ENABLE,
#else
  .duty_cycle = LORA_DISABLE,
#endif
  .DevEui = LORAWAN_DEVICE_EUI,
  .JoinEui = LORAWAN_JOIN_EUI,
  .AppKey = LORAWAN_APP_KEY,
  .NwkKey = LORAWAN_NWK_KEY,
  .NwkSEncKey = LORAWAN_NWK_S_ENC_KEY,
  .AppSKey = LORAWAN_APP_S_KEY,
  .FNwkSIntKey=LORAWAN_F_NWK_S_INT_KEY,
  .SNwkSIntKey=LORAWAN_S_NWK_S_INT_KEY,
  .Rssi = 0,
  .Snr = 0,
  .ReqAck = LORAWAN_UNCONFIRMED_MSG,
  .McpsConfirm = NULL
};

static uint16_t reJoinIterrationCounter = 0;

/*!
 *  Select either Device_Time_req or Beacon_Time_Req following LoRaWAN version 
 *  - Device_Time_req   Available for V1.0.3 or later                          
 *  - Beacon_time_Req   Available for V1.0.2 and before                        
 */
#define USE_DEVICE_TIMING

/*!
 * Join requests trials duty cycle.
 */
#define OVER_THE_AIR_ACTIVATION_DUTYCYCLE           10000  // 10 [s] value in ms

#include "LoRaMacTest.h"

#if defined( REGION_EU868 )
/*!
 * LoRaWAN ETSI duty cycle control enable/disable
 *
 * \remark Please note that ETSI mandates duty cycled transmissions. Use only for test purposes
 */
#define LORAWAN_DUTYCYCLE_ON                        true
   
#ifdef LORAMAC_CLASSB_ENABLED    
/*!
 * Default ping slots periodicity
 *
 * \remark periodicity is equal to 2^LORAWAN_DEFAULT_PING_SLOT_PERIODICITY seconds
 *         example: 2^3 = 8 seconds. The end-device will open an Rx slot every 8 seconds.
 */
#define LORAWAN_DEFAULT_PING_SLOT_PERIODICITY       0   
uint8_t  DefaultPingSlotPeriodicity;  
#endif
#endif

static MlmeReqJoin_t JoinParameters;

static uint32_t DevAddr = LORAWAN_DEVICE_ADDRESS;

#ifdef LORAMAC_CLASSB_ENABLED
static LoraErrorStatus LORA_BeaconReq( void);
static LoraErrorStatus LORA_PingSlotReq( void);

#if defined( USE_DEVICE_TIMING )
static LoraErrorStatus LORA_DeviceTimeReq(void);
#else
static LoraErrorStatus LORA_BeaconTimeReq(void);
#endif /* USE_DEVICE_TIMING */
#endif /* LORAMAC_CLASSB_ENABLED */

/*!
 * Defines the LoRa parameters at Init
 */
static LoRaParam_t* LoRaParamInit;
static LoRaMacPrimitives_t LoRaMacPrimitives;
static LoRaMacCallback_t LoRaMacCallbacks;
static MibRequestConfirm_t mibReq;

static LoRaMainCallback_t *LoRaMainCallbacks;

/*!
 * MAC event info status strings.
 */
const char* EventInfoStatusStrings[] =
{ 
    "OK", "Error", "Tx timeout", "Rx 1 timeout",
    "Rx 2 timeout", "Rx1 error", "Rx2 error",
    "Join failed", "Downlink repeated", "Tx DR payload size error",
    "Downlink too many frames loss", "Address fail", "MIC fail",
    "Multicast faile", "Beacon locked", "Beacon lost", "Beacon not found"
};


/*!
 * \brief   MCPS-Confirm event function
 *
 * \param   [IN] McpsConfirm - Pointer to the confirm structure,
 *               containing confirm attributes.
 */
static void McpsConfirm(McpsConfirm_t *mcpsConfirm)
{
   ketCube_terminal_NewDebugPrintln(
      KETCUBE_LISTS_MODULEID_LORA, 
      "APP> McpsConfirm STATUS: %s", EventInfoStatusStrings[mcpsConfirm->Status] 
   );

   lora_config.McpsConfirm = mcpsConfirm;
   if(mcpsConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK)
   {
      switch( mcpsConfirm->McpsRequest )
      {
            case MCPS_UNCONFIRMED:
            {
               // Check Datarate
               // Check TxPower
               break;
            }
            case MCPS_CONFIRMED:
            {
                  // Check Datarate
                  // Check TxPower
                  // Check AckReceived
                  if(mcpsConfirm->AckReceived)
                  {
                     LoRaMainCallbacks->LORA_McpsDataConfirm();
                  }  
                  // Check NbTrials
                  break;
            }
            case MCPS_PROPRIETARY:
            {
                  break;
            }
            default:
                  break;
      }
   }
}

/*!
 * \brief   MCPS-Indication event function
 *
 * \param   [IN] mcpsIndication - Pointer to the indication structure,
 *               containing indication attributes.
 */
static void McpsIndication(McpsIndication_t *mcpsIndication)
{
   lora_AppData_t AppData;

   ketCube_terminal_NewDebugPrintln(
      KETCUBE_LISTS_MODULEID_LORA, 
      "APP> McpsInd STATUS: %s", EventInfoStatusStrings[mcpsIndication->Status] 
   );

   if(mcpsIndication->Status != LORAMAC_EVENT_INFO_STATUS_OK)
   {
      return;
   }

   switch(mcpsIndication->McpsIndication)
   {
      case MCPS_UNCONFIRMED:
      {
         break;
      }
      case MCPS_CONFIRMED:
      {
         break;
      }
      case MCPS_PROPRIETARY:
      {
         break;
      }
      case MCPS_MULTICAST:
      {
         break;
      }
      default:
         break;
   }

   // Check Multicast
   // Check Port
   // Check Datarate
   // Check FramePending
   if(mcpsIndication->FramePending == true)
   {
      // The server signals that it has pending data to be sent.
      // We schedule an uplink as soon as possible to flush the server.
      LoRaMainCallbacks->LORA_TxNeeded( );
   }
   // Check Buffer
   // Check BufferSize
   // Check Rssi
   // Check Snr
   // Check RxSlot
   if (certif_running() == true)
   {
      certif_DownLinkIncrement();
   }

   if( mcpsIndication->RxData == true )
   {
      switch( mcpsIndication->Port )
      {
         case CERTIF_PORT:
            certif_rx( mcpsIndication, &JoinParameters );
            break;
         default:
            AppData.Port = mcpsIndication->Port;
            AppData.BuffSize = mcpsIndication->BufferSize;
            AppData.Buff = mcpsIndication->Buffer;
            lora_config.Rssi = mcpsIndication->Rssi;
            lora_config.Snr  = mcpsIndication->Snr;
            LoRaMainCallbacks->LORA_RxData( &AppData );
            break;
      }
   }
}

/*!
 * \brief   MLME-Confirm event function
 *
 * \param   [IN] MlmeConfirm - Pointer to the confirm structure,
 *               containing confirm attributes.
 */
static void MlmeConfirm( MlmeConfirm_t *mlmeConfirm )
{
#ifdef LORAMAC_CLASSB_ENABLED
   MibRequestConfirm_t mibReq;
#endif /* LORAMAC_CLASSB_ENABLED */

   ketCube_terminal_NewDebugPrintln(
      KETCUBE_LISTS_MODULEID_LORA, 
      "APP> MlmeConfirm STATUS: %s", EventInfoStatusStrings[mlmeConfirm->Status] 
   );
   
   switch( mlmeConfirm->MlmeRequest )
   {
         case MLME_JOIN:
         {
               if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
               {
                  // Status is OK, node has joined the network
                  LoRaMainCallbacks->LORA_HasJoined();
   #ifdef LORAMAC_CLASSB_ENABLED
   #if defined( USE_DEVICE_TIMING )              
                  LORA_DeviceTimeReq();
   #else              
                  LORA_BeaconTimeReq();
   #endif /* USE_DEVICE_TIMING */
   #endif /* LORAMAC_CLASSB_ENABLED */
                  /* reset re-join counter */
                  reJoinIterrationCounter = 0;
               }
               else
               {
                     // Join was not successful. Try to join again
                     LORA_Join();
               }
               break;
         }
         case MLME_LINK_CHECK:
         {
               if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
               {
                     // Check DemodMargin
                     // Check NbGateways
                     if (certif_running() == true )
                     {
                            certif_linkCheck( mlmeConfirm);
                     }
               }
               break;
         }
   #ifdef LORAMAC_CLASSB_ENABLED
         case MLME_BEACON_ACQUISITION:
         {
               if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
               {
                     /* Beacon has been acquired */
                     /* REquest Server for Ping Slot */
                     LORA_PingSlotReq( );
               }
               else
               {
                     /* Beacon not acquired */
                     /* Search again */
                     /* we can check if the MAC has received a time reference for the beacon*/
                     /* in this case do either a Device_Time_Req  or a Beacon_Timing_req*/
                     LORA_BeaconReq( );
               }
               break;
         }
         case MLME_PING_SLOT_INFO:
         {
               if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
               {
                   /* class B is now ativated*/
                     mibReq.Type = MIB_DEVICE_CLASS;
                     mibReq.Param.Class = CLASS_B;
                     LoRaMacMibSetRequestConfirm( &mibReq );
                     
   #if defined( REGION_AU915 ) || defined( REGION_US915 )
                     mibReq.Type = MIB_PING_SLOT_DATARATE;
                     mibReq.Param.PingSlotDatarate = DR_8;
                     LoRaMacMibSetRequestConfirm( &mibReq );
   #endif
                     
                     /*notify upper layer*/
                     LoRaMainCallbacks->LORA_ConfirmClass(CLASS_B);
               }
               else
               {
                     LORA_PingSlotReq( );
               }
               break;
         }
   #if defined( USE_DEVICE_TIMING )        
         case MLME_DEVICE_TIME:
         {        
               if( mlmeConfirm->Status != LORAMAC_EVENT_INFO_STATUS_OK )
               {
                  LORA_DeviceTimeReq();
               }  
         }              
   #endif /* USE_DEVICE_TIMING */
   #endif /* LORAMAC_CLASSB_ENABLED */
         default:
               break;
   }
}

/*!
 * \brief   MLME-Indication event function
 *
 * \param   [IN] MlmeIndication - Pointer to the indication structure.
 */
static void MlmeIndication( MlmeIndication_t *MlmeIndication )
{
#ifdef LORAMAC_CLASSB_ENABLED
   MibRequestConfirm_t mibReq;
#endif /* LORAMAC_CLASSB_ENABLED */


   ketCube_terminal_NewDebugPrintln(
      KETCUBE_LISTS_MODULEID_LORA, 
      "APP> MLMEInd STATUS: %s", EventInfoStatusStrings[MlmeIndication->Status] 
   );

   switch( MlmeIndication->MlmeIndication )
   {
         case MLME_SCHEDULE_UPLINK:
         {
               // The MAC signals that we shall provide an uplink as soon as possible
               LoRaMainCallbacks->LORA_TxNeeded( );         
               break;
         }
   #ifdef LORAMAC_CLASSB_ENABLED
         case MLME_BEACON_LOST:
         {
               // Switch to class A again
               mibReq.Type = MIB_DEVICE_CLASS;
               mibReq.Param.Class = CLASS_A;
               LoRaMacMibSetRequestConfirm( &mibReq );

               LORA_BeaconReq();
               break;
         }
         case MLME_BEACON:
         {
               if( MlmeIndication->Status == LORAMAC_EVENT_INFO_STATUS_BEACON_LOCKED )
               {
               //  TVL2( PRINTF( "BEACON RECEIVED\n\r");)
               }
               else
               {
               //  TVL2( PRINTF( "BEACON NOT RECEIVED\n\r");)
               }
               break;

         }
   #endif /* LORAMAC_CLASSB_ENABLED */
         default:
               break;
   }
}
/**
 *  lora Init
 */
void LORA_Init (LoRaMainCallback_t *callbacks, LoRaParam_t* LoRaParam )
{
  /* init the Tx Duty Cycle*/
  LoRaParamInit = LoRaParam;
  
  /* init the main call backs*/
  LoRaMainCallbacks = callbacks;
  
   if (lora_config.static_dev_eui != 1) {
      LoRaMainCallbacks->BoardGetUniqueId( lora_config.DevEui );  
   }
   
   if (lora_config.static_dev_addr != 1) {
      // Random seed initialization
      srand1( LoRaMainCallbacks->BoardGetRandomSeed( ) );
      // Choose a random device address
      DevAddr = randr( 0, 0x01FFFFFF );
   }
   
  lora_config_print();
   
  LoRaMacPrimitives.MacMcpsConfirm = McpsConfirm;
  LoRaMacPrimitives.MacMcpsIndication = McpsIndication;
  LoRaMacPrimitives.MacMlmeConfirm = MlmeConfirm;
  LoRaMacPrimitives.MacMlmeIndication = MlmeIndication;
  LoRaMacCallbacks.GetBatteryLevel = LoRaMainCallbacks->BoardGetBatteryLevel;
  LoRaMacCallbacks.GetTemperatureLevel = LoRaMainCallbacks->BoardGetTemperatureLevel;
  LoRaMacCallbacks.MacProcessNotify = LoRaMainCallbacks->MacProcessNotify;
#if defined( REGION_AS923 )
  LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_AS923 );
#elif defined( REGION_AU915 )
  LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_AU915 );
#elif defined( REGION_CN470 )
  LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_CN470 );
#elif defined( REGION_CN779 )
  LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_CN779 );
#elif defined( REGION_EU433 )
  LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_EU433 );
 #elif defined( REGION_IN865 )
  LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_IN865 );
#elif defined( REGION_EU868 )
  LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_EU868 );
#elif defined( REGION_KR920 )
  LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_KR920 );
#elif defined( REGION_US915 )
  LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_US915 );

#if defined( HYBRID )
                uint16_t channelMask[] = { 0x00FF, 0x0000, 0x0000, 0x0000, 0x0001, 0x0000};
                mibReq.Type = MIB_CHANNELS_MASK;
                mibReq.Param.ChannelsMask = channelMask;
                LoRaMacMibSetRequestConfirm( &mibReq );
                mibReq.Type = MIB_CHANNELS_DEFAULT_MASK;
                mibReq.Param.ChannelsDefaultMask = channelMask;
                LoRaMacMibSetRequestConfirm( &mibReq );
#endif
#elif defined( REGION_RU864 )
  LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_RU864 );
#else
    #error "Please define a region in the compiler options."
#endif
  
  mibReq.Type = MIB_ADR;
  mibReq.Param.AdrEnable = LoRaParamInit->AdrEnable;
  LoRaMacMibSetRequestConfirm( &mibReq );

  mibReq.Type = MIB_PUBLIC_NETWORK;
  mibReq.Param.EnablePublicNetwork = LoRaParamInit->EnablePublicNetwork;
  LoRaMacMibSetRequestConfirm( &mibReq );
          
  mibReq.Type = MIB_APP_KEY;
  mibReq.Param.AppKey = lora_config.AppKey;
  LoRaMacMibSetRequestConfirm( &mibReq );

  mibReq.Type = MIB_NWK_KEY;
  mibReq.Param.NwkKey = lora_config.NwkKey;
  LoRaMacMibSetRequestConfirm( &mibReq );
                      
  mibReq.Type = MIB_DEVICE_CLASS;
  mibReq.Param.Class= CLASS_A;
  LoRaMacMibSetRequestConfirm( &mibReq );

#if defined( REGION_EU868 )
  LoRaMacTestSetDutyCycleOn( LORAWAN_DUTYCYCLE_ON );
  lora_config.duty_cycle = LORA_ENABLE;
#else
  lora_config.duty_cycle = LORA_DISABLE;
#endif
      
  mibReq.Type = MIB_SYSTEM_MAX_RX_ERROR;
  mibReq.Param.SystemMaxRxError = 20;
  LoRaMacMibSetRequestConfirm( &mibReq );
  
#ifdef LORAMAC_CLASSB_ENABLED
  DefaultPingSlotPeriodicity =  LORAWAN_DEFAULT_PING_SLOT_PERIODICITY;
#endif /* LORAMAC_CLASSB_ENABLED */

  /*set Mac statein Idle*/
  LoRaMacStart( );
}

/**
 *
 * ReJoin after certain time to reset couners
 * 
 * @todo to be validated
 *
 */
LoraErrorStatus LORA_ReJoin(void)
{
   MlmeReq_t mlmeReq;

   // mlmeReq.Type = MLME_REJOIN_0; // Rejoin is not supported
   mlmeReq.Type = MLME_JOIN;
   mlmeReq.Req.Join.DevEui = lora_config.DevEui;
   mlmeReq.Req.Join.JoinEui = lora_config.JoinEui;
   mlmeReq.Req.Join.Datarate = LoRaParamInit->TxDatarate;

   /* only for OTAA */
   if (lora_config.otaa != LORA_ENABLE) {
      return LORA_ERROR;
   }
   
   reJoinIterrationCounter++;
   /* set bound staticaly here to 2^16/2*/
   if (reJoinIterrationCounter < 0x8FFF) {
       return LORA_ERROR;
   }
   
   /* take some minimal rest between re-join requests */
   if ((reJoinIterrationCounter % 8) != 0) {
       return LORA_ERROR;
   }
   
   // Do it
   ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "Re-Join in progress");
   LoRaMacMlmeRequest( &mlmeReq );
   
   return LORA_SUCCESS;
}

LoraErrorStatus LORA_Join( void)
{
   MlmeReq_t mlmeReq;

   mlmeReq.Type = MLME_JOIN;
   mlmeReq.Req.Join.DevEui = lora_config.DevEui;
   mlmeReq.Req.Join.JoinEui = lora_config.JoinEui;
   mlmeReq.Req.Join.Datarate = LoRaParamInit->TxDatarate;

   JoinParameters = mlmeReq.Req.Join;

   // OTAA
   if (lora_config.otaa == LORA_ENABLE)
   {
      LoRaMacMlmeRequest( &mlmeReq );    
      return LORA_SUCCESS;
   }

   // ABP
   mibReq.Type = MIB_NET_ID;
   mibReq.Param.NetID = LORAWAN_NETWORK_ID;
   LoRaMacMibSetRequestConfirm( &mibReq );

   mibReq.Type = MIB_DEV_ADDR;
   mibReq.Param.DevAddr = DevAddr;
   LoRaMacMibSetRequestConfirm( &mibReq );

   mibReq.Type = MIB_F_NWK_S_INT_KEY;
   mibReq.Param.FNwkSIntKey = lora_config.FNwkSIntKey;
   LoRaMacMibSetRequestConfirm( &mibReq );

   mibReq.Type = MIB_S_NWK_S_INT_KEY;
   mibReq.Param.SNwkSIntKey = lora_config.SNwkSIntKey;
   LoRaMacMibSetRequestConfirm( &mibReq );

   mibReq.Type = MIB_NWK_S_ENC_KEY;
   mibReq.Param.NwkSEncKey = lora_config.NwkSEncKey;
   LoRaMacMibSetRequestConfirm( &mibReq );

   mibReq.Type = MIB_APP_S_KEY;
   mibReq.Param.AppSKey = lora_config.AppSKey;
   LoRaMacMibSetRequestConfirm( &mibReq );

   mibReq.Type = MIB_NETWORK_ACTIVATION;
   mibReq.Param.NetworkActivation = ACTIVATION_TYPE_ABP;
   LoRaMacMibSetRequestConfirm( &mibReq );

   // Enable legacy mode to operate according to LoRaWAN Spec. 1.0.3
   Version_t abpLrWanVersion;

   #if (KETCUBE_LORA_LRWAN_VERSION == KETCUBE_LORA_LRWAN_VERSION_V11x)
   abpLrWanVersion.Fields.Major    = 1;
   abpLrWanVersion.Fields.Minor    = 1;
   abpLrWanVersion.Fields.Revision = 0;
   abpLrWanVersion.Fields.Rfu      = 0;      
   #else
   abpLrWanVersion.Fields.Major    = 1;
   abpLrWanVersion.Fields.Minor    = 0;
   abpLrWanVersion.Fields.Revision = 3;
   abpLrWanVersion.Fields.Rfu      = 0;
   #endif

   mibReq.Type = MIB_ABP_LORAWAN_VERSION;
   mibReq.Param.AbpLrWanVersion = abpLrWanVersion;
   LoRaMacMibSetRequestConfirm( &mibReq );

   LoRaMainCallbacks->LORA_HasJoined();

   return LORA_SUCCESS;
}


LoraFlagStatus LORA_JoinStatus( void)
{
   MibRequestConfirm_t mibReq;

   mibReq.Type = MIB_NETWORK_ACTIVATION;

   LoRaMacMibGetRequestConfirm( &mibReq );

   if( mibReq.Param.NetworkActivation == ACTIVATION_TYPE_NONE )
   {
      return LORA_RESET;
   }
   else
   {
      return LORA_SET;
   }
}

LoraErrorStatus LORA_send(lora_AppData_t* AppData, LoraConfirm_t IsTxConfirmed)
{
   McpsReq_t mcpsReq;
   LoRaMacTxInfo_t txInfo;

   /*if certification test are on going, application data is not sent*/
   if (certif_running() == true)
   {
      return LORA_ERROR;
   }

   if( LoRaMacQueryTxPossible( AppData->BuffSize, &txInfo ) != LORAMAC_STATUS_OK )
   {
      // Send empty frame in order to flush MAC commands
      mcpsReq.Type = MCPS_UNCONFIRMED;
      mcpsReq.Req.Unconfirmed.fBuffer = NULL;
      mcpsReq.Req.Unconfirmed.fBufferSize = 0;
      mcpsReq.Req.Unconfirmed.Datarate = LoRaParamInit->TxDatarate;
   }
   else
   {
      if( IsTxConfirmed == LORAWAN_UNCONFIRMED_MSG )
      {
         mcpsReq.Type = MCPS_UNCONFIRMED;
         mcpsReq.Req.Unconfirmed.fPort = AppData->Port;
         mcpsReq.Req.Unconfirmed.fBufferSize = AppData->BuffSize;
         mcpsReq.Req.Unconfirmed.fBuffer = AppData->Buff;
         mcpsReq.Req.Unconfirmed.Datarate = LoRaParamInit->TxDatarate;
      }
      else
      {
         mcpsReq.Type = MCPS_CONFIRMED;
         mcpsReq.Req.Confirmed.fPort = AppData->Port;
         mcpsReq.Req.Confirmed.fBufferSize = AppData->BuffSize;
         mcpsReq.Req.Confirmed.fBuffer = AppData->Buff;
         mcpsReq.Req.Confirmed.NbTrials = 8;
         mcpsReq.Req.Confirmed.Datarate = LoRaParamInit->TxDatarate;
      }
   }
   
   /* get status */
   switch (LoRaMacMcpsRequest( &mcpsReq ) ) {
       case LORAMAC_STATUS_OK:
           return LORA_SUCCESS;
       case LORAMAC_STATUS_BUSY:
           ketCube_terminal_NewDebugPrintln(KETCUBE_LISTS_MODULEID_LORA, "McpReq: busy");
           break;
       case LORAMAC_STATUS_SERVICE_UNKNOWN:
           ketCube_terminal_NewDebugPrintln(KETCUBE_LISTS_MODULEID_LORA, "McpReq: unknown");
           break;
       case LORAMAC_STATUS_PARAMETER_INVALID:
           ketCube_terminal_NewDebugPrintln(KETCUBE_LISTS_MODULEID_LORA, "McpReq: invalid");
           break;
       case LORAMAC_STATUS_NO_NETWORK_JOINED:
           ketCube_terminal_NewDebugPrintln(KETCUBE_LISTS_MODULEID_LORA, "McpReq: notJoined");
           break;
       case LORAMAC_STATUS_LENGTH_ERROR:
           ketCube_terminal_NewDebugPrintln(KETCUBE_LISTS_MODULEID_LORA, "McpReq: lenErr");
           break;
       case LORAMAC_STATUS_MAC_COMMAD_ERROR:
           ketCube_terminal_NewDebugPrintln(KETCUBE_LISTS_MODULEID_LORA, "McpReq: MACcmdErr");
       case LORAMAC_STATUS_SKIPPED_APP_DATA:
           ketCube_terminal_NewDebugPrintln(KETCUBE_LISTS_MODULEID_LORA, "McpReq: skippedAD");
           break;
       case LORAMAC_STATUS_NO_CHANNEL_FOUND:
           ketCube_terminal_NewDebugPrintln(KETCUBE_LISTS_MODULEID_LORA, "McpReq: noCHFound");
           break;
       case LORAMAC_STATUS_DUTYCYCLE_RESTRICTED:
           ketCube_terminal_NewDebugPrintln(KETCUBE_LISTS_MODULEID_LORA, "McpReq: dutyCycle");
           break;
       default:
           ketCube_terminal_NewDebugPrintln(KETCUBE_LISTS_MODULEID_LORA, "McpReq: ?");
           break;
   }
   return LORA_ERROR;
   
}  

#ifdef LORAMAC_CLASSB_ENABLED
#if defined( USE_DEVICE_TIMING )
static LoraErrorStatus LORA_DeviceTimeReq( void)
{
  MlmeReq_t mlmeReq;

  mlmeReq.Type = MLME_DEVICE_TIME;

  if( LoRaMacMlmeRequest( &mlmeReq ) == LORAMAC_STATUS_OK )
  {
    return LORA_SUCCESS;
  }
  else
  {
    return LORA_ERROR;
  }
}
#else
static LoraErrorStatus LORA_BeaconTimeReq( void)
{
  MlmeReq_t mlmeReq;

  mlmeReq.Type = MLME_BEACON_TIMING;

  if( LoRaMacMlmeRequest( &mlmeReq ) == LORAMAC_STATUS_OK )
  {
    return LORA_SUCCESS;
  }
  else
  {
    return LORA_ERROR;
  }
}
#endif

static LoraErrorStatus LORA_BeaconReq( void)
{
  MlmeReq_t mlmeReq;

  mlmeReq.Type = MLME_BEACON_ACQUISITION;

  if( LoRaMacMlmeRequest( &mlmeReq ) == LORAMAC_STATUS_OK )
  {
    return LORA_SUCCESS;
  }
  else
  {
    return LORA_ERROR;
  }
}

static LoraErrorStatus LORA_PingSlotReq( void)
{

  MlmeReq_t mlmeReq;

  mlmeReq.Type = MLME_LINK_CHECK;
  LoRaMacMlmeRequest( &mlmeReq );

  mlmeReq.Type = MLME_PING_SLOT_INFO;
  mlmeReq.Req.PingSlotInfo.PingSlot.Fields.Periodicity = LORAWAN_DEFAULT_PING_SLOT_PERIODICITY;
  mlmeReq.Req.PingSlotInfo.PingSlot.Fields.RFU = 0;

  if( LoRaMacMlmeRequest( &mlmeReq ) == LORAMAC_STATUS_OK )
  {
      return LORA_SUCCESS;
  }
  else
  {
     return LORA_ERROR;
  }
}
#endif /* LORAMAC_CLASSB_ENABLED */

LoraErrorStatus LORA_RequestClass( DeviceClass_t newClass )
{
  LoraErrorStatus Errorstatus = LORA_SUCCESS;
  MibRequestConfirm_t mibReq;
  DeviceClass_t currentClass;
  
  mibReq.Type = MIB_DEVICE_CLASS;
  LoRaMacMibGetRequestConfirm( &mibReq );
  
  currentClass = mibReq.Param.Class;
  /*attempt to swicth only if class update*/
  if (currentClass != newClass)
  {
    switch (newClass)
    {
      case CLASS_A:
      {
        mibReq.Param.Class = CLASS_A;
        if( LoRaMacMibSetRequestConfirm( &mibReq ) == LORAMAC_STATUS_OK )
        {
        /*switch is instantanuous*/
          LoRaMainCallbacks->LORA_ConfirmClass(CLASS_A);
        }
        else
        {
          Errorstatus = LORA_ERROR;
        }
        break;
      }
      case CLASS_B:
      {
#ifdef LORAMAC_CLASSB_ENABLED
        if (currentClass != CLASS_A)
        {
          Errorstatus = LORA_ERROR;
        }
        /*switch is not instantanuous*/
        Errorstatus = LORA_BeaconReq( );
#else
        ketCube_terminal_ErrorPrintln(KETCUBE_LISTS_MODULEID_LORA, "warning: LORAMAC_CLASSB_ENABLED has not been defined at compilation\n\r");
#endif /* LORAMAC_CLASSB_ENABLED */
        break;
      }
      case CLASS_C:
      {
        if (currentClass != CLASS_A)
        {
          Errorstatus = LORA_ERROR;
        }
        /*switch is instantanuous*/
        mibReq.Param.Class = CLASS_C;
        if( LoRaMacMibSetRequestConfirm( &mibReq ) == LORAMAC_STATUS_OK )
        {
          LoRaMainCallbacks->LORA_ConfirmClass(CLASS_C);
        }
        else
        {
            Errorstatus = LORA_ERROR;
        }
        break;
      }
      default:
        break;
    } 
  }
  return Errorstatus;
}

void LORA_GetCurrentClass( DeviceClass_t *currentClass )
{
  MibRequestConfirm_t mibReq;
  
  mibReq.Type = MIB_DEVICE_CLASS;
  LoRaMacMibGetRequestConfirm( &mibReq );
  
  *currentClass = mibReq.Param.Class;
}


void lora_config_print(void)
{
  if (lora_config.otaa == LORA_ENABLE) {
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "OTAA Mode enabled"); 
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "devEUI=%s", ketCube_common_bytes2Str(&(lora_config.DevEui[0]), KETCUBE_LORA_CFGLEN_DEVEUI));
#if (KETCUBE_LORA_LRWAN_VERSION == KETCUBE_LORA_LRWAN_VERSION_V11x)
    // v1.1.0
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "joinEui=%s", ketCube_common_bytes2Str(&(lora_config.JoinEui[0]), KETCUBE_LORA_CFGLEN_APPEUI));
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "nwkKey=%s", ketCube_common_bytes2Str(&(lora_config.NwkKey[0]), KETCUBE_LORA_CFGLEN_NWKKEY));
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "appKey=%s", ketCube_common_bytes2Str(&(lora_config.AppKey[0]), KETCUBE_LORA_CFGLEN_APPKEY));
#else
    // v1.0.3
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "appEUI=%s", ketCube_common_bytes2Str(&(lora_config.JoinEui[0]), KETCUBE_LORA_CFGLEN_APPEUI));
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "appKey=%s", ketCube_common_bytes2Str(&(lora_config.NwkKey[0]), KETCUBE_LORA_CFGLEN_NWKKEY));
#endif
  } else {
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "ABP Mode enabled"); 
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "devEUI=%s", ketCube_common_bytes2Str(&(lora_config.DevEui[0]), KETCUBE_LORA_CFGLEN_DEVEUI));
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "devAddr=%08X", DevAddr);
      
#if (KETCUBE_LORA_LRWAN_VERSION == KETCUBE_LORA_LRWAN_VERSION_V11x)
      // v1.1.0
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "nwkSEncKey=%s", ketCube_common_bytes2Str(&(lora_config.NwkSEncKey[0]), KETCUBE_LORA_CFGLEN_NWKSENCKEY));
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "sNwkSIntKey=%s", ketCube_common_bytes2Str(&(lora_config.SNwkSIntKey[0]), KETCUBE_LORA_CFGLEN_SNWKSINTKEY));
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "fNwkSIntKey=%s", ketCube_common_bytes2Str(&(lora_config.FNwkSIntKey[0]), KETCUBE_LORA_CFGLEN_FNWKSINTKEY));
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "appSKey=%s", ketCube_common_bytes2Str(&(lora_config.AppSKey[0]), KETCUBE_LORA_CFGLEN_APPSKEY));
#else
    // v1.0.3
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "nwkSKey=%s", ketCube_common_bytes2Str(&(lora_config.NwkSEncKey[0]), KETCUBE_LORA_CFGLEN_NWKSENCKEY));
    ketCube_terminal_InfoPrintln(KETCUBE_LISTS_MODULEID_LORA, "appSKey=%s", ketCube_common_bytes2Str(&(lora_config.AppSKey[0]), KETCUBE_LORA_CFGLEN_APPSKEY));
#endif
  }
}

void lora_config_duty_cycle_set(LoraState_t duty_cycle)
{
   lora_config.duty_cycle = duty_cycle;
   LoRaMacTestSetDutyCycleOn((duty_cycle == LORA_ENABLE) ? 1 : 0);
}

LoraState_t lora_config_duty_cycle_get(void)
{
   return lora_config.duty_cycle;
}

void lora_config_reqack_set(LoraConfirm_t reqack)
{
   lora_config.ReqAck = reqack;
}

LoraConfirm_t lora_config_reqack_get(void)
{
   return lora_config.ReqAck;
}

int8_t lora_config_snr_get(void)
{
   return lora_config.Snr;
}

int16_t lora_config_rssi_get(void)
{
   return lora_config.Rssi;
}

LoraState_t lora_config_isack_get(void)
{
   if (lora_config.McpsConfirm == NULL)
   {
    return LORA_DISABLE;
   }
   else
   {
    return (lora_config.McpsConfirm->AckReceived ? LORA_ENABLE : LORA_DISABLE);
   }
}

LoraState_t lora_config_otaa_get(void)
{
  return lora_config.otaa;
}

/* Dummy data sent periodically to let the tester respond with start test command*/
static TimerEvent_t TxcertifTimer;

void OnCertifTimer( void* context)
{
  uint8_t Dummy[1]= {1};
  lora_AppData_t AppData;
  AppData.Buff=Dummy;
  AppData.BuffSize=sizeof(Dummy);
  AppData.Port = 224;

  LORA_send( &AppData, LORAWAN_UNCONFIRMED_MSG);
}

void lora_wan_certif( void )
{
  LoRaMacTestSetDutyCycleOn( false );
  LORA_Join( );
  TimerInit( &TxcertifTimer,  OnCertifTimer); /* 8s */
  TimerSetValue( &TxcertifTimer,  8000); /* 8s */
  TimerStart( &TxcertifTimer );
}


/**
 *  lora KETCube Init
 */
ketCube_cfg_Error_t lora_ketCubeInit(void) 
{
   uint8_t i;

   if (ketCube_lora_moduleCfg.cfg.devEUIType == KETCUBE_LORA_SELDEVEUI_CUSTOM) {
      memcpy(&(lora_config.DevEui[0]), &(ketCube_lora_moduleCfg.devEUI[0]), 8);
      lora_config.static_dev_eui = 1;
   } else {
      lora_config.static_dev_eui = 0;
   }

   if (ketCube_lora_moduleCfg.cfg.connectionType == KETCUBE_LORA_SELCONNMETHOD_ABP) {   
      // ABP
      #if (KETCUBE_LORA_LRWAN_VERSION == KETCUBE_LORA_LRWAN_VERSION_V11x)
         memcpy1(lora_config.NwkSEncKey, ketCube_lora_moduleCfg.nwkSEncKey, KETCUBE_LORA_CFGLEN_NWKSENCKEY);
         memcpy1(lora_config.SNwkSIntKey, ketCube_lora_moduleCfg.sNwkSIntKey, KETCUBE_LORA_CFGLEN_SNWKSINTKEY);
         memcpy1(lora_config.FNwkSIntKey, ketCube_lora_moduleCfg.fNwkSIntKey, KETCUBE_LORA_CFGLEN_FNWKSINTKEY);
         memcpy1(lora_config.AppSKey, ketCube_lora_moduleCfg.appSKey, KETCUBE_LORA_CFGLEN_APPSKEY);
      #else
         memcpy1(lora_config.NwkSEncKey, ketCube_lora_moduleCfg.nwkSEncKey, KETCUBE_LORA_CFGLEN_NWKSENCKEY);
         memcpy1(lora_config.SNwkSIntKey, ketCube_lora_moduleCfg.nwkSEncKey, KETCUBE_LORA_CFGLEN_NWKSENCKEY);
         memcpy1(lora_config.FNwkSIntKey, ketCube_lora_moduleCfg.nwkSEncKey, KETCUBE_LORA_CFGLEN_NWKSENCKEY);
         memcpy1(lora_config.AppSKey, ketCube_lora_moduleCfg.appSKey, KETCUBE_LORA_CFGLEN_APPSKEY);
      #endif
      memcpy(&(lora_config.NwkID), &(ketCube_lora_moduleCfg.netID[0]), 4);
      /* Copy devAddr in the reverse byte order to achieve correct endianity */
      for (i = 0; i < 4 ; i++) {
          ((uint8_t *) &DevAddr)[i] = ketCube_lora_moduleCfg.devAddr[3-i];
      }
      lora_config.otaa = LORA_DISABLE;
      lora_config.static_dev_addr = 1;
   } else {
      // OTAA
      #if (KETCUBE_LORA_LRWAN_VERSION == KETCUBE_LORA_LRWAN_VERSION_V11x)
         memcpy1(lora_config.JoinEui, ketCube_lora_moduleCfg.appEUI, KETCUBE_LORA_CFGLEN_APPEUI);
         memcpy1(lora_config.AppKey, ketCube_lora_moduleCfg.appKey, KETCUBE_LORA_CFGLEN_APPKEY);
         memcpy1(lora_config.NwkKey, ketCube_lora_moduleCfg.nwkKey, KETCUBE_LORA_CFGLEN_NWKKEY);
      #else
         memcpy1(lora_config.JoinEui, ketCube_lora_moduleCfg.appEUI, KETCUBE_LORA_CFGLEN_APPEUI);
         memcpy1(lora_config.NwkKey, ketCube_lora_moduleCfg.appKey, KETCUBE_LORA_CFGLEN_NWKKEY);
      #endif
       
      lora_config.otaa = LORA_ENABLE;
      lora_config.static_dev_addr = 0;
   }

   return KETCUBE_CFG_OK;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

