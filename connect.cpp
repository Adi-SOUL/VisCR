#include "includes/connect.h"
#include <QVector>
#include <QMatrix4x4>
#include <cmath>
#include <winsock.h>
#include <QEventLoop>
#include <QTimer>
#pragma comment(lib,"ws2_32.lib")

#define PI 3.14159265
const float ratio = 0.5;

bool flag = false;
float sqrt_2 = sqrt(2);


QMatrix4x4 get_m(float sphi, float _step, float L) {
    float c_theta = cos(_step);
    float s_theta = sin(_step);
    QMatrix4x4 T_x;
    T_x.rotate(-(sphi*180.)/PI, QVector3D(0,1.0,0));
    QMatrix4x4 T_x1;
    T_x1.rotate((sphi*180.)/PI, QVector3D(0,1.0,0));

    QMatrix4x4 T_y;
    T_y.rotate((_step*180)/PI, QVector3D(0,0,1.));
    //qDebug() << cos(sphi) << T_x;
    QMatrix4x4 T;
    T.translate(QVector3D((L/_step - (L*c_theta)/_step)*cos(sphi),(L*s_theta)/_step,(L/_step - (L*c_theta)/_step)*sin(sphi)));
    QMatrix4x4 matrix = T_x*T_y*T_x1*T;
    return matrix;
}

Connecter::Connecter() {
    WORD w_req = MAKEWORD(2, 2);
    WSADATA wsadata;
    int err;
    err = WSAStartup(w_req, &wsadata);
    if (err != 0) {

        state1 += "Failed to initialize socket library!\n";
        return;
    }

    if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
        state1 += "The socket library version number does not match!\n";
        return;
    } else {
        state1 += "The socket library version is correct!\n";
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
    s_server = socket(AF_INET, SOCK_STREAM, 0);

    if (bind(s_server, (SOCKADDR*)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
        state1 += "Socket bind failed!\n" ;
        return;
    } else {
        state1 += "Socket bind successful!\n";
    }

    if (listen(s_server, SOMAXCONN) < 0) {
        state1 += "Failed to set monitoring status!\n";
        return;
    } else {
        state1 += "Setting monitoring status successfully!\n";
    }
}

Connecter::~Connecter() {
    closesocket(s_server);
    closesocket(s_accept);
    WSACleanup();
}

void Connecter::set_port(int _port) {
    port = _port;
}

void Connecter::disc() {
    if(connected){
        SOCKADDR_IN accept_add;
        accept_addr = accept_add;
        connected = false;
        End = true;
        state1 = "Disconnected\n";
    }
}

void Connecter::connect() {
    len = sizeof(SOCKADDR);
    s_accept = accept(s_server, (SOCKADDR*)&accept_addr, &len);
    if (s_accept == SOCKET_ERROR) {
        state1 = "Connection failed!\n";
        return;
    }
    connected = true;
    state1 = "connection succeeded!\n";
    End = false;

}

QVector<QMatrix4x4> Connecter::sim() {
    QVector<QMatrix4x4> f;
    if(!connected) {
        return f;
    }

    int num = 0;
    len = sizeof(SOCKADDR);

    std::string s = "s";
    send_len = send(s_accept, s.c_str(), 100, 0);
    if (send_len < 0) {
        return f;
    }

    recv_len = recv(s_accept, recv_buf, 100, 0);
    if (recv_len < 0) {
        return f;
    } else {
        num = QString(recv_buf).toInt();
    }

    std::string ss = "o";
    send_len = send(s_accept, ss.c_str(), 100, 0);
    if (send_len < 0) {
        return f;
    }

    for(int i=0; i<num; i++) {
        QMatrix4x4 m;
        for(int n=0; n<4; n++) {
            for(int j=0; j<4; j++) {
                recv_len = recv(s_accept, recv_buf, 100, 0);
                if (recv_len < 0) {
                    QVector<QMatrix4x4> ff;
                    return ff;
                } else {
                    m(n, j) = QString(recv_buf).toFloat();
                }
            }
        }

        for (int i = 0; i < 3; i++) {
            m(i, 3) /= 40.5;
        }

        f.append(m);
    }
    return f;
}
