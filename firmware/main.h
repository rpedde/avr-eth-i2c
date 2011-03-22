#ifndef _MAIN_H_
#define _MAIN_H_

extern uint8_t mymac[6];
extern uint8_t myip[4];

#define BUFFER_SIZE 590
extern uint8_t buf[BUFFER_SIZE + 1];

#ifndef DEBUG
#define dprintf(...)
#endif

#endif /* _MAIN_H_ */
