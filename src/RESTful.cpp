//sgn

#include <iostream>
#include <exception>
#include <fstream>
#include <sstream>

#include <nlohmann/json.hpp>
#include "RESTful.h"

using json = nlohmann::json;

void RESTful::configureRoutes()
{
    Pistache::Rest::Routes::Get(mRouter, "/help", Pistache::Rest::Routes::bind(&RESTful::getHelp, this));
    Pistache::Rest::Routes::Get(mRouter, "/ping", Pistache::Rest::Routes::bind(&RESTful::serverPong, this));
    Pistache::Rest::Routes::Post(mRouter, "/sqlupdate", Pistache::Rest::Routes::bind(&RESTful::updateDB, this));
    Pistache::Rest::Routes::Post(mRouter, "/sqlinsert", Pistache::Rest::Routes::bind(&RESTful::insertToDB, this));
    Pistache::Rest::Routes::Post(mRouter, "/sqlquery", Pistache::Rest::Routes::bind(&RESTful::getQueryResult, this));
}

void RESTful::serverPong(const PistacheReq &request, PistacheResp response) {
    std::cout << "Got ping request" << std::endl;
    response.send(Pistache::Http::Code::Ok, "pong", MIME(Text, Plain));
}

void RESTful::getHelp(const PistacheReq &request, PistacheResp response) {
    std::ifstream iQueryFile("/home/tstone10/sgnHiringAsst/sgnHiringServer/sql_queries.txt");
    std::stringstream ss;
    ss << iQueryFile.rdbuf();
    json pRoot;
    pRoot["queries"] = ss.str();
    response.send(Pistache::Http::Code::Ok, packResponse(true, pRoot.dump()), MIME(Application, Json));    
}

void RESTful::getQueryResult(const PistacheReq &request, PistacheResp response) {
    auto root   = json::parse(request.body());
    auto query  = root.value<std::string>("query", "none");
    auto candidates = mpDBInterface->executeQuery(query);
    candidates.size() ?
        response.send(Pistache::Http::Code::Ok, packResponse(true, candidates), MIME(Application, Json)):
        response.send(Pistache::Http::Code::Not_Found, packResponse(false, "Invalid Query"), MIME(Application, Json));
}

void RESTful::insertToDB(const PistacheReq &request, PistacheResp response) {
    uint32_t iCount = 0;
    std::cout << "Got request to insert" << std::endl;
    auto root       = json::parse(request.body()); 

    if(root.is_array()) for(const auto& jsCandidate : root) {
        Candidate::Ptr pCandidate   = Candidate::fromJson(jsCandidate);
        if(!pCandidate) continue;

        auto candidates = mpDBInterface->executeQuery(pCandidate->getQueryToCheckCandidate());
        if(candidates.size() > 0) continue;

        std::string strInsertQuery  = pCandidate->getInsertQuery();
        mpDBInterface->executeTransaction(strInsertQuery);
        iCount++;
    }

    json pRoot;
    pRoot["rows_inserted"] = iCount;
    (iCount > 0) ?
        response.send(Pistache::Http::Code::Ok, packResponse(true, pRoot.dump()), MIME(Application, Json)):
        response.send(Pistache::Http::Code::Not_Found, packResponse(false, "No new rows inserted"), MIME(Application, Json));
}

void RESTful::updateDB(const PistacheReq &request, PistacheResp response) {
    std::cout << "Got request to update" << std::endl;
    auto root   = json::parse(request.body());

    Candidate::Ptr pCandidate   = Candidate::fromJson(root);
    std::string strUpdateQuery  = pCandidate->getUpdateQuery();

    std::cout << "Update query : " << strUpdateQuery << std::endl;
    mpDBInterface->executeTransaction(strUpdateQuery) ?
        response.send(Pistache::Http::Code::Ok, "{\"isOk\" : true}", MIME(Application, Json)):
        response.send(Pistache::Http::Code::Ok, "{\"isOk\" : false}", MIME(Application, Json));
}

void RESTful::run() {
    mEndPoint->init(Pistache::Http::Endpoint::options().threads(5).flags(Pistache::Tcp::Options::ReuseAddr).flags(Pistache::Tcp::Options::ReusePort));
    configureRoutes();
    mEndPoint->setHandler(mRouter.handler());
    mEndPoint->serve();
}

std::string RESTful::packResponse(bool isSuccess, const std::vector<Candidate::Ptr>& pCandidates) {
    auto jsonCandidates = json::array();
    json pRoot;
    if(isSuccess) {
        pRoot["isOk"]   = isSuccess;
        for(const auto& pCandidate : pCandidates) {
            auto candidate = pCandidate->toJson();
            jsonCandidates.push_back(candidate);
        }
        pRoot["candidates"] = jsonCandidates;
    } else {
        pRoot["isOk"]   = false;
        pRoot["error"]  = "Invalid Query";
    }
    return pRoot.dump();
}

std::string RESTful::packResponse(bool isSuccess, const std::string& pPkt){
    json pRoot;
    if(isSuccess) {
        if(!pPkt.empty()) pRoot = json::parse(pPkt);
        pRoot["isOk"]   = isSuccess;
    } else {
        pRoot["isOk"]   = false;
        pRoot["error"]  = pPkt;
    }
    return pRoot.dump();
}

/*void RESTful::getUser(const PistacheReq &request, PistacheResp response) {
    try {
        uint32_t membershipNo = request.param(":id").as<uint32_t>();
        User::Ptr pUser = mpDBInterface->getUser(membershipNo);
        if(pUser) {
            response.send(Pistache::Http::Code::Ok, packResponse(true, pUser->toJson()), MIME(Application, Json));
        } else {
            response.send(Pistache::Http::Code::Not_Found, packResponse(false, "Unknown membership no"), MIME(Application, Json));
        }
    } catch(std::exception &e) {
        std::cout << e.what() << std::endl;
        response.send(Pistache::Http::Code::Internal_Server_Error, packResponse(false, "Exception getting user"), MIME(Application, Json));
    }
}

void RESTful::putAttendance(const PistacheReq &request, PistacheResp response) {
    try {
        uint32_t membershipNo = request.param(":id").as<uint32_t>();
        User::Ptr pUser = mpDBInterface->getUser(membershipNo); // Check if valid membership no
        if(pUser) {
            bool isSuccess = mpDBInterface->markAttendance(membershipNo);
            if(!isSuccess) {
                response.send(Pistache::Http::Code::Bad_Request, packResponse(false, "Attendance already marked for today"), MIME(Application, Json));
                return;
            }
            Attendance::Ptr pAttendance = mpDBInterface->getAttendance(membershipNo);
            response.send(Pistache::Http::Code::Ok, packResponse(true, pAttendance->toJson()), MIME(Application, Json));
        } else {
            response.send(Pistache::Http::Code::Not_Found, packResponse(false, "Unknown membership no"), MIME(Application, Json));
        }
    } catch(std::exception &e) {
        std::cout << e.what() << std::endl;
        response.send(Pistache::Http::Code::Internal_Server_Error, packResponse(false, "Exception putting attendance"), MIME(Application, Json));
    }
}
    //Pistache::Rest::Routes::Delete(mRouter, "/comic/:id", Pistache::Rest::Routes::bind(&Service::deleteComic, this));
    //Pistache::Rest::Routes::Put(mRouter, "/attendance/:id", Pistache::Rest::Routes::bind(&RESTful::putAttendance, this));
*/