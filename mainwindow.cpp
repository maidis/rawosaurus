#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QImageWriter>
#include <QByteArray>
#include <QBitArray>

#include <QFileDialog>

#include <bitset>
#include <vector>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->spinBoxPlane->setValue(4);
    ui->spinBoxPlane->setEnabled(false);

    ui->btnOpenRAWImage->setEnabled(false);
    ui->btnSaveImage->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::Converter(QByteArray line)
{
    QBitArray bits(line.count()*8);
    // Convert from QByteArray to QBitArray
    for(int i=0; i<line.count(); ++i)
    {
        for(int b=0; b<8;b++)
        {
            bits.setBit( i*8+b, line.at(i)&(1<<(7-b)));
        }
    }

    // https://stackoverflow.com/questions/46101782/qt-how-do-i-convert-qbitarray-to-qstring
    QString arrStr = "";
    for(int i = 0; i<bits.size(); i++)
    {
        // https://stackoverflow.com/questions/3211771/how-to-convert-int-to-qstring
        arrStr += QString::number(bits.at(i));
    }
    return arrStr;
}

void MainWindow::ClearImage()
{
    plane1.clear();
    plane2.clear();
    plane3.clear();
    plane4.clear();
    dortlu.clear();
}

QByteArray MainWindow::OpenImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open RAW file"), "", tr("RAW Files (*.*)"));

    file = new QFile(fileName);
    QFileInfo fileinfo(fileName);

    // https://forum.qt.io/topic/79053/how-can-i-read-large-files-to-qbytearray
    if(!file->open(QIODevice::ReadOnly))
    {
        return "";
    }

    int fileSize = fileinfo.size() * 2; // TODO: why 2?
    qInfo() << fileSize << '\n';

    for(int i = 1; i <= fileSize; ++i)
        {
            if(fileSize % i == 0)
            {
                vect.push_back(std::make_pair(i, fileSize / i));
            }

        }

        for (auto i = vect.begin(); i != vect.end(); ++i)
        {
            ui->comboBoxResolution->addItem(QString::number(std::get<0>(*i)) + " x " + QString::number(std::get<1>(*i)) + " = " + QString::number(fileSize));
            qInfo() << std::get<0>(*i) << " x " << std::get<1>(*i) << " = " << fileSize << endl;
        }

    QByteArray line = file->readAll();
    return line;
}

void MainWindow::DrawImage()
{
    bool ok;

    int i = 0;
    while(i < imgX * imgY * ui->spinBoxPlane->value())
    {
        //QString str1 = hadi.mid(i, 1);
        //QString str2 = hadi.mid(32 * 34 + i, 1);
        //QString str3 = hadi.mid(32 * 34 + 32 * 34 + i, 1);
        //QString str4 = hadi.mid(32 * 34 + 32 * 34 + 32 * 34 +i, 1);
        QString str1 = hadi.mid(i, 1);
        QString str2 = hadi.mid(imgX * imgY + i, 1);
        QString str3 = hadi.mid(imgX * imgY + imgX * imgY + i, 1);
        QString str4 = hadi.mid(imgX * imgY + imgX * imgY + imgX * imgY +i, 1);

        //QString string = str1 + str2 + str3 + str4;
        QString string = str4 + str3 + str2 + str1;
        int ival = string.toInt(&ok, 2);

        int iVal1 = str1.toInt(&ok,2);
        int iVal2 = str2.toInt(&ok,2);
        int iVal3 = str3.toInt(&ok,2);
        int iVal4 = str4.toInt(&ok,2);

        dortlu.push_back(ival);
        plane1.push_back(iVal1);
        plane2.push_back(iVal2);
        plane3.push_back(iVal3);
        plane4.push_back(iVal4);
        i = i + 1;
    }

    //QImage image(32, 34, QImage::Format_ARGB32);
    image = new QImage(imgX, imgY, QImage::Format_ARGB32);
    int l = 0;
    int x = 0;
    int y = 0;
    do
    {
        //image.setPixel(x, y, qRgba(red.at(l)*255 * 0.33, green.at(l)*255*0.6, blue.at(l)*255*0.11, alpha.at(l)*255));

        //QColor renk = QColor::fromCmyk(red.at(l)*255, green.at(l)*255, blue.at(l)*255, alpha.at(l)*255);

        //image.setPixel(x,y,renk.rgba());

        //image.setPixel(x, y, qRgb(dortlu.at(l)*255 * 0.33, dortlu.at(l)*255*0.6, dortlu.at(l)*255*0.11));
        if (dortlu.at(l) == 0)
            image->setPixel(x, y, qRgba(red.at(dortlu.at(l))*17, green.at(dortlu.at(l))*17, blue.at(dortlu.at(l))*17, 0 ));
        else
            image->setPixel(x, y, qRgba(red.at(dortlu.at(l))*17, green.at(dortlu.at(l))*17, blue.at(dortlu.at(l))*17, 255 ));

        x = x + 1;
        if (x == imgX)
        {
            x = 0;
            y = y +1;
        }
        l = l + 1;

    } while(l <= imgX * imgY);

    QPixmap a;

    a.convertFromImage(*image);
    ui->labelImage->setPixmap(a.scaled(ui->labelImage->width(), ui->labelImage->height(), Qt::KeepAspectRatio));
}

void MainWindow::on_btnOpenRAWImage_clicked()
{
    ClearImage();

    hadi = Converter(OpenImage());

    DrawImage();
    ui->btnSaveImage->setEnabled(true);
}

void MainWindow::on_btnSaveImage_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save Chunky Image"), "",
            tr("Portable Network Graphics (*.png)"));

    QImageWriter writer(fileName);
    writer.setFormat("png");
    writer.write(*image);
}

void MainWindow::ClearPalette()
{
    red.clear();
    green.clear();
    blue.clear();
    alpha.clear();
}

void MainWindow::on_btnOpenPalette_clicked()
{
    ClearPalette();

    bool ok;

    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Palette File"), "", tr("All Files (*)"));

    paletteFile = new QFile(fileName);
    QFileInfo fileinfo(fileName);

    // https://forum.qt.io/topic/79053/how-can-i-read-large-files-to-qbytearray
    if(!paletteFile->open(QIODevice::ReadOnly))
    {
        return;
    }
    QByteArray line = paletteFile->readAll();

    QString palet_xxx = Converter(line);

    int palet_i = 4;

    while(palet_i < 32*8)
    {
        QString str1 = palet_xxx.mid(palet_i, 4);
        QString str2 = palet_xxx.mid(palet_i + 4, 4);
        QString str3 = palet_xxx.mid(palet_i + 4 + 4, 4);
        QString str4 = palet_xxx.mid(palet_i + 4 + 4 + 4, 4);

        int iVal1 = str1.toInt(&ok,2);
        int iVal2 = str2.toInt(&ok,2);
        int iVal3 = str3.toInt(&ok,2);
        int iVal4 = str4.toInt(&ok,2);

        red.push_back(iVal1);
        green.push_back(iVal2);
        blue.push_back(iVal3);
        alpha.push_back(iVal4);

        qInfo() << palet_i / 12 << iVal1*17 << " " << iVal2*17 << " " << iVal3*17 << iVal4*17 << '\n';
        palet_i = palet_i + 16;
    }

    int x = 0;
    int y = 0;
    image2 = new QImage(4, 4, QImage::Format_RGB32);
    for (int i = 0; i < 16; ++i)
    {
        image2->setPixel(x, y, qRgb(red.at(i)*17, green.at(i)*17, blue.at(i)*17));
        x = x + 1;
        if (x == 4)
        {
            x = 0;
            y = y +1;
        }
    }

    QPixmap a2;

    a2.convertFromImage(*image2);
    ui->labelPalatte->setPixmap(a2.scaled(ui->labelPalatte->width(), ui->labelPalatte->height(), Qt::KeepAspectRatio));
    if(hadi != "")
    {
        DrawImage();
    }
    ui->btnOpenRAWImage->setEnabled(true);
}

void MainWindow::on_comboBoxResolution_currentIndexChanged(int index)
{
    imgX = vect.at(index).first;
    imgY = vect.at(index).second;
    DrawImage();
}
