#ifndef _WEB_SERVER_H_
#define _WEB_SERVER_H_

static const char ApplicationJson[] = {"application/json"};

void webServerBegin();
void eventSourceSend(const char *message,
        const char *event = NULL,
        uint32_t id = 0,
        uint32_t reconnect = 0);
void eventSourceTeardown();

void webServerSetup();

#endif // _WEB_SERVER_H_