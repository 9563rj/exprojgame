#include <QtWidgets>
#include "mainwidget.h"

int main(int argc, char* argv[])
{
  QApplication a(argc, argv);

  MainWidget w;
  w.resize(200, 200);
  w.show();
  
  return a.exec();
}
