
#include "WiFi.h"

#include <TFT_eSPI.h> 

// CONFIGURATION

#define SERIAL_PRINT_BAUDRATE 115200
#define SCANNING_DELAY 5000
#define TEXT_SIZE      1
#define ROTATE_SCREEN  0
#define CHANNEL_COUNT  13

// DEBUG DEFINITIONS

// #define NSERIAL
// #define OUTPUT_WIFI

#ifdef NSERIAL
#define serialln(s)
#define serial(s)
#else
#define serialln(s) Serial.println(s);
#define serial(s)   Serial.print(s);
#endif

/*******************************************************************************
 * TYPES
 ******************************************************************************/

typedef struct {
    uint8_t occupants;
    long    total_strength;
} channel_t;

/*******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/

TFT_eSPI tft = TFT_eSPI(); // Display
channel_t channels[CHANNEL_COUNT]; // Array with all the channels

/*******************************************************************************
 * SETUP
 ******************************************************************************/

void setup() {

    #ifndef NSERIAL
    Serial.begin(SERIAL_PRINT_BAUDRATE);
    #endif

    serialln("Initializing the screen");

    tft.init();
    tft.setRotation(ROTATE_SCREEN);
    tft.setTextSize(TEXT_SIZE);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    serialln("Setting the WiFi mode");

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    serialln("Setup done");
}

/*******************************************************************************
 * LOOP
 ******************************************************************************/

void loop() {

    // Preparing for the next loop
    int networks = WiFi.scanNetworks();
    tft.fillScreen(TFT_BLACK);
    reset_channels();

    // No networks found
    if (networks == 0) {
        tft.drawString("No networks found...", 0, 0);
        serialln("No networks found...");
    } else {
        char s[25] = "";
        // Heading
        sprintf(s, "Found %d networks", networks);
        tft.drawString(s, 25, 0);
        serialln(s);

        // Column headings
        tft.drawString("C", 80, 20);  // Clients
        tft.drawString("S", 108, 20); // Average signal strength

        // Creating a representation of all the networks
        for (int i = 0; i < networks; ++i) {
            int c = WiFi.channel(i);

            #ifdef OUTPUT_WIFI
            serialln("##########");
            serial("WiFi name: ");
            serialln(WiFi.SSID(i));
            serial("WiFi channel: ");
            serialln(c);
            serial("WiFi strength: ");
            serialln(WiFi.RSSI(i));
            #endif

            if (c < CHANNEL_COUNT) {
                channels[c].occupants += 1;
                channels[c].total_strength += WiFi.RSSI(i);
            }
        }

        // Drawing the necessary networks
        for (int i = 0; i < CHANNEL_COUNT; i++) {
            if (channels[i].occupants > 0) {
                serial("Drawing channel ");
                serialln(i);
                draw_channel(i+1, channels[i].occupants, channels[i].total_strength);
            }
        }
        // Drawing the channel numbers and graph line
        serialln("Drawing the channel numbers");
        draw_channel_numbers();
        tft.drawLine(15, 0, 15, 240, TFT_WHITE);
    }
    serialln("Waiting for scan...");
    delay(SCANNING_DELAY);
}

/*******************************************************************************
 * HELPER FUNCTIONS
 ******************************************************************************/

/**
 * Function to draw the frequency graph for each channel.
 * 
 * This function chooses the frequency color based on the average guest signal strength,
 * calculated as strength/clients.
 * 
 * @param number The number of the channel (1 - 13)
 * @param clients The number of clients connected to that channel
 * @param strength The compound signal strength of all the clients
 */
void draw_channel(uint8_t number, uint8_t clients, long strength) {
    int radius = 33;
    int spacing = 15;
    int y_center = 30+(number-1)*spacing;

    int color;

    if      (clients == 1) color = TFT_GREEN;
    else if (clients == 2) color = TFT_GREENYELLOW;
    else if (clients <= 4) color = TFT_YELLOW;
    else if (clients <= 6) color = TFT_ORANGE;
    else                   color = TFT_RED;

    // if (clients == 1) color = TFT_GREEN;
    // else if (clients == 2) color = TFT_YELLOW;
    // else if (clients == 3) color = TFT_ORANGE;
    // else color = TFT_RED;

    // Drawing the graph
    tft.drawCircle(15, y_center, radius, color);

    // Drawing the info
    tft.drawLine(60, y_center, 70, y_center, TFT_WHITE);
    tft.drawNumber(clients, 80, y_center-4);
    tft.drawNumber((long)strength/clients, 100, y_center-4);
}

/**
 * This function draws a black rectangle to cover the left side of the circles created
 * by draw_channel(), and then draws the numbers.
 * 
 * I chose to create this function to get rid of the issue where channel numbers
 * would be hidden by the newly drawn channels.
 */
void draw_channel_numbers() {
    char c[5] = "";
    tft.fillRect(0, 0, 15, 240, TFT_BLACK);
    for (int i = 0; i < CHANNEL_COUNT; i++) {
        if (channels[i].occupants != 0) {
            tft.drawNumber(i+1, 2, 26+i*15);
        }
    }
}

/**
 * Sets all the channels back to 0.
 */
void reset_channels() {
    for (int i = 0; i < CHANNEL_COUNT; i++) {
        channels[i].occupants = 0;
        channels[i].total_strength = 0;
    }
}
