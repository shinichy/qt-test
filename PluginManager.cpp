#include <unordered_map>
#include <string>
#include <vector>
#include <tuple>
#include <QProcess>
#include <QDebug>
#include <QFile>
#include <QKeyEvent>
#include <QMessageBox>

#include "PluginManager_p.h"

msgpack::rpc::msgid_t PluginManager::s_msgId = 0;
std::unordered_map<msgpack::rpc::msgid_t, ResponseResult*> PluginManager::s_eventLoopMap;

PluginManagerPrivate::~PluginManagerPrivate() {
  qDebug("~PluginManagerPrivate");
  if (m_pluginProcess) {
    disconnect(m_pluginProcess.get(), static_cast<void (QProcess::*)(int)>(&QProcess::finished),
               this, &PluginManagerPrivate::onFinished);
    m_pluginProcess->terminate();
  }
}

void PluginManagerPrivate::init() {
  Q_ASSERT(!m_pluginProcess);

  m_server = new QLocalServer(this);
  QFile socketFile("/tmp/deasync.sock");
  if (socketFile.exists()) {
    socketFile.remove();
  }

  if (!m_server->listen("/tmp/deasync.sock")) {
    qCritical("Unable to start the server: %s", qPrintable(m_server->errorString()));
    return;
  }

  connect(m_server, &QLocalServer::newConnection, this,
          &PluginManagerPrivate::pluginRunnerConnected);
}

void PluginManagerPrivate::sendError(const std::string& err, msgpack::rpc::msgid_t id) {
  if (q->m_isStopped)
    return;

  msgpack::sbuffer sbuf;
  msgpack::rpc::msg_response<msgpack::type::nil, std::string> response;
  response.msgid = id;
  response.error = err;
  response.result = msgpack::type::nil();

  msgpack::pack(sbuf, response);

  q->m_socket->write(sbuf.data(), sbuf.size());
}

PluginManagerPrivate::PluginManagerPrivate(PluginManager* q_ptr)
    : q(q_ptr), m_pluginProcess(nullptr), m_server(nullptr) {
}

void PluginManagerPrivate::readStdout() {
  QProcess* p = (QProcess*)sender();
  QByteArray buf = p->readAllStandardOutput();
  qDebug() << buf;
}

void PluginManagerPrivate::readStderr() {
  QProcess* p = (QProcess*)sender();
  QByteArray buf = p->readAllStandardError();
  qWarning() << buf;
}

void PluginManagerPrivate::pluginRunnerConnected() {
  qDebug() << "new Plugin runner connected";

  q->m_socket = m_server->nextPendingConnection();
  Q_ASSERT(q->m_socket);
  connect(q->m_socket, &QLocalSocket::disconnected, q->m_socket, &QLocalSocket::deleteLater);

  // QueuedConnection is necessary to emit readyRead() recursively.
  // > readyRead() is not emitted recursively; if you reenter the event loop or call
  // waitForReadyRead() inside a slot connected to the readyRead() signal, the signal will not be
  // reemitted (although waitForReadyRead() may still return true).
  connect(q->m_socket, &QLocalSocket::readyRead, this, &PluginManagerPrivate::readRequest,
          Qt::QueuedConnection);
  connect(q->m_socket,
          static_cast<void (QLocalSocket::*)(QLocalSocket::LocalSocketError)>(&QLocalSocket::error),
          this, &PluginManagerPrivate::displayError);
  q->run();
}

void PluginManagerPrivate::onFinished(int exitCode) {
  qWarning("plugin runner has stopped working. exit code: %d", exitCode);
  q->m_isStopped = true;
  auto reply =
      QMessageBox::question(nullptr, "Error",
                            "Plugin runner has stopped working. SilkEdit can continue to run but "
                            "you can't use any plugins. Do you want to restart the plugin runner?");
  if (reply == QMessageBox::Yes) {
    q->m_isStopped = false;
  }
}

void PluginManagerPrivate::readRequest() {
  //  qDebug("readRequest");

  msgpack::unpacker unpacker;
  std::size_t readSize = q->m_socket->bytesAvailable();

  // Message receive loop
  while (true) {
    unpacker.reserve_buffer(readSize);
    // unp has at least readSize buffer on this point.

    // read message to msgpack::unpacker's internal buffer directly.
    qint64 actual_read_size = q->m_socket->read(unpacker.buffer(), readSize);
    //    qDebug() << actual_read_size;
    QByteArray array(unpacker.buffer(), actual_read_size);
    //    qDebug() << QString(array.toHex());
    if (actual_read_size == 0) {
      break;
    } else if (actual_read_size == -1) {
      qCritical("unable to read a socket. %s", qPrintable(q->m_socket->errorString()));
      break;
    }

    // tell msgpack::unpacker actual consumed size.
    unpacker.buffer_consumed(actual_read_size);

    msgpack::unpacked result;
    // Message pack data loop
    while (unpacker.next(result)) {
      msgpack::object obj(result.get());
      msgpack::rpc::msg_rpc rpc;
      try {
        obj.convert(&rpc);

        switch (rpc.type) {
          case msgpack::rpc::REQUEST: {
            msgpack::rpc::msg_request<msgpack::object, msgpack::object> req;
            obj.convert(&req);
            QString methodName = QString::fromUtf8(req.method.as<std::string>().c_str());
            if (methodName == "add") {
              q->sendResponse(9, msgpack::type::nil(), req.msgid);
            }
          } break;

          case msgpack::rpc::RESPONSE: {
//            msgpack::rpc::msg_response<msgpack::object, msgpack::object> res;
//            obj.convert(&res);
//            auto found = PluginManager::s_eventLoopMap.find(res.msgid);
//            if (found != PluginManager::s_eventLoopMap.end()) {
//              //              qDebug("result of %d arrived", res.msgid);
//              if (res.error.type == msgpack::type::NIL) {
//                found->second->setResult(std::move(std::unique_ptr<object_with_zone>(
//                    new object_with_zone(res.result, std::move(result.zone())))));
//              } else {
//                found->second->setError(std::move(std::unique_ptr<object_with_zone>(
//                    new object_with_zone(res.error, std::move(result.zone())))));
//              }
//            } else {
//              qWarning("no matched response result for %d", res.msgid);
//            }
          } break;

          case msgpack::rpc::NOTIFY: {
//            msgpack::rpc::msg_notify<msgpack::object, msgpack::object> notify;
//            obj.convert(&notify);
//            QString methodName = QString::fromUtf8(notify.method.as<std::string>().c_str());
//            callNotifyFunc(methodName, notify.param);
          } break;
          default:
            qCritical("invalid rpc type");
        }
      } catch (msgpack::v1::type_error e) {
        qCritical() << "type error. bad cast.";
        continue;
      }
    }
  }
}

void PluginManagerPrivate::displayError(QLocalSocket::LocalSocketError socketError) {
  switch (socketError) {
    case QLocalSocket::ServerNotFoundError:
      qWarning("The host was not found.");
      break;
    case QLocalSocket::ConnectionRefusedError:
      qWarning("The connection was refused by the peer.");
      break;
    case QLocalSocket::PeerClosedError:
      break;
    default:
      qWarning("The following error occurred: %s.", qPrintable(q->m_socket->errorString()));
      break;
  }
}

void ResponseResult::setResult(std::unique_ptr<object_with_zone> obj) {
  //  qDebug("setResult");
  m_isReady = true;
  m_isSuccess = true;
  m_result = std::move(obj);
  emit ready();
}

void ResponseResult::setError(std::unique_ptr<object_with_zone> obj) {
  qDebug("setError");
  m_isReady = true;
  m_isSuccess = false;
  m_result = std::move(obj);
  emit ready();
}

PluginManager::~PluginManager() {
  qDebug("~PluginManager");
}

void PluginManager::init() {
  d->init();
}

void PluginManager::run()
{
    std::tuple<std::string> params = std::make_tuple("hoge");
    try {
      sendRequest<std::tuple<std::string>, bool>("run", params,
                                                                  msgpack::type::BOOLEAN, -1);
    } catch (const std::exception& e) {
//      qWarning("cmd: %s:, cause: %s", qPrintable(cmd), e.what());
    }
}

PluginManager::PluginManager()
    : d(new PluginManagerPrivate(this)), m_isStopped(false), m_socket(nullptr) {}
