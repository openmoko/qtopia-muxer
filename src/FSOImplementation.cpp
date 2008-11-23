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

#include "FSOImplementation.h"
#include "qserialiodevicemultiplexer.h"

#include <QSocketNotifier>

#include <pty.h>
#include <limits.h>

// forward data....
class ForwardPty : public QObject {
    Q_OBJECT
public:
    ForwardPty(int channelNumber, QSerialIODevice*, int master, int slave);
    ~ForwardPty();

Q_SIGNALS:
    void channelDied(int);

private Q_SLOTS:
    void slotSendToPty();
    void slotReadFromPty();
    void slotError();

private:
    int m_channelNumber;
    QSerialIODevice* m_device;
    int m_masterFd;
    int m_slaveFd;
};

ForwardPty::ForwardPty(int channelNumber, QSerialIODevice* device, int master, int slave)
    : m_channelNumber(channelNumber)
    , m_device(device)
    , m_masterFd(master)
    , m_slaveFd(slave)
{
    // Read and handle error from the pty
    QSocketNotifier* notifier;
    notifier = new QSocketNotifier(m_masterFd, QSocketNotifier::Read);
    connect(notifier, SIGNAL(activated(int)), SLOT(slotReadFromPty()));

    notifier = new QSocketNotifier(m_masterFd, QSocketNotifier::Exception);
    connect(notifier, SIGNAL(activated(int)), SLOT(slotError()));

    // Read from the channel
    connect(m_device, SIGNAL(readyRead()), SLOT(slotSendToPty()));
}

ForwardPty::~ForwardPty()
{
    close(m_masterFd);
    close(m_slaveFd);
}

// From pty to the muxer
void ForwardPty::slotReadFromPty()
{
    char data[4096];
    int length = read(m_masterFd, data, 4096);
    if (length <= 0) {
        fprintf(stderr, "Failed to read from pty of channel: %d\n", m_channelNumber);
        return;
    }

    int result = m_device->write(data, length);
    if (result != length)
        fprintf(stderr, "Failed to forward from pty to muxer...\n");
}

// From muxer to the pty master
void ForwardPty::slotSendToPty()
{
    char data[4096];
    int length = m_device->read(data, 4096);

    if (length < 0) {
        fprintf(stderr, "Failed to read from channel: %d\n", m_channelNumber);
        return;
    }

    int output = write(m_masterFd, data, length);
    if (output != length)
        fprintf(stderr, "Failed to write the same amount of data to the pty...\n");
}

void ForwardPty::slotError()
{
    emit channelDied(m_channelNumber);
    deleteLater();
}



FSOImplementation::FSOImplementation(QSerialIODeviceMultiplexer* mux)
    : m_multiplexer(mux)
{}

void FSOImplementation::SetPower(const QString& origin, bool on)
{
    Q_UNUSED(origin);
    Q_UNUSED(on);
}

bool FSOImplementation::GetPower(const QString& origin)
{
    Q_UNUSED(origin);
    return true;
}

void FSOImplementation::Reset(const QString& origin)
{
    Q_UNUSED(origin);
}

QString FSOImplementation::AllocChannel(const QString& origin)
{
    int channelNumber = allocateChannelNumber(origin);

    // okay. either we are out of channels or the origin is already occupied..
    if (channelNumber == -1 || m_allocatedChannels.contains(channelNumber))
        return QString();

    // allocate channel and forward the data between the pty and hits
    return allocateChannel(channelNumber);
}

// a pty connection died... close the channel
void FSOImplementation::channelDied(int channelNumber)
{
    Q_ASSERT(m_allocatedChannels.contains(channelNumber));

    m_multiplexer->close(channelNumber);
    m_allocatedChannels.remove(channelNumber);

    Q_ASSERT(!m_allocatedChannels.contains(channelNumber));
}

int FSOImplementation::allocateChannelNumber(const QString& origin)
{
    if (origin == QLatin1String("ppp"))
        return 3;

    // try to find a free id...
    for (int i = 1; i <= 63; ++i) {
        if (!m_allocatedChannels.contains(i))
            return i;
    }

    return -1;
}

QString FSOImplementation::allocateChannel(int number)
{
    Q_ASSERT(!m_allocatedChannels.contains(number));

    // create a pty now
    int master_fd;
    int slave_fd;
    char slave_name[PATH_MAX];
    int result = openpty(&master_fd, &slave_fd, slave_name, NULL, NULL);
    if (result < 0 ) {
        perror("allocateChannel:");
        return QString();
    }

    QSerialIODevice* device = m_multiplexer->open(number);
    if (!device) {
        fprintf(stderr, "Failed to open the multiplexer channel\n");
        close(master_fd);
        close(slave_fd);
        return QString();
    }

    ForwardPty* forward = new ForwardPty(number, device, master_fd, slave_fd);
    connect(forward, SIGNAL(channelDied(int)), SLOT(channelDied(int)), Qt::QueuedConnection);

    m_allocatedChannels[number] = QString::fromLocal8Bit(slave_name);
    m_multiplexer->open(number);
    return m_allocatedChannels[number];
}

#include "FSOImplementation.moc"
