//www.elegoo.com
//2016.12.9

#include <LiquidCrystal.h>
#include <LedControl.h>
#define DEAD_ZONE 200
#define MAX_SNAKE_LENGTH 64


bool showingTemperature;
//LCD screen pins BS  E  D4 D5  D6 D7
LiquidCrystal lcd(6, 7, 8, 9, 10, 11);
//MAX7219 pins             DIN,CS,CLK
LedControl lc = LedControl(4, 2, 3, 1);
//Joystick pins
const int SW_pin = 13;  
const int X_pin = 0;    
const int Y_pin = 1;
//LED pin
const int LED_pin = 12;

unsigned long delayTime = 100;
int snakeLength;

int snakeX[MAX_SNAKE_LENGTH];
int snakeY[MAX_SNAKE_LENGTH];

int inputX;
int inputY;

//current X and Y direction of the snake
int currentX;
int currentY;
//current X and Y coordinates of the fruit
int fruitX;
int fruitY;

int score;
bool hasCrashed;
bool hasHitFruit;
bool inGame;

void setup() {

  //LC setup
  lc.shutdown(0, false);
  lc.setIntensity(0, 5);
  lc.clearDisplay(0);

  //LCD setup
  lcd.begin(16, 2);
  lcd.print("!!SNAKE!!");
  lcd.setCursor(0, 1);
  lcd.print("press 2 play!");

  //LED setup
  pinMode(LED_pin, OUTPUT);

  hasHitFruit = false;
  hasCrashed = false;

  //draws snake
  resetSnake();
  drawSnake();

  //default snake movement
  currentX = 1;
  currentY = 0;

  score = 0;
}

void loop() {

  if (inGame) {
    inputX = analogRead(X_pin);
    inputY = analogRead(Y_pin);

    moveSnake(inputX, inputY);
    hasCrashed = checkCollision();
    hasHitFruit = checkFruit();

    if(hasHitFruit){
      increaseLength();
      updateScore();
      generateFruit();
      analogWrite(LED_pin, 255);    
    }
    else {
      analogWrite(LED_pin, 0);    
    }

    if (hasCrashed) {
      inGame = false;
      hasCrashed = false;
      lcd.clear();
      lcd.print("GAME OVER!");
      lcd.setCursor(0, 1);
      lcd.print("Final score: ");
      lcd.print(score);
    } else {      
      lc.clearDisplay(0);
      drawFruit();
      drawSnake();
      delay(delayTime);
    }

  } else {
    if (digitalRead(SW_pin) == LOW) {
      inGame = true;
      score = 0;
      lcd.clear();
      lcd.print("Score: ");
      lcd.setCursor(0, 1);
      lcd.print(score);
    } else {
      resetSnake();
    }
  }
}

void drawSnake() {
  for (int i = 0; i < snakeLength; i++) {
    lc.setLed(0, snakeX[i], snakeY[i], true);
  }
}

void drawFruit(){
  lc.setLed(0, fruitX, fruitY, true);
}

void resetSnake() {
  snakeX[0] = 4;
  snakeY[0] = 4;
  snakeX[1] = 5;
  snakeY[1] = 4;
  snakeX[2] = 6;
  snakeY[2] = 4;
  snakeLength = 3;
}

void generateFruit(){
  fruitX = 7 - snakeX[0];
  fruitY = 7 - snakeX[0];
}

bool moveSnake(int x, int y) {

  if (x < 512 - DEAD_ZONE && currentX != 1) {
    updateSnakeCoords(-1, 0);
    currentX = -1;
    currentY = 0;
  } else if (x > 512 + DEAD_ZONE && currentX != -1) {
    updateSnakeCoords(1, 0);
    currentX = 1;
    currentY = 0;
  } else if (y > 512 + DEAD_ZONE && currentY != 1) {
    updateSnakeCoords(0, -1);
    currentX = 0;
    currentY = -1;
  } else if (y < 512 - DEAD_ZONE && currentY != -1) {
    updateSnakeCoords(0, 1);
    currentX = 0;
    currentY = 1;
  }
  else updateSnakeCoords(currentX, currentY);

}

void updateSnakeCoords(int xOffset, int yOffset) {
  for (int i = snakeLength - 1; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }
  snakeX[0] = snakeX[0] + xOffset;
  snakeY[0] = snakeY[0] + yOffset;

  if (snakeX[0] > 7) snakeX[0] = 0;
  else if (snakeX[0] < 0) snakeX[0] = 7;

  if (snakeY[0] > 7) snakeY[0] = 0;
  else if (snakeY[0] < 0) snakeY[0] = 7;
}

void increaseLength() {
  snakeX[snakeLength] = snakeX[snakeLength - 1] + currentX;
  snakeY[snakeLength] = snakeY[snakeLength - 1] + currentY;
  snakeLength++;
}

void updateScore() {
  score++;
  lcd.setCursor(0, 1);
  lcd.print(score);
}

bool checkCollision() {
  int headX = snakeX[0];
  int headY = snakeY[0];

  for (int i = 1; i < snakeLength; i++) {
    if (snakeX[i] == headX && snakeY[i] == headY) return true;
  }
  return false;
}

bool checkFruit() {
  int headX = snakeX[0];
  int headY = snakeY[0];
  if (headX == fruitX && headY == fruitY) {
    return true;
  }
  else return false;
}
