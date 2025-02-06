#ifndef _BLINK_H_
#define _BLINK_H_

typedef enum {
    DISCONNECTED = 0,
    CONNECTED,
    CONNECTING
} connection_status_t;

void rgb_led_task(void *arg);

extern connection_status_t connection_status;

#endif