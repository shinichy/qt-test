#pragma once

#include <QLocalServer>
#include <QLocalSocket>
#include <QFile>
#include <QEventLoop>
#include <QObject>
#include <QDataStream>

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

class Server : public QObject {
  Q_OBJECT

  Result m_result1;
  Result m_result3;
  QLocalSocket* m_sock;

 public:
  void start();

  void onReadyRead();

  void sendData(QLocalSocket* sock, int num);
};
