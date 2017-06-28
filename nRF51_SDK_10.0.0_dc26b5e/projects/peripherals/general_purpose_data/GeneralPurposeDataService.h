#ifndef SERVICE_H__
#define SERVICE_H__

#include <stdint.h>
#include "ble.h"
#include "ble_srv_common.h"

#define BASE_UUID             		{{0x23, 0xD1, 0x13, 0xEF, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0xBA, 0xDC}} // 128-bit base UUID
#define SERVICE_UUID             	0x3164
#define BYTE_CHARACTERISTIC_UUID	0x1533
#define SHORT_CHARACTERISTIC_UUID	0x1534
#define INT_CHARACTERISTIC_UUID		0x1535
#define LONG_CHARACTERISTIC_UUID	0x1536
#define FLOAT_CHARACTERISTIC_UUID	0x1537
#define DOUBLE_CHARACTERISTIC_UUID	0x1538
#define STRING_CHARACTERISTIC_UUID	0x1539

#define MAX_STRING_LENGTH 200

typedef struct Characteristic
{
	uint16_t					uuid;
	char*						name;
	char*						format; // printf and scanf
	uint16_t					size;
	ble_gatts_char_handles_t*	handles;
	bool						notificationsEnabled;
}
Characteristic_t;

typedef struct DataService
{
	ble_uuid_t  				uuid;
    uint16_t    				connectionHandle;

	Characteristic_t*			characteristics;
    uint16_t    				characteristicsCount;

	void 						(*bleEventHandler)(struct DataService*, ble_evt_t*); /* Callback into service when the application receives a BLE event */
	void		 				(*updateHandler)(struct DataService*, Characteristic_t*, char*); /* Callback into service when the application provides a new value */
	void		 				(*buttonHandler)(struct DataService*, int); /* Callback into service when a button is pressed */
}
DataService_t;

DataService_t* createDataService(Characteristic_t*, uint16_t);

#endif
