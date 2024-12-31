#include "Led.h"
#include <HomeSpan.h>

void setup() {
    Serial.begin(115200);
 
    homeSpan.setPairingCode("12344555");
    homeSpan.setQRID("123-44-555"); 

    homeSpan.begin(Category::Lighting, "LED Strip Controller");

    new SpanAccessory();
    new Service::AccessoryInformation();
    new Characteristic::Identify();

    new LED();
}

void loop() {
    homeSpan.poll();
}




