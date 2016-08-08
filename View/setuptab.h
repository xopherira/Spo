#ifndef SETUPTAB_H
#define SETUPTAB_H
#include <QWidget>
#include <QXmlStreamReader>

namespace Ui {
    class SetupTab;
}

class SetupTab : public QWidget
{
    Q_OBJECT

public:
    explicit SetupTab(QWidget *parent);
    ~SetupTab();
    void GetAddressXML();
    void XmlAddressSearchRequestParsing(QXmlStreamReader &XmlFileReader);

public slots:
    void SaveAddressXML();
private slots:
    //void on_btnAddShipToAddress_clicked();
    void on_leShipToDealer_ID_editingFinished();
private:
    QString m_basefilename;
    QString basefilename;
    Ui::SetupTab *ui;
    QString ReadBaseXMLFile();
    QString ReadBaseXMLFromInternalResource();
};

#endif // SETUPTAB_H
