/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *PicturePreviewLabel;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_4;
    QVBoxLayout *verticalLayout_3;
    QVBoxLayout *verticalLayout;
    QListView *albumListView;
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *horizontalLayout;
    QPushButton *addAlbumButton;
    QLineEdit *albumNameEdit;
    QPushButton *delAlbumButton;
    QPushButton *editAlbumButton;
    QPushButton *backToGalleryButton;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *nextPictureButton;
    QPushButton *prevPictureButton;
    QVBoxLayout *verticalLayout_2;
    QListView *pictureListView;
    QPushButton *delPictureButton;
    QPushButton *addPictureButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(870, 652);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        PicturePreviewLabel = new QLabel(centralwidget);
        PicturePreviewLabel->setObjectName("PicturePreviewLabel");
        PicturePreviewLabel->setGeometry(QRect(540, 210, 271, 201));
        PicturePreviewLabel->setScaledContents(true);
        layoutWidget = new QWidget(centralwidget);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(260, 30, 273, 314));
        verticalLayout_4 = new QVBoxLayout(layoutWidget);
        verticalLayout_4->setObjectName("verticalLayout_4");
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        albumListView = new QListView(layoutWidget);
        albumListView->setObjectName("albumListView");

        verticalLayout->addWidget(albumListView);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        addAlbumButton = new QPushButton(layoutWidget);
        addAlbumButton->setObjectName("addAlbumButton");

        horizontalLayout->addWidget(addAlbumButton);

        albumNameEdit = new QLineEdit(layoutWidget);
        albumNameEdit->setObjectName("albumNameEdit");

        horizontalLayout->addWidget(albumNameEdit);


        horizontalLayout_2->addLayout(horizontalLayout);

        delAlbumButton = new QPushButton(layoutWidget);
        delAlbumButton->setObjectName("delAlbumButton");

        horizontalLayout_2->addWidget(delAlbumButton);


        verticalLayout->addLayout(horizontalLayout_2);


        verticalLayout_3->addLayout(verticalLayout);

        editAlbumButton = new QPushButton(layoutWidget);
        editAlbumButton->setObjectName("editAlbumButton");

        verticalLayout_3->addWidget(editAlbumButton);


        verticalLayout_4->addLayout(verticalLayout_3);

        backToGalleryButton = new QPushButton(layoutWidget);
        backToGalleryButton->setObjectName("backToGalleryButton");

        verticalLayout_4->addWidget(backToGalleryButton);

        layoutWidget1 = new QWidget(centralwidget);
        layoutWidget1->setObjectName("layoutWidget1");
        layoutWidget1->setGeometry(QRect(180, 340, 344, 256));
        horizontalLayout_3 = new QHBoxLayout(layoutWidget1);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        nextPictureButton = new QPushButton(layoutWidget1);
        nextPictureButton->setObjectName("nextPictureButton");

        horizontalLayout_3->addWidget(nextPictureButton);

        prevPictureButton = new QPushButton(layoutWidget1);
        prevPictureButton->setObjectName("prevPictureButton");

        horizontalLayout_3->addWidget(prevPictureButton);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        pictureListView = new QListView(layoutWidget1);
        pictureListView->setObjectName("pictureListView");

        verticalLayout_2->addWidget(pictureListView);

        delPictureButton = new QPushButton(layoutWidget1);
        delPictureButton->setObjectName("delPictureButton");

        verticalLayout_2->addWidget(delPictureButton);

        addPictureButton = new QPushButton(layoutWidget1);
        addPictureButton->setObjectName("addPictureButton");

        verticalLayout_2->addWidget(addPictureButton);


        horizontalLayout_3->addLayout(verticalLayout_2);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 870, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        PicturePreviewLabel->setText(QString());
        addAlbumButton->setText(QString());
        albumNameEdit->setText(QCoreApplication::translate("MainWindow", "Nuevo  album", nullptr));
        delAlbumButton->setText(QString());
        editAlbumButton->setText(QString());
        backToGalleryButton->setText(QString());
        nextPictureButton->setText(QString());
        prevPictureButton->setText(QString());
        delPictureButton->setText(QString());
        addPictureButton->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
