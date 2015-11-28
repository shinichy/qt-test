#include <QPlainTextEdit>
#include <QApplication>
#include <QMetaMethod>
#include <QDebug>
#include <iostream>
#include <msgpack.hpp>
#include "protocol.h"

#include "PluginManager.h"

using namespace std;

int main(int argv, char** args)
{
  QApplication app(argv, args);
  auto helper = new PluginManager();
  helper->init();
  return app.exec();
}

