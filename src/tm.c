#include "tm.h"

const unsigned char num[] = {0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111};

void tm_send(unsigned char data) {
    char i;
    for(i = 0; i < 8; i++) {
        TM_POUT &= ~TM_CLK; //low
        TM_POUT = (TM_POUT & ~TM_DIO) | ((data & 1)?TM_DIO:0);
        TM_POUT |= TM_CLK; // high
        data >>= 1;
    }
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

void tm_init() {
    char i;
    TM_PDIR |= TM_DIO | TM_CLK | TM_STB;
    TM_POUT |= TM_CLK | TM_STB;
    
    tm_cmd(0x40);
    tm_cmd(0x88);
    
    TM_POUT &= ~TM_STB;
    tm_send(0xC0);
    for(i = 0; i < 16; i++) {
        tm_send(0x00);
    }
    TM_POUT |= TM_STB;
    
}

unsigned char tm_recv() {
    unsigned char temp = 0;
    char i;
    TM_PDIR &= ~TM_DIO;
    TM_PREN |= TM_DIO;
    
    for(i = 0; i < 8; i++) {
        temp >>= 1;
        TM_POUT &= ~TM_CLK;
        temp |= (TM_PIN & TM_DIO)?0x80u:0;
        TM_POUT |= TM_CLK;
    }
    
    TM_PREN &= ~TM_DIO;
    TM_PDIR |= TM_DIO;
    
    return temp;
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