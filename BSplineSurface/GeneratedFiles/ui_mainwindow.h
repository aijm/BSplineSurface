/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "OpenGLWidget.h"

QT_BEGIN_NAMESPACE

class Ui_mainwindowClass
{
public:
    QAction *actionFace;
    QAction *actionEdge;
    QAction *actionControl_mesh;
    QWidget *centralWidget;
    OpenGLWidget *openGLWidget;
    QMenuBar *menuBar;
    QMenu *menuRender;
    QStatusBar *statusBar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *mainwindowClass)
    {
        if (mainwindowClass->objectName().isEmpty())
            mainwindowClass->setObjectName(QStringLiteral("mainwindowClass"));
        mainwindowClass->resize(600, 408);
        actionFace = new QAction(mainwindowClass);
        actionFace->setObjectName(QStringLiteral("actionFace"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/facemode.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionFace->setIcon(icon);
        actionEdge = new QAction(mainwindowClass);
        actionEdge->setObjectName(QStringLiteral("actionEdge"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/images/linemode.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionEdge->setIcon(icon1);
        actionControl_mesh = new QAction(mainwindowClass);
        actionControl_mesh->setObjectName(QStringLiteral("actionControl_mesh"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/images/controlmesh.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionControl_mesh->setIcon(icon2);
        centralWidget = new QWidget(mainwindowClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        openGLWidget = new OpenGLWidget(centralWidget);
        openGLWidget->setObjectName(QStringLiteral("openGLWidget"));
        openGLWidget->setGeometry(QRect(-1, -1, 601, 341));
        mainwindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(mainwindowClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 23));
        menuRender = new QMenu(menuBar);
        menuRender->setObjectName(QStringLiteral("menuRender"));
        mainwindowClass->setMenuBar(menuBar);
        statusBar = new QStatusBar(mainwindowClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        mainwindowClass->setStatusBar(statusBar);
        toolBar = new QToolBar(mainwindowClass);
        toolBar->setObjectName(QStringLiteral("toolBar"));
        mainwindowClass->addToolBar(Qt::TopToolBarArea, toolBar);

        menuBar->addAction(menuRender->menuAction());
        menuRender->addAction(actionFace);
        menuRender->addAction(actionEdge);
        menuRender->addAction(actionControl_mesh);
        toolBar->addAction(actionControl_mesh);
        toolBar->addAction(actionFace);
        toolBar->addAction(actionEdge);
        toolBar->addSeparator();

        retranslateUi(mainwindowClass);
        QObject::connect(actionFace, SIGNAL(triggered()), openGLWidget, SLOT(on_showface()));
        QObject::connect(actionEdge, SIGNAL(triggered()), openGLWidget, SLOT(on_showedge()));
        QObject::connect(actionControl_mesh, SIGNAL(triggered()), openGLWidget, SLOT(on_showcontrol()));

        QMetaObject::connectSlotsByName(mainwindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *mainwindowClass)
    {
        mainwindowClass->setWindowTitle(QApplication::translate("mainwindowClass", "mainwindow", 0));
        actionFace->setText(QApplication::translate("mainwindowClass", "face", 0));
#ifndef QT_NO_TOOLTIP
        actionFace->setToolTip(QApplication::translate("mainwindowClass", "face mode", 0));
#endif // QT_NO_TOOLTIP
        actionEdge->setText(QApplication::translate("mainwindowClass", "edge", 0));
#ifndef QT_NO_TOOLTIP
        actionEdge->setToolTip(QApplication::translate("mainwindowClass", "edge mode", 0));
#endif // QT_NO_TOOLTIP
        actionControl_mesh->setText(QApplication::translate("mainwindowClass", "control mesh", 0));
        menuRender->setTitle(QApplication::translate("mainwindowClass", "Render", 0));
        toolBar->setWindowTitle(QApplication::translate("mainwindowClass", "toolBar", 0));
    } // retranslateUi

};

namespace Ui {
    class mainwindowClass: public Ui_mainwindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
