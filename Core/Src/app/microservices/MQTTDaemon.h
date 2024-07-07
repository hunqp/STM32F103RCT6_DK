#ifndef __MQTT_DAEMON_H
#define __MQTT_DAEMON_H

class MQTTDaemon {
public:
    static void Start();
    static void Stop();

private:
    static void onService(void*); 
};

#endif /* __MQTT_DAEMON_H */