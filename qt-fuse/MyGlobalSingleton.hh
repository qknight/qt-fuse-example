#ifndef MYGLOBALSINGLETON__HH
#define MYGLOBALSINGLETON__HH

#include <QObject>

class MyGlobalSingleton : public QObject {
  Q_OBJECT
public:
    static MyGlobalSingleton* Instance();
    
public slots:
  void processRequest(QString s);
  
signals:
  void reply(QString s);
  private:
    static MyGlobalSingleton* _instance;
};

#endif 