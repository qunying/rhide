/*---------------------------------------------------------*/
/*                                                         */
/*   Turbo Vision 1.0                                      */
/*   Copyright (c) 1991 by Borland International           */
/*                                                         */
/*   Puzzle.h : Header file for puzzle.cpp                 */
/*---------------------------------------------------------*/

#if !defined( __PUZZLE_H )
#define __PUZZLE_H

class TPuzzleView:public TView
{

public:

  TPuzzleView(TRect & r);
  virtual TPalette & getPalette() const;
  virtual void handleEvent(TEvent & event);
  virtual void draw();
  void moveKey(int key);
  void moveTile(TPoint point);
  void scramble();
  void winCheck();

private:

  char board[6][6];
  int moves;
  char solved;

};

class TPuzzleWindow:public TWindow
{

public:

  TPuzzleWindow();

};

#endif // __PUZZLE_H
