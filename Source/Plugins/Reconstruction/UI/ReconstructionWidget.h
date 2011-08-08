/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef RECONSTRUCTIONWIDGET_H_
#define RECONSTRUCTIONWIDGET_H_

#include <QtCore/QString>
#include <QtCore/QSettings>

#include "ui_ReconstructionWidget.h"

#include "QtSupport/AIMPluginFrame.h"
#include "QReconstruction.h"



/**
 * @class ReconstructionWidget ReconstructionWidget.h Plugins/Reconstruction/UI/ReconstructionWidget.h
 * @brief This class represents the User Interface for the Reconstruction module
 * of the AIMRepresentation program. The user interface subclasses QFrame which
 * should make it able to be embedded in most Qt applications.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 30, 2011
 * @version 1.0
 */
class ReconstructionWidget : public AIMPluginFrame, private Ui::ReconstructionWidget
{
  Q_OBJECT
  public:
    ReconstructionWidget(QWidget *parent = 0);
    virtual ~ReconstructionWidget();

    /**
      * @brief Reads the Preferences from the users pref file
      */
    virtual void readSettings(QSettings &prefs);

     /**
      * @brief Writes the preferences to the users pref file
      */
    virtual void writeSettings(QSettings &prefs);

    /**
     * @brief Enables or Disables all the widgets in a list
     * @param b
     */
    virtual void setWidgetListEnabled(bool b);

    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    virtual void setupGui();


    virtual void checkIOFiles();

#if 0
    /**
     * @brief Returns the EBSD Collection System manufacturer.
     * @param ebsdFile The path to the HDF5 based File to look for the manufacturer
     * at the path /Manufacturer
     */
    virtual std::string getEbsdManufacturer(const std::string &ebsdFile);
#endif


  protected slots:

    /* Reconstruction Slots */
    void on_m_OIMH5Btn_clicked();
    void on_m_OutputDirBtn_clicked();
  //  void on_m_AlreadyFormed_stateChanged(int);
    void on_m_GoBtn_clicked();
    void on_m_SaveSettingsBtn_clicked();
    void on_m_LoadSettingsBtn_clicked();

    void on_m_H5InputFile_textChanged(const QString &text);

    void on_m_OutputDir_textChanged(const QString &text);
    void on_m_OutputFilePrefix_textChanged(const QString &text);

    void on_m_VtkOptionsBtn_clicked();

    void on_m_HDF5GrainFile_stateChanged(int state);

    /**
     *
     */
    void m_SetSliceInfo();

    private slots:
      // slots for our worker thread to communicate
      virtual void threadHasMessage(QString message);

      /* Reconstruction Thread communicates throught these methods */
      virtual void threadFinished();
      virtual void threadProgressed(int value);
      void phaseTypeEdited(int i);

  private:
    QList<QWidget*>             m_WidgetList;
    QReconstruction*            m_Reconstruction;
    QThread*                    m_WorkerThread;
    bool                        m_phaseTypeEdited;
    bool                        m_WritePhaseIdScalars;
    bool                        m_WriteImageQualityScalars;
    bool                        m_WriteIPFColorScalars;
    bool                        m_WriteBinaryVTKFile;

    QString m_OpenDialogLastDirectory;

    bool checkPhaseTypes();

    ReconstructionWidget(const ReconstructionWidget&); // Copy Constructor Not Implemented
    void operator=(const ReconstructionWidget&); // Operator '=' Not Implemented
};

#endif /* RECONSTRUCTIONWIDGET_H_ */
