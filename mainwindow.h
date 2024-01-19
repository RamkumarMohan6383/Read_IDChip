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

    int write_data(const QByteArray &data);

    void erase_data();

    void on_pushButton_clicked();

    QByteArray read_data();

    std::string hexToAscii(const std::string& hex);

    bool hasConsecutiveNullBytes(const QByteArray &data);

  //  bool write_datas(const QByteArray &data);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
