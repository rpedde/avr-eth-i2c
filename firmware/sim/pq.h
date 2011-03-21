/*
 *
 */

#ifndef _PQ_H_
#define _PQ_H_

extern void pq_init(char *interface);
extern int pq_packet_available(void);
extern int pq_fetch(char **packet, int *len);
extern void pq_exit(void);
extern void pq_inject(char *packet, int len);
extern void pq_packet_wait(void);

#endif /* _PQ_H_ */
