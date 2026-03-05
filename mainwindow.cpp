#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QPushButton.h>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QAudioDevice>
#include <QMediaDevices>

#include <QAudioSink>
#include <QtMultimedia/qaudioformat.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    waveform = new Waveform(ui->graphicsView);
    spectrogram = new Spectrogram(ui->graphicsView);

    ui->graphicsView->setPixmap(spectrogram->Pixmap());

    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
}

MainWindow::~MainWindow() {
    delete ui;
    delete audio_decoder;
    delete waveform;
    delete audio_playback;
    delete audio_file;
    if (fft) {
        delete fft;
    }
}

void MainWindow::resetAudio() {
    if (audio_decoder) {
        delete audio_decoder;
        audio_decoder = nullptr;
    }
    audio_decoder = new QAudioDecoder(this);
    waveform->reset(audio_decoder);
    spectrogram->reset(audio_decoder);
}

void MainWindow::openFile() {
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    if (dialog.exec()) {
        resetAudio();
        audio_filename = dialog.selectedFiles().at(0).toStdString();

        audio_decoder->setSource(QUrl(audio_filename.c_str()));
        audio_decoder->start();
        QAudioBuffer buffer = audio_decoder->read();
        connect(audio_decoder, &QAudioDecoder::bufferReady,
                this, &MainWindow::readBuffer);
        connect(audio_decoder, &QAudioDecoder::finished,
                this, &MainWindow::decodeFinished);
    }
}

void MainWindow::on_actionOpen_triggered() {
    openFile();
}

// Currently draws a waveform. Needs work when it comes to mono / stereo.
// In that case we should draw 2 waveforms, probably.
void MainWindow::readBuffer() {
    ui->radioButton->setEnabled(false);
    ui->radioButton_2->setEnabled(false);

    QAudioBuffer buffer = audio_decoder->read();
    audioFormat = buffer.format();
    if (graphicType == WAVEFORM) {
        bool update = waveform->drawBuffer(&buffer);
        if (update) {
            ui->graphicsView->update();
        }
        spectrogram->drawBuffer(&buffer);
    } else {
        waveform->drawBuffer(&buffer);
        bool update = spectrogram->drawBuffer(&buffer);
        if (update) {
            ui->graphicsView->update();
        }
    }
}

void MainWindow::decodeFinished() {
    ui->radioButton->setEnabled(true);
    ui->radioButton_2->setEnabled(true);
    ui->graphicsView->update();

    QAudioDevice info(QMediaDevices::defaultAudioOutput());

    if (!info.isFormatSupported(audioFormat)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        return;
    }

    audio_file = new FileDevice(audioFormat.sampleRate());
    audio_file->setFileName(audio_filename.c_str());
    bool open = audio_file->open(QIODevice::ReadOnly);
    if (!open) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        return;
    }

    audio_playback = new QAudioSink(audioFormat, this);
    //audio_playback = new QAudioSink(audioFormat, this);
    connect(audio_playback, SIGNAL(stateChanged(QAudio::State)),
            this, SLOT(handleStateChanged(QAudio::State)));

    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
}

void MainWindow::on_actionExit_triggered() {
    close();
}

// Waveform
void MainWindow::on_radioButton_toggled(bool checked)
{
    if (checked) {
        graphicType = WAVEFORM;
        ui->graphicsView->setPixmap(waveform->Pixmap());
        ui->graphicsView->update();
    }
}

// Spectrogram
void MainWindow::on_radioButton_2_toggled(bool checked)
{
    if (checked) {
        graphicType = SPECTROGRAM;
        ui->graphicsView->setPixmap(spectrogram->Pixmap());
        ui->graphicsView->update();
    }
}


void MainWindow::on_pushButton_clicked()
{
    std::cout << (void*)audio_playback << std::endl;
    std::cout << "audioFile = " << audio_file->fileName().toStdString() << std::endl;
    audio_playback->start(audio_file);
}


void MainWindow::on_pushButton_2_clicked()
{
    audio_playback->stop();
}


void MainWindow::on_checkBox_checkStateChanged(const Qt::CheckState &arg1)
{
    bool checked = (arg1 == Qt::CheckState::Checked) ? true : false;
    ui->graphicsView->setDarkMode(checked);
}


void MainWindow::on_pushButton_3_clicked()
{
    openFile();
}

