#ifndef PLOT_H
#define PLOT_H

#include <QLabel>
#include <QLocale>
#include <QMainWindow>
#include <QObject>
#include <QStatusBar>
#include <QString>
#include <QTranslator>
#include <QtCore/qmath.h>
#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/Q3DTheme>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QValue3DAxis>
#include <QtWidgets/QSlider>

class Plot : public QObject
{
    Q_OBJECT
public:
    int sampleCountX = 50;
    int sampleCountZ = 50;
    const float sampleMin = -10.0f;
    const float sampleMax = 10.0f;
    bool m_isSinc1Enabled;
    bool m_isSinc2Enabled;
    bool isLabelOn;
    QString m_curGradient1;
    QString m_curGradient2;

    Plot(QtDataVisualization::Q3DSurface *surface, QMainWindow *window);
    ~Plot();

    QString getCurrentGradient1() { return m_curGradient1; }
    QString getCurrentGradient2() { return m_curGradient2; }

public slots:
    void enableGrid(bool enable);
    void enableLabelBorder(bool enable);
    void enableBorder(bool enable);

    void toggleModeNone()
    {
        m_graph->setSelectionMode(QtDataVisualization::QAbstract3DGraph::SelectionNone);
    }
    void toggleModeItem()
    {
        m_graph->setSelectionMode(QtDataVisualization::QAbstract3DGraph::SelectionItem);
    }

    void adjustXMin(int min);
    void adjustXMax(int max);
    void adjustZMin(int min);
    void adjustZMax(int max);

    void setAxisMinSliderX(QSlider *slider) { m_axisMinSliderX = slider; }
    void setAxisMaxSliderX(QSlider *slider) { m_axisMaxSliderX = slider; }
    void setAxisMinSliderZ(QSlider *slider) { m_axisMinSliderZ = slider; }
    void setAxisMaxSliderZ(QSlider *slider) { m_axisMaxSliderZ = slider; }
    void initSliders();

    void enableSinc_1(bool enable);
    void enableSinc_2(bool enable);

    void setBlackToYellowGradient();
    void setGreenToRedGradient();

    void setCountX(QString newCountX);
    void setCountZ(QString newCountX);

    void setColumnInterval1(QString newcolumnInterval1);
    void setColumnInterval2(QString newcolumnInterval2);
    void setRawInterval1(QString value);
    void setRawInterval2(QString value);

private slots:
    void pointChanged(const QPoint &pt);

private:
    float m_rangeMinX;
    float m_rangeMinZ;
    float m_stepX;
    float m_stepZ;
    int m_heightMapWidth;
    int m_heightMapHeight;

    QSlider *m_axisMinSliderX;
    QSlider *m_axisMaxSliderX;
    QSlider *m_axisMinSliderZ;
    QSlider *m_axisMaxSliderZ;

    QMainWindow *m_MainWindow;
    QtDataVisualization::Q3DSurface *m_graph;

    QtDataVisualization::QSurfaceDataProxy *m_sqrtSinProxy;
    QtDataVisualization::QSurface3DSeries *m_sincSeries;

    void fillSqrtSinProxy_1();
    void fillSqrtSinProxy_2();
    void enableSincModel(bool enable);
    void setAxisXRange(float min, float max);
    void setAxisZRange(float min, float max);
};

#endif // PLOT_H
