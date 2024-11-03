#ifndef _WEB_SERVER_H_
#define _WEB_SERVER_H_

static const char ApplicationJson[] = {"application/json"};
static const char StatusOnline[] = {"online"};
static const char StatusOffline[] = {"offline"};
static const char TopicConfig[] = {"config"};
static const char TopicStatus[] = {"status"};
static const char TopicLog[] = {"log"};

void webServerBegin();
void eventSourceSend(const char *message,
        const char *event = NULL,
        uint32_t id = 0,
        uint32_t reconnect = 0);
void eventSourceTeardown();
size_t sprintConfig(char *buf);

void webServerSetup();

#endif // _WEB_SERVER_H_