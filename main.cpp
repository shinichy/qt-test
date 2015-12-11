#include <QApplication>
#include <QDebug>
#include <QObject>
#include "Server.h"

int main(int argv, char** args) {
  QApplication app(argv, args);

  Server server;
  server.start();

  return app.exec();
}
