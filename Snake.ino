#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define JOY_X A1
#define JOY_Y A0
#define JOY_SW 2

#define GRID_SIZE 8
#define GRID_WIDTH SCREEN_WIDTH/GRID_SIZE
#define GRID_HEIGHT SCREEN_HEIGHT/GRID_SIZE

#define SNAKE_SPEED 1

uint16_t scene = 1;//1 - menu, 2 - snake, 3 - deadScreen, 4 - dino;

const char* text1;
const char* text2;
const char* text3;

short int list = 1;

unsigned long lastMoveTime = 0;
uint8_t moveDelay = 1000;

unsigned long lastReset = 0;
const unsigned long resetInterval = 400000;


uint16_t yValue = 0;
uint16_t xValue = 0;

uint8_t snakeDirection = 4; //1 = 0, 2 = 90, 3 = 180, 4 = -90; 

uint8_t score = 0;
uint8_t bestScore = 0;
uint8_t snakePosttionY = 0;
uint8_t snakePosttionX = 0;
uint8_t snakeTail = 0;

uint8_t snake[GRID_HEIGHT][GRID_WIDTH]{
  {253,253,253,253,253,253,253,254,254,254,254,254,254,254,254,254},
  {254,0,0,0,0,0,0,0,0,0,0,0,0,0,0,254},
  {254,0,0,0,0,0,0,0,0,0,0,0,0,0,0,254},
  {254,0,0,0,0,0,0,0,0,1,0,0,0,0,0,254},
  {254,0,0,0,0,0,224,0,0,0,0,0,0,0,0,254},
  {254,0,0,0,0,0,0,0,0,0,0,0,0,0,0,254},
  {254,0,0,0,0,0,0,0,0,0,0,0,0,0,0,254},
  {254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,254}
};

//0 = empty, 1 = snake`s had,2 snake`s body, 224 = apple;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void draw();
void update();
void menuSwich();
void menuDraw();
void snakeDraw();
void snakeUpdate();
void Appeleat();
void end();
void resetSnake();
void DeadScreenDraw();

void setup() {
  Serial.begin(9600);
  pinMode(JOY_SW, INPUT_PULLUP);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Loh ti koroche");
    for(;;);
  }
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,0);
  display.println("  Made by\n");
  display.println("  BersBot");
  display.println("    :)");
  resetSnake();
  display.display();
  delay(1500);
}

void loop() {
  update();

  draw();
} 

void update(){
  if (millis() - lastReset > resetInterval) {
    display.ssd1306_command(SSD1306_DISPLAYOFF);
    delay(10);
    display.ssd1306_command(SSD1306_DISPLAYON);
    lastReset = millis();
  }

  if (bestScore < score){
    bestScore = score;
  }
  xValue = analogRead(JOY_X);
  yValue = analogRead(JOY_Y);

  if (list == 1 && digitalRead(JOY_SW) == 0 && scene == 1){
    scene = 2;
  }

  if (scene == 1){
    menuSwich();
  }
  else if (scene == 2){
    snakeUpdate();
  }
  else if (scene == 3){
    DeadScreenDraw();
  }

}

void draw(){
  display.clearDisplay();

  if (scene == 1){
    menuDraw();
  }
  else if (scene == 2){
    snakeDraw();
  }
  else if (scene == 3){
    DeadScreenDraw();
  }

  display.display();
}

void menuDraw(){
  display.setCursor(0, 0);
  if(list == 1){
    display.setTextSize(2);
    text1 = "Eat Blocks\n";
  }else{
    text1 = "Eat Blocks \n\n";
    display.setTextSize(1);
  }
  display.println(text1);

  if(list == 2){
    display.setTextSize(2);
    text2 = "Dino \n";
  }else{
    text2 = "Dino \n\n";
    display.setTextSize(1);
  }
  display.println(text2);

  if(list == 3){
    display.setTextSize(2);
    text3 = "67 \n";
  }else{
    text3 = "67 \n\n";
    display.setTextSize(1);
  }
  display.println(text3);
}

void menuSwich(){

  moveDelay = 400;
  unsigned long now = millis();

  if (now - lastMoveTime > moveDelay) {

    if (yValue > 950) {
      list++;
      if (list > 3) list = 1;
      lastMoveTime = now;
    }

    if (yValue < 50) {
      list--;
      if (list < 1) list = 3;
      lastMoveTime = now;
    }

  }
  //Serial.print(list);

}

void snakeDraw(){

  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.print(" ");
  display.print(score);
  display.print(" | ");
  display.print(bestScore);


  for(uint8_t i = 0; i < GRID_HEIGHT; i++){
    for(uint8_t j = 0; j < GRID_WIDTH; j++){
      if(snake[i][j] == 1){
        display.fillRect(j*GRID_SIZE, i*GRID_SIZE, GRID_SIZE, GRID_SIZE, SSD1306_WHITE);
      }
      else if(snake[i][j] == 224){
        display.drawRect(j*GRID_SIZE, i*GRID_SIZE, GRID_SIZE, GRID_SIZE, SSD1306_WHITE);
      }
      else if(snake[i][j] == 254){
        display.fillRect(j*GRID_SIZE, i*GRID_SIZE, GRID_SIZE, GRID_SIZE, SSD1306_WHITE);
      }
      else if(snake[i][j] == 253){
        display.fillRect(j*GRID_SIZE, i*GRID_SIZE+7, GRID_SIZE, 1, SSD1306_WHITE);
      }
    }
  }
}

void snakeUpdate(){
  moveDelay = 1000;
  for (uint8_t i = 0; i < GRID_HEIGHT; i++) {
    for (uint8_t j = 0; j < GRID_WIDTH; j++) {

      if (i == 0 || i == GRID_HEIGHT-1 || j == 0 || j == GRID_WIDTH-1){
        snake[i][j] = 254;
      }

    }
  }

  snake[0][0] = 253; 
  snake[0][1] = 253; 
  snake[0][2] = 253; 
  snake[0][3] = 253; 
  snake[0][4] = 253; 
  snake[0][5] = 253; 
  snake[0][6] = 253; 

  
  unsigned long now = millis();
  if (yValue > 800){
    snakeDirection = 3;//0
  }else if(yValue < 200){
    snakeDirection = 1;//180
  }else if(xValue > 800){
    snakeDirection = 4;//90
  }else if(xValue < 200){
    snakeDirection = 2;//-90
  }

  Serial.print(snakeDirection);

  for(uint8_t i = 0; i < GRID_HEIGHT; i++){
    for(uint8_t j = 0; j < GRID_WIDTH; j++){
      if(snake[i][j] == 1){
        snakePosttionY = i;
        snakePosttionX = j;
      }
    }
  }

  if (now - lastMoveTime > moveDelay) {
    snake[snakePosttionY][snakePosttionX] = 0;
    switch(snakeDirection){
      case 1:
        if (snake[snakePosttionY-1][snakePosttionX] == 224){
          Appeleat();
        }else if(snake[snakePosttionY-1][snakePosttionX] == 254 || snake[snakePosttionY-1][snakePosttionX] == 253){
          end();
        }
        snake[snakePosttionY-1][snakePosttionX] = 1;
        lastMoveTime = now;
        break;
      case 3:
        if (snake[snakePosttionY+1][snakePosttionX] == 224){
          Appeleat();
        }else if(snake[snakePosttionY+1][snakePosttionX] == 254 || snake[snakePosttionY+1][snakePosttionX] == 253){
          end();
        }
        snake[snakePosttionY+1][snakePosttionX] = 1;
        lastMoveTime = now;
        break;
      case 2:
        if (snake[snakePosttionY][snakePosttionX+1] == 224){
          Appeleat();
        }else if(snake[snakePosttionY][snakePosttionX+1] == 254 || snake[snakePosttionY][snakePosttionX+1] == 253){
          end();
        }
        snake[snakePosttionY][snakePosttionX+1] = 1;
        lastMoveTime = now;
        break;


      case 4:
        if (snake[snakePosttionY][snakePosttionX-1] == 224){
          Appeleat();
        }else if(snake[snakePosttionY][snakePosttionX-1] == 254 || snake[snakePosttionY][snakePosttionX-1] == 253){
          end();
        }
        snake[snakePosttionY][snakePosttionX-1] = 1;
        lastMoveTime = now;
        break;
    }
  }
}
void Appeleat(){
  int rY, rX;
  int oldY = snakePosttionY;
  int oldX = snakePosttionX;
  do {
    rY = random(0, GRID_HEIGHT);
    rX = random(0, GRID_WIDTH);
  } while(snake[rY][rX] != 0);
  snake[rY][rX] = 224;
  
  do {
    rY = random(0, GRID_HEIGHT);
    rX = random(0, GRID_WIDTH);
  } while(snake[rY][rX] != 0);
  snake[rY][rX] = 254;
  score++;
}
void end(){
  scene = 3;
  score = 0;
  resetSnake();
}
void resetSnake() {
  for (uint8_t i = 0; i < GRID_HEIGHT; i++) {
    for (uint8_t j = 0; j < GRID_WIDTH; j++) {

      if (i == 0 || i == GRID_HEIGHT-1 || j == 0 || j == GRID_WIDTH-1)
        snake[i][j] = 254;
      else
        snake[i][j] = 0;

    }
  }
  snake[0][0] = 253; 
  snake[0][1] = 253; 
  snake[0][2] = 253; 
  snake[0][3] = 253; 
  snake[0][4] = 253; 
  snake[0][5] = 253; 
  snake[0][6] = 253; 
  snake[3][9] = 1;    
  snake[4][6] = 224;  
}

void DeadScreenDraw(){
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.print("  You`re\n\nDead & LOX");
  if (digitalRead(JOY_SW) == 0 && scene == 3){
    scene = 1;
  }
  delay(100);
}