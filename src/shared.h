/****************************************************************************
* QThisPlayer - media player
* Copyright (C) 2021 Helder Batalha <helderbatalha3@gmail.com>
*
* This library is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library. If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#ifndef SHARED_H
#define SHARED_H

#include <QStringList>
#include <QUrl>
#include <tuple>
#include <QList>
#include <QFileInfo>

const QStringList supportedMediaFormats = {"3g2", "3gp", "3gp2", "3gpp", "amv", "asf", "avi", "bik", "bin", "divx",
                                           "drc", "dv ", "f4v", "flv", "gvi", "gxf", "iso", "m1v", "m2v", "m2t",
                                           "m2ts", "m4v", "mkv", "mov", "mp2", "mp2V", "mp4", "mp4v", "mpe", "mpeg",
                                           "mpeg1", "mpeg2", "mpeg4", "mpg", "mpv2","mts", "mtv", "mxf", "mxg"
                                           "nsv", "nuv", "ogg", "ogm", "ogv", "ogx", "ps", "rec", "rm", "rmvb", "rpl",
                                           "thp", "tod", "ts", "tts", "txd", "vob", "vro", "webm", "wm", "wmv", "wtv",
                                           "xesc", "3ga", "669", "152", "aac", "ac3", "adt", "adts", "aif", "aiff", "amr",
                                           "aob", "ape", "awb", "caf", "dts", "flac", "it", "kar", "m4a","m4b",
                                           "m4p", "m5p", "mid", "mka", "mlp", "mod", "mpa", "mp1", "mp2", "mp3", "mpc",
                                           "mpga", "mus", "oga", "ogg", "oma", "opus", "qcp", "ra", "rmi", "s3m", "sid",
                                           "spx", "thd", "tta", "voc", "vqf", "w64", "wav", "wma", "wv", "xa", "xm", "3g2",
                                           "3gp", "3gp2", "3gpp", "amv", "asf", "avi", "bik",   "bin", "divx", "drc",
                                           "dv", "f4v", "flv", "gvi", "gxf", "iso", "m1v", "m2v", "m2t", "m2ts", "m4v",
                                           "mkv", "mov", "mp2", "mp2V", "mp4", "mp4v", "mpe", "mpeg", "mpeg1", "mpeg2",
                                           "mpeg4", "mpg", "mpv2", "mts", "mtv", "mxf", "mxg", "nsv", "nuv", "ogg", "ogm",
                                           "ogv", "ogx", "ps", "rec", "rm", "rmvb", "rpl", "thp", "tod", "ts", "tts",
                                           "txd", "vob", "vro", "webm", "wm", "wmv", "wtv", "xesc", "3ga", "669", "152",
                                           "aac", "ac3", "adt", "adts", "aif", "aiff", "amr", "aob", "ape", "awb", "caf",
                                           "dts", "flac", "it", "kar", "m4a", "m4b", "m4p", "m5p", "mid", "mka",
                                           "mlp", "mod", "mpa", "mp1", "mp2", "mp3", "mpc", "mpga", "mus", "oga", "ogg",
                                           "oma", "opus", "qcp", "ra", "rmi", "s3m", "sid", "spx", "thd", "tta", "voc",
                                           "vqf", "w64", "wav", "wma", "wv", "xa", "xm"
                                          }; // TODO: separate the video and audio extensions

QList<QFileInfo> filterSupportedMediaFormats(const QList<QUrl>& urls);
bool areAllSubtitleFiles(const QList<QUrl>& urls);
QString formattedTime(int millSec);
QIcon invertedColorIcon(QIcon icon);

#endif // SHARED_H
