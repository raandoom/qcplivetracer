#include <QApplication>
#include <QTextCodec>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  QApplication::setGraphicsSystem("raster");
#endif
  QApplication a(argc, argv);
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
  MainWindow w;
  w.show();
  
  return a.exec();
}
