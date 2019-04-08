/**
 * @file sudokumatrixWidget.hpp
 *
 *
 * @authors Mohith Bhargav Sunkara
 */
#ifndef SUDOKUMATRIXWIDGET_H
#define SUDOKUMATRIXWIDGET_H

#include<QWidget>
#include<QtGui>
#include<iostream>
#include<list>
#include "source/utils/Sudoku.hpp"

QT_BEGIN_NAMESPACE class QPainter;
class QPaintEvent;
class QString;
class QMessageBox;
class QFont;
QT_END_NAMESPACE class SudokuMatrixWidget:public QWidget
{
Q_OBJECT public:
  explicit SudokuMatrixWidget (QWidget * parent);
  ~SudokuMatrixWidget ();
  void DrawMatrix (QPainter & painter);

  void ChangeRowColumnValue (int row, int column);
  void ChangeInnerValues (Sudoku s);
  void ChangeInnerResults (Sudoku sim, bool let);
  void ChangePainterState (bool paintme);

protected:
  void paintEvent (QPaintEvent * event);

private:
  int row_grey;
  int columns_grey;
  bool wakeup;
  bool toggle_Painter;
  int row_class;
  int col_class;
  int obtained_value;
  Sudoku sudoku;
  Sudoku sudokuresult;
};

#endif // SUDOKUMATRIXWIDGET_H
