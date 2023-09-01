#include "plot.h"

Plot::Plot(QtDataVisualization::Q3DSurface *surface, QMainWindow *window)
    : m_MainWindow(window)
    , m_graph(surface)
{
    setParent(window);
    m_graph->setAxisX(new QtDataVisualization::QValue3DAxis);
    m_graph->setAxisY(new QtDataVisualization::QValue3DAxis);
    m_graph->setAxisZ(new QtDataVisualization::QValue3DAxis);

    m_sqrtSinProxy = new QtDataVisualization::QSurfaceDataProxy();
    m_sincSeries = new QtDataVisualization::QSurface3DSeries(m_sqrtSinProxy);

    QObject::connect(m_sincSeries,
                     &QtDataVisualization::QSurface3DSeries::selectedPointChanged,
                     this,
                     &Plot::pointChanged);

    enableSincModel(true);
    fillSqrtSinProxy_1();
    return;
}

Plot::~Plot()
{
    delete m_graph;
}

void Plot::enableGrid(bool enable)
{
    m_graph->activeTheme()->setGridEnabled(enable);
}

void Plot::enableLabelBorder(bool enable)
{
    if (isLabelOn) {
        m_graph->activeTheme()->setLabelBorderEnabled(enable);
    }
}

void Plot::enableBorder(bool enable)
{
    isLabelOn = enable;
    m_graph->axisX()->setLabelFormat(enable ? "%.2f" : "");
    m_graph->axisY()->setLabelFormat(enable ? "%.2f" : "");
    m_graph->axisZ()->setLabelFormat(enable ? "%.2f" : "");
}

void Plot::pointChanged(const QPoint &pt)
{
    if ((pt.x() >= 0) && (pt.y() >= 0)) {
        QString msg = "x=" + QString::number(m_sincSeries->dataProxy()->itemAt(pt)->x())
                      + ", y= " + QString::number(m_sincSeries->dataProxy()->itemAt(pt)->y())
                      + ", z= " + QString::number(m_sincSeries->dataProxy()->itemAt(pt)->z());
        m_MainWindow->statusBar()->showMessage(msg);
    } else {
        m_MainWindow->statusBar()->showMessage("");
    }
}

void Plot::enableSinc_1(bool enable)
{
    if (enable) {
        m_isSinc1Enabled = true;
        m_isSinc2Enabled = false;
        fillSqrtSinProxy_1();
        if (getCurrentGradient1() == "yellow") {
            setBlackToYellowGradient();
        } else {
            setGreenToRedGradient();
        }
    }
}

void Plot::enableSinc_2(bool enable)
{
    m_isSinc1Enabled = false;
    m_isSinc2Enabled = true;
    if (enable)
        fillSqrtSinProxy_2();
    if (getCurrentGradient2() == "yellow") {
        setBlackToYellowGradient();
    } else {
        setGreenToRedGradient();
    }
}

void Plot::fillSqrtSinProxy_1()
{
    float stepX = (sampleMax - sampleMin) / float(sampleCountX - 1);
    float stepZ = (sampleMax - sampleMin) / float(sampleCountZ - 1);

    QtDataVisualization::QSurfaceDataArray *dataArray = new QtDataVisualization::QSurfaceDataArray;
    dataArray->reserve(sampleCountZ);
    for (int i = 0; i < sampleCountZ; i++) {
        QtDataVisualization::QSurfaceDataRow *newRow = new QtDataVisualization::QSurfaceDataRow(
            sampleCountX);
        float z = qMin(sampleMax, (i * stepZ + sampleMin));
        int index = 0;
        for (int j = 0; j < sampleCountX; j++) {
            float x = qMin(sampleMax, (j * stepX + sampleMin));
            float y = qSin(qSqrt(x * x + z * z)) / qSqrt(x * x + z * z);
            (*newRow)[index++].setPosition(QVector3D(x, y, z));
        }
        *dataArray << newRow;
    }
    m_sqrtSinProxy->resetArray(dataArray);
}

void Plot::fillSqrtSinProxy_2()
{
    float stepX = (sampleMax - sampleMin) / float(sampleCountX - 1);
    float stepZ = (sampleMax - sampleMin) / float(sampleCountZ - 1);
    QtDataVisualization::QSurfaceDataArray *dataArray = new QtDataVisualization::QSurfaceDataArray;
    dataArray->reserve(sampleCountZ);
    for (int i = 0; i < sampleCountZ; i++) {
        QtDataVisualization::QSurfaceDataRow *newRow = new QtDataVisualization::QSurfaceDataRow(
            sampleCountX);
        float z = qMin(sampleMax, (i * stepZ + sampleMin));
        int index = 0;
        for (int j = 0; j < sampleCountX; j++) {
            float x = qMin(sampleMax, (j * stepX + sampleMin));
            float y = qSin(x) / x * qSin(z) / z;
            (*newRow)[index++].setPosition(QVector3D(x, y, z));
        }
        *dataArray << newRow;
    }
    m_sqrtSinProxy->resetArray(dataArray);
}

void Plot::enableSincModel(bool enable)
{
    if (enable) {
        m_sincSeries->setDrawMode(QtDataVisualization::QSurface3DSeries::DrawSurfaceAndWireframe);
        m_sincSeries->setFlatShadingEnabled(true);

        m_graph->axisX()->setLabelFormat("%.2f");
        m_graph->axisZ()->setLabelFormat("%.2f");
        m_graph->axisX()->setRange(sampleMin, sampleMax);
        m_graph->axisY()->setRange(-0.25f, 1.0f);
        m_graph->axisZ()->setRange(sampleMin, sampleMax);
        m_graph->axisX()->setLabelAutoRotation(30);
        m_graph->axisY()->setLabelAutoRotation(90);
        m_graph->axisZ()->setLabelAutoRotation(30);
        m_graph->addSeries(m_sincSeries);
        m_rangeMinX = sampleMin;
        m_rangeMinZ = sampleMin;
        m_stepX = (sampleMax - sampleMin) / float(sampleCountX - 1);
        m_stepZ = (sampleMax - sampleMin) / float(sampleCountZ - 1);
        return;
    }
}

void Plot::adjustXMin(int min)
{
    float minX = m_stepX * float(min) + m_rangeMinX;

    int max = m_axisMaxSliderX->value();
    if (min >= max) {
        max = min + 1;
        m_axisMaxSliderX->setValue(max);
    }
    float maxX = m_stepX * max + m_rangeMinX;

    setAxisXRange(minX, maxX);
}

void Plot::adjustXMax(int max)
{
    float maxX = m_stepX * float(max) + m_rangeMinX;

    int min = m_axisMinSliderX->value();
    if (max <= min) {
        min = max - 1;
        m_axisMinSliderX->setValue(min);
    }
    float minX = m_stepX * min + m_rangeMinX;

    setAxisXRange(minX, maxX);
}

void Plot::adjustZMin(int min)
{
    float minZ = m_stepZ * float(min) + m_rangeMinZ;

    int max = m_axisMaxSliderZ->value();
    if (min >= max) {
        max = min + 1;
        m_axisMaxSliderZ->setValue(max);
    }
    float maxZ = m_stepZ * max + m_rangeMinZ;

    setAxisZRange(minZ, maxZ);
}

void Plot::adjustZMax(int max)
{
    float maxX = m_stepZ * float(max) + m_rangeMinZ;

    int min = m_axisMinSliderZ->value();
    if (max <= min) {
        min = max - 1;
        m_axisMinSliderZ->setValue(min);
    }
    float minX = m_stepZ * min + m_rangeMinZ;

    setAxisZRange(minX, maxX);
}

void Plot::setAxisXRange(float min, float max)
{
    m_graph->axisX()->setRange(min, max);
}

void Plot::setAxisZRange(float min, float max)
{
    m_graph->axisZ()->setRange(min, max);
}

void Plot::initSliders()
{
    if ((sampleCountX > 3 && sampleCountX < 100000) && (sampleCountZ > 3 && sampleCountZ < 100000)) {
        m_axisMinSliderX->setMaximum(sampleCountX - 2);
        m_axisMinSliderX->setValue(0);
        m_axisMaxSliderX->setMaximum(sampleCountX - 1);
        m_axisMaxSliderX->setValue(sampleCountX - 1);
        m_axisMinSliderZ->setMaximum(sampleCountZ - 2);
        m_axisMinSliderZ->setValue(0);
        m_axisMaxSliderZ->setMaximum(sampleCountZ - 1);
        m_axisMaxSliderZ->setValue(sampleCountZ - 1);
    } else {
        m_axisMinSliderX->setMaximum(48);
        m_axisMinSliderX->setValue(0);
        m_axisMaxSliderX->setMaximum(49);
        m_axisMaxSliderX->setValue(49);
        m_axisMinSliderZ->setMaximum(48);
        m_axisMinSliderZ->setValue(0);
        m_axisMaxSliderZ->setMaximum(49);
        m_axisMaxSliderZ->setValue(49);
    }
}

void Plot::setBlackToYellowGradient()
{
    QLinearGradient gr;
    gr.setColorAt(1.0, QColor(0, 0, 0));
    gr.setColorAt(0.8, QColor(120, 120, 0));
    gr.setColorAt(0.6, QColor(255, 255, 0));
    gr.setColorAt(0.4, QColor(163, 163, 65));
    gr.setColorAt(0.2, QColor(156, 156, 112));
    gr.setColorAt(0.0, QColor(176, 176, 176));

    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(
        QtDataVisualization::Q3DTheme::ColorStyleRangeGradient);
    if (m_isSinc1Enabled) {
        m_curGradient1 = "yellow";
    } else {
        m_curGradient2 = "yellow";
    }
}

void Plot::setGreenToRedGradient()
{
    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::darkGreen);
    gr.setColorAt(0.5, Qt::yellow);
    gr.setColorAt(0.8, Qt::red);
    gr.setColorAt(1.0, Qt::darkRed);

    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(
        QtDataVisualization::Q3DTheme::ColorStyleRangeGradient);

    if (m_isSinc1Enabled) {
        m_curGradient1 = "red";
    } else {
        m_curGradient2 = "red";
    }
}

void Plot::setCountX(QString value)
{
    if (value != "" && value.toInt() > 3) {
        sampleCountX = value.toInt();
        enableSincModel(true);
        initSliders();
        if (m_isSinc1Enabled) {
            fillSqrtSinProxy_1();
        } else {
            fillSqrtSinProxy_2();
        }
    }
}

void Plot::setCountZ(QString value)
{
    if (value != "" && value.toInt() > 3) {
        sampleCountZ = value.toInt();
        enableSincModel(true);
        initSliders();
        if (m_isSinc1Enabled) {
            fillSqrtSinProxy_1();
        } else {
            fillSqrtSinProxy_2();
        }
    }
}

void Plot::setColumnInterval1(QString newcolumnInterval1)
{
    m_axisMinSliderX->setValue(newcolumnInterval1.toInt());
}

void Plot::setColumnInterval2(QString newcolumnInterval2)
{
    m_axisMaxSliderX->setValue(newcolumnInterval2.toInt());
}

void Plot::setRawInterval1(QString value)
{
    m_axisMinSliderZ->setValue(value.toInt());
}

void Plot::setRawInterval2(QString value)
{
    m_axisMaxSliderZ->setValue(value.toInt());
}
