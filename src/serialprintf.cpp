#include <Arduino.h>
#include <stdarg.h>

#if (defined(ESP32) || defined(ESP8266))
void SerialPrintf (const char * format, ...)
{
  char *printfbuff;
  va_list args;
  va_start(args, format);
  vasprintf(&printfbuff,format,args);
  va_end(args);
  Serial.print(printfbuff);
  free(printfbuff);
}
#else
void SerialPrintf (const char * format, ...)
{
  char printfbuff[256];
  va_list args;
  va_start(args, format);
  vsprintf(printfbuff,format,args);
  va_end(args);
  Serial.print(printfbuff);
}
#endif

