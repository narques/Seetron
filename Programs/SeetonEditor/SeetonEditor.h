#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SeetonEditor.h"

class SeetonEditor : public QMainWindow
{
    Q_OBJECT

public:
    SeetonEditor(QWidget *parent = Q_NULLPTR);

private:
    Ui::SeetonEditorClass ui;
};
