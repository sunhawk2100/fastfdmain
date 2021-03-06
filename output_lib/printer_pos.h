#ifndef PRINTER_POS_H
#define PRINTER_POS_H

#include "printer_0.h"
#include <QtDebug>

class Printer_POS
{
public:
    Printer_POS();
    virtual  ~Printer_POS();
    virtual qint64 writedirect(const QByteArray & byte)=0;
    virtual qint64 write(const QString & byte)=0;

    virtual qint64 writeHex(const QString & byte)=0;

    virtual bool tryOpen(const QString &port)=0;

    virtual void close()=0;

    virtual QString lastError()=0;

    virtual QString port()=0;

    virtual bool waitForReadyReadv(int msecs)=0;//+v做区分

    virtual QByteArray readAll()=0;

    static QstandarditemmodelManager class_name_model;

    static Printer_POS *get_pos_printer_by_path(const QString &path);


    static bool com_upsiseffect;

};

class Printer_POS_auto_destroy{
public:
    Printer_POS_auto_destroy(Printer_POS *printer);
    ~Printer_POS_auto_destroy();
    Printer_POS *printer;
};
#endif // PRINTER_POS_H
