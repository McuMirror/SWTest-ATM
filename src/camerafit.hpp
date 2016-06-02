/**
 * @file camerafit.hpp
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

#ifndef CAMERAFIT_HPP
#define CAMERAFIT_HPP

#include <QMainWindow>
#include <QCamera>
#include <QCameraImageCapture>
#include <QMediaRecorder>
#include <QCameraInfo>

// Override!
namespace Ui {
    class CameraFit;
}

/**
 * @class CameraFit
 *
 * @brief Widget UI shows image camera.
 *
 * @details This class contains the UI, it executes commands related the
 * Camera device.
 */
class CameraFit : public QMainWindow {

    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param parent - The widget parent.
     */
    CameraFit(QWidget *parent = 0);

    /**
     * Destructor.
     */
    ~CameraFit();

public slots:
    /**
     * Set camera.
     *
     * Eg. Use: QCameraInfo::defaultCamera()
     *
     * @param cameraInfo - The camera information.
     */
    void setCamera(const QCameraInfo &cameraInfo);

    /**
     * Check if camera actived.
     *
     * @return Return truu if camera activated; otherwize false.
     */
    static bool isCameraStarted();

private slots:
    /**
     * Start camera.
     */
    void startCamera();

    /**
     * Stop camera.
     */
    void stopCamera();

    /**
     * Record camera.
     */
    void record();

    /**
     * Pause camera.
     */
    void pause();

    /**
     * Stop camera.
     */
    void stop();

    /**
     * Set camera (audio) muted.
     *
     * @param muted - Set true for muted camera.
     */
    void setMuted(bool muted);

    /**
     * Camera toogle (lock).
     */
    void toggleLock();

    /**
     * Camera take (image).
     */
    void takeImage();

    /**
     * Get display capture errors.
     *
     * @param id - Number id error.
     * @param error - Type error.
     * @param errorString - String error.
     */
    void displayCaptureError(int id, QCameraImageCapture::Error error, const QString &errorString);

    /**
     * Configure capture settings.
     */
    void configureCaptureSettings();

    /**
     * Configure video settings.
     */
    void configureVideoSettings();

    /**
     * Configure image settings.
     */
    void configureImageSettings();

    /**
     * Show display record error.
     */
    void displayRecorderError();

    /**
     * Show display camera error.
     */
    void displayCameraError();

    /**
     * Update camera device.
     *
     * @param action - Action pointer.
     */
    void updateCameraDevice(QAction *action);

    /**
     * Update camera state.
     *
     * @param state - Camera type state: Activate, Loaded, Unloaded.
     */
    void updateCameraState(QCamera::State state);

    /**
     * Update capture mode.
     */
    void updateCaptureMode();

    /**
     * Update recorder state.
     *
     * @param state - Media record state: Stop, pause, record.
     */
    void updateRecorderState(QMediaRecorder::State state);

    /**
     * Set camera exposure.
     *
     * @param index - Number exposure.
     */
    void setExposureCompensation(int index);

    /**
     * Update record time in status bar.
     */
    void updateRecordTime();

    /**
     * Display captured image (screenshot).
     *
     * @param requestId - Number ID.
     * @param img - Name image.
     */
    void processCapturedImage(int requestId, const QImage &img);

    /**
     * Update lock status.
     *
     * @param status - Status: Search, lock, unlock.
     * @param reason - Color indicator.
     */
    void updateLockStatus(QCamera::LockStatus status, QCamera::LockChangeReason reason);

    /**
     * Display view finder.
     */
    void displayViewfinder();

    /**
     * Display captured image.
     */
    void displayCapturedImage();

    /**
     * Set enable or disable take button capture.
     *
     * @param ready - If true enable button; if false disable button.
     */
    void readyForCapture(bool ready);

    /**
     * @brief imageSaved
     * @param id
     * @param fileName
     */
    void imageSaved(int id, const QString &fileName);

protected:
    /**
     * Reimplemented method, triggered automatically when the widget
     * is closed.
     *
     * @param event - Some event.
     */
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

    /**
     * Reimplemented method, triggered automatically when the widget
     * is changed.
     *
     * @param event - Some event.
     */
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;

    /**
     * Key events are sent to the widget with keyboard input focus
     * when keys are pressed or released.
     *
     * @param event - Some event.
     */
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

    /**
     * Key events are sent to the widget with keyboard input focus
     * when keys are pressed or released.
     *
     * @param event - Some event.
     */
    void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private:
    /// Store instance (ui interface).
    Ui::CameraFit *ui;

    /// Store instance camera.
    QCamera *m_camera;

    /// Store instance image capture.
    QCameraImageCapture *m_imageCapture;

    /// Store instance media recorder.
    QMediaRecorder* m_mediaRecorder;

    /// Store instance image settigns.
    QImageEncoderSettings m_imageSettings;

    /// Store instance audio settigns.
    QAudioEncoderSettings m_audioSettings;

    /// Store instance vide settigns.
    QVideoEncoderSettings m_videoSettings;

    /// Store instance image format.
    QString m_videoContainerFormat;

    /// Check capture image.
    bool m_isCapturingImage;

    /// Check application exit.
    bool m_applicationExiting;

    /// Check if camera started.
    static bool m_isCameraStarted;
};

#endif // CAMERAFIT_HPP
