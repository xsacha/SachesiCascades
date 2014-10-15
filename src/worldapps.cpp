
// Copyright (C) 2014 Sacha Refshauge

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3.0.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License 3.0 for more details.

// A copy of the GPL 3.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official GIT repository and contact information can be found at
// http://github.com/xsacha/Sachesi

#include "worldapps.h"

#define SET_QML2(type, name, caps) \
    type AppWorldApps::name() const { \
        return _ ## name; \
    } \
    void AppWorldApps::caps(const type &var) { \
        if (var != _ ## name) { \
            _ ## name = var; \
            emit name ## Changed(); \
        } \
    }

AppWorldApps::AppWorldApps(QObject *parent)
    : QObject(parent)
    , _name(""), _friendlyName(""), _packageId("")
    , _id(""), _fileId(""), _size(0), _image("")
    , _isMarked(false), _isAvailable(true)
    , _type("")
    , _version(""), _versionId("")
    , _checksum(""), _vendor(""), _vendorId(""), _price(""), _description("")
{ }

SET_QML2(QString, name, setName)
SET_QML2(QString, friendlyName, setFriendlyName)
SET_QML2(QString, packageId, setPackageId)
SET_QML2(QString, id, setId)
SET_QML2(QString, fileId, setFileId)
SET_QML2(int, size, setSize)
SET_QML2(QString, image, setImage)
SET_QML2(bool, isMarked, setIsMarked)
SET_QML2(bool, isAvailable, setIsAvailable)
SET_QML2(QString, type, setType)
SET_QML2(QString, version, setVersion)
SET_QML2(QString, versionId, setVersionId)
SET_QML2(QString, checksum, setChecksum)
SET_QML2(QString, vendor, setVendor)
SET_QML2(QString, vendorId, setVendorId)
SET_QML2(QString, price, setPrice)
SET_QML2(QString, description, setDescription)
