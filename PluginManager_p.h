#pragma once

#include "PluginManager.h"

class PluginManagerPrivate : public QObject {
  Q_OBJECT
 public:
  PluginManager* q;
  std::unique_ptr<QProcess> m_pluginProcess;
  QLocalServer* m_server;

  explicit PluginManagerPrivate(PluginManager* q_ptr);
  ~PluginManagerPrivate();

  void init();

  // IKeyEventFilter interface
  void readStdout();
  void readStderr();
  void pluginRunnerConnected();
  void onFinished(int exitCode);
  void readRequest();
  void displayError(QLocalSocket::LocalSocketError);

  void sendError(const std::string& err, msgpack::rpc::msgid_t id);

};
