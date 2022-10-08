//sgn
#include <iostream>
#include "nlohmann_json.hpp"
#include "candidate.h"

using json = nlohmann::json;

Candidate::Candidate()
    : mId(0)
    , mJd(0)
    , mCount(-1)
    , mExperience(0.01)
    , mRemarks("-")
    , mStatus(Status::new_resume)
{}

Candidate::~Candidate() {}

int32_t Candidate::convertStatusToIndex() {
    for(int32_t idx = 0; idx < sizeof(allStatusCodes)/sizeof(allStatusCodes[0]); idx++)
        if(mStatus == allStatusCodes[idx]) return idx;
    return 0;
}

int32_t Candidate::convertIndexToStatus(int32_t pIndex) {
    if((sizeof(allStatusCodes)/sizeof(allStatusCodes[0])) > pIndex)
        return static_cast<int32_t>(allStatusCodes[pIndex]);
    return -1;
}

Candidate::Ptr Candidate::fromJson(const json& pJsonCandidate) {
    if(pJsonCandidate.size() == 0) return nullptr;

    Candidate::Ptr pCandidate = std::make_shared<Candidate>();

    if(pJsonCandidate.contains("id"))               pCandidate->mId             = pJsonCandidate.value<int32_t>("id", 0);
    if(pJsonCandidate.contains("jd"))               pCandidate->mJd             = pJsonCandidate.value<int32_t>("jd", 0);
    if(pJsonCandidate.contains("experience"))       pCandidate->mExperience     = pJsonCandidate.value<double>("experience", 0.01);
    if(pJsonCandidate.contains("name"))             pCandidate->mName           = pJsonCandidate.value<std::string>("name", "").c_str();
    if(pJsonCandidate.contains("mobile"))           pCandidate->mMobile         = pJsonCandidate.value<std::string>("mobile", "").c_str();
    if(pJsonCandidate.contains("status"))           pCandidate->mStatus         = static_cast<Candidate::Status>(pJsonCandidate["status"]);
    if(pJsonCandidate.contains("vendor"))           pCandidate->mVendor         = pJsonCandidate.value<std::string>("vendor", "").c_str();
    if(pJsonCandidate.contains("resume"))           pCandidate->mResumeName     = pJsonCandidate.value<std::string>("resume", "").c_str();
    if(pJsonCandidate.contains("remarks"))          pCandidate->mRemarks        = pJsonCandidate.value<std::string>("remarks", "").c_str();
    if(pJsonCandidate.contains("referred_on"))      pCandidate->mDateReferredOn = pJsonCandidate.value<std::string>("referred_on", "").c_str();
    if(pJsonCandidate.contains("last_call"))        pCandidate->mLastCall       = pJsonCandidate.value<std::string>("last_call", "").c_str();
    if(pJsonCandidate.contains("next_call"))        pCandidate->mNextCall       = pJsonCandidate.value<std::string>("next_call", "").c_str();
    if(pJsonCandidate.contains("doj"))              pCandidate->mDOJ            = pJsonCandidate.value<std::string>("doj", "").c_str();
    if(pJsonCandidate.contains("lwd"))              pCandidate->mLWD            = pJsonCandidate.value<std::string>("lwd", "").c_str();
    if(pJsonCandidate.contains("count"))            pCandidate->mCount          = pJsonCandidate.value<int32_t>("count", -1);

    return pCandidate;
}

json Candidate::toJson() {
    json jsCandidate;

    jsCandidate["id"]       = mId;
    jsCandidate["jd"]       = mJd;
    jsCandidate["status"]   = mStatus;
    jsCandidate["name"]     = mName.toStdString();
    jsCandidate["vendor"]   = mVendor.toStdString();
    jsCandidate["resume"]   = mResumeName.toStdString();
    jsCandidate["remarks"]  = mRemarks.toStdString();
    jsCandidate["experience"]   = mExperience;
    jsCandidate["mobile"]       = mMobile.toStdString();
    jsCandidate["referred_on"]  = mDateReferredOn.toStdString();
    jsCandidate["last_call"]    = mLastCall.toStdString();
    jsCandidate["next_call"]    = mNextCall.toStdString();
    jsCandidate["doj"]          = mDOJ.toStdString();
    jsCandidate["lwd"]          = mLWD.toStdString();
    jsCandidate["count"]        = mCount;
    return jsCandidate;
}

//  Check the order in which getAllFieldsAsWidgetItems returns
QStringList Candidate::getColumnHeaders() {
    QStringList headerList;

    if(mJd)                         headerList << "JD";
    if(!mDateReferredOn.isEmpty())  headerList << "Reffered On";
    if(!mName.isEmpty())            headerList << "Name";
    if(mExperience > 0.1)           headerList << "Experience";
    if(!mMobile.isEmpty())          headerList << "Mobile";
    if(!mVendor.isEmpty())          headerList << "Vendor";
    if(!mLastCall.isEmpty())        headerList << "Last Call";
    if(mStatus != Status::none)     headerList << "Status";
    if(!mNextCall.isEmpty())        headerList << "Next Call";
    if(!mLWD.isEmpty())             headerList << "LWD";
    if(!mDOJ.isEmpty())             headerList << "DOJ";
    if(!mRemarks.isEmpty())         headerList << "Remarks";
    if(mCount >= 0)                 headerList << "Count";

    return headerList;
}

//  Check the order in which getColumnHeaders returns
QVector<QTableWidgetItem*> Candidate::getAllFieldsAsWidgetItems() {
    QVector<QTableWidgetItem*> allFields;
    QTableWidgetItem* pItem;

    if(mJd)                         { pItem = new QTableWidgetItem(); pItem->setText(QString::number(mJd)); allFields.push_back(pItem); }
    if(!mDateReferredOn.isEmpty())  { pItem = new QTableWidgetItem(); pItem->setText(mDateReferredOn); allFields.push_back(pItem); }
    if(!mName.isEmpty())            { pItem = new QTableWidgetItem(); pItem->setText(mName); allFields.push_back(pItem); }
    if(mExperience > 0.1)           { pItem = new QTableWidgetItem(); pItem->setText(QString::number(mExperience)); allFields.push_back(pItem); }
    if(!mMobile.isEmpty())          { pItem = new QTableWidgetItem(); pItem->setText(mMobile); allFields.push_back(pItem); }
    if(!mVendor.isEmpty())          { pItem = new QTableWidgetItem(); pItem->setText(mVendor); allFields.push_back(pItem); }
    if(!mLastCall.isEmpty())        { pItem = new QTableWidgetItem(); pItem->setText(mLastCall); allFields.push_back(pItem);}
    if(mStatus != Status::none)     { pItem = new QTableWidgetItem(); pItem->setText(getResumeStatus(mStatus)); allFields.push_back(pItem); }
    if(!mNextCall.isEmpty())        { pItem = new QTableWidgetItem(); pItem->setText(mNextCall); allFields.push_back(pItem); }
    if(!mLWD.isEmpty())             { pItem = new QTableWidgetItem(); pItem->setText(mLWD); allFields.push_back(pItem); }
    if(!mDOJ.isEmpty())             { pItem = new QTableWidgetItem(); pItem->setText(mDOJ); allFields.push_back(pItem); }
    if(!mRemarks.isEmpty())         { pItem = new QTableWidgetItem(); pItem->setText(mRemarks); allFields.push_back(pItem); }
    if(mCount >= 0)                 { pItem = new QTableWidgetItem(); pItem->setText(QString::number(mCount)); allFields.push_back(pItem); }
    return allFields;
}

Candidate::Ptr Candidate::fromStringList(const QStringList& tokens) {
    Candidate::Ptr pCandidate = std::make_shared<Candidate>();
    for(int32_t iLoop = 0; iLoop < tokens.size(); iLoop ++) {
        const auto& token = tokens[iLoop];
             if(iLoop == POS_JD_NO)     pCandidate->mJd         = token.toUInt();
        else if(iLoop == POS_NAME)      pCandidate->mName       = token;
        else if(iLoop == POS_MOBILE)    pCandidate->mMobile     = token;
        else if(iLoop == POS_VENDOR)    pCandidate->mVendor     = token;
        else if(iLoop == POS_LWD)       pCandidate->mLWD        = token;
        else if(iLoop == POS_RESUME)    pCandidate->mResumeName = token;
        else if(iLoop == POS_EXPERIENCE)pCandidate->mExperience = token.toFloat();
    }
    populateMandatoryFields(pCandidate);
    return pCandidate;
}

void Candidate::populateMandatoryFields(Candidate::Ptr pCandidate) {
    pCandidate->mDateReferredOn = QDate::currentDate().toString("dd-MM-yyyy");
    pCandidate->mLastCall       = pCandidate->mNextCall = pCandidate->mDOJ = pCandidate->mDateReferredOn;
    pCandidate->mStatus         = Status::new_resume;
    pCandidate->mRemarks        = "-";
}

//  getStatusStringsForQuery and getQueryChunkForStatus are related, mindful about the status texts
QStringList Candidate::getStatusStringsForQuery() {
    QStringList statusStrings;
    statusStrings << "All" << "In Progress" << "On Hold" << "Selected" << "Rejected" << "Dropped";
    return statusStrings;
}

//  getQueryChunkForStatus and getStatusStringsForQuery are related, mindful about the status texts
QString Candidate::getQueryChunkForStatus(QString pStrStatus) {
    QString strQueryChunk;
         if(pStrStatus == "In Progress") strQueryChunk = " (status % 10) = 1";
    else if(pStrStatus == "On Hold")     strQueryChunk = " (status % 10) = 2";
    else if(pStrStatus == "Selected")    strQueryChunk = " status = 51";
    else if(pStrStatus == "Rejected")    strQueryChunk = " (status % 10) = 0";
    else if(pStrStatus == "Dropped")     strQueryChunk = " (status % 10) = 3";
    return strQueryChunk;
}

QString Candidate::getResumeStatus(Status pStatusCode) {
    switch(pStatusCode) {
    case Status::none:
    case Status::new_resume:
        return "Fresh";

    case Status::resume_rejected:
        return "Resume Rejected";
    case Status::resume_shortlisted:
        return "Resume Shortlisted";

    case Status::telephonic_rejected:
        return "Telephonic Rejected";
    case Status::telephonic_shortlisted:
        return "Telephonic Shortlisted";

    case Status::first_round_rejected:
        return "1st Round Rejected";
    case Status::first_round_shortlisted:
        return "1st Round Shortlisted";

    case Status::second_round_rejected:
        return "2nd Round Rejected";
    case Status::second_round_shortlisted:
        return "2nd Round Shorlisted";

    case Status::candidate_dropped:
        return "Dropped";
    case Status::candidate_on_hold:
        return "On Hold";
    }
    return QString();
}

QVector<QString> Candidate::getAllStatusCodes() {
    QVector<QString> allStatusString;
    for(auto const status : allStatusCodes) {
        allStatusString.push_back(getResumeStatus(status));
    }
    return allStatusString;
}
