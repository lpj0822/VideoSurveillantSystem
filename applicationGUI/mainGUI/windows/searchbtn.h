#ifndef SEARCHBTN_H
#define SEARCHBTN_H

#include <QLineEdit>

class SearchBtn : public QLineEdit {

    Q_OBJECT

public:
    SearchBtn(const QString &linetext, QWidget* parent=0);

signals:
    void signal_beginToSreach(const QString &);//text

private:

};

#endif // SEARCHBTN_H
