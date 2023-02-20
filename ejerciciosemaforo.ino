#include <Arduino_FreeRTOS.h>
#include <timers.h>
#include <queue.h>
#include <stdlib.h>
#include <semphr.h>


// Prototipo de la tarea
int segons = 0;
void startA(void *pvParameters);
void startB(void *pvParameters);
void timerCallback(TimerHandle_t timer);

// Creem objectes
TaskHandle_t tascaA;
TaskHandle_t tascaB;

TimerHandle_t timer;

QueueHandle_t cua;

SemaphoreHandle_t semafor;

void setup() {
  Serial.begin(9600);

  // Creem la cua, les tasques, el timer i el semafor
  cua = xQueueCreate(32, sizeof(int));
  semafor = xSemaphoreCreateBinary(); // Semafor de tipus binari
  xTaskCreate(startA, "TascaA", 100, NULL, 0, &tascaA);
  xTaskCreate(startB, "TascaB", 100, NULL, 0, &tascaB);
  timer = xTimerCreate("Timer", pdMS_TO_TICKS(10000), pdTRUE, (void*)0, timerCallback);
  xTimerStart(timer, 0);

  vTaskStartScheduler();
}

void loop() {
  
}

void startA(void *pvParameters)
{
  while(1)
  {
    int numa = rand();
    // Esperem a que el semafor estigui lliure (ho fa el callback)
    xSemaphoreTake(semafor, portMAX_DELAY);
    Serial.print("PING ");
    Serial.println(numa);
    // Enviem el valor
    xQueueSend(cua, &numa, 0);
  }
}

void startB(void *pvParameters)
{
  int numb;
  while(1)
  {
    xQueueReceive( cua, &numb, portMAX_DELAY); // Agafa el numero de la cua
    Serial.print("PONG ");
    Serial.println(numb);
    Serial.println();
  }
}

void timerCallback(TimerHandle_t timer) {
  segons = segons + 10; // Incrementa 10
  Serial.print(segons);
  Serial.println(" TIMEOUT");
  xSemaphoreGive(semafor); // Es deixa el semafor lliure
}
