#ifndef DLOCALPARSECONFIG_H
#define DLOCALPARSECONFIG_H

#include <QDialog>
#include <QPushButton>
#include <QComboBox>
#include "../expand/editablelabel.h"

class DLocalParseConfig : public QDialog
{
    Q_OBJECT
public:
    explicit DLocalParseConfig(QWidget *parent = 0);
    ~DLocalParseConfig();
    void updatePreview(const QImage &image);

    int getDirection();
    QList<QPolygonF> getPolygon();

signals:
    void signal_ConfigFinished(QList<QPolygonF> &list);

public slots:

private slots:

    void on_btnOKClicked();
    void on_btnCancleClicked();
    void on_btnReConfigClicked();

private:
    void initData();
    void initUI();
    void initConnect();

private:
    EditableLabel *editArea;
    QPushButton *btnReConfig;
    QPushButton *btnOK;
    QPushButton *btnCancel;
    QComboBox *cbbDirection;
};

#endif // DLOCALPARSECONFIG_H
