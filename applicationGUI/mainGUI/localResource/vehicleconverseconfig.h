#ifndef VEHICLECONVERSECONFIG_H
#define VEHICLECONVERSECONFIG_H

#include <QDialog>
#include <QPushButton>
#include <QComboBox>
#include "../expand/editablelabel.h"

class VehicleConverseConfig : public QDialog
{
    Q_OBJECT
public:
    explicit VehicleConverseConfig(QWidget *parent = 0);
    ~VehicleConverseConfig();
    void updatePreview(const QImage &image);

    int getDirection();
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
    QComboBox *cbbDirection;
};

#endif // VEHICLECONVERSECONFIG_H
