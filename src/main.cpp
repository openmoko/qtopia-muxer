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

#include <QCoreApplication>
#include <QFile>

#include "qserialport.h"
#include "qgsm0710multiplexer.h"
#include "FSOImplementation.h"

#include <stdlib.h>

// Command Line Parsing
#define PARSE_OPTION_STRING(str, var)             \
    if (qstrcmp(argv[i], str) == 0 && i+1 < argc) \
        var = QString::fromLocal8Bit(argv[++i]);
#define PARSE_OPTION_NUMBER(str, var)             \
    if (qstrcmp(argv[i], str) == 0 && i+1 < argc) \
        var = atoi(argv[++i]);

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    /*
     * Some poor man command line parsing
     */
    QString muxerDevice = QString::fromLatin1("/dev/ttySAC0");
    int frameSize = 64;
    int baudRate = 115200;
    int advanced = 1;
    for (int i = 1; i < argc; ++i) {
        PARSE_OPTION_STRING("-f", muxerDevice)
        PARSE_OPTION_NUMBER("-s", frameSize)
        PARSE_OPTION_NUMBER("-b", baudRate)
        PARSE_OPTION_NUMBER("-a", advanced);
        if (qstrcmp(argv[i], "-h") == 0) {
            fprintf(stderr, "Muxer help\n");
            fprintf(stderr, "\t-f deviceFile (default: %s)\n", qPrintable(muxerDevice));
            fprintf(stderr, "\t-s frameSize  (default: %d)\n", frameSize);
            fprintf(stderr, "\t-b baudRate   (default: %d)\n", baudRate);
            fprintf(stderr, "\t-a 0|1 advanced mode (default: %d)\n", advanced);
        }
    }

    if (!muxerDevice.startsWith("sim:") && !QFile::exists(muxerDevice)) {
        fprintf(stderr, "The muxer device '%s' does not exist\n", qPrintable(muxerDevice));
        return EXIT_FAILURE;
    }

    QSerialIODevice* device = QSerialPort::create(muxerDevice, baudRate);
    if (!device) {
        fprintf(stderr, "Failed to create a serial device\n");
        return EXIT_FAILURE;
    }

    QSerialIODeviceMultiplexer* multiplexer = QSerialIODeviceMultiplexer::create(device, frameSize, !!advanced);
    if (!multiplexer) {
        fprintf(stderr, "Failed to create a multiplexer device\n");
        return EXIT_FAILURE;
    }

    QDBusConnection::systemBus().registerObject("/Muxer",
                                                 new FSOImplementation(multiplexer),
                                                 QDBusConnection::ExportScriptableContents);
    bool result = QDBusConnection::systemBus().registerService("org.openmoko.qtopia.fso");
    if (!result)
        fprintf(stderr, "Failed to register service org.openmoko.qtopia.fso\n");

    printf("Set up the multiplexer. awaiting commands\n");
    app.exec();
}
