#ifndef MYWRAPPER__HH
#define MYWRAPPER__HH

#include <QObject>
#include <QDebug>
#include <QString>

class MyWrapper : public QObject {
    Q_OBJECT

public:
    MyWrapper();
    QString result();
public slots:
    void sendRequest(QString s);
    void receiveReply(QString s);

signals:
    void request(QString s);
    void finished();
private:
  QString st;

};

#endif //MYWRAPPER__HH
