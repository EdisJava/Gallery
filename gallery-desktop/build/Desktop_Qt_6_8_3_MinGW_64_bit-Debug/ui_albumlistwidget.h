/********************************************************************************
** Form generated from reading UI file 'albumlistwidget.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ALBUMLISTWIDGET_H
#define UI_ALBUMLISTWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "albumlistwidget.h"

QT_BEGIN_NAMESPACE

class Ui_AlbumListWidget
{
public:
    QVBoxLayout *verticalLayout;
    AlbumListWidget *listWidget;

    void setupUi(QWidget *AlbumListWidget)
    {
        if (AlbumListWidget->objectName().isEmpty())
            AlbumListWidget->setObjectName("AlbumListWidget");
        AlbumListWidget->resize(400, 300);
        verticalLayout = new QVBoxLayout(AlbumListWidget);
        verticalLayout->setObjectName("verticalLayout");
        listWidget = new class AlbumListWidget(AlbumListWidget);
        listWidget->setObjectName("listWidget");

        verticalLayout->addWidget(listWidget);


        retranslateUi(AlbumListWidget);

        QMetaObject::connectSlotsByName(AlbumListWidget);
    } // setupUi

    void retranslateUi(QWidget *AlbumListWidget)
    {
        AlbumListWidget->setWindowTitle(QCoreApplication::translate("AlbumListWidget", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AlbumListWidget: public Ui_AlbumListWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ALBUMLISTWIDGET_H
