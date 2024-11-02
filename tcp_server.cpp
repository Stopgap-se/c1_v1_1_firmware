#define ENABLE_LOG4ARDUINO
#include <log4arduino.h>
#include <AsyncTCP.h>
#include <ESPmDNS.h>

AsyncServer *_asyncTcpServer = NULL;
AsyncClient *_asyncClient = NULL;

void _tcpClientFree() {
    if (_asyncClient) {
        LOG("TCP.clientFree %s", _asyncClient->remoteIP().toString());
        _asyncClient->close(true);
        free(_asyncClient);
        _asyncClient = NULL;
    }
}

size_t tcpServerWrite(const char * data, size_t len) {
    size_t n = 0;
    if (_asyncClient) {
        if (_asyncClient->connected()) {
            n = _asyncClient->write(data, len);
            if (n != len) {
                LOG("COULD ONLY WRITE %d of %d to tcpClient", n, len);
            }
        }
        else {
            _tcpClientFree();
        }
    }
    return n;
}

void _onDisconnect(void *arg, AsyncClient* client) {
    LOGS("TCP.client DISCONNECTED");
    _tcpClientFree();
}
void _onError(void *arg, AsyncClient* client, int8_t error) {
    LOG("TCP.client ERROR %d %s", error, client->errorToString(error));
    _tcpClientFree();
}

void _tcpClientConnectHandler(void *arg, AsyncClient *client) {
    // free old existing?
    if (_asyncClient && !_asyncClient->connected()) {
        _tcpClientFree();
    }

    if (!_asyncClient) {
        _asyncClient = client;
        _asyncClient->onDisconnect(_onDisconnect, NULL);
        _asyncClient->onError(_onError, NULL);
        LOG("TCP.client connected from %s", client->remoteIP().toString());
    }
    else {
        client->close();
        LOG("TCP.client already serving %s", _asyncClient->remoteIP().toString());
    }
}

void _tcpServerSetup() {
    _tcpClientFree();
    _asyncTcpServer = new AsyncServer(23);
    _asyncTcpServer->onClient(&_tcpClientConnectHandler, _asyncTcpServer);
    MDNS.addService("telnet", "tcp", 23);
    LOGS("tcpServerSetup(23)");
}

void tcpServerBegin() {
    if (!_asyncTcpServer) {
        _tcpServerSetup();
    }
    _asyncTcpServer->begin();
}

void tcpServerEnd() {
    _asyncTcpServer->end();
}
