/**
 * @file main.cpp
 *
 *
 * @authors Mohith Bhargav Sunkara
 */
#include "mainwindow.h"

#include <QApplication>
#include <QtGui>

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "C");
	QApplication app(argc, argv);
	setlocale(LC_ALL, "C");
	MainWindow mainWindow;
	mainWindow.setAttribute(Qt::WA_QuitOnClose);
	mainWindow.show();

	return app.exec();
}
