#pragma once

#include <QThread>
#include <QEventLoop>

class MyThread;

class MyThread2 : public QThread {
  Q_OBJECT

 public:
  MyThread* m_thread1;

 public slots:
  void receive(int i);

 signals:
  void finishedHoge();

 protected:
  void run() {
    QEventLoop loop;
    loop.exec();
  }
};
