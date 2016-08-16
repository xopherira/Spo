#ifndef SETUPTAB_H
#define SETUPTAB_H
#include <QWidget>
#include <QXmlStreamReader>
#include <QFile>
#include "ui_setuptab.h"
#include <View/addlineitem.h>

namespace Ui {
    class SetupTab;
}

class SetupTab : public QWidget, public Ui::SetupTab
{
    Q_OBJECT

public:

    explicit SetupTab(QWidget *parent = 0);

    ~SetupTab();
void AddLineItemFromDialog(QString &item);
bool PartOkToAdd(QString partName, QString partDesc, QString partCost, QString partQty);
void handleDeleteSelectedRow();
void updateExtendedCostTotal();
void OverLimitDialog();
    void GetAddressXML();
    void XmlAddressSearchRequestParsing(QXmlStreamReader &XmlFileReader);
    QString GetDefaultXML();
    void ShowXmlOnScreen();
public slots:
    void SaveAddressXML();
private slots:
    //void on_btnAddShipToAddress_clicked();
    void on_leShipToDealer_ID_editingFinished();
    void on_btnAddLineItem_GetMacPacPart_clicked();
    void on_btnAddLineItem_AddLine_clicked();
    void on_btnRecapAndSubmit_Clear_clicked();

    void on_btnSubmitOrder_clicked();
private:
    QString m_basefilename;
    QString basefilename;

    //QString ReadBaseXMLFile();
    QString ReadBaseXMLFromInternalResource();
    QString ReadBaseXMLFile();
    void XmlSpoParsing(QXmlStreamReader &XmlFileReader);
    bool overCostLimit;
    Ui::SetupTab *ui;
    bool AddLineItem_OrderTotal(QString orderTotal);
    void resetFields();
    bool GetOrderTotal();
    void WriteXml();
    void resetForm();
};

#endif // SETUPTAB_H
