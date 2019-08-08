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
#include <QTimer>
#include <map>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QtWidgets/QMessageBox>
#include <QDebug>
#include <QDateTime>


class MonitoringServer : public QObject {
Q_OBJECT
    QTcpServer *server;
    // TODO: nSchedine è ancora inutile, sono da implementare i thread
    int nSchedine;
    bool running;
    std::deque<std::pair<Packet, int>> packets;
    std::mutex m;
    QTimer *timer;
    QSqlDatabase nDatabase;


public:
    MonitoringServer() {
        timer = nullptr;
        nDatabase = QSqlDatabase::addDatabase("QMYSQL");
        nDatabase.setHostName("localhost");
        nDatabase.setDatabaseName("data");
        nDatabase.setPort(3306);
        nDatabase.setUserName("root");
        nDatabase.setPassword("NewRoot12Kz");
    }

    ~MonitoringServer() {
        if (timer != nullptr) {
            delete timer;
        }
    }

    /**
     * Connessione al database
     */
    void connectDB() {
        ;
        if (!nDatabase.open()) {
            qDebug() << nDatabase.lastError();
            return;
        }
        qDebug() << "DB connected";
    }

    void disconnectDB() {
        nDatabase.close();
    }

    /**
     * Funzione utilizzata per convertire il vettore di stringhe ricevute dalla esp in oggetti packet
     * @param packets
     * @return
     */
    std::deque<Packet> string2packet(const std::vector<std::string> &packets);

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
            timer = new QTimer(this);
            connect(timer, &QTimer::timeout, this, &MonitoringServer::aggregate);
            timer->start(30000);
        }
    }

    /**
     * Chiude il server non appena viene premuto il pulsante di stop
     */
    void serverStop() {
        server->close();
        timer->stop();
        delete timer;
        timer = nullptr;
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
        // TODO: finetuning di questo parametro, più è piccolo, meglio è!
        while (socket->waitForReadyRead(500)) {
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
            for (auto &p: packetsConn) {
                std::cout << p << std::endl;
                packets.emplace_back(std::make_pair(p, 0));
            }

            /** FINE ESECUZIONE THREAD-SAFE */
        }

        //todo controllare se va bene un conteiner pacchetti per ogni newConnection (stesso anche in caso la schedina usi più pacchetti tcp per inviare l'intero elenco)
        socket->flush();
        socket->close();
        std::cout << "Connection closed" << std::endl;
        delete socket;

    };

    void aggregate() {

        connectDB();

        // TODO: Capire come aggregare bene

        std::unique_lock lk{m};

        // Generazione Mappa <ChiavePacchetto, DequePacchetto>

        /* TODO: capire se si può fare direttamente in acquisizione, secondo me si può usare la mappa in maniera thread safe
         * in questo modo si andrebbe a creare l'aggregazione per id pacchetto già in ricezione, limitando lo sforzo
         * computazionale che viene fatto in aggregate*/

        std::map<std::string, std::deque<Packet>> aggregate{};
        std::for_each(packets.begin(), packets.end(), [&](std::pair<Packet, int> el) {
            std::string id = el.first.getFcs();
            el.second++;
            auto it = aggregate.find(id);
            if (it == aggregate.end()) {
                std::deque<Packet> newDeque{};
                newDeque.push_back(el.first);
                aggregate.insert(std::make_pair(id, newDeque));
            } else {
                it->second.push_back(el.first);
            }
        });

        // Eliminazione di tutti i pacchetti che non possiedono un numero di elementi pari al numero di schedine.
        for (auto i = aggregate.begin(), last = aggregate.end(); i != last;) {
            if (nSchedine != i->second.size()) {
                i = aggregate.erase(i);
            } else {

                for (auto it = packets.begin(); it != packets.end();) {
                    if (it->first.getFcs() == i->first) {
                        it = packets.erase(it);
                    } else {
                        ++it;
                    }
                }
                ++i;
            }
        }

        // TODO: Calcolo della posizione da RSSI

        // Stampa id pacchetti aggregati rilevati.
        std::cout << "Starting aggregation" << std::endl;
        for (auto fil : aggregate) {
            std::cout << "ID packet:" << fil.first << " " << fil.second.begin()->getMacPeer() << std::endl;
            /*
             * TODO: Query al database, capire cosa e quanto salvare
             * Attualmente non trovo utilità del campo id e del campo board. L'RSSI dovrà essere modificato con le
             * coordinate (x,y) se non sbaglio. Il timestamp forse sarebbe da "castrare" ad ogni minuto, esempio 16:23:40
             * diventa 16:23:00.
             */

            /*
             * TODO: aggregazione di pacchetti multipli appartenenti allo stesso MAC in un'unico pacchetto.
             */
            QSqlQuery query;
            query.prepare(
                    "INSERT INTO campi (id, hash, rssi, mac, timestamp, ssid, board) VALUES (:id, :hash, :rssi, :mac, :timestamp, :ssid, :board);");
            query.bindValue(":id", 0);
            query.bindValue(":hash", QString::fromStdString(fil.second.begin()->getFcs()));
            query.bindValue(":rssi", fil.second.begin()->getRssi());
            query.bindValue(":mac", QString::fromStdString(fil.second.begin()->getMacPeer()));
            query.bindValue(":timestamp", QDateTime::fromSecsSinceEpoch(fil.second.begin()->getTimestamp()));
            query.bindValue(":ssid", QString::fromStdString(fil.second.begin()->getSsid()));
            query.bindValue(":board", fil.second.begin()->getIdSchedina());
            if (!query.exec()) {
                qDebug() << query.lastError();
            }
        }
        std::cout << "Ending aggregation" << std::endl;



        // Pulizia deque pacchetti attraverso meccanismo di second chance
        /* Un pacchetto viene eliminato dalla deque solo ed esclusivamente dopo due aggregate. In questo maniera
         * si dovrebbe evitare la possibilità che vengano analizzate ricezioni di pacchetti parziali */
        for (auto it = packets.begin(); it != packets.end();) {
            if (it->second >= 2) {
                it = packets.erase(it);
            } else {
                ++it;
            }
        }

        disconnectDB();
    }


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
