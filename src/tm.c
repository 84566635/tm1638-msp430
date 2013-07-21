#include "tm.h"

const unsigned char num[] = {0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111};

inline void tm_send(unsigned char data) {
    UCB0TXBUF = data;
    while(!(UCB0STAT & UCBUSY)); //busywaiting!
}

inline unsigned char tm_recv() {
    UCB0TXBUF = 0xFF;
    while(!(UCB0STAT & UCBUSY)); //busywaiting!
    return UCB0RXBUF;
}

void tm_cmd(unsigned char data) {
    TM_POUT &= ~TM_STB;
    tm_send(data);
    TM_POUT |= TM_STB;
}

void tm_data(unsigned char data, unsigned char addr) {
    tm_cmd(0x44);
    TM_POUT &= ~TM_STB;
    tm_send(0xC0 | addr);
    tm_send(data);
    TM_POUT |= TM_STB;
}

static void tm_clear() {
    unsigned char i;
    TM_POUT &= ~TM_STB;
    tm_send(0xC0);
    for(i = 0; i < 16; i++) {
        tm_send(0x00);
    }
    TM_POUT |= TM_STB;
}

void tm_init() {
    UCB0CTL1 |= UCSWRST;
    
    UCB0CTL0 = UCMST | UCSYNC;
    UCB0CTL1 = UCSSEL_2 | UCSWRST; // Still in reset while configuring
    
    IE2 &= ~(UCB0TXIE | UCB0RXIE);
    IFG2 &= ~(UCB0TXIFG | UCB0RXIFG);
    
    TM_PDIR |= TM_MOSI | TM_CLK | TM_STB;
    TM_PDIR &= ~TM_MISO;
    TM_POUT |= TM_STB;
    
    UCB0BR0 = 16; // SMCLK/16 = 1mhz
    UCB0BR1 = 0;
    
    UCB0STAT = 0;
    
    UCB0CTL1 &= ~UCSWRST; // OK Enable USCI_B0
    
    tm_cmd(0x40);
    tm_cmd(0x88);
    
    tm_clear();
}

unsigned char tm_getButtons() {
    unsigned char i, temp = 0;
    TM_POUT &= ~TM_STB;
    tm_send(0x42);
    for(i=0; i < 4; i++) {
        temp |= tm_recv() << i;
    }
    TM_POUT |= TM_STB;
    return temp;
}

void tm_setLed(unsigned char color, unsigned char idx) {
    tm_data(color, (idx<<1)+1);
}

void tm_setDigit(unsigned char digit, unsigned char idx) {
    tm_data(num[digit], (idx<<1));
}


void tm_testDots() {
    unsigned char i;
    for(i = 0; i < 8; i++) {
        tm_data(0x80u, i<<1);
    }
}

void tm_testDigits() {
    unsigned char i;
    for(i = 0; i < 8; i++) {
        tm_setDigit(i, i);
    }
}

void tm_testLeds() {
    unsigned char i;
    for(i = 0; i < 4; i++) {
        tm_setLed(TM_RED, (i<<1)+1);
        tm_setLed(TM_GREEN, (i<<1));
    }
}

void tm_allOn() {
    unsigned char i;
    for(i = 0; i < 8; i++) {
        tm_setLed(TM_RED | TM_GREEN, i);
        tm_data(0xFFu, i<<1);
    }
}