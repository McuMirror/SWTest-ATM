/**
 * @file cameravideosettings.hpp
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

#ifndef CAMERA_VIDEOSETTINGS_HPP
#define CAMERA_VIDEOSETTINGS_HPP

#include <QObject>
#include <QDialog>
#include <QAudioEncoderSettings>
#include <QVideoEncoderSettings>
#include <QMediaRecorder>

// Override!
class QComboBox;
class QMediaRecorder;

namespace Ui {
    class CameraVideoSettings;
}

/**
 * @class CameraVideoSettings
 *
 * @brief Widget UI configure image settings.
 *
 * @details This class contains the UI, used to configure video
 * settings.
 */
class CameraVideoSettings : public QDialog {

    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param mediaRecorder - Media recorder.
     * @param parent - Some widget.
     */
    CameraVideoSettings(QMediaRecorder *mediaRecorder, QWidget *parent = 0);

    /**
     * Destructor.
     */
    ~CameraVideoSettings();

    /**
     * Store the audio settings.
     *
     * @return Return the audio settings.
     */
    QAudioEncoderSettings audioSettings() const;

    /**
     * Set audio configuration.
     *
     * @param audioSettings - The audio settings.
     */
    void setAudioSettings(const QAudioEncoderSettings &audioSettings);

    /**
     * Store the video settings.
     *
     * @return Return the audio settings.
     */
    QVideoEncoderSettings videoSettings() const;

    /**
     * Set image encode configuration..
     *
     * @return Return the image encode settings.
     */
    void setVideoSettings(const QVideoEncoderSettings&);

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
    QVariant boxValue(const QComboBox*) const;

    /**
     * Select comboBox items.
     *
     * @param box - ComboBox.
     * @param value - Data value.
     */
    void selectComboBoxItem(QComboBox *box, const QVariant &value);

    /// Store instance (ui interface).
    Ui::CameraVideoSettings *ui;

    /// Store instance media recorder.
    QMediaRecorder *m_mediaRecorder;
};

#endif // CAMERA_VIDEOSETTINGS_HPP
