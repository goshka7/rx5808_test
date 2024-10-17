#include "stm8s.h"
#include "RX5808.h"
#include "main.h"

static void rx5808_send(uint32_t data);
/*=====================================================*/
uint16_t RX5808_SetFreq(uint16_t FreqMHz)
{
	uint32_t tmp = (FreqMHz - 479)/2;
        uint32_t v;
        v= ((tmp/32)<<7)|(tmp%32);
        v =  (0x01 | (1<<4)) | ((v & 0x000FFFFF)<<5);
	rx5808_send(v);
	return (uint16_t)(tmp * 2 + 479);
}
/*=====================================================*/
static void rx5808_send(uint32_t data)
{
  uint8_t i, v;
  GPIO_WriteLow(SPI_NCS_PORT, SPI_NCS_PIN);
#ifdef HARDWARE_SPI  
  for(i=0; i < 4; i++, data>>=8 )
  {
      v= data & 0x00ff;
      while(SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET)
        continue;
      SPI->DR = v;
  }
#else
  for(i=0; i < 25; i++ )
  {
     v= 0x01;
     v<<=i;
     if(data & v)
        GPIO_WriteHigh(SPI_GPIO_PORT, SPI_MOSI_PIN);
     else
        GPIO_WriteLow(SPI_GPIO_PORT, SPI_MOSI_PIN);
     GPIO_WriteHigh(SPI_GPIO_PORT, SPI_SCK_PIN);
     ;
     ;
     ;
     GPIO_WriteLow(SPI_GPIO_PORT, SPI_SCK_PIN);
     
  }  
#endif  
  GPIO_WriteHigh(SPI_NCS_PORT, SPI_NCS_PIN);
}
/*=====================================================*/

