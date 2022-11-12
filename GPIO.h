/* Used to initialize a GPIO device and read its value */

#ifndef GPIO_H
#define GPIO_H

void  GPIO_configurePins(void);
void  GPIO_configurePin(const int INDEX);
void  GPIO_setDirectionAllDevices(char* mode);
void  GPIO_setDirection(const int INDEX, char* mode);
char* GPIO_readValue(const int INDEX);

#endif