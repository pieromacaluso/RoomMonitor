//
// Created by pieromack on 26/07/19.
//

#ifndef ROOMMONITOR_MONITORINGSERVER_H
#define ROOMMONITOR_MONITORINGSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <ui_main.h>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <array>
#include "Packet.h"
#include <mutex>


class MonitoringServer : public QObject {
Q_OBJECT
    QTcpServer *server;
    // TODO: nSchedine è ancora inutile, sono da implementare i thread
    int nSchedine;
    bool running;
    std::deque<Packet> packets;
    std::mutex m;


public:
    MonitoringServer() {

    }

    /**
     * Funzione utilizzata per convertire il vettore di stringhe ricevute dalla esp in oggetti packet
     * @param packets
     * @return
     */
    std::deque<Packet> string2packet(const std::vector<std::string>& packets);

    template<class Container>
    void split(const std::string &str, Container &cont, char delim);

    /**
     * Funzione che viene avviata non appena viene premuto il pulsante Start
     * Si occupa di inizializzare il server (127.0.0.1:1234) e di impostare la callback di ogni nuova connessione
     * a this->newConnection
     */
    void serverStart() {
        server = new QTcpServer(this);
        QObject::connect(server, &QTcpServer::newConnection, this, &MonitoringServer::newConnection);


        if (!server->listen(QHostAddress::Any, 27015)) {
            qDebug() << "Server Did not start";
        } else {
            qDebug() << "Server Started on port:" << server->serverPort();
        }
    }

    /**
     * Chiude il server non appena viene premuto il pulsante di stop
     */
    void serverStop() {
        server->close();
    }

    /**
     * Setup della finestra del Server
     * @param ui Ui finestra principale da cui andare a prendere i vari file.
     */
    void setup(Ui::MainWindow &ui) {
        ui.stopButton->setDisabled(true);

        // Click Start Button

        QObject::connect(ui.startButton, &QPushButton::clicked, [&]() {
            try {
                this->nSchedine = ui.nSchedine->text().toInt();
                this->started(ui.nSchedine->text().toInt());
                ui.startButton->setDisabled(true);
                ui.stopButton->setDisabled(false);
            } catch (std::exception &e) {
                // Does not started signal
                std::cout << "Ecc" << std::endl;
                return;
            }
            this->running = true;

        });

        // Conseguenze Click Start Button

        QObject::connect(this, &MonitoringServer::started, [&]() { this->serverStart(); });

        // Click Stop Button

        QObject::connect(ui.stopButton, &QPushButton::clicked, [&]() {
            this->stopped();
            ui.startButton->setDisabled(false);
            ui.stopButton->setDisabled(true);
            this->running = false;
            this->serverStop();


        });

        // Conseguenze Click Start Button

        QObject::connect(this, &MonitoringServer::stopped, [&]() {
            std::cout << "Stopped" << std::endl;
        });

    }


    /**
     * Split fatto con stringhe per evitare caratteri casuali/involuti inviati dalla schedina
     * @tparam Container
     * @param str
     * @param cont
     * @param startDelim
     * @param stopDelim
     */
    template<class Container>
    void splitString(const std::string &str, Container &cont, std::string &startDelim, std::string &stopDelim) {
        unsigned first = 0;
        unsigned end = 0;
        while ((first = str.find(startDelim, first)) < str.size() && (end = str.find(stopDelim, first)) < str.size()) {
            std::string val = str.substr(first + startDelim.size() + 1, end - first - startDelim.size() - 2);
            cont.push_back(val);
            first = end + stopDelim.size();
        }
    }

public slots:

    /**
     * Slot new Connection
     * E' il corpo principale che rappresenta cosa bisogna fare ogni volta che si presenta una nuova connessione
     */
    void newConnection() {
        std::cout << "New Connection started" << std::endl;
        std::string startDelim("init");
        std::string stopDelim("end");
        QTcpSocket *socket = server->nextPendingConnection();
        std::vector<std::string> pacchetti;
        std::deque<Packet> packetsConn;
        std::string allData{};

        while (socket->waitForReadyRead(1000)) {
            // Concatenazione stringhe ricevute in un'unica stringa
            QByteArray a = socket->readAll();
            if (!a.isEmpty()) {
                std::string packet = a.toStdString();
                allData += packet;
            }
        }

        // Creazione pacchettini
        if (!allData.empty()) {
            // Divisione singoli pacchetti
            MonitoringServer::split(allData, pacchetti, ';');
            // Conversione in oggetti Packet
            packetsConn = string2packet(pacchetti);

            // TODO: ESECUZIONE THREAD_SAFE
            /** INIZIO ESECUZIONE THREAD-SAFE */

            std::unique_lock lk{m};
            for (auto p: packetsConn) {
                std::cout << p << std::endl;
                packets.push_back(p);
            }

            /** FINE ESECUZIONE THREAD-SAFE */
        }

        //todo controllare se va bene un conteiner pacchetti per ogni newConnection (stesso anche in caso la schedina usi più pacchetti tcp per inviare l'intero elenco)
        socket->flush();
        socket->close();
        std::cout << "Connection closed" << std::endl;
        delete socket;

    };


signals:

    /**
     * Signal che segnala l'avvio del Server TCP
     * @param nSchedine numero delle schedine a disposizione
     */
    void started(int nSchedine);

    /**
     * Signal che segnala la chiusura del server TCP
     */
    void stopped();


};


#endif //ROOMMONITOR_MONITORINGSERVER_H
