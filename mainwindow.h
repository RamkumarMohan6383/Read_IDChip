#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    int i2cFileDescriptor;  // Declare i2cFileDescriptor as a member variable

private slots:
    void on_pushButton_clicked();

    int get_data();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
