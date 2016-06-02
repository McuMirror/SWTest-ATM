/**
 * @file cameraimagesettings.hpp
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

#ifndef CAMERA_IMAGESETTINGS_HPP
#define CAMERA_IMAGESETTINGS_HPP

#include <QDialog>
#include <QAudioEncoderSettings>
#include <QVideoEncoderSettings>
#include <QObject>
#include <QCameraImageCaptureControl>

// Override!
class QComboBox;
class QCameraImageCapture;

// Override!
namespace Ui {
    class CameraImageSettings;
}

/**
 * @class CameraImageSettings
 *
 * @brief Widget UI configure image settings.
 *
 * @details This class contains the UI, used to configure image
 * settings.
 */
class CameraImageSettings : public QDialog {

    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param imageCapture - Camera image capture.
     * @param parent - Some widget.
     */
    CameraImageSettings(QCameraImageCapture *imageCapture, QWidget *parent = 0);

    /**
     * Destructor.
     */
    ~CameraImageSettings();

    /**
     * Store the audio settings.
     *
     * @return Return the audio settings.
     */
    QAudioEncoderSettings audioSettings() const;

    /**
     * Set audio configuration.
     *
     * @param settings - The audio settings.
     */
    void setAudioSettings(const QAudioEncoderSettings &settings);

    /**
     * Store the image settings.
     *
     * @return Return the image settings.
     */
    QImageEncoderSettings imageSettings() const;

    /**
     * Set image encode configuration..
     *
     * @return Return the image encode settings.
     */
    void setImageSettings(const QImageEncoderSettings &settings);

    /**
     * Print format encode.
     *
     * @return The format encode.
     */
    QString format() const;

    /**
     * Set format encode.
     *
     * @param format - Type format.
     */
    void setFormat(const QString &format);

protected:
    /**
     * Reimplemented method, triggered automatically when the widget
     * is changed.
     *
     * @param event - Some event.
     */
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;

private:
    /**
     * Box value.
     *
     * @param box - ComboBox.
     * @return Return data.
     */
    QVariant boxValue(const QComboBox *box) const;

    /**
     * Select comboBox items.
     *
     * @param box - ComboBox.
     * @param value - Data value.
     */
    void selectComboBoxItem(QComboBox *box, const QVariant &value);

    /// Store instance (ui interface).
    Ui::CameraImageSettings *ui;

    /// Store instance camera image capture.
    QCameraImageCapture *m_imagecapture;
};

#endif // CAMERA_IMAGESETTINGS_HPP
