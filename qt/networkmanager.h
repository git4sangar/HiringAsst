//sgn
#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <memory>
#include <QtNetwork/QNetworkAccessManager>

class NetworkManager
{
public:
    typedef std::shared_ptr<NetworkManager> Ptr;

    NetworkManager(QNetworkAccessManager *pHttpMgr);
    virtual ~NetworkManager();

    bool pingServer();
    bool queryRemoteDB(const QString& pQuery);
    bool getSampleQueries();
    bool insertToDB(const QString& jsonCandidates);
    bool updateDB(const QString& jsonCandidate);

private:
    QNetworkAccessManager *mpHttpMgr;
};

#endif // NETWORKMANAGER_H
