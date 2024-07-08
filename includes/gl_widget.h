#ifndef MYWIDGET_H
#define MYWIDGET_H

#include "camera.h"
#include "cons.h"
#include "connect.h"
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QTimer>
#include <QTime>
#include <QtMath>
#include <QKeyEvent>
#include <functional>

class Widget : public QOpenGLWidget,public QOpenGLExtraFunctions
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();
    float a = 1.;
    void add_sim(QObject o);
    std::function<QVector<QMatrix4x4>()> get_t;
    Camera camera;
    void set_disk_num(int);
    void set_tendon_num(int);
    void set_section_num(int);

    void set_show_positions_1(bool);
    void set_show_positions_2(bool);
    void set_show_frame_1(bool);
    void set_show_frame_2(bool);
    void set_show_positions_frame(bool);
    void set_step_of_show_position_frame(int);
    void set_show_base_frame(bool);

    void set_position_of_base_frame(QVector3D);
    void set_line_width(float);
    void set_frame_width(float);
    void set_color_1(QVector4D);
    void set_color_2(QVector4D);
    void set_bg(QVector4D);


    void set_texture(QString);
    void set_texture_1(QString);
    void set_texture_2(QString);
    void set_texture_b(QString);
    void set_texture_w(QString);


    QString tb;
    QString tw;
    QString t;
    QString t1;
    QString t2;
    int section_num = 2;
    int disk_num = 5;
    int tendon_num = 3;

    float line_width_factor = 10.;
    float frame_width_factor = 10.;
    QVector4D background_color = QVector4D(.7f, .7f, .7f, 1.0f);
    QVector4D color_of_position_1 = QVector4D(4.f, 255.f, 255.f, 255.f);
    QVector4D color_of_position_2 = QVector4D(255.f, 255.f, 255.f, 255.f);
    bool show_positions_1 = true;
    bool show_positions_2 = true;

    bool show_base_frame = true;
    bool show_frame_1 = true;
    bool show_frame_2 = true;

    QVector3D position_of_base_frame = QVector3D(0, 0, 0);

    bool show_position_frame = true;
    int step_of_show_position_frame = 1;

    void clear_positions();

protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w,int h) override;
    virtual void paintGL() override;
    virtual bool event(QEvent *e) override;


private:
    void draw_floor();
    void draw_single_section();
    void draw_two_sections();
    void draw_three_sections();

    void draw_base_frame();

    QWidget son;
    QVector<float> vertices;
    QVector<float> vertices2;
    QVector<QVector3D> cubePositions;
    QVector<QVector3D> cubePositions2;
    QVector<QMatrix4x4> wtransforms;
    QVector<QMatrix4x4> dtransforms;

    QVector<QMatrix4x4> positions;
    QVector<QMatrix4x4> sub_positions_1;

    QOpenGLShaderProgram shaderProgram;
    QOpenGLBuffer VBO;
    QOpenGLVertexArrayObject VAO;

    QOpenGLTexture texture; //disk

    QOpenGLTexture texture1;//t

    QOpenGLTexture texture2;//c

    QOpenGLTexture texture_black;

    QOpenGLTexture texture1_white;


    // QOpenGLTexture texture_for_tendon;
    // QOpenGLTexture texture_for_disk;
    // QOpenGLTexture texture_for_backbone;
    bool start = false;




    QTimer timer;

    float ass = 1.;


    float r = 7.5f;
    float r_tendon = .25f;
    float r_center = .5f;

};

#endif // MYWIDGET_H
