#include "calculator.h"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[]) {

  QApplication a(argc, argv);
  Calculator w;

  w.show();

  return a.exec();
}
