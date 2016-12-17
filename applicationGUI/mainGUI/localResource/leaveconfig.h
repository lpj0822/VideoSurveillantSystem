#ifndef LEAVECONFIG_H
#define LEAVECONFIG_H

#include <QDialog>
#include <QPushButton>
#include <QTimeEdit>
#include "../expand/editablelabel.h"

class LeaveConfig : public QDialog
{
    Q_OBJECT
public:
    explicit LeaveConfig(QWidget *parent = 0);
    ~LeaveConfig();
    void updatePreview(const QImage &image);

    int getTime();
    QList<QPolygonF> getPolygon();

signals:

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
    QTimeEdit *timeEdit;
};
#endif // LEAVECONFIG_H
