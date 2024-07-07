#ifndef __DHCP_DAEMON_H
#define __DHCP_DAEMON_H

class DHCPDaemon {
public:
    static void Start();
    static void Stop();

private:
    static void onService(void*); 
};

#endif /* __DHCP_DAEMON_H */