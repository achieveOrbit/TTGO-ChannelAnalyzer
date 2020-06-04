
#include "WiFi.h"

#include <TFT_eSPI.h> //135x240
// #include <SPI.h>

#define SERIAL_PRINT_BAUDRATE 115200

#define SCANNING_DELAY 5000
#define TEXT_SIZE      1
#define ROTATE_SCREEN  0

TFT_eSPI tft = TFT_eSPI();

typedef struct {
  uint8_t occupants;
  long    total_strength;
} channel_t;

channel_t channels[14];

const char* frequencies[] = {
    "2401 to 2423",
    "2406 to 2428",
    "2411 to 2433",
    "2416 to 2438",
    "2421 to 2443",
    "2426 to 2448",
    "2431 to 2453",
    "2436 to 2458",
    "2441 to 2463",
    "2446 to 2468",
    "2451 to 2473",
    "2456 to 2478",
    "2461 to 2483",
    "2473 to 2495"
};

/*******************************************************************************
 * SETUP
 ******************************************************************************/

void setup() {

    Serial.begin(SERIAL_PRINT_BAUDRATE);

    tft.init();
    tft.setRotation(ROTATE_SCREEN);
    tft.setTextSize(TEXT_SIZE);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    reset_channels();

    Serial.println("Setup done");

    draw_channel(1, 3, 20);
    draw_channel(3, 6, 2);
    draw_channel(7, 3, 3);
    draw_channel(13, 2, 1);
    tft.drawLine(15, 0, 15, 240, TFT_WHITE);
}

/*******************************************************************************
 * LOOP
 ******************************************************************************/

void loop() {
    // char s[12] = "";
    // int networks = WiFi.scanNetworks();

    // if (networks == 0) {
    //     tft.fillScreen(TFT_BLACK);
    //     tft.drawString("No networks found...", 0, 0);
    //     Serial.println("No networks found...");
    // } else {
    //     for (int i = 0; i < networks; ++i) {
    //         int c = WiFi.channel(i);
    //         channels[c].occupants += 1;
    //         channels[c].total_strength += WiFi.RSSI(i);
    //     }
    //     for (int i = 0; i < sizeof(channels)/sizeof(channel_t); i++) {
    //         tft.fillRect(0, i*8, 8, 200, TFT_BLACK); // Smazeme aktualni radek zejo
    //         // Channel name
    //         // snprintf(s, 12, "Channel %d", i+1);
    //         // tft.drawString(s, 0, i*8);
    //         tft.drawString(frequencies[i], 0, i*8);
    //         // Channel occupants
    //         snprintf(s, 12, "%d", channels[i].occupants);
    //         tft.drawString((const char*)s, 95, i*8);
    //         // Channel signal
    //         if (channels[i].occupants == 0) snprintf(s, 12, "%d", 0);
    //         else snprintf(s, 12, "%d", (channels[i].total_strength)/(channels[i].occupants));
    //         tft.drawString(s, 115, i*8);
    //     }
    // }
    // Serial.println("Waiting...");
    // delay(SCANNING_DELAY);
    // reset_channels();
}

void draw_channel(uint8_t number, uint8_t clients, uint8_t strength) {
    int radius = 33;
    int spacing = 15;
    int y_center = 30+(number-1)*spacing;
    char c[2] = "";
    sprintf(c, "%d", number);
    tft.drawCircle(15, y_center, radius, TFT_GREEN);
    tft.fillRect(0, y_center-radius, 14, radius*2, TFT_BLACK);
    tft.drawString(c, 2, y_center-4);
}

void reset_channels() {
    for (int i = 0; i < sizeof(channels)/sizeof(channel_t); i++) {
        channels[i].occupants = 0;
        channels[i].total_strength = 0;
    }
}
