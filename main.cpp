#include <time.h>
#include <iostream>
#include <string>
#include <curses.h>

int w = 16;
int h = 16;
int b = 40;

void prepareBoard(int board){
  for(int x=0;x<w;x++){
    for(int y=0;y<h;y++){
      board[x][y] = 0;
    }
  }
  for(int i=0;i<bombs;i++){
    bool success = false;
    while(success==false){
      int x = rand()%w;
      int y = rand()%h;
      if(board[x][y]==0){
        board[x][y]==9;
        success==true;
      }
    }
  }
}

void displayBoard(int board){
  
}

int main(){
  if(w<1)std::cout<<"Error: width must be a positive non-zero integer.\n";
  if(h<1)std::cout<<"Error: height must be a positive non-zero integer.\n";
  if(b==0)std::cout<<"Error: there must be at least 1 bomb.\n";
  if(b>=w*h)std::cout<<"Error: bombs cannot match or exceed area.\n";
  initscr();
  cbreak();
  srand(time(NULL));
  bool playing = true
  while(playing){
    int board[w][h];
    prepareBoard(board);
    bool mainloop = true;
    while(mainloop){
      displayBoard(board);
      std::string input = "";
      std::cin >> input;
      std::cin.clear();
      
    }
  }
  endwin();
}
