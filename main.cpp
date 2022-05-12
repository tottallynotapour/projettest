#include "mbed.h"
#include "ultrasonic.h"

#define ARRIERE_TX PA_0 //PA_2     //Bluetooth 3 à gauche PCB, communication entre module arriere et module avant
#define ARRIERE_RX PA_1 //PA_3
#define ROUE_TX PA_9        //Bluetooth 2, à droite PCB, communication entre module roue et module arriere
#define ROUE_RX PA_10
//#define TEL_TX PB_10        //Bluetooth 5, au milieu PCB, communication entre module arrière et telephone
//#define TEL_RX PB_11

static BufferedSerial bt_roue(ROUE_TX, ROUE_RX, 38400);
//static BufferedSerial bt_tel(TEL_TX, TEL_RX, 9600);
static BufferedSerial bt_arriere(ARRIERE_TX, ARRIERE_RX, 38400);

DigitalOut OG (PA_4);//(PB_5);       //Clignotant Orange Gauche
DigitalOut rouge (PB_9);//(PB_4);    //Clignotant Rouge
DigitalOut OD (PB_8); //(PB_3);       //Clignotant Orange Droite
DigitalOut DEL_vert (PC_10);

void reed(void)
{
    
    if (bt_roue.readable()==1) 
        {
            
            char buf2[16]={};
            ThisThread::sleep_for(50ms);
            bt_roue.read(buf2, sizeof(buf2)); 
            bt_arriere.write(buf2,sizeof(buf2));
            printf("%s", buf2);
        }
}
void clignotant(void)       //Controle de clignotants
{
    char buf[1];
    ThisThread::sleep_for(10ms);
    bt_arriere.read(buf, sizeof(buf));
    //printf("%s", buf);
    if(buf[0] == 'G')
        {
            OG = 1;
            OD = 0;
        }
    else if(buf[0] == 'D')
    {
        OG = 0;
        OD = 1;
    
    }  
    else
    {
        OD = 0;
        OG = 0;
    }
}
void LED_demarrage(void)
{
        DEL_vert = 1;
        ThisThread::sleep_for(3s);
        DEL_vert= 0;
}
void dist(int distance)     //À 3 mètres, clignotant rouge sur module avant et arriere
{
    
    if(distance <= 600)
    {
        rouge = 1;
        char msg2[] = {"a a a a a"};
        bt_arriere.write(msg2, sizeof(msg2));
    } 
    else 
    {
        rouge = 0;
    }
   
}
ultrasonic mu(PB_5, PB_3, 100ms, 1s, &dist);  //Set the trigger pin to D8 and the echo pin to D9
                                                //have updates every .1 seconds and a timeout after 1
                                                //second, and call dist when the distance changes

void distance_interrupt(void) 
{
    mu.checkDistance(); //call checkDistance() as much as possible, as this is where the class checks if dist needs to be called.  
}
Ticker distanceTicker;

int main()
{
    //bt_roue.set_blocking(false);
    bt_arriere.set_blocking(false);

    LED_demarrage();        //LED blinking power up
    mu.startUpdates();//start mesuring the distance

    distanceTicker.attach(distance_interrupt, 100ms);     
    printf("start"); 
  
    while(1)
    {          
        clignotant();  
        reed();
        
    }
}