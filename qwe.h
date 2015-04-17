#ifndef QWE_H
#define QWE_H

#include <QThread>

class qwe : public QThread
{
    Q_OBJECT
public:
    explicit qwe(QObject *parent = 0);

signals:

public slots:

};

#endif // QWE_H
