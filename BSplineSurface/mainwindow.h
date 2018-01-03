#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"
#include <OpenGLWidget.h>
class mainwindow : public QMainWindow
{
	Q_OBJECT

public:
	mainwindow(QWidget *parent = Q_NULLPTR);

protected:
	
private:
	Ui::mainwindowClass ui;

};
