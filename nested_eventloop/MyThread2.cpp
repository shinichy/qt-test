#include <QDebug>
#include "MyThread2.h"
#include "MyThread.h"

void MyThread2::receive(int i) {
  qDebug() << "received" << i;
  if (i == 1) {
    qDebug() << "send 2";
    QMetaObject::invokeMethod(m_thread1, "receive", Qt::QueuedConnection, Q_ARG(int, 2));
    qDebug() << "send -1";
    QMetaObject::invokeMethod(m_thread1, "receive", Qt::QueuedConnection, Q_ARG(int, -1));
  } else if (i == 3) {
    qDebug() << "send -3";
    QMetaObject::invokeMethod(m_thread1, "receive", Qt::QueuedConnection, Q_ARG(int, -3));
  }
}
