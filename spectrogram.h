#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H

#include "mygraphicsview.h"

#include <QAudioDecoder>

#include "fft.h"

class Spectrogram
{
public:
    Spectrogram(MyGraphicsView* graphicsView);
    ~Spectrogram();
    QPixmap* Pixmap() { return pixmap; }

    bool drawBuffer(QAudioBuffer* buffer);
    void drawSample(float magnitude, float frequency, float min, float max);

    void reset(QAudioDecoder* decoder);

private:
    MyGraphicsView* graphics_view;
    QAudioDecoder* audio_decoder = nullptr;

    QColor colorFromHeatmap(float value);

    void prepare(QAudioFormat fmt);

    QPixmap* pixmap = nullptr;
    FFT* fft = nullptr;

    int current_sample = 0;

    float audio_length;
    float max_volume;
    float x_scale;
    float y_scale;

    int last_xpos = 0;
    bool parameters_ready = false;
};

#endif // SPECTROGRAM_H
