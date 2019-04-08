/**
 * @file mainwindow.h
 *
 *
 * @authors Mohith Bhargav Sunkara
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "source/utils/spikingSudokuSolver.hpp"

#include <memory>

#include <QWidget>
#include <QtGui>
#include "sudokumatrixwidget.h"



QT_BEGIN_NAMESPACE class QPushButton;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QGroupBox;
class QMessageBox;
class QLineEdit;
class QLabel;
class QString;
class QKeyEvent;
class QProcess;
QT_END_NAMESPACE class MainWindow:public QWidget
{
Q_OBJECT public:
  explicit MainWindow (QWidget * parent = 0);
  ~MainWindow ();
  int row;
  int col;
  bool activate;
  bool painterActiver;
  QString folder;
  QString Folder_to_Save;
  int crop_1;
  int crop_2;


private:
  QPushButton * butSolve;
  QPushButton *butAbout;
  QPushButton *UpdateSudoku;
  QPushButton *butFile;
  QPushButton *butVideo;
  QPushButton *butParams;
  QComboBox *simulatorBox;
  QComboBox *solverBox;
  QCheckBox *cubeBox;

  QLineEdit *rowgrey;
  QLineEdit *columngrey;

  QLabel *rows;
  QLabel *columns;
  QLabel *solved;
  QLabel *Solvers;
  QLabel *Simulators;

  QHBoxLayout *HBox;
  QHBoxLayout *HBox2;
  QHBoxLayout *HBox3;

  QGridLayout *AllContent1;

  QGridLayout *Grid2;

  QGroupBox *SudokuSimulators;
  QGroupBox *GroupTools;
  QGroupBox *SudokuUpdater;
  QGroupBox *GroupSudoku;

  QScrollBar *ScrubBar;

  vector < vector < vector < int >>>result;

  SudokuMatrixWidget *SudokuMain;

  Sudoku sudoku;
  std::shared_ptr < spikingSudokuSolver > solver;
  int sudoku_size_widget;	// Size of the Sudoku for cropping

  std::string home, old_path;
  bool first_time = true;
  std::string parameter_file = "";



  public slots:void showAboutBox () const;
  void updateSudokuBox ();
  void keyPressEvent (QKeyEvent * event);
  void sudokuspikingsolver ();
  void scrubThroughResult (int bin);
  void fromFile ();
  void paramsFromFile ();
  void makeVideo ();
};

#endif // MAINWINDOW_H
