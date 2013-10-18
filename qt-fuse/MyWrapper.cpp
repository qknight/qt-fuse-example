#include "MyWrapper.hh"

MyWrapper::MyWrapper() {

}

void MyWrapper::sendRequest(QString s) {
    emit request(s);
}

void MyWrapper::receiveReply(QString s) {
    st = s;
    emit finished();
}

QString MyWrapper::result() {
    return st;
}
