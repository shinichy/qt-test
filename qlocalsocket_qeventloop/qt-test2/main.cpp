#include <QDebug>
#include <QLocalSocket>
#include <QApplication>
#include <QDataStream>

void sendData(QLocalSocket& sock, int num) {
  qDebug("send %d", num);
  QByteArray block;
  QDataStream ds(&block, QIODevice::WriteOnly);
  ds << num;
  sock.write(block);
}

int main(int argv, char** args) {
  QApplication app(argv, args);

  QLocalSocket sock;
  QObject::connect(&sock, &QLocalSocket::readyRead, [&sock]() {
    qint64 bytesAvailable = sock.bytesAvailable();
    QByteArray buffer = sock.readAll();
    QDataStream ds(buffer);
    while (bytesAvailable > 0) {
      int num;
      ds >> num;

      qDebug("received %d", num);
      bytesAvailable -= 4;

      if (num == 1) {
        sendData(sock, 2);
        sendData(sock, -1);
      } else if (num == 3) {
        sendData(sock, -3);
      }
    }
  });

  sock.connectToServer("/tmp/qeventloop_test");
  return app.exec();
}
