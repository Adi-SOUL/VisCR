#include "includes/camera.h"
#include <iostream>

Camera::Camera(QWidget *widget)
    : widget(widget)
    , yaw(0)
    , pitch(0)
    , sensitivity(0.005f)
    , cameraPos(-10.f,3.f,0.0f)
    , cameraDirection(cos(yaw)*cos(pitch), sin(pitch), sin(yaw)*cos(pitch))
    , cameraRight(QVector3D::crossProduct({0.0f,1.0f,0.0f},cameraDirection))
, cameraUp(QVector3D::crossProduct(cameraDirection,cameraRight))
, moveSpeed(0.5f)
, timeId(0) {
}

float Camera::getMoveSpeed() const {
    return moveSpeed;
}

void Camera::setMoveSpeed(float value) {
    moveSpeed = value;
}

float Camera::getSensitivity() const {
    return sensitivity;
}

void Camera::setSensitivity(float value) {
    sensitivity = value;
}

float Camera::getYaw() const {

    return yaw;

}

void Camera::setYaw(float value) {
    yaw = value;
}

float Camera::getPitch() const {
    return pitch;
}

void Camera::setPitch(float value) {
    pitch = value;
}

QVector3D Camera::getCameraPos() const {
    return cameraPos;
}

void Camera::setCameraPos(const QVector3D &value) {
    cameraPos = value;
}

void Camera::handle(QEvent *e) {
    if(e->type()==QEvent::MouseMove) {
        if(!press) {
            widget->setCursor(Qt::BlankCursor);             
            widget->setMouseTracking(true);
            QMouseEvent *event=static_cast<QMouseEvent*>(e);
            float xoffset = event->x() - widget->rect().center().x();
            float yoffset = widget->rect().center().y() - event->y();
            yoffset *= sensitivity;
            xoffset *= sensitivity;
            yaw   += xoffset;
            pitch += yoffset;
            if(pitch > 1.55)         
                pitch =  1.55;
            if(pitch < -1.55)
                pitch = -1.55;
            cameraDirection.setX(cos(yaw)*cos(pitch));
            cameraDirection.setY(sin(pitch));
            cameraDirection.setZ(sin(yaw)*cos(pitch));
            view.setToIdentity();
            view.lookAt(cameraPos,cameraPos+cameraDirection,cameraUp);
            QCursor::setPos(widget->geometry().center());       
        } else {
            widget->setCursor(Qt::ArrowCursor);   
            widget->setMouseTracking(false);
        }
    } else if(e->type()==QEvent::Timer) {
        float cameraSpeed = moveSpeed * deltaTime;
        if (keys.contains(Qt::Key_W))                           
            cameraPos+=cameraSpeed * cameraDirection;
        if (keys.contains(Qt::Key_S))                           
            cameraPos -= cameraSpeed * cameraDirection;
        if (keys.contains(Qt::Key_A))                           
            cameraPos-=QVector3D::crossProduct(cameraDirection,cameraUp)*cameraSpeed;
        if (keys.contains(Qt::Key_D))                           
            cameraPos+=QVector3D::crossProduct(cameraDirection,cameraUp)*cameraSpeed;
        if (keys.contains(Qt::Key_Space))                       
            cameraPos.setY(cameraPos.y()+cameraSpeed);
        if (keys.contains(Qt::Key_Shift)) {                      
            float y = cameraPos.y()-cameraSpeed;
            cameraPos.setY(y);
        }
        if (keys.contains(Qt::Key_U)) {                      
            moveSpeed += 0.01;
        }
        if (keys.contains(Qt::Key_J)) {                      
            moveSpeed -= 0.01;
        }
        if (keys.contains(Qt::Key_Alt)) {
            press = true;
        }
        view.setToIdentity();
        view.lookAt(cameraPos,cameraPos+cameraDirection,cameraUp);
    } else if(e->type()==QEvent::KeyPress) {
        QKeyEvent *event=static_cast<QKeyEvent*>(e);
        keys.insert(event->key());                              
        if(!event->isAutoRepeat()&&timeId==0) {                 
            timeId=widget->startTimer(1);
        }
    } else if(e->type()==QEvent::KeyRelease) {
        QKeyEvent *event=static_cast<QKeyEvent*>(e);
        if(event->key()==Qt::Key_Alt) {
            press = false;
        }
        keys.remove(event->key());
        if(!event->isAutoRepeat()&&timeId!=0&&keys.empty()) {   
            widget->killTimer(timeId);
            timeId=0;                                          
        }
    } else if(e->type()==QEvent::UpdateRequest) {
        float time=QTime::currentTime().msecsSinceStartOfDay()/1000.0;
        deltaTime = time - lastFrame;                           
        lastFrame = time;
    } else if(e->type()==QEvent::FocusIn) {
        widget->setCursor(Qt::BlankCursor);             
        QCursor::setPos(widget->geometry().center());   
        widget->setMouseTracking(true);                 
        press = false;
    } else if(e->type()==QEvent::FocusOut) {
        widget->setCursor(Qt::ArrowCursor);   
        widget->setMouseTracking(false);      
    }

}

void Camera::init() {
    view.lookAt(cameraPos,cameraPos+cameraDirection,cameraUp);
    widget->activateWindow();                
    widget->setFocus();

}

QMatrix4x4 Camera::getView() const {
    return view;
}
