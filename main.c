// PIC16F877A Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#define _XTAL_FREQ 20000000
#define SBIT_TXEN     5
#define SBIT_SPEN     7
#define SBIT_CREN     4

void UART_Init(int baudRate);
void UART_TxChar(char ch);
char UART_RxChar();
void UART_TxString(char *str);
void UART_RxString(char *str);


void write_eeprom(unsigned char ,unsigned char);
unsigned char read_eeprom(unsigned char );


void UART_Init(int baudRate)
{    
    TRISC=0x80;            // Configure Rx pin as input and Tx as output  
    TXSTA=(1<<SBIT_TXEN);  // Asynchronous mode, 8-bit data & enable transmitter
    RCSTA=(1<<SBIT_SPEN) | (1<<SBIT_CREN);  // Enable Serial Port and 8-bit continuous receive
    SPBRG = (20000000UL/(long)(64UL*baudRate))-1;      // baud rate @20Mhz Clock
}


void UART_TxChar(char ch)
{
    while(TXIF==0);    // Wait till the transmitter register becomes empty   PIR1
    TXIF=0;            // Clear transmitter flag
    TXREG=ch;          // load the char to be transmitted into transmit reg
}

char UART_RxChar()
{
    while(RCIF==0);    // Wait till the data is received 
    RCIF=0;            // Clear receiver flag
    return(RCREG);     // Return the received data to calling function
}

void UART_TxString(char *str){
    for(int i=0;str[i];i++){
        UART_TxChar(str[i]);
    }
}

void UART_RxString(char *str){
    char ch = UART_RxChar();
    int i = 0;
    while(ch != '\r'){
        UART_TxChar(ch);
        str[i++] = ch;
        ch = UART_RxChar();
    }
    str[i] = '\0';
}
void write_eeprom(unsigned char address,unsigned char data)
{
    unsigned char status_gie;
    EEADR=address;
    EEDATA=data;
   WREN=1;
   status_gie=GIE;
   GIE=0;    
   EECON2=0x55;
   EECON2=0xAA;
   WR=1;
   GIE=status_gie;
    WREN=0;
       
}
unsigned char read_eeprom(unsigned char Address)
{
    unsigned char data;
    EEADR = Address;
    EEPGD=0;
    RD=1;
    data = EEDATA;
    return (data);  
}

void main(void) {
  while(1){
     
   //only internal 256 bytes eeprom
   char op,op2,ch;//
    int address;
   
    UART_Init(2400);//
    write_eeprom(0,0);
    while(1) {
        UART_TxString("Please select the option\n\r");
        UART_TxString("1:Write to EEPROM\n\r");
        UART_TxString("2:Read From EEPROM\n\r");
        UART_TxString("3:Clear the EEPROM\n\r");
        op = UART_RxChar();//reads and gets stored in op
        UART_TxChar(op);//prints o echo
    ss: op2=UART_RxChar();//reads and op2
        UART_TxChar(op2);//
    if(op2 == '\r'){
        switch(op){
            case '1':
                UART_TxString("Write the data to be stored\n\r");
                write_eeprom(0,1);
                address=1;
                ch=UART_RxChar();
                while(ch!='\r')
                {
                    UART_TxChar(ch);
                    write_eeprom(address++,ch);
                    ch=UART_RxChar();
                }
                write_eeprom(address,'\0');
                
                break;
            case '2':
                UART_TxString("The data on the EEPROM\n\r");
                if(read_eeprom(0)==0){
                   UART_TxString("The EEPROM is empty\n\r"); 
                }
                else{
                    for(int i=1;read_eeprom(i)!='\0';i++)
                    {
                       UART_TxChar(read_eeprom(i));
                    }
                    }
                   break; 
            case '3':
                UART_TxString("EEPROM data is cleared\n\r");
                write_eeprom(0,0);
                break;
                
            default:
                UART_TxString("wrong option  is selected\n\r");
        }
        }
    
        else{
            goto ss;
        }
        UART_TxString("\n\r");
}
    return;
}
}