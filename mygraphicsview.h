#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>

class MyGraphicsView : public QWidget
{
    Q_OBJECT
public:
    MyGraphicsView(QWidget* parent);
    ~MyGraphicsView();
    void mouseMoveEvent(QMouseEvent *event) override;
    void drawSample(const int xpos, const int ypos);
    void paintCheckeredBackground();
    void setPixmap(QPixmap* pixmap);
    void setDarkMode(bool dark);

    // Hold a pointer to a pixmap so we can paint it inside paintEvent(). The
    // pixmap itself belongs to either Waveform or Spectrogram.
    QPixmap* pixmap;

private:
    void paintEvent(QPaintEvent *e) override;

    bool dark_mode = true;

    QPoint mousePoint;
    QGraphicsScene* scene;
};

#endif // MYGRAPHICSVIEW_H
