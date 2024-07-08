#include "includes/mainwindow.h"
#include "./ui_mainwindow.h"
#include "includes/connect.h"
#include <QFileDialog>
#include <QSettings>
#include <QColorDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    QString iniFilePath = QApplication::applicationDirPath() + "\\Config.ini";
    QSettings *configIniFile = new QSettings(iniFilePath, QSettings::IniFormat);

    w.t = configIniFile->value("all/t").toString();
    w.t1 = configIniFile->value("all/t_1").toString();
    w.t2 = configIniFile->value("all/t_2").toString();
    w.tb = configIniFile->value("all/t_b").toString();
    w.tw = configIniFile->value("all/t_w").toString();

    w.set_disk_num(configIniFile->value("all/disk_num").toInt());
    ui->spinBox->setValue(configIniFile->value("all/disk_num").toInt());
    w.set_section_num(configIniFile->value("all/section_num").toInt());
    ui->spinBox_3->setValue(configIniFile->value("all/section_num").toInt());
    w.set_tendon_num(configIniFile->value("all/tendon_num").toInt());
    ui->spinBox_2->setValue(configIniFile->value("all/tendon_num").toInt());
    w.set_step_of_show_position_frame(configIniFile->value("all/step").toInt());
    ui->spinBox_step->setValue(configIniFile->value("all/step").toInt());

    w.set_show_base_frame(configIniFile->value("all/show_base_frame").toBool());
    ui->checkBox_show_base->setChecked(configIniFile->value("all/show_base_frame").toBool());
    w.set_show_positions_1(configIniFile->value("all/show_p1").toBool());
    ui->checkBox_p1->setChecked(configIniFile->value("all/show_p1").toBool());
    w.set_show_positions_2(configIniFile->value("all/show_p2").toBool());
    ui->checkBox_p2->setChecked(configIniFile->value("all/show_p2").toBool());
    w.set_show_positions_frame(configIniFile->value("all/show_frame_p1").toBool());
    ui->checkBox_show_p_f_1->setChecked(configIniFile->value("all/show_frame_p1").toBool());
    w.set_show_frame_1(configIniFile->value("all/show_frame_1").toBool());
    ui->checkBox_show_f_1->setChecked(configIniFile->value("all/show_frame_1").toBool());
    w.set_show_frame_2(configIniFile->value("all/show_frame_2").toBool());
    ui->checkBox_show_f_2->setChecked(configIniFile->value("all/show_frame_2").toBool());

    w.set_frame_width(configIniFile->value("all/frame_width").toFloat());
    ui->doubleSpinBox_3->setValue(configIniFile->value("all/frame_width").toFloat());
    w.set_line_width(configIniFile->value("all/line_width").toFloat());
    ui->doubleSpinBox->setValue(configIniFile->value("all/line_width").toFloat());

    w.set_bg(configIniFile->value("all/bg").value<QVector4D>());
    w.set_color_1(configIniFile->value("all/color_t1").value<QVector4D>());
    w.set_color_2(configIniFile->value("all/color_t2").value<QVector4D>());
    //qDebug() << iniFilePath;

    //connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(set_ass()));
    connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(set_disk()));
    //ui->spinBox->setValue(3);
    ui->spinBox->setMaximum(10);
    ui->spinBox->setMinimum(3);
    connect(ui->spinBox_2, SIGNAL(valueChanged(int)), this, SLOT(set_tendon()));
    //ui->spinBox_2->setValue(3);
    ui->spinBox_2->setMaximum(4);
    ui->spinBox_2->setMinimum(3);
    connect(ui->spinBox_3, SIGNAL(valueChanged(int)), this, SLOT(set_section()));
    //ui->spinBox_3->setValue(1);
    ui->spinBox_3->setMaximum(3);
    ui->spinBox_3->setMinimum(1);
    connect(ui->spinBox_step, SIGNAL(valueChanged(int)), this, SLOT(set_step_of_show_position_frame()));
    ui->spinBox_step->setMinimum(1);
    ui->spinBox_step->setMaximum(214748);

    connect(ui->doubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(set_line_width()));
    ui->doubleSpinBox->setMinimum(1.);
    connect(ui->doubleSpinBox_3, SIGNAL(valueChanged(double)), this, SLOT(set_frame_width()));
    ui->doubleSpinBox_3->setMinimum(1.);

    connect(ui->checkBox_p1, SIGNAL(stateChanged(int)), this, SLOT(set_show_positions_1()));
    connect(ui->checkBox_p2, SIGNAL(stateChanged(int)), this, SLOT(set_show_positions_2()));
    connect(ui->checkBox_show_f_1, SIGNAL(stateChanged(int)), this, SLOT(set_show_frame_1()));
    connect(ui->checkBox_show_f_2, SIGNAL(stateChanged(int)), this, SLOT(set_show_frame_2()));

    connect(ui->checkBox_show_p_f_1, SIGNAL(stateChanged(int)), this, SLOT(set_show_positions_frame()));
    connect(ui->checkBox_show_base, SIGNAL(stateChanged(int)), this, SLOT(set_show_base_frame()));


    connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(set_dark()));
    connect(ui->pushButton_2, SIGNAL(clicked(bool)), this, SLOT(set_light()));

    connect(ui->pushButton_4, SIGNAL(clicked(bool)), this, SLOT(set_disk_c()));
    connect(ui->pushButton_5, SIGNAL(clicked(bool)), this, SLOT(set_t()));
    connect(ui->pushButton_3, SIGNAL(clicked(bool)), this, SLOT(set_c()));

    connect(ui->pushButton_6, SIGNAL(clicked(bool)), this, SLOT(set_back_ground_color()));
    connect(ui->pushButton_7, SIGNAL(clicked(bool)), this, SLOT(set_color_1()));
    connect(ui->pushButton_8, SIGNAL(clicked(bool)), this, SLOT(set_color_2()));

    connect(ui->pushButton_9, SIGNAL(clicked(bool)), this, SLOT(connect_c()));
    connect(ui->pushButton_10, SIGNAL(clicked(bool)), this, SLOT(disc()));

    connect(ui->pushButton_11, SIGNAL(clicked(bool)), this, SLOT(clear_positions()));

    state1 += c.state1;
    ui->label ->setText(state1);

    this -> setGeometry(900, 50, 200, 800);
    this -> setWindowIcon(QIcon(":/logo.png"));
    this -> setWindowTitle("VisCR");


    //c.connect();
}
void MainWindow::connect_c() {
    c.connect();
    state1 += c.state1;
    ui->label ->setText(state1);
    if(c.connected) {
        ui->pushButton_9->setDisabled(true);
    }
}
void MainWindow::clear_positions() {
    w.clear_positions();
}
void MainWindow::disc() {
    c.disc();
    state1 = c.state1;
    ui->label ->setText(state1);
    ui->pushButton_9->setDisabled(false);
}
//void set_back_ground_color();
void MainWindow::set_back_ground_color() {
    auto color = QColorDialog::getColor();
    QVector4D c(color.redF(), color.greenF(), color.blueF(), color.alphaF());
    w.set_bg(c);
}
// void set_color_1();
void MainWindow::set_color_1() {
    auto color = QColorDialog::getColor();
    QVector4D c(color.redF(), color.greenF(), color.blueF(), color.alphaF());
    w.set_color_1(c);
}
// void set_color_2();
void MainWindow::set_color_2() {
    auto color = QColorDialog::getColor();
    QVector4D c(color.redF(), color.greenF(), color.blueF(), color.alphaF());
    w.set_color_2(c);
}
void MainWindow::set_dark() {
    auto fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
    if(fileName!=QString("")) w.set_texture_b(fileName);
}
void MainWindow::set_light() {
    auto fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
    if(fileName!=QString("")) w.set_texture_w(fileName);
}
void MainWindow::set_disk_c() {
    auto fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
    if(fileName!=QString("")) w.set_texture(fileName);
}
void MainWindow::set_t() {
    auto fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
    w.set_texture_1(fileName);
}
void MainWindow::set_c() {
    auto fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
    w.set_texture_2(fileName);
}
void MainWindow::show_w() {
    w.get_t = std::bind(&Connecter::sim, &c);
    w.setGeometry(50, 50, 800, 800);
    w.show();
}
void MainWindow::set_disk() {
    int disk = ui -> spinBox ->value();
    w.set_disk_num(disk);
}
void MainWindow::set_tendon() {
    int tendon = ui -> spinBox_2 ->value();
    w.set_tendon_num(tendon);
}
void MainWindow::set_section() {
    int section = ui -> spinBox_3 ->value();
    w.set_section_num(section);
}
void MainWindow::closeEvent(QCloseEvent* e) {
    QString iniFilePath = QApplication::applicationDirPath() + "\\Config.ini";
    QSettings *configIniFile = new QSettings(iniFilePath, QSettings::IniFormat);
    configIniFile -> setValue("/all/disk_num", w.disk_num);
    configIniFile -> setValue("/all/section_num", w.section_num);
    configIniFile -> setValue("/all/tendon_num", w.tendon_num);

    configIniFile -> setValue("/all/show_p1", w.show_positions_1);
    configIniFile -> setValue("/all/show_p2", w.show_positions_2);
    configIniFile -> setValue("/all/show_base_frame", w.show_base_frame);
    configIniFile -> setValue("/all/show_frame_1", w.show_frame_1);
    configIniFile -> setValue("/all/show_frame_2", w.show_frame_2);

    configIniFile -> setValue("/all/show_frame_p1", w.show_position_frame);

    configIniFile -> setValue("/all/step", w.step_of_show_position_frame);

    configIniFile -> setValue("/all/t", w.t);
    configIniFile -> setValue("/all/t_1", w.t1);
    configIniFile -> setValue("/all/t_2", w.t2);
    configIniFile -> setValue("/all/t_b", w.tb);
    configIniFile -> setValue("/all/t_w", w.tw);

    configIniFile -> setValue("/all/line_width", w.line_width_factor);
    configIniFile -> setValue("/all/frame_width", w.frame_width_factor);

    configIniFile -> setValue("/all/bg", w.background_color);
    configIniFile -> setValue("/all/color_t1", w.color_of_position_1);
    configIniFile -> setValue("/all/color_t2", w.color_of_position_2);
    w.close();

}
MainWindow::~MainWindow() {

    delete ui;
}
// void set_show_positions_1(bool);
void MainWindow::set_show_positions_1() {
    bool res = ui->checkBox_p1->isChecked();
    w.set_show_positions_1(res);
}
// void set_show_positions_2(bool);
void MainWindow::set_show_positions_2() {
    bool res = ui->checkBox_p2->isChecked();
    w.set_show_positions_2(res);
}
// void set_show_positions_frame(bool);
void MainWindow::set_show_positions_frame() {
    bool res = ui->checkBox_show_p_f_1->isChecked();
    w.set_show_positions_frame(res);
}
// void set_step_of_show_position_frame(int);
void MainWindow::set_step_of_show_position_frame() {
    int step = ui -> spinBox_step -> value();
    w.set_step_of_show_position_frame(step);
}
// void set_show_base_frame(bool);
void MainWindow::set_show_base_frame() {
    bool res = ui->checkBox_show_base -> isChecked();
    w.set_show_base_frame(res);
}
void MainWindow::set_show_frame_1() {
    bool res = ui->checkBox_show_f_1 -> isChecked();
    w.set_show_frame_1(res);
}
void MainWindow::set_show_frame_2() {
    bool res = ui->checkBox_show_f_2 -> isChecked();
    w.set_show_frame_2(res);
}
// void set_position_of_base_frame(QVector3D);
void MainWindow::set_line_width() {
    float f = ui->doubleSpinBox->value();
    w.set_line_width(f);
};
void MainWindow::set_frame_width() {
    float f = ui->doubleSpinBox_2->value();
    w.set_frame_width(f);
};
// void set_color_1(QVector4D);
// void set_color_2(QVector4D);

