//sgn
#include <ctime>
#include <memory>
#include <sstream>

#include <nlohmann/json.hpp>

#include "MyDateTime.h"
#include "DBInterface.h"

using json = nlohmann::json;

DBInterface* DBInterface::pThis = nullptr;

std::vector<Candidate::Ptr> DBInterface::executeQuery(const std::string& pQuery) {
    std::vector<Candidate::Ptr> candidates;
    Candidate::Ptr pCandidate;
    try {
        SQLite::Statement query(*mDB, pQuery);
        while(query.executeStep()) {
            pCandidate = Candidate::parseCandidate(&query);
            candidates.push_back(pCandidate);
        }
    } catch(std::exception &e) { std::cout << e.what() << std::endl; }
    return candidates;
}

bool DBInterface::executeTransaction(const std::string& pQuery) {
    if(pQuery.empty()) return false;
    SQLite::Transaction transaction(*mDB);
    if(mDB->exec(pQuery)) {
        transaction.commit();
        return true;
    }
    return false;
}

Candidate::Ptr Candidate::parseCandidate(SQLite::Statement *pQuery) {
    if(!pQuery) return nullptr;

    Candidate::Ptr pCandidate   = std::make_shared<Candidate>();
    uint32_t iColCount  = pQuery->getColumnCount();

    for(uint32_t iLoop = 0; iLoop < iColCount; iLoop++) {
        std::string colName    = pQuery->getColumnName(iLoop);
             if(colName == "id")             pCandidate->mId             = pQuery->getColumn("id").getUInt();
        else if(colName == "jd")             pCandidate->mJd             = pQuery->getColumn("jd").getUInt();
        else if(colName == "status")         pCandidate->mStatus         = pQuery->getColumn("status").getUInt();
        else if(colName == "name")           pCandidate->mName           = pQuery->getColumn("name").getString();
        else if(colName == "mobile")         pCandidate->mMobile         = pQuery->getColumn("mobile").getInt64();
        else if(colName == "vendor")         pCandidate->mVendor         = pQuery->getColumn("vendor").getString();
        else if(colName == "resume")         pCandidate->mResume         = pQuery->getColumn("resume").getString();
        else if(colName == "remarks")        pCandidate->mRemarks        = pQuery->getColumn("remarks").getString();
        else if(colName == "experience")     pCandidate->mExperience     = pQuery->getColumn("experience").getDouble();
        else if(colName == "referred_on")    pCandidate->mReferredOn     = pQuery->getColumn("referred_on").getInt64();
        else if(colName == "last_call")      pCandidate->mLastCall       = pQuery->getColumn("last_call").getInt64();
        else if(colName == "next_call")      pCandidate->mNextCall       = pQuery->getColumn("next_call").getInt64();
        else if(colName == "doj")            pCandidate->mDOJ            = pQuery->getColumn("doj").getInt64();
        else if(colName == "lwd")            pCandidate->mLWD            = pQuery->getColumn("lwd").getInt64();
        else pCandidate->mCount = pQuery->getColumn(colName.c_str()).getUInt();
    }
    return pCandidate;
}

json Candidate::toJson() {
    json jsCandidate;

    if(mId)                     jsCandidate["id"]       = mId;
    if(mJd)                     jsCandidate["jd"]       = mJd;
    if(mStatus)                 jsCandidate["status"]   = mStatus;
    if(!mName.empty())          jsCandidate["name"]     = mName;
    if(!mVendor.empty())        jsCandidate["vendor"]   = mVendor;
    if(!mResume.empty())        jsCandidate["resume"]   = mResume;
    if(!mRemarks.empty())       jsCandidate["remarks"]  = mRemarks;
    if(mExperience > 0.1)       jsCandidate["experience"]       = mExperience;
    if(mMobile)                 jsCandidate["mobile"]           = std::to_string(mMobile);
    if(mReferredOn)             jsCandidate["referred_on"]      = MyDateTime(mReferredOn).getDateStr();
    if(mLastCall)               jsCandidate["last_call"]        = MyDateTime(mLastCall).getDateStr();
    if(mNextCall)               jsCandidate["next_call"]        = MyDateTime(mNextCall).getDateStr();
    if(mDOJ)                    jsCandidate["doj"]              = MyDateTime(mDOJ).getDateStr();
    if(mLWD)                    jsCandidate["lwd"]              = MyDateTime(mLWD).getDateStr();
    if(mCount > -1)             jsCandidate["count"]            = mCount;
    return jsCandidate;
}

std::string Candidate::getUpdateQuery() {
    std::stringstream ss, ssUpdate;
    std::string strPrefix;

    if(!mName.empty())          { ss << strPrefix << " name = \"" << mName << "\""; strPrefix = ","; }
    if(mStatus)                 { ss << strPrefix << " status = " << mStatus; strPrefix = ","; }
    if(!mVendor.empty())        { ss << strPrefix << " vendor = \"" << mVendor << "\""; strPrefix = ","; }
    if(!mResume.empty())        { ss << strPrefix << " resume = \"" << mResume << "\""; strPrefix = ","; }
    if(!mRemarks.empty())       { ss << strPrefix << " remarks = \"" << mRemarks << "\""; strPrefix = ","; }
    if(mExperience > 0.1)       { ss << strPrefix << " experience = " << mExperience; strPrefix = ","; }
    if(mMobile)                 { ss << strPrefix << " mobile = " << mMobile; strPrefix = ","; }
    if(mReferredOn)             { ss << strPrefix << " referred_on = " << mReferredOn; strPrefix = ","; }
    if(mLastCall)               { ss << strPrefix << " last_call = " << mLastCall; strPrefix = ","; }
    if(mNextCall)               { ss << strPrefix << " next_call = " << mNextCall; strPrefix = ","; }
    if(mDOJ)                    { ss << strPrefix << " doj = " << mDOJ; strPrefix = ","; }
    if(mLWD)                    { ss << strPrefix << " lwd = " << mLWD; strPrefix = ","; }
    if(ss.str().length())       ssUpdate << "UPDATE Candidates SET " << ss.str() << " WHERE id = " << mId << ";";

    return ssUpdate.str();
}

Candidate::Ptr Candidate::fromJson(const json& pJsonCandidate) {
    if(pJsonCandidate.size() == 0) return nullptr;

    Candidate::Ptr pCandidate = std::make_shared<Candidate>();

    if(pJsonCandidate.contains("id"))               pCandidate->mId             = pJsonCandidate.value<int32_t>("id", 0);
    if(pJsonCandidate.contains("jd"))               pCandidate->mJd             = pJsonCandidate.value<int32_t>("jd", 0);
    if(pJsonCandidate.contains("experience"))       pCandidate->mExperience     = pJsonCandidate.value<double>("experience", 0.01);
    if(pJsonCandidate.contains("name"))             pCandidate->mName           = pJsonCandidate.value<std::string>("name", "");
    if(pJsonCandidate.contains("mobile"))           pCandidate->mMobile         = std::stoll(pJsonCandidate.value<std::string>("mobile", ""));
    if(pJsonCandidate.contains("status"))           pCandidate->mStatus         = pJsonCandidate.value<uint32_t>("status", 0);
    if(pJsonCandidate.contains("vendor"))           pCandidate->mVendor         = pJsonCandidate.value<std::string>("vendor", "");
    if(pJsonCandidate.contains("resume"))           pCandidate->mResume         = pJsonCandidate.value<std::string>("resume", "");
    if(pJsonCandidate.contains("remarks"))          pCandidate->mRemarks        = pJsonCandidate.value<std::string>("remarks", "");
    if(pJsonCandidate.contains("referred_on"))      pCandidate->mReferredOn     = MyDateTime::create(pJsonCandidate, "referred_on", "dd-MM-yyyy")->getEpoch();
    if(pJsonCandidate.contains("last_call"))        pCandidate->mLastCall       = MyDateTime::create(pJsonCandidate, "last_call", "dd-MM-yyyy")->getEpoch();
    if(pJsonCandidate.contains("next_call"))        pCandidate->mNextCall       = MyDateTime::create(pJsonCandidate, "next_call", "dd-MM-yyyy")->getEpoch();
    if(pJsonCandidate.contains("doj"))              pCandidate->mDOJ            = MyDateTime::create(pJsonCandidate, "doj", "dd-MM-yyyy")->getEpoch();
    if(pJsonCandidate.contains("lwd")) {
        std::string strDate = pJsonCandidate.value<std::string>("lwd", "");
        pCandidate->mLWD    = MyDateTime::checkFormatAndCreate(strDate, pCandidate->mReferredOn)->getEpoch();
    }
    if(pJsonCandidate.contains("count"))            pCandidate->mCount          = pJsonCandidate.value<int32_t>("count", -1);

    return pCandidate;
}

std::string Candidate::getInsertQuery() {
    std::stringstream ss;

    ss.str(""); ss << "INSERT INTO Candidates (jd, name, mobile, experience, vendor, status, resume, referred_on, last_call, next_call, lwd, doj, remarks) VALUES ("
        << mJd << ", \"" << mName << "\", " << mMobile << ", " << mExperience << ", \"" << mVendor << "\", " << mStatus << ", \"" << mResume << "\", "
        << mReferredOn << ", " << mLastCall << ", " << mNextCall << ", " << mLWD << ", " << mDOJ << ", \"" << mRemarks << "\");";
    return ss.str();
}

/*User::Ptr DBInterface::getUser(int32_t pMembershipNo) {
    if(pMembershipNo <= 0) return nullptr;

    User::Ptr pUser = nullptr;
    std::stringstream ss;
    ss << "SELECT * FROM User WHERE membership_no = " << pMembershipNo << ";";

    SQLite::Statement query(*mDB, ss.str());
    if(query.executeStep()) {
        pUser   = User::parseUser(&query);
    }
    return pUser;
}

bool DBInterface::addNewUser(User::Ptr pUser) {
    if(!pUser) return false;
    std::stringstream ss;

    ss << "SELECT * FROM User WHERE membership_no = " << pUser->mMembershipNo << ";";
    SQLite::Statement query(*mDB, ss.str());
    if(query.executeStep())  return false;

    SQLite::Transaction transaction(*mDB);
    ss.str(""); ss << "INSERT INTO user (name, membership_no, dob, validity_end, last_visit, address, email, photo, mobile) VALUES ("
                << "\"" << pUser->mName << "\", " << pUser->mMembershipNo << ", " << pUser->mDOB << ", " << pUser->mValidityEnd
                << ", " << pUser->mLastVisit << ", \"" << pUser->mAddress << "\", \"" << pUser->mEmail << "\", \"" << pUser->mPhoto << "\", \""
                << pUser->mMobile << "\");";
    mDB->exec(ss.str());
    transaction.commit();
    return true;
}

Attendance::Ptr DBInterface::getAttendance(uint32_t pMembershipNo) {
    if(pMembershipNo == 0) return nullptr;

    std::stringstream ss;
    MyDateTime::Ptr pNow = std::make_shared<MyDateTime>();

    ss.str(""); ss << "SELECT * FROM Attendance WHERE membership_no = " << pMembershipNo
                    << " AND in_date_string = \"" << pNow->getDateStr() << "\";";
    SQLite::Statement query(*mDB, ss.str());
    if(query.executeStep()) return Attendance::parseAttendance(&query);
    return nullptr;
}

bool DBInterface::markAttendance(int32_t pMembershipNo) {
    if(pMembershipNo == 0) return false;
    std::stringstream ss;

    MyDateTime::Ptr pNow        = std::make_shared<MyDateTime>();
    Attendance::Ptr pAttendance = getAttendance(pMembershipNo);
    if(pAttendance) {
        if(pAttendance->mOutTime != 0) return false; // how come? is he entering again on the same day?
        pAttendance->mOutTime = pNow->getEpoch();
        pAttendance->mDuration= pAttendance->mInTime - pAttendance->mDuration;
    } else {
        pAttendance = std::make_shared<Attendance>();
        pAttendance->mMembershipNo = pMembershipNo;
        pAttendance->mInTime = pNow->getEpoch();
        pAttendance->mInDateString = pNow->getDateStr();        
    }

    SQLite::Transaction transaction(*mDB);
    ss.str(""); ss << "INSERT INTO attendance (membership_no, in_time, out_time, in_date_string) VALUES ("
                    << pAttendance->mMembershipNo << ", " << pAttendance->mInTime << ", " << pAttendance->mOutTime << ", \""
                    << pAttendance->mInDateString << "\");";
    mDB->exec(ss.str());
    transaction.commit();
    return true;
}

User::Ptr User::parseUser(std::string pUserJson) {
    if(pUserJson.empty()) return nullptr;

    User::Ptr pUser = std::make_shared<User>();
    auto jsRoot = json::parse(pUserJson);
    
    pUser->mId              = jsRoot.value<int32_t>("id", 0);
    pUser->mAddress         = jsRoot.value<std::string>("address", "");    
    pUser->mDOB             = jsRoot.value<time_t>("dob", 0);
    pUser->mEmail           = jsRoot.value<std::string>("email", "");
    pUser->mLastVisit       = jsRoot.value<time_t>("last_visit", 0);
    pUser->mMembershipNo    = jsRoot.value<int32_t>("membership_no", 0);
    pUser->mMobile          = jsRoot.value<std::string>("mobile", "");
    pUser->mName            = jsRoot.value<std::string>("name", "");
    pUser->mPhoto           = jsRoot.value<std::string>("photo", "");
    pUser->mValidityEnd     = jsRoot.value<time_t>("validity_end", 0);
    return pUser;
}

User::Ptr User::parseUser(SQLite::Statement *pQuery) {
    if(!pQuery) return nullptr;
    User::Ptr pUser         = std::make_shared<User>();
    pUser->mId              = pQuery->getColumn("id").getInt();
    pUser->mAddress         = pQuery->getColumn("address").getString();
    pUser->mDOB             = pQuery->getColumn("dob").getInt64();
    pUser->mEmail           = pQuery->getColumn("email").getString();
    pUser->mLastVisit       = pQuery->getColumn("last_visit").getInt64();
    pUser->mMembershipNo    = pQuery->getColumn("membership_no").getInt();
    pUser->mMobile          = pQuery->getColumn("mobile").getString();
    pUser->mName            = pQuery->getColumn("name").getString();
    pUser->mPhoto           = pQuery->getColumn("photo").getString();
    pUser->mValidityEnd     = pQuery->getColumn("validity_end").getInt64();
    return pUser;
}


std::string User::toJson() {
    json jsUser;
    jsUser["id"]            = mId;
    jsUser["address"]       = mAddress;
    jsUser["dob"]           = std::make_shared<MyDateTime>(mDOB)->getDateStr();
    jsUser["email"]         = mEmail;
    jsUser["last_visit"]    = std::make_shared<MyDateTime>(mLastVisit)->getDateStr();
    jsUser["membership_no"] = mMembershipNo;
    jsUser["mobile"]        = mMobile;
    jsUser["name"]          = mName;
    jsUser["photo"]         = mPhoto;
    jsUser["validity_endy"] = std::make_shared<MyDateTime>(mValidityEnd)->getDateStr();
    return jsUser.dump();
}

Attendance::Ptr Attendance::parseAttendance(std::string pAttendanceJson) {
    if(pAttendanceJson.empty()) return nullptr;
    Attendance::Ptr pAttendance = std::make_shared<Attendance>();

    auto jsRoot = json::parse(pAttendanceJson);
    pAttendance->mId            = jsRoot.value<int32_t>("id", 0);
    pAttendance->mMembershipNo  = jsRoot.value<int32_t>("membership_no", 0);
    pAttendance->mInTime        = jsRoot.value<time_t>("in_time", 0);
    pAttendance->mOutTime       = jsRoot.value<time_t>("out_time", 0);
    pAttendance->mInDateString  = jsRoot.value<std::string>("in_date_string", "");
    pAttendance->mDuration      = jsRoot.value<time_t>("duration", 0);
    return pAttendance;
}

Attendance::Ptr Attendance::parseAttendance(SQLite::Statement *pQuery) {
    if(!pQuery) return nullptr;
    Attendance::Ptr pAttendance = std::make_shared<Attendance>();
    pAttendance->mId            = pQuery->getColumn("id").getInt();
    pAttendance->mMembershipNo  = pQuery->getColumn("membership_no").getInt();
    pAttendance->mInTime        = pQuery->getColumn("in_time").getInt64();
    pAttendance->mOutTime       = pQuery->getColumn("out_time").getInt64();
    pAttendance->mInDateString  = pQuery->getColumn("in_date_string").getString();
    pAttendance->mDuration      = pQuery->getColumn("duration").getInt64();
    return pAttendance;
}

std::string Attendance::toJson() {
    json jsUser;
    jsUser["id"]            = mId;
    jsUser["membership_no"] = mMembershipNo;
    jsUser["in_time"]       = mInTime;
    jsUser["out_time"]      = mOutTime;
    jsUser["in_date_string"]= mInDateString;
    jsUser["duration"]      = mDuration;
    return jsUser.dump();
}
Candidate::Ptr Candidate::parseCandidate(SQLite::Statement *pQuery) {
    if(!pQuery) return nullptr;

    Candidate::Ptr pCandidate   = std::make_shared<Candidate>();
    uint32_t iColCount  = pQuery->getColumnCount();
    for(uint32_t iLoop = 0; iLoop < iColCount; iLoop++) {
        std::string colName    = pQuery->getColumnName(iLoop);
        std::cout << colName << ", value : ";
             if(colName == "id"){            pCandidate->mId             = pQuery->getColumn("id").getUInt(); std::cout << pQuery->getColumn("id").getUInt() << std::endl;}
        else if(colName == "jd") {           pCandidate->mJd             = pQuery->getColumn("jd").getUInt(); std::cout << pCandidate->mJd << std::endl;}
        else if(colName == "status") {        pCandidate->mStatus         = pQuery->getColumn("status").getUInt(); std::cout << pCandidate->mStatus << std::endl;}
        else if(colName == "name") {          pCandidate->mName           = pQuery->getColumn("name").getString(); std::cout << pCandidate->mName << std::endl;}
        else if(colName == "mobile") {        pCandidate->mMobile         = pQuery->getColumn("mobile").getInt64(); std::cout << pCandidate->mMobile << std::endl;}
        else if(colName == "vendor") {        pCandidate->mVendor         = pQuery->getColumn("vendor").getString(); std::cout << pCandidate->mVendor << std::endl;}
        else if(colName == "resume_file") {   pCandidate->mResume         = pQuery->getColumn("resume_file").getString(); std::cout << pCandidate->mResume << std::endl;}
        else if(colName == "jd_description") {pCandidate->mJdDescription  = pQuery->getColumn("jd_description").getString(); std::cout << pQuery->getColumn("jd_description").getString() << std::endl;}
        else if(colName == "referred_on") {   pCandidate->mReferredOn     = pQuery->getColumn("referred_on").getInt64(); std::cout << pCandidate->mReferredOn << std::endl;}
        else if(colName == "telephonic_on") { pCandidate->mTelephonicOn   = pQuery->getColumn("telephonic_on").getInt64(); std::cout << pCandidate->mTelephonicOn << std::endl;}
        else if(colName == "first_round_on") {pCandidate->mFirstRoundOn   = pQuery->getColumn("first_round_on").getInt64(); std::cout << pCandidate->mFirstRoundOn << std::endl;}
        else if(colName == "second_round_on") {pCandidate->mSecondRoundOn = pQuery->getColumn("second_round_on").getInt64(); std::cout << pCandidate->mSecondRoundOn << std::endl;}
        else if(colName == "lwd") {           pCandidate->mLWD            = pQuery->getColumn("lwd").getInt64(); std::cout << pCandidate->mLWD << std::endl;}
        else { pCandidate->mCount = pQuery->getColumn(colName.c_str()).getUInt(); std::cout << " did not match any column - " << pQuery->getColumn(colName.c_str()).getUInt() << std::endl; }
    }
    return pCandidate;
}

*/