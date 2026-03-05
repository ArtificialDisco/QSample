#include "spectrogram.h"

#include <algorithm>

#include <cmath>

Spectrogram::Spectrogram(MyGraphicsView* graphicsView)
    : graphics_view(graphicsView) {
    QRect r = graphicsView->rect();
    pixmap = new QPixmap(r.width(), r.height());
    pixmap->fill(Qt::transparent);

    QPainter painter(pixmap);
    painter.setPen(Qt::blue);
    painter.setFont(QFont("Arial", 30));
    painter.drawText(r, Qt::AlignCenter, "SPECTROGRAM");
}

Spectrogram::~Spectrogram() {
    if (pixmap) {
        delete pixmap;
    }
    if (fft) {
        delete fft;
    }
}

void Spectrogram::reset(QAudioDecoder* decoder) {
    audio_decoder = decoder;
    parameters_ready = false;
    current_sample = 0;
    pixmap->fill(Qt::transparent);
}

float heatMap[13][5] = {
    { 133, 193, 200, 0,       0.01 },
    { 0, 0x39, 0x74, 255,     0.0925 },
    { 0x00, 0x52, 0xdd, 255,  0.175 },
    { 0xf7, 0xd4, 0x5d, 255,  0.2575 },
    { 0xf7, 0xd4, 0x5d, 255,  0.34 },
    { 0xf7, 0xd4, 0x5d, 255,  0.4225 },
    { 0xe0, 0x4d, 0x1d, 255,  0.505 },
    { 0xe0, 0x4d, 0x1d, 255,  0.5875 },
    { 0xee, 0x8f, 0x19, 255,  0.67 },
    { 0xeb, 0x82, 0x00, 355,  0.7525 },
    { 0x37, 0x6b, 0x2b, 255,  0.835 },
    { 0xe6, 0x4e, 0x00, 255,  0.9175 },
    { 255, 255, 0, 255,   1.0 }
};

QColor Spectrogram::colorFromHeatmap(float value) {
    if (value > 0 && value <= heatMap[0][4])
        return QColor::fromRgb(heatMap[0][0], heatMap[0][1],
                               heatMap[0][2], heatMap[0][3]);
    for (int i = 1; i < 13; i++) {
        if (value > heatMap[i-1][4] && value <= heatMap[i][4]) {
            return QColor(heatMap[i][0], heatMap[i][1],
                          heatMap[i][2], heatMap[i][3]);
        }
    }
    return Qt::black;
}

void Spectrogram::prepare(QAudioFormat fmt) {
    if (fft) {
        delete fft;
    }
    fft = new FFT(fmt.sampleRate(), 2048, 1.0);

    QRect r = graphics_view->rect();
    max_volume = pow(2, 8*fmt.bytesPerSample() - 1);
    audio_length = (float)audio_decoder->duration() * fmt.sampleRate() / 1000.0;

    const float max_freq = fmt.sampleRate() / 2;
    x_scale = ((float)r.width() / audio_length);
    y_scale = (float)r.height() / std::log(20000.0);
}

void Spectrogram::drawSample(float magnitude, float frequency, float min, float max) {
    static float cmax = 0;
    static float cmin = 0;
    QRect r = graphics_view->rect();
    int xpos = current_sample * x_scale;
    int ypos = (float)r.height() - (std::log(frequency)) * y_scale;

    float f_int = abs(magnitude - min) / (max - min);
    int intensity = f_int * 255;
    if (intensity == 0) {
        return;
    }
    QPainter painter(pixmap);
    if (intensity < 255 && intensity > 0) {
        painter.setPen(colorFromHeatmap(f_int));
        painter.fillRect(xpos, ypos, std::max<float>(1.0, 6*f_int),
                         std::max<float>(1.0, 3*f_int), QBrush(colorFromHeatmap(f_int)));
    }

    last_xpos = xpos;
}

bool Spectrogram::drawBuffer(QAudioBuffer* buffer) {
    if (!parameters_ready) {
        prepare(buffer->format());
        parameters_ready = true;
    }
    char* data = buffer->data<char>();
    float indata[buffer->byteCount()];
    for (int i = 0; i < buffer->byteCount(); i += 4) {
        qint16 left = (data[i+1] << 8) | (0xff & data[i]);
        data[i+1] = (left >> 8) & 0xff;
        data[i] = left & 0xff;
        qint16 right = (data[i+3] << 8) | (0xff & data[i+2]);
        data[i+3] = (right >> 8) & 0xff;
        data[i+2] = right & 0xff;

        indata[i/2] = left;
        indata[i/2+1] = right;
        current_sample++;
    }

    float outdata[buffer->byteCount()/2];
    fft->processForwardOnly(indata, outdata,
                            buffer->byteCount()/2);

    float max_out = 1.0;
    float min_out = 1.0;
    for (int i = 0; i < buffer->byteCount()/2; i+=2) {
        if (outdata[i] > max_out) {
            max_out = outdata[i];
        }
        if (outdata[i] < min_out) {
            min_out = outdata[i];
        }
    }
    float freq_min = 20000.0;
    for(int i = 0; i < buffer->byteCount()/2; i += 2) {
        if (outdata[i] > 0 && outdata[i] < freq_min) {
            freq_min = outdata[i];
        }
    }

    for (int i = 1; i < buffer->byteCount()/2; i++) {
        drawSample(outdata[2*i], outdata[2*i+1], min_out, max_out);
    }
    if (last_xpos % 10 == 0) {
        return true;
    }
    return false;
}
