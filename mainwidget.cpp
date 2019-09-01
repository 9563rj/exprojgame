#include <QtWidgets>
#include "mainwidget.h"

MainWidget::MainWidget(QWidget *parent)
  : QWidget(parent)
{}

void MainWidget::paintEvent(QPaintEvent *e)
{
  Q_UNUSED(e);

  QPainter qp(this);
  drawImage(&qp);
}

void MainWidget::drawImage(QPainter *qp)
{
  QRectF target(0.0, 0.0, 200.0, 200.0);
  QRectF source(0.0, 0.0, 200.0, 200.0);
  QPixmap pixmap("sprites/cool.png");

  qp->drawPixmap(target, pixmap, source);
}
