/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Nathan Osman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 **/

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QMessageBox>
#include <QFileDialog>

#ifdef BUILD_UPDATECHECKER
#include "updatechecker.h"
#endif

#include "../util/settings.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    // General
    ui->lnEdtdeviceName->setText(Settings::get(Settings::DeviceName).toString());

#ifdef BUILD_UPDATECHECKER
    int updateInterval = Settings::get(Settings::UpdateInterval).toInt();
    bool checkForUpdates = (updateInterval != 0);
    ui->lblUpdateInterval->setEnabled(checkForUpdates);
    ui->spnBoxUpdateInterval->setEnabled(checkForUpdates);
    ui->chkBoxCheckUpdates->setChecked(checkForUpdates);
    ui->spnBoxUpdateInterval->setValue(updateInterval / Settings::Hour);
#else
    ui->chkBoxCheckUpdates->setVisible(false);
    ui->lblUpdateInterval->setVisible(false);
    ui->spnBoxUpdateInterval->setVisible(false);
#endif

    // Transfer
    ui->spnBoxBuffer->setValue(Settings::get(Settings::TransferBuffer).toInt() / Settings::Kb);
    ui->lnEdtDirectory->setText(Settings::get(Settings::TransferDirectory).toString());
    ui->spnBoxTransferPort->setValue(Settings::get(Settings::TransferPort).toLongLong());

    // Broadcast
    ui->spnBoxBroadcastPort->setValue(Settings::get(Settings::BroadcastPort).toLongLong());
    ui->spnBoxBroadcastTimeout->setValue(Settings::get(Settings::BroadcastTimeout).toInt() / Settings::Second);
    ui->spnBoxBroadcastInterval->setValue(Settings::get(Settings::BroadcastInterval).toInt() / Settings::Second);

    connect(ui->pshBtnSelectDir, &QPushButton::clicked,
            this, &SettingsDialog::onBtnSelectDirClicked);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::accept()
{
    // General
    Settings::set(Settings::DeviceName, ui->lnEdtdeviceName->text());

#ifdef BUILD_UPDATECHECKER
    if (ui->chkBoxCheckUpdates->isChecked()) {
        Settings::set(Settings::UpdateInterval, ui->spnBoxUpdateInterval->value() * Settings::Hour);
        Q_EMIT configureUpdateChecker();
    } else {
        Settings::set(Settings::UpdateInterval, 0);
        UpdateChecker::deleteInstance();
    }
#endif

    // Transfer
    Settings::set(Settings::TransferDirectory, ui->lnEdtDirectory->text());
    Settings::set(Settings::TransferPort, ui->spnBoxTransferPort->value());
    Settings::set(Settings::TransferBuffer, ui->spnBoxBuffer->value() * Settings::Kb);

    // Broadcast
    Settings::set(Settings::BroadcastPort, ui->spnBoxBroadcastPort->value());
    Settings::set(Settings::BroadcastTimeout, ui->spnBoxBroadcastTimeout->value() * Settings::Second);
    Settings::set(Settings::BroadcastInterval, ui->spnBoxBroadcastInterval->value() * Settings::Second);

    QDialog::accept();
}

void SettingsDialog::onBtnResetClicked()
{
    QMessageBox::StandardButton response = QMessageBox::question(this,
                                                                 "Reset settings",
                                                                 "Are you sure you want to reset "
                                                                 "all settings to their default "
                                                                 "values? This cannot be undone.");
    if (response == QMessageBox::Yes) {
        Settings::reset();
#ifdef BUILD_UPDATECHECKER
        Q_EMIT configureUpdateChecker();
#endif
        this->reject();
    }
}

void SettingsDialog::onBtnSelectDirClicked()
{
    QString path(QFileDialog::getExistingDirectory(this, tr("Select Directory")));

    if(!path.isNull())
        ui->lnEdtDirectory->setText(path);
}