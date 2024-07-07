#ifndef __APP_H
#define __APP_H

#ifdef __cplusplus
extern "C"
{
#endif

#define URTOS_DEFINE_SIGNAL  (10)

/*-------------------------------------------------------*
/ TASK SYSTEM SIGNALS
/
/-------------------------------------------------------*/
#define SIG_SYSTEM_KEEP_ALIVE_INTERVAL        	 ( 1000 )
#define SIG_SYSTEM_PERIOD_5_SECONDS_DO_INTERVAL	 ( 5000 )
#define SIG_SYSTEM_PERIOD_10_SECONDS_DO_INTERVAL ( 10000 )
#define SIG_SYSTEM_PERIOD_15_SECONDS_DO_INTERVAL ( 15000 )

enum {
	SIG_SYSTEM_KEEP_PING_ALIVE = URTOS_DEFINE_SIGNAL,
	SIG_SYSTEM_REBOOT,
	SIG_SYSTEM_PERIOD_5_SECONDS_DO,
	SIG_SYSTEM_PERIOD_10_SECONDS_DO,
	SIG_SYSTEM_PERIOD_15_SECONDS_DO,
};

/*-------------------------------------------------------*
/ TASK CONSOLE SIGNALS
/
/-------------------------------------------------------*/
enum {
	SIG_CONSOLE_CLI_INIT = URTOS_DEFINE_SIGNAL,
	SIG_CONSOLE_HANDLE_CMD_LINE,
};

/*-------------------------------------------------------*
/ TASK CLOUD SIGNALS
/
/-------------------------------------------------------*/
#define SIG_CLOUD_DHCP_TIMER_HANDLE_INTERVAL	( 1000 )

enum {
	SIG_CLOUD_INTERNET_ON_CONNECTED = URTOS_DEFINE_SIGNAL,
	SIG_CLOUD_INTERNET_ON_DISCONNECTED,
	SIG_CLOUD_SNTP_ON_CONNECTED,
	SIG_CLOUD_MQTT_ON_CONNECTED,
	SIG_CLOUD_MQTT_PERIODIC_KEEP_ALIVE,
	SIG_CLOUD_MQTT_ON_MESSAGE,
};

/*-------------------------------------------------------*
/ TASK DEVICE MANAGER SIGNALS
/
/-------------------------------------------------------*/

enum {
	SIG_DMN_CONTROL_CM4_POWER_REQ = URTOS_DEFINE_SIGNAL,
	SIG_DMN_CONTROL_NX_POWER_REQ,
};

#define HTTP_SOCKET_NUMBER_1 	0
#define HTTP_SOCKET_NUMBER_2 	1
#define HTTP_SOCKET_NUMBER_3 	2
#define HTTP_SOCKET_NUMBER_4 	3
#define DHCP_SOCKET_NUMBER 		4
#define DNS_SOCKET_NUMBER 		5
#define MQTT_SOCKET_NUMBER 		6
#define SNTP_SOCKET_NUMBER		7

#define HW_VERSION      		(const char*)"v1.0.0"
#define FW_VERSION      		(const char*)"v1.0.0"

extern void appInit();

#ifdef __cplusplus
}
#endif

#endif /* __APP_H */
