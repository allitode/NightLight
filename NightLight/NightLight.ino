/*
 Name:		NightLight.ino
 Created:	4/14/2018 1:01:26 PM
 Author:	Jace
*/

#include <Adafruit_NeoPixel.h>
#define PIN 0
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(7, PIN);
uint8_t mode = 4,
offset = 0;
uint32_t color = 0X00A4B3;
uint32_t prevTime;

// the setup function runs once when you press reset or power the board
void setup() {
	pixels.begin();
	pixels.setBrightness(40);
	prevTime = millis();
}

// the loop function runs over and over again until power down or reset
void loop() {
	uint8_t i;
	uint32_t t;

	switch (mode) {
	case 0: // off
		blackout();
		delay(8000);
		break;

	case 1: // rainbow cycle slow
		rainbowCycleslow(20);
		delay(50);
		break;

	case 2: // rainbow cycle fast
		rainbowCycle(5);
		delay(50);
		break;

	case 3: // rainbow hold
		rainbowHold(20);
		delay(500);
		break;

	case 4: // pink twinkle
		pinkTwinkle(20);
		delay(500);
		break;
	}

	t = millis();
	if ((t - prevTime) > 8000) {
		//mode++;
		if (mode>4) {
			mode = 0;
			color >>= 8;
			if (!color) color = 0xB300A4;
		}
		for (i = 0; i<pixels.numPixels(); i++) pixels.setPixelColor(i, 0);
		prevTime = t;
	}
}

void pinkTwinkle(uint8_t wait) {
	uint16_t i, j;

	for (i = 0; i<pixels.numPixels(); i++) {
		//pixels.setPixelColor(i, 0x330011);
		pixels.setPixelColor(i, 56, 0, 19);
	}
	pixels.show();
	j = random(pixels.numPixels());
	/*pixels.setPixelColor(j, 0x990C3F);
	pixels.show();
	delay(wait);*/

	//fadePixel(j, 0x990C3F, 20, wait);
	fadePixel(j, 153, 0, 63, 20, wait);
}

void fadePixel(uint16_t pixel, uint8_t r, uint8_t g, uint8_t b, uint8_t steps, uint8_t wait) {
	int startColor = pixels.getPixelColor(pixel);
	int stepColor = startColor;
	int8_t stepR, stepG, stepB;

	stepR = (r - Red(startColor)) / steps;
	stepG = (g - Green(startColor)) / steps;
	stepB = (b - Blue(startColor)) / steps;
	
	for (int step = 1; step < steps; step++) {
		stepColor = pixels.Color((Red(startColor) + stepR * step), (Green(startColor) + stepG * step), (Blue(startColor) + stepB * step));
		pixels.setPixelColor(pixel, stepColor);
		pixels.show();
		delay(wait);
	}
	// deal with rounding errors
	pixels.setPixelColor(pixel, pixels.Color(r, g, b));
	pixels.show();
	delay(wait * steps);
}

// Returns the Red component of a 32-bit color
uint8_t Red(uint32_t color)
{
	return (color >> 16) & 0xFF;
}

// Returns the Green component of a 32-bit color
uint8_t Green(uint32_t color)
{
	return (color >> 8) & 0xFF;
}

// Returns the Blue component of a 32-bit color
uint8_t Blue(uint32_t color)
{
	return color & 0xFF;
}

void blackout() {
	for (int i = 0; i < pixels.numPixels(); i++) {
		pixels.setPixelColor(i, 0);
	}
	pixels.show();
}

void rainbow(uint8_t wait) {
	uint16_t i, j;

	for (j = 0; j<256; j++) {
		for (i = 0; i<pixels.numPixels(); i++) {
			pixels.setPixelColor(i, Wheel((i + j) & 255));
		}
		pixels.show();
		delay(wait);
	}
}

void rainbowCycle(uint8_t wait) {
	uint16_t r, j;

	for (j = 0; j<256 * 6; j++) { // 6 cycles of all colors on wheel
		for (r = 0; r< pixels.numPixels(); r++) {
			pixels.setPixelColor(r, Wheel(((r * 256 / pixels.numPixels()) + j) & 255));
		}
		pixels.show();
		delay(wait);
	}
}
void rainbowCycleslow(uint8_t wait) {
	uint16_t r, j;

	for (j = 0; j<256 * 3; j++) { // 3 cycles of all colors on wheel
		for (r = 0; r< pixels.numPixels(); r++) {
			pixels.setPixelColor(r, Wheel(((r * 256 / pixels.numPixels()) + j) & 255));
		}
		pixels.show();
		delay(wait);
	}
}
void rainbowHold(uint8_t wait) {
	uint16_t r, j;

	for (j = 0; j<256 * 1; j++) { // 3 cycles of all colors on wheel
		for (r = 0; r< pixels.numPixels(); r++) {
			pixels.setPixelColor(r, Wheel(((r * 256 / pixels.numPixels()) + j) & 255));
		}
		pixels.show();
		delay(wait);
	}
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
	if (WheelPos < 85) {
		return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
	}
	else if (WheelPos < 170) {
		WheelPos -= 85;
		return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
	}
	else {
		WheelPos -= 170;
		return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
	}
}
