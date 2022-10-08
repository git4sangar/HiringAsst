//sgn

#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <ctime>

#include "nlohmann/json.hpp"
#include "SQLiteCpp/SQLiteCpp.h"

using json = nlohmann::json;

struct Candidate {
    typedef std::shared_ptr<Candidate> Ptr;

    Candidate()
    : mId(0)
    , mJd(0)
    , mStatus(0)
    , mExperience(0.01)
    , mCount(-1)
    , mReferredOn(0)
    , mLastCall(0)
    , mNextCall(0)
    , mDOJ(0)
    , mLWD(0)
    , mMobile(0) {}

    std::string mName, mVendor, mResume, mRemarks;
    uint32_t    mId, mJd, mStatus;
    float       mExperience;
    int32_t     mCount;
    time_t      mReferredOn, mLastCall, mNextCall, mDOJ, mLWD;
    uint64_t    mMobile;

    json toJson();
    std::string getInsertQuery();
    std::string getUpdateQuery();
    std::string getQueryToCheckCandidate() {
        std::stringstream ss;
        ss << "SELECT * FROM Candidates WHERE mobile = " << mMobile << " AND jd = " << mJd << ";";
        return ss.str();
    }
    static Candidate::Ptr fromJson(const json& jsonCandidate);
    static Candidate::Ptr parseCandidate(SQLite::Statement *pQuery);
};

class DBInterface {
    DBInterface(std::string pDBFileName) {
        mDB = std::make_shared<SQLite::Database>(pDBFileName, SQLite::OPEN_READWRITE);
    }

    std::shared_ptr<SQLite::Database> mDB;
    static DBInterface* pThis;

public:
    typedef std::shared_ptr<DBInterface> Ptr;
    static DBInterface *getInstance( std::string pFileName = "")
        { if(!pThis) { pThis = new DBInterface(pFileName); } return pThis; }
    virtual ~DBInterface() {}

    std::vector<Candidate::Ptr> executeQuery(const std::string& pQuery);
    bool executeTransaction(const std::string& pQuery);
};