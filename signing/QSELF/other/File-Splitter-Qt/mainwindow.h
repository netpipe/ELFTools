#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "splitterthread.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QString path();
    void setPath(QString path);
    void setOutPath(QString path);
    
private:
    Ui::MainWindow *ui;
    
    SplitterThread* splitterThread;
    
    bool splitStarted;

private:
    void addActions();
    void addShortcuts();
    void status(QString path);
    
    QString formatPath(QString path);
    void createDirs(QString path);

private slots:
    void bRunClick();
    void bBrowseInClick();
    void bBrowseOutClick();
    
    void splitThreadFinished();

};

#endif // MAINWINDOW_H
