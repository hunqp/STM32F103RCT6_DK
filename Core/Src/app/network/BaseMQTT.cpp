#include "app.h"
#include "app_dbg.h"
#include "network.h"
#include "BaseMQTT.h"

#define TAG     "BaseMQTT"

BaseMQTT::BaseMQTT() {
	memset(host, 0, sizeof(host));
	memset(username, 0, sizeof(username));
	memset(password, 0, sizeof(password));
	
	#if BASEMQTT_MUTEX_EN
    mMutex = xSemaphoreCreateMutex();
    #endif
}

BaseMQTT::~BaseMQTT() {

}

void BaseMQTT::InitConnection(const char *host, uint16_t port) {
	if (mState != eState::Initialize) {
		/* DO NOT: Initialize again */
		return;
	}

	strcpy(this->host, host);
	this->port = port;

	/* ----------------------- */
	/* Prepare data connection */
	/* ----------------------- */
	if (dnsNetworkRun((const char*)host, this->dnsHostIP) == NETWORK_RET_FAILURE) {
		APP_DBG(TAG, "Couldn't get \"%s\" from DNS", host);
		return;
	}
	APP_DBG(TAG, "Host: %s, DNS IP: %d.%d.%d.%d", host, this->dnsHostIP[0], this->dnsHostIP[1], this->dnsHostIP[2], this->dnsHostIP[3]);

	this->c = DefaultClient;
	this->c.keepAliveInterval = 60;
	NewNetwork(&this->nw, MQTT_SOCKET_NUMBER);

	this->d = MQTTPacket_connectData_initializer;
	this->d.willFlag 			= 0;
	this->d.MQTTVersion 		= 3;
	this->d.clientID.cstring 	= (char*)id;
	this->d.username.cstring 	= (char*)(strlen(username) > 0 ? username : "");
	this->d.password.cstring 	= (char*)(strlen(password) > 0 ? password : "");
	this->d.keepAliveInterval 	= 60;
	this->d.cleansession 		= 0;

	mState = eState::Disconnected;
}

void BaseMQTT::setAuthentication(const char *username, const char *password) {
	if (username != NULL) {
		strcpy(this->username, username);
	}

	if (password != NULL) {
		strcpy(this->password, password);
	}
}

int8_t BaseMQTT::tryConnect() {
	#if BASEMQTT_MUTEX_EN
    xSemaphoreTake(mMutex, 0xFFFF);
    #endif

	ConnectNetwork(&this->nw, this->dnsHostIP, port);
	MQTTClientInit(&this->c, &this->nw, 1000, txBuffers, sizeof(txBuffers), rxBuffers, sizeof(rxBuffers));

	int ret = MQTTConnect(&this->c, &this->d);
	
	if (ret != SUCCESSS) {
		APP_DBG(TAG, "Couldn't connect to broker \"%s\"", host);
		mState = eState::Disconnected;
	}
	else {
		mState = eState::Connected;
		BaseMQTT::onConnected();
		APP_DBG(TAG, "MQTT Connect to \"%s\" successfully", host);
	}

	#if BASEMQTT_MUTEX_EN
    xSemaphoreGive(mMutex);
    #endif

	return (ret == SUCCESSS ? 1 : -1);
}

bool BaseMQTT::isConnected() {
	#if BASEMQTT_MUTEX_EN
    xSemaphoreTake(mMutex, 0xFFFF);
    #endif

	bool bConnected = (mState == eState::Connected) ? true : false;

	#if BASEMQTT_MUTEX_EN
    xSemaphoreGive(mMutex);
    #endif

	return bConnected;
}

void BaseMQTT::setState(eState state) {
	#if BASEMQTT_MUTEX_EN
    xSemaphoreTake(mMutex, 0xFFFF);
    #endif

	mState = state;

	#if BASEMQTT_MUTEX_EN
    xSemaphoreGive(mMutex);
    #endif
}

BaseMQTT::eState BaseMQTT::getState() {
	#if BASEMQTT_MUTEX_EN
    xSemaphoreTake(mMutex, 0xFFFF);
    #endif

	eState state = mState;

	#if BASEMQTT_MUTEX_EN
    xSemaphoreGive(mMutex);
    #endif

	return state;
}

void BaseMQTT::forceDisconnect() {
	#if BASEMQTT_MUTEX_EN
    xSemaphoreTake(mMutex, 0xFFFF);
    #endif
	
	if (mState == eState::Connected) {
		printf("%s, %d\r\n", __func__, __LINE__);
		MQTTDisconnect(&this->c);
		printf("%s, %d\r\n", __func__, __LINE__);
		mState = eState::Disconnected;
	}

	#if BASEMQTT_MUTEX_EN
    xSemaphoreGive(mMutex);
    #endif
}

int8_t BaseMQTT::performSubcribe(const char *topic, QoS qos) {
	int ret = FAILURE;
	#if BASEMQTT_MUTEX_EN
    xSemaphoreTake(mMutex, 0xFFFF);
    #endif

	if (mState == eState::Connected) {
		ret = MQTTSubscribe(&this->c, topic, qos, BaseMQTT::onMessage);
	}

	#if BASEMQTT_MUTEX_EN
    xSemaphoreGive(mMutex);
    #endif

	APP_DBG(TAG, "Subscribe topic \"%s\" %s", topic, (ret == SUCCESSS) ? "SUCCESS" : "FAILED");

	return (ret == SUCCESSS) ? MQTT_RET_SUCCESS : MQTT_RET_FAILURE;
}

void BaseMQTT::performUnsubcribe(const char *topic) {
	#if BASEMQTT_MUTEX_EN
    xSemaphoreTake(mMutex, 0xFFFF);
    #endif

	if (mState == eState::Connected) {
		MQTTUnsubscribe(&this->c, topic);
	}

	#if BASEMQTT_MUTEX_EN
    xSemaphoreGive(mMutex);
    #endif
}

void BaseMQTT::onYEILD() {
	if (mState == BaseMQTT::eState::Connected) {
		MQTTYield(&this->c, this->d.keepAliveInterval);
	}
}

int8_t BaseMQTT::performPublish(const char *topic, void *payload, size_t payloadLen) {
	int ret = FAILURE;

	#if BASEMQTT_MUTEX_EN
    xSemaphoreTake(mMutex, 0xFFFF);
    #endif

	if (mState == eState::Connected) {
		MQTTMessage msg;
		msg.qos 		= QOS0,
		msg.retained	= 0;
		msg.dup			= 0;
		msg.id			= 0;
		msg.payload		= payload;
		msg.payloadlen = payloadLen;

		ret = MQTTPublish(&this->c, topic, &msg);	
	}

	#if BASEMQTT_MUTEX_EN
    xSemaphoreGive(mMutex);
    #endif

	APP_DBG(TAG, "Publish topic \"%s\" %s", topic, (ret == SUCCESSS) ? "SUCCESS" : "FAILED");
	
	return (ret == SUCCESSS ? MQTT_RET_SUCCESS : MQTT_RET_FAILURE);
}

void BaseMQTT::dumpMQTTInformation() {
	APP_PRINT("\r\n-- MQTT INFORMATION --\r\n");
    APP_PRINT("Host   : %s\r\n", host);
	APP_PRINT("Port   : %d\r\n", port);
	APP_PRINT("Status : %s\r\n", (mState == BaseMQTT::eState::Connected) ? "Connected" : "Disconnected");
    APP_PRINT("\r\n");
}