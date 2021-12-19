#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QShortcut>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    addActions();
    addShortcuts();

    setWindowTitle("File Splitter v1.0.1");
    splitStarted = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::addActions() {
  connect( ui->bRun, SIGNAL(clicked()), this, SLOT(bRunClick()) );
  connect( ui->bBrowseIn, SIGNAL(clicked()), this, SLOT(bBrowseInClick()) );
  connect( ui->bBrowseOut, SIGNAL(clicked()), this, SLOT(bBrowseOutClick()) );
}

void MainWindow::addShortcuts() {
  QShortcut *bRun = new QShortcut(QKeySequence("F5"), this);
  connect( bRun, SIGNAL(activated()), ui->bRun, SLOT(click()) );

  QShortcut *bBrowseIn = new QShortcut(QKeySequence("F3"), this);
  connect( bBrowseIn, SIGNAL(activated()), ui->bBrowseIn, SLOT(click()) );

  QShortcut *bBrowseOut = new QShortcut(QKeySequence("F2"), this);
  connect( bBrowseOut, SIGNAL(activated()), ui->bBrowseOut, SLOT(click()) );

  QShortcut *quit = new QShortcut(QKeySequence("Esc"), this);
  connect( quit, SIGNAL(activated()), this, SLOT(close()) );
}


// ---------------------------------------------- button handlers ----------------------------------------------

void MainWindow::bRunClick() {
  if (splitStarted && splitterThread) {
    splitterThread->stop();
    return;
  }
  
  status("");
  
  int linesCount = ui->leLinesCount->text().toInt();
  QString inputFile = ui->lePath->text();
  
  QString outDir = ui->leOutDir->text();
  if (outDir.trimmed().length() == 0) {
    QString inDir = ui->lePath->text();
    QFileInfo info(inDir);
    outDir = info.absolutePath();
  }
  
  outDir = formatPath(outDir);
  outDir.replace(QRegExp("/$"), "");
  outDir += "/";
  createDirs(outDir);
  
  QString outFilename = ui->leOutFilename->text();
  if (outFilename.trimmed().length() == 0) {
    QFileInfo info(inputFile);
    outFilename = info.fileName();
  }
  string outputFile = outDir.toStdString() + outFilename.toStdString();
  
  splitterThread = new SplitterThread();
  splitterThread->setInputFile(inputFile.toStdString());
  splitterThread->setOutputFile(outputFile);
  splitterThread->setLinesCount(linesCount);
  
  connect( splitterThread, SIGNAL(finished()), this, SLOT(splitThreadFinished()) );
  
  splitterThread->start();
  
  splitStarted = true;
  ui->bRun->setText("Stop");
}


void MainWindow::splitThreadFinished() {
  splitStarted = false;
  ui->bRun->setText("Run");
  qDebug() << "Finish";
  status("Splitting Finished");
}


void MainWindow::bBrowseInClick() {
  QString dir = path();
  if (dir.length() == 0) dir = QDir::currentPath();

  QString filename = QFileDialog::getOpenFileName(this, tr("Select a text file"), dir);
  if ( !filename.isNull() )
  {
    setPath(filename);
  }
}

void MainWindow::bBrowseOutClick() {
  QString dir = path();
  if (dir.length() == 0) dir = QDir::currentPath();

  QString dirname = QFileDialog::getExistingDirectory(this, tr("Select a Directory"), dir);
  if ( !dirname.isNull() )
  {
    setOutPath(dirname);
  }
}


// --------------------------------------------- set/get ---------------------------------------------

QString MainWindow::path() {
  return ui->lePath->text();
}

void MainWindow::setPath(QString path) {
  ui->lePath->setText(path);
}

void MainWindow::setOutPath(QString path) {
  ui->leOutDir->setText(path);
}


// --------------------------------------------- service ---------------------------------------------

void MainWindow::status(QString msg) {
  if (msg == "") {
    ui->statusBar->clearMessage();
    return;
  }
  ui->statusBar->showMessage("[" + msg + "]");
}

QString MainWindow::formatPath(QString path) {
  QString res = "";
  QFileInfo info(path);
  res = info.absoluteFilePath();
  res.replace(QRegExp("/$"), "");
  return res;
}

void MainWindow::createDirs(QString path) {
  QDir d;
  d.mkpath(path);
}
