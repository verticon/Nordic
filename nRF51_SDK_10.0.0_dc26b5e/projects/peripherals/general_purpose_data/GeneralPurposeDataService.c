#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "bsp.h"
#include "ble_srv_common.h"
#include "app_error.h"

#include "SEGGER_RTT.h"

#include "GeneralPurposeData.h"
#include "GeneralPurposeDataService.h"

static void buttonHandler(DataService_t* pService, int buttonNumber) {
    if (pService != NULL && pService->connectionHandle != BLE_CONN_HANDLE_INVALID)
	{
		switch (buttonNumber)
		{
			case BSP_BUTTON_0:
			{
				break;
			}

			case BSP_BUTTON_1:
			{
				break;
			}

			case BSP_BUTTON_2:
			{
				break;
			}

			case BSP_BUTTON_3:
			{
				break;
			}
		}
	}
}
static void updateHandler(DataService_t* pService, Characteristic_t* pCharacteristic, char* pNewValue) {

	#ifdef SEGGER_LOGGING
	SEGGER_RTT_printf(0, "Updating the %s characteristic: %s\n", pCharacteristic->name, pNewValue);
	#endif

	uint8_t buffer[pCharacteristic->size];
	memset(buffer, 0, sizeof(buffer));

	if (pCharacteristic->uuid == STRING_CHARACTERISTIC_UUID)
	{
		if (strlen(pNewValue) < sizeof(buffer))
		{
			strcpy((char *)buffer, pNewValue);
		}
		else
		{
			#ifdef SEGGER_LOGGING
			SEGGER_RTT_printf(0, "The size of new value is greater than that of the %s characteristic\n", pCharacteristic->size);
			#endif
			return;
		}
	}
	else
	{
		if (sscanf(pNewValue, pCharacteristic->format, (unsigned char *) buffer) != 1)
		{
			#ifdef SEGGER_LOGGING
			SEGGER_RTT_printf(0, "Cannot scan the new value\n");
			#endif
			return;
		}
	}

	// Note: The ble_gatts_value_t specifys a range within the total space allocated for the characteristic.
	// If that range is less than the total space then the bytes outside of that range will not be altered.
	ble_gatts_value_t value;
	value.p_value = buffer;
	value.offset = 0;
	value.len = sizeof(buffer);
	sd_ble_gatts_value_set(pService->connectionHandle, pCharacteristic->handles->value_handle, &value);

	if (pCharacteristic->notificationsEnabled)
	{
		ble_gatts_hvx_params_t params;
		memset(&params, 0, sizeof(params));
		
		params.handle = pCharacteristic->handles->value_handle;
		params.p_data = value.p_value;
		params.p_len  = &value.len;
		params.type   = BLE_GATT_HVX_NOTIFICATION;

		uint32_t status = sd_ble_gatts_hvx(pService->connectionHandle, &params);
		switch (status)
		{
			case NRF_SUCCESS:
				#ifdef SEGGER_LOGGING
				SEGGER_RTT_printf(0, "A notification was successfully sent\n");
				#endif
				break;
			default:
				#ifdef SEGGER_LOGGING
				SEGGER_RTT_printf(0, "A notification could not be sent; error = %d\n", status);
				#endif
				break;
		}
	}
}

static void onBleWrite(DataService_t* pService, ble_evt_t* pEvent) {

    ble_gatts_evt_write_t* pWriteEvent = &pEvent->evt.gatts_evt.params.write;

	for (int i = 0; i < pService->characteristicsCount; ++i)
	{
		Characteristic_t* pCharacteristic = pService->characteristics + i;

		if (pWriteEvent->handle == pCharacteristic->handles->value_handle)
		{
			#ifdef SEGGER_LOGGING
			uint8_t data[pCharacteristic->size];

			ble_gatts_value_t value;
			value.len = sizeof(data);
			value.offset = 0;
			value.p_value = data;

			sd_ble_gatts_value_get(pService->connectionHandle, pCharacteristic->handles->value_handle, &value);

			char format[100];
			sprintf(format,  "%d bytes were written to the %s characteristic: %s\n", pWriteEvent->len, pCharacteristic->name, pCharacteristic->format);
			SEGGER_RTT_printf(0, format, data); 
			#endif

			return;
		}
		else if (pWriteEvent->handle == pCharacteristic->handles->cccd_handle)
		{
			pCharacteristic->notificationsEnabled = ble_srv_is_notification_enabled(pWriteEvent->data);
			#ifdef SEGGER_LOGGING
			SEGGER_RTT_printf(0, "%s notifications were %s\n", pCharacteristic->name, pCharacteristic->notificationsEnabled ? "Enabled" : "Disabled"); 
			#endif
		}
	}
	
	#ifdef SEGGER_LOGGING
	SEGGER_RTT_printf(0, "BLE write handler: unrecognized write event\n");
	#endif
}


static void onBleEvent(DataService_t* pService, ble_evt_t* pEvent) {
    switch (pEvent->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
			pService->connectionHandle = pEvent->evt.gap_evt.conn_handle;
            break;
            
        case BLE_GAP_EVT_DISCONNECTED:
			pService->connectionHandle = BLE_CONN_HANDLE_INVALID;
            break;
            
        case BLE_GATTS_EVT_WRITE:
            onBleWrite(pService, pEvent);
            break;
            
        default:
            break;
    }
}

static bool addCharacteristic(DataService_t* pService, Characteristic_t *pCharacteristic) {
    ble_gatts_attr_md_t cccd;
    memset(&cccd, 0, sizeof(cccd));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd.write_perm);
    cccd.vloc = BLE_GATTS_VLOC_STACK;

	pCharacteristic->notificationsEnabled = false;
 
    ble_gatts_char_md_t configuration;
    memset(&configuration, 0, sizeof(configuration));
    configuration.p_char_user_desc  = (uint8_t *) pCharacteristic->name;
	configuration.char_user_desc_size = strlen(pCharacteristic->name);
	configuration.char_user_desc_max_size = strlen(pCharacteristic->name);
    configuration.p_user_desc_md    = NULL;
    configuration.char_props.read   = 1;
    configuration.char_props.write  = 1;
    configuration.char_props.notify = 1;
    configuration.p_char_pf         = NULL;
    configuration.p_cccd_md         = &cccd;
    configuration.p_sccd_md         = NULL;
  
    ble_uuid_t uuid;
    uuid.type = pService->uuid.type;
    uuid.uuid = pCharacteristic->uuid;
    
    ble_gatts_attr_md_t metadata;
    memset(&metadata, 0, sizeof(metadata));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&metadata.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&metadata.write_perm);
    metadata.vloc       = BLE_GATTS_VLOC_STACK;
    metadata.rd_auth    = 0;
    metadata.wr_auth    = 0;
    metadata.vlen       = 0;
    
	uint8_t initialValue[pCharacteristic->size];
    memset(&initialValue, 0, sizeof(initialValue));
    ble_gatts_attr_t characteristic;
    memset(&characteristic, 0, sizeof(characteristic));
    characteristic.p_uuid       = &uuid;
    characteristic.p_attr_md    = &metadata;
    characteristic.init_len     = sizeof(initialValue);
    characteristic.init_offs    = 0;
    characteristic.max_len      = sizeof(initialValue);
    characteristic.p_value      = (uint8_t *) &initialValue;
    
    uint32_t status = sd_ble_gatts_characteristic_add(pService->connectionHandle, &configuration, &characteristic, pCharacteristic->handles);
	#ifdef SEGGER_LOGGING
	if (status == NRF_SUCCESS) 
		SEGGER_RTT_printf(0, "The %s characteristic was successfully added\n", pCharacteristic->name);
	else  
		SEGGER_RTT_printf(0, "Cannot add the %s characteristic; error code = %d\n", pCharacteristic->name, status);
	#endif
	return status == NRF_SUCCESS;
}

DataService_t* createDataService(Characteristic_t* characteristics, uint16_t count) {
    uint32_t   err_code; // Variable to hold return codes from library and softdevice functions
	DataService_t*	pService = (DataService_t*) malloc(sizeof(DataService_t));
    
    // Declare 16 bit service and 128 bit base UUIDs and add them to BLE stack table     
	pService->uuid.uuid = SERVICE_UUID;
	ble_uuid128_t base_uuid = BASE_UUID;
	err_code = sd_ble_uuid_vs_add(&base_uuid, &pService->uuid.type);
	APP_ERROR_CHECK(err_code);
	
    // Add service
	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &pService->uuid, &pService->connectionHandle);
	APP_ERROR_CHECK(err_code);

	pService->bleEventHandler = onBleEvent;
	pService->buttonHandler = buttonHandler;
	pService->updateHandler = updateHandler;

	pService->characteristics = characteristics;
	pService->characteristicsCount = count;
	for (int i = 0; i < pService->characteristicsCount; ++i)
	{
		addCharacteristic(pService, pService->characteristics + i);
	}
	
	pService->connectionHandle = BLE_CONN_HANDLE_INVALID;

	return pService;
}
