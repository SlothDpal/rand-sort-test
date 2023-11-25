#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include "serialprintf.h"

// если определена (раскоментировать), 
// то за итерацию перемешивается весь массив
// иначе, только два случайных элемента
//#define SHUFFLE_ARRAY

// *** совместимость светодиодного моргания ***
// раскомментировать если плата esp32 s2 mini
//#define LED_BUILTIN 15
// раскоментировать если плата RP2040-ZERO
//#define RP2040BIWS 16
// раскоментировать если плата RP2040 с алика со встроенным ws2812
//#define RP2040BIWS 23

#ifdef RP2040BIWS
    #include <Adafruit_NeoPixel.h>
    Adafruit_NeoPixel builtinLed(1,RP2040BIWS,NEO_GRBW + NEO_KHZ800);
#endif

bool isSorted(uint8_t * data, size_t len){
    for (size_t i=0;i<len-1;i++) if (data[i]>data[i+1]) return false;
    return true;
}

void shuffleArray(uint8_t * data, size_t len) {
    size_t i;
    if (len > 1) {
        for (i = 0; i < len; i++) {
            int j = rand() % len;
            uint8_t temp = data[j];
            data[j] = data[i];
            data[i] = temp;
        }
    }
}

void setup() {
    uint8_t *data=NULL;
    uint8_t *startData=NULL;
    size_t dataLen=10;
    unsigned long start, end;
    unsigned long delta_us,min_iter=-1,max_iter=0,iter=0,avg_iter=0,min_t=-1,max_t=0,avg_t=0;
    unsigned seed=43434,count=0,max_count=10;
    #if defined(RP2040BIWS)
        builtinLed.begin();
        builtinLed.setPixelColor(0,builtinLed.Color(255,0,0));
        builtinLed.setBrightness(25);
        builtinLed.show();
    #endif
    pinMode(LED_BUILTIN,OUTPUT); 
    digitalWrite(LED_BUILTIN,true);
    #if (defined(ESP32)||defined(ESP8266))
        Serial.begin(74800);
    #else
        Serial.begin(9600);
    #endif
    delay(5000);
    SerialPrintf("\n");
    SerialPrintf("Seed: %u\n",seed);
    srand(seed);

    data=(uint8_t*)malloc(sizeof(uint8_t)*dataLen);
    if (!data) { SerialPrintf("Ошибка выделения памяти (data)"); return;}
    startData=(uint8_t*)malloc(sizeof(uint8_t)*dataLen);
    if (!startData) { SerialPrintf("Ошибка выделения памяти (startData)"); return;}
    for (size_t i=0;i<dataLen;i++) data[i]=dataLen-i-1;
    shuffleArray(data,dataLen);
    SerialPrintf ("Стартовая последовательность: ");
    for (size_t i=0;i<dataLen;i++) SerialPrintf ("%i ",data[i]);
    SerialPrintf ("\n");

    while (count<max_count){
        iter=0;
        memcpy((uint8_t*)startData,(uint8_t*)data,sizeof(uint8_t)*dataLen);
        start=millis();
        unsigned long lastMs=millis();
        bool blLed=true;
        do { 
            #ifdef SHUFFLE_ARRAY
                shuffleArray(startData,dataLen); 
            #else
                int i = rand() % dataLen;
                int j = rand() % dataLen;
                uint8_t temp = startData[j];
                startData[j] = startData[i];
                startData[i] = temp;
            #endif
            #if (defined(ESP8266))
                ESP.wdtFeed();
            #endif
            iter++;
            if ((millis()-lastMs)>500) {
                if (blLed) blLed=false; else blLed=true;
                digitalWrite(LED_BUILTIN,blLed);
                #if defined(RP2040BIWS)
                    if (blLed) {builtinLed.setPixelColor(0,builtinLed.Color(0,0,255)); builtinLed.show();}
                    else {builtinLed.setPixelColor(0,builtinLed.Color(0,255,0)); builtinLed.show();}
                #endif
                lastMs=millis();
            }
        } while (!isSorted(startData,dataLen));
        SerialPrintf ("Отсортировано: ");
        end=millis();
        delta_us = end-start;
        if (count == 0) {
            min_t=delta_us;
            max_t=delta_us;
            min_iter=iter;
            max_iter=iter;
        }
        if (delta_us<min_t) min_t=delta_us;
        if (delta_us>max_t) max_t=delta_us;
        //for (size_t i=0;i<dataLen;i++) SerialPrintf ("%i ",startData[i]);
        //SerialPrintf("\n");
        SerialPrintf ("%u: Цикл занял %lu мсек (%lu сек), %lu итераций.\n",count+1,delta_us,(unsigned long)(delta_us/1000L),iter);
        count++;
    }
    avg_t=(min_t+max_t)/2;
    avg_iter=(min_iter+max_iter)/2;
    SerialPrintf ("%u сортировок. Время мин:%lu макс:%lu среднее:%lu Среднее количество итераций:%lu\n",max_count,min_t,max_t,avg_t,avg_iter);
    #if defined(RP2040BIWS)
        builtinLed.setPixelColor(0,builtinLed.Color(0,0,0,255)); 
        builtinLed.show();
    #endif
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
}
