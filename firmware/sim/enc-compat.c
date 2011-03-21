#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <dnet.h>

static int pq_initialized = 0;
static int dnet_initialized = 0;
static eth_t *dnet_dev = NULL;

static char *dev = "en0";

void enc28j60PacketSend(uint16_t len, uint8_t* packet) {
    if(!pq_initialized) {
        pq_init(dev);
        pq_initialized=1;
    }

    pq_inject(packet, len);

    /* if(!dnet_initialized) { */
    /*     dnet_dev = eth_open(dev); */
    /*     if(!dnet_dev) { */
    /*         perror("eth_open"); */
    /*         exit(EXIT_FAILURE); */
    /*     } */
    /* } */

    /* eth_send(dnet_dev, packet, len); */
}

uint16_t enc28j60PacketReceive(uint16_t maxlen, uint8_t* packet) {
    uint16_t len;
    uint8_t *new_packet;

    if(!pq_initialized) {
        pq_init(dev);
        pq_initialized=1;
    }

    /* if(!pq_packet_available()) */
    /*     return 0; */

    pq_packet_wait();

    pq_fetch(&new_packet, &len);
    fprintf(stderr, "Got packet of length %d\n", len);

    memcpy(packet, new_packet, len < maxlen ? len : maxlen);

    free(new_packet);
    return (len < maxlen ? len : maxlen);
}

