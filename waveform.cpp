#include "waveform.h"
#include <iostream>

#include <QPainter>

Waveform::Waveform(MyGraphicsView* graphicsView)
    : graphics_view(graphicsView) {
    QRect r = graphicsView->rect();
    pixmap = new QPixmap(r.width(), r.height());
    pixmap->fill(Qt::transparent);

    QPainter painter(pixmap);
    painter.setPen(Qt::blue);
    painter.setFont(QFont("Arial", 30));
    painter.drawText(r, Qt::AlignCenter, "WAVEFORM");
}

Waveform::~Waveform() {
    std::cout << "~Waveform" << std::endl;
    if (pixmap) {
        delete pixmap;
    }
}

void Waveform::reset(QAudioDecoder* decoder) {
    // Values used for drawing the waveform and need to
    // outlive the readBuffer method.
    current_sample = 0;
    last_xpos = 0;
    y_max = 0;
    y_min = 0;

    parameters_ready = false;
    audio_decoder = decoder;

    pixmap->fill(Qt::transparent);
}

void Waveform::drawSample(const int xpos, const int ypos) {
    QRect r = graphics_view->rect();
    QPainter painter(pixmap);
    painter.setPen(Qt::blue);
    int x = xpos;
    int y = r.height()/2 + ypos;
    painter.drawLine(xpos, r.height()/2, x, y);
}

void Waveform::prepare(QAudioFormat fmt) {
    QRect r = graphics_view->rect();

    std::cout << "Sample rate: " << fmt.sampleRate() << std::endl;
    std::cout << "Bytes per sample: " << fmt.bytesPerSample() << std::endl;
    std::cout << "Channel count: " << fmt.channelCount() << std::endl;

    // Some parameters that remains constant through drawing the
    // waveform but we need the first read() to be called first.
    max_volume = pow(2, 8*fmt.bytesPerSample() - 1);
    audio_length = audio_decoder->duration() * (fmt.sampleRate() / 1000.0);

    x_scale = ((float)r.width() / audio_length) / fmt.channelCount();
    y_scale = ((float)r.height() / max_volume) / 4;
}

bool Waveform::drawBuffer(QAudioBuffer* buffer) {
    if (!parameters_ready) {
        prepare(buffer->format());
        parameters_ready = true;
    }
    bool updateGraphics = false;

    const int bps = buffer->format().bytesPerSample();

    const qint16* const_data_16 =
        bps == 2 ? buffer->constData<qint16>() : nullptr;
    const qint32* const_data_32 =
        bps == 4 ? buffer->constData<qint32>() : nullptr;

    for (int i = 0; i < buffer->sampleCount(); i++) {
        const int xpos = current_sample * x_scale;
        if (xpos > last_xpos) {
            drawSample(xpos, y_max*y_scale);
            drawSample(xpos, y_min*y_scale);
            y_max = 0;
            y_min = 0;
            if (xpos % 10 == 0) {
                updateGraphics = true;
            }
        } else {
            qint32 data = 0; // 32 bit can hold both 32 and 16 values.
                             // Falls back to 0 if bps is weird.
            if (bps == 2) {
                data = const_data_16[i];
            } else if (bps == 4) {
                data = const_data_32[i];
            }
            y_max = (data > y_max) ? data : y_max;
            y_min = (data < y_min) ? data : y_min;

        }
        current_sample++;
        last_xpos = xpos;
    }
    return updateGraphics;
}
