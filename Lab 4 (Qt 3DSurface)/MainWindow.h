#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "plot.h"

#include <QApplication>
#include <QCheckBox>
#include <QDebug>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QMainWindow>
#include <QMap>
#include <QMenu>
#include <QMenuBar>
#include <QRadioButton>
#include <QRegExpValidator>
#include <QSettings>
#include <QSlider>
#include <QStatusBar>
#include <QStyle>
#include <QToolBar>
#include <QTranslator>
#include <QtGui/QPainter>
#include <QtGui/QScreen>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void setColumnIntervalBySlider1(int value);
    void setColumnIntervalBySlider2(int value);
    void setRawIntervalBySlider1(int value);
    void setRawIntervalBySlider2(int value);
    void isLabelToggled();

private slots:
    void saveSettings();
    void loadSettings();

private:
    QtDataVisualization::Q3DSurface *m_graphMain;
    Plot *m_modifier;

    QSettings *m_sett;

    QCheckBox *m_checkboxShowGrid;
    QCheckBox *m_checkboxShowLabelBorder;
    QCheckBox *m_checkboxShowLabel;

    QRadioButton *m_sinc1;
    QRadioButton *m_sinc2;

    QRadioButton *m_modeNoneRB;
    QRadioButton *m_modeItemRB;

    QLineEdit *m_rangeX;
    QLineEdit *m_rangeZ;
    QLineEdit *m_columnInterval1;
    QLineEdit *m_columnInterval2;
    QLineEdit *m_rawInterval1;
    QLineEdit *m_rawInterval2;
    QLabel *m_rangeXLabel;
    QLabel *m_rangeZLabel;

    QSlider *m_axisMinSliderX;
    QSlider *m_axisMaxSliderX;
    QSlider *m_axisMinSliderZ;
    QSlider *m_axisMaxSliderZ;
};

#endif // MAINWINDOW_H
