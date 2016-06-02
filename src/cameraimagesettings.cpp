/**
 * @file cameraimagesettings.cpp
 *
 * Definitions for a SWTest-ATM.
 *
 * ATM for banks Project.
 *
 * Copyright (c) 2015-2016 BlueSalute Company.
 *
 * @author Ederson Moura - <ederson.moura@bluesalute.com>
 *
 * $Id: Exp$
 */

#include <cameraimagesettings.hpp>
#include "ui_cameraimagesettings.h"

#include <utils/log.hpp>
#include <utils/tools.hpp>

#include <QComboBox>
#include <QDebug>
#include <QCameraImageCapture>
#include <QMediaService>

CameraImageSettings::CameraImageSettings(QCameraImageCapture *imageCapture, QWidget *parent) :
    QDialog(parent), ui(new Ui::CameraImageSettings), m_imagecapture(imageCapture) {

    // First log class.
    QLOG_INFO() << "Constructor class:" << this->metaObject()->className();

    ui->setupUi(this);

    // Image codecs.
    ui->imageCodecBox->addItem(tr("Default image format"), QVariant(QString()));
    foreach(const QString &codecName, m_imagecapture->supportedImageCodecs()) {
        QString description = m_imagecapture->imageCodecDescription(codecName);
        ui->imageCodecBox->addItem(codecName+": "+description, QVariant(codecName));
    }

    ui->imageQualitySlider->setRange(0, int(QMultimedia::VeryHighQuality));

    ui->imageResolutionBox->addItem(QObject::tr("Default Resolution"));
    QList<QSize> supportedResolutions = m_imagecapture->supportedResolutions();
    foreach(const QSize &resolution, supportedResolutions) {
        ui->imageResolutionBox->addItem(QString("%1x%2").arg(resolution.width()).arg(resolution.height()),
                                        QVariant(resolution));
    }
}

CameraImageSettings::~CameraImageSettings() {
    delete ui;
}

void CameraImageSettings::changeEvent(QEvent *event) {
    QDialog::changeEvent(event);
    switch (event->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QImageEncoderSettings CameraImageSettings::imageSettings() const {
    QImageEncoderSettings settings = m_imagecapture->encodingSettings();
    settings.setCodec(boxValue(ui->imageCodecBox).toString());
    settings.setQuality(QMultimedia::EncodingQuality(ui->imageQualitySlider->value()));
    settings.setResolution(boxValue(ui->imageResolutionBox).toSize());

    return settings;
}

void CameraImageSettings::setImageSettings(const QImageEncoderSettings &CameraImageSettings) {
    selectComboBoxItem(ui->imageCodecBox, QVariant(CameraImageSettings.codec()));
    selectComboBoxItem(ui->imageResolutionBox, QVariant(CameraImageSettings.resolution()));
    ui->imageQualitySlider->setValue(CameraImageSettings.quality());
}

QVariant CameraImageSettings::boxValue(const QComboBox *box) const {
    int idx = box->currentIndex();
    if (idx == -1)
        return QVariant();

    return box->itemData(idx);
}

void CameraImageSettings::selectComboBoxItem(QComboBox *box, const QVariant &value) {
    for (register int i = 0; i < box->count(); ++i) {
        if (box->itemData(i) == value) {
            box->setCurrentIndex(i);
            break;
        }
    }
}
