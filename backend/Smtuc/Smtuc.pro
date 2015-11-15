TEMPLATE = lib
TARGET = Smtucbackend
QT += qml quick sql network
CONFIG += qt plugin

load(ubuntu-click)

TARGET = $$qtLibraryTarget($$TARGET)

# Input
SOURCES += \
    backend.cpp \
    database.cpp \
    filedownloader.cpp \
    database_updater_worker.cpp \
    database_updater.cpp \
    sqlquerymodel.cpp \
    settings.cpp \
    database_connection.cpp

HEADERS += \
    backend.h \
    database.h \
    filedownloader.h \
    database_updater_worker.h \
    database_updater.h \
    sqlquerymodel.h \
    settings.h \
    database_connection.h

OTHER_FILES = qmldir

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
testdb.files = testdb
installPath = $${UBUNTU_CLICK_PLUGIN_PATH}/Smtuc
qmldir.path = $$installPath
target.path = $$installPath
INSTALLS += target qmldir


