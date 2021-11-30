#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

volatile char num0[10] = { 0x55,0xAA,0x04,0x03,0x0D,0x00,0x01,0x00,0x01,0xF5 };
volatile char num1[10] = { 0x55,0xAA,0x04,0x03,0x0D,0x00,0x01,0x00,0x02,0xF5 };
volatile char num2[10] = { 0x55,0xAA,0x04,0x03,0x0D,0x00,0x01,0x00,0x03,0xF5 };
volatile char num3[10] = { 0x55,0xAA,0x04,0x03,0x0D,0x00,0x01,0x00,0x05,0xF5 };
volatile char num4[10] = { 0x55,0xAA,0x04,0x03,0x0D,0x00,0x01,0x00,0x06,0xF5 };
volatile char num5[10] = { 0x55,0xAA,0x04,0x03,0x0D,0x00,0x01,0x00,0x07,0xF5 };
volatile char num6[10] = { 0x55,0xAA,0x04,0x03,0x0D,0x00,0x01,0x00,0x08,0xF5 };
volatile char num7[10] = { 0x55,0xAA,0x04,0x03,0x0D,0x00,0x01,0x00,0x09,0xF5 };
volatile char num8[10] = { 0x55,0xAA,0x04,0x03,0x0D,0x00,0x01,0x00,0x0A,0xF5 };
volatile char num9[10] = { 0x55,0xAA,0x04,0x03,0x0D,0x00,0x01,0x00,0x0B,0xF5 }; // MP3 노래재생 문자열 
volatile char okay[10] = { 0x55,0xAA,0x04,0x03,0x0D,0x00,0x01,0x00,0x0C,0xF5 }; // 확인 문자열
volatile char usb[6] = { 0x55,0xAA,0x01,0x01,0x02,0x01 }; // MP3 재생준비 문자열
volatile char floor[11] = { 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0xFF }; // 층수 전달 문자열
volatile char test[10] = { 0x6A,0x6B,0x6C,0x6D,0x6F,0x70,0x71,0x72,0x74,0x75 };
volatile int ready = 1; // MP3 준비 확인 변수
volatile int cnt1 = 0; // MP3 준비 문자열 송신 카운트 변수
volatile int cnt2 = 0; // MP3 노래 재생 문자열 송신 카운트 변수
volatile int push = 1; // 키패드 입력 플래그
volatile int number = 0;	// 키패드 입력 숫자
volatile int x = 0;	// 가려는 층수의 변수
int ready_mp3(int condition) // MP3 준비 문자열 송신 함수
{
    if (condition == 1) // 입력 변수가 1일 때
    {
        for (cnt1 = 0; cnt1 < 6; cnt1++) // 문자열 0에서 5까지
        {
            while ((UCSR0A & (1 << UDRE0)) == 0); // UDR0가 비었을 때
            UDR0 = usb[cnt1]; // 값을 전송
        }
    }
    return 0; // 0을 반환, 반환함으로써 ready 변수를 0으로 재설정
}

int play_music(int condition) // MP3에서 노래를 재생하는 함수
{
    if (condition == 1) // 입력값이 1일 때
    {
        for (cnt2 = 0; cnt2 < 10; cnt2++) // 문자열 0~9까지
        {
            while ((UCSR0A & (1 << UDRE0)) == 0); // UDR0가 비었을 때 통과
            if (number == 0) UDR0 = num0[cnt2]; // 키패드 입력 숫자에 따라 송신할 문자열 선택
            else if (number == 1) { UDR0 = num1[cnt2]; }
            else if (number == 2) { UDR0 = num2[cnt2]; }
            else if (number == 3) { UDR0 = num3[cnt2]; }
            else if (number == 4) { UDR0 = num4[cnt2]; }
            else if (number == 5) { UDR0 = num5[cnt2]; }
            else if (number == 6) { UDR0 = num6[cnt2]; }
            else if (number == 7) { UDR0 = num7[cnt2]; }
            else if (number == 8) { UDR0 = num8[cnt2]; }
            else if (number == 9) { UDR0 = num9[cnt2]; }
        }
        return 0; // 0을 변환함으로써 push 변수를 0으로 재설정
    }
}

void tx_floor(int n) // 다른 아트메가로 층수를 송신하는 함수
{
    while ((UCSR1A & (1 << UDRE1)) == 0); // UDR1이 비었을 때 통과
    UDR1 = floor[n]; // 층수를 자리수별로 다른 아트메가로 송신
}

void tx_end(int condition) // 층수를 다 보냈음을 알리는 신호를 송신하는 함수
{
    if (condition == 1) // 입력값이 1일 때
    {
        while ((UCSR1A & (1 << UDRE1)) == 0); // UDR1이 비었을 때 통과
        UDR1 = floor[10]; // 종료값인 0XFF값을 다른 아트메가로 송신
    }
}

void setDDRN() // DDRN을 설정하는 함수
{
    DDRE = 0x02; // (E1)TXD OUTPUT (E0)RXD INPUT
    DDRD = 0x08; // (D3)TXD OUTPUT (D2)RXD x INTPUT INTERRUPT INPUT
    DDRC = 0x0F; // ROW을 입력으로, Col을 출력으로 사용
    DDRB = 0xFF; // 진동센서를 출력으로 사용
}

void setUART() //UART를 설정하는 함수
{
    UCSR0C = 0x06; // parity mode 비활성화 + 스탑비트 1bit + 송수신 데이터 8bit
    UCSR0B = 0x08; // 보내는 레지스터를 활성화 + 송수신 데이터 8bit
    UBRR0H = 0;
    UBRR0L = 16;   // MP3 baud rate 57600
    UCSR1C = 0x06; // parity mode 비활성화 + 스탑비트 1bit + 송수신 데이터 8bit
    UCSR1B = 0x08; // 보내는 레지스터를 활성화 + 송수신 데이터 8bit
    UBRR1H = 0x00;
    UBRR1L = 0x67; // baud rate 9600
}

void setTIMER() //타이머를 설정하는 함수
{
    TIMSK = 0x04;	//timer 1 사용
    TCCR1B = 0x05;  //prescale : 1024
    TCNT1H = 0xFF;	//초기값
    TCNT1L = 0xC0;
}

char Keypad_input(void)	//키패드 입력을 받는 함수
{
    PORTC = 0xFF;	//전체 출력을 HIGH로
    // pin 0,1,2,3 -> Row, pin 4,5,6 -> Column (하드웨어 연결)
    DDRC = 0x0F;	// ROW을 입력으로, Col을 출력으로 사용
    char input;	    // 입력 받은 키패드 (문자형)

    PORTC = 0b11111110;  //1 ROW을 low로 선택했을  때
    _delay_us(1);
    if ((PINC & 0x10) == 0)	// 1 col을 low로 입력했을 때
        input = '1';
    else if ((PINC & 0x20) == 0)	// 2 col을 low로 입력했을 때
        input = '2';
    else if ((PINC & 0x40) == 0)	// 3col을 low로 입력했을 때
        input = '3';

    PORTC = 0b11111101; 	//2 ROW을 low로 선택했을 때
    _delay_us(1);
    if ((PINC & 0x10) == 0)
        input = '4';
    else if ((PINC & 0x20) == 0)
        input = '5';
    else if ((PINC & 0x40) == 0)
        input = '6';
   
    PORTC = 0b11111011; // 3ROW을 low로 선택했을 때
    _delay_us(1);
    if ((PINC & 0x10) == 0)
        input = '7';
    else if ((PINC & 0x20) == 0)
        input = '8';
    else if ((PINC & 0x40) == 0)
        input = '9';
   
    PORTC = 0b11110111; // 4ROW을 low로 선택했을 때
    _delay_us(1);
    if ((PINC & 0x10) == 0)
        input = 'o';	// *에 해당하는 키패드입력
    if ((PINC & 0x20) == 0)
        input = '0';
    else if ((PINC & 0x40) == 0)
        input = 'G';	// #에 해당하는 키패드 입력
    return input;	// 문자형의 키패드 입력 반환
}

void vibration(int n) {	// 진동센서 함수
    switch (n) {

    case 1:	// 1이 입력에 들어오면 1번 진동
        for (int i = 0; i < 1; i++) {
            PORTB = 0xFF;
            _delay_ms(100);
            PORTB = 0x00;
            _delay_ms(200);
        }
        break;

    case 2:
        for (int i = 0; i < 2; i++) {
            PORTB = 0xFF;
            _delay_ms(100);
            PORTB = 0x00;
            _delay_ms(200);
        }
        break;

    case 3:
        for (int i = 0; i < 3; i++) {
            PORTB = 0xFF;
            _delay_ms(100);
            PORTB = 0x00;
            _delay_ms(200);
        }
        break;

    case 4:
        for (int i = 0; i < 4; i++) {
            PORTB = 0xFF;
            _delay_ms(100);
            PORTB = 0x00;
            _delay_ms(200);
        }
        break;

    case 5:
        for (int i = 0; i < 5; i++) {
            PORTB = 0xFF;
            _delay_ms(100);
            PORTB = 0x00;
            _delay_ms(200);
        }
        break;

    case 6:
        for (int i = 0; i < 6; i++) {
            PORTB = 0xFF;
            _delay_ms(100);
            PORTB = 0x00;
            _delay_ms(200);
        }
        break;

    case 7:
        for (int i = 0; i < 7; i++) {
            PORTB = 0xFF;
            _delay_ms(100);
            PORTB = 0x00;
            _delay_ms(200);
        }
        break;

    case 8:
        for (int i = 0; i < 8; i++) {
            PORTB = 0xFF;
            _delay_ms(100);
            PORTB = 0x00;
            _delay_ms(200);
        }
        break;

    case 9:
        for (int i = 0; i < 9; i++) {
            PORTB = 0xFF;
            _delay_ms(100);
            PORTB = 0x00;
            _delay_ms(200);
        }
        break;

    case 0:
        for (int i = 0; i < 10; i++) {
            PORTB = 0xFF;
            _delay_ms(100);
            PORTB = 0x00;
            _delay_ms(200);
        }
        break;
    }
    _delay_ms(800);
}
int main()
{
    cli();
    setDDRN();
    setUART();
    setTIMER();
    sei();
    do {
    } while (1);
}

ISR(TIMER1_OVF_vect)    // 키패드 입력을 타이머를 사용해서 사용
{
    cli();
    TCNT1H = 0xC2;
    TCNT1L = 0xF6; // 1sec
    ready = ready_mp3(ready); // MP3를 재생준비시키고 ready 변수를 0으로 재설정
    if (Keypad_input() != 'o' & Keypad_input() != 'G') {	// 키패드 0~9의 숫자만 입력 받음
        number = (int)Keypad_input() - 48;	// 문자형을 실수형으로 변환
        x = number;	// 입력받은 키패드 숫자를 x에 저장
        vibration(number);	// 입력받은 숫자만큼 진동
    }
    else if (Keypad_input() == 'o') {	// 키패드에 '*(o)' 입력시 
        tx_floor(x);	// uart로 수치 전송
        play_music(1);	// MP3에서 노래를 한번 재생
    }
    else if (Keypad_input() == 'G') { // 키패드에 '#(G)' 입력시 입력 받은 층수 초기화
        tx_end(1);	// 층수 송신이 끝났음을 알리는 종결신호를 다른 아트메가로 송신
    }
    sei();
}
