#ifndef ERRORCODESTRING_H
#define ERRORCODESTRING_H

#include <QObject>

class ErrorCodeString : public QObject
{
    Q_OBJECT
public:
    ErrorCodeString(QObject *parent = 0);
    ~ErrorCodeString();

    void setErrorCode(int code);
    int getErrorCode();

    QString getErrCodeString(int errCode);//得到错误码对应的字符串

signals:

public slots:

private:
    int errorCode;

    void init();
};

#endif // ERRORCODESTRING_H
