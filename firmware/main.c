/*********************************************
 * vim:sw=8:ts=8:si:et
 * To use the above modeline in vim you must have "set modeline" in your .vimrc
 * Author: Guido Socher
 * Copyright: GPL V2
 *
 * Ethernet remote device and sensor: test program 1
 *
 * Title: Microchip ENC28J60 Ethernet Interface Driver
 * Chip type           : ATMEGA88 with ENC28J60
 *********************************************/
#include <avr/io.h>
#include "enc28j60.h"
#include "timeout.h"
#include "avr_compat.h"
#include "net.h"
#include "dhcp.h"
#include "main.h"
#include "arp.h"
#include "icmp.h"

// please modify the following two lines. mac and ip have to be unique
// in your local area network. You can not have the same numbers in
// two devices:
uint8_t mymac[6] = {0x54,0x55,0x58,0x10,0x00,0x24};
uint8_t myip[4] = {10,253,1,100};
uint8_t buf[BUFFER_SIZE+1];

int main(void){
    uint16_t plen;

    DDRC |= 1U << 3;
    /*         DDRB|= (1<<DDB1); */

    /* set output to Vcc, LED off */

    PORTC |= 1U << 3;
    delay_ms(20);
    PORTC &= ~(1U << 3);
    delay_ms(20);


    // set the clock speed to 8MHz
    // set the clock prescaler. First write CLKPCE to enable setting of clock the
    // next four instructions.
    /*         CLKPR=(1<<CLKPCE); */
    /*         CLKPR=0; // 8 MHZ */

    /* enable PB0, reset as output */
    DDRB|= (1<<DDB0);

    /* set output to gnd, reset the ethernet chip */
    PORTB &= ~(1<<PB0);
    delay_ms(20);
    /* set output to Vcc, reset inactive */
    PORTB|= (1<<PB0);
    delay_ms(100);

    PORTC |= 1U << 3;
    delay_ms(20);
    PORTC &= ~(1U << 3);
    delay_ms(20);

    // LED
    /* enable PB1, LED as output */

    /*initialize enc28j60*/
    enc28j60Init(mymac);

    /* Magjack leds configuration, see enc28j60 datasheet, page 11 */
    // LEDA=greed LEDB=yellow
    //
    // 0x880 is PHLCON LEDB=on, LEDA=on
    // enc28j60PhyWrite(PHLCON,0b0000 1000 1000 00 00);
    enc28j60PhyWrite(PHLCON,0x880);
    delay_ms(500);
    //
    // 0x990 is PHLCON LEDB=off, LEDA=off
    // enc28j60PhyWrite(PHLCON,0b0000 1001 1001 00 00);
    enc28j60PhyWrite(PHLCON,0x990);
    delay_ms(500);
    //
    // 0x880 is PHLCON LEDB=on, LEDA=on
    // enc28j60PhyWrite(PHLCON,0b0000 1000 1000 00 00);
    enc28j60PhyWrite(PHLCON,0x880);
    delay_ms(500);
    //
    // 0x990 is PHLCON LEDB=off, LEDA=off
    // enc28j60PhyWrite(PHLCON,0b0000 1001 1001 00 00);
    enc28j60PhyWrite(PHLCON,0x990);
    delay_ms(500);
    //
    // 0x476 is PHLCON LEDA=links status, LEDB=receive/transmit
    // enc28j60PhyWrite(PHLCON,0b0000 0100 0111 01 10);
    enc28j60PhyWrite(PHLCON,0x476);
    delay_ms(100);

    PORTC &= ~(1<<PC3);

    delay_ms(1000);
    dhcp_init();

    while(1){
        // get the next new packet:
        if((plen = enc28j60PacketReceive(BUFFER_SIZE, buf))) {
            if(dhcp_process_packet(buf, plen))
                continue;

            if(arp_process_packet(buf, plen))
                continue;

            if(icmp_process_packet(buf, plen)) {
                if (PORTC & (1 << PC3))
                    PORTC &= ~(1 << PC3);
                else
                    PORTC |= (1 << PC3);

                continue;
            }

        } else {
            //dhcp_tick_seconds();
        }
    }
    return (0);
}
