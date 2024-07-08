#ifndef CAMERA_H
#define CAMERA_H

#include <QSet>
#include <QVector3D>
#include <QEvent>
#include <QWidget>
#include <QtMath>
#include <QMatrix4x4>
#include <QKeyEvent>
#include <QTime>


class Camera
{
public:
    Camera(QWidget *widget);

    float getMoveSpeed() const;
    void setMoveSpeed(float value);

    float getSensitivity() const;
    void setSensitivity(float value);

    float getYaw() const;
    void setYaw(float value);

    float getPitch() const;
    void setPitch(float value);

    QVector3D getCameraPos() const;
    void setCameraPos(const QVector3D& value);
    void init();                    
    void handle(QEvent *event);     
    QMatrix4x4 getView() const;    

private:
    bool press=true;
    QWidget *widget;

    float yaw;                 
    float pitch;               
    float sensitivity;          

    QVector3D cameraPos;        

    QVector3D cameraDirection;  
    QVector3D cameraRight;     
    QVector3D cameraUp;         

    float moveSpeed;    
    QSet<int> keys;     

    int timeId;         
    float deltaTime;    
    float lastFrame;    

    QMatrix4x4 view;    

};

#endif // CAMERA_H
