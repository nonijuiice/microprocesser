#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
volatile double i = 0;
volatile int destination = 1; // 가고자하는 층수
volatile int start_point = 1; // 현재 층수
volatile int number = 0; // 유아트로 들어온 값을 층수로 변환한 값
volatile int cnt = 0; // 유아트로 값이 들어온 횟수

int main(void)
{
	cli();
	DDRB = 0xFF; // A (ena), B (clk) , C (dir), F를 출력으로 사용
	DDRA = 0xFF;
	DDRE = 0x02; // E 1번핀만 출력으로 사용
	DDRC = 0xFF;
	DDRF = 0xFF;
	TCCR1B = 0x05; // prescale = 1024
	TIMSK = 0x04; // 타이머1 사용
	TCNT1H = 0xFF; // 초기값 설정
	TCNT1L = 0xC0;
	TCCR0 = 0x6F; // fast pwm, prescale = 1024
	OCR0 = 125; // 주기 설정
	PORTA = 0x00; // 모터 드라이버 전원 on

	UCSR0C = 0x06; // 비동기화 + parity mode 비활성화 + 스탑비트 1bit + 송수신 
	데이터 8bit
		UCSR0B = 0x18; // 받는 레지스터 활성화 + 보내는 레지스터 활성화 + 송수신 데이
	터 8bit
		UBRR0H = 0x00; // baud rate 9600
	UBRR0L = 0x67;

	sei();

	do {
	} while (1);
}

int USART_Receive()
{
	int ch; // UDR0 값을 받는 변수
	do {

		ch = UDR0; // ch에 UDR0값을 저장
		if (ch == 0x00) { number = 0; } // ch값의 상황에 맞는 층수로 변환
		else if (ch == 0x01) { number = 1; }
		else if (ch == 0x02) { number = 2; }
		else if (ch == 0x03) { number = 3; }
		else if (ch == 0x04) { number = 4; }
		else if (ch == 0x05) { number = 5; }
		else if (ch == 0x06) { number = 6; }
		else if (ch == 0x07) { number = 7; }
		else if (ch == 0x08) { number = 8; }
		else if (ch == 0x09) { number = 9; }
		if (cnt == 0) { destination = number; } // 처음 유아트 수신시 가고자하는 층 = 수신값
		else if (cnt == 1) { // 두번째 유아트 수신시 가고자하는 층
			if (ch == 0xFF) { break; } // 두번째 수신값이 END값이면 탈출
			else { destination = destination * 10 + number; } // 두번째 수신값이 END가 아니면 
			가고자하는 층 계산
		}
		else if (cnt == 2) { break; } // 수시값에 상관 없이 유아트 3번 받으면 탈출
		cnt++; // 유아트 수신횟수 증가
	} while (1);
	return destination; // 최종 가고자 하는 값 반환
}

ISR(TIMER1_OVF_vect) //0.004
{
	cli();
	destination = USART_Receive(); // uart로부터 가고자하는 층수 수신받음
	if (destination > start_point) // 가고자하는 층수가 현재 층수보다 높으면
	{

		TCNT1H = 0xF5; // 타이머 0.17초 설정
		TCNT1L = 0X66;

		PORTB = 0xff; // 모터드라이버 clk 펄스 입력
		PORTC = 0x00; // 모터드라이버 dir(-)로 설정 ( 정방향)

		i = i + 0.5; // 펄스 1개당 1.8도, 한 층 당 3.6도로 설정
		if (i == (destination – start_point)) // i = (가고자하는 층수 – 현재 층수) 이면 
		{
			PORTA = 0x01; // 드라이버 ENA(+) (드라이버 전원off)
			start_point = destination; // 현재층수를 이동한 층수로 최신화
		}
	}

	else if (destination < start_point) // 가고자하는 층수가 현재층수보다 낮으면
	{
		TCNT1H = 0xF5; // 타이머 0.17초로 설정
		TCNT1L = 0X66;

		PORTB = 0xff; // 모터 드라이버 clk 펄스 입력
		PORTC = 0xff; // 모터 드라이버 dir(+)로 설정 (역방향)
		i = i + 0.5;

		if (i == start_point – destination) // i = (가고자하는 층수 – 현재 층수) 이면 
		{
			PORTA = 0x01; // 드라이버 ENA(+) (드라이버 전원 off)
			start_point = destination; // 현재 층수를 가고자하는 층수로 최신화
		}
	}

	else // (현재 층수와 가고자하는 층수가 같으면)
	{
		PORTA = 0x01; //드라이버 ENA(+) (드라이버 전원 off)
	}
	sei();
}