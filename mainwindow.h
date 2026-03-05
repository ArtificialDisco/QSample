#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAudioBuffer>
#include <QAudioOutput>
#include <QtMultimedia/qaudiodecoder.h>
#include <QAudioSink>
#include <QFile>

#include "waveform.h"
#include "spectrogram.h"
#include "fft.h"

#include <iostream>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class FileDevice : public QFile {
public:
    FileDevice(int sampleRate)
        : sampleRate(sampleRate) {
        std::cout << "sr = " << sampleRate << std::endl;
        fft = new FFT(sampleRate, 2048, 4.0);
        QFile();
    };
    ~FileDevice() {
        delete fft;
    }

    bool open(QIODeviceBase::OpenMode mode) override { return QFile::open(mode); }
    void setFileName(const char* s) { QFile::setFileName(s); }

    qint64 readData(char* data, qint64 maxlen) override {
        qint64 len = QFile::readData(data, maxlen);

        float indata[len*2];
        float outdata[len*2];

        for (int i = 0; i < len; i += 4) {
            qint16 left = (data[i+1] << 8) | (0xff & data[i]);
            data[i+1] = (left >> 8) & 0xff;
            data[i] = left & 0xff;
            qint16 right = (data[i+3] << 8) | (0xff & data[i+2]);
            data[i+3] = (right >> 8) & 0xff;
            data[i+2] = right & 0xff;

            indata[i/2] = left;
            indata[i/2+1] = right;
        }

        fft->processPitchShift(indata, outdata, len/2, 1.1);


        /*for (int i = 0; i < len; i += 2) {
            qint16 left = (qint16)(outdata[i/2] / 2);
            data[i+1] = (left >> 8) & 0xff;
            data[i] = left & 0xff;
        }*/

        return len;
    }
private:
    int sampleRate;
    FFT* fft;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    enum GraphicType {
        WAVEFORM,
        SPECTROGRAM
    } graphicType = WAVEFORM;

    void openFile();
    void resetAudio();

    Ui::MainWindow* ui;
    QAudioDecoder* audio_decoder = nullptr;

    QAudioSink* audio_playback;
    std::string audio_filename;
    FileDevice* audio_file;

    Waveform* waveform;
    Spectrogram* spectrogram;

    QAudioFormat audioFormat;

    FFT* fft = nullptr;

private slots:
    void on_actionOpen_triggered();
    void readBuffer();
    void decodeFinished();
    void on_actionExit_triggered();
    void on_radioButton_toggled(bool checked);
    void on_radioButton_2_toggled(bool checked);
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_checkBox_checkStateChanged(const Qt::CheckState &arg1);
    void on_pushButton_3_clicked();
};
#endif // MAINWINDOW_H
