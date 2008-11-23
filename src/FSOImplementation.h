/*
 * Copyright (C) 2008 Openmoko Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FSOImplementation_h
#define FSOImplementation_h

#include <QObject>
#include <QDBusConnection>

class QSerialIODeviceMultiplexer;

/**
 * Implement the FSO muxer interface
 */
class FSOImplementation : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freesmartphone.GSM.MUX")

public:
    FSOImplementation(QSerialIODeviceMultiplexer* mux);


public Q_SLOTS:
    Q_SCRIPTABLE void SetPower(const QString&, bool);
    Q_SCRIPTABLE bool GetPower(const QString&);
    Q_SCRIPTABLE void Reset(const QString&);
    Q_SCRIPTABLE QString AllocChannel(const QString&);

private Q_SLOTS:
    void channelDied(int);

private:
    int allocateChannelNumber(const QString&);
    QString allocateChannel(int number);

private:
    QSerialIODeviceMultiplexer* m_multiplexer;
    QMap<int, QString> m_allocatedChannels;
};


#endif
