//sgn
#include <QCoreApplication>
#include <QSqlQueryModel>
#include <QVector>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

//#include "jsonmodel.h"
#include "candidate.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    mpStatusTimer   = new QTimer();
    connect(mpStatusTimer, SIGNAL(timeout()), this, SLOT(clearStatus()));
    mpHttpMgr       = new QNetworkAccessManager();

    mpParser        = std::make_shared<Parser>();
    mpNetworkMgr    = std::make_shared<NetworkManager>(mpHttpMgr);

    auto allCodes   = Candidate::getAllStatusCodes();
    for(auto itr = allCodes.begin(); itr != allCodes.end(); itr++) {
        ui->cmbBxStatus->addItem(*itr);
    }

    auto allStatus  = Candidate::getStatusStringsForQuery();
    for(const auto& strStatus : allStatus)
        ui->cmbBxQueryStatus->addItem(strStatus);
    ui->cmbBxQueryStatus->setCurrentIndex(0);

    ui->cmbBxVendor->addItem("All");
    ui->cmbBxVendor->addItem("ALTEN");
    ui->cmbBxVendor->addItem("SATVEN");
    ui->cmbBxVendor->addItem("SACHA");
    ui->cmbBxVendor->addItem("ONWARD");
    ui->cmbBxVendor->addItem("HCL");
    ui->cmbBxVendor->addItem("KPIT");
    ui->cmbBxVendor->addItem("RLE");
    ui->cmbBxVendor->addItem("DEP");
    ui->cmbBxVendor->setCurrentIndex(0);

    ui->lblStatus->clear();
    ui->lnEdtQuery->setText("SELECT * FROM Candidates;");

    mCurrentRow = 0;
    mCurrentCol = 0;

    connect(mpHttpMgr,&QNetworkAccessManager::finished, this, &MainWindow::onPingResponse);
    mpNetworkMgr->pingServer();
}

MainWindow::~MainWindow() {
    delete ui;
    delete mpHttpMgr;
    delete mpStatusTimer;
}




// ----------------------------------------------------------
//                   Update Table Widget
// ----------------------------------------------------------

void MainWindow::on_tblWdgtReport_cellClicked(int row, int column) {
    mCurrentRow = row;
    mCurrentCol = column;

    if(mpCandidates.size() > row) {
        ui->lnEdtName->setText(mpCandidates[row]->getName());
        auto curIdx = mpCandidates[row]->convertStatusToIndex();
        ui->cmbBxStatus->setCurrentIndex(curIdx);

        auto doj = mpCandidates[row]->getDOJ();
        if(!doj.isEmpty()) ui->dtEdtDOJ->setDate(QDate::fromString(doj, "dd-MM-yyyy"));

        auto lwd = mpCandidates[row]->getLWD();
        if(!lwd.isEmpty()) ui->dtEdtLWD->setDate(QDate::fromString(lwd, "dd-MM-yyyy"));

        auto nextCall   = mpCandidates[row]->getNextCall();
        if(!nextCall.isEmpty()) ui->dtEdtNextCall->setDate(QDate::fromString(nextCall, "dd-MM-yyyy"));

        ui->txtEdtRemarks->setText(mpCandidates[row]->getRemarks());
    }
}

void MainWindow::loadTableWidget(const QVector<Candidate::Ptr>& pCandidates) {
    if(pCandidates.empty()) return;
    mpCandidates= pCandidates;
    mCurrentRow = 0;
    mCurrentCol = 0;

    QStringList colHeaders = pCandidates[0]->getColumnHeaders();
    ui->tblWdgtReport->setRowCount(pCandidates.size());
    ui->tblWdgtReport->setColumnCount(colHeaders.size());

    //  make sure row count and column count are set before setting headers
    ui->tblWdgtReport->setHorizontalHeaderLabels(colHeaders);

    uint32_t iRow = 0;
    for(const auto& pCandidate : pCandidates) {
        uint32_t iCol = 0;
        const auto& pItemWidgets = pCandidate->getAllFieldsAsWidgetItems();
        for(auto pItemWidget : pItemWidgets) {
            ui->tblWdgtReport->setItem(iRow, iCol++, pItemWidget);
        }
        iRow++;
    }
}





// ----------------------------------------------------------
//                   Handle UI Changes
// ----------------------------------------------------------

void MainWindow::on_txtEdtInput_selectionChanged() {
    ui->lnEdtQuery->setText(ui->txtEdtInput->textCursor().selectedText());
}

QString MainWindow::getJdVendorStatus() {
    QString strJd       = ui->lnEdtJd->text();
    QString strVendor   = ui->cmbBxVendor->itemText(ui->cmbBxVendor->currentIndex());
    QString strStatus   = ui->cmbBxQueryStatus->itemText(ui->cmbBxQueryStatus->currentIndex());

    QString appendSelections;
    if(strJd.length() > 0)  appendSelections    = appendSelections + " jd = " + strJd;

    if(strVendor != "All")  {
        if(!appendSelections.isEmpty()) appendSelections = appendSelections + " AND";
        appendSelections    = appendSelections + " vendor = \"" + strVendor + "\"";
    }

    if(strStatus != "All")  {
        if(!appendSelections.isEmpty()) appendSelections = appendSelections + " AND";
        appendSelections    = appendSelections + Candidate::getQueryChunkForStatus(strStatus);
    }

    if(!appendSelections.isEmpty()) appendSelections = "SELECT * FROM Candidates WHERE" + appendSelections + ";";
    return appendSelections;
}

void MainWindow::on_lnEdtJd_textChanged(const QString &arg1) {
    auto strQuery   = getJdVendorStatus();
    if(strQuery.isEmpty()) ui->lnEdtQuery->setText("SELECT * FROM Candidates;");
    else ui->lnEdtQuery->setText(strQuery);
}

void MainWindow::on_cmbBxVendor_currentIndexChanged(int index) {
    auto strQuery   = getJdVendorStatus();
    if(strQuery.isEmpty()) ui->lnEdtQuery->setText("SELECT * FROM Candidates;");
    else ui->lnEdtQuery->setText(strQuery);
}

void MainWindow::on_cmbBxQueryStatus_currentIndexChanged(int index) {
    auto strQuery   = getJdVendorStatus();
    if(strQuery.isEmpty()) ui->lnEdtQuery->setText("SELECT * FROM Candidates;");
    else ui->lnEdtQuery->setText(strQuery);
}






// ----------------------------------------------------------
//                   Network Requests & Responses
// ----------------------------------------------------------

void MainWindow::onPingResponse(QNetworkReply *pReply) {
    const auto& resp= QString(pReply->readAll());
    if(QString(resp) != "pong")
        updateStatus("Pls connect to VPN if not already?", 5000);
    disconnect(mpHttpMgr,&QNetworkAccessManager::finished, this, &MainWindow::onPingResponse);
}

void MainWindow::on_btnQueryDB_clicked() {
    QString strQuery = ui->lnEdtQuery->text();
    if(strQuery.length() == 0) {updateStatus("Enter a query string..."); return;}
    if(strQuery.contains("DELETE")) {updateStatus("You cannot delete..."); return;}

    connect(mpHttpMgr,&QNetworkAccessManager::finished, this, &MainWindow::onQueryResponse);
    mpNetworkMgr->queryRemoteDB(ui->lnEdtQuery->text());
}

void MainWindow::onQueryResponse(QNetworkReply *pReply) {
    ui->tblWdgtReport->clear();
    ui->tblWdgtReport->setRowCount(0);
    ui->tblWdgtReport->setColumnCount(0);

    const auto& resp = QString(pReply->readAll());
    //ui->txtEdtRemarks->setText(resp);
    QVector<Candidate::Ptr> candidates = mpParser->parseQueryResults(resp);
    (candidates.empty()) ? updateStatus("No results for query.") : loadTableWidget(candidates);

    disconnect(mpHttpMgr, &QNetworkAccessManager::finished, this, &MainWindow::onQueryResponse);
}

void MainWindow::on_btnSubmit_clicked() {
    QString jsonCandidates = mpParser->parseVendorInput(ui->txtEdtInput->toPlainText());
    ui->txtEdtRemarks->setText(jsonCandidates);
    connect(mpHttpMgr,&QNetworkAccessManager::finished, this, &MainWindow::onInsertDBResponse);
    mpNetworkMgr->insertToDB(jsonCandidates);
}

void MainWindow::onInsertDBResponse(QNetworkReply *pReply) {
    const auto& resp = QString(pReply->readAll());
    auto root = json::parse(resp.toStdString());
    if(root["isOk"]) {
        int32_t iRowsInserted   = root["rows_inserted"];
        QString strStatus       = QString("Inserted ") + QString::number(iRowsInserted) + QString(" rows");
        updateStatus(strStatus);
    } else {
        updateStatus(QString(root.value<std::string>("error", "").c_str()));
    }
    disconnect(mpHttpMgr,&QNetworkAccessManager::finished, this, &MainWindow::onInsertDBResponse);
}

void MainWindow::on_btnUpdate_clicked() {
    if(mpCandidates.size() < mCurrentRow) return;

    Candidate::Ptr pCandidate   = mpCandidates[mCurrentRow];
    json pRoot;
    pRoot["id"]         = pCandidate->getId();
    pRoot["name"]       = ui->lnEdtName->text().toStdString();
    pRoot["remarks"]    = ui->txtEdtRemarks->toPlainText().toStdString();
    int32_t statusIndex = ui->cmbBxStatus->currentIndex();
    pRoot["status"]     = pCandidate->convertIndexToStatus(statusIndex);
    pRoot["next_call"]  = ui->dtEdtNextCall->date().toString("dd-MM-yyyy").toStdString();
    pRoot["last_call"]  = QDate::currentDate().toString("dd-MM-yyyy").toStdString();
    pRoot["lwd"]        = ui->dtEdtLWD->date().toString("dd-MM-yyyy").toStdString();
    pRoot["doj"]        = ui->dtEdtDOJ->date().toString("dd-MM-yyyy").toStdString();

    connect(mpHttpMgr,&QNetworkAccessManager::finished, this, &MainWindow::onUpdateDBResponse);
    mpNetworkMgr->updateDB(pRoot.dump().c_str());
}

void MainWindow::onUpdateDBResponse(QNetworkReply *pReply) {
    const auto& resp = QString(pReply->readAll());
    auto root = json::parse(resp.toStdString());
    root.value<bool>("isOk", false) ? updateStatus("Updated Successfully"): updateStatus("Failed Updating");
    disconnect(mpHttpMgr,&QNetworkAccessManager::finished, this, &MainWindow::onUpdateDBResponse);
}

void MainWindow::on_btnHelp_clicked() {
    connect(mpHttpMgr,&QNetworkAccessManager::finished, this, &MainWindow::onHelpResponse);
    mpNetworkMgr->getSampleQueries();
}

void MainWindow::onHelpResponse(QNetworkReply *pReply) {
    const auto& resp = QString(pReply->readAll());
    if(resp.length() == 0) { updateStatus("Connected to VPN...?"); return; }
    auto root = json::parse(resp.toStdString());
    if(root["isOk"]) {
        std::string strResp = root["queries"];
        ui->txtEdtInput->setText(QString(strResp.c_str()));
    }
    disconnect(mpHttpMgr, &QNetworkAccessManager::finished, this, &MainWindow::onHelpResponse);
}





// ----------------------------------------------------------
//                   Update and Clear Status
// ----------------------------------------------------------

void MainWindow::updateStatus(QString pStatus, int delay) {
    mpStatusTimer->stop();
    ui->lblStatus->setText(pStatus);
    mpStatusTimer->start(delay);
}

void MainWindow::clearStatus() {
    mpStatusTimer->stop();
    ui->lblStatus->clear();
}





// ----------------------------------------------------------
//                   Clear UI
// ----------------------------------------------------------

void MainWindow::on_btnCleanTable_clicked() {
    ui->tblWdgtReport->clear();
    ui->tblWdgtReport->setRowCount(0);
    ui->tblWdgtReport->setColumnCount(0);
}

void MainWindow::on_btnCleanTextBox_clicked() {
    ui->txtEdtInput->clear();
}




void MainWindow::on_btnExit_clicked() {
    QApplication::quit();
}
