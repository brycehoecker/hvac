///////////////////////////////////////////////////////////////////////////////
// $Id: VHVFrame.h,v 1.3 2006/03/28 04:50:53 humensky Exp $
//
// Filename : 
// Created  : Tue Mar 19 2002 by Jim Hinton
///////////////////////////////////////////////////////////////////////////////
#ifndef VHVFRAME_H
#define VHVFRAME_H

/** \file VHVFrame.h 
 *  \brief Header file for classes in VHVFrame.cpp
*/
#include <vector>
#include <bitset>
#include <ctime>
#include <unistd.h>
#include <sys/time.h>

#include <qpopupmenu.h>
#include <qmainwindow.h>
#include <qintdict.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qtextedit.h>
#include <qbuttongroup.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qspinbox.h>
#include <qslider.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qlcdnumber.h> 
#include <qtimer.h>
#include <qdialog.h>
#include <qcheckbox.h>
#include <qlistbox.h>
#include <qtabwidget.h>

#include <VDBHighVoltage.h>
#include <VDBPositioner.h>

//#include "VAAzElRADecXY.h"
//#include "VATime.h"
//#include "VACommon.h"

#include "VHVData.h"
#include "VHVControl.h"
#include "VHelpWindow.h"
#include "VNETFunctionsServerThread.h"
#include "Vcorba.h"
#include "exception.h"
#include "astrometry.h"

/** \enum EnumMenu
 * Enumeration of menu selections 
 */
enum EnumMenu { MENU_DOWN, MENU_UP, MENU_VOLTS, MENU_LOAD,
  MENU_SAVE, MENU_EXIT, MENU_TELESCOPE_EXIT, MENU_HVAC_EXIT, 
		MENU_SELECT, MENU_ALLON,
  MENU_ALLOFF, MENU_RESTORE_ALL, MENU_SUPPRESS_ALL, MENU_REPORT, MENU_NO_OPT,
  MENU_SHOW_STARS, MENU_HIDE_STARS, MENU_SHOW_ARCS,
  MENU_HIDE_ARCS, MENU_CHANGE_MAG, MENU_SUPPRESS, MENU_RESTORE,
  MENU_LOGGING_ON, MENU_LOGGING_OFF, MENU_LOGGING_PERIOD, MENU_SCALE_VOLTAGES,
  MENU_OFFSET_VOLTAGES, MENU_SET_ALL_VOLTAGES, MENU_AUTORESPONSE,
		MENU_READ_DB, MENU_WRITE_DB, MENU_READ_MAP, MENU_WRITE_MAP,
		MENU_BROKEN, MENU_UNBROKEN, MENU_RESTORE_RECENT,
		MENU_FLATFIELD, MENU_YESAUTO, MENU_NOAUTO, MENU_FEEDBACK,
		MENU_GLOBAL_FDBK_ON, MENU_GLOBAL_FDBK_OFF, MENU_PIXEL_FDBK_ON,
		MENU_PIXEL_FDBK_OFF, MENU_GLOBAL_FDBK_RESET,
		MENU_PIXEL_FDBK_RESET, MENU_HILOINNER, MENU_HILOOUTER
};

#define MINMAG 1 /**< Minimum value of star magnitude limit */
#define MAXMAG 7 /**< Maximum value of star magnitude limit */
#define CURRSAMPLES 240 /**< Number of PMT current samples to compare */

class VChannelReport;
class VBoardReport;
class VStarPanel;
class VTelStatus;
class VLED;
class VTimeWidget;
class VAutoResponseDialog;

/**********************************************/

class VHVFrame : public QMainWindow {

Q_OBJECT 

public:
  VHVFrame(QWidget * parent = 0, const char *name = 0, WFlags f = 0);
  virtual ~VHVFrame(); 
  int  getDisplayOpt();
  void contextMenu();
  void menuPmtSelect( int index );
  VPMTIcon *getSelected();
  VHVControl *getControl();
  VHVControl *getControl( int index );
  QTabWidget *getTab() { return fTab; };
  bool getStillInConstructor();


public slots: 
  void help();
  void helpBrowser();
  void about();
  void notYet(); 
  void init();
  void fileMenu(int id);
  void dbMenu(int id);
  void pmtMenu(int id);
  void hvParamsMenu(int id);
  void starMenu(int id);
  void optionButtonClicked(int);
  void pmtSelect(VPMTIcon *, QMouseEvent *, int );
  void contextAction(int id);
  void textInputVoltage();
  void textInputChannel();
  void updateAll();
  void scaleVoltages();
  void offsetVoltages();
  void setAllVoltages();
  void markDisabled();
  void unmarkDisabled();
  void markAsNoAuto();
  void unmarkAsNoAuto();
  void sendMessage(const QString & text);
  void setStatusPeriod();
  void checkCurrentsChanged();

private:
  VHVControl     *fControl[NUMTEL];
  VHVControlView *fView[2*NUMTEL];   
  QWidget        *fCentral;
  QTabWidget     *fTab;
  QTextEdit      *fMessages;
  QButtonGroup   *fBG;
  VChannelReport *fChannelReport;
  VBoardReport   *fBoardReport;
  VStarPanel     *fStarPanel;
  VTelStatus     *fTelStatus;
/*   VLED           *fHVStatusLED; */
/*   VLED           *fCurrentsStatusLED; */
/*   VLED           *fDBLoggingStatusLED; */
  VPMTIcon       *fSelectedPMT;
  QLineEdit      *fLineInput;
  VTimeWidget    *fUpdateTime;
  int             fDisplayOption;
  QTimer         *fTimer;
/*   int             fTransientCheckTime; */
/*   int             fCurrentThreshold; */
  VCamInfo       *fCamInfo[NUMTEL];
  bool            fStillInConstructor;
  VPMTIndicator  *fPMTIndicator[NUMTEL][MAX_PMTS];
  bool            fDisplayNotIPMTFlag; // true=already flagged display not IPMT
  int             fStartTimeNotIPMT;
  int             fUpdatePeriod;
  float           fCurrent[NUMTEL][MAX_PMTS][CURRSAMPLES];
  int             fCurrentWarningTime[NUMTEL];
};

/**********************************************/

class VChannelReport : public QGroupBox {

Q_OBJECT 

public:
    VChannelReport(VHVFrame *, QWidget *, const char *name = 0);
    virtual ~VChannelReport(); 
    void update();

private slots: 
    void voltageChanged(int value);
    void powerChanged();
    
private:
    QSpinBox    *fVoltBox;
    QLabel      *fVSetLabel;
    QLabel      *fVTrueLabel;
    QLabel      *fCurrentLabel;
    QLabel      *fFeedbackLabel;
    QLabel      *fStatusLabel;
    QPushButton *fPowerButton;
    VHVFrame    *fMainFrame;
    
};

/**********************************************/

class VBoardReport : public QGroupBox {

Q_OBJECT 

public:
  VBoardReport(VHVFrame *, QWidget *, const char *name = 0);
  virtual ~VBoardReport(); 
  void update();

private slots: 
  void voltageChanged(int value);
  void currentChanged(int value);
  void rampUpChanged(int value);
  void rampDownChanged(int value);
  void powerChanged();
  void checkboxChanged();

private:
  QSpinBox    *fVoltBox;
  QSpinBox    *fRampUpBox;
  QSpinBox    *fRampDownBox;
  QSpinBox    *fCurrentBox;
  QLabel      *fVTrueLabel;
  QLabel      *fRampUpLabel;
  QLabel      *fRampDownLabel;
  QLabel      *fCurrentLabel;
  QLabel      *fTempLabel;
  QLabel      *fVSetLabel;
  QPushButton *fPowerButton;
  QCheckBox   *fLimitCheckBox;
  VHVFrame    *fMainFrame;
  int         fSlot;

};

/*********************************************/

class VIntValidator:public QValidator {

public:
  VIntValidator(QWidget * parent, const char *name, int min, int max);
  virtual ~VIntValidator();
  QValidator::State validate(QString &, int &) const;
  void setup(int min, int max);

private:
  int fMin;
  int fMax;
  
};

/*********************************************/

class VFloatValidator:public QValidator {
  
 public:
  VFloatValidator(QWidget * parent, const char *name, double min, double max);
  virtual ~VFloatValidator();
  QValidator::State validate(QString &, int &) const;
  void setup(double min, double max);
  
 private:
  double fMin;
  double fMax;
  
};

/*********************************************/

class VTelStatus:public QGroupBox {

Q_OBJECT 

public:
  VTelStatus(VHVFrame *mf, QWidget* parent);
  virtual ~VTelStatus();
  void update();

private:
  VHVFrame    *fMainFrame;
/*   VHVControl  *fControl; */
  QLabel *fHVCr0Label;
  QLabel *fHVCr1Label; 
  QLabel *fCMLabel; 
  QLabel *fDBLabel; 
  QLabel *fTLabel[NUMTEL];
  VLED   *fTCr0LED[NUMTEL];
  VLED   *fTCr1LED[NUMTEL];
  VLED   *fTCMLED[NUMTEL]; 
  VLED   *fTDBLED[NUMTEL]; 
  
};


 /*********************************************/

class VStarPanel:public QGroupBox {

Q_OBJECT 

public:
  VStarPanel(VHVFrame * mf, VHVControl *control[NUMTEL], QWidget * w);
   virtual ~VStarPanel();
  void update();
  void changedCoords();

private slots: 
  void changedMaxMag(int value);
  void changedRA();
  void changedDec(); 
  void showStars();
  void showArcs();
  bool starsOn();  bool arcsOn();
  void setStarsOn(bool on);
  void setArcsOn(bool on);
  void optionButtonClicked( int id );

private:
  VHVFrame    *fMainFrame;
  VHVControl  *fControl[NUMTEL];
  QSlider     *fSlider;
  QLineEdit   *fRAInput;
  QLineEdit   *fDecInput;
  QListBox    *fListBox;
  QPushButton *fStarsButton;
  QPushButton *fArcsButton;  
  std::vector<float>  fRAHR;
  std::vector<float>  fRAMIN;
  std::vector<float>  fRASEC;
  std::vector<float>  fDecDEG;
  std::vector<float>  fDecMIN;
  std::vector<float>  fDecSEC;
  float fStarOffset;
};

/**********************************************/

class VLED : public QLabel {

public:
  VLED(QWidget *, const char *name = 0);
  virtual ~VLED();
  void setState(int state);

private:
  int      fState;
  QPixmap *fGreen;
  QPixmap *fRed;
  QPixmap *fYellow;

};

/**********************************************/

class VTimeWidget : public QWidget {
  
 public: 
  VTimeWidget(QString label, QWidget* parent);
  virtual ~VTimeWidget();
  void update();
  
 private:
  QTime   fTime;
  QLCDNumber *fText;
  QLabel *fLabel;
  
};


/**********************************************/

class VAutoResponseDialog : public QDialog {

Q_OBJECT

  public:
 VAutoResponseDialog( VHVFrame* parent,
		      VHVControl* control, 
		      const char * name );
 virtual ~VAutoResponseDialog();

 bool isAutoCheckBoxChecked() const;
 bool isRestoreCheckBoxChecked() const;
 int getTimeValue() const;
 int getThresholdValue() const;
 int getHiThresholdValue() const;
 int getPatternThresholdValue() const;
 int getPatternMultiplicityValue() const;
 int getRecentTimeValue() const;
 float getRestoreMarginValue() const;
 float getPatternRestoreMarginValue() const;

 private:
 VHVControl *fControl;
 QSpinBox *fTimeSpinBox;
 QSpinBox *fThresholdSpinBox;
 QSpinBox *fHiThresholdSpinBox;
 QSpinBox *fPatternThresholdSpinBox;
 QSpinBox *fPatternMultiplicitySpinBox;
 QSpinBox *fRecentTimeSpinBox;
 QSpinBox *fRestoreMarginSpinBox;
 QSpinBox *fPatternRestoreMarginSpinBox;
 QCheckBox *fAutoResponseCheckBox;
 QCheckBox *fRestoreCheckBox;
 QLabel *fTimeLabel;
 QLabel *fThresholdLabel;
 QLabel *fHiThresholdLabel;
 QLabel *fPatternThresholdLabel;
 QLabel *fPatternMultiplicityLabel;
 QLabel *fRecentTimeLabel;
 QLabel *fRestoreMarginLabel;
 QLabel *fPatternRestoreMarginLabel;

};


/**********************************************/


/**********************************************/


class VSpinBox : public QSpinBox {

 public:
  VSpinBox( int minValue, int maxValue, int step, 
	    QWidget* parent, const char* name );
  virtual ~VSpinBox();
  QString mapValueToText( int value );
  int mapTextToValue( bool *ok );
/*   QString setValue( double dv ); */

 private:
  QDoubleValidator *fDoubleValidator;
};


/**********************************************/

class VFlatFieldDialog : public QDialog {

Q_OBJECT

  public:
 VFlatFieldDialog(VHVFrame* parent);
 virtual ~VFlatFieldDialog();

 bool isAutoCheckBoxChecked() const;
 int getFeedbackGainValue() const;
 int getTargetCurrentValue() const;
 int getPMTIndexValue() const;
 int getMaxVoltageValue() const;
 bool isZeroAmpsBoxChecked() const;

 public slots:
 void flatField();
 void setValues();

 private:
 VHVFrame *fParent;
 QSpinBox *fFeedbackGainSpinBox;
 QSpinBox *fTargetSpinBox;
 QSpinBox *fPMTIndexSpinBox;
 QSpinBox *fMaxVoltageSpinBox;
 QCheckBox *fZeroAmpsCheckBox;
 QLabel *fTargetLabel;
 QLabel *fFeedbackGainLabel;
 QLabel *fPMTIndexLabel;
 QLabel *fMaxVoltageLabel;


};


/**********************************************/



/**********************************************/

class VFeedbackDialog : public QDialog {

Q_OBJECT

  public:
 VFeedbackDialog(VHVFrame* parent, const char *name, int telID);
 virtual ~VFeedbackDialog();

 bool isOldSchoolChecked() const;
 bool isFeedbackChecked() const;
 int getThresholdValue() const;
 int getHiThresholdValue() const;
 int getPatternThresholdValue() const;
 int getPatternMultiplicityValue() const;
 float getRestoreMarginValue() const;
 int getMeasurementsPerCycleValue() const;
 int getMinTimeBeforeRestoreValue() const;
 int getNumberOfFeedbackLevelsValue() const;
 float getGainScalingValue() const;
 float getSafetyFactorValue() const;
 int getRestoreAttemptTimeValue() const;
 float getMinAbsPositionChangeValue() const;
 float getPatternDistanceDropValue() const;
 float getPSFValue() const;
 int getCyclesPerFeedbackTestValue() const;
 float getPMTIndexValue() const;

 private:
 int fTelID;

 QSpinBox *fThresholdSBX;
 QSpinBox *fHiThresholdSBX;
 QSpinBox *fPatternThresholdSBX;
 QSpinBox *fPatternMultiplicitySBX;
 VSpinBox *fRestoreMarginSBX;
 QSpinBox *fMeasurementsPerCycleSBX;
 QSpinBox *fMinTimeBeforeRestoreSBX;
 QSpinBox *fNumberOfFeedbackLevelsSBX;
 VSpinBox *fGainScalingSBX;
 VSpinBox *fSafetyFactorSBX;
 QSpinBox *fRestoreAttemptTimeSBX;
 VSpinBox *fMinAbsPositionChangeSBX;
 VSpinBox *fPatternDistanceDropSBX;
 VSpinBox *fPSFSBX;
 QSpinBox *fCyclesPerFeedbackTestSBX;
 VSpinBox *fPMTIndexSBX;
 QCheckBox *fFeedbackCBX;
 QCheckBox *fOldSchoolCBX;

 QLabel *fTitleLab;
 QLabel *fThresholdLab;
 QLabel *fHiThresholdLab;
 QLabel *fPatternThresholdLab;
 QLabel *fPatternMultiplicityLab;
 QLabel *fRestoreMarginLab;
 QLabel *fMeasurementsPerCycleLab;
 QLabel *fMinTimeBeforeRestoreLab;
 QLabel *fNumberOfFeedbackLevelsLab;
 QLabel *fGainScalingLab;
 QLabel *fSafetyFactorLab;
 QLabel *fRestoreAttemptTimeLab;
 QLabel *fMinAbsPositionChangeLab;
 QLabel *fPatternDistanceDropLab;
 QLabel *fPSFLab;
 QLabel *fCyclesPerFeedbackTestLab;
 QLabel *fPMTIndexLab;
 QLabel *fOldSchoolLab;

};

/**********************************************/


QButtonGroup *OptionButtons(QWidget * p);

#endif
