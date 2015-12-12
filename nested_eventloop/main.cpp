#include <QApplication>
#include <QDebug>
#include <QObject>
#include "SleepThread.h"

int main(int argv, char** args) {
  QApplication app(argv, args);

  SleepThread* thread1 = new SleepThread();
  QObject::connect(thread1, &QThread::finished, &app, &QApplication::quit);
  thread1->moveToThread(thread1);
  thread1->start();
  QMetaObject::invokeMethod(thread1, "sleep", Q_ARG(unsigned long, 3000));
  QMetaObject::invokeMethod(thread1, "sleep", Q_ARG(unsigned long, 5000));
  app.exec();
}
