#include "mainwindow.h"

mainwindow::mainwindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	myopenglwidget = new OpenGLWidget;

	this->setCentralWidget(myopenglwidget);
}
