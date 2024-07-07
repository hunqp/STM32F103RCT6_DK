#ifndef __SNTP_DAEMON_H
#define __SNTP_DAEMON_H

class SNTPDaemon {
public:
    static void Start();
    static void Stop();

private:
    static void onService(void*); 
};

#endif /* __SNTP_DAEMON_H */