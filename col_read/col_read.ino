#include <Servo.h>

// Game information
const int NUM_ROW = 6;
const int NUM_COL = 7;
int board[NUM_ROW][NUM_COL];
int cols[NUM_COL];
int nextPlayer = 1;
int aiPlayer = 1;


// Sensor config.
const int numSensors = 7;
const int sensorPins[numSensors] = {
  0, 1, 2, 3, 4, 5, 6};

// Servo config.
const int colSelectorServoPin = 49;
const int servoPositions[] = {0, 6, 12, 18, 24, 32, 38};
const int dispenserServoPin = 51;

// Detection parameters.
const int checkLoopMS = 0;
const int debounceDelayMS = 0;
const int thresholdPerSensor[numSensors] = {
  850, 975, 125, 975, 800, 800, 850};

// Ramp physics parameters.
const int rampRotateMS = 1000;
const int coinRollMS = 1000;
const int dispenseMS = 1000;

// Private state for debouncing.
int lastReading[numSensors];
int lastConfirmedState[numSensors];
long lastReadingChangeMS[numSensors];

// Private state for ramp servo.
Servo colSelectorServo;
Servo dispenserServo;

void clearBoard() {
  for (int row = 0; row < NUM_ROW;row++)
    for (int col = 0; col<NUM_COL; col++)
    {
      board[row][col]=0;
    }
  for (int col = 0 ; col < NUM_COL; col++)
    cols[col] = 0;
}

void setup() {
  // Zero debounce state.
  for (int i = 0; i < numSensors; i++) {
    lastReading[i] = 0;
    lastConfirmedState[i] = 0;
    lastReadingChangeMS[i] = 0L;
  }

  colSelectorServo.attach(colSelectorServoPin);
  colSelectorServo.write(servoPositions[0]);
  dispenserServo.attach(dispenserServoPin);
  dispenserServo.write(0);

  Serial.begin(9600);

  //init board state
  clearBoard();
}

void loop() {
  if (!gameOver()) {
    if (nextPlayer == aiPlayer) {
      // if it's the AI's turn, move for the AI
      int col = aiMove();
      updateBoardWithMove(col);
      dispenseInCol(col);
    } else {
      //human's turn
      boardRead();
    }
    delay(checkLoopMS);
  }
}

void commandRead() {
  if (Serial.available()) {
    int inCol = Serial.read() - 48;
    if (inCol >= 0 && inCol <= 6) {
      dispenseInCol(inCol);
    }
  }
}

void dispenseInCol(int col) {
  colSelectorServo.write(servoPositions[col]);
  delay(rampRotateMS);
  dispenserServo.write(90);
  delay(dispenseMS);
  delay(coinRollMS);
  dispenserServo.write(0);
  colSelectorServo.write(servoPositions[0]);
  delay(rampRotateMS);
}

void boardRead() {
  for (int i = 0; i < numSensors; i++) {
    int reading = getReading(i);
    int newState = confirmState(i, reading, lastConfirmedState[i]);
    if(newState != lastConfirmedState[i]) {
      lastConfirmedState[i] = newState;
      if (newState && !lastConfirmedState[i]) {
        // rising edge (when a piece blocks the sensor)
        updateBoardWithMove(i);
      }
    }
  }
//  Serial.println("");
}

int confirmState(int i, int reading, int lastConfirmedState) {
  if (reading != lastReading[i]) {
    lastReadingChangeMS[i] = millis();
  }
  int confirmedState = lastConfirmedState;
  // A reading is only confirmed when it's been in that state for more than debounceDelayMS.
  if ((millis() - lastReadingChangeMS[i]) > debounceDelayMS) {
    confirmedState = reading;
  }
  lastReading[i] = reading;
  return confirmedState;
}

int getReading(int i) {
  int sensorPin = sensorPins[i];
  int val = analogRead(sensorPin);
//  Serial.print(i);
//  Serial.print("=");
//  Serial.print(val);
//  Serial.print(" ");
  int reading = (int)(val > thresholdPerSensor[i]);
  return reading;
}

void updateBoardWithMove(int col) {
  if ( col<0 || col >= NUM_COL)
    Serial.println("invalid input\n\n");
  else{
    if (cols[col]==NUM_ROW)
      Serial.println("Column full");
    else{
      int m_x= 5-cols[col];
      cols[col]++;
      board[m_x][col] = nextPlayer;
      nextPlayer = 3-nextPlayer;
    }
  }
}

bool gameOver() {
  int col, row;

  // Check for horizontal victory conditions
  for (col=0; col<NUM_COL-3; col++) {
    for (row=0; row<NUM_ROW; row++) {
      if (board[row][col] != 0 &&
          board[row][col] == board[row][col+1] &&
          board[row][col] == board[row][col+2] &&
          board[row][col] == board[row][col+3])
        return true;
    }
  }

  // Check for vertical victory conditions
  for (col=0; col<NUM_COL; col++) {
    for (row=0; row<NUM_ROW-3; row++) {
      if (board[row][col] != 0 &&
          board[row][col] == board[row+1][col] &&
          board[row][col] == board[row+2][col] &&
          board[row][col] == board[row+3][col])
        return true;
    }
  }

  // Check for one diagonal victory conditions
  for (col=0; col<NUM_COL-3; col++) {
    for (row=0; row<NUM_ROW-3; row++) {
      if (board[row][col] != 0 &&
          board[row][col] == board[row+1][col+1] &&
          board[row][col] == board[row+2][col+2] &&
          board[row][col] == board[row+3][col+3])
        return true;
    }
  }

  // Check for the other diagonal victory conditions
  for (col=3; col<NUM_COL; col++) {
    for (row=0; row<NUM_ROW-3; row++) {
      if (board[row][col] != 0 &&
          board[row][col] == board[row+1][col-1] &&
          board[row][col] == board[row+2][col-2] &&
          board[row][col] == board[row+3][col-3])
        return true;
    }
  }

  return false;
}

void printBoard(){
  Serial.println("   0 1 2 3 4 5 6");
  for (int i = 0; i < 6; i++){
    Serial.print(i+ ": ");
    for (int j = 0; j< 7; j++){
      Serial.print(board[i][j]);
      Serial.print(" ");
    }
    Serial.println();
  }
  Serial.println();
}

bool check_row(int r) { return ((r>=0) && (r <=5));}
bool check_col(int c) { return ((c>=0) && (c <=6));}
bool valid(int i, int j) {return ((check_row(i)) && (check_col(j)));}

// returns the column the ai wants to play in
int aiMove() {
  int[][] bo=board;
  int[] c = cols;
  int me = nextPlayer;
  int oppo = 3 - me;

  int[] h= new int[7];
  for (int i = 0; i < 7; i++) h[i]=0;


  for (int i=0; i<7; i++)
  {
    if (c[i]==6) h[i]=0; else{
      int col = i;
      int row = 5 - c[i];


      if ((col>=3)
          && (bo[row][col-1] == me)
          && (bo[row][col-2] == me)
          && (bo[row][col-3] == me))
        h[i]=h[i]+16;
      //right
      if ((col<=3)
          && (bo[row][col+1] == me)
          && (bo[row][col+2] == me)
          && (bo[row][col+3] == me))
        h[i]=h[i]+16;
      //check y direction
      if ((row<=2)
          && (bo[row+1][col] == me)
          && (bo[row+2][col] == me)
          && (bo[row+3][col] == me))
        h[i]=h[i]+16;
      //check left diagonal
      if ((col>=3) && (row<=2)
          && (bo[row+1][col-1] == me)
          && (bo[row+2][col-2] == me)
          && (bo[row+3][col-3] == me))
        h[i]=h[i]+16;

      if ((col<=3) && (row<=2)
          && (bo[row+1][col+1] == me)
          && (bo[row+2][col+2] == me)
          && (bo[row+3][col+3] == me))
        h[i]=h[i]+16;

      if ((col>=3) && (row>=3)
          && (bo[row-1][col-1] == me)
          && (bo[row-2][col-2] == me)
          && (bo[row-3][col-3] == me))
        h[i]=h[i]+16;

      if ((col<=3) && (row>=3)
          && (bo[row-1][col+1] == me)
          && (bo[row-2][col+2] == me)
          && (bo[row-3][col+3] == me))
        h[i]=h[i]+16;

      if ((col>=2)
          && (bo[row][col-1] == me)
          && (bo[row][col-2] == me))
        h[i]=h[i]+4;
      //right
      if ((col<=4)
          && (bo[row][col+1] == me)
          && (bo[row][col+2] == me))
        h[i]=h[i]+4;
      //check y direction
      if ((row<=3)
          && (bo[row+1][col] == me)
          && (bo[row+2][col] == me))
        h[i]=h[i]+4;
      //check left diagonal
      if ((col>=2) && (row<=3)
          && (bo[row+1][col-1] == me)
          && (bo[row+2][col-2] == me))
        h[i]=h[i]+4;

      if ((col<=4) && (row<=3)
          && (bo[row+1][col+1] == me)
          && (bo[row+2][col+2] == me))
        h[i]=h[i]+4;

      if ((col>=2) && (row>=2)
          && (bo[row-1][col-1] == me)
          && (bo[row-2][col-2] == me))
        h[i]=h[i]+4;

      if ((col<=4) && (row>=2)
          && (bo[row-1][col+1] == me)
          && (bo[row-2][col+2] == me))
        h[i]=h[i]+4;

      if ((col>=1)
          && (bo[row][col-1] == me))
        h[i]=h[i]+2;
      //right

      if ((col<=5)
          && (bo[row][col+1] == me))
        h[i]=h[i]+2;
      //check y direction
      if ((row<=4)
          && (bo[row+1][col] == me))
        h[i]=h[i]+2;
      //check left diagonal
      if ((col>=1) && (row<=4)
          && (bo[row+1][col-1] == me))
        h[i]=h[i]+2;

      if ((col<=5) && (row<=4)
          && (bo[row+1][col+1] == me))
        h[i]=h[i]+2;

      if ((col>=1) && (row>=1)
          && (bo[row-1][col-1] == me))
        h[i]=h[i]+2;

      if ((col<=5) && (row>=1)
          && (bo[row-1][col+1] == me))
        h[i]=h[i]+2;

      //check x direction.
      //left
      if ((col>=3)
          && (bo[row][col-1] == oppo)
          && (bo[row][col-2] == oppo)
          && (bo[row][col-3] == oppo))
        h[i]=h[i]+8;
      //right
      if ((col<=3)
          && (bo[row][col+1] == oppo)
          && (bo[row][col+2] == oppo)
          && (bo[row][col+3] == oppo))
        h[i]=h[i]+8;
      //check y direction
      if ((row<=2)
          && (bo[row+1][col] == oppo)
          && (bo[row+2][col] == oppo)
          && (bo[row+3][col] == oppo))
        h[i]=h[i]+8;
      //check left diagonal
      if ((col>=3) && (row<=2)
          && (bo[row+1][col-1] == oppo)
          && (bo[row+2][col-2] == oppo)
          && (bo[row+3][col-3] == oppo))
        h[i]=h[i]+8;

      if ((col<=3) && (row<=2)
          && (bo[row+1][col+1] == oppo)
          && (bo[row+2][col+2] == oppo)
          && (bo[row+3][col+3] == oppo))
        h[i]=h[i]+8;

      if ((col>=3) && (row>=3)
          && (bo[row-1][col-1] == oppo)
          && (bo[row-2][col-2] == oppo)
          && (bo[row-3][col-3] == oppo))
        h[i]=h[i]+8;

      if ((col<=3) && (row>=3)
          && (bo[row-1][col+1] == oppo)
          && (bo[row-2][col+2] == oppo)
          && (bo[row-3][col+3] == oppo))
        h[i]=h[i]+8;

      if ((col>=2)
          && (bo[row][col-1] == oppo)
          && (bo[row][col-2] == oppo))
        h[i]=h[i]+4;
      //right
      if ((col<=4)
          && (bo[row][col+1] == oppo)
          && (bo[row][col+2] == oppo))
        h[i]=h[i]+4;
      //check y direction
      if ((row<=3)
          && (bo[row+1][col] == oppo)
          && (bo[row+2][col] == oppo))
        h[i]=h[i]+4;
      //check left diagonal
      if ((col>=2) && (row<=3)
          && (bo[row+1][col-1] == oppo)
          && (bo[row+2][col-2] == oppo))
        h[i]=h[i]+4;

      if ((col<=4) && (row<=3)
          && (bo[row+1][col+1] == oppo)
          && (bo[row+2][col+2] == oppo))
        h[i]=h[i]+4;

      if ((col>=2) && (row>=2)
          && (bo[row-1][col-1] == oppo)
          && (bo[row-2][col-2] == oppo))
        h[i]=h[i]+4;

      if ((col<=4) && (row>=2)
          && (bo[row-1][col+1] == oppo)
          && (bo[row-2][col+2] == oppo))
        h[i]=h[i]+4;

      if ((col>=1)
          && (bo[row][col-1] == oppo))
        h[i]=h[i]+2;
      //right

      if ((col<=5)
          && (bo[row][col+1] == oppo))
        h[i]=h[i]+2;
      //check y direction
      if ((row<=4)
          && (bo[row+1][col] == oppo))
        h[i]=h[i]+2;
      //check left diagonal
      if ((col>=1) && (row<=4)
          && (bo[row+1][col-1] == oppo))
        h[i]=h[i]+2;

      if ((col<=5) && (row<=4)
          && (bo[row+1][col+1] == oppo))
        h[i]=h[i]+2;

      if ((col>=1) && (row>=1)
          && (bo[row-1][col-1] == oppo))
        h[i]=h[i]+2;

      if ((col<=5) && (row>=1)
          && (bo[row-1][col+1] == oppo))
        h[i]=h[i]+2;
    }
  }
  int max = 0;
  int mm = 3;
  int sum = 0;
  for (int i=0; i<7; i++) {
    if (h[i]>max) {max=h[i]; mm=i;}
    sum= sum+h[i];
  }
  if (sum==0) mm = (int) (Math.random()*7);
  return mm;
}
