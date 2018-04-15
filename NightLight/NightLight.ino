#include <Adafruit_NeoPixel.h>

// Pattern types supported:
enum  pattern { NONE, RAINBOW_CYCLE, THEATER_CHASE, COLOR_WIPE, TWO_COLOR_WIPE, SCANNER, FADE, SINGLE_PIXEL_FADE };
// Patern directions supported:
enum  direction { FORWARD, REVERSE };

// NeoPattern Class - derived from the Adafruit_NeoPixel class
class NeoPatterns : public Adafruit_NeoPixel
{
public:

	// Member Variables:  
	pattern  ActivePattern;		// which pattern is running
	direction Direction;		// direction to run the pattern
	direction PixelDirection;	// direction to increment pixels (used for cases where an Index and PixelIndex need to be used)

	unsigned long Interval;		// milliseconds between updates
	unsigned long lastUpdate;	// last update of position

	uint32_t Color1, Color2;	// What colors are in use
	uint16_t TotalSteps;		// total number of steps in the pattern
	uint16_t Index;				// current step within the pattern
	uint16_t PixelIndex;		// current pixel within the pattern (used for cases where an Index and a PixelIndex need to be used)

	void(*OnComplete)();		// Callback on completion of pattern
	void(*OnPixelComplete)();	// Callback on completion of Pixel pattern (used for cases where an Index and a PixelIndex need to be used)

	// Constructor - calls base-class constructor to initialize strip
	NeoPatterns(uint16_t pixels, uint8_t pin, uint8_t type, void(*callback)(), void(*pixelcallback)())
		:Adafruit_NeoPixel(pixels, pin, type)
	{
		OnComplete = callback;
		OnPixelComplete = pixelcallback;
	}

	// Update the pattern
	void Update()
	{
		if ((millis() - lastUpdate) > Interval) // time to update
		{
			lastUpdate = millis();
			switch (ActivePattern)
			{
			case RAINBOW_CYCLE:
				RainbowCycleUpdate();
				break;
			case THEATER_CHASE:
				TheaterChaseUpdate();
				break;
			case COLOR_WIPE:
				ColorWipeUpdate();
				break;
			case SCANNER:
				ScannerUpdate();
				break;
			case FADE:
				FadeUpdate();
				break;
			case TWO_COLOR_WIPE:
				TwoColorWipeUpdate();
				break;
			case SINGLE_PIXEL_FADE:
				SinglePixelFadeUpdate();
				break;
			default:
				break;
			}
		}
	}

	// Increment the Index and reset at the end
	void Increment()
	{
		if (Direction == FORWARD)
		{
			Index++;
			if (Index >= TotalSteps)
			{
				Index = 0;
				if (OnComplete != NULL)
				{
					OnComplete(); // call the comlpetion callback
				}
			}
		}
		else // Direction == REVERSE
		{
			--Index;
			if (Index <= 0)
			{
				Index = TotalSteps - 1;
				if (OnComplete != NULL)
				{
					OnComplete(); // call the comlpetion callback
				}
			}
		}
	}

	void PixelIncrement() 
	{
		if (PixelDirection == FORWARD)
		{
			PixelIndex++;
			if (PixelIndex >= numPixels())
			{
				PixelIndex = 0;
				if (OnPixelComplete != NULL)
				{
					OnPixelComplete(); // call the comlpetion callback
				}
			}
		}
		else // Direction == REVERSE
		{
			--PixelIndex;
			if (PixelIndex <= 0)
			{
				PixelIndex = numPixels() - 1;
				if (OnPixelComplete != NULL)
				{
					OnPixelComplete(); // call the comlpetion callback
				}
			}
		}
	}

	// Reverse pattern direction
	void Reverse()
	{
		if (Direction == FORWARD)
		{
			Direction = REVERSE;
			Index = TotalSteps - 1;
		}
		else
		{
			Direction = FORWARD;
			Index = 0;
		}
	}

	//  Reverse the pixel direction
	void PixelReverse() {
		if (PixelDirection == FORWARD) 
		{
			PixelDirection = REVERSE;
			PixelIndex = numPixels() - 1;
		}
		else 
		{
			PixelDirection = FORWARD;
			PixelIndex = 0;
		}
	}

	// Initialize for a RainbowCycle
	void RainbowCycle(uint8_t interval, direction dir = FORWARD)
	{
		ActivePattern = RAINBOW_CYCLE;
		Interval = interval;
		TotalSteps = 255;
		Index = 0;
		Direction = dir;
	}

	// Update the Rainbow Cycle Pattern
	void RainbowCycleUpdate()
	{
		for (int i = 0; i< numPixels(); i++)
		{
			setPixelColor(i, Wheel(((i * 256 / numPixels()) + Index) & 255));
		}
		show();
		Increment();
	}

	// Initialize for a Theater Chase
	void TheaterChase(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = FORWARD)
	{
		ActivePattern = THEATER_CHASE;
		Interval = interval;
		TotalSteps = numPixels();
		Color1 = color1;
		Color2 = color2;
		Index = 0;
		Direction = dir;
	}

	// Update the Theater Chase Pattern
	void TheaterChaseUpdate()
	{
		for (int i = 0; i< numPixels(); i++)
		{
			if ((i + Index) % 3 == 0)
			{
				setPixelColor(i, Color1);
			}
			else
			{
				setPixelColor(i, Color2);
			}
		}
		show();
		Increment();
	}

	// Initialize for a ColorWipe
	void ColorWipe(uint32_t color, uint8_t interval, direction dir = FORWARD)
	{
		ActivePattern = COLOR_WIPE;
		Interval = interval;
		TotalSteps = numPixels();
		Color1 = color;
		Index = 0;
		Direction = dir;
	}

	// Initialize for a TwoColorWipe
	void TwoColorWipe(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = FORWARD) {
		ActivePattern = TWO_COLOR_WIPE;
		Interval = interval;
		TotalSteps = numPixels();
		Color1 = color1;
		Color2 = color2;
		Index = 0;
		Direction = dir;
	}

	// Update the Color Wipe Pattern
	void ColorWipeUpdate()
	{
		setPixelColor(Index, Color1);
		show();
		Increment();
	}

	// Update the Color Wipe Pattern
	void TwoColorWipeUpdate()
	{
		ColorSet(Color1);
		setPixelColor(Index, Color2);
		show();
		Increment();
	}

	// Initialize for a SCANNNER
	void Scanner(uint32_t color1, uint8_t interval)
	{
		ActivePattern = SCANNER;
		Interval = interval;
		TotalSteps = (numPixels() - 1) * 2;
		Color1 = color1;
		Index = 0;
	}

	// Update the Scanner Pattern
	void ScannerUpdate()
	{
		for (int i = 0; i < numPixels(); i++)
		{
			if (i == Index)  // Scan Pixel to the right
			{
				setPixelColor(i, Color1);
			}
			else if (i == TotalSteps - Index) // Scan Pixel to the left
			{
				setPixelColor(i, Color1);
			}
			else // Fading tail
			{
				setPixelColor(i, DimColor(getPixelColor(i)));
			}
		}
		show();
		Increment();
	}

	// Initialize for a Fade
	void Fade(uint32_t color1, uint32_t color2, uint16_t steps, uint8_t interval, direction dir = FORWARD)
	{
		ActivePattern = FADE;
		Interval = interval;
		TotalSteps = steps;
		Color1 = color1;
		Color2 = color2;
		Index = 0;
		Direction = dir;
	}

	// Initialize for a SinglePixelFade
	void SinglePixelFade(uint32_t color1, uint32_t color2, uint16_t steps, uint8_t interval, direction dir = FORWARD, direction pixelDir = FORWARD)
	{
		ActivePattern = SINGLE_PIXEL_FADE;
		Interval = interval;
		TotalSteps = steps;
		Color1 = color1;
		Color2 = color2;
		Index = 0;
		PixelIndex = 0;
		Direction = dir;
		PixelDirection = pixelDir;
	}

	// Update the Fade Pattern
	void FadeUpdate()
	{
		// Calculate linear interpolation between Color1 and Color2
		// Optimise order of operations to minimize truncation error
		uint8_t red = ((Red(Color1) * (TotalSteps - Index)) + (Red(Color2) * Index)) / TotalSteps;
		uint8_t green = ((Green(Color1) * (TotalSteps - Index)) + (Green(Color2) * Index)) / TotalSteps;
		uint8_t blue = ((Blue(Color1) * (TotalSteps - Index)) + (Blue(Color2) * Index)) / TotalSteps;

		ColorSet(Color(red, green, blue));
		show();
		Increment();
	}

	// update the SinglePixelFade Pattern
	void SinglePixelFadeUpdate() {
		ColorSet(Color1);
		
		// Calculate linear interpolation between Color1 and Color2
		// Optimise order of operations to minimize truncation error
		uint8_t red = ((Red(Color1) * (TotalSteps - Index)) + (Red(Color2) * Index)) / TotalSteps;
		uint8_t green = ((Green(Color1) * (TotalSteps - Index)) + (Green(Color2) * Index)) / TotalSteps;
		uint8_t blue = ((Blue(Color1) * (TotalSteps - Index)) + (Blue(Color2) * Index)) / TotalSteps;

		setPixelColor(PixelIndex, red, green, blue);
		show();
		Increment();
	}

	// Calculate 50% dimmed version of a color (used by ScannerUpdate)
	uint32_t DimColor(uint32_t color)
	{
		// Shift R, G and B components one bit to the right
		uint32_t dimColor = Color(Red(color) >> 1, Green(color) >> 1, Blue(color) >> 1);
		return dimColor;
	}

	// Set all pixels to a color (synchronously)
	void ColorSet(uint32_t color)
	{
		for (int i = 0; i < numPixels(); i++)
		{
			setPixelColor(i, color);
		}
		show();
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

	// Input a value 0 to 255 to get a color value.
	// The colours are a transition r - g - b - back to r.
	uint32_t Wheel(byte WheelPos)
	{
		WheelPos = 255 - WheelPos;
		if (WheelPos < 85)
		{
			return Color(255 - WheelPos * 3, 0, WheelPos * 3);
		}
		else if (WheelPos < 170)
		{
			WheelPos -= 85;
			return Color(0, WheelPos * 3, 255 - WheelPos * 3);
		}
		else
		{
			WheelPos -= 170;
			return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
		}
	}
};

void JewelComplete();
void JewelPixelComplete();
uint8_t mode = 0;
bool modeChanged = false;
uint32_t prevTime;

// Define some NeoPatterns for the two rings and the stick
//  as well as some completion routines
NeoPatterns Jewel(7, 0, NEO_GRB + NEO_KHZ800, &JewelComplete, &JewelPixelComplete);

// Initialize everything and prepare to start
void setup()
{
	//pinMode(1, INPUT_PULLUP); // FUTURE BUTTON

	// Initialize all the pixelStrips
	Jewel.begin();
	Jewel.setBrightness(128);

	// Kick off a pattern
	//Jewel.Fade(0x330011, 0x880066, 20, 150, FORWARD);
	//Jewel.TwoColorWipe(0x330011, 0x880066, 150, FORWARD);
	PinkGlow();
}

void PinkGlow() 
{
	Jewel.SinglePixelFade(0x330011, 0x880066, 20, 75, FORWARD, FORWARD);
}

void Campfire() 
{
	Jewel.SinglePixelFade(0x880600, 0xff0000, 10, 50, FORWARD, FORWARD);
}

// Main loop
void loop()
{
	uint32_t t;
	if (modeChanged) {
		switch (mode)
		{
		case 0: // PinkGlow
			PinkGlow();
			modeChanged = false;
			break;
		case 1: // CampFire
			Campfire();
			modeChanged = false;
			break;

		}
	}
	Jewel.Update();

	t = millis();
	if ((t - prevTime) > 8000) {
		mode++;
		modeChanged = true;
		if (mode > 1) {
			mode = 0;
		}
		prevTime = t;
	}

	//// Switch patterns on a button press:
	//if (digitalRead(8) == LOW) // Button #1 pressed
	//{
	//	// Switch Ring1 to FADE pattern
	//	Ring1.ActivePattern = FADE;
	//	Ring1.Interval = 20;
	//	// Speed up the rainbow on Ring2
	//	Ring2.Interval = 0;
	//	// Set stick to all red
	//	Stick.ColorSet(Stick.Color(255, 0, 0));
	//}
}

//------------------------------------------------------------
//Completion Routines - get called on completion of a pattern
//------------------------------------------------------------
void JewelComplete() {
	Jewel.Reverse();
	if (Jewel.Direction == FORWARD) {
		Jewel.PixelIncrement();
	}
}

void JewelPixelComplete() {
	Jewel.PixelReverse();
	if (Jewel.PixelDirection == REVERSE) {
		Jewel.PixelIncrement();
	}
}