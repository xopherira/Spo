#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H
#include <QDialog>

namespace Ui {
    class SearchDialog;
}

class QXmlStreamReader;
class AddLineItem;

class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(QWidget *parent = 0);
    void XmlDialogSearchRequestParsing(QXmlStreamReader &XmlFile);
    ~SearchDialog();
    //QList<QTableWidgetItem *> getSelected();
    QMap<QString, QString> getMap();

private slots:
    void on_btnSearchDialog_clicked();
    void startSearchRequest();
    void on_btnAddSelectedItem_clicked();
    void on_btnCancelSearch_clicked();

private:
    Ui::SearchDialog *ui;
};

#endif // SEARCHDIALOG_H
