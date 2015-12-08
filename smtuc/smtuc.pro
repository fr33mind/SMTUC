TEMPLATE = aux
TARGET = smtuc

RESOURCES += smtuc.qrc

QML_FILES += Main.qml
UI_FILES +=  ui/StopsPopup.qml \
    ui/TicketsPage.qml \
    ui/Popup.qml \
    ui/MessagePopup.qml \
    ui/ObservationsPopup.qml \
    ui/RoutePage.qml \
    ui/StopsPopup.qml \
    ui/RoutesPage.qml \
    ui/TicketsPage.qml \
    ui/OutletsPage.qml \
    ui/OutletPopup.qml \
    ui/CategoryItem.qml \
    ui/UpdateDialog.qml \
    ui/AboutPage.qml

QML_FILES += $$files(*.qml,true) \
             $$files(*.js,true)

CONF_FILES += smtuc.apparmor \
              smtuc.desktop \
              smtuc.png

OTHER_FILES += $${CONF_FILES} \
               $${QML_FILES}

IMAGE_FILES += images/bus.svg

#copy database file
DB_FILENAME = smtuc.sqlite
!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_db.target = $$OUT_PWD/$${DB_FILENAME}
    copy_db.depends = $$_PRO_FILE_PWD_/$${DB_FILENAME}
    copy_db.commands = $(COPY_FILE) \"$$replace(copy_db.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_db.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_db
    PRE_TARGETDEPS += $$copy_db.target
}

db_file.path = /smtuc
db_file.files += $${DB_FILENAME}

#specify where the qml/js files are installed to
qml_files.path = /smtuc
qml_files.files += $${QML_FILES}

#specify where the config files are installed to
config_files.path = /smtuc
config_files.files += $${CONF_FILES}

ui_files.path = /smtuc/ui
ui_files.files += $${UI_FILES}

image_files.path = /smtuc/images
image_files.files += $${IMAGE_FILES}

INSTALLS+=config_files qml_files db_file ui_files image_files

