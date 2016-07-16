#include <QString>
#include <QDebug>
#include <QVariant>
#include "MyThread.h"

/* Creating and using text boundaries */
int main(void) {
  TestClass obj;
  QString returnedValue;
  bool rslt =
      QMetaObject::invokeMethod(&obj, "test", Qt::DirectConnection,
                                Q_RETURN_ARG(QString, returnedValue), Q_ARG(QVariant, QVariant()));
  qDebug() << returnedValue;
}
