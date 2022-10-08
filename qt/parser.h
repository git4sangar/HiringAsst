//sgn
#ifndef PARSER_H
#define PARSER_H

#include <QVector>
#include <memory>
#include <regex>

#include "candidate.h"

class Parser
{
public:
    typedef std::shared_ptr<Parser> Ptr;
    Parser();
    virtual ~Parser();

    QVector<Candidate::Ptr> parseQueryResults(const QString& pJsonQueryResults);
    QStringList lineToTokens(const QString& pLine, char charDelimiter = ',');
    QString parseVendorInput(const QString& pInputText);
};

#endif // PARSER_H
