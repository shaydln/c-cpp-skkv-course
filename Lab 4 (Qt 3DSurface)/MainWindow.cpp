#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QStatusBar *statusBar = new QStatusBar;
    setStatusBar(statusBar);

    QMenuBar *menuBar = new QMenuBar(this);

    QMenu *project_menu = new QMenu(QStringLiteral("&Main menu"));
    QAction *actionSave1 = project_menu->addAction(style()->standardIcon(
                                                       QStyle::SP_DialogSaveButton),
                                                   "Save settings",
                                                   this,
                                                   SLOT(saveSettings()),
                                                   Qt::CTRL + Qt::Key_S);

    QAction *actionLoad1 = project_menu->addAction(style()->standardIcon(
                                                       QStyle::SP_DialogResetButton),
                                                   "Reload settigns",
                                                   this,
                                                   SLOT(loadSettings()),
                                                   Qt::CTRL + Qt::Key_L);

    QObject::connect(actionSave1, &QAction::triggered, this, &MainWindow::saveSettings);
    QObject::connect(actionLoad1, &QAction::triggered, this, &MainWindow::loadSettings);

    menuBar->addMenu(project_menu);

    setMenuBar(menuBar);

    QToolBar *toolbar = addToolBar("main toolbar");

    QAction *actionSave = toolbar->addAction(style()->standardIcon(QStyle::SP_DialogSaveButton),
                                             QStringLiteral("Save settings"));
    actionSave->setStatusTip("Save settings");

    QAction *actionLoad = toolbar->addAction(style()->standardIcon(QStyle::SP_DialogResetButton),
                                             QStringLiteral("Load settings"));
    actionLoad->setStatusTip("Load settings");

    QAction *actionQuit = toolbar->addAction(style()->standardIcon(QStyle::SP_TitleBarCloseButton),
                                             QStringLiteral("Exit application"));
    actionQuit->setStatusTip("Quit");

    QObject::connect(actionSave, &QAction::triggered, this, &MainWindow::saveSettings);
    QObject::connect(actionLoad, &QAction::triggered, this, &MainWindow::loadSettings);
    QObject::connect(actionQuit, &QAction::triggered, qApp, &QApplication::quit);

    m_graphMain = new QtDataVisualization::Q3DSurface();

    QWidget *container = QWidget::createWindowContainer(m_graphMain);
    QSize screenSize = m_graphMain->screen()->size();
    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 1.6));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    QWidget *widget = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    QVBoxLayout *vLayout = new QVBoxLayout();
    hLayout->addWidget(container, 1);
    hLayout->addLayout(vLayout);
    vLayout->setAlignment(Qt::AlignTop);

    m_checkboxShowGrid = new QCheckBox(widget);
    m_checkboxShowGrid->setText(QStringLiteral("Show grid"));
    vLayout->addWidget(m_checkboxShowGrid);

    m_checkboxShowLabelBorder = new QCheckBox(widget);
    m_checkboxShowLabelBorder->setText(QStringLiteral("Show label border"));
    vLayout->addWidget(m_checkboxShowLabelBorder);

    m_checkboxShowLabel = new QCheckBox(widget);
    m_checkboxShowLabel->setText(QStringLiteral("Show label"));
    vLayout->addWidget(m_checkboxShowLabel);

    QGroupBox *modelGroupBox = new QGroupBox(tr("Plot"));

    m_sinc1 = new QRadioButton(widget);
    m_sinc1->setText(QStringLiteral("sinc(distance_from_zero)"));
    m_sinc1->setChecked(false);

    m_sinc2 = new QRadioButton(widget);
    m_sinc2->setText(QStringLiteral("sinc(x)*sinc(z)"));
    m_sinc2->setChecked(false);

    QVBoxLayout *modelVBox = new QVBoxLayout;
    modelVBox->addWidget(m_sinc1);
    modelVBox->addWidget(m_sinc2);

    modelGroupBox->setLayout(modelVBox);
    vLayout->addWidget(modelGroupBox);

    QGroupBox *selectionGroupBox = new QGroupBox(QStringLiteral("Selection Mode"));

    m_modeNoneRB = new QRadioButton(widget);
    m_modeNoneRB->setText(QStringLiteral("No selection"));
    m_modeNoneRB->setChecked(false);

    m_modeItemRB = new QRadioButton(widget);
    m_modeItemRB->setText(QStringLiteral("Item"));
    m_modeItemRB->setChecked(false);

    QVBoxLayout *selectionVBox = new QVBoxLayout;
    selectionVBox->addWidget(m_modeNoneRB);
    selectionVBox->addWidget(m_modeItemRB);
    selectionGroupBox->setLayout(selectionVBox);
    vLayout->addWidget(selectionGroupBox);

    m_modifier = new Plot(m_graphMain, this);

    QVBoxLayout *rangeVBoxLayout = new QVBoxLayout();

    QHBoxLayout *hLayoutRangeX = new QHBoxLayout();
    m_rangeX = new QLineEdit(widget);
    m_rangeX->setText(QString::number(m_modifier->sampleCountX));
    m_rangeX->setValidator(new QIntValidator(5, 100000, this));
    m_rangeX->setAlignment(Qt::AlignCenter);
    m_rangeXLabel = new QLabel(widget);
    m_rangeXLabel->setText("Number of steps X:   ");
    hLayoutRangeX->addWidget(m_rangeXLabel);
    hLayoutRangeX->addWidget(m_rangeX);

    QHBoxLayout *hLayoutRangeZ = new QHBoxLayout();
    m_rangeZ = new QLineEdit(widget);
    m_rangeZ->setText(QString::number(m_modifier->sampleCountZ));
    m_rangeZ->setAlignment(Qt::AlignCenter);
    m_rangeZ->setValidator(new QIntValidator(5, 100000, this));
    m_rangeZLabel = new QLabel(widget);
    m_rangeZLabel->setText("Number of steps Z:   ");
    hLayoutRangeZ->addWidget(m_rangeZLabel);
    hLayoutRangeZ->addWidget(m_rangeZ);

    rangeVBoxLayout->addItem(hLayoutRangeX);
    rangeVBoxLayout->addItem(hLayoutRangeZ);

    vLayout->addItem(rangeVBoxLayout);

    m_axisMinSliderX = new QSlider(Qt::Horizontal, widget);
    m_axisMinSliderX->setMinimum(0);
    m_axisMinSliderX->setTickInterval(1);
    m_axisMinSliderX->setEnabled(true);
    m_axisMaxSliderX = new QSlider(Qt::Horizontal, widget);
    m_axisMaxSliderX->setMinimum(1);
    m_axisMaxSliderX->setTickInterval(1);
    m_axisMaxSliderX->setEnabled(true);
    m_axisMinSliderZ = new QSlider(Qt::Horizontal, widget);
    m_axisMinSliderZ->setMinimum(0);
    m_axisMinSliderZ->setTickInterval(1);
    m_axisMinSliderZ->setEnabled(true);
    m_axisMaxSliderZ = new QSlider(Qt::Horizontal, widget);
    m_axisMaxSliderZ->setMinimum(1);
    m_axisMaxSliderZ->setTickInterval(1);
    m_axisMaxSliderZ->setEnabled(true);

    m_modifier->setAxisMinSliderX(m_axisMinSliderX);
    m_modifier->setAxisMaxSliderX(m_axisMaxSliderX);
    m_modifier->setAxisMinSliderZ(m_axisMinSliderZ);
    m_modifier->setAxisMaxSliderZ(m_axisMaxSliderZ);
    m_modifier->initSliders();

    QHBoxLayout *hLayoutColumnIntervals = new QHBoxLayout();

    m_columnInterval1 = new QLineEdit(widget);
    m_columnInterval1->setText(QString::number(m_axisMinSliderX->value()));

    m_columnInterval2 = new QLineEdit(widget);
    m_columnInterval2->setText(QString::number(m_axisMaxSliderX->value()));

    hLayoutColumnIntervals->addWidget(m_columnInterval1);
    hLayoutColumnIntervals->addWidget(m_columnInterval2);

    QHBoxLayout *hLayoutRawIntervals = new QHBoxLayout();

    m_rawInterval1 = new QLineEdit(widget);
    m_rawInterval1->setValidator(new QIntValidator(5, 100000, this));
    m_rawInterval1->setText(QString::number(m_axisMinSliderZ->value()));

    m_rawInterval2 = new QLineEdit(widget);
    m_rawInterval2->setValidator(new QIntValidator(5, 100000, this));
    m_rawInterval2->setText(QString::number(m_axisMaxSliderZ->value()));

    hLayoutRawIntervals->addWidget(m_rawInterval1);
    hLayoutRawIntervals->addWidget(m_rawInterval2);

    vLayout->addWidget(new QLabel(QStringLiteral("Column range")));

    vLayout->addItem(hLayoutColumnIntervals);
    vLayout->addWidget(m_axisMinSliderX);
    vLayout->addWidget(m_axisMaxSliderX);
    vLayout->addWidget(new QLabel(QStringLiteral("Row range")));

    vLayout->addItem(hLayoutRawIntervals);
    vLayout->addWidget(m_axisMinSliderZ);
    vLayout->addWidget(m_axisMaxSliderZ);

    QGroupBox *colorGroupBox = new QGroupBox(QStringLiteral("Custom gradient"));

    QLinearGradient grBtoY(0, 0, 1, 100);
    grBtoY.setColorAt(0.0, QColor(0, 0, 0));
    grBtoY.setColorAt(0.2, QColor(120, 120, 0));
    grBtoY.setColorAt(0.4, QColor(255, 255, 0));
    grBtoY.setColorAt(0.6, QColor(163, 163, 65));
    grBtoY.setColorAt(0.8, QColor(156, 156, 112));
    grBtoY.setColorAt(1.0, QColor(176, 176, 176));
    QPixmap pm(24, 100);
    QPainter pmp(&pm);
    pmp.setBrush(QBrush(grBtoY));
    pmp.setPen(Qt::NoPen);
    pmp.drawRect(0, 0, 24, 100);
    QPushButton *gradientBtoYPB = new QPushButton(widget);
    gradientBtoYPB->setIcon(QIcon(pm));
    gradientBtoYPB->setIconSize(QSize(24, 100));

    QLinearGradient grGtoR(0, 0, 1, 100);
    grGtoR.setColorAt(1.0, Qt::darkGreen);
    grGtoR.setColorAt(0.5, Qt::yellow);
    grGtoR.setColorAt(0.2, Qt::red);
    grGtoR.setColorAt(0.0, Qt::darkRed);
    pmp.setBrush(QBrush(grGtoR));
    pmp.drawRect(0, 0, 24, 100);
    QPushButton *gradientGtoRPB = new QPushButton(widget);
    gradientGtoRPB->setIcon(QIcon(pm));
    gradientGtoRPB->setIconSize(QSize(24, 100));

    QHBoxLayout *colorHBox = new QHBoxLayout;
    colorHBox->addWidget(gradientBtoYPB);
    colorHBox->addWidget(gradientGtoRPB);
    colorGroupBox->setLayout(colorHBox);

    vLayout->addWidget(colorGroupBox);

    QObject::connect(m_checkboxShowGrid, &QRadioButton::toggled, m_modifier, &Plot::enableGrid);
    QObject::connect(m_checkboxShowLabelBorder,
                     &QRadioButton::toggled,
                     m_modifier,
                     &Plot::enableLabelBorder);
    QObject::connect(m_checkboxShowLabel, &QRadioButton::toggled, m_modifier, &Plot::enableBorder);
    QObject::connect(m_checkboxShowLabel, &QRadioButton::toggled, this, &MainWindow::isLabelToggled);

    QObject::connect(m_sinc1, &QRadioButton::toggled, m_modifier, &Plot::enableSinc_1);
    QObject::connect(m_sinc2, &QRadioButton::toggled, m_modifier, &Plot::enableSinc_2);

    QObject::connect(m_modeNoneRB, &QRadioButton::toggled, m_modifier, &Plot::toggleModeNone);
    QObject::connect(m_modeItemRB, &QRadioButton::toggled, m_modifier, &Plot::toggleModeItem);

    QObject::connect(m_axisMinSliderX, &QSlider::valueChanged, m_modifier, &Plot::adjustXMin);
    QObject::connect(m_axisMaxSliderX, &QSlider::valueChanged, m_modifier, &Plot::adjustXMax);
    QObject::connect(m_axisMinSliderZ, &QSlider::valueChanged, m_modifier, &Plot::adjustZMin);
    QObject::connect(m_axisMaxSliderZ, &QSlider::valueChanged, m_modifier, &Plot::adjustZMax);

    QObject::connect(gradientBtoYPB,
                     &QPushButton::pressed,
                     m_modifier,
                     &Plot::setBlackToYellowGradient);
    QObject::connect(gradientGtoRPB,
                     &QPushButton::pressed,
                     m_modifier,
                     &Plot::setGreenToRedGradient);

    QObject::connect(m_rangeX, &QLineEdit::textChanged, m_modifier, &Plot::setCountX);
    QObject::connect(m_rangeZ, &QLineEdit::textChanged, m_modifier, &Plot::setCountZ);
    /*
    QObject::connect(m_columnInterval1, &QLineEdit::textChanged,
                     m_modifier, &Plot::setColumnInterval1);
    QObject::connect(m_columnInterval2, &QLineEdit::textChanged,
                     m_modifier, &Plot::setColumnInterval2);
    QObject::connect(m_rawInterval1, &QLineEdit::textChanged,
                     m_modifier, &Plot::setRawInterval1);
    QObject::connect(m_rawInterval2, &QLineEdit::textChanged,
                     m_modifier, &Plot::setRawInterval2);
    */
    QObject::connect(m_axisMinSliderX,
                     &QSlider::valueChanged,
                     this,
                     &MainWindow::setColumnIntervalBySlider1);
    QObject::connect(m_axisMaxSliderX,
                     &QSlider::valueChanged,
                     this,
                     &MainWindow::setColumnIntervalBySlider2);
    QObject::connect(m_axisMinSliderZ,
                     &QSlider::valueChanged,
                     this,
                     &MainWindow::setRawIntervalBySlider1);
    QObject::connect(m_axisMaxSliderZ,
                     &QSlider::valueChanged,
                     this,
                     &MainWindow::setRawIntervalBySlider2);

    m_modifier->enableGrid(false);
    m_modifier->enableLabelBorder(false);
    m_modifier->enableBorder(false);

    m_sett = new QSettings("./sett.ini", QSettings::IniFormat, this);
    loadSettings();
    isLabelToggled();
    setCentralWidget(widget);
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::loadSettings()
{
    if (m_sett->value("m_checkboxShowGrid").toBool())
        m_checkboxShowGrid->setChecked(true);
    if (m_sett->value("m_checkboxShowLabelBorder").toBool())
        m_checkboxShowLabelBorder->setChecked(true);
    if (m_sett->value("m_checkboxShowLabel").toBool())
        m_checkboxShowLabel->setChecked(true);

    if (m_sett->value("m_sinc1").toBool())
        m_sinc1->setChecked(true);
    if (m_sett->value("m_sinc2").toBool())
        m_sinc2->setChecked(true);
    if (!m_sett->value("m_sinc1").toBool() && !m_sett->value("m_sinc2").toBool())
        m_sinc1->setChecked(true);

    if (m_sett->value("m_modeNoneRB").toBool())
        m_modeNoneRB->setChecked(true);
    if (m_sett->value("m_modeItemRB").toBool())
        m_modeItemRB->setChecked(true);
    if (!m_sett->value("m_modeNoneRB").toBool() && !m_sett->value("m_modeItemRB").toBool())
        m_modeItemRB->setChecked(true);

    if (m_sett->value("m_axisMinSliderX").toInt()) {
        m_axisMinSliderX->setValue(m_sett->value("m_axisMinSliderX").toInt());
        m_columnInterval1->setText(QString::number(m_axisMinSliderX->value()));
    }
    if (m_sett->value("m_axisMaxSliderX").toInt()) {
        m_axisMaxSliderX->setValue(m_sett->value("m_axisMaxSliderX").toInt());
        m_columnInterval2->setText(QString::number(m_axisMaxSliderX->value()));
    }
    if (m_sett->value("m_axisMinSliderZ").toInt()) {
        m_axisMinSliderZ->setValue(m_sett->value("m_axisMinSliderZ").toInt());
        m_rawInterval1->setText(QString::number(m_axisMinSliderZ->value()));
    }
    if (m_sett->value("m_axisMaxSliderZ").toInt()) {
        m_axisMaxSliderZ->setValue(m_sett->value("m_axisMaxSliderZ").toInt());
        m_rawInterval2->setText(QString::number(m_axisMaxSliderZ->value()));
    }

    if (m_sett->value("gradient1").toString() == "yellow")
        m_modifier->m_curGradient1 = "yellow";
    if (m_sett->value("gradient1").toString() == "red")
        m_modifier->m_curGradient1 = "red";
    if (m_sett->value("gradient2").toString() == "yellow")
        m_modifier->m_curGradient2 = "yellow";
    if (m_sett->value("gradient2").toString() == "red")
        m_modifier->m_curGradient2 = "red";

    if (m_sett->value("x-rotation").isValid())
        m_graphMain->scene()->activeCamera()->setXRotation(m_sett->value("x-rotation").toFloat());
    if (m_sett->value("y-rotation").isValid())
        m_graphMain->scene()->activeCamera()->setYRotation(m_sett->value("y-rotation").toFloat());
    if (m_sett->value("zoom").isValid())
        m_graphMain->scene()->activeCamera()->setZoomLevel(m_sett->value("zoom").toFloat());

    if (m_sett->value("m_sampleCountX").toInt())
        m_rangeX->setText(QString::number(m_sett->value("m_sampleCountX").toInt()));
    if (m_sett->value("m_sampleCountZ").toInt())
        m_rangeZ->setText(QString::number(m_sett->value("m_sampleCountZ").toInt()));
}

void MainWindow::saveSettings()
{
    m_sett->setValue("m_checkboxShowGrid", m_checkboxShowGrid->isChecked());
    m_sett->setValue("m_checkboxShowLabelBorder", m_checkboxShowLabelBorder->isChecked());
    m_sett->setValue("m_checkboxShowLabel", m_checkboxShowLabel->isChecked());

    m_sett->setValue("m_sinc1", m_sinc1->isChecked());
    m_sett->setValue("m_sinc2", m_sinc2->isChecked());

    m_sett->setValue("m_modeNoneRB", m_modeNoneRB->isChecked());
    m_sett->setValue("m_modeItemRB", m_modeItemRB->isChecked());

    m_sett->setValue("m_axisMinSliderX", m_axisMinSliderX->value());
    m_sett->setValue("m_axisMaxSliderX", m_axisMaxSliderX->value());
    m_sett->setValue("m_axisMinSliderZ", m_axisMinSliderZ->value());
    m_sett->setValue("m_axisMaxSliderZ", m_axisMaxSliderZ->value());

    m_sett->setValue("m_sampleCountX", m_rangeX->text().toInt());
    m_sett->setValue("m_sampleCountZ", m_rangeZ->text().toInt());

    m_sett->setValue("gradient1", m_modifier->getCurrentGradient1());
    m_sett->setValue("gradient2", m_modifier->getCurrentGradient2());

    m_sett->setValue("x-rotation", m_graphMain->scene()->activeCamera()->xRotation());
    m_sett->setValue("y-rotation", m_graphMain->scene()->activeCamera()->yRotation());
    m_sett->setValue("zoom", m_graphMain->scene()->activeCamera()->zoomLevel());
}

void MainWindow::setColumnIntervalBySlider1(int value)
{
    m_columnInterval1->setText(QString::number(value));
}
void MainWindow::setColumnIntervalBySlider2(int value)
{
    m_columnInterval2->setText(QString::number(value));
}
void MainWindow::setRawIntervalBySlider1(int value)
{
    m_rawInterval1->setText(QString::number(value));
}
void MainWindow::setRawIntervalBySlider2(int value)
{
    m_rawInterval2->setText(QString::number(value));
}
void MainWindow::isLabelToggled()
{
    if (!m_checkboxShowLabel->isChecked()) {
        m_checkboxShowLabelBorder->setEnabled(false);
    } else {
        m_checkboxShowLabelBorder->setEnabled(true);
    }
}
