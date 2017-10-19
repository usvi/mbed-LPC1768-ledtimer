#include "LPC17xx.h"


#define LED1_PIN_SELECTOR (LPC_PINCON->PINSEL3)
#define LED1_PIN_SELECTOR_MASK (((uint32_t)1 << 5) | ((uint32_t)1 << 4))
#define LED1_GPIO_MASK ((uint32_t)1 << 2)
#define LED1_GPIO_DIRECTOR (LPC_GPIO1->FIODIR2)
#define LED1_GPIO_SETTER (LPC_GPIO1->FIOSET2)
#define LED1_GPIO_CLEARER (LPC_GPIO1->FIOCLR2)

#define BUTTON1_PIN_SELECTOR (LPC_PINCON->PINSEL0)
#define BUTTON1_PIN_SELECTOR_MASK (((uint32_t)1 << 1) | ((uint32_t)1 << 0))
#define BUTTON1_PIN_MODE (LPC_PINCON->PINMODE0)
#define BUTTON1_GPIO_MASK ((uint32_t)1 << 0)
#define BUTTON1_GPIO_DIRECTOR (LPC_GPIO0->FIODIR0)
#define BUTTON1_GPIO_READ_DATA (LPC_GPIO0->FIOPIN)

#define TIMER0_INT_MASK ((uint32_t)1 << 0)
#define TIMER0_RESET_MASK ((uint32_t)1 << 1)
#define TIMER0_ENA_MASK ((uint32_t)1 << 0)
#define TIMER0_CLOCK_DIVISOR ((uint32_t)4)

#define STATE_NORMAL ((uint8_t)1)
#define STATE_LED_ON ((uint8_t)2)

#define CONV_1MS_TO_HERTZ ((uint32_t)1000)
#define TIME_50MS ((uint32_t)50)
#define TIME_200MS_IN_TICKS ((uint32_t)4)
#define TIME_3S_IN_TICKS ((uint32_t)60)

#define PIN_RESISTORS_MASK ((((uint32_t)1) << 1) | (((uint32_t)1) << 0))



void setup_led1(void)
{
  // Configure LED pin functionality to GPIO:
  LED1_PIN_SELECTOR &= ~LED1_PIN_SELECTOR_MASK;
  // Configure LED GPIO direction to write:
  LED1_GPIO_DIRECTOR |= LED1_GPIO_MASK;
  // Dim the LED:
  LED1_GPIO_CLEARER |= LED1_GPIO_MASK;
}

void setup_button1(void)
{
  // Configure button pin functionality to GPIO:
  BUTTON1_PIN_SELECTOR &= ~BUTTON1_PIN_SELECTOR_MASK;
  // Configure button GPIO direction to read:
  BUTTON1_GPIO_DIRECTOR &= ~BUTTON1_GPIO_MASK;
  // Make button read logical level high by default:
  BUTTON1_PIN_MODE &= PIN_RESISTORS_MASK;
}



void TIMER0_IRQHandler(void)
{
  // Dummy interrupt handler to wake the system from sleep
  LPC_TIM0->IR = LPC_TIM0->IR;
}


void setup_50ms_timer(void)
{
  // Clear counter and generate interrupt when timer matches:
  LPC_TIM0->MCR |= (TIMER0_INT_MASK | TIMER0_RESET_MASK);
  // With prescaler set the counting step to 1 ms:
  LPC_TIM0->PR = (SystemCoreClock /
		  (CONV_1MS_TO_HERTZ * TIMER0_CLOCK_DIVISOR))  - 1;
  // Make counter count to 50 counting steps, equaling 50 ms:
  LPC_TIM0->MR0 = TIME_50MS;
  // Connect the interrupt handler:
  NVIC_EnableIRQ(TIMER0_IRQn);
  // Finally, enable the timer:
  LPC_TIM0->TCR |= TIMER0_ENA_MASK;
}


void run_50ms_task(void)
{
  static volatile uint8_t u8_state = STATE_NORMAL;
  static volatile uint8_t u8_button_samples = 0;
  static volatile uint8_t u8_led_wait_samples = 0;

  if(u8_state == STATE_NORMAL)
  {
    if((BUTTON1_GPIO_READ_DATA & BUTTON1_GPIO_MASK) == 0) // Button read ok
    {
      if((u8_button_samples++) >= TIME_200MS_IN_TICKS)
      {
	LED1_GPIO_SETTER |= LED1_GPIO_MASK; // Enough read samples,
	u8_button_samples = 0;              // light up the LED
	u8_state = STATE_LED_ON;            // and change state.
      }
    }
    else
    {
      u8_button_samples = 0;
    }
  } // Check pass of 3 seconds in state STATE_LED_ON:
  else if((u8_led_wait_samples++) >= TIME_3S_IN_TICKS)
  {
    LED1_GPIO_CLEARER |= LED1_GPIO_MASK; // Enough wait samples,
    u8_led_wait_samples = 0;             // dim the LED and
    u8_state = STATE_NORMAL;             // change the state.
  }
}


int main() {

  // Initialize and start system
  SystemInit();
  setup_led1();
  setup_button1();
  setup_50ms_timer();

  while(1)
  {
    // Sleep the CPU until we hit the next 50ms mark:
    __WFI();
    // Run the actual 50ms task:
    run_50ms_task();
  }
  
  return 0;
}


