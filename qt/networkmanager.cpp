//sgn

#include "nlohmann_json.hpp"
#include "networkmanager.h"

using json = nlohmann::json;

NetworkManager::NetworkManager(QNetworkAccessManager *pHttpMgr) : mpHttpMgr{pHttpMgr} {}

NetworkManager::~NetworkManager() {}

bool NetworkManager::pingServer() {
    QNetworkRequest request;
    request.setUrl(QUrl("http://ugc13r4hn83.chennaigtbc.ford.com:8080/ping"));
    mpHttpMgr->get(request);
    return true;
}

bool NetworkManager::queryRemoteDB(const QString& pQuery) {
    if(pQuery.length() == 0) return false;

    auto root = json();
    root["query"] = pQuery.toStdString();

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(QUrl("http://ugc13r4hn83.chennaigtbc.ford.com:8080/sqlquery"));

    mpHttpMgr->post(request, root.dump().c_str());
    return true;
}

bool NetworkManager::insertToDB(const QString& jsonCandidates) {
    if(jsonCandidates.length() == 0) return false;

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(QUrl("http://ugc13r4hn83.chennaigtbc.ford.com:8080/sqlinsert"));
    mpHttpMgr->post(request, jsonCandidates.toUtf8());
    return true;
}

bool NetworkManager::updateDB(const QString& jsonCandidate) {
    if(jsonCandidate.length() == 0) return false;

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(QUrl("http://ugc13r4hn83.chennaigtbc.ford.com:8080/sqlupdate"));
    mpHttpMgr->post(request, jsonCandidate.toUtf8());
    return true;
}

bool NetworkManager::getSampleQueries() {
    QNetworkRequest request;

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(QUrl("http://ugc13r4hn83.chennaigtbc.ford.com:8080/help"));
    mpHttpMgr->get(request);
    return true;
}
