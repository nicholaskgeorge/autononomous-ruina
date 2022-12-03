#include "decode_ibus.h"
#include "definitions.h"

void get_channels(uint8_t *message, int *channels)
{
    int sum=0;
    int i=2;
    for(int a=0;a<30;a++)
    {
        sum+=*(message+a);    
    }
//    int *ptr = message;
//    int c1 = *(message+30);
//    int c2 = *(message+31);
//    int c3 = *(message+30)+((*(message+31))<<8);
//    USART1_Write(&sum,sizeof(sum));
//    USART1_Write(&c1,sizeof(c1));
//    USART1_Write(&c2,sizeof(c2));
//   USART1_Write(&c3,sizeof(c3));
//    c1+=0;
//    c2+=0;
    
    if((0xFFFF-sum) == *(message+30)+((*(message+31))<<8))
    {
        if(*message==0x20)
        {
            while(i<30)
            {
                *(channels+i/2-1)=(*(message+i))+((*(message+i+1)<<8));
                i=i+2; 

            }
        }
        
        else
        {
            *channels=0;
        }  
    }    
//    for (int j=0;j<14;j++){
//        USART1_Write(&channels[j],sizeof(channels[j]));
//    }
}