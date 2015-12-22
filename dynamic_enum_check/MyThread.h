#pragma once

#include <QThread>
#include <QDebug>

class Result : public QObject {
  Q_OBJECT

  int m_result;

 public:
  int result() { return m_result; }
  void setResult(int result) {
    m_result = result;
    emit ready();
  }

 signals:
  void ready();
};

class MyThread2;

class MyThread : public QThread {
  Q_OBJECT

  Result m_result1;
  Result m_result3;

 public:
  MyThread2* m_thread2;

public slots:
  void receive(int i);

 signals:
  void finishedFoo();

 protected:
  void run();
};
