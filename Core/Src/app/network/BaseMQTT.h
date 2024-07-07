#ifndef __BaseMQTT_H
#define __BaseMQTT_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "dns.h"
#include "MQTTClient.h"
#include "FreeRTOS.h"
#include "semphr.h"

#define BASEMQTT_MUTEX_EN           ( 0 )

#define DEFAULT_MQTT_BROKER_HOST    "broker.emqx.io"
#define DEFAULT_MQTT_BROKER_PORT    ( 1883 )

#define DEFAULT_MQTT_SUB_TOPIC      "v5/devices/me/telemetry"
#define DEFAULT_MQTT_PUB_TOPIC      "v5/devices/me/attributes"

#define MQTT_RET_FAILURE            (0)
#define MQTT_RET_SUCCESS            (1)

typedef struct {
    char *topic;
    char *payload;
} BaseMQTTMessage_t;

class BaseMQTT {
public:
    enum eState {
        Initialize,
        Disconnected,
        Connected,
    };

    BaseMQTT();
    ~BaseMQTT();

    void InitConnection(const char *host, uint16_t port);
    void setAuthentication(const char *username, const char *password);
    int8_t tryConnect();
    bool isConnected();
    void setState(eState state);
    eState getState();
    void forceDisconnect();
    int8_t performSubcribe(const char *topic, QoS qos);
    void performUnsubcribe(const char *topic);
    int8_t performPublish(const char *topic, void *payload, size_t len);
    void dumpMQTTInformation();
    void onYEILD();
    
    static void onConnected();
    static void onDisconnected();
    static void onMessage(MessageData*);
    
private:
    const char *id = "unknown-01";
    char host[32];
    uint8_t dnsHostIP[4];
    char username[16];
    char password[16];
    uint16_t port;
    eState mState = eState::Initialize;
    
    #if BASEMQTT_MUTEX_EN
    SemaphoreHandle_t mMutex;
    #endif

    uint8_t txBuffers[2048];
	uint8_t rxBuffers[2048];

    Network nw;
	MQTTClient c;
    MQTTPacket_connectData d;
};

#endif /* __BaseMQTT_H */
