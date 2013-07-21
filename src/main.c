
#include "tm.h"

struct {
    unsigned char h, m, s;
} t;
enum {State_Normal, State_SetTime, State_SetAlarm, State_Adjust} state;

void showTime() {
    tm_setDigit(t.h/10, 2);
    tm_setDigit(t.h%10, 3);
    tm_setDigit(t.m/10, 4);
    tm_setDigit(t.m%10, 5);
    tm_setDigit(t.s/10, 6);
    tm_setDigit(t.s%10, 7);
}

void showTACCR0() {
    unsigned int ccr = TACCR0;
    unsigned int i = 7;
    while (i > 1) {
        tm_setDigit(ccr%10, i);
        ccr/=10;
        --i;
    }
}

int main() {
    unsigned char tmp, i;

    WDTCTL = WDTPW | WDTHOLD;
    DCOCTL = 0;
    BCSCTL1 = CALBC1_16MHZ;
    DCOCTL = CALDCO_16MHZ;
    
    // ACLK and Timer1_A
    BCSCTL3 = XCAP_2; //10pf caps
    
    TACCR0 = 32768u;
    TACTL = TASSEL_1 | ID_0 | MC_1;
    TACCTL0 |= CCIE;
    
    P1DIR |= BIT0;
    P1OUT &= ~BIT0;
    
    t.h = t.m = t.s = 0;
    state = State_Normal;
    
    //tm_init();
    
    _BIS_SR(GIE);
    
    P1OUT |= BIT0;
    
    
    // while(1) {        
//         tmp = tm_getButtons();
//         switch (state) {
//             case State_Normal:
//             tm_data(0, 0);
//             tm_data(0, 2);
//             showTime();
//             if (tmp & BIT0) {
//                 state = State_SetTime;
//             }
//             break;
//             case State_SetTime:
//             tm_data(0b01101101, 0); //S
//             tm_data(0b11111000, 2); //t.
//             showTime();
//             if (tmp & BIT7) t.s = 0;
//             if (tmp & BIT6) t.m = (t.m+1)%60;
//             if (tmp & BIT5) t.h = (t.h+1)%24;
//             if (tmp & BIT0) {
//                 TAR = 0;
//                 state = State_SetAlarm;
//             }
//             break;
//             case State_SetAlarm:
//             tm_data(0b01110111, 0); //A
//             tm_data(0b10111000, 2); //L.
//             if (tmp & BIT0) {
//                 state = State_Adjust;
//             }
//             break;
//             case State_Adjust:
//             tm_data(0b01110111, 0); //A
//             tm_data(0b11011110, 2); //d.
//             showTACCR0();
//             if (tmp & BIT7) ++TACCR0;
//             if (tmp & BIT6) --TACCR0;
//             if (tmp & BIT0) {
//                 state = State_Normal;
//             }
//         }
//         
//         for(i = 0; i < 8; i++) {
//             if(tmp & (1<<i)) {
//                 tm_setLed(TM_RED, i);
//             } else {
//                 tm_setLed(TM_GREEN, i);
//             }
//         }
//         
//         __delay_cycles(1000000);
//     }
    while(1);
    return 0;
}

void
__attribute__((interrupt(TIMER0_A0_VECTOR)))
t0a0isr() {
    P1OUT ^= BIT1;
    TACCTL0 &= ~CCIFG;
    if (state == State_SetTime) return;
    if (++t.s >= 60) {
        t.s = 0;
        if (++t.m >= 60) {
            t.m = 0;
            if (++t.h >= 24) {
                t.h = 0;
            }
        }
    }
}