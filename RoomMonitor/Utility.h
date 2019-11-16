//
// Created by pieromack on 01/10/19.
//

#ifndef ROOMMONITOR_UTILITY_H
#define ROOMMONITOR_UTILITY_H


#include <QtSql>
#include <QObject>
#include <QtCore/QSettings>
#include <QDebug>
#include <QThread>
#include <QtCharts>
#include "Strings.h"
#include <vector>
#include <monitoring/Board.h>
#include "Query.h"

class Utility {
public:
    static const QString ORGANIZATION;
    static const QString APPLICATION;
    static QSqlDatabase getDB(bool &error);
    static QLineSeries * generateRoomSeries(QObject *parent = nullptr);
    static void warningMessage(const QString &title, const QString &text, const QString &error);
    static void infoMessage(const QString &title, const QString &text);
    static int infoMessageTimer(const QString &title, const QString &text, int millisec);
    static bool yesNoMessage(QWidget *parent, const QString &title, const QString &text);

    static bool testTable(QSqlDatabase &db);

    static std::vector<Board> getBoards();

    static void dropBoards();

    static const int RETRY_STEP_BOARD;
};


#endif //ROOMMONITOR_UTILITY_H
