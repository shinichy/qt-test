#include <QApplication>
#include <QDebug>
#include <QObject>
#include <QThread>
#include "MyThread.h"
#include "MyThread2.h"

int main(int argv, char** args) {
  QApplication app(argv, args);

  MyThread* thread1 = new MyThread();
  MyThread2* thread2 = new MyThread2();
  thread1->m_thread2 = thread2;
  thread2->m_thread1 = thread1;
  QObject::connect(thread1, &MyThread::finished, &app, &QApplication::quit);
  thread1->start();
  thread2->start();
  thread1->moveToThread(thread1);
  thread2->moveToThread(thread2);
  return app.exec();
}
