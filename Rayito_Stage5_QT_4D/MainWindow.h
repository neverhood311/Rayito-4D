#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <vector>
#include <string>
#include "rayito.h"
#include "lodepng.h"
#include <QMainWindow>


namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
   Q_OBJECT
   
public:
   explicit MainWindow(QWidget *pParent = NULL);
   ~MainWindow();
   
private slots:
   void on_actionQuit_triggered();
   
   void on_renderButton_clicked();
   
   void on_actionRender_Scene_triggered();

   void on_actionSave_as_triggered();
   
private:
   Ui::MainWindow *ui;

   Rayito::Image *pImage;   //pointer to the image
};


#endif // MAINWINDOW_H
