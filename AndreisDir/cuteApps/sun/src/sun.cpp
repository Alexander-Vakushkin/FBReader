#include "sun.h"
#include "ui_sun.h"

Sun::Sun(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Sun)
{
    ui->setupUi(this);
}

Sun::~Sun()
{
    delete ui;
}

/* Метод, в котором происходит рисование
 * */
void Sun::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap));
    painter.setBrush(QBrush(Qt::yellow, Qt::SolidPattern));
    painter.drawEllipse(100, 80, 150, 150);
    painter.drawLine(QPoint(175,80), QPoint(175, 10));
    painter.drawLine(QPoint(175,230), QPoint(175, 300));
    painter.drawLine(QPoint(250,155), QPoint(325, 155));
    painter.drawLine(QPoint(35,155), QPoint(100, 155));
    painter.drawLine(QPoint(120,105), QPoint(50, 55));
    painter.drawLine(QPoint(120,205), QPoint(50, 255));
    painter.drawLine(QPoint(230,205), QPoint(305, 255));
    painter.drawLine(QPoint(230,105), QPoint(305, 55));
}

