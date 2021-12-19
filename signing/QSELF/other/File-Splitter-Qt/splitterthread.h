#ifndef SPLITTERTHREAD_H
#define SPLITTERTHREAD_H

#include <QThread>
#include <QDebug>

#include <string>
#include <fstream>

#include <iostream>
#include <vector>

using namespace std;


class SplitterThread : public QThread
{
  Q_OBJECT
  
  static const int ENC_UTF8 = 0;
  static const int ENC_UTF16_LE = 1;
  static const int ENC_UTF16_BE = 2;
  
public:
  explicit SplitterThread(QObject *parent = 0);
  void run();
  void copy_Old();
  
  
  void setInputFile(string inputFile) {
    this->inputFile = inputFile;
  }
  
  void setOutputFile(string outputFile) {
    this->outputFile = outputFile;
  }
  
  void setLinesCount(int linesCount) {
    this->linesCount = linesCount;
  }
  
  void stop();


private:
  
  string inputFile;
  string outputFile;
  int linesCount;
  
  bool stopThread;

signals:

public slots:

};

#endif // SPLITTERTHREAD_H
