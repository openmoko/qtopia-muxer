/****************************************************************************
**
** Copyright (C) 2000-2008 TROLLTECH ASA. All rights reserved.
**
** This file is part of the Opensource Edition of the Qtopia Toolkit.
**
** This software is licensed under the terms of the GNU General Public
** License (GPL) version 2.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef QSERIALIODEVICE_H
#define QSERIALIODEVICE_H

#include <muxerglobal.h>

#include <qobject.h>
#include <qiodevice.h>
#include <qstring.h>
#include <qstringlist.h>


class QTOPIACOMM_EXPORT QSerialIODevice : public QIODevice
{
    Q_OBJECT
public:
    explicit QSerialIODevice( QObject *parent = 0 );
    ~QSerialIODevice();

    bool isSequential() const;

    virtual int rate() const;
    virtual bool dtr() const = 0;
    virtual void setDtr( bool value ) = 0;
    virtual bool dsr() const = 0;
    virtual bool carrier() const = 0;
    virtual bool setCarrier( bool value );
    virtual bool rts() const = 0;
    virtual void setRts( bool value ) = 0;
    virtual bool cts() const = 0;

    virtual void discard() = 0;

    virtual bool waitForReady() const;

    virtual void abortDial();

    virtual bool isValid() const;

signals:
    void dsrChanged( bool value );
    void carrierChanged( bool value );
    void ctsChanged( bool value );
    void ready();

protected slots:
    void internalReadyRead();
};


class QTOPIACOMM_EXPORT QNullSerialIODevice : public QSerialIODevice
{
    Q_OBJECT
public:
    explicit QNullSerialIODevice( QObject *parent = 0 );
    ~QNullSerialIODevice();

    bool open( OpenMode mode );
    void close();
    qint64 bytesAvailable() const;
    int rate() const;
    bool dtr() const;
    void setDtr( bool value );
    bool dsr() const;
    bool carrier() const;
    bool rts() const;
    void setRts( bool value );
    bool cts() const;
    void discard();
    bool isValid() const;

protected:
    qint64 readData( char *data, qint64 maxlen );
    qint64 writeData( const char *data, qint64 len );
};

#endif // QSERIALIODEVICE_H
