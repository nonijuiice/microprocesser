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
volatile char num9[10] = { 0x55,0xAA,0x04,0x03,0x0D,0x00,0x01,0x00,0x0B,0xF5 }; // MP3 �뷡��� ���ڿ� 
volatile char okay[10] = { 0x55,0xAA,0x04,0x03,0x0D,0x00,0x01,0x00,0x0C,0xF5 }; // Ȯ�� ���ڿ�
volatile char usb[6] = { 0x55,0xAA,0x01,0x01,0x02,0x01 }; // MP3 ����غ� ���ڿ�
volatile char floor[11] = { 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0xFF }; // ���� ���� ���ڿ�
volatile char test[10] = { 0x6A,0x6B,0x6C,0x6D,0x6F,0x70,0x71,0x72,0x74,0x75 };
volatile int ready = 1; // MP3 �غ� Ȯ�� ����
volatile int cnt1 = 0; // MP3 �غ� ���ڿ� �۽� ī��Ʈ ����
volatile int cnt2 = 0; // MP3 �뷡 ��� ���ڿ� �۽� ī��Ʈ ����
volatile int push = 1; // Ű�е� �Է� �÷���
volatile int number = 0;	// Ű�е� �Է� ����
volatile int x = 0;	// ������ ������ ����
int ready_mp3(int condition) // MP3 �غ� ���ڿ� �۽� �Լ�
{
    if (condition == 1) // �Է� ������ 1�� ��
    {
        for (cnt1 = 0; cnt1 < 6; cnt1++) // ���ڿ� 0���� 5����
        {
            while ((UCSR0A & (1 << UDRE0)) == 0); // UDR0�� ����� ��
            UDR0 = usb[cnt1]; // ���� ����
        }
    }
    return 0; // 0�� ��ȯ, ��ȯ�����ν� ready ������ 0���� �缳��
}

int play_music(int condition) // MP3���� �뷡�� ����ϴ� �Լ�
{
    if (condition == 1) // �Է°��� 1�� ��
    {
        for (cnt2 = 0; cnt2 < 10; cnt2++) // ���ڿ� 0~9����
        {
            while ((UCSR0A & (1 << UDRE0)) == 0); // UDR0�� ����� �� ���
            if (number == 0) UDR0 = num0[cnt2]; // Ű�е� �Է� ���ڿ� ���� �۽��� ���ڿ� ����
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
        return 0; // 0�� ��ȯ�����ν� push ������ 0���� �缳��
    }
}

void tx_floor(int n) // �ٸ� ��Ʈ�ް��� ������ �۽��ϴ� �Լ�
{
    while ((UCSR1A & (1 << UDRE1)) == 0); // UDR1�� ����� �� ���
    UDR1 = floor[n]; // ������ �ڸ������� �ٸ� ��Ʈ�ް��� �۽�
}

void tx_end(int condition) // ������ �� �������� �˸��� ��ȣ�� �۽��ϴ� �Լ�
{
    if (condition == 1) // �Է°��� 1�� ��
    {
        while ((UCSR1A & (1 << UDRE1)) == 0); // UDR1�� ����� �� ���
        UDR1 = floor[10]; // ���ᰪ�� 0XFF���� �ٸ� ��Ʈ�ް��� �۽�
    }
}

void setDDRN() // DDRN�� �����ϴ� �Լ�
{
    DDRE = 0x02; // (E1)TXD OUTPUT (E0)RXD INPUT
    DDRD = 0x08; // (D3)TXD OUTPUT (D2)RXD x INTPUT INTERRUPT INPUT
    DDRC = 0x0F; // ROW�� �Է�����, Col�� ������� ���
    DDRB = 0xFF; // ���������� ������� ���
}

void setUART() //UART�� �����ϴ� �Լ�
{
    UCSR0C = 0x06; // parity mode ��Ȱ��ȭ + ��ž��Ʈ 1bit + �ۼ��� ������ 8bit
    UCSR0B = 0x08; // ������ �������͸� Ȱ��ȭ + �ۼ��� ������ 8bit
    UBRR0H = 0;
    UBRR0L = 16;   // MP3 baud rate 57600
    UCSR1C = 0x06; // parity mode ��Ȱ��ȭ + ��ž��Ʈ 1bit + �ۼ��� ������ 8bit
    UCSR1B = 0x08; // ������ �������͸� Ȱ��ȭ + �ۼ��� ������ 8bit
    UBRR1H = 0x00;
    UBRR1L = 0x67; // baud rate 9600
}

void setTIMER() //Ÿ�̸Ӹ� �����ϴ� �Լ�
{
    TIMSK = 0x04;	//timer 1 ���
    TCCR1B = 0x05;  //prescale : 1024
    TCNT1H = 0xFF;	//�ʱⰪ
    TCNT1L = 0xC0;
}

char Keypad_input(void)	//Ű�е� �Է��� �޴� �Լ�
{
    PORTC = 0xFF;	//��ü ����� HIGH��
    // pin 0,1,2,3 -> Row, pin 4,5,6 -> Column (�ϵ���� ����)
    DDRC = 0x0F;	// ROW�� �Է�����, Col�� ������� ���
    char input;	    // �Է� ���� Ű�е� (������)

    PORTC = 0b11111110;  //1 ROW�� low�� ��������  ��
    _delay_us(1);
    if ((PINC & 0x10) == 0)	// 1 col�� low�� �Է����� ��
        input = '1';
    else if ((PINC & 0x20) == 0)	// 2 col�� low�� �Է����� ��
        input = '2';
    else if ((PINC & 0x40) == 0)	// 3col�� low�� �Է����� ��
        input = '3';

    PORTC = 0b11111101; 	//2 ROW�� low�� �������� ��
    _delay_us(1);
    if ((PINC & 0x10) == 0)
        input = '4';
    else if ((PINC & 0x20) == 0)
        input = '5';
    else if ((PINC & 0x40) == 0)
        input = '6';
   
    PORTC = 0b11111011; // 3ROW�� low�� �������� ��
    _delay_us(1);
    if ((PINC & 0x10) == 0)
        input = '7';
    else if ((PINC & 0x20) == 0)
        input = '8';
    else if ((PINC & 0x40) == 0)
        input = '9';
   
    PORTC = 0b11110111; // 4ROW�� low�� �������� ��
    _delay_us(1);
    if ((PINC & 0x10) == 0)
        input = 'o';	// *�� �ش��ϴ� Ű�е��Է�
    if ((PINC & 0x20) == 0)
        input = '0';
    else if ((PINC & 0x40) == 0)
        input = 'G';	// #�� �ش��ϴ� Ű�е� �Է�
    return input;	// �������� Ű�е� �Է� ��ȯ
}

void vibration(int n) {	// �������� �Լ�
    switch (n) {

    case 1:	// 1�� �Է¿� ������ 1�� ����
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

ISR(TIMER1_OVF_vect)    // Ű�е� �Է��� Ÿ�̸Ӹ� ����ؼ� ���
{
    cli();
    TCNT1H = 0xC2;
    TCNT1L = 0xF6; // 1sec
    ready = ready_mp3(ready); // MP3�� ����غ��Ű�� ready ������ 0���� �缳��
    if (Keypad_input() != 'o' & Keypad_input() != 'G') {	// Ű�е� 0~9�� ���ڸ� �Է� ����
        number = (int)Keypad_input() - 48;	// �������� �Ǽ������� ��ȯ
        x = number;	// �Է¹��� Ű�е� ���ڸ� x�� ����
        vibration(number);	// �Է¹��� ���ڸ�ŭ ����
    }
    else if (Keypad_input() == 'o') {	// Ű�е忡 '*(o)' �Է½� 
        tx_floor(x);	// uart�� ��ġ ����
        play_music(1);	// MP3���� �뷡�� �ѹ� ���
    }
    else if (Keypad_input() == 'G') { // Ű�е忡 '#(G)' �Է½� �Է� ���� ���� �ʱ�ȭ
        tx_end(1);	// ���� �۽��� �������� �˸��� �����ȣ�� �ٸ� ��Ʈ�ް��� �۽�
    }
    sei();
}
