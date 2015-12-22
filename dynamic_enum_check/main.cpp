#include <algorithm>
#include <QApplication>
#include <QDebug>
#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QMetaMethod>
#include <QDialog>
#include <QDialogButtonBox>
#include <QMetaObject>
#include <qnamespace.h>

#include "SleepThread.h"

struct QVariantArgument {
  operator QGenericArgument() const {
    if (value.isValid()) {
      return QGenericArgument(value.typeName(), value.constData());
    } else {
      return QGenericArgument();
    }
  }

  QVariant value;
};

bool qObjectPointerTypeCheck(QVariant var, const QByteArray& typeName) {
  return var.canConvert<QObject*>() &&
         var.value<QObject*>()->inherits(typeName.left(typeName.size() - 1));
}

bool enumTypeCheck(QVariant var, const QByteArray& typeName) {
  if (!var.canConvert<int>()) {
    return false;
  }

  int typeId = QMetaType::type(typeName);
  if (typeId != QMetaType::UnknownType) {
    const QMetaObject* metaObj = QMetaType(typeId).metaObject();
    const QByteArray& enumName = typeName.mid(typeName.lastIndexOf(":") + 1);
    return metaObj->indexOfEnumerator(enumName) > 0;
  }

  return false;
}

bool matchTypes(QList<QByteArray> types, QVariantList args) {
  if (types.size() != args.size()) {
    return false;
  }

  for (int i = 0; i < types.size(); i++) {
    if (types[i] != args[i].typeName() && !qObjectPointerTypeCheck(args[i], types[i]) &&
        !enumTypeCheck(args[i], types[i])) {
      return false;
    }
  }

  return true;
}

int main(int argv, char** args) {
  QApplication app(argv, args);

  //  const QMetaObject metaObj = SleepThread::staticMetaObject;
  //  for (int i = 0; i < metaObj.constructorCount(); i++) {
  //    qDebug() << metaObj.constructor(i).methodSignature();
  //  }

  //  int typeId = qRegisterMetaType<QLabel*>();
  //  qDebug() << typeId;
  //  QVariantArgument varArg;
  //  QByteArray className = obj->metaObject()->className();
  //  className.append('*');
  //  varArg.value = QVariant(QMetaType::type(className.constData()), obj);
  //  varArg.value = QVariant(typeId, obj);
  //  qDebug() << varArg.name();
  //  SleepThread* newObj =
  //  qobject_cast<SleepThread*>(SleepThread::staticMetaObject.newInstance(varArg));
  //  if (newObj) {
  //    qDebug() << "new SleepThread is created";
  //  }

  //  Qt::WindowFlags obj = Qt::Widget | Qt::Window;
  //  int typeId = qRegisterMetaType<Qt::WindowType>("Qt::WindowType");
  int typeId = qRegisterMetaType<QFlags<Qt::WindowType>>("Qt::WindowFlags");
  qDebug() << QMetaType::type("Qt::WindowFlags");
  qDebug() << QMetaType(typeId).flags();
//  const QMetaObject* metaObj = QMetaType(typeId).metaObject();
//  qDebug() << metaObj->className();
//  qDebug() << metaObj->enumeratorCount();
//  qDebug() << metaObj->enumerator(metaObj->indexOfEnumerator("WindowFlags")).name();
//  qDebug() << metaObj->enumerator(metaObj->indexOfEnumerator("WindowFlags")).isFlag();
  int obj = Qt::Widget | Qt::Window;
  QVariantList variants;
  variants.append(QVariant::fromValue(obj));

  QMetaObject metaObj = SleepThread::staticMetaObject;
  for (int i = 0; i < metaObj.constructorCount(); i++) {
    const QMetaMethod& ctor = metaObj.constructor(i);
    if (ctor.access() != QMetaMethod::Public) {
      continue;
    }

    if (matchTypes(ctor.parameterTypes(), variants)) {
      for (int j = 0; j < ctor.parameterCount(); j++) {
        variants[j] = QVariant(QMetaType::type(ctor.parameterTypes()[j]), variants[j].data());
      }
      QVariantArgument varArgs[10];
      for (int i = 0; i < variants.size(); i++) {
        varArgs[i].value = variants[i];
      }

      // return value is copied from another thread so it doesn't need memory barrier
      SleepThread* newObj =
          qobject_cast<SleepThread*>(SleepThread::staticMetaObject.newInstance(varArgs[0]));
      if (!newObj) {
        qWarning() << "invoking" << ctor.methodSignature() << "failed";
      }
    }
  }
}

// Q_DECLARE_METATYPE(QWidget*)
