#ifndef MYWRAPPER__HH
#define MYWRAPPER__HH

#include <QObject>

class MyWrapper : public QObject {
    Q_OBJECT

public:
    MyWrapper();

public slots:
    void sendRequest();
    void receiveReply();

signals:
    void request();
    void finished();

};

#endif //MYWRAPPER__HH
