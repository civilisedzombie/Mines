/*****************************************************************************/

//TODO:
// -Flagging numbers should flag hidden tiles arround them
// -Change to cursor control instead of commands
// -Fix game occasionally not restarting
// -Fix wrapping floodfill
// -Fix mass fill not floodfilling

/*****************************************************************************/
#include <iostream>
#include <time.h>
#include <string>
#include <vector>

const int width = 16;
const int height = 16;
const int bombs = 40;
int end = 0;

struct vec2{
  int x;
  int y;
  
  vec2(int a, int b){
    x = a;
    y = b;
  }
};

std::vector<vec2> ffbq;

struct Row{
  int c[width];
};

struct Board{
  Row r[height];
  /**********************************************/
  //Places bombs
  void placeBombs(){
    for(int i=0;i<bombs;i++){
      bool success = false;
      while(success == false){
        int c = rand()%width;
        int row = rand()%height;
        if(r[row].c[c] == 0){
          r[row].c[c] = 9;
          success = true;
        }
      }
    }
  }
  /**********************************************/
  //called on board initialization after placeBombs. Calculates cell values.
  void determineNumbers(){
    for(int i=0;i<height;i++){
      for(int j=0;j<width;j++){
        if(r[i].c[j] == 9)continue;
        int bombs[] = {0,0,0,0,0,0,0,0};
        int total = 0;
        if(i==0){bombs[0] = 1; bombs[1] = 1; bombs[2] = 1;}
        if(i==height-1){bombs[4] = 1; bombs[5] = 1; bombs[6] = 1;}
        if(j==0){bombs[0] = 1; bombs[6] = 1; bombs[7] = 1;}
        if(j==width-1){bombs[2] = 1; bombs[3] = 1; bombs[4] = 1;}
        for(int k=0;k<8;k++){
          if(bombs[k]==1)continue;
          if(
            (k==0 && r[i-1].c[j-1] == 9)||
            (k==1 && r[i-1].c[j] == 9)||
            (k==2 && r[i-1].c[j+1] == 9)||
            (k==3 && r[i].c[j+1] == 9)||
            (k==4 && r[i+1].c[j+1] == 9)||
            (k==5 && r[i+1].c[j] == 9)||
            (k==6 && r[i+1].c[j-1] == 9)||
            (k==7 && r[i].c[j-1] == 9)
          )total++;
        }
        r[i].c[j] = total;
      }
    }
  }
  /**********************************************/
  Board(){
    placeBombs();
    determineNumbers();
  }
  /**********************************************/
  //Draw board.
  //Values 0 to 9 are hidden (▓)
  //10 is blank ( )
  //11 to 18 are numbered (value-10)
  //19 is a bomb (Ø)
  //20 to 29 are flagged (╕)
  void draw(){
    std::cout << "----------------------------------------\n";
    for(int i=0;i<height;i++){
      for(int j=0;j<width;j++){
        if(r[i].c[j] < 10){
          std::cout << "▓";
        }else if(r[i].c[j] == 10){
          std::cout << " ";
        }else if(r[i].c[j] < 19){
          std::cout << r[i].c[j]-10;
        }else if(r[i].c[j] == 19){
          std::cout << "Ø";
        }else if(r[i].c[j] >= 20){
          std::cout << "╕";
        }
      }
      std::cout << "\n";
    }
  }
  /**********************************************/
};
/************************************************/
//Floodfill blank tiles
void floodFillUtil(Board* board, int x, int y, int prevC, int newC){
  // Base cases
  if(x < 0 || x >= width || y < 0 || y >= height)return;
  if(board->r[y].c[x] != prevC){
    vec2 tmp(x,y);
    ffbq.push_back(tmp);
    return;
  }
  if(board->r[y].c[x] == newC)return; 

  // Replace the color at (x, y)
  board->r[y].c[x] = newC;

  // Recur for north, east, south and west (and now NE, SE, SW, and NW too)
  floodFillUtil(board, x+1, y, prevC, newC);
  floodFillUtil(board, x-1, y, prevC, newC);
  floodFillUtil(board, x, y+1, prevC, newC);
  floodFillUtil(board, x, y-1, prevC, newC);
  floodFillUtil(board, x+1, y+1, prevC, newC);
  floodFillUtil(board, x+1, y-1, prevC, newC);
  floodFillUtil(board, x-1, y-1, prevC, newC);
  floodFillUtil(board, x-1, y+1, prevC, newC);
}

void floodFill(Board* board, int x, int y){
  int prevC = board->r[y].c[x];
  if(prevC==10)return;
  floodFillUtil(board, x, y, prevC, 10);
  for(int i=0;i<ffbq.size();i++){
    if(board->r[ffbq[i].y].c[ffbq[i].x] < 10){
      board->r[ffbq[i].y].c[ffbq[i].x] += 10;
    }
  }
  ffbq.clear();
}
/************************************************/
//Parse input and execute the command.
enum parseModes{
  READ_COMMAND,
  READ_ROW,
  READ_c
};

enum commandModes{
  NO_MODE,
  SHOW_TILE,
  TOGGLE_FLAG
};

void parse(std::string input, Board* board){
  int parsemode = READ_COMMAND;
  int commandmode = NO_MODE;
  int cx = 0;
  int cy = 0;
  std::string readnumber("");
  for(int i=0;i<sizeof(input);i++){
    if(input[i]==' '||input[i]==*"\n")continue;
    if(parsemode==READ_COMMAND){
      if(input[i]=='r'){
        parsemode = READ_ROW;
        readnumber.clear();
        continue;
      }else if(input[i]=='c'){
        parsemode = READ_c;
        readnumber.clear();
        continue;
      }else if(input[i]=='s'){
        commandmode = SHOW_TILE;
        continue;
      }else if(input[i]=='f'){
        commandmode = TOGGLE_FLAG;
        continue;
      }else{
        //std::cout << "Unknown command: " << input[i] << "\n";
        continue;
      }
    }else if(int(input[i])<58&&int(input[i])>47){
      readnumber += input[i];
    }else if(readnumber==""){
      if(parsemode==READ_ROW)std::cout<<"No value for row.\n";
      if(parsemode==READ_c)std::cout<<"No value for c.\n";
      return;
    }else{
      /******************************************/
      //Define row and c based on readnumber
      if(stoi(readnumber)<=0){
        std::cout << "Error: out of bounds.\n";
      }else if(parsemode==READ_ROW){
        if(stoi(readnumber)>height){
          std::cout << "Error: out of bounds.\n";
          return;
        }else if(cy!=0){
          std::cout << "Row already defined.\n";
          return;
        }else{
          cy = stoi(readnumber);
          parsemode = READ_COMMAND;
          i--;
          continue;
        }
        return;
      }else if(parsemode==READ_c){
        if(stoi(readnumber)>width){
          std::cout << "Error: out of bounds.\n";
        }else if(cx!=0){
          std::cout << "c already defined.\n";
        }else{
          cx = stoi(readnumber);
          parsemode = READ_COMMAND;
          i-=1;
          continue;
        }
        return;
      }
      parsemode = READ_COMMAND;
      /******************************************/
    }
  }
  if(commandmode==SHOW_TILE){
    //Reveal tile value
    if(board->r[cy-1].c[cx-1]>=20){
      std::cout << "Error: cannot reveal flagged tile.\n";
    }else if(board->r[cy-1].c[cx-1]>=10){
      int check[8] = {1,1,1,1,1,1,1,1};
      if(cy-2<=0){
        check[0] = 0;
        check[1] = 0;
        check[2] = 0;
      }
      if(cy>height){
        check[4] = 0;
        check[5] = 0;
        check[6] = 0;
      }
      if(cx-2<=0){
        check[0] = 0;
        check[7] = 0;
        check[6] = 0;
      }
      if(cx>width){
        check[2] = 0;
        check[3] = 0;
        check[4] = 0;
      }
      int tmp = 0;
      if(check[0]==1&&board->r[cy-2].c[cx-2]>19)tmp++;
      if(check[1]==1&&board->r[cy-2].c[cx-1]>19)tmp++;
      if(check[2]==1&&board->r[cy-2].c[cx]>19)tmp++;
      if(check[3]==1&&board->r[cy-1].c[cx]>19)tmp++;
      if(check[4]==1&&board->r[cy].c[cx]>19)tmp++;
      if(check[5]==1&&board->r[cy].c[cx-1]>19)tmp++;
      if(check[6]==1&&board->r[cy].c[cx-2]>19)tmp++;
      if(check[7]==1&&board->r[cy-1].c[cx-2]>19)tmp++;
      if(tmp==(board->r[cy-1].c[cx-1]-10)){
        if(check[0]==1&&board->r[cy-2].c[cx-2]<10)board->r[cy-2].c[cx-2] += 10;
        if(check[1]==1&&board->r[cy-2].c[cx-1]<10)board->r[cy-2].c[cx-1] += 10;
        if(check[2]==1&&board->r[cy-2].c[cx]<10)board->r[cy-2].c[cx] += 10;
        if(check[3]==1&&board->r[cy-1].c[cx]<10)board->r[cy-1].c[cx] += 10;
        if(check[4]==1&&board->r[cy].c[cx]<10)board->r[cy].c[cx] += 10;
        if(check[5]==1&&board->r[cy].c[cx-1]<10)board->r[cy].c[cx-1] += 10;
        if(check[6]==1&&board->r[cy].c[cx-2]<10)board->r[cy].c[cx-2] += 10;
        if(check[7]==1&&board->r[cy-1].c[cx-2]<10)board->r[cy-1].c[cx-2] += 10;
      }
      if(
        board->r[cy-2].c[cx-2] == 19 ||
        board->r[cy-2].c[cx-1] == 19 ||
        board->r[cy-2].c[cx] == 19   ||
        board->r[cy-1].c[cx] == 19   ||
        board->r[cy].c[cx] == 19     ||
        board->r[cy].c[cx-1] == 19   ||
        board->r[cy].c[cx-2] == 19   ||
        board->r[cy-1].c[cx-2] == 19
      )end = 1;
    }else if(board->r[cy-1].c[cx-1]==0){
      floodFill(board, cx-1, cy-1);
    }else if(board->r[cy-1].c[cx-1]==9){
      end = 1;
    }else{
      board->r[cy-1].c[cx-1] += 10;
      std::cout << "Tile shown.\n";
    }
  }else if(commandmode==TOGGLE_FLAG){
    //Toggle if tile is flagged
    if(board->r[cy-1].c[cx-1]>=20){
      board->r[cy-1].c[cx-1] -= 20;
      std::cout << "Tile unflagged.\n";
    }else if(board->r[cy-1].c[cx-1]>=10){
      std::cout << "Error: cannot flag revealed tile.\n";
      return;
    }else{
      board->r[cy-1].c[cx-1] += 20;
      std::cout << "Tile flagged.\n";
    }
  }else{
    //Handle lack of a command
    std::cout << "No command specified. s - show tile  f - toggle flag\n";
    return;
  }
}
/************************************************/
int main(){
  while(true){
	  srand (time(NULL));
    Board board;
    while(end==0){
      std::string input = "";
      board.draw();
      std::cin >> input;
      parse(input, &board);
      std::cin.clear();
    }
    if(end==1){
      for(int x=0;x<width;x++){
        for(int y=0;y<height;y++){
          if(
            board.r[y].c[x] == 9 ||
            board.r[y].c[x] == 29
          ){
            board.r[y].c[x] = 19;
          }
        }
      }
      board.draw();
      std::cout << "You lost. Play again? (y,n):\n";
      std::string a = "";
      std::cin >> a;
      if(a!="" && a!="y" && a!="Y")return 0;
      std::cin.clear();
      end = 0;
    }else if(end==2){
      std::cout << "You won! Play again? (y,n):\n";
      std::string a = "";
      std::cin >> a;
      if(a!="" && a!="y" && a!="Y")return 0;
      std::cin.clear();
      end = 0;
    }
  }
}
