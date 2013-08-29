#include "MyWrapper.hh"

MyWrapper::MyWrapper() {

}

void MyWrapper::sendRequest() {
    emit request();
}

void MyWrapper::receiveReply() {
    emit finished();
}
