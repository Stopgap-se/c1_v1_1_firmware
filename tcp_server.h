#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

size_t tcpServerWrite(const char * data, size_t len);
void tcpServerBegin();
void tcpServerEnd();

#endif // _TCP_SERVER_H_