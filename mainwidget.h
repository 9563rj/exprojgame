#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QtWidgets>
#include <QWidget>

class MainWidget : public QWidget
{
 public:
  MainWidget(QWidget *parent = 0);

 protected:
  void paintEvent(QPaintEvent *event); // Painter event
  void drawImage(QPainter *qp); // Draw image function
};

#endif // MAINWIDGET_H
