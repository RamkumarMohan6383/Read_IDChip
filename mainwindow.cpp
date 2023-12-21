#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDebug>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>
#include <QThread>
#include <QStringList>
#include <QtEndian>
#define REG_ADDRESS 0x50  // Replace with the actual register address you want to read
extern QStringList Data;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Open the I2C device file for reading with POSIX open
    i2cFileDescriptor = open("/dev/i2c-1", O_RDWR);
    if (i2cFileDescriptor < 0) {
        qWarning() << "Failed to open I2C device file:" << strerror(errno);
        // Handle the error as needed
    } else {
        // Set the I2C device address with ioctl
        int i2cDeviceAddress = 0x50;  // Replace with your actual I2C device address
        if (ioctl(i2cFileDescriptor, I2C_SLAVE, i2cDeviceAddress) < 0) {
            qWarning() << "Failed to set I2C device address:" << strerror(errno);
            ::close(i2cFileDescriptor);  // Close file descriptor before returning
            // Handle the error as needed
        }
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    get_data();
}

int MainWindow::get_data()
{
    // Write the register address to the device
      uint8_t regAddr[1] = {REG_ADDRESS};
      if (write(i2cFileDescriptor, regAddr, sizeof(regAddr)) < 0) {
          qWarning() << "Failed to write register address to I2C device:" << strerror(errno);
          return -1;
      }

      // Read data from the specified register
      char buffer[2500];  // Adjust buffer size based on expected data size
      ssize_t bytesRead = read(i2cFileDescriptor, buffer, sizeof(buffer));
      if (bytesRead < 0) {
          qWarning() << "Failed to read data from I2C device:" << strerror(errno);
      } else {
          qDebug() << "Read" << bytesRead << "bytes from I2C device:" << QByteArray(buffer, bytesRead).toHex();
      }

    return 0;
}
