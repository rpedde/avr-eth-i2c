/*
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>
#include <string.h>
#include <pcap.h>

#include "main.h"
#include "pq.h"

typedef struct pq_list_t {
    char *packet;
    int len;
    struct pq_list_t *next;
} pq_list_t;

static pq_list_t pq_list = { NULL, 0, NULL };
static pq_list_t *pq_tail = NULL;
static pthread_mutex_t pq_list_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t pq_list_notify = PTHREAD_COND_INITIALIZER;
static pthread_t pq_tid;
static pcap_t *pq_pcap_handle = NULL;

static void pq_callback(u_char *args, const struct pcap_pkthdr *header,
                        const u_char *packet) {
    int size;
    char *new_packet;
    pq_list_t *new_item;

    new_item = (pq_list_t *)malloc(sizeof(pq_list_t));
    if(!new_item) {
        perror("malloc");

    }

    size = header->caplen < header->len ? header->caplen : header->len;

    new_packet = (char *)malloc(size);
    if(!new_packet) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    memcpy(new_packet, packet, size);
    new_item->len = size;
    new_item->packet = new_packet;

    pthread_mutex_lock(&pq_list_lock);

    new_item->next = NULL;
    pq_tail->next = new_item;
    pq_tail = new_item;

    pthread_cond_signal(&pq_list_notify);
    pthread_mutex_unlock(&pq_list_lock);
}

static void *pq_loop(void *arg) {
    char *interface = (char*)arg;
    char errbuf[PCAP_ERRBUF_SIZE];
    char filter[4096];
    struct bpf_program fp;

    pq_pcap_handle = pcap_open_live(interface, 1500, 1, 1000, errbuf);
    if(pq_pcap_handle == NULL) {
        fprintf(stderr,"Can't open pcap: %s", errbuf);
        exit(EXIT_FAILURE);
    }

    sprintf(filter, "(not (ether src %02x:%02x:%02x:%02x:%02x:%02x)) and "
            "((ether dst ff:ff:ff:ff:ff:ff) or (ether dst %02x:%02x:%02x:%02x:%02x:%02x))",
            mymac[0], mymac[1], mymac[2], mymac[3], mymac[4], mymac[5],
            mymac[0], mymac[1], mymac[2], mymac[3], mymac[4], mymac[5]);

    if(pcap_compile(pq_pcap_handle, &fp, filter, 0, PCAP_NETMASK_UNKNOWN) == -1) {
        fprintf(stderr, "Could not parse filter %s: %s", filter, errbuf);
        exit(EXIT_FAILURE);
    }

    pcap_setfilter(pq_pcap_handle, &fp);

    fprintf(stderr, "Using filter: %s\n", filter);

    pcap_loop(pq_pcap_handle, 0, pq_callback, NULL);

    pcap_close(pq_pcap_handle);
    exit(EXIT_SUCCESS);

    return NULL;
}

void pq_inject(char *packet, int len) {
    pcap_inject(pq_pcap_handle, packet, len);
}

void pq_init(char *interface) {
    int counter=0;

    pq_tail = &pq_list;

    pthread_create(&pq_tid, NULL, pq_loop, interface);

    while((counter < 10) && (!pq_pcap_handle)) {
        counter++;
        sleep(1);
    }

    if(counter == 10) {
        fprintf(stderr,"handle not initializing....\n");
        exit(EXIT_FAILURE);
    }
}

int pq_packet_available(void) {
    if(pq_list.next)
        return 1;
    return 0;
}

int pq_fetch(char **packet, int *len) {
    pq_list_t *item;

    pthread_mutex_lock(&pq_list_lock);

    item = pq_list.next;
    if(!item) {
        fprintf(stderr, "Oops... can't get next packet\n");
        exit(EXIT_FAILURE);
    }

    pq_list.next = item->next;
    if(pq_list.next == NULL)
        pq_tail = &pq_list;

    pthread_mutex_unlock(&pq_list_lock);

    *packet = item->packet;
    *len = item->len;

    free(item);
    return 1;
}

void pq_exit(void) {
}

void pq_packet_wait(void) {
    pthread_mutex_lock(&pq_list_lock);
    while(pq_list.next == NULL) {
        pthread_cond_wait(&pq_list_notify, &pq_list_lock);
    }
    pthread_mutex_unlock(&pq_list_lock);
}
