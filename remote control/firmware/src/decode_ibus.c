#include "decode_ibus.h"

void get_channels(uint8_t *message, int *channels)
{
    int sum=0;
    int i=2;
    for(int a=0;a<30;a++)
    {
        sum+=*(message+a);    
    }
    int c1 = *message;
    int c2 = *(message+1);
    c1+=0;
    c2+=0;
    if((0xFFFF-sum) == *(message+31)+((*(message+32))<<8))
    {
        if(*message==0x20)
        {
            while(i<30)
            {
                message=message+i;
                *channels=*message+(*(message+1)<<8);
                channels=channels+1;
                i=i+2; 
            }
        }
        
        else
        {
            *channels=0;
        }  
    }     
}