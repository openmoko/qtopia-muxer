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

#ifndef QSERIALIODEVICEMULTIPLEXER_H
#define QSERIALIODEVICEMULTIPLEXER_H

#include <qserialiodevice.h>

class QSerialIODeviceMultiplexerPrivate;

class QTOPIACOMM_EXPORT QSerialIODeviceMultiplexer : public QObject
{
    Q_OBJECT
public:
    explicit QSerialIODeviceMultiplexer( QObject *parent = 0 );
    ~QSerialIODeviceMultiplexer();

    virtual QSerialIODevice* open (int channel) = 0;
    virtual void close(int channel) = 0;


    static bool chat( QSerialIODevice *device, const QString& cmd );
    static QString chatWithResponse
            ( QSerialIODevice *device, const QString& cmd );
    static QSerialIODeviceMultiplexer *create(QSerialIODevice *device, int frameSize, bool advanced );
};

#if ENABLED_FOO
class QNullSerialIODeviceMultiplexerPrivate;

class QTOPIACOMM_EXPORT QNullSerialIODeviceMultiplexer : public QSerialIODeviceMultiplexer
{
    Q_OBJECT
public:
    QNullSerialIODeviceMultiplexer
        ( QSerialIODevice *device, QObject *parent = 0 );
    ~QNullSerialIODeviceMultiplexer();

    QSerialIODevice *channel( const QString& name );

private slots:
    void dataOpened();
    void dataClosed();

private:
    QNullSerialIODeviceMultiplexerPrivate *d;
};
#endif

#endif /* QSERIALIODEVICEMULTIPLEXER_H */
