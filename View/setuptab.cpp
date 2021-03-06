#include "setuptab.h"
#include "ui_setuptab.h"
#include "utils.h"
#include "View/mainview.h"
#include "ui_mainview.h"
#include "View/setuptab.h"
#include "View/searchdialog.h"
#include "View/missingdatadialog.h"
#include "View/overcostlimitdialog.h"
#include "View/outofstock.h"
#include <string>
#include <sstream>
#include <QDir>
#include <QFile>
#include <QTableWidget>
#include <QString>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QtXml>
#include <QFileDialog>
#include <iostream>
#include <QMessageBox>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QDataStream>
#include <QTextStream>
#include <QPushButton>
#include <QIcon>
#include <QtNetwork>
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <Iads.h>
//#pragma comment(lib, "Secur32.lib")
#define SECURITY_WIN32
#include <Security.h>
#include <lmcons.h>
#include <Windows.h>

/* Add a timer, start when session is initiated */
static auto RESOURCE_PREFIX = QStringLiteral(":/xml");
static auto OUTFILE_PREFIX = QStringLiteral("C:/");
static QString FILEERROR_MSG = QStringLiteral("ERROR OPENING FILE");
QString defaultXML;

/************************************************************************
 *
 *  SetupTab(QWidget *parent) : QWidget(parent), ui(new Ui::SetupTab)
 *
 ************************************************************************/
SetupTab::SetupTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SetupTab)
{
    qDebug() << "QWidget SetupTab has been envoked!";
    ui->setupUi(this);
    //Start: PO generation (fjd 8.5.16 1:46PM)
    QString po = "";
    Utils *u;
    u->createPO(po);
    ui->leShipToDealer_PO->setText(po);
    //End: PO generation
    //  Set date, add one day for shipping default.
    QDate shipDate = QDate::currentDate();
    ui->dateEdit->setDate(shipDate.addDays(1));
    //get logged on user name in order to append to @DraperInc.com
    TCHAR Username[UNLEN+1];
    DWORD nULen = UNLEN;
    GetUserName(Username, &nULen);
    QString adUserName = QString::fromWCharArray(Username);
    ui->leSalespersonEmail->setText(adUserName+"@DraperInc.com");
    //  Hidden line edit on form to hold "on hand" inventory value.
    ui->leOnHandStock->setVisible(0);
}
/************************************************************************
 *
 *  ReadBaseXMLFromInternalResource()
 *
 ************************************************************************/
//QString SetupTab::ReadBaseXMLFromInternalResource()
//{
//    QDir res_dir (RESOURCE_PREFIX);
//    if(!res_dir.exists())
//    {
//        //TODO Send Error message
//        return "";
//    }
//    auto path = res_dir.filePath("spoBase.xml");
//    QFile res_file(path);

//    if(!res_file.open(QFile::ReadOnly|QFile::Text))
//    {
//        //TODO Send Error message
//        qDebug() << FILEERROR_MSG;
//    }

//}
/************************************************************************
 *
 *  ShowXmlOnScreen()
 *
 ************************************************************************/
void SetupTab::ShowXmlOnScreen()
{
//    QFile file(basefilename);
//    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
//        return;
//    while (!file.atEnd())
//    {
//        QByteArray line = file.readLine();
//        qDebug() << "inside the print xml loop" << line;
//    }
}

/************************************************************************
 *
 *  GetAddressXML()
 *
 ************************************************************************/
void SetupTab::GetAddressXML()
{
    if (ui->leShipToDealer_ID->text().isEmpty() == false || ui->leShipToDealer_ID->text().contains("END USER") == false)
    {
        //Create custom temporary event loop on stack
        QEventLoop eventLoop;
        QNetworkAccessManager mgr;
        QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
        QString textSearch = ui->leShipToDealer_ID->text();
        //HTTP request
        QNetworkRequest req( QUrl( QString("http://spi-rabbit2:8080/addresses/address/" + textSearch)));
        QNetworkReply *reply = mgr.get(req);
        //Blocking
        eventLoop.exec();
        QByteArray result = reply->readAll();
        QXmlStreamReader xmlReader(result);
        XmlAddressSearchRequestParsing(xmlReader);
    }
}
/************************************************************************
 *
 *  XmlAddressSearchRequestParsing(QXmlStreamReader &)
 *
 ************************************************************************/
void SetupTab::XmlAddressSearchRequestParsing(QXmlStreamReader &XmlFileReader)
{
    QString dealerID, dname, status, address1, address2, address3, city, state, zip, countryCode, region, phone;
    while(!XmlFileReader.atEnd() && !XmlFileReader.hasError())
    {
        QXmlStreamReader::TokenType token = XmlFileReader.readNext();
        if(token == QXmlStreamReader::StartDocument) {
            continue;
        }
        if(token == QXmlStreamReader::StartElement)
        {
            //QStringRef  name = XmlFileReader.name();
            if(XmlFileReader.name() == "DealerID")
            {
                dealerID = XmlFileReader.readElementText();
            }
            else if(XmlFileReader.name() == "Name")
            {
                dname = XmlFileReader.readElementText();
            }
            else if(XmlFileReader.name() == "Status")
            {
                status = XmlFileReader.readElementText();
            }
            else if(XmlFileReader.name() == "Address1")
            {
                address1 = XmlFileReader.readElementText();
            }
            else if(XmlFileReader.name() == "Address2")
            {
                address2 = XmlFileReader.readElementText();
            }
            else if(XmlFileReader.name() == "Address3")
            {
                address3 = XmlFileReader.readElementText();
            }
            else if(XmlFileReader.name() == "City")
            {
                city = XmlFileReader.readElementText();
            }
            else if(XmlFileReader.name() == "State")
            {
                state = XmlFileReader.readElementText();
            }
            else if(XmlFileReader.name() == "Zip")
            {
                zip = XmlFileReader.readElementText();
            }
            else if(XmlFileReader.name() == "CountryCode")
            {
                countryCode = XmlFileReader.readElementText();
            }
            else if(XmlFileReader.name() == "Region")
            {
                region = XmlFileReader.readElementText();
            }
            else if(XmlFileReader.name() == "Phone")
            {
                phone = XmlFileReader.readElementText();
            }

            //leShipToAddress_
            ui->leShipToAddress_Name->setText(dname);           // = dname;
            ui->leShipToAddress_Address1->setText(address1);    //= address1;
            ui->leShipToAddress_Address2->setText(address2);    //= address2;
            ui->leShipToAddress_City->setText(city);            //= city;
            ui->leShipToAddress_State->setText(state);          //= state;
            ui->leShipToAddress_Zip->setText(zip);              //= zip;
            ui->leShipToAddress_Country->setText(countryCode);  //   County line edit field
            ui->leShipToAddress_ContactNumber->setText(phone);  // Contact number returned from macPac

        }
    }
}
/************************************************************************
 *
 *  ~SetupTab()
 *
 ************************************************************************/
SetupTab::~SetupTab()
{
    Utils::DestuctorMsg(this);
    delete ui;
}
/* ========================================================================
 *
 * void on_leShipToDealer_ID_editingFinished()
 *
 * =======================================================================*/
void SetupTab::on_leShipToDealer_ID_editingFinished()
{
    GetAddressXML();
}
/* ========================================================================
 *
 * void AddLineItemFromDialog()
 *
 * =======================================================================*/
void SetupTab::AddLineItemFromDialog(QString & item)
{
    ui->tblOrderLinesWidget->setItem(0,0, new QTableWidgetItem(item));
}
/* ========================================================================
 *
 * void on_btnAddLineItem_GetMacPacPart_clicked()
 *
 * =======================================================================*/
void SetupTab::on_btnAddLineItem_GetMacPacPart_clicked()
{
    SearchDialog *dialog = new SearchDialog;

    if(dialog->exec())
    {
        QMap<QString, QString> m = dialog->getMap();

        QString partName = m.value("PartName");
        ui->leAddLineItem_PartNumber->setText(partName);

        QString partDesc = m.value("PartDesc");
        ui->leAddlineItem_Description->setText(partDesc);

        QString partCost = m.value("PartCost");
        ui->leAddLineItem_Value->setText(partCost);
        //Default the quantity to 1 for users
        ui->spinBoxAddLineItem_Quantity->setValue(1);
        //ui->leAddLineItem_Quantity->setText("1");

        QString partOnHand = m.value("PartOnHand");
        ui->leOnHandStock->setText(partOnHand);

        if(partOnHand < 0)
        {
            //this->ui->tblOrderLinesWidget->removeRow(i);
            OverLimitDialog();
            return;
        }

    }
}
/* ========================================================================
 *
 * void updateExtendedCostTotal()
 *
 * =======================================================================*/
void SetupTab::updateExtendedCostTotal()
{
    int rowCt = ui->tblOrderLinesWidget->rowCount();
    if (rowCt == 0) {
        ui->leOrderTotal->setText("0.00");
        return;
    }
    QString uiTotal;
    float tmpTotal;
    QTableWidgetItem *wi;

    for (int i = 0; i < rowCt; i++) {
        wi = ui->tblOrderLinesWidget->item(i,4);
        tmpTotal += wi->text().toFloat();

        if(tmpTotal > 100)
        {
            this->ui->tblOrderLinesWidget->removeRow(i);
            OverLimitDialog();
            return;
        }
    }
    uiTotal.setNum(tmpTotal);
    ui->leOrderTotal->setText(uiTotal);
}
/* ========================================================================
 *
 * void resetFields()
 *
 * =======================================================================*/
void SetupTab::resetFields()
{
    ui->leAddLineItem_PartNumber->setText("");
    ui->leAddlineItem_Description->setText("");
    ui->spinBoxAddLineItem_Quantity->setValue(0);
    ui->leAddLineItem_Value->setText("");
    //ui->leAddLineItem_Quantity->setText("");
    //aui->leShipToAddress_ContactName->setText("");
}
/* ========================================================================
 *
 * void resetForm()
 *
 * =======================================================================*/
void SetupTab::resetForm()
{
    int rowCt = ui->tblOrderLinesWidget->rowCount();
    //Clear table
    rowCt += 1;
    int rowCtIndex= rowCt;

    for (int i = 0; i <= rowCt ; i++)
    {   //  Delete rows from the table after order submitted.
        this->ui->tblOrderLinesWidget->removeRow(rowCtIndex);
        //  Clears data from table.
        //this->ui->tblOrderLinesWidget->clear();
        //rowCt = ui->tblOrderLinesWidget->rowCount();
        rowCtIndex--;
    }// End for loop
    ui->leShipToDealer_PO->setText("");
    ui->leShipToAddress_Address1->setText("");
    ui->leShipToAddress_Address2->setText("");
    ui->leShipToAddress_City->setText("");
    ui->leShipToAddress_Name->setText("");
    ui->leShipToAddress_State->setText("");
    ui->leShipToAddress_Zip->setText("");
    ui->leShipToDealer_ID->setText("");
    ui->leShipToDealer_ID->setText("");
    ui->leOrderTotal->setText("");
    ui->leShipToAddress_ContactNumber->setText("");
    ui->leShipToAddress_ContactName->setText("");
    resetFields();
    QDate shipDate = QDate::currentDate();
    ui->dateEdit->setDate(shipDate.addDays(1));
    QString po = "";
    Utils *u;
    u->createPO(po);
    ui->leShipToDealer_PO->setText(po);
}
/* ========================================================================
 *
 * void on_btnAddLineItem_AddLine_clicked()
 *
 * =======================================================================*/
void SetupTab::on_btnAddLineItem_AddLine_clicked()
{
    QString partName;
    QString partDesc;
    QString partCost;
   //QString partQty;
    QString partQtyString;
    QString extCost;
    QString onHand;
    int partQtyFormVale;
    int partQtyInt;
    int onHandInt;
    float xCost;

    partName = ui->leAddLineItem_PartNumber->text();
    partDesc = ui->leAddlineItem_Description->text();
    partCost = ui->leAddLineItem_Value->text();
    partQtyFormVale  = ui->spinBoxAddLineItem_Quantity->value();
    //partQty = ui->leAddLineItem_Quantity->text();
    onHand = ui->leOnHandStock->text();
    //START: Extended Cost Mod - FJD - 8.9.2016
    if(partCost.toFloat() < 0.01)
    {
        //  If lees than penny, round to $00.01
        partCost.setNum(0.01);
    }
    xCost = (roundf((partQtyFormVale * partCost.toFloat()) * 100) / 100);
    extCost.setNum(xCost);
    //END: Extended Cost Mod
    //  Convert string to int.
   // partQtyInt = partQty.toInt();
    //  Convert string to sting.
    partQtyString = QString::number(partQtyFormVale);
    qDebug("Conver of the int to string %s", qUtf8Printable(partQtyString));

    partQtyInt = partQtyFormVale;
    onHandInt = onHand.toInt();

    if(partQtyInt > onHandInt)
    {
        OutOfStockDialog();
        return;
    }

    if(xCost > 100)
    {
        OverLimitDialog();
        return;
    }

    if((PartOkToAdd(partName, partDesc, partCost, partQtyFormVale) == true) && (xCost < 100))
    {
        int row = ui->tblOrderLinesWidget->rowCount();
        ui->tblOrderLinesWidget->insertRow(row);
        ui->tblOrderLinesWidget->setItem(row,0, new QTableWidgetItem (partName));
        ui->tblOrderLinesWidget->setItem(row,1, new QTableWidgetItem (partDesc));
        ui->tblOrderLinesWidget->setItem(row,2, new QTableWidgetItem (partCost));
        qDebug("What is part value ??????? %d", partQtyFormVale);
        ui->tblOrderLinesWidget->setItem(row,3, new QTableWidgetItem (partQtyString));
        ui->tblOrderLinesWidget->setItem(row,4, new QTableWidgetItem (extCost));
        updateExtendedCostTotal();
        resetFields();
    }
}
/* =================================================================================================
 *
 * bool PartOkToAdd(QString , QString , QString , QString)
 *
 * ===============================================================================================*/
bool SetupTab::PartOkToAdd(QString partName, QString partDesc, QString partCost, int partQty)
{
    bool retval = true;
    if (partName == "") {
        retval = false;
    }
    if (partDesc == "") {
        retval = false;
    }
    if (partCost == "") {
        retval = false;
    }
    if (partQty == 0){  // || partQty == "0" || partQty.contains("-")) {
        retval = false;
    }
    if (retval == false) {
        MissingDataDialog *mdDialog = new MissingDataDialog;
        mdDialog->show();
    }
    return retval;
}
/* ===============================================================================================
 *
 * void handleDeleteSelectedRow()
 *
 * =============================================================================================*/
void SetupTab::handleDeleteSelectedRow()
{
    QList<QTableWidgetItem*> selectionRangeList = this->ui->tblOrderLinesWidget->selectedItems();
    int rowIndex;
    QListIterator<QTableWidgetItem*> selectionRangeListIter(selectionRangeList);
    while(selectionRangeListIter.hasNext()){
        rowIndex = ((int)((QTableWidgetItem*)selectionRangeListIter.next())->row());
        this->ui->tblOrderLinesWidget->removeRow(rowIndex);
    }
}
/* ========================================================================
 *
 * void on_btnRecapAndSubmit_Clear_clicked()
 *
 * =======================================================================*/
void SetupTab::on_btnRecapAndSubmit_Clear_clicked()
{
    handleDeleteSelectedRow();
    updateExtendedCostTotal();
}
/* ========================================================================
 *
 * void OutOfStockDialog()
 *
 * =======================================================================*/
void SetupTab::OutOfStockDialog()
{
    outOfStock *dialog = new outOfStock;
    dialog->exec();
}
/* ========================================================================
 *
 * void OverLimitDialog()
 *
 * =======================================================================*/
void SetupTab::OverLimitDialog()
{
    overCostLimitDialog *dialog = new overCostLimitDialog;
    dialog->exec();
}
/* ========================================================================
 *
 * void CheckDealerIsAdded()
 *
 * =======================================================================*/
bool SetupTab::CheckRequiredDataAdded()
{
    bool boolLogicValue = true;
    QString dealerName = ui->leShipToAddress_Name->text();
    QString address = ui->leShipToAddress_Address1->text();
    QString city = ui->leShipToAddress_City->text();
    QString country = ui->leShipToAddress_Country->text();
    QString contactPhone = ui->leShipToAddress_ContactNumber->text();

    if((dealerName == "") || (address == "") || (city == "") || (country == "") || (ui->tblOrderLinesWidget->rowCount() == 0) || (contactPhone == ""))
    {
        MissingDataDialog *mdDialog = new MissingDataDialog;
        mdDialog->show();
        boolLogicValue = false;
    }
    return boolLogicValue;
}
/* ========================================================================
 *
 * void on_btnSubmitOrder_clicked()
 *
 * =======================================================================*/
void SetupTab::on_btnSubmitOrder_clicked()
{
    QString output;
    QString plusText = " small parts order has been submitted.";
    if(CheckRequiredDataAdded() == true)
    {
        output = ui->leShipToDealer_PO->text();
        ui->lblOrderSubmitTotal->setText(output + plusText);
        WriteXml();
        resetForm();
    }

}
/* ========================================================================
 *
 * QString mapRoutingComboBoxCodes(QString &)
 *
 * =======================================================================*/
QString SetupTab::mapRoutingComboBoxCodes(QString &routingCode)
{
    //  Get row from routing combo box, map and return code.
    int row = ui->cbxShipToDealer_Routing->currentIndex();
     switch (row)
     {
     case 0:
         routingCode = "GRD";
         break;
     case 1:
         routingCode = "GSTD";
         break;
     case 2:
         routingCode = "NAM";
         break;
     case 3:
         routingCode = "NEAM";
         break;
     case 4:
         routingCode = "NPM";
         break;
     case 5:
         routingCode = "NSAT";
         break;
     case 6:
         routingCode = "2PM";
         break;
     case 7:
         routingCode = "3DF";
         break;
     }
     return routingCode;
}
/* ========================================================================
 *
 * void WriteXml()
 *
 * =======================================================================*/
void SetupTab::WriteXml()
{
    QString shipViaCode;
    QDate orderDate = QDate::currentDate();
    QString orderDay = QString::number(orderDate.day());
    QString orderMonth = QString::number(orderDate.month());
    QString orderYear = QString::number(orderDate.year());

    QDate dateToShip = ui->dateEdit->date();
    QString shipDay = QString::number(dateToShip.day());
    QString shipMonth = QString::number(dateToShip.month());
    QString shipYear = QString::number(dateToShip.year());
    //  Map routing codes from xml
    mapRoutingComboBoxCodes(shipViaCode);

    QDir res_dir (OUTFILE_PREFIX);

    const QString PLACEHOLDER;

    auto path = res_dir.filePath("XmlForMacPac.xml");
    QFile res_file(path);
    QFile xmlToService_file(path);

    if(!res_file.open(QIODevice::WriteOnly))
    {
        //TODO Send Error message
        qDebug() << FILEERROR_MSG;
    }
    res_file.open(QIODevice::ReadWrite);
    QXmlStreamWriter xmlWriter(&res_file);
    xmlWriter.setAutoFormatting(true);
    //PLACEHOLDER denotes where form data might go
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("Communication");
    xmlWriter.writeTextElement("CommunicationType", "SPO");
    xmlWriter.writeStartElement("Contexts");
    xmlWriter.writeTextElement("ParserVersion", "1.0");
    xmlWriter.writeTextElement("POGUID", ui->leShipToDealer_PO->text());
    xmlWriter.writeEndElement();// End Contexts
    xmlWriter.writeStartElement("OrderInstructions");
    xmlWriter.writeTextElement("Environment", "Internal");
    xmlWriter.writeTextElement("RefQuote", "");
    xmlWriter.writeTextElement("DiscountCode", "");
    xmlWriter.writeTextElement("ShipVia", shipViaCode);
    xmlWriter.writeTextElement("ShipViaAcctNum", "");
    xmlWriter.writeTextElement("ContactName", "");  //Salesperson info
    xmlWriter.writeTextElement("ContactPhone", "");
    xmlWriter.writeTextElement("PrevPOGUID", "");
    xmlWriter.writeTextElement("ContactEmail", ui->leSalespersonEmail->text());
    xmlWriter.writeTextElement("DeliveryNotes", "");
    xmlWriter.writeStartElement("OrderComments");
    xmlWriter.writeTextElement("OrderComment", PLACEHOLDER);
    xmlWriter.writeEndElement();//End OrderComments
    xmlWriter.writeEndElement();//End OrderInstructions
    xmlWriter.writeStartElement("MetaInfo");
    xmlWriter.writeTextElement("PONumber", ui->leShipToDealer_PO->text());
    xmlWriter.writeTextElement("EndCustPO", "");
    xmlWriter.writeTextElement("DealerSONumber", "");
    xmlWriter.writeStartElement("POCreated");
    xmlWriter.writeTextElement("Year", orderYear);
    xmlWriter.writeTextElement("Month", orderMonth);
    xmlWriter.writeTextElement("Day", orderDay);
    xmlWriter.writeEndElement();//End POCreated
    xmlWriter.writeStartElement("ProjectInfo");
    xmlWriter.writeTextElement("ProjectName", "");
    xmlWriter.writeTextElement("ProjectID", "");
    xmlWriter.writeEndElement();//End ProjectInfo
    xmlWriter.writeStartElement("PartnerInfo");
    xmlWriter.writeTextElement("PartnerType", "Internal");
    xmlWriter.writeTextElement("ERPID", "1600S");
    xmlWriter.writeTextElement("PartnerName", "Draper, Inc.");
    xmlWriter.writeEndElement();//End PartnerInfo
    xmlWriter.writeStartElement("Addressing");
    xmlWriter.writeStartElement("SoldTo");
    xmlWriter.writeTextElement("Name", "Draper, Inc.");
    xmlWriter.writeTextElement("Address1", "411 Pearl Street");
    xmlWriter.writeTextElement("Address2", "");
    xmlWriter.writeTextElement("Address3", "");
    xmlWriter.writeTextElement("City", "Spiceland");
    xmlWriter.writeTextElement("State", "IN");
    xmlWriter.writeTextElement("Zip", "47385");
    xmlWriter.writeTextElement("CountryCode", "USA");
    xmlWriter.writeStartElement("GPS");
    xmlWriter.writeTextElement("Lat", "0.0");
    xmlWriter.writeTextElement("Lon", "0.0");
    xmlWriter.writeEndElement();//End GPS
    xmlWriter.writeEndElement();//End SoldTo
    xmlWriter.writeStartElement("ShipTo");
    xmlWriter.writeTextElement("Name", ui->leShipToAddress_Name->text());
    xmlWriter.writeTextElement("Address1", ui->leShipToAddress_Address1->text());
    xmlWriter.writeTextElement("Address2", ui->leShipToAddress_Address2->text());
    xmlWriter.writeTextElement("Address3", PLACEHOLDER);
    xmlWriter.writeTextElement("City", ui->leShipToAddress_City->text());
    xmlWriter.writeTextElement("State", ui->leShipToAddress_State->text());
    xmlWriter.writeTextElement("Zip", ui->leShipToAddress_Zip->text());
    xmlWriter.writeTextElement("CountryCode", ui->leShipToAddress_Country->text());
    xmlWriter.writeStartElement("GPS");
    xmlWriter.writeTextElement("Lat", "0.0");
    xmlWriter.writeTextElement("Lon", "0.0");
    xmlWriter.writeEndElement();//End GPS
    xmlWriter.writeStartElement("Instructions");
    xmlWriter.writeTextElement("Attention", ui->leShipToAddress_ContactName->text());
    xmlWriter.writeTextElement("Phone", ui->leShipToAddress_ContactNumber->text()); // ui contact phone goes here.
    xmlWriter.writeTextElement("Email", "");
    xmlWriter.writeTextElement("Note1", "");
    xmlWriter.writeTextElement("Note2", "");
    xmlWriter.writeTextElement("Note3", "");
    xmlWriter.writeTextElement("Note4", "");
    xmlWriter.writeTextElement("ShippingQuote", "");
    xmlWriter.writeEndElement();//End Instructions
    xmlWriter.writeEndElement();//End ShipTo
    xmlWriter.writeEndElement();//End Addressing
    xmlWriter.writeEndElement();//End MetaInfo
    xmlWriter.writeStartElement("OrderLines");
    //Loop for lines
    QVariant partValue;
    QVariant partDesc;
    QVariant partCost;
    QVariant partQty;
    QMap<QString, QString> partMap;
    QTableWidgetItem* tmpItem;
    int lineOrderNumber = 0;
    for (int row = 0; row < this->ui->tblOrderLinesWidget->rowCount(); row++) {
        for (int col = 0; col <= this->ui->tblOrderLinesWidget->columnCount(); col++){
            tmpItem = this->ui->tblOrderLinesWidget->item(row, col);
            if (col == 0) {
                partValue = tmpItem->text();
                partMap["PartName"] = partValue.toString();
            }
            else if (col == 1) {
                partDesc = tmpItem->text();
                partMap["PartDesc"] = partDesc.toString();
            }
            else if (col == 2) {
                partCost = tmpItem->text();
                partMap["PartCost"] = partCost.toString();
            }
            else if (col == 3) {
                partQty = tmpItem->text();
                partMap["PartQty"] = partQty.toString();
                qDebug("This is my console print for Qt!!!!!!!");
            }
        }
        lineOrderNumber++; //   Count for </Number> node

        xmlWriter.writeStartElement("OrderLine");
        xmlWriter.writeStartElement("LineInstructions");
        xmlWriter.writeTextElement("LineInstruction", "");
        xmlWriter.writeEndElement();//End LineInstructions
        xmlWriter.writeStartElement("POInfo");
        xmlWriter.writeTextElement("Status", "");
        xmlWriter.writeTextElement("Number", QString::number(lineOrderNumber));
        xmlWriter.writeTextElement("Qty", partMap["PartQty"]);
        xmlWriter.writeTextElement("QtyUOM", "");
        xmlWriter.writeTextElement("Price", partMap["PartCost"]);
        xmlWriter.writeTextElement("Location", "");
        xmlWriter.writeTextElement("SourcePart", "");
        xmlWriter.writeTextElement("TargetPart", partMap["PartName"]);
        xmlWriter.writeTextElement("Salesperson", "");
        xmlWriter.writeTextElement("ContactEmail", "");
        xmlWriter.writeStartElement("RequestedShipDate");
        xmlWriter.writeTextElement("Year", shipYear);
        xmlWriter.writeTextElement("Month", shipMonth);
        xmlWriter.writeTextElement("Day", shipDay);
        xmlWriter.writeEndElement();//End RequestedShipDate
        xmlWriter.writeTextElement("CarrierName", "");
        xmlWriter.writeEndElement();//End POInfo
        xmlWriter.writeEndElement();//End OrderLine
    }
    xmlWriter.writeEndElement();//End OrderLines
    xmlWriter.writeEndElement();//End Communication
    res_file.flush();
    xmlToService_file.open(QIODevice::ReadOnly);
    QByteArray xmlToServiceAry = xmlToService_file.readAll();
    res_file.close();
    xmlToService_file.close();
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////PostXMLToService(xmlToServiceAry);
}
/************************************************************************
 *
 *  PostXMLToService()
 *
 ************************************************************************/
void SetupTab::PostXMLToService(QByteArray& xmlData)
{
    QString PO = ui->leShipToDealer_PO->text();
    QEventLoop eventLoop;
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    //QString textSearch = ui->leEnterSearchCriteria->text();
    QNetworkRequest request(QUrl("http://spi-rabbit2:8080/sporders/new/"+PO));
    request.setRawHeader("Content-Type", "application/xml");
    //QNetworkReply *reply = mgr.post(request, xmlData);
    mgr.post(request, xmlData);
    eventLoop.exec();
}

