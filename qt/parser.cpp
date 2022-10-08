//sgn

#include <QDebug>
#include <iostream>
#include "nlohmann_json.hpp"
#include "candidate.h"
#include "parser.h"

using json = nlohmann::json;

Parser::Parser() {}
Parser::~Parser() {}

QVector<Candidate::Ptr> Parser::parseQueryResults(const QString& pJsonQueryResults) {
    QVector<Candidate::Ptr> candidates;

    try {
        auto jsonResp = json::parse(pJsonQueryResults.toStdString());
        bool isSuccess  = jsonResp.value<bool>("isOk", false);
        if(isSuccess) {
            for(const auto& jsonCandidate : jsonResp["candidates"]) {
                Candidate::Ptr pCandidate = Candidate::fromJson(jsonCandidate);
                candidates.push_back(pCandidate);
            }
        }
    } catch(json::parse_error& e) {}
    return candidates;
}

QStringList Parser::lineToTokens(const QString& pLine, char charDelimiter) {
    QStringList tokens;
    QString token;

    for(const auto& ch : pLine) {
        if(charDelimiter == ch) {
            if(token.isEmpty()) continue;
            tokens << token.trimmed();
            token.clear();
        } else token += ch;
    }
    if (!token.isEmpty()) tokens << token.trimmed();
    return tokens;
}

QString Parser::parseVendorInput(const QString& pInputText) {
    auto candidates = json::array();
    auto inputLines = pInputText.split(QRegExp("[\r\n]"));
    for(const auto& line : inputLines) {
        const auto& tokens          = lineToTokens(line);
        Candidate::Ptr pCandidate   = Candidate::fromStringList(tokens);
        candidates.push_back(pCandidate->toJson());
    }
    return QString(candidates.dump().c_str());
}
