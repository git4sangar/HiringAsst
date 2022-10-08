//sgn
#pragma once

#include <memory>

#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/mime.h>
#include <pistache/net.h>
#include <pistache/router.h>

#include "DBInterface.h"

class RESTful {
public:
    using PistacheReq   = Pistache::Rest::Request;
    using PistacheResp  = Pistache::Http::ResponseWriter;

    RESTful(uint32_t pPort, DBInterface::Ptr pDBInterface)
    : mpDBInterface(pDBInterface)
    //, mEndPoint(std::make_shared<Pistache::Http::Endpoint>(Pistache::Address("localhost", pPort)))
    , mEndPoint(std::make_shared<Pistache::Http::Endpoint>(Pistache::Address("UGC13R4HN83.chennaigtbc.ford.com", pPort)))
    {}

    virtual ~RESTful() {}
    void run();

private:
    void configureRoutes();
    void getHelp(const PistacheReq &request, PistacheResp response);
    void getQueryResult(const PistacheReq &request, PistacheResp response);
    void insertToDB(const PistacheReq &request, PistacheResp response);
    void updateDB(const PistacheReq &request, PistacheResp response);
    void serverPong(const PistacheReq &request, PistacheResp response);
    std::string packResponse(bool isSuccess, const std::vector<Candidate::Ptr>& pCandidates);
    std::string packResponse(bool isSuccess, const std::string& pPkt);

    DBInterface::Ptr mpDBInterface;
    Pistache::Rest::Router mRouter;
    std::shared_ptr<Pistache::Http::Endpoint> mEndPoint;
};
