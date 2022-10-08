#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include "parser.h"
#include "networkmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_tblWdgtReport_cellClicked(int row, int column);
    void onQueryResponse(QNetworkReply *pReply);
    void onHelpResponse(QNetworkReply *pReply);
    void onInsertDBResponse(QNetworkReply *pReply);
    void onUpdateDBResponse(QNetworkReply *pReply);
    void onPingResponse(QNetworkReply *pReply);

    void on_btnQueryDB_clicked();
    void clearStatus();

    void on_btnCleanTextBox_clicked();
    void on_txtEdtInput_selectionChanged();
    void on_btnHelp_clicked();
    void on_btnSubmit_clicked();
    void on_lnEdtJd_textChanged(const QString &arg1);
    void on_cmbBxVendor_currentIndexChanged(int index);
    void on_cmbBxQueryStatus_currentIndexChanged(int index);

    void on_btnCleanTable_clicked();
    void on_btnExit_clicked();
    void on_btnUpdate_clicked();

private:
    void updateStatus(QString pStatus, int delay = 3000);
    void loadTableWidget(const QVector<Candidate::Ptr>& pCandidates);
    QString getJdVendorStatus();

    Ui::MainWindow          *ui;
    QNetworkAccessManager   *mpHttpMgr;
    QTimer                  *mpStatusTimer;
    NetworkManager::Ptr     mpNetworkMgr;
    Parser::Ptr             mpParser;
    int32_t                 mCurrentRow, mCurrentCol;
    QVector<Candidate::Ptr> mpCandidates;

};
#endif // MAINWINDOW_H
