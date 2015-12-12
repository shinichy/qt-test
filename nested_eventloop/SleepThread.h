#pragma once

#include <QThread>
#include <QEventLoop>
#include <QTimer>

class SleepThread : public QThread {
  Q_OBJECT
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
