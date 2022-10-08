//sgn

#include <iostream>
#include <ctime>
#include <memory>

#include "nlohmann/json.hpp"

#define SECS_IN_A_DAY   (24 * 60 * 60)
#define IST_OFFSET      ((5 * 3600) + (30 * 60))

using json = nlohmann::json;

class MyDateTime {
    time_t mDateTime;
    static constexpr uint32_t mSecsInDay = (24 * 60 * 60);

    static std::vector<std::string> splitBy(const std::string& strLine) {
        std::vector<std::string> digits;
        std::string strToken;

        for (auto ch : strLine) {
            if (ch == '/' || ch == '-' || ch == '.') {
                if (strToken.empty()) continue;
                digits.push_back(strToken);
                strToken.clear();
            }
            else strToken += ch;
        }
        if (!strToken.empty()) digits.push_back(strToken);
        return digits;
    }

    static int32_t getInt(const std::string& strNum) {
        std::string strDigitis;
        for(const auto& ch : strNum) {
            if(std::isdigit(ch)) strDigitis += ch;
        }
        return std::stoi(strDigitis);
    }

    static int32_t getMonth(const std::string& month) {
        std::string pMonth = month;
        std::transform(pMonth.begin(), pMonth.end(), pMonth.begin(), ::toupper);
             if(pMonth == "JAN") return 1;
        else if(pMonth == "FEB") return 2;
        else if(pMonth == "MAR") return 3;
        else if(pMonth == "APR") return 4;
        else if(pMonth == "MAY") return 5;
        else if(pMonth == "JUN") return 6;
        else if(pMonth == "JUL") return 7;
        else if(pMonth == "AUG") return 8;
        else if(pMonth == "SEP") return 9;
        else if(pMonth == "OCT") return 10;
        else if(pMonth == "NOV") return 11;
        else if(pMonth == "DEC") return 12;
        return -1;
    }
public:
    typedef std::shared_ptr<MyDateTime> Ptr;

    MyDateTime(time_t pDateTime = 0)
        : mDateTime{ pDateTime } {
        if(mDateTime == 0)
            mDateTime = time(NULL);
    }

    virtual ~MyDateTime() {}

    time_t getEpoch() { return mDateTime; }
    time_t getEpochBackInDays(uint32_t pDays) { return mDateTime - (pDays * SECS_IN_A_DAY); }

    std::string getDateStr() {
        char buffer[64];
        auto tmDate  = mDateTime;
        strftime(buffer, 64, "%d-%m-%Y", localtime(&tmDate));
        return std::string(buffer);
    }

    std::string getTimeStr() {
        char buffer[64];
        auto tmTime = mDateTime;
        strftime(buffer, 64, "%H:%M:%S", localtime(&tmTime));
        return std::string(buffer);
    }

    static Ptr createEOD(int yyyy, int mon, int day) {
        return create(yyyy, mon, day, 23, 59, 59);
    }

    static Ptr createSOD(int yyyy, int mon, int day) {
        return create(yyyy, mon, day, 9, 0, 0);
    }

    static Ptr create(const json& pJsonDateStr, const std::string& pField, const std::string& pFormat) {
        if(pField.empty() || pFormat.empty()) return nullptr;
        std::string strDate    = pJsonDateStr.value<std::string>(pField, "");
        return create(strDate, pFormat);
    }

    static Ptr create(const std::string& pDate, const std::string& pFormat) {
        if(pDate.empty() || pFormat.empty()) return nullptr;

        const auto& fmtDate = splitBy(pFormat);
        const auto& ddmmyyyy= splitBy(pDate);
        if(fmtDate.size() != ddmmyyyy.size()) return nullptr;

        int32_t day = 0, month = 0, year = 0;
        for(uint32_t iLoop = 0; iLoop < fmtDate.size(); iLoop++) {
                 if(fmtDate[iLoop] == "dd")     day      = std::stoi(ddmmyyyy[iLoop]);
            else if(fmtDate[iLoop] == "yyyy")   year     = std::stoi(ddmmyyyy[iLoop]);
            else if(fmtDate[iLoop] == "MM" && -1 == (month = getMonth(ddmmyyyy[iLoop]))) {
                month    = std::stoi(ddmmyyyy[iLoop]);
            }
        }
        return createSOD(year, month, day);
    }

    static Ptr checkFormatAndCreate(const std::string& pDateStr, time_t pRefDate) {
        if(pDateStr.empty()) return std::make_shared<MyDateTime>(pRefDate);
        const auto& ddmmyyyy = splitBy(pDateStr);

        if(ddmmyyyy.size() == 3) return create(pDateStr, "dd-MM-yyyy");
        else {
            int32_t days    = getInt(pDateStr);
            time_t newDay   = pRefDate + (mSecsInDay * days);
            return std::make_shared<MyDateTime>(newDay);
        }
        return nullptr;
    }

    static Ptr create(int yyyy, int mon, int day, int hh = 0, int mm = 0, int ss = 0) {
        struct tm epochTime;
    
        epochTime.tm_hour   = hh;
        epochTime.tm_min    = mm;
        epochTime.tm_sec    = ss;

        if(yyyy < 100)  yyyy += 2000;
        epochTime.tm_year   = yyyy - 1900;
        epochTime.tm_mon    = mon - 1;
        epochTime.tm_mday   = day;
        epochTime.tm_isdst  = 0;

        time_t pNewTime = mktime(&epochTime);
        return std::make_shared<MyDateTime>(pNewTime);
    }
};