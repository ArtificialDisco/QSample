#include "mygraphicsview.h"

MyGraphicsView::MyGraphicsView(QWidget* parent)
 : QWidget(parent) {
    scene = new QGraphicsScene(parent);

    setAutoFillBackground(true);

    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, Qt::black);

    setPalette(pal);
}

MyGraphicsView::~MyGraphicsView() {
    delete scene;
}

void MyGraphicsView::setPixmap(QPixmap* p) {
    pixmap = p;
}

void MyGraphicsView::setDarkMode(bool dark) {
    if (dark_mode == dark) {
        return;
    }
    dark_mode = dark;

    QPalette pal = QPalette();
    if (dark) {
        pal.setColor(QPalette::Window, Qt::black);
    } else {
        pal.setColor(QPalette::Window, Qt::white);
    }
    setPalette(pal);
}

void MyGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    mousePoint = event->pos();
    const int x = mousePoint.x();
    const int y = mousePoint.y();
    scene->addEllipse(x, y, 10, 10, QPen(), QBrush(Qt::SolidPattern));
    update();
    QWidget::mouseMoveEvent(event);
}

void MyGraphicsView::paintCheckeredBackground() {
    QPainter painter(pixmap);
    QBrush brush1(QColor("cyan"));
    QBrush brush2(QColor("white"));
    for (int x = 0; x < rect().width(); x += 20) {
        bool colored_square = !(x % 40);
        for (int y = 0; y < rect().height(); y += 20) {
            painter.fillRect(x, y, 20, 20,
                             colored_square ? brush1 : brush2);
            colored_square = (colored_square ? false : true);
        }
    }
}

void MyGraphicsView::paintEvent(QPaintEvent *e) {
    QPainter painter(this);
    painter.drawPixmap(rect(), *pixmap, rect());
}
