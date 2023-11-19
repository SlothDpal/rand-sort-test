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
    uint8_t data[10]={9,8,7,6,5,4,3,2,1,0};
    uint8_t startData[10];
    size_t dataLen=10;
    unsigned long start, end;
    unsigned long delta_us,iter=0,avg_iter=0,min_t=-1,max_t=0,avg_t=0;
    unsigned seed=43434,count=0,max_count=10;
    Serial.begin(9600);
    srand(seed);
    pinMode(LED_BUILTIN,OUTPUT);    
    shuffleArray(data,dataLen);
    SerialPrintf ("\nСтартовая последовательность: ");
    for (size_t i=0;i<dataLen;i++) SerialPrintf ("%i ",data[i]);
    SerialPrintf("\n");

    while (count<max_count){
        iter=0;
        memcpy(startData,data,sizeof(uint8_t)*dataLen);
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
            iter++;
            if ((millis()-lastMs)>500) {
                if (blLed) blLed=false; else blLed=true;
                digitalWrite(LED_BUILTIN,blLed);
                lastMs=millis();
            }
        } while (!isSorted(startData,dataLen));
        SerialPrintf ("Отсортировано: ");
        end=millis();
        delta_us = end-start;
        if (delta_us<min_t) min_t=delta_us;
        if (delta_us>max_t) max_t=delta_us;
        avg_t=(avg_t+delta_us)/2;
        avg_iter=(avg_iter+iter)/2;
        for (size_t i=0;i<dataLen;i++) SerialPrintf ("%i ",startData[i]);
        SerialPrintf("\n");
        SerialPrintf ("Цикл занял %lu мсек (%lu сек), %lu итераций.\n",delta_us,(unsigned long)(delta_us/1000L),iter);
        count++;
    }
    SerialPrintf ("%u сортировок. Время мин:%lu макс:%lu среднее:%lu Среднее количество итераций:%lu\n",max_count,min_t,max_t,avg_t,avg_iter);

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
}
