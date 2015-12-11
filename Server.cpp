#include "Server.h"

void Server::start() {
  QString sockPath = "/tmp/qeventloop_test";
  QLocalServer* server = new QLocalServer();
  QFile sockFile(sockPath);
  if (sockFile.exists()) {
    sockFile.remove();
  }
  server->listen(sockPath);
  QObject::connect(server, &QLocalServer::newConnection, [this, server]() {
    m_sock = server->nextPendingConnection();
    QObject::connect(m_sock, &QLocalSocket::disconnected, m_sock, &QLocalSocket::deleteLater);
    QObject::connect(m_sock, &QLocalSocket::readyRead, this, &Server::onReadyRead,
                     Qt::QueuedConnection);

    sendData(m_sock, 1);

    QEventLoop loop;
    QObject::connect(&m_result1, &Result::ready, &loop, &QEventLoop::quit);
    qDebug("start event loop to wait for 1");
    loop.exec();
    qDebug("end event loop to wait for 1");
  });
}

void Server::onReadyRead() {
  qDebug("bytesAvailable: %lld", m_sock->bytesAvailable());
  qint64 bytesAvailable = m_sock->bytesAvailable();
  QByteArray buffer = m_sock->readAll();
  QDataStream ds(buffer);
  while (bytesAvailable > 0) {
    int num;
    ds >> num;
    qDebug("received %d", num);
    bytesAvailable -= 4;
    if (num == 2) {
      sendData(m_sock, 3);

      QEventLoop loop;
      QObject::connect(&m_result3, &Result::ready, &loop, &QEventLoop::quit);
      qDebug("start event loop to wait for 3");
      loop.exec();
      qDebug("end event loop to wait for 3");

    } else if (num == -1) {
      m_result1.setResult(num);
    } else if (num == -3) {
      m_result3.setResult(num);
    }
  }
}

void Server::sendData(QLocalSocket* sock, int num) {
  qDebug("send %d", num);
  QByteArray block;
  QDataStream ds(&block, QIODevice::WriteOnly);
  ds << num;
  sock->write(block);
}
