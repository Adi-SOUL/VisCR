#ifndef MY_THREAD_H
#define MY_THREAD_H
#include <QObject>
#include <winsock.h>
#pragma comment(lib,"ws2_32.lib")

class Connecter : public QObject
{
    Q_OBJECT

public:
    Connecter();
    ~Connecter();
    void set_port(int);
    QVector<QMatrix4x4> sim();
    void connect();
    void disc();
    bool connected = false;
    int port = 8888;
    int send_len = 0;
    int recv_len = 0;
    int len = 1;
    char send_buf[1] = {'s'};
    char recv_buf[100];
    fd_set readSet;
    SOCKET s_server;
    SOCKET s_accept;
    SOCKADDR_IN server_addr;
    SOCKADDR_IN accept_addr;

    QString state1 = "";

private:

    bool End = true;
    float step = 0.1f;
    bool flag = false;
    float phi = 1.;

    float step2 = 0.1f;
    bool flag2 = false;
    float phi2 = 1.f;
    double px0, py0, pz0;

};

#endif // MY_THREAD_H
