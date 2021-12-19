#include "splitterthread.h"

SplitterThread::SplitterThread(QObject *parent) :
  QThread(parent)
{
}

void SplitterThread::run() {
  fstream textFile(inputFile.c_str(), ios::binary | ios::in);
  if (!textFile.is_open()) {
    qFatal("cannot open file");
    return;
  }
  
  int enc = ENC_UTF8;
  
  char buf[2];
  textFile.read(buf, 2);
  unsigned char b0 = (unsigned char) buf[0];
  unsigned char b1 = (unsigned char) buf[1];
  
  if (b0 == 0xFF && b1 == 0xFE || b0 != 0x00 && b1 == 0x00) {
    enc = ENC_UTF16_LE;
  }
  else if (b0 == 0xFE && b1 == 0xFF || b0 == 0x00 && b1 != 0x00) {
    enc = ENC_UTF16_BE;
  }
  textFile.seekg(0);
  
  stopThread = false;
  bool finish = false;
  int count = 1;
  int lines = 0;
  
  char c0;

  while(textFile) {
    if (stopThread) break;
    
    lines = 0;
    
    string outFileC = outputFile + "_" + to_string(count++) + ".txt";
    fstream outFile(outFileC.c_str(), ios::binary | ios::out);
    if (!outFile.is_open()) {
      qFatal("cannot open file");
      return;
    }
    
    while (lines < linesCount) {
      textFile.read(&c0, 1);
      outFile.write(&c0, 1);
      
      if (!textFile) {
        if (lines == 0) {
          outFile.close();
          remove(outFileC.c_str());
        }
        finish = true;
        break;
      }
      
      if (c0 == 0x0a) {
        if (enc == ENC_UTF16_LE) {
          textFile.read(&c0, 1);
          outFile.write(&c0, 1);
        }
        
        lines++;
      }
    }
    
    if (finish) break;
    outFile.close();
  }
  
  textFile.close();
}


void SplitterThread::copy_Old() {
  fstream textFileBin(inputFile.c_str(), ios::binary | ios::in);
  if (!textFileBin.is_open()) {
    qFatal("cannot open file");
    return;
  }
  
  int enc = ENC_UTF8;
  // string sep = {0x0a};
  string sep;
  
  char buf[2];
  textFileBin.read(buf, 2);
  unsigned char b0 = (unsigned char) buf[0];
  unsigned char b1 = (unsigned char) buf[1];
  
  if (b0 == 0xFF && b1 == 0xFE || b0 != 0x00 && b1 == 0x00) {
    enc = ENC_UTF16_LE;
    // sep = {0x0a, 0x00};
    sep = {0x0d};
  }
  else if (b0 == 0xFE && b1 == 0xFF || b0 == 0x00 && b1 != 0x00) {
    enc = ENC_UTF16_BE;
    sep = {0x00, 0x0a};
  }
  
  textFileBin.close();
  // return;
  
  fstream textFile(inputFile.c_str(), ios::in);
  
  stopThread = false;
  bool finish = false;
  string str;
  int count = 1;
  int filesLimit = 10;

  while(textFile) {
    if (stopThread) break;

    string outFileC = outputFile + "_" + to_string(count++) + ".txt";
    fstream outFile(outFileC.c_str(), ios::out);
    if (!outFile.is_open()) {
      qFatal("cannot open file");
      return;
    }

    for (int i=0; i<linesCount; ++i) {
      getline(textFile, str);
      
      if (!textFile) {
        if (i == 0) {
          outFile.close();
          remove(outFileC.c_str());
        }
        
        finish = true;
        break;
      }
      
      // if (i != linesCount-1) str += '\n';
      if (i != linesCount-1) str += sep;
      outFile << str;
    }
    
    if (finish) break;
    outFile.close();
  }

  textFile.close();
}


void SplitterThread::stop() {
  stopThread = true;
}
