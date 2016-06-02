/**
 * @file cameravideosettings.cpp
 *
 * Implementations for a SWTest-ATM.
 *
 * ATM for banks Project.
 *
 * Copyright (c) 2015-2016 BlueSalute Company.
 *
 * @author Ederson Moura - <ederson.moura@bluesalute.com>
 *
 * $Id: Exp$
 */

#include <cameravideosettings.hpp>
#include "ui_cameravideosettings.h"

#include <utils/log.hpp>
#include <utils/tools.hpp>

#include <QComboBox>
#include <QDebug>
#include <QMediaRecorder>
#include <QMediaService>

CameraVideoSettings::CameraVideoSettings(QMediaRecorder *mediaRecorder, QWidget *parent) :
    QDialog(parent), ui(new Ui::CameraVideoSettings), m_mediaRecorder(mediaRecorder) {

    // First log class.
    QLOG_INFO() << "Constructor class:" << this->metaObject()->className();

    // Set up.
    ui->setupUi(this);

    // Audio codecs.
    ui->audioCodecBox->addItem(QObject::tr("Default audio codec"), QVariant(QString()));
    foreach (const QString &codecName, mediaRecorder->supportedAudioCodecs()) {
        QString description = mediaRecorder->audioCodecDescription(codecName);
        ui->audioCodecBox->addItem(codecName+": "+description, QVariant(codecName));
    }

    // Rates.
    foreach (int sampleRate, mediaRecorder->supportedAudioSampleRates()) {
        ui->audioSampleRateBox->addItem(QString::number(sampleRate), QVariant(sampleRate));
    }

    ui->audioQualitySlider->setRange(0, int(QMultimedia::VeryHighQuality));

    // Video codecs.
    ui->videoCodecBox->addItem(tr("Default video codec"), QVariant(QString()));
    foreach (const QString &codecName, mediaRecorder->supportedVideoCodecs()) {
        QString description = mediaRecorder->videoCodecDescription(codecName);
        ui->videoCodecBox->addItem(codecName+": "+description, QVariant(codecName));
    }

    ui->videoQualitySlider->setRange(0, int(QMultimedia::VeryHighQuality));

    ui->videoResolutionBox->addItem(tr("Default"));
    QList<QSize> supportedResolutions = mediaRecorder->supportedResolutions();
    foreach (const QSize &resolution, supportedResolutions) {
        ui->videoResolutionBox->addItem(QString("%1x%2").arg(resolution.width()).arg(resolution.height()),
                                        QVariant(resolution));
    }

    ui->videoFramerateBox->addItem(tr("Default"));
    QList<qreal> supportedFrameRates = mediaRecorder->supportedFrameRates();
    qreal rate;
    foreach (rate, supportedFrameRates) {
        QString rateString = QString("%1").arg(rate, 0, 'f', 2);
        ui->videoFramerateBox->addItem(rateString, QVariant(rate));
    }

    // Containers.
    ui->containerFormatBox->addItem(tr("Default container"), QVariant(QString()));
    foreach (const QString &format, mediaRecorder->supportedContainers()) {
        ui->containerFormatBox->addItem(format+":"+mediaRecorder->containerDescription(format),
                                        QVariant(format));
    }
}

CameraVideoSettings::~CameraVideoSettings() {
    delete ui;
}

void CameraVideoSettings::changeEvent(QEvent *event) {
    QDialog::changeEvent(event);
    switch (event->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QAudioEncoderSettings CameraVideoSettings::audioSettings() const {
    QAudioEncoderSettings settings = this->m_mediaRecorder->audioSettings();
    settings.setCodec(boxValue(ui->audioCodecBox).toString());
    settings.setQuality(QMultimedia::EncodingQuality(ui->audioQualitySlider->value()));
    settings.setSampleRate(boxValue(ui->audioSampleRateBox).toInt());
    return settings;
}

void CameraVideoSettings::setAudioSettings(const QAudioEncoderSettings &audioSettings) {
    selectComboBoxItem(ui->audioCodecBox, QVariant(audioSettings.codec()));
    selectComboBoxItem(ui->audioSampleRateBox, QVariant(audioSettings.sampleRate()));
    ui->audioQualitySlider->setValue(audioSettings.quality());
}

QVideoEncoderSettings CameraVideoSettings::videoSettings() const {
    QVideoEncoderSettings settings = this->m_mediaRecorder->videoSettings();
    settings.setCodec(boxValue(ui->videoCodecBox).toString());
    settings.setQuality(QMultimedia::EncodingQuality(ui->videoQualitySlider->value()));
    settings.setResolution(boxValue(ui->videoResolutionBox).toSize());
    settings.setFrameRate(boxValue(ui->videoFramerateBox).value<qreal>());

    return settings;
}

void CameraVideoSettings::setVideoSettings(const QVideoEncoderSettings &CameraVideoSettings) {
    selectComboBoxItem(ui->videoCodecBox, QVariant(CameraVideoSettings.codec()));
    selectComboBoxItem(ui->videoResolutionBox, QVariant(CameraVideoSettings.resolution()));
    ui->videoQualitySlider->setValue(CameraVideoSettings.quality());

    // Special case for frame rate.
    for (register int i = 0; i < ui->videoFramerateBox->count(); ++i) {
        qreal itemRate = ui->videoFramerateBox->itemData(i).value<qreal>();
        if (qFuzzyCompare(itemRate, CameraVideoSettings.frameRate())) {
            ui->videoFramerateBox->setCurrentIndex(i);
            break;
        }
    }
}

QString CameraVideoSettings::format() const {
    return boxValue(ui->containerFormatBox).toString();
}

void CameraVideoSettings::setFormat(const QString &format) {
    selectComboBoxItem(ui->containerFormatBox, QVariant(format));
}

QVariant CameraVideoSettings::boxValue(const QComboBox *box) const {
    int idx = box->currentIndex();
    if (idx == -1)
        return QVariant();

    return box->itemData(idx);
}

void CameraVideoSettings::selectComboBoxItem(QComboBox *box, const QVariant &value) {
    for (register int i = 0; i < box->count(); ++i) {
        if (box->itemData(i) == value) {
            box->setCurrentIndex(i);
            break;
        }
    }
}
