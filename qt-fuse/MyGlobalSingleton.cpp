#include "MyGlobalSingleton.hh"
#include <QDebug>


MyGlobalSingleton* MyGlobalSingleton::_instance = NULL;


MyGlobalSingleton* MyGlobalSingleton::Instance() {
    if(_instance == NULL) {
        _instance = new MyGlobalSingleton();
    }
    return _instance;
}

void MyGlobalSingleton::processRequest() {
  qDebug() << "hello world";
   emit reply();
}

