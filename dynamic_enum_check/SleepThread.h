#pragma once

#include <QThread>
#include <QEventLoop>
#include <QTimer>

class SleepThread : public QThread {
  Q_OBJECT

public:
  Q_FLAG(Qt::WindowFlags)
//  Q_INVOKABLE SleepThread() : QThread() {
//    qDebug() << "default constructor called";
//  }
//  Q_INVOKABLE SleepThread(QWidget * parent, Qt::WindowFlags ) : QThread() {
//    qDebug() << "parent widget is passed";
//  }
  Q_INVOKABLE SleepThread(Qt::WindowFlags ) : QThread() {
    qDebug() << "parent widget is passed";
  }

  Q_INVOKABLE SleepThread(Qt::Orientation ) : QThread() {
    qDebug() << "parent widget is passed";
  }

 public slots:
  void sleep(unsigned long msec) {
    QEventLoop loop;
    qDebug() << "start timer with" << msec << "[msec]";
    QTimer::singleShot(msec, &loop, [&loop, msec]{
      qDebug() << "finished waiting" << msec << "[msec]";
      loop.quit();
    });
    loop.exec();
    qDebug() << "event loop of" << msec << "[msec] finished";
    exit();
  }
};
