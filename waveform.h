#ifndef WAVEFORM_H
#define WAVEFORM_H

#include "mygraphicsview.h"

#include <QAudioBuffer>
#include <QAudioDecoder>

class Waveform
{
public:
    Waveform(MyGraphicsView* graphicsView);
    ~Waveform();

    void reset(QAudioDecoder* decoder);
    void start(QUrl source);

    void drawSample(const int xpos, const int ypos);
    bool drawBuffer(QAudioBuffer* buffer);

    QPixmap* Pixmap() { return pixmap; }

private:
    MyGraphicsView* graphics_view;
    QAudioDecoder* audio_decoder = nullptr;

    void prepare(QAudioFormat fmt);

    // WaveformDisplay owns its own pixmap and is resposible for creating
    // and destroying it.
    QPixmap* pixmap = nullptr;

    int current_sample = 0;
    int last_xpos = 0;
    float y_max = 0;
    float y_min = 0;

    float audio_length;
    float max_volume;
    float x_scale;
    float y_scale;

    bool parameters_ready = false;
};

#endif // WAVEFORM_H
