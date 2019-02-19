#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QString>
#include <QByteArray>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnOpenRAWImage_clicked();

    void on_btnSaveImage_clicked();

    void on_btnOpenPalette_clicked();

    void on_comboBoxResolution_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    QFile *file;
    QFile *paletteFile;
    QImage *image;
    QImage *image2;
    QString Converter(QByteArray);
    void ClearImage();
    void ClearPalette();
    QByteArray OpenImage();
    QByteArray OpenPalette();
    void DrawImage();
    void DrawPalette();

    std::vector<unsigned char> red;
    std::vector<unsigned char> green;
    std::vector<unsigned char> blue;
    std::vector<unsigned char> alpha;

    std::vector<unsigned char> plane1;
    std::vector<unsigned char> plane2;
    std::vector<unsigned char> plane3;
    std::vector<unsigned char> plane4;

    std::vector<unsigned char> dortlu;

    std::vector <std::pair<int, int>> vect;

    QString hadi;
    int imgX;
    int imgY;
};

#endif // MAINWINDOW_H
