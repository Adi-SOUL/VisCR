#include "includes/gl_widget.h"
#include <QtMath>
#include <QOpenGLFramebufferObject>
#define _CRT_SECURE_NO_WARNINGS
#define PI 3.14159265

QVector<float> get_with_norm() {
    QVector<float> res;

    for(int i=0; i < my_vertices.size(); i+=(3*5)) {
        QVector3D v1(my_vertices[i], my_vertices[i+1], my_vertices[i+2]);
        QVector3D v2(my_vertices[i+5], my_vertices[i+6], my_vertices[i+7]);
        QVector3D v3(my_vertices[i+10], my_vertices[i+11], my_vertices[i+12]);

        QVector3D edge1 = v2-v1;
        QVector3D edge2 = v3-v1;
        QVector3D norm = QVector3D::crossProduct(edge1, edge2).normalized();

        res << my_vertices[i]<< my_vertices[i+1]<< my_vertices[i+2]<< my_vertices[i+3]<< my_vertices[i+4]<< norm[0]<< norm[1]<< norm[2]<<
            my_vertices[i+5] << my_vertices[i+6]<< my_vertices[i+7]<< my_vertices[i+8]<< my_vertices[i+9]<<norm[0]<< norm[1] << norm[2]<<
            my_vertices[i+10]<< my_vertices[i+11]<< my_vertices[i+12]<< my_vertices[i+13]<< my_vertices[i+14]<<norm[0]<< norm[1]<< norm[2];

    }
    qDebug() << res.size();
    return res;
}


Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent)
    , camera(this)
    , VBO(QOpenGLBuffer::VertexBuffer)
    , texture(QOpenGLTexture::Target2D)
    , texture1(QOpenGLTexture::Target2D)
    , texture2(QOpenGLTexture::Target2D)
    , texture_black(QOpenGLTexture::Target2D)
    , texture1_white(QOpenGLTexture::Target2D) {

    vertices = get_with_norm();
    this -> setWindowIcon(QIcon(":/logo.png"));
    this -> setWindowTitle("VisCR");
    connect(&timer,&QTimer::timeout,this,static_cast<void (Widget::*)()>(&Widget::update));
    timer.start();

    float L = 200./40.;
    for(int i=0; i < 100; i++) {
        QMatrix4x4 m;
        m.translate(QVector3D(0, L/100.*(i), 0));
        dtransforms.append(m);
    }
    //c.connect;
}

Widget::~Widget() {
    makeCurrent();
    texture.destroy();
    texture1.destroy();
    texture1_white.destroy();
    texture_black.destroy();
    doneCurrent();
}

void Widget::initializeGL() {
    this->initializeOpenGLFunctions();
    if(!shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,":/shader/triangle.vert")) {    //添加并编译顶点着色器
        qDebug()<<"ERROR:"<<shaderProgram.log();    
    }
    if(!shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,":/shader/triangle.frag")) {  //添加并编译片段着色器
        qDebug()<<"ERROR:"<<shaderProgram.log();    
    }
    if(!shaderProgram.link()) {                     
        qDebug()<<"ERROR:"<<shaderProgram.log();    
    }

    QOpenGLVertexArrayObject::Binder{&VAO};

    VBO.create();       
    VBO.bind();         
    VBO.allocate(vertices.data(),int(sizeof(float)*vertices.size()));

    texture.create();
    texture.setData(QImage(t).mirrored());
    texture.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear,QOpenGLTexture::Linear);
    texture.setWrapMode(QOpenGLTexture::DirectionS,QOpenGLTexture::Repeat);
    texture.setWrapMode(QOpenGLTexture::DirectionT,QOpenGLTexture::Repeat);

    texture1.create();
    texture1.setData(QImage(t1).mirrored());
    texture1.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear,QOpenGLTexture::Linear);
    texture1.setWrapMode(QOpenGLTexture::DirectionS,QOpenGLTexture::Repeat);
    texture1.setWrapMode(QOpenGLTexture::DirectionT,QOpenGLTexture::Repeat);

    texture2.create();
    texture2.setData(QImage(t2).mirrored());
    texture2.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear,QOpenGLTexture::Linear);
    texture2.setWrapMode(QOpenGLTexture::DirectionS,QOpenGLTexture::Repeat);
    texture2.setWrapMode(QOpenGLTexture::DirectionT,QOpenGLTexture::Repeat);

    texture_black.create();
    texture_black.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear,QOpenGLTexture::Linear);
    texture_black.setWrapMode(QOpenGLTexture::DirectionS,QOpenGLTexture::Repeat);
    texture_black.setWrapMode(QOpenGLTexture::DirectionT,QOpenGLTexture::Repeat);
    texture_black.setData(QImage(tb).mirrored());

    texture1_white.create();
    texture1_white.setData(QImage(tw).mirrored());
    texture1_white.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear,QOpenGLTexture::Linear);
    texture1_white.setWrapMode(QOpenGLTexture::DirectionS,QOpenGLTexture::Repeat);
    texture1_white.setWrapMode(QOpenGLTexture::DirectionT,QOpenGLTexture::Repeat);

    //设置顶点解析格式，并启用顶点
    shaderProgram.setAttributeBuffer("aPos", GL_FLOAT, 0, 3, sizeof(GLfloat) * 8);
    shaderProgram.enableAttributeArray("aPos");
    shaderProgram.setAttributeBuffer("aTexCoord", GL_FLOAT,sizeof(GLfloat) * 3, 2, sizeof(GLfloat) * 8);
    shaderProgram.enableAttributeArray("aTexCoord");
    shaderProgram.setAttributeBuffer("aNorm", GL_FLOAT, sizeof(GLfloat) * 5, 3, sizeof(GLfloat) * 8);
    shaderProgram.enableAttributeArray("aNorm");


    this->glEnable(GL_DEPTH_TEST);
    QSurfaceFormat format;
    format.setSamples(40);
    setFormat(format);
    glEnable(GL_MULTISAMPLE);
    camera.init();

}

void Widget::resizeGL(int w, int h) {
    this->glViewport(0,0,w,h);                  
}

void Widget::paintGL() {
    this->glClearColor(background_color[0], background_color[1], background_color[2], background_color[3]);                       //设置清屏颜色
    this->glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);       

    shaderProgram.bind();                                          
    shaderProgram.setUniformValue("view",camera.getView());
    QMatrix4x4 projection;
    projection.perspective(45.0f,width()/(float)height(),0.1f,100.0f);
    shaderProgram.setUniformValue("projection",projection);

    QOpenGLVertexArrayObject::Binder{&VAO};


    wtransforms = get_t();
    if(wtransforms.size()==0) {
        start = false;
        wtransforms = dtransforms;
    } else {
        start = true;
    }

    if(show_base_frame) {
        draw_base_frame();
    }

    if(section_num==1) {
        draw_single_section();
    } else {
        if(section_num==2) {
            draw_two_sections();
        }
    }

    draw_floor();

}


void Widget::clear_positions() {
    positions.clear();
    sub_positions_1.clear();
}

void Widget::set_bg(QVector4D c) {
    background_color = c;
}
void Widget::set_disk_num(int num) {
    if(num>2) {
        if(num<10) {
            disk_num = num;
        }
    }
}

void Widget::set_tendon_num(int num) {
    if(num==3) tendon_num = 3;
    if(num==4) tendon_num = 4;
}

void Widget::set_section_num(int num) {
    if(num==1) section_num = 1;
    if(num==2) section_num = 2;
    //if(num==3) section_num = 3;
}

// void set_texture(QString);
void Widget::set_texture(QString s) {
    texture.destroy();
    texture.setData(QImage(s).mirrored());
    texture.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear,QOpenGLTexture::Linear);
    texture.setWrapMode(QOpenGLTexture::DirectionS,QOpenGLTexture::Repeat);
    texture.setWrapMode(QOpenGLTexture::DirectionT,QOpenGLTexture::Repeat);
    t=s;
}

// void set_texture_1(QString);
void Widget::set_texture_1(QString s) {
    texture1.destroy();
    texture1.setData(QImage(s).mirrored());
    texture1.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear,QOpenGLTexture::Linear);
    texture1.setWrapMode(QOpenGLTexture::DirectionS,QOpenGLTexture::Repeat);
    texture1.setWrapMode(QOpenGLTexture::DirectionT,QOpenGLTexture::Repeat);
    t1=s;
}
// void set_texture_2(QString);

void Widget::set_texture_2(QString s) {
    texture2.destroy();
    texture2.setData(QImage(s).mirrored());
    texture2.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear,QOpenGLTexture::Linear);
    texture2.setWrapMode(QOpenGLTexture::DirectionS,QOpenGLTexture::Repeat);
    texture2.setWrapMode(QOpenGLTexture::DirectionT,QOpenGLTexture::Repeat);
    t2=s;
}

// void set_texture_b(QString);
void Widget::set_texture_b(QString s) {
    texture_black.destroy();
    texture_black.setData(QImage(s).mirrored());
    texture_black.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear,QOpenGLTexture::Linear);
    texture_black.setWrapMode(QOpenGLTexture::DirectionS,QOpenGLTexture::Repeat);
    texture_black.setWrapMode(QOpenGLTexture::DirectionT,QOpenGLTexture::Repeat);
    tb=s;
}
// void set_texture_w(QString);
void Widget::set_texture_w(QString s) {
    texture1_white.destroy();
    texture1_white.create();
    texture1_white.setData(QImage(s).mirrored());
    texture1_white.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear,QOpenGLTexture::Linear);
    texture1_white.setWrapMode(QOpenGLTexture::DirectionS,QOpenGLTexture::Repeat);
    texture1_white.setWrapMode(QOpenGLTexture::DirectionT,QOpenGLTexture::Repeat);
    tw=s;
}


void Widget::draw_single_section() {
    shaderProgram.setUniformValue("s", 1);
    size_t num = wtransforms.size();
    size_t step = (num-disk_num)/(disk_num-1)+1;
    size_t now = 1;

    texture.bind(0);
    shaderProgram.setUniformValue("ourTexture",0);

    QMatrix4x4 model;
    QMatrix4x4 scale;
    scale.scale(QVector3D((10/200.f)*r, (10/200.f), (10/200.f)*r));
    QMatrix4x4 T;
    T.translate(QVector3D(0.f, -1.f/2+1/20.f, 0.f));
    model = T*wtransforms[0];
    shaderProgram.setUniformValue("model",model);
    shaderProgram.setUniformValue("scale",scale);
    this->glDrawArrays(GL_TRIANGLES, 42, 756);
    QVector<QVector3D> T_ts;
    if(tendon_num==4) {
        T_ts = {
            QVector3D((10/200.f)*r*.9, 0, 0),
            QVector3D(0, 0, (10/200.f)*r*.9),
            QVector3D(-(10/200.f)*r*.9, 0, 0),
            QVector3D(0, 0, -(10/200.f)*r*.9)
        };
    } else {
        if(tendon_num==3) {
            T_ts = {
                QVector3D((10/200.)*r*.9, 0, 0),
                QVector3D(-(10/200.)*r*.9*.5, 0, (10/200.)*r*.9*sqrt(3.)/2.),
                QVector3D(-(10/200.)*r*.9*.5, 0, -(10/200.)*r*.9*sqrt(3.)/2.)
            };
        }
    }

    for(unsigned int i = 1; i < num-1; i++) {
        if(now == step+1) {
            texture.bind(0);
            shaderProgram.setUniformValue("ourTexture",0);
            now = 0;
            QMatrix4x4 model_disk;
            model_disk = T*wtransforms[i];
            shaderProgram.setUniformValue("model",model_disk);
            shaderProgram.setUniformValue("scale",scale);
            this->glDrawArrays(GL_TRIANGLES, 42, 756);
        } else {
            texture2.bind(2);
            shaderProgram.setUniformValue("ourTexture",2);
            QMatrix4x4 model_c;
            QMatrix4x4 scale_c;
            scale_c.scale(QVector3D((10/200.f)*r_center, (10/100.f), (10/200.f)*r_center));
            model_c = T*wtransforms[i];
            shaderProgram.setUniformValue("model",model_c);
            shaderProgram.setUniformValue("scale",scale_c);
            this->glDrawArrays(GL_TRIANGLES, 42, 756);

            for(int j = 0; j<tendon_num; j++) {
                texture1.bind(1);
                shaderProgram.setUniformValue("ourTexture",1);
                QMatrix4x4 model_t;
                QMatrix4x4 scale_t;
                scale_t.scale(QVector3D((10/200.f)*r_tendon, (10/100.f), (10/200.f)*r_tendon));
                QMatrix4x4 T_t;
                T_t.translate(T_ts[j]);
                model_t = T*wtransforms[i]*T_t;
                shaderProgram.setUniformValue("model",model_t);
                shaderProgram.setUniformValue("scale",scale_t);
                this->glDrawArrays(GL_TRIANGLES, 42, 756);
            }
        }
        now += 1;
    }

    texture.bind(0);
    shaderProgram.setUniformValue("ourTexture",0);
    QMatrix4x4 model_end;
    model_end = T*wtransforms[num-1];
    shaderProgram.setUniformValue("model",model_end);
    shaderProgram.setUniformValue("scale",scale);
    this->glDrawArrays(GL_TRIANGLES, 42, 756);


    shaderProgram.setUniformValue("s", 0);
    if (show_frame_1) {
        glLineWidth(100.*frame_width_factor);
        QMatrix4x4 scale;
        scale.scale(.5);
        shaderProgram.setUniformValue("scale",scale);
        shaderProgram.setUniformValue("color", QVector4D(255., 0., 0., 255.));
        this->glDrawArrays(GL_LINE_STRIP, 798, 2);
        shaderProgram.setUniformValue("color", QVector4D(0., 255., 0., 255.));
        this->glDrawArrays(GL_LINE_STRIP, 800, 2);
        shaderProgram.setUniformValue("color", QVector4D(0., 0., 255., 255.));
        this->glDrawArrays(GL_LINE_STRIP, 802, 2);
    }

    if(start) {
        QMatrix4x4 pos;
        pos = model_end;
        positions.append(pos);
        vertices = get_with_norm();
        for(auto s:positions) {
            vertices << s(0, 3) << s(1, 3) << s(2, 3) << 1.f << 1.f << 0.f << 1.f << 0.f;
        }
        VBO.bind();
        int _ = sizeof(float)*vertices.size();
        VBO.allocate(vertices.data(), _);
    }


    if (show_position_frame&&show_positions_1) {
        for(int i = 0; i < positions.size(); i+=step_of_show_position_frame) {
            glLineWidth(.1*frame_width_factor);
            QMatrix4x4 scale;
            scale.scale(.5);
            shaderProgram.setUniformValue("scale",scale);
            shaderProgram.setUniformValue("model",positions[i]);
            shaderProgram.setUniformValue("color", QVector4D(255., 0., 0., 255.));
            this->glDrawArrays(GL_LINE_STRIP, 798, 2);
            shaderProgram.setUniformValue("color", QVector4D(0., 255., 0., 255.));
            this->glDrawArrays(GL_LINE_STRIP, 800, 2);
            shaderProgram.setUniformValue("color", QVector4D(0., 0., 255., 255.));
            this->glDrawArrays(GL_LINE_STRIP, 802, 2);
        }
    }

    if (show_positions_1) {
        QMatrix4x4 I;
        I.scale(1.);
        shaderProgram.setUniformValue("model",I);
        shaderProgram.setUniformValue("scale",I);
        glLineWidth(.1*line_width_factor);
        //qDebug() << line_width_factor;
        shaderProgram.setUniformValue("color", color_of_position_1);
        // 804
        GLsizei size_of_positions = positions.size();
        this->glDrawArrays(GL_LINE_STRIP, 804, size_of_positions);
    }

}

void Widget::draw_base_frame() {
    shaderProgram.setUniformValue("s", 0);
    glLineWidth(100.*frame_width_factor);
    QMatrix4x4 scale;
    scale.scale(.5);
    QMatrix4x4 I;
    I.scale(1.);
    QMatrix4x4 T;
    T.translate(QVector3D(0, -1.f/2+1/20.f, 0));
    QMatrix4x4 T2;
    T2.translate(position_of_base_frame);
    shaderProgram.setUniformValue("model",T*T2*I);
    shaderProgram.setUniformValue("scale",scale);
    shaderProgram.setUniformValue("color", QVector4D(255.f, 0., 0., 255.f));
    this->glDrawArrays(GL_LINE_STRIP, 798, 2);
    shaderProgram.setUniformValue("color", QVector4D(0., 255.f, 0., 255.f));
    this->glDrawArrays(GL_LINE_STRIP, 800, 2);
    shaderProgram.setUniformValue("color", QVector4D(0., 0., 255.f, 255.f));
    this->glDrawArrays(GL_LINE_STRIP, 802, 2);
}
// void set_show_positions_1(bool);
void Widget::set_show_positions_1(bool show) {
    show_positions_1 = show;
}
// void set_show_positions_2(bool);
void Widget::set_show_positions_2(bool show) {
    show_positions_2 = show;
}
// void set_show_positions_frame(bool);
void Widget::set_show_positions_frame(bool show) {
    show_position_frame = show;
}
// void set_step_of_show_position_frame(int);
void Widget::set_step_of_show_position_frame(int step) {
    step_of_show_position_frame = step;
}
// void set_show_base_frame(bool);
void Widget::set_show_base_frame(bool show) {
    show_base_frame = show;
}
// void set_position_of_base_frame(QVector3D);
void Widget::set_position_of_base_frame(QVector3D position) {
    position_of_base_frame = position;
}
// void set_line_width(float);
void Widget::set_line_width(float width) {
    line_width_factor = width;
}
// void set_frame_width(float);
void Widget::set_frame_width(float width) {
    frame_width_factor = width;
}
// void set_color_1(QVector4D);
void Widget::set_color_1(QVector4D color) {
    color_of_position_1 = color;
}
// void set_color_2(QVector4D);
void Widget::set_color_2(QVector4D color) {
    color_of_position_2 = color;
}
void Widget::set_show_frame_1(bool res) {
    show_frame_1 = res;
}
void Widget::set_show_frame_2(bool res) {
    show_frame_2 = res;
}

void Widget::draw_two_sections() {
    shaderProgram.setUniformValue("s", 1);
    size_t num = wtransforms.size();
    size_t step = (num-disk_num*2+1)/(disk_num*2-2)+1;
    size_t now = 1;
    QMatrix4x4 model_section_1;

    texture.bind(0);
    shaderProgram.setUniformValue("ourTexture",0);
    QMatrix4x4 model;
    QMatrix4x4 scale;
    scale.scale(QVector3D((10/200.f)*r, (10/200.f), (10/200.f)*r));
    QMatrix4x4 T;
    T.translate(QVector3D(0, -1.f/2+1/20.f, 0));
    model = T*wtransforms[0];
    shaderProgram.setUniformValue("model",model);
    shaderProgram.setUniformValue("scale",scale);
    this->glDrawArrays(GL_TRIANGLES, 42, 756);

    QMatrix4x4 rot90;

    QVector<QVector3D> T_ts;
    if(tendon_num==4) {
        T_ts = {
            QVector3D((10/200.)*r*.9, 0, 0),
            QVector3D(0, 0, (10/200.)*r*.9),
            QVector3D(-(10/200.)*r*.9, 0, 0),
            QVector3D(0, 0, -(10/200.)*r*.9)
        };
        rot90.rotate(45, QVector3D(0, 1, 0));
    } else {
        if(tendon_num==3) {
            T_ts = {
                QVector3D((10/200.)*r*.9, 0, 0),
                QVector3D(-(10/200.)*r*.9*.5, 0, (10/200.f)*r*.9*sqrt(3.)/2.),
                QVector3D(-(10/200.)*r*.9*.5, 0, -(10/200.f)*r*.9*sqrt(3.)/2.)
            };
        }
        rot90.rotate(60, QVector3D(0, 1, 0));
    }

    int d = 1;
    for(unsigned int i = 1; i < num-1; i++) {
        if(now == step+1) {
            now = 0;
            texture.bind(0);
            shaderProgram.setUniformValue("ourTexture",0);
            QMatrix4x4 model_disk;
            model_disk = T*wtransforms[i];
            shaderProgram.setUniformValue("model",model_disk);
            shaderProgram.setUniformValue("scale",scale);
            this->glDrawArrays(GL_TRIANGLES, 42, 756);
            d+=1;
            if(d == disk_num) {
                model_section_1 = model_disk;
            }
        } else {
            texture2.bind(2);
            shaderProgram.setUniformValue("ourTexture",2);
            QMatrix4x4 model_c;
            QMatrix4x4 scale_c;
            scale_c.scale(QVector3D((10/200.f)*r_center, (10/100.f), (10/200.f)*r_center));
            model_c = T*wtransforms[i];
            shaderProgram.setUniformValue("model",model_c);
            shaderProgram.setUniformValue("scale",scale_c);
            this->glDrawArrays(GL_TRIANGLES, 42, 756);

            if(d<disk_num) {
                for(int j = 0; j<tendon_num; j++) {
                    texture1.bind(1);
                    shaderProgram.setUniformValue("ourTexture",1);
                    QMatrix4x4 model_t;
                    QMatrix4x4 scale_t;
                    scale_t.scale(QVector3D((10/200.f)*r_tendon, (10/100.f), (10/200.f)*r_tendon));
                    QMatrix4x4 T_t;
                    T_t.translate(T_ts[j]);
                    model_t = T*wtransforms[i]*rot90*T_t;
                    shaderProgram.setUniformValue("model",model_t);
                    shaderProgram.setUniformValue("scale",scale_t);
                    this->glDrawArrays(GL_TRIANGLES, 42, 756);
                }
            }
            for(int j = 0; j<tendon_num; j++) {
                texture1.bind(1);
                shaderProgram.setUniformValue("ourTexture",1);
                QMatrix4x4 model_t;
                QMatrix4x4 scale_t;
                scale_t.scale(QVector3D((10/200.f)*r_tendon, (10/100.f), (10/200.f)*r_tendon));
                QMatrix4x4 T_t;
                T_t.translate(T_ts[j]);
                model_t = T*wtransforms[i]*T_t;
                shaderProgram.setUniformValue("model",model_t);
                shaderProgram.setUniformValue("scale",scale_t);
                this->glDrawArrays(GL_TRIANGLES, 42, 756);
            }
        }
        now += 1;
    }

    texture.bind(0);
    shaderProgram.setUniformValue("ourTexture",0);
    QMatrix4x4 model_end;
    model_end = T*wtransforms[num-1];
    shaderProgram.setUniformValue("model",model_end);
    shaderProgram.setUniformValue("scale",scale);
    this->glDrawArrays(GL_TRIANGLES, 42, 756);


    shaderProgram.setUniformValue("s", 0);
    if (show_frame_2) {
        glLineWidth(100.*frame_width_factor);
        QMatrix4x4 scale;
        scale.scale(.5);
        shaderProgram.setUniformValue("scale",scale);
        shaderProgram.setUniformValue("color", QVector4D(255., 0., 0., 255.));
        this->glDrawArrays(GL_LINE_STRIP, 798, 2);
        shaderProgram.setUniformValue("color", QVector4D(0., 255., 0., 255.));
        this->glDrawArrays(GL_LINE_STRIP, 800, 2);
        shaderProgram.setUniformValue("color", QVector4D(0., 0., 255., 255.));
        this->glDrawArrays(GL_LINE_STRIP, 802, 2);
    }

    if (show_frame_1) {
        glLineWidth(100.*frame_width_factor);
        QMatrix4x4 scale;
        scale.scale(.5);
        shaderProgram.setUniformValue("model",model_section_1);
        shaderProgram.setUniformValue("scale",scale);
        shaderProgram.setUniformValue("color", QVector4D(255., 0., 0., 255.));
        this->glDrawArrays(GL_LINE_STRIP, 798, 2);
        shaderProgram.setUniformValue("color", QVector4D(0., 255., 0., 255.));
        this->glDrawArrays(GL_LINE_STRIP, 800, 2);
        shaderProgram.setUniformValue("color", QVector4D(0., 0., 255., 255.));
        this->glDrawArrays(GL_LINE_STRIP, 802, 2);
    }

    if(start) {
        QMatrix4x4 pos;
        pos = model_end;
        positions.append(pos);
        vertices = my_vertices;
        for(auto s:positions) {
            vertices << s(0, 3) << s(1, 3) << s(2, 3) << 1.f << 1.f << 0.f << 1.f << 0.f;
        }

        sub_positions_1.append(model_section_1);
        for(auto s:sub_positions_1) {
            vertices << s(0, 3) << s(1, 3) << s(2, 3) << 1.f << 1.f << 0.f << 1.f << 0.f;
        }
        VBO.bind();
        int _ = sizeof(float)*vertices.size();
        VBO.allocate(vertices.data(), _);
    }


    if (show_position_frame&&show_positions_2) {
        for(int i = 0; i < positions.size(); i+=step_of_show_position_frame) {
            glLineWidth(.1*frame_width_factor);
            QMatrix4x4 scale;
            scale.scale(.5);
            shaderProgram.setUniformValue("scale",scale);
            shaderProgram.setUniformValue("model",positions[i]);
            shaderProgram.setUniformValue("color", QVector4D(255., 0., 0., 255.));
            this->glDrawArrays(GL_LINE_STRIP, 798, 2);
            shaderProgram.setUniformValue("color", QVector4D(0., 255., 0., 255.));
            this->glDrawArrays(GL_LINE_STRIP, 800, 2);
            shaderProgram.setUniformValue("color", QVector4D(0., 0., 255., 255.));
            this->glDrawArrays(GL_LINE_STRIP, 802, 2);
        }
    }

    if (show_position_frame&&show_positions_1) {
        for(int i = 0; i < positions.size(); i+=step_of_show_position_frame) {
            glLineWidth(.1*frame_width_factor);
            QMatrix4x4 scale;
            scale.scale(.5);
            shaderProgram.setUniformValue("scale",scale);
            shaderProgram.setUniformValue("model",sub_positions_1[i]);
            shaderProgram.setUniformValue("color", QVector4D(255., 0., 0., 255.));
            this->glDrawArrays(GL_LINE_STRIP, 798, 2);
            shaderProgram.setUniformValue("color", QVector4D(0., 255., 0., 255.));
            this->glDrawArrays(GL_LINE_STRIP, 800, 2);
            shaderProgram.setUniformValue("color", QVector4D(0., 0., 255., 255.));
            this->glDrawArrays(GL_LINE_STRIP, 802, 2);
        }
    }

    GLint size_of_positions_2 = positions.size();
    GLint size_of_positions_1 = sub_positions_1.size();
    if (show_positions_2) {
        QMatrix4x4 I;
        I.scale(1.);
        shaderProgram.setUniformValue("model",I);
        shaderProgram.setUniformValue("scale",I);
        glLineWidth(.1*line_width_factor);
        shaderProgram.setUniformValue("color", color_of_position_2);
        // 804
        this->glDrawArrays(GL_LINE_STRIP, 804, size_of_positions_2);
    }

    if (show_positions_1) {
        QMatrix4x4 I;
        I.scale(1.);
        shaderProgram.setUniformValue("model",I);
        shaderProgram.setUniformValue("scale",I);
        glLineWidth(.1*line_width_factor);
        //qDebug() << line_width_factor;
        shaderProgram.setUniformValue("color", color_of_position_1);
        // 804
        this->glDrawArrays(GL_LINE_STRIP, 804+size_of_positions_2, size_of_positions_1);
    }


}

void Widget::draw_floor() {
    shaderProgram.setUniformValue("s", 1);
    int all = 100;
    texture_black.bind(2);
    shaderProgram.setUniformValue("ourTexture", 2);
    //shaderProgram.setUniformValue("s",0);
    QMatrix4x4 scale;
    scale.scale(1.);
    shaderProgram.setUniformValue("scale",scale);

    for(int i = 0; i<all; i+=2) {
        float x = all/2. - i;
        for(int j =0; j<all; j+=2) {
            float z = all/2. - j;
            QMatrix4x4 model;
            //model.translate(cubePositions[i]);
            model.translate(QVector3D(x,0,z));
            //model.rotate(180*time+i*20.0f,QVector3D(1.0f,0.5f,0.3f));
            shaderProgram.setUniformValue("model",model);
            this->glDrawArrays(GL_TRIANGLES, 24, 6);
        }
    }

    texture1_white.bind(3);
    shaderProgram.setUniformValue("ourTexture", 3);
    //shaderProgram.setUniformValue("s",0);
    QMatrix4x4 scale_2;
    scale_2.scale(1.);
    shaderProgram.setUniformValue("scale",scale_2);
    for(int i = 0; i<all; i+=2) {
        float x = all/2. - i;
        for(int j =1; j<all; j+=2) {
            float z = all/2. - j;
            QMatrix4x4 model;
            //model.translate(cubePositions[i]);
            model.translate(QVector3D(x,0,z));
            //model.rotate(180*time+i*20.0f,QVector3D(1.0f,0.5f,0.3f));
            shaderProgram.setUniformValue("model",model);
            this->glDrawArrays(GL_TRIANGLES, 24, 6);
        }
    }
    texture_black.bind(2);
    shaderProgram.setUniformValue("ourTexture", 2);
    //shaderProgram.setUniformValue("s",0);
    QMatrix4x4 scale_4;
    scale_4.scale(1.);
    shaderProgram.setUniformValue("scale",scale_4);

    for(int i = 1; i<all; i+=2) {
        float x = all/2. - i;
        for(int j =1; j<all; j+=2) {
            float z = all/2. - j;
            QMatrix4x4 model;
            //model.translate(cubePositions[i]);
            model.translate(QVector3D(x,0,z));
            //model.rotate(180*time+i*20.0f,QVector3D(1.0f,0.5f,0.3f));
            shaderProgram.setUniformValue("model",model);
            this->glDrawArrays(GL_TRIANGLES, 24, 6);
        }
    }

    texture1_white.bind(3);
    shaderProgram.setUniformValue("ourTexture", 3);
    //shaderProgram.setUniformValue("s",0);
    QMatrix4x4 scale_3;
    scale_3.scale(1.);
    shaderProgram.setUniformValue("scale",scale_3);
    for(int i = 1; i<all; i+=2) {
        float x = all/2. - i;
        for(int j =0; j<all; j+=2) {
            float z = all/2. - j;
            QMatrix4x4 model;
            //model.translate(cubePositions[i]);
            model.translate(QVector3D(x,0,z));
            //model.rotate(180*time+i*20.0f,QVector3D(1.0f,0.5f,0.3f));
            shaderProgram.setUniformValue("model",model);
            this->glDrawArrays(GL_TRIANGLES, 24, 6);
        }
    }
}

bool Widget::event(QEvent *e) {
    camera.handle(e);
    if(e->type()==QEvent::KeyPress) {
        QKeyEvent *event=static_cast<QKeyEvent*>(e);
        if(event->key()==Qt::Key_Escape) {
            //doing nothing is better
        }
    }
    return QWidget::event(e);   //调用父类的事件分发函数
}
