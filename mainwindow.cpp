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
#define DATA_SIZE    36   // Adjust the size based on your requirements
#include <string>
#include <sstream>
#include <iomanip>
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
    read_data();

}

QByteArray MainWindow::read_data()
{
    // Write the register address to the device
    uint8_t regAddr[1] = {REG_ADDRESS};
    if (write(i2cFileDescriptor, regAddr, sizeof(regAddr)) < 0) {
        qWarning() << "Failed to write register address to I2C device:" << strerror(errno);
        return QByteArray();
    }

    // Read data from the specified register
    char buffer[4096];  // Adjust buffer size based on expected data size
    ssize_t bytesRead = read(i2cFileDescriptor, buffer, sizeof(buffer));
    if (bytesRead < 0) {
        qWarning() << "Failed to read data from I2C device:" << strerror(errno);
        return QByteArray();
    }

    return QByteArray(buffer, bytesRead);
}


int MainWindow::write_data(const QByteArray &data)
{
    // Write the register address to the device
    uint8_t regAddr[1] = {REG_ADDRESS};
    if (write(i2cFileDescriptor, regAddr, sizeof(regAddr)) < 0) {
        qWarning() << "Failed to write register address to I2C device:" << strerror(errno);
        return -1;
    }

    // Write data to the specified register
    if (write(i2cFileDescriptor, data.constData(), data.size()) < 0) {
        qWarning() << "Failed to write data to I2C device:" << strerror(errno);
        return -1;
    }

    qDebug() << "Successfully wrote" << data.size() << "bytes to I2C device.";

    return 0;
}
void MainWindow::on_pushButton_2_clicked()
{
    QByteArray dataToWrite = "F2071245BD\nName\nmgg/l\n14.0 & 14.5\nT2";
    write_data(dataToWrite);
}

void MainWindow::erase_data()
{
    // Write the register address to the device
    uint8_t regAddr[1] = {REG_ADDRESS};
    if (write(i2cFileDescriptor, regAddr, sizeof(regAddr)) < 0) {
        qWarning() << "Failed to write register address to I2C device:" << strerror(errno);
        return;
    }

    // Write 0xFF to each memory location to erase data
    QByteArray eraseData(DATA_SIZE, 0xFF);
    if (write(i2cFileDescriptor, eraseData.constData(), eraseData.size()) < 0) {
        qWarning() << "Failed to erase data in I2C device:" << strerror(errno);
        return;
    }

    qDebug() << "Successfully erased all data in I2C device.";
}
void MainWindow::on_pushButton_3_clicked()
{
    // Example: Erase all data in the chip
    erase_data();
}

void MainWindow::on_pushButton_4_clicked()
{
    // Read existing data from the EEPROM
    QByteArray existingData = read_data();
    if (existingData.isEmpty()) {
        qWarning() << "Failed to read existing data from I2C device.";
        return;
    }
    QByteArray newData = existingData;
    QByteArray mergedData;
    QByteArray currentSection;

    for (int i = 0; i < newData.size(); i++) {
        if (newData[i] != '\0') {
            // Non-null byte, add it to the current section
            currentSection.append(newData[i]);
            qDebug()<<"data : "<<i<<" " <<newData[i];
        } else if (!currentSection.isEmpty()) {
            // Null byte, but the current section is not empty, so add it to mergedData
            mergedData += currentSection;
            currentSection.clear(); // Clear current section for the next one
        }
    }

    // If the last section is not empty, add it to mergedData
    if (!currentSection.isEmpty()) {
        mergedData += currentSection;
    }

    // Display the merged data
  //  qDebug() << "Merged Data:" << mergedData.toHex();
    std::string hexData = mergedData.toHex().toStdString();
    std::string asciiData = hexToAscii(hexData);
  //  qDebug() << "ASCII Data:" << QString::fromStdString(asciiData);

      // std::cout << "ASCII Data: " << &asciiData << std::endl;
}
/* QByteArray newData = existingData;
    QByteArray mergedData;
    QList<QByteArray> sections;
    //qDebug()<<"New Data : "<<newData;
    for (int i=0;i< newData.size();i++) {

        if (newData[i] != '\0' )//&& newData[i] == '\x00' && newData[i + 1] == '\x00') // Check if the byte is not empty
        {
            qDebug() << "Data " << "i" << i << "Byte : " << newData[i];
            sections = newData.split('\n');
        }
    }
    qDebug()<<"sec: "<<sections;
    for (int i = 0; i < sections.size(); i++)
    {
        if (!hasConsecutiveNullBytes(sections[i]))
        {
            mergedData += sections[i];
        }
    }

    // Display the merged data
    qDebug() << "Merged Data:" << mergedData.toHex();

}
*/
std::string MainWindow::hexToAscii(const std::string& hex) {
    std::string ascii;
    std::stringstream ss(hex);

    while (ss.good()) {
        std::string byte;
        ss >> std::setw(2) >> byte;

        if (!byte.empty()) {
            char chr = std::stoi(byte, nullptr, 16);
            ascii += chr;
        }
    }

    return ascii;
}

bool MainWindow::hasConsecutiveNullBytes(const QByteArray &data)
{
    // Check if the data contains consecutive null bytes
    for (int i = 0; i < data.size() - 1; i++)
    {
        if (data[i] == '\x00' && data[i + 1] == '\x00')
        {
            return true;
        }
    }
    return false;
}

void MainWindow::on_pushButton_5_clicked()
{
    QByteArray existingData = read_data();
    if (existingData.isEmpty()) {
        qWarning() << "Failed to read existing data from I2C device.";
        return;
    }
    // Specify the index to modify
      int indexToModify = 1371;  // Replace with the index you want to modify

      // Check if the index is within the valid range
      if (indexToModify >= 0 && indexToModify < existingData.size()) {
          // Retrieve the old data at the specified index
          char oldData = existingData.at(indexToModify);

          // Modify the data at the specified index
          char newValue = 'X';  // Replace with the new value
          existingData[indexToModify] = newValue;

          qDebug() << "Data at index " << indexToModify << " was: " << oldData;
          qDebug() << "Data at index " << indexToModify << " is now: " << newValue;
      } else {
          qWarning() << "Invalid index specified.";
      }
    // Specify the index to modify
     /*  int entryIndex = 1371;  // Replace with the entry index you want to modify
       int entrySize = 5;      // Replace with the size of each entry

       // Calculate the starting index of the entry
       int startIndex = entryIndex * entrySize;

       // Debug statements
       qDebug() << "startIndex:" << startIndex;
       qDebug() << "existingData size:" << existingData.size();

       // Check if the starting index is within the valid range
       if (startIndex >= 0 && startIndex + entrySize <= existingData.size()) {
           // Retrieve the old data of the entire entry
           QByteArray oldData = existingData.mid(startIndex, entrySize);

           // Modify the data of the entire entry
           QByteArray newValue = "NewX";  // Replace with the new value
           existingData.replace(startIndex, entrySize, newValue);

           qDebug() << "Data at entry " << entryIndex << " was: " << oldData;
           qDebug() << "Data at entry " << entryIndex << " is now: " << newValue;
       } else {
           qWarning() << "Invalid entry index specified.";
       }
    // Specify the index to modify

  /*  int entryIndex = 1371;  // Replace with the entry index you want to modify
        int entrySize = 5/* replace with the size of each entry ;

        // Calculate the starting index of the entry
        int startIndex = entryIndex * entrySize;

        // Check if the starting index is within the valid range
        if (startIndex >= 0 && startIndex + entrySize <= existingData.size()) {
            // Retrieve the old data of the entire entry
            QByteArray oldData = existingData.mid(startIndex, entrySize);

            // Modify the data of the entire entry
            QByteArray newValue = "NewX";  // Replace with the new value
            existingData.replace(startIndex, entrySize, newValue);

            qDebug() << "Data at entry " << entryIndex << " was: " << oldData;
            qDebug() << "Data at entry " << entryIndex << " is now: " << newValue;
        } else {
            qWarning() << "Invalid entry index specified.";
        } */
}

void MainWindow::on_pushButton_6_clicked()
{
    // Read existing data from the EEPROM
      QByteArray existingData = read_data();
      if (existingData.isEmpty()) {
          qWarning() << "Failed to read existing data from EEPROM.";
          return;
      }

      // Modify the desired portions of the data
      QList<QByteArray> sections = existingData.split('\n');

      for (int i = 0; i < sections.size(); i++)
      {
          QByteArray section = sections[i];
qDebug() << "Datasssss " << i;
          // Skip sections containing the specified character 'ÿ'
          if (section.contains('\xff')) {
              continue;

          }

          // Print the non-skipped sections
          qDebug() << "Data " << i + 1 << ":" << section;
      }
  }

void MainWindow::on_pushButton_7_clicked()
{
    // Read existing data from the EEPROM
      QByteArray existingData = read_data();
      if (existingData.isEmpty()) {
          qWarning() << "Failed to read existing data from EEPROM.";
          return;
      }

    // Find the index of a specific value
        char valueToFind = 'N';  // Replace with the value you want to find

        int indexOfValue = existingData.indexOf(valueToFind);
        if (indexOfValue != -1) {
            qDebug() << "Index of value " << valueToFind << " is " << indexOfValue;
        } else {
            qWarning() << "Value not found in the data.";
        }

        // Find all indices of a specific pattern
        QByteArray patternToFind = "ABCD";  // Replace with the pattern you want to find

        int currentIndex = 0;
        QList<int> indices;
        while ((currentIndex = existingData.indexOf(patternToFind, currentIndex)) != -1) {
            indices.append(currentIndex);
            currentIndex += patternToFind.size();
        }

        if (!indices.isEmpty()) {
            qDebug() << "Indices of pattern " << patternToFind << " are: " << indices;
        } else {
            qWarning() << "Pattern not found in the data.";
        }
}
