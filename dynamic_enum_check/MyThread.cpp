#include <assert.h>
#include <QEventLoop>
#include <QDebug>
#include "MyThread.h"
#include "MyThread2.h"

void MyThread::receive(int i) {
  qDebug() << "received" << i;
  if (i == 2) {
    QEventLoop loop;
    qDebug() << "send 3";
    QMetaObject::invokeMethod(m_thread2, "receive", Qt::QueuedConnection, Q_ARG(int, 3));
    connect(&m_result3, &Result::ready, &loop, [&] {
      qDebug() << "result for 3 is ready. quit event loop for 3";
      loop.quit();
    });
    qDebug() << "start event loop for 3";
    loop.exec();
    qDebug() << "end event loop for 3";
  } else if (i == -1) {
    m_result1.setResult(i);
  } else if (i == -3) {
    m_result3.setResult(i);
  }
}

void MyThread::run() {
  QEventLoop loop;
  qDebug() << "send 1";
  bool result =
      QMetaObject::invokeMethod(m_thread2, "receive", Qt::QueuedConnection, Q_ARG(int, 1));
  assert(result);
  connect(&m_result1, &Result::ready, &loop, [&] {
    qDebug() << "result for 1 is ready. quit event loop for 1";
    loop.quit();
  });
  qDebug() << "start event loop for 1";
  loop.exec();
  qDebug() << "end event loop for 1";
}
