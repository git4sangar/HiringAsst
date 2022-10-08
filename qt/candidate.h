//sgn
#ifndef CANDIDATE_H
#define CANDIDATE_H

#include <QDate>
#include <memory>
#include <QVector>
#include <QTableWidgetItem>

#include "nlohmann_json.hpp"

using json = nlohmann::json;

class Candidate
{
public:
    typedef std::shared_ptr<Candidate> Ptr;
    enum class Status {
        none                        = 0,
        new_resume                  = 1,

        resume_rejected             = 20,
        resume_shortlisted          = 21,

        telephonic_rejected         = 30,
        telephonic_shortlisted      = 31,

        first_round_rejected        = 40,
        first_round_shortlisted     = 41,

        second_round_rejected       = 50,
        second_round_shortlisted    = 51,

        candidate_dropped           = 13,
        candidate_on_hold           = 12
    };

    Candidate();
    virtual ~Candidate();

    QVector<QTableWidgetItem*>  getAllFieldsAsWidgetItems();
    QStringList                 getColumnHeaders();
    int32_t                     convertStatusToIndex();
    int32_t                     convertIndexToStatus(int32_t pIndex);
    int32_t                     getId() { return mId; }

    QString                     getName() { return mName; }
    static QString              getResumeStatus(Status pStatusCode);
    QString                     getNextCall() { return mNextCall; }
    QString                     getLWD() { return mLWD; }
    QString                     getDOJ() { return mDOJ; }
    QString                     getRemarks() { return mRemarks; }
    json                        toJson();

    static Candidate::Ptr       fromJson(const json& pJsonCandidate);
    static Candidate::Ptr       fromStringList(const QStringList& tokens);
    static void                 populateMandatoryFields(Candidate::Ptr pCandidate);
    static QVector<QString>     getAllStatusCodes();
    static QStringList          getStatusStringsForQuery();
    static QString              getQueryChunkForStatus(QString pStrStatus);

private:
    static constexpr unsigned int  POS_JD_NO		= (0);
    static constexpr unsigned int  POS_NAME         = (1);
    static constexpr unsigned int  POS_MOBILE		= (2);
    static constexpr unsigned int  POS_VENDOR		= (3);
    static constexpr unsigned int  POS_LWD			= (4);
    static constexpr unsigned int  POS_RESUME		= (5);
    static constexpr unsigned int  POS_EXPERIENCE	= (6);
    static constexpr unsigned int  POS_STATUS		= (7);
    static constexpr unsigned int  POS_REMARKS		= (8);
    static constexpr unsigned int  MAX_ARGS         = (9);

    static constexpr Status allStatusCodes[] = {                   Status::new_resume,
                                                Status::resume_rejected,        Status::resume_shortlisted,
                                                Status::telephonic_rejected,    Status::telephonic_shortlisted,
                                                Status::first_round_rejected,   Status::first_round_shortlisted,
                                                Status::second_round_rejected,  Status::second_round_shortlisted,
                                                Status::candidate_dropped,      Status::candidate_on_hold
                                               };
    uint32_t    mId, mJd;
    int32_t     mCount;
    double      mExperience;
    QString     mName, mVendor, mResumeName, mMobile, mRemarks;
    QString     mDateReferredOn, mLWD, mLastCall, mNextCall, mDOJ;
    Status      mStatus;
};

#endif // CANDIDATE_H
