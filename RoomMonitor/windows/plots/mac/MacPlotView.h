#ifndef ROOMMONITOR_MACPLOTVIEW_H
#define ROOMMONITOR_MACPLOTVIEW_H

#include <QtCharts>
#include "MacChart.h"
#include "windows/common/callout.h"


class MacPlotView : public QChartView {
    MacChart * chart = nullptr;
    Callout *m_tooltip = nullptr;
    QString calloutText = "%1\nPresenze: %2";
public:
    explicit MacPlotView(QWidget *parent = nullptr);

    explicit MacPlotView(QChart *chart, QWidget *parent = nullptr);

    void setChart(MacChart *chart);

    void mouseDoubleClickEvent(QMouseEvent *event);

    void wheelEvent(QWheelEvent *event);

    void tooltip(bool state, int index, QBarSet *barset);

    void resizeEvent(QResizeEvent *event);

    MacChart *getChart() const;

    void setCalloutText(const QString &calloutText);

};


#endif //ROOMMONITOR_MACPLOTVIEW_H
