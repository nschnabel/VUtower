#include <FastLED.h>


DEFINE_GRADIENT_PALETTE( heatmap_gp ) {
 0,     120,  0,  255,   //black
25,      60,  0 , 255,
50,     0 , 170 , 255,
65,     0 , 255, 0,
86, 0 , 255 , 0,
100,    0 , 255, 0,
125,    125, 255, 0,
150,    255, 255, 0,
175,    255, 125, 0,
200,    255, 75, 0,  
225,    255,0,0,
255,    255, 0 , 0}; //full white


int sp = 25;

CRGB leds[30];
int column_one[10];
int column_two[10];
int column_three[10];
int brightness = 70;

const int sampleWindow = 10; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample1;
unsigned int sample2;
int rows_on_last = 1;
CRGBPalette16 currentPalette;

void setup() {
  delay( 3000 ); // power-up safety delay
  
  currentPalette = heatmap_gp;
  
  for( int i = 0 ; i < 10 ; i++)
    {
      if(i % 2 == 0)
      {
        column_one[i] = i*3;
        column_three[i] = i*3 + 2;
      }
      else
      {
        column_one[i] = i*3 + 2;
        column_three[i] = i*3;
      }

      column_two[i] = i*3 + 1;
    }
    
  //pinMode(3, INPUT);
  pinMode(10, OUTPUT);
  FastLED.addLeds<WS2811, 10, GRB>(leds, 30).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  255 );

  
  
}


void loop()
{
  //ChangePalettePeriodically();
   unsigned long startMillis= millis();  // Start of sample window
   //unsigned int peakToPeak = 0;   // peak-to-peak level
 
   unsigned int signalMax1 = 0;
   unsigned int signalMax2 = 0;
   unsigned int signalMin1 = 1024;
   unsigned int signalMin2 = 1024;
  
  while (millis() - startMillis < sampleWindow)
   {
      sample1 = analogRead(1);
      sample2 = analogRead(0);
      
      if (sample1 < 1024 || sample2 < 1024)  // toss out spurious readings
      {
         if (sample1 > signalMax1)
         {
            signalMax1 = sample1;  // save just the max levels
         }
         else if (sample1 < signalMin1)
         {
            signalMin1 = sample1;  // save just the min levels
         }

         if (sample2 > signalMax2)
         {
            signalMax2 = sample2;  // save just the max levels
         }
         else if (sample2 < signalMin2)
         {
            signalMin2 = sample2;  // save just the min levels
         }
      }
   }
  static uint8_t startIndex = 0;
  //startIndex = startIndex + 1; /* motion speed */

  //sample1 is regular input, sample2 is low pass filtered input
  double volts1 = ((signalMax1 - signalMin1) * 5.0) / 1024;  // convert to volts


  //regular input controls first four rows, bass controls next 6
  int rows_on_current = ((double)volts1 - 0.267)/0.15;
  if(rows_on_current <= 0)
    rows_on_current = 1;
  if(rows_on_current > 5)
    rows_on_current = 5;

  double volts2 = ((signalMax2 - signalMin2) * 5.0) / 1024;  // convert to volts
  int rows_to_add = ((double)volts2 - 0.267)/0.18;

  rows_on_current += rows_to_add;
  if(rows_on_current <= 0)
    rows_on_current = 1;
  if(rows_on_current > 10)
    rows_on_current = 10;

  if(rows_on_current > rows_on_last)
  {
    for(int i = 0 ; i < rows_on_last; i++)
    {
      leds[column_one[i]] = ColorFromPalette( currentPalette,  i*sp, brightness, LINEARBLEND);
      leds[column_two[i]] = ColorFromPalette( currentPalette, i*sp, brightness, LINEARBLEND);
      leds[column_three[i]] = ColorFromPalette( currentPalette, i*sp, brightness, LINEARBLEND);
    }
    
    for( int i = rows_on_last; i < rows_on_current; i++) {
      leds[column_one[i]] = ColorFromPalette( currentPalette, i*sp, brightness, LINEARBLEND);
      leds[column_two[i]] = ColorFromPalette( currentPalette,  i*sp, brightness, LINEARBLEND);
      leds[column_three[i]] = ColorFromPalette( currentPalette, i*sp, brightness, LINEARBLEND);
      FastLED.show();
      delay(80/(rows_on_current - rows_on_last));
    }
  }
  else if (rows_on_current < rows_on_last)
  {
    for( int i = 0; i < rows_on_last; i++) {
      leds[column_one[i]] = ColorFromPalette( currentPalette, i*sp, brightness, LINEARBLEND);
      leds[column_two[i]] = ColorFromPalette( currentPalette,i*sp, brightness, LINEARBLEND);
      leds[column_three[i]] = ColorFromPalette( currentPalette, i*sp, brightness, LINEARBLEND);
    }
    for( int i = rows_on_last; i < 10; i++) {
      leds[column_one[i]] = ColorFromPalette( currentPalette, i*sp, 0, LINEARBLEND);
      leds[column_two[i]] = ColorFromPalette( currentPalette, i*sp, 0, LINEARBLEND);
      leds[column_three[i]] = ColorFromPalette( currentPalette,  i*sp, 0, LINEARBLEND);
    }
    
    for( int i = rows_on_last - 1; i >= rows_on_current; i--) {
      leds[column_one[i]] = ColorFromPalette( currentPalette, i*sp, 0, LINEARBLEND);
      leds[column_two[i]] = ColorFromPalette( currentPalette, i*sp, 0, LINEARBLEND);
      leds[column_three[i]] = ColorFromPalette( currentPalette,i*sp, 0, LINEARBLEND);
      FastLED.show();

      delay(80/(rows_on_last - rows_on_current));
    }
  }
  else{
    for( int i = 0; i < rows_on_last; i++) {
      leds[column_one[i]] = ColorFromPalette( currentPalette, i*sp, brightness, LINEARBLEND);
      leds[column_two[i]] = ColorFromPalette( currentPalette, i*sp, brightness, LINEARBLEND);
      leds[column_three[i]] = ColorFromPalette( currentPalette, i*sp, brightness, LINEARBLEND);
    }
    delay(80);
  }
 FastLED.show();
 //startIndex++;
 rows_on_last = rows_on_current;
}

