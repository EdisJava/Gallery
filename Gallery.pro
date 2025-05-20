TEMPLATE = subdirs
CONFIG += c++17

SUBDIRS += \
    gallery-core \
    gallery-desktop


# Definir dependencias
gallery-desktop.depends = gallery-core

