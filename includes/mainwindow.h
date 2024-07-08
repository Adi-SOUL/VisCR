#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gl_widget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Ui::MainWindow *ui;
    void show_w();
    void closeEvent(QCloseEvent* e) override;
public slots:
    //void set_ass();
    void clear_positions();
    void connect_c();
    void disc();
    void set_disk();
    void set_tendon();
    void set_section();
    void set_show_positions_1();
    void set_show_positions_2();
    void set_show_frame_1();
    void set_show_frame_2();
    void set_show_positions_frame();
    void set_step_of_show_position_frame();
    void set_show_base_frame();

    void set_light();
    void set_dark();

    void set_disk_c();
    void set_c();
    void set_t();

    void set_back_ground_color();
    void set_color_1();
    void set_color_2();
    // void set_position_of_base_frame();
    void set_line_width();
    void set_frame_width();
    // void set_color_1();
    // void set_color_2();

private:
    Widget w;
    Connecter c;
    QString state1;
};
#endif // MAINWINDOW_H
