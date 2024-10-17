
#include "stm8s.h"
#include "main.h"
#include "RX5808.h"
/*=====================================================*/
#ifdef FREQ_TABLE
static const uint16_t channelFreqTable[CHANNELS_SIZE] = {
    5474, 5492, 5510, 5528, 5546, 5564, 5582, 5600, // A
    5362, 5399, 5436, 5473, 5510, 5547, 5584, 5621, // D b AKK
    5325, 5348, 5366, 5384, 5402, 5420, 5438, 5456, // E
    5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725, // A
    5129, 5159, 5189, 5219, 5249, 5279, 5309, 5339, // F AKK 
    5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866, // B
    5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945, // E
    5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880, // F / Airwave
    5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917, // C / Immersion Raceband
    5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917, // R     
    4990, 5020, 5050, 5080, 5110, 5140, 5170, 5200, // X
    6002, 6028, 6054, 6080, //
};
#endif
__IO uint32_t TimingDelay = 0;
void Delay(__IO uint32_t nTime);
static void CLK_Config(void);
static void GPIO_Config(void);
static void TIM4_Config(void);
static void ADC_Config(void);
void TimingDelay_Decrement(void);
#ifdef HARDWARE_SPI
static void spi_init(void);
#endif
/*=====================================================*/
void main(void)
{
  uint16_t freq;  
  uint8_t j;
  uint16_t rssi;
  uint16_t rssi_sum;
  uint16_t rssi_max;
  uint16_t rssi_max_ch;
  uint8_t button[3];
  CLK_Config();  /*  16 MHz */
  GPIO_Config();
#ifdef HARDWARE_SPI  
  spi_init();
#endif
  ADC_Config();
  TIM4_Config(); 
#ifdef FREQ_TABLE
	freq= 0;
#else
	freq= FREQ_MIN;
#endif
  while (1)
  {
    GPIO_WriteReverse(LED_GPIO_PORT, LED_GPIO_PIN);
#ifdef FREQ_TABLE
    RX5808_SetFreq(channelFreqTable[freq]);
#else
    RX5808_SetFreq(freq);	
#endif	
    Delay(10);      /* Insert 50 ms delay */  
    for(j= 0, rssi_sum= 0; j < 4 ; j++)
    {
          ADC1_StartConversion();
          while(ADC1_GetFlagStatus(ADC1_FLAG_EOC) == RESET)
            continue;
          rssi = ADC1_GetConversionValue();
          rssi_sum+= rssi;
    }
    rssi= rssi_sum>>2;
    if(rssi > rssi_max)
    {
        rssi_max= rssi;	  
        rssi_max_ch= freq;
    }
    ADC1_ClearFlag(ADC1_FLAG_EOC);
#ifdef FREQ_TABLE		
    freq++;
    if(freq >= CHANNELS_SIZE )
    {
      freq = 0;
#else
    freq+=2;      
    if(freq >= FREQ_MAX )
    {
      freq = FREQ_MIN;	
#endif	
      j= GPIO_ReadInputPin(BUITTON_PORT, BUTTON0_PIN);
      if((rssi_max > RSSI_LEVEL) || j) /* stop scan */
      {
#ifdef FREQ_TABLE			  
         RX5808_SetFreq(channelFreqTable[rssi_max_ch]);
#else
         RX5808_SetFreq(rssi_max_ch);
#endif
         while(1)
         {
           button[1]= button[0];
           button[0]= GPIO_ReadInputPin(BUITTON_PORT, BUTTON0_PIN);
           if((button[0] == button[1]) && !button[0] && button[2]))
            {
              button[2]= button[1];
              break;
            }
           continue;
         }
      }
      rssi_max= 0;
      rssi_max_ch= freq;
    }
  }

}
/*=====================================================*/
void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;
  while (TimingDelay != 0);
}
/*=====================================================*/
static void CLK_Config(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
}
/*=====================================================*/
static void GPIO_Config(void)
{
    GPIO_Init(LED_GPIO_PORT, LED_GPIO_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
#ifndef HARDWARE_SPI      
    GPIO_Init(SPI_GPIO_PORT, SPI_SCK_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_Init(SPI_GPIO_PORT, SPI_MOSI_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
#endif    
    GPIO_Init(SPI_NCS_PORT, SPI_NCS_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_Init(BUITTON_PORT, (BUTTON0_PIN | BUTTON1_PIN), GPIO_MODE_IN_PU_NO_IT);
}
/*=====================================================*/
#ifdef HARDWARE_SPI 
static void spi_init(void)
{
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, ENABLE);
  GPIO_Init(SPI_GPIO_PORT,(GPIO_Pin_TypeDef)(SPI_SCK_PIN | SPI_MOSI_PIN), GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(SPI_NCS_PORT,(GPIO_Pin_TypeDef)SPI_NCS_PIN, GPIO_MODE_OUT_PP_LOW_FAST);  
  
  SPI_Init(SPI_FIRSTBIT_LSB, SPI_BAUDRATEPRESCALER_32, SPI_MODE_MASTER, SPI_CLOCKPOLARITY_LOW, SPI_CLOCKPHASE_1EDGE, SPI_DATADIRECTION_1LINE_TX, SPI_NSS_SOFT, 0x07);
  SPI_Cmd(ENABLE);  
  
}
#endif

/*=====================================================*/
static void ADC_Config(void)
{
  GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_IN_FL_NO_IT);
  ADC1_DeInit();
  ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, ADC1_CHANNEL_2, ADC1_PRESSEL_FCPU_D2, \
            ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_CHANNEL2,\
            DISABLE);
 
 // ADC1_ITConfig(ENABLE); 
 // enableInterrupts();
  ADC1_StartConversion();
}
/*=====================================================*/

static void TIM4_Config(void)
{
  /* TIM4 configuration:
   - TIM4CLK is set to 16 MHz, the TIM4 Prescaler is equal to 128 so the TIM1 counter
   clock used is 16 MHz / 128 = 125 000 Hz
  - With 125 000 Hz we can generate time base:
      max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
      min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
  - In this example we need to generate a time base equal to 1 ms
   so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 */

  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, ENABLE);
  /* Time base configuration */
  TIM4_TimeBaseInit(TIM4_PRESCALER_128, TIM4_PERIOD);
  /* Clear TIM4 update flag */
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  /* Enable update interrupt */
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
  
  /* enable interrupts */
  enableInterrupts();

  /* Enable TIM4 */
  TIM4_Cmd(ENABLE);
}
/*=====================================================*/
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}
/*=====================================================*/
#ifdef USE_FULL_ASSERT


void assert_failed(uint8_t* file, uint32_t line)
{ 

  while (1)
  {
  }
}
#endif


