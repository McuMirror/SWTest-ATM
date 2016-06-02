/**
 * @file camerafit.cpp
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

#include <camerafit.hpp>
#include "ui_camerafit.h"

#include <cameravideosettings.hpp>
#include <cameraimagesettings.hpp>

#include <atm/e1.hpp>
#include <utils/log.hpp>
#include <utils/tools.hpp>

#include <QMediaService>
#include <QMediaRecorder>
#include <QCameraViewfinder>
#include <QMediaMetaData>
#include <QMessageBox>
#include <QPalette>

#include <QtWidgets>

Q_DECLARE_METATYPE(QCameraInfo)

// Start static variable and objects.
bool CameraFit::m_isCameraStarted = false;

CameraFit::CameraFit(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CameraFit),
    m_camera(0),
    m_imageCapture(0),
    m_mediaRecorder(0),
    m_isCapturingImage(false),
    m_applicationExiting(false) {
    ui->setupUi(this);

    // First log class.
    QLOG_INFO() << "Constructor class:" << this->metaObject()->className();

    QActionGroup *videoDevicesGroup = new QActionGroup(this);
    videoDevicesGroup->setExclusive(true);
    foreach (const QCameraInfo &cameraInfo, QCameraInfo::availableCameras()) {
        QAction *videoDeviceAction = new QAction(cameraInfo.description(), videoDevicesGroup);
        videoDeviceAction->setCheckable(true);
        videoDeviceAction->setData(QVariant::fromValue(cameraInfo));
        if (cameraInfo == QCameraInfo::defaultCamera())
            videoDeviceAction->setChecked(true);

        ui->menuDevices->addAction(videoDeviceAction);
    }

    QObject::connect(videoDevicesGroup, SIGNAL(triggered(QAction*)), SLOT(updateCameraDevice(QAction*)));
    QObject::connect(ui->captureWidget, SIGNAL(currentChanged(int)), SLOT(updateCaptureMode()));
}

CameraFit::~CameraFit() {
    delete this->m_mediaRecorder;
    delete this->m_imageCapture;
    delete this->m_camera;
}

void CameraFit::setCamera(const QCameraInfo &cameraInfo) {
    delete this->m_imageCapture;
    delete this->m_mediaRecorder;
    delete this->m_camera;

    CameraFit::m_isCameraStarted = false;

    m_camera = new QCamera(cameraInfo);

    QObject::connect(m_camera, SIGNAL(stateChanged(QCamera::State)),
                     this, SLOT(updateCameraState(QCamera::State)));
    QObject::connect(m_camera, SIGNAL(error(QCamera::Error)),
                     this, SLOT(displayCameraError()));

    m_mediaRecorder = new QMediaRecorder(this->m_camera);
    QObject::connect(m_mediaRecorder, SIGNAL(stateChanged(QMediaRecorder::State)),
                     this, SLOT(updateRecorderState(QMediaRecorder::State)));

    this->m_imageCapture = new QCameraImageCapture(m_camera);

    QObject::connect(m_mediaRecorder, SIGNAL(durationChanged(qint64)),
                     this, SLOT(updateRecordTime()));
    QObject::connect(m_mediaRecorder, SIGNAL(error(QMediaRecorder::Error)),
                     this, SLOT(displayRecorderError()));

    m_mediaRecorder->setMetaData(QMediaMetaData::Title, QVariant(QLatin1String("Test Title")));

    QObject::connect(ui->exposureCompensation, SIGNAL(valueChanged(int)), SLOT(setExposureCompensation(int)));

    m_camera->setViewfinder(ui->viewfinder);

    updateCameraState(m_camera->state());
    updateLockStatus(m_camera->lockStatus(), QCamera::UserRequest);
    updateRecorderState(m_mediaRecorder->state());

    QObject::connect(m_imageCapture, SIGNAL(readyForCaptureChanged(bool)),
	                 this, SLOT(readyForCapture(bool)));
    QObject::connect(m_imageCapture, SIGNAL(imageCaptured(int,QImage)),
                     this, SLOT(processCapturedImage(int,QImage)));
    QObject::connect(m_imageCapture, SIGNAL(imageSaved(int,QString)),
	                 this, SLOT(imageSaved(int,QString)));
    QObject::connect(m_imageCapture, SIGNAL(error(int,QCameraImageCapture::Error,QString)),
	                 this, SLOT(displayCaptureError(int,QCameraImageCapture::Error,QString)));
    QObject::connect(m_camera, SIGNAL(lockStatusChanged(QCamera::LockStatus,QCamera::LockChangeReason)),
                     this, SLOT(updateLockStatus(QCamera::LockStatus,QCamera::LockChangeReason)));

    ui->captureWidget->setTabEnabled(0, (m_camera->isCaptureModeSupported(QCamera::CaptureStillImage)));
    ui->captureWidget->setTabEnabled(1, (m_camera->isCaptureModeSupported(QCamera::CaptureVideo)));

    updateCaptureMode();
    m_camera->start();
}

void CameraFit::changeEvent(QEvent *event) {
    // Event change language.
    if(event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        ui->captureWidget->repaint();
    }
    QWidget::changeEvent(event);
}

void CameraFit::keyPressEvent(QKeyEvent * event) {
    if (event->isAutoRepeat()) {
        return;
    }

    switch (event->key()) {
    case Qt::Key_CameraFocus:
        displayViewfinder();
        m_camera->searchAndLock();
        event->accept();
        break;
    case Qt::Key_Camera:
        if (m_camera->captureMode() == QCamera::CaptureStillImage) {
            takeImage();
        } else {
            if (m_mediaRecorder->state() == QMediaRecorder::RecordingState) {
                stop();
            } else {
                record();
            }
        }
        event->accept();
        break;
    default:
        QMainWindow::keyPressEvent(event);
    }
}

void CameraFit::keyReleaseEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) {
        return;
    }

    switch (event->key()) {
    case Qt::Key_CameraFocus:
        m_camera->unlock();
        break;
    default:
        QMainWindow::keyReleaseEvent(event);
    }
}

void CameraFit::updateRecordTime() {
    QString str = QString("Recorded %1 sec").arg(m_mediaRecorder->duration()/1000);
    ui->statusbar->showMessage(str);
}

void CameraFit::processCapturedImage(int requestId, const QImage& img) {
    Q_UNUSED(requestId);
    QImage scaledImage = img.scaled(ui->viewfinder->size(),
                                    Qt::KeepAspectRatio,
                                    Qt::SmoothTransformation);

    ui->lastImagePreviewLabel->setPixmap(QPixmap::fromImage(scaledImage));

    // Display captured image for 4 seconds.
    displayCapturedImage();
    QTimer::singleShot(4000, this, SLOT(displayViewfinder()));
}

void CameraFit::configureCaptureSettings() { 
    switch (m_camera->captureMode()) {
    case QCamera::CaptureStillImage:
        configureImageSettings();
        break;
    case QCamera::CaptureVideo:
        configureVideoSettings();
        break;
    default:
        break;
    }
}

void CameraFit::configureVideoSettings() {
    CameraVideoSettings settingsDialog(m_mediaRecorder);

    settingsDialog.setAudioSettings(m_audioSettings);
    settingsDialog.setVideoSettings(m_videoSettings);
    settingsDialog.setFormat(m_videoContainerFormat);

    if (settingsDialog.exec()) {
        m_audioSettings = settingsDialog.audioSettings();
        m_videoSettings = settingsDialog.videoSettings();
        m_videoContainerFormat = settingsDialog.format();
        m_mediaRecorder->setEncodingSettings(m_audioSettings, m_videoSettings, m_videoContainerFormat);
    }
}

void CameraFit::configureImageSettings() {
    CameraImageSettings settingsDialog(this->m_imageCapture);

    settingsDialog.setImageSettings(m_imageSettings);

    if (settingsDialog.exec()) {
        m_imageSettings = settingsDialog.imageSettings();
        this->m_imageCapture->setEncodingSettings(m_imageSettings);
    }
}

void CameraFit::record() {
    m_mediaRecorder->record();
    updateRecordTime();
}

void CameraFit::pause() {
    m_mediaRecorder->pause();
}

void CameraFit::stop() {
    m_mediaRecorder->stop();
}

void CameraFit::setMuted(bool muted) {
    m_mediaRecorder->setMuted(muted);
}

void CameraFit::toggleLock() {
    switch (this->m_camera->lockStatus()) {
    case QCamera::Searching:
    case QCamera::Locked:
        this->m_camera->unlock();
        break;
    case QCamera::Unlocked:
        this->m_camera->searchAndLock();
    }
}

void CameraFit::updateLockStatus(QCamera::LockStatus status, QCamera::LockChangeReason reason) {
    QColor indicationColor = Qt::black;

    switch (status) {
    case QCamera::Searching:
        indicationColor = Qt::yellow;
        ui->statusbar->showMessage(QObject::tr("Focusing..."));
        ui->lockButton->setText(QObject::tr("Focusing..."));
        break;
    case QCamera::Locked:
        indicationColor = Qt::darkGreen;
        ui->lockButton->setText(QObject::tr("Unlock"));
        ui->statusbar->showMessage(QObject::tr("Focused"), 2000);
        break;
    case QCamera::Unlocked:
        indicationColor = reason == QCamera::LockFailed ? Qt::red : Qt::black;
        ui->lockButton->setText(QObject::tr("Focus"));
        if (reason == QCamera::LockFailed)
            ui->statusbar->showMessage(QObject::tr("Focus Failed"), 2000);
    }

    QPalette palette = ui->lockButton->palette();
    palette.setColor(QPalette::ButtonText, indicationColor);
    ui->lockButton->setPalette(palette);
}

void CameraFit::takeImage() {
    this->m_isCapturingImage = true;
    this->m_imageCapture->capture();
}

void CameraFit::displayCaptureError(int id, const QCameraImageCapture::Error error, const QString &errorString) {
    Q_UNUSED(id);
    Q_UNUSED(error);
    QMessageBox::warning(this, QObject::tr("Image capture error."), errorString);
    this->m_isCapturingImage = false;
}

void CameraFit::startCamera() {
    this->m_camera->start();
}

void CameraFit::stopCamera() {
    this->m_camera->stop();
}

void CameraFit::updateCaptureMode() {
    int tabIndex = ui->captureWidget->currentIndex();
    QCamera::CaptureModes captureMode = tabIndex == 0 ? QCamera::CaptureStillImage : QCamera::CaptureVideo;

    if (this->m_camera->isCaptureModeSupported(captureMode))
        this->m_camera->setCaptureMode(captureMode);
}

void CameraFit::updateCameraState(QCamera::State state) {
    switch (state) {
    case QCamera::ActiveState:
        ui->actionStartCamera->setEnabled(false);
        ui->actionStopCamera->setEnabled(true);
        ui->captureWidget->setEnabled(true);
        ui->actionSettings->setEnabled(true);
        CameraFit::m_isCameraStarted = true;
        break;
    case QCamera::UnloadedState:
        break;
    case QCamera::LoadedState:
        ui->actionStartCamera->setEnabled(true);
        ui->actionStopCamera->setEnabled(false);
        ui->captureWidget->setEnabled(false);
        ui->actionSettings->setEnabled(false);
        break;
    }
}

bool CameraFit::isCameraStarted() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();

    if (CameraFit::m_isCameraStarted) {
        atm::E1::insertLogAutoCamera("cameraStarted+AUTO=PASSED");
    } else {
        atm::E1::insertLogAutoCamera("cameraStarted+AUTO=FAILED");
    }

    elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
    atm::E1::insertLogAutoCamera("cameraStarted+AUTO+ELAPSEDTIMER=" + QString::number(elapsedtimer));

    return CameraFit::m_isCameraStarted;
}

void CameraFit::updateRecorderState(QMediaRecorder::State state) {
    switch (state) {
    case QMediaRecorder::StoppedState:
        ui->recordButton->setEnabled(true);
        ui->pauseButton->setEnabled(true);
        ui->stopButton->setEnabled(false);
        break;
    case QMediaRecorder::PausedState:
        ui->recordButton->setEnabled(true);
        ui->pauseButton->setEnabled(false);
        ui->stopButton->setEnabled(true);
        break;
    case QMediaRecorder::RecordingState:
        ui->recordButton->setEnabled(false);
        ui->pauseButton->setEnabled(true);
        ui->stopButton->setEnabled(true);
        break;
    }
}

void CameraFit::setExposureCompensation(int index) {
    this->m_camera->exposure()->setExposureCompensation(index*0.5);
}

void CameraFit::displayRecorderError() {
    QMessageBox::warning(this, QObject::tr("Capture error"), this->m_mediaRecorder->errorString());
}

void CameraFit::displayCameraError() {
    QMessageBox::warning(this, QObject::tr("Camera error"), this->m_camera->errorString());
}

void CameraFit::updateCameraDevice(QAction *action) {
    this->setCamera(qvariant_cast<QCameraInfo>(action->data()));
}

void CameraFit::displayViewfinder() {
    ui->stackedWidget->setCurrentIndex(0);
}

void CameraFit::displayCapturedImage() {
    ui->stackedWidget->setCurrentIndex(1);
}

void CameraFit::readyForCapture(bool ready) {
    ui->takeImageButton->setEnabled(ready);
}

void CameraFit::imageSaved(int id, const QString &fileName) {
    Q_UNUSED(id);
    Q_UNUSED(fileName);

    this->m_isCapturingImage = false;
    if (m_applicationExiting)
        close();
}

void CameraFit::closeEvent(QCloseEvent *event) {
    if (this->m_isCapturingImage) {
        setEnabled(false);
        m_applicationExiting = true;
        event->ignore();
    } else {
        event->accept();
    }
}
