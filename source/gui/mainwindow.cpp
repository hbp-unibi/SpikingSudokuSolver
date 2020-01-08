/**
 * @file mainwindow.cpp
 *
 *
 * @authors Mohith Bhargav Sunkara
 */

#include "mainwindow.h"

#include <qtconcurrentrun.h>
#include <QDebug>
#include <QFileDialog>
#include <QFuture>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <fstream>
#include <iostream>
#include <vector>

QT_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
	setMinimumSize(850, 850);
	const QSize btnSize = QSize(250, 30);
	butSolve = new QPushButton("Solve By Spiking");
	butSolve->setEnabled(false);
	butSolve->setFixedSize(btnSize);
	butAbout = new QPushButton("About Us");
	butAbout->setFixedSize(btnSize);
	UpdateSudoku = new QPushButton("Update Sudoku");
	butFile = new QPushButton("Get Sudoku");
	butVideo = new QPushButton("Make Video");
	butVideo->setFixedSize(btnSize);
	butVideo->setEnabled(false);

	simulatorBox = new QComboBox;
	simulatorBox->addItem(tr("pyNN.nest"));
	simulatorBox->addItem(tr("pyNN.spiNNaker"));
	simulatorBox->addItem(tr("pyNN.spiNNaker0.1"));
	simulatorBox->addItem(tr("spikey"));
	simulatorBox->addItem(tr("BrainScaleS"));
	simulatorBox->addItem(tr("GeNN-CPU"));
	simulatorBox->addItem(tr("GeNN-GPU"));
	simulatorBox->setFixedSize(btnSize);

	cubeBox = new QCheckBox("LEDCube");
	cubeBox->setFixedSize(QSize(120, 30));

	solverBox = new QComboBox;
	solverBox->addItem(tr("Spiking"));
	solverBox->addItem(tr("Spiking, Single Pop"));
	solverBox->addItem(tr("Spiking, Single Neuron"));
	solverBox->addItem(tr("Spiking, Mirror Inhib"));
	solverBox->setFixedSize(btnSize);

	butParams = new QPushButton("Parameter File");
	butParams->setFixedSize(btnSize);

	solved = new QLabel();
	solved->setAlignment(Qt::AlignCenter);
	ScrubBar = new QScrollBar(Qt::Horizontal);
	connect(ScrubBar, SIGNAL(valueChanged(int)), this,
	        SLOT(scrubThroughResult(int)));
	ScrubBar->setEnabled(false);

	rowgrey = new QLineEdit;
	columngrey = new QLineEdit;
	// Labes definied in the whole GUI
	rows = new QLabel("Rows:");
	columns = new QLabel("Columns:");
	const QSize LabSize = QSize(80, 30);
	Solvers = new QLabel("Solvers:");
	Solvers->setFixedSize(LabSize);
	Simulators = new QLabel("Simulators:");
	Simulators->setFixedSize(LabSize);

	AllContent1 = new QGridLayout;

	HBox = new QHBoxLayout;
	HBox->addWidget(butAbout);
	HBox->addWidget(butSolve);
	HBox->addWidget(butVideo);
	HBox->addWidget(cubeBox);

	HBox2 = new QHBoxLayout;
	HBox2->addWidget(rows);
	HBox2->addWidget(rowgrey);
	HBox2->addWidget(columns);
	HBox2->addWidget(columngrey);
	HBox2->addWidget(UpdateSudoku);
	HBox2->addWidget(butFile);

	HBox3 = new QHBoxLayout;
	HBox3->addWidget(Solvers);
	HBox3->addWidget(solverBox);
	HBox3->addWidget(butParams);
	HBox3->addWidget(Simulators);
	HBox3->addWidget(simulatorBox);

	SudokuSimulators = new QGroupBox("Sudoku Solver--Simulator Tools");
	SudokuSimulators->setLayout(HBox3);

	GroupTools = new QGroupBox("Application Tools");
	GroupTools->setLayout(HBox);

	SudokuUpdater = new QGroupBox("Sudoku Drawing Tools");
	SudokuUpdater->setLayout(HBox2);

	GroupSudoku = new QGroupBox();
	Grid2 = new QGridLayout;
	SudokuMain = new SudokuMatrixWidget(this);
	Grid2->addWidget(SudokuMain);
	GroupSudoku->setLayout(Grid2);
	GroupSudoku->setFlat(true);
	GroupSudoku->setStyleSheet("background-color: #FFFAF0");

	AllContent1->addWidget(SudokuSimulators, 1, 2, 1, 1);
	AllContent1->addWidget(GroupSudoku, 2, 2, 1, 1);
	AllContent1->addWidget(ScrubBar, 4, 2, 1, 1);
	AllContent1->addWidget(solved, 5, 2, 1, 1);
	AllContent1->addWidget(SudokuUpdater, 6, 2, 1, 1);
	AllContent1->addWidget(GroupTools, 7, 2, 1, 1);

	setLayout(AllContent1);
	setWindowTitle("Sudoku Solver Using Spiking Neural Networks - Test bed");

	painterActiver = false;
	SudokuMain->ChangePainterState(painterActiver);

	Folder_to_Save = "./Save_Sudoku_png/";
	QDir dir(Folder_to_Save);
	if (!dir.exists()) {
		dir.mkpath(".");
	}

	// QObject connectors area

	// Connecting the aboutus button
	QObject::connect(butAbout, SIGNAL(clicked()), this, SLOT(showAboutBox()));
	// Connecting the sudoku update button
	QObject::connect(UpdateSudoku, SIGNAL(clicked()), this,
	                 SLOT(updateSudokuBox()));
	// Connecting the sudoku slover button
	QObject::connect(butSolve, SIGNAL(clicked()), this,
	                 SLOT(sudokuspikingsolver()));
	// Connecting the butFile button to read json sudokufiles
	QObject::connect(butFile, SIGNAL(clicked()), this, SLOT(fromFile()));
	// Connecting the butParams button to read json parameter files
	QObject::connect(butParams, SIGNAL(clicked()), this,
	                 SLOT(paramsFromFile()));
	// Connecting the butVideo button to make videos on created folder
	QObject::connect(butVideo, SIGNAL(clicked()), this, SLOT(makeVideo()));

	// Initialize Path values
	old_path = std::string(getenv("PATH"));
	home = std::string(getenv("HOME"));
}

MainWindow::~MainWindow() {}

void MainWindow::showAboutBox() const
{
	QString Text =
	    "This application is developed for testing spiking neural networks on "
	    "solving sudoku part as of ISY-Project @ Bielefeld University";
	QMessageBox msg(QMessageBox::Information, "About", Text, QMessageBox::Ok);
	msg.exec();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_A) {
		// qInfo() << "hi i am left";
		int i = 20;
		GroupSudoku->scroll(i, 0);
	}
	if (event->key() == Qt::Key_D) {
		// qInfo() << "hi i am right";
		int j = 20;
		GroupSudoku->scroll(-j, 0);
	}
	if (event->key() == Qt::Key_W) {
		// qInfo() << "hi i am Up";
		int k = 20;
		GroupSudoku->scroll(0, k);
	}
	if (event->key() == Qt::Key_S) {
		// qInfo() << "hi i am down";
		int l = 20;
		GroupSudoku->scroll(0, -l);
	}
}
namespace {
int SudokuInWidgetSize(int row, int col)
{
	if (row == 2 && col == 2) {
		return 310;
	}
	else if (row == 2 && col == 3) {
		return 400;
	}
	else if (row == 2 && col == 4) {
		return 525;
	}
	else if (row == 2 && col == 5) {
		return 560;
	}
	else if (row == 3 && col == 3) {
		return 505;
	}
	else if (row == 3 && col == 4) {
		return 610;
	}
	else if (row == 3 && col == 5) {
		return 700;
	}
	else if (row == 4 && col == 4) {
		return 800;
	}
	else {
		std::cerr << "Warning: Unknown size of Sudoku for creation of videos!"
		          << std::endl;
		return 0;
	}
}
}  // namespace
void MainWindow::fromFile()
{

	ScrubBar->setEnabled(false);

	QString filename = QFileDialog::getOpenFileName(
	    this, tr("Open File"), QDir("../sudokus/").absolutePath(),
	    "All files (*.*);;Text File(*.json)");

	std::string fileload = filename.toUtf8().constData();

	if (!fileload.empty()) {
		sudoku = Sudoku(fileload);
		row = sudoku.getHeight();
		col = sudoku.getWidth();
		if (row > col) {
			col = sudoku.getHeight();
			row = sudoku.getWidth();
		}
		sudoku_size_widget = SudokuInWidgetSize(row, col);

		activate = true;
		painterActiver = true;

		SudokuMain->ChangePainterState(painterActiver);
		SudokuMain->ChangeRowColumnValue(row, col);
		SudokuMain->ChangeInnerValues(sudoku);
		SudokuMain->ChangeInnerResults(sudoku, activate);

		folder = Folder_to_Save +
		         QString("%1").arg(QDateTime::currentMSecsSinceEpoch()) + "--" +
		         QString(QVariant(row).toString()) + "x" +
		         QString(QVariant(col).toString());
		QDir dir(folder);
		if (!dir.exists()) {
			dir.mkpath(".");
		}

		butSolve->setEnabled(true);

		SudokuMain->update();
	}
}

void MainWindow::paramsFromFile()
{
	QString filename = QFileDialog::getOpenFileName(
	    this, tr("Open Parameter File"), QDir("../config/").absolutePath(),
	    "JSON File(*.json);;All files (*.*)");

	std::string fileload = filename.toUtf8().constData();
	if (!fileload.empty()) {
		parameter_file = filename.toStdString();
	}
}

void MainWindow::makeVideo()
{
	QString input_file = folder + "/" + "'%d.png'";

	QString output_file = folder + "/" + "sample_output.mp4";

	QString process_1 = "ffmpeg -framerate 5 -i";

	QString finial_process =
	    QString("%1 %2 %3").arg(process_1).arg(input_file).arg(output_file);

	char *cstr;
	std::string fname = finial_process.toStdString();
	cstr = new char[fname.size() + 1];
	strcpy(cstr, fname.c_str());

	int status = system(cstr);
	qDebug() << status;
}

void MainWindow::updateSudokuBox()
{
	ScrubBar->setEnabled(false);

	row = rowgrey->text().toInt();
	col = columngrey->text().toInt();
	if (row > col) {
		col = rowgrey->text().toInt();
		row = columngrey->text().toInt();
	}
	sudoku = Sudoku(row, col);
	sudoku_size_widget = SudokuInWidgetSize(row, col);

	activate = true;
	painterActiver = true;

	SudokuMain->ChangePainterState(painterActiver);
	SudokuMain->ChangeRowColumnValue(row, col);

	SudokuMain->ChangeInnerValues(sudoku);
	SudokuMain->ChangeInnerResults(sudoku, activate);

	butSolve->setEnabled(true);

	SudokuMain->update();

	folder = Folder_to_Save +
	         QString("%1").arg(QDateTime::currentMSecsSinceEpoch()) + "--" +
	         QString(QVariant(row).toString()) + "x" +
	         QString(QVariant(col).toString());
	QDir dir(folder);
	if (!dir.exists()) {
		dir.mkpath(".");
	}
}

void MainWindow::scrubThroughResult(int bin)
{
	sudoku = solver->updateSudokuFromResult(sudoku, result, bin);
	activate = false;
	painterActiver = true;

	SudokuMain->ChangePainterState(painterActiver);
	SudokuMain->ChangeInnerResults(sudoku, activate);

	if (sudoku.complete()) {
		solved->setStyleSheet("background-color:green;");
	}
	else {
		solved->setStyleSheet("background-color:red;");
	}

	int bin_size = solver->config()["bin_size"];
	QString bin_info = "";
	bin_info.append(QString::number(bin_size * bin));
	bin_info.append("-");
	bin_info.append(QString::number(bin_size * (bin + 1)));
	bin_info.append(" ms");
	solved->setText(bin_info);

	solved->show();
	SudokuMain->update();

	QPixmap pic;
	if (sudoku_size_widget > 0) {
		pic = QPixmap::grabWidget(SudokuMain, 0, 0, sudoku_size_widget,
		                          sudoku_size_widget);
		QRegion region(0, 0, sudoku_size_widget, sudoku_size_widget);
		QPoint offset(0, 0);
		SudokuMain->render(&pic, offset, region);
	}
	else {
		pic = QPixmap::grabWidget(SudokuMain);
		SudokuMain->render(&pic);
	}
	int imageID = bin;
	QString imageName =
	    folder + "/" + QString(QVariant(imageID).toString()) + ".png";

	std::string imageNametocstring = imageName.toUtf8().constData();
	std::ifstream ifile(imageNametocstring);
	if (!ifile) {
		pic.save(imageName);
	}
}

void MainWindow::sudokuspikingsolver()
{
	std::string simulator = simulatorBox->currentText().toStdString();
	std::string solver_str = solverBox->currentText().toStdString();
	if (first_time) {
		// Append path by VirtualEnvironment
		std::string new_path;
		if (simulator == "pyNN.nest") {
			new_path = home + "/venvs/nest/bin:" + old_path;
			// Disable Spikey and spinnaker
			simulatorBox->setItemData(1, false, Qt::UserRole - 1);
			simulatorBox->setItemData(2, false, Qt::UserRole - 1);
			simulatorBox->setItemData(3, false, Qt::UserRole - 1);
		}
		else if (simulator == "spikey") {
			new_path = old_path;
			// Disable spiNNaker and Nest
			simulatorBox->setItemData(0, false, Qt::UserRole - 1);
			simulatorBox->setItemData(1, false, Qt::UserRole - 1);
			simulatorBox->setItemData(2, false, Qt::UserRole - 1);
		}
		else {
			//new_path = home + "/venvs/spinnaker4/bin:" + old_path;
			simulatorBox->setItemData(0, false, Qt::UserRole - 1);
			simulatorBox->setItemData(3, false, Qt::UserRole - 1);
		}
		//setenv("PATH", new_path.c_str(), 1);
		first_time = false;
	}
	if (simulator == "pyNN.nest") {
		simulator = "nest={\"threads\": 4}";
	}
	else if (simulator == "pyNN.spiNNaker") {
		simulator =
		    "spinnaker={\"source_neurons_per_core\": 25, \"neurons_per_core\" "
		    ": 25}";
	}
	else if (simulator == "pyNN.spiNNaker0.1") {
		simulator =
		    "spinnaker={\"source_neurons_per_core\": 40, \"neurons_per_core\" "
		    ": 40, \"timestep\": 0.1}";
	}
	else if (simulator == "BrainScaleS") {
		simulator =
		    "slurm.nmpm1={\"wafer\": 33, \"hicann\": "
		    "[268,269,270,296,297,298], \"calib_path\" : "
		    "\"/wang/data/calibration/brainscales/default-2017-09-26-1\", "
		    "\"neuron_size\" : 4,\"digital_weight\" : true,\"bandwidth\" : "
		    "0.8}";
	}
	else if (simulator == "GeNN-CPU") {
		simulator = "genn";
	}
	else if (simulator == "GeNN-GPU") {
		simulator = "genn={\"gpu\":true}";
	}

	// Reset Sudoku solver --> Read parameters file
	try {
		if (solver_str == "Spiking") {
			solver = std::make_shared<spikingSudokuSolver>(
			    spikingSudokuSolver(parameter_file));
		}
		else if (solver_str == "Spiking, Single Pop") {
			solver = std::make_shared<SpikingSolverSinglePop>(
			    SpikingSolverSinglePop(parameter_file));
		}
		else if (solver_str == "Spiking, Mirror Inhib") {
			solver = std::make_shared<SSolveMirrorInhib>(
			    SSolveMirrorInhib(parameter_file));
		}
		else if (solver_str == "Spiking, Single Neuron") {
			solver = std::make_shared<SpikingSolverSingleNeuron>(
			    SpikingSolverSingleNeuron(parameter_file));
		}
		else {
			throw;
		}
	}
	catch (std::exception &e) {
		QMessageBox::warning(
		    this, QString("Internal error"),
		    QString("Solver not known or parameter file is corrupt!"));
		std::cerr << e.what() << std::endl;
		return;
	}

	try {
		solver->initialize(sudoku);
	}
	catch (std::exception &e) {
		butVideo->setEnabled(false);
		ScrubBar->setEnabled(false);
		QMessageBox::warning(
		    this, QString("Wrong Parameter File"),
		    QString("You chose a parameter file that either is faulty or does "
		            "not match to the solver! Please retry using a different "
		            "paramter file!"));
		std::cerr << e.what() << std::endl;
		return;
	}
	const char *instance = QFileInfo(QCoreApplication::applicationFilePath())
	                           .fileName()
	                           .toLatin1()
	                           .data();
	try {
		solver->run(instance, simulator.c_str(), cubeBox->isChecked());
	}
	catch (std::exception &e) {
		butVideo->setEnabled(false);
		ScrubBar->setEnabled(false);
		QMessageBox::warning(
		    this, QString("Simulation broke down"),
		    QString("The simulation broke down. Either you chose parameters "
		            "that are not valid for this simulator or the solver "
		            "cannot be executed on the given simulation backend"));
		std::cerr << e.what() << std::endl;
		exit(1);
	}
	activate = false;
	painterActiver = true;

	// result dims [row][col][bin]
	butVideo->setEnabled(true);
	result = solver->evaluate();
	ScrubBar->setEnabled(true);
	ScrubBar->setMinimum(0);
	ScrubBar->setMaximum(int(result[0][0].size()) - 1);
	ScrubBar->setValue(0);
	sudoku = solver->updateSudokuFromResult(sudoku, result, 0);
	if (sudoku.complete()) {
		solved->setStyleSheet("background-color:green;");
	}
	else {
		solved->setStyleSheet("background-color:red;");
	}

	int bin_size = solver->config()["bin_size"];
	QString bin_info = "0-";
	bin_info.append(QString::number(bin_size));
	bin_info.append(" ms");
	solved->setText(bin_info);
	solved->show();

	SudokuMain->ChangeInnerResults(sudoku, activate);
	SudokuMain->ChangePainterState(painterActiver);

	SudokuMain->update();
}
