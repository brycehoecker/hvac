
///////////////////////////////////////////////////////////////////////////////
// $Id: VHVFrame.cpp,v 1.4 2006/03/30 17:11:40 humensky Exp $
//
// Filename : 
// Created  : Tue Mar 19 2002 by Jim Hinton
///////////////////////////////////////////////////////////////////////////////
/*! \file VHVFrame.cpp
  \brief Defines classes for the main frame and right hand control panel of the HV GUI
*/

#include <cstdlib>
#include <cstdio> 
#include <cmath>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include <qcursor.h>      //SPW 6-11-2003
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qmenubar.h>
#include <qpainter.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qapplication.h>
#include <qradiobutton.h>
#include <qtooltip.h>
#include <qtextedit.h>
#include <qpopupmenu.h>
#include <qfiledialog.h>
#include <qfile.h>
#include <qhbox.h>
#include <qtextbrowser.h>
#include <qwmatrix.h>
#include <qgrid.h>

#include "VHVFrame.h"

using namespace std;
//using namespace VERITAS;

extern bool kDBLogging[NUMTEL];
extern bool kAutoResponse[NUMTEL];
extern bool kStarRestore[NUMTEL];
extern bool kFeedback[NUMTEL];
extern bool kOldSchool[NUMTEL];
extern bool kFlatField;
extern bool kNoCurrents;
extern bool kArrayCorba;
extern bool kCurrMonCorba;
extern ofstream kLogFile;
extern double kLatitude;
extern double kLongitude;
extern bool kActiveTelescope[NUMTEL];

/****************************************************************************/
/**********************VHVFrame*********************************************/
/** \class VHVFrame VHVFrame.h
 * \brief This class contains the code to generate the GUI frame and invokes all other display widgets.
 * \sa VHVControl
 */

/** Constructor:
 * - Instantiates a VHVControl object and an associated VHVControlView
 * - Connects signals to slots for new data and mouse clicks
 * - Lays out the windows and builds the menus
 * - Generates the right hand control panel
 *
 * \arg \c parent Qt parent widget
 * \arg \c name   Identifier
 * \arg \c flags  Qt flags
 */
VHVFrame::VHVFrame(QWidget* parent, const char* name, WFlags flags) :
  QMainWindow(parent,name,flags)
{
  fStillInConstructor = true;
  //   fTransientCheckTime = 4000; // Units are milliseconds.
  //   fCurrentThreshold = 40;      // Units are microamps.

  fDisplayOption = IPMT;
  fDisplayNotIPMTFlag = false;
  fStartTimeNotIPMT = 0;
  fCentral = new QWidget( this, "mainFrame" );
  fTab = new QTabWidget( fCentral, "telescopeTabs" );
  QFont aFont( "Helvetica", 10, QFont::Normal );
  setFont( aFont );

  /* Create VHVControl */  
  QString telName[NUMTEL];
  telName[0] = (string)"Telescope &1: Shock";
  telName[1] = (string)"Telescope &2: Denial";
  telName[2] = (string)"Telescope &3: Anger";
  telName[3] = (string)"Telescope &4: Acceptance";
  QWMatrix halfScale;
  halfScale.scale( 0.5, 0.5 );
  int siz = 17;
  for ( int i=0; i<NUMTEL; ++i ) {
    fControl[i] = new VHVControl( i, siz, this, "control" );
    //     cout << "Made fControl[" << i << "].  T" << i+1 << " kActiveTelescope is "
    // 	 << kActiveTelescope[i] << endl;
    fControl[i]->setAdvancePeriod( 30 );
    fView[i] = new VHVControlView( fControl[i], fTab, this );
    fCamInfo[i] = new VCamInfo( this, fControl[i], fControl[i]->getData(), i );
    fControl[i]->loadStars();
    fTab->addTab( fView[i], telName[i] );
    connect(fControl[i]->getVHVClient(), 
	    SIGNAL( printMessage(const QString&) ),
	    this, 
	    SLOT( sendMessage(const QString&) ));
  }
  //   for ( int i=0; i<NUMTEL; ++i ) {
  //     cout << "After all constructors:  T" << i+1 << " kActiveTelescope is "
  // 	 << kActiveTelescope[i] << endl;
  //   }
  QGrid* allTel = new QGrid( 2, fTab, "allTelescopes" );
  for ( int i=0; i<NUMTEL; ++i ) {
    fView[NUMTEL+i] = new VHVControlView( fControl[i], allTel, this );
    fView[NUMTEL+i]->setWorldMatrix( halfScale );
  }
  allTel->setMinimumSize( 700, 700 );
  allTel->setSpacing( 0 );
  fTab->addTab( allTel, QString( "All Telescopes" ) );

  fTimer = new QTimer( this );

  // PMT.
  bool selectedFlag = false;
  for ( int i=0; i<NUMTEL; ++i ) {
    if ( kActiveTelescope[i] ) {
      fSelectedPMT = fControl[i]->getPMT( 0 );
      selectedFlag = true;
      break;
    }
  }
  if ( !selectedFlag ) {
    cout << "No active telescopes!  That ain't good.  Go check your vhv's."
	 << endl;
    exit( 1 );
  }


  for ( int j=0; j<2*NUMTEL; ++j ) {
    connect(fView[j], SIGNAL( clickedPMT(VPMTIcon*,QMouseEvent*, int ) ),
	    this, SLOT( pmtSelect( VPMTIcon*, QMouseEvent*, int ) ));
  }

  connect(fTimer, SIGNAL( timeout() ),
	  this, SLOT( updateAll() ) );

  /* Layout the main window */  
  QHBoxLayout *g1 = new QHBoxLayout( fCentral, 0 );
  QWidget *rightbox = new QWidget( fCentral );
  QVBoxLayout *g2 = new QVBoxLayout( rightbox, 0 );

  /* Set canvas as central window */
  setCentralWidget(fCentral);
  
  /* Generate right hand panel controls */
  //  QButtonGroup *bg = OptionButtons(rightbox);
  //  connect( bg, SIGNAL(clicked(int)), SLOT(optionButtonClicked(int)) );
  fBG = OptionButtons(rightbox);
  connect( fBG, SIGNAL(clicked(int)), SLOT(optionButtonClicked(int)) );

  // Define the message box.
  fMessages = new QTextBrowser( rightbox, "<h3>Messages</h3>" );
  fMessages->setFrameStyle( QFrame::WinPanel | QFrame::Sunken );
  fMessages->setMinimumSize( 180, 315 );
  fMessages->setTextFormat( RichText );
  fMessages->setReadOnly( true );

  g2->addWidget( fTelStatus = new VTelStatus( this, rightbox ) );
  //g2->addWidget( bg ); 
  g2->addWidget( fBG ); 
  g2->addWidget( fStarPanel = new VStarPanel( this, fControl, rightbox ) );
  g2->addWidget( fChannelReport = new VChannelReport( this, rightbox, "" ) );
  g2->addWidget( fBoardReport = new VBoardReport( this, rightbox, "" ) );
  g2->addWidget( fMessages );  

  g2->addStretch( 1 );
  g2->setSpacing( 2 );
  g1->addWidget( fTab );
  g1->addWidget( rightbox );
  g1->setSpacing( 2 );
  
  QMenuBar* menu = menuBar();
  QPopupMenu* file = new QPopupMenu;
  file->insertItem( "&Read Voltages from DB", this, SLOT( dbMenu( int ) ),
		    CTRL+Key_V, MENU_READ_DB, 0 );
  file->insertItem( "&Write Voltages to DB", this, SLOT( dbMenu( int ) ),
		    CTRL+Key_W, MENU_WRITE_DB, 1 );
  file->insertItem("&Load File", this, SLOT(fileMenu(int)), 
		   CTRL+Key_L, MENU_LOAD, 2 );
  file->insertItem("&Save File", this, SLOT(fileMenu(int)), 
		   CTRL+Key_S, MENU_SAVE, 3 );
  file->insertSeparator();
  file->insertItem("E&xit", this, SLOT(fileMenu(int)),
		   CTRL+Key_X, MENU_EXIT, 4 );
  file->insertItem("Telescope (vhv) Exit", this, SLOT(fileMenu(int)),
		   CTRL+Key_A, MENU_TELESCOPE_EXIT, 5 );
  //  file->setAccel( 0, 5 );
  file->insertItem("HVAC Exit (leaves vhv's running)", this, SLOT(fileMenu(int)),
		   CTRL+Key_Z, MENU_HVAC_EXIT, 6 );
  //  file->setAccel( 0, 6 );
  menu->insertItem("&File", file);
  
  QPopupMenu* db = new QPopupMenu;
  db->insertItem("Turn &DB Logging ON", this, SLOT(dbMenu(int)), 
		 CTRL+Key_D, MENU_LOGGING_ON, 0);
  db->insertItem("Turn D&B Logging OFF", this, SLOT(dbMenu(int)), 
		 CTRL+Key_B, MENU_LOGGING_OFF, 1);
  db->insertItem("Change Status Logging Period", this, SLOT(dbMenu(int)),
		 CTRL+Key_P, MENU_LOGGING_PERIOD, 2 );
  //   db->insertItem( "&Read Camera Map from DB", this, SLOT( dbMenu( int ) ),
  // 		  CTRL+Key_M, MENU_READ_MAP, 4 );
  //   db->insertItem( "&Write Camera Map to DB", this, SLOT( dbMenu( int ) ),
  // 		  CTRL+Key_N, MENU_WRITE_MAP, 5 );
  menu->insertItem( "&DB Logging", db );

  QPopupMenu* pmt = new QPopupMenu;
  pmt->insertItem("S&elect PMT", this, SLOT(pmtMenu(int)),
		  CTRL+Key_E, MENU_SELECT);
  pmt->insertItem("All &On", this, SLOT(pmtMenu(int)),
		  CTRL+Key_O, MENU_ALLON);
  pmt->insertItem("All Of&f", this, SLOT(pmtMenu(int)), 
		  CTRL+Key_F, MENU_ALLOFF);
  pmt->insertSeparator();
  pmt->insertItem("Suppress All", this, SLOT(pmtMenu(int)), 
		  CTRL+Key_Y, MENU_SUPPRESS_ALL);
  pmt->insertItem("Res&tore All", this, SLOT(pmtMenu(int)), 
		  CTRL+Key_T, MENU_RESTORE_ALL);
  pmt->insertItem("Res&tore Recent", this, SLOT(pmtMenu(int)), 
		  CTRL+Key_U, MENU_RESTORE_RECENT);
  pmt->insertSeparator();
  pmt->insertItem("Res&et Feedback", this, SLOT(pmtMenu(int)),
		  CTRL+Key_I, MENU_GLOBAL_FDBK_RESET);
  pmt->insertItem("Ena&ble Feedback", this, SLOT(pmtMenu(int)),
		  CTRL+Key_J, MENU_GLOBAL_FDBK_ON);
  pmt->insertItem("Dis&able Feedback", this, SLOT(pmtMenu(int)),
		  CTRL+Key_K, MENU_GLOBAL_FDBK_OFF);
  //   pmt->insertItem("Disable Pixel", this, SLOT(hvParamsMenu(int)),
  // 		   CTRL+Key_5, MENU_BROKEN );
  //   pmt->insertItem("UnDisable Pixel", this, SLOT(hvParamsMenu(int)),
  // 		   CTRL+Key_6, MENU_UNBROKEN );
  pmt->insertItem("&Report", this, SLOT(pmtMenu(int)),
		  CTRL+Key_R, MENU_REPORT);
  menu->insertItem("&PMT",  pmt);
  
  QPopupMenu* tele = new QPopupMenu;

  tele->insertItem("Scale To ... %", this, SLOT(hvParamsMenu(int)), CTRL+Key_1,
		   MENU_SCALE_VOLTAGES, 0);
  tele->insertItem("Offset By ... V", this, SLOT(hvParamsMenu(int)),
		   CTRL+Key_2, MENU_OFFSET_VOLTAGES, 1);
  tele->insertItem("Set All Pixels to ... V", this, SLOT(hvParamsMenu(int)),
		   CTRL+Key_3, MENU_SET_ALL_VOLTAGES, 2);
  tele->insertItem("Set Feedback Params", this, SLOT(hvParamsMenu(int)),
		   CTRL+Key_4, MENU_FEEDBACK, 3);
  tele->insertItem("Set AutoResponse Params", this, SLOT(hvParamsMenu(int)),
		   CTRL+Key_5, MENU_AUTORESPONSE, 4);
  tele->insertItem("HiLo Calib Inner Half High", this, SLOT(hvParamsMenu(int)),
		   CTRL+Key_6, MENU_HILOINNER, 5);
  tele->insertItem("HiLo Calib Outer Half High", this, SLOT(hvParamsMenu(int)),
		   CTRL+Key_6, MENU_HILOOUTER, 6);
  //   tele->insertItem("Flat Field", this, SLOT(hvParamsMenu(int)),
  // 		   CTRL+Key_7, MENU_FLATFIELD, 5);
  menu->insertItem("&HV Params",  tele);

  QPopupMenu* star = new QPopupMenu;
  star->insertItem("Show Stars", this, SLOT(starMenu(int)), 
		   0, MENU_SHOW_STARS);
  star->insertItem("Hide Stars", this, SLOT(starMenu(int)), 
		   0, MENU_HIDE_STARS);
  //   star->insertItem("Show Arcs",  this, SLOT(starMenu(int)), 
  // 		   0, MENU_SHOW_ARCS);
  //   star->insertItem("Hide Arcs",  this, SLOT(starMenu(int)), 
  // 		   0, MENU_HIDE_ARCS);
  //   star->insertItem("Change Mag.", this, SLOT(starMenu(int)),
  // 		   0, MENU_CHANGE_MAG);
  menu->insertItem("&Stars",  star);

  menu->insertSeparator(2);
  
  QPopupMenu* help = new QPopupMenu;
  //   help->insertItem("Help Browser", this, SLOT(help()), Key_F2);
  //   help->insertItem( "Help Browser", this, SLOT( helpBrowser() ), Key_F2 );
  help->insertItem("&About", this, SLOT(about()), Key_F1);
  menu->insertItem("&Help",help);

  fUpdateTime = new VTimeWidget("Last Update Time  ",this);
  menu->insertItem( fUpdateTime );
  
  statusBar();

  setCaption("VERITAS HV Array Control (UChicago)");

  init();

  for ( int j=0; j<NUMTEL; ++j ) {
    for ( int i=0; i<MAX_PMTS; ++i ) {
      fPMTIndicator[j][i] = new VPMTIndicator( this, fControl[j], 
					       fControl[j]->getData(), 
					       siz, i );
      int x = fControl[j]->getPMTCentreX( i );
      int y = fControl[j]->getPMTCentreY( i );
      //       if ( i == 0 ) {
      // 	cout << "T" << j+1 << ":  (x, y) = (" << x << ", " << y << ")" << endl;
      //       }
      fPMTIndicator[j][i]->setPosition( x, y );
    }
    for ( int k=0; k<MAX_PMTS; ++k ) 
      {
	for ( int i=0; i<CURRSAMPLES; ++i )
	  {
	    fCurrent[j][k][i] = i;
	  }
      }
    fCurrentWarningTime[j] = 0;
  }
  fTab->setCurrentPage( NUMTEL );
  //   fTab->setCurrentPage( 0 );
  fTab->show();

  fUpdatePeriod = 500;
  fTimer->start( fUpdatePeriod, FALSE );
  cout << "hvac:  VHVFrame::VHVFrame():  Starting main update timer -- "
       << "\n\tupdates every " 
       << fUpdatePeriod << " ms." << endl;
  
  // This should be the last line of the constructor.
  fStillInConstructor = false;
}

/** Destructor
 */
VHVFrame::~VHVFrame()
{

  delete fMessages;
  delete fChannelReport;
  delete fBoardReport;
  delete fTelStatus;
  delete fStarPanel;
  delete fLineInput;
  delete fUpdateTime;
  delete fView;
  for ( int i=0; i<NUMTEL; ++i ) {
    delete fCamInfo[i];
    delete fControl[i];
  }
  delete fTab;
  delete fCentral;
  delete fBG;
}


/** Initialisation - draw the camera and load stars.
 */
void VHVFrame::init()
{
  int viewHt = 420;
  int viewWd = 420;
  for ( int j=0; j<NUMTEL; ++j ) {
    fView[NUMTEL+j]->setMaximumSize( viewWd, viewHt );
    if ( kActiveTelescope[j] ) {
      fControl[j]->resize( 2*viewWd, 2*viewHt );
      fControl[j]->drawCamera();  
      fControl[j]->drawStars();
    }
  }
  fControl[fSelectedPMT->getTelescopeID()]->drawSelected(fSelectedPMT);

  statusBar()->message("HV connected: ready for commands, Master\n",4000);
}


/** Raise a message box with a brief description of the project.
 */
void VHVFrame::about()
{
  static QMessageBox* about = 
    new QMessageBox( "VERITAS HV Array Control","<h3>This is the "
		     "HV Array Control GUI (hvac) for "
		     "VERITAS</h3>",
		     QMessageBox::Information, 
		     1, 0, 0, this, 0, FALSE );
  about->setButtonText( 1, "Exterminate" );
  about->show();
}


void VHVFrame::help()
{
  // Make the main window.
  QMainWindow *mw = new QMainWindow;
  //  mw->setUsesTextLabel( true );
  mw->setCaption( "Help Browser" );

  // Set up the browser.
  QTextBrowser *browser = new QTextBrowser( mw, "Help" );
  browser->setMinimumSize(400,500);
  browser->setMaximumSize(500,700);
  browser->setTextFormat( RichText );
  browser->setFocus();
  //   browser->setSource( "/usr/local/veritas/SP05/vhv/DataFiles/index.html" );
  browser->setSource( "http://jelley.wustl.edu/wiki/index.php/T2_Manual" );
  mw->setCentralWidget( browser );
  
  // Set up the toolbar.
  QToolBar *tool = new QToolBar( mw, "Navigation" );
  QToolButton *backButton = 
    new QToolButton( QIconSet( QPixmap( "/home/humensky/noia_kde_100/32x32/actions/back.png" ) ),
		     "Back", "navigation", browser, SLOT( backward() ), 
		     tool );
  backButton->setEnabled( false );
  QToolButton *foreButton = 
    new QToolButton( QIconSet( QPixmap( "/home/humensky/noia_kde_100/32x32/actions/forward.png" ) ),
		     "Forward", "navigation", browser, SLOT( forward() ), 
		     tool );
  foreButton->setEnabled( false );
  QToolButton *homeButton = 
    new QToolButton( QIconSet( QPixmap( "/home/humensky/noia_kde_100/32x32/actions/gohome.png" ) ),
		     "Home", "navigation", browser, SLOT( home() ), 
		     tool );
  connect( browser, SIGNAL( backwardAvailable( bool ) ), 
	   backButton, SLOT( setEnabled( bool ) ) );
  connect( browser, SIGNAL( forwardAvailable( bool ) ), 
	   backButton, SLOT( setEnabled( bool ) ) );

  mw->show();

}


void VHVFrame::helpBrowser()
{
  QString home;
  //   home = QDir( "../DataFiles/index.html" ).absPath();
  home = QDir( "http://jelley.wustl.edu/wiki/index.php/T2_Manual" ).path();
  
  VHelpWindow *help = new VHelpWindow( home, ".", 0, "Help Browser" );
  help->setCaption( "VHV Documentation Browser" );
  help->show();
}


/** Raise a message box with a 'not yet implemented' message.
 */
void VHVFrame::notYet()
{
  static QMessageBox* message = 
    new QMessageBox( "VERITAS HV control","<h3>Congratulations!  You have wasted time.</h3>",
		     QMessageBox::Information, 
		     1, 0, 0, this, 0, FALSE );
  message->setButtonText( 1, "Dismiss" );
  message->show();
}


/** Called when an item on the PMT menu is selected
 * \arg \c id - Menu item identifer (::EnumMenu)
 */
void VHVFrame::pmtMenu(int id)
{
  int index = fTab->currentPageIndex();
  switch (id) {
  case MENU_SELECT:
    if ( index != NUMTEL ) menuPmtSelect( index );
    break;
  case MENU_ALLON:
    if ( index == NUMTEL ) {
      for ( int i=0; i<NUMTEL; ++i ) {
	fControl[i]->allOn();
      }
    } else {
      fControl[index]->allOn();
    }
    break;
  case MENU_ALLOFF:
    if ( index == NUMTEL ) {
      for ( int i=0; i<NUMTEL; ++i ) {
	fControl[i]->allOff();
      }
    } else {
      fControl[index]->allOff(); 
    }
    break;
  case MENU_RESTORE_ALL:
    if ( index == NUMTEL ) {
      for ( int i=0; i<NUMTEL; ++i ) {
	fControl[i]->restoreAll(); 
      }
    } else {
      fControl[index]->restoreAll(); 
    }
    break;
  case MENU_RESTORE_RECENT:
    if ( index == NUMTEL ) {
      for ( int i=0; i<NUMTEL; ++i ) {
	fControl[i]->restoreRecent(); 
      }
    } else {
      fControl[index]->restoreRecent(); 
    }
    break;
  case MENU_SUPPRESS_ALL:
    if ( index == NUMTEL ) {
      for ( int i=0; i<NUMTEL; ++i ) {
	fControl[i]->suppressAll(); 
      }
    } else {
      fControl[index]->suppressAll(); 
    }
    break;
  case MENU_REPORT:
    statusBar()->message("Generating Report",3000);
    if ( index == NUMTEL ) {
      for ( int i=0; i<NUMTEL; ++i ) {
	fControl[i]->report("hv_report.txt"); 
      }
    } else {
      fControl[index]->report("hv_report.txt"); 
    }
    sendMessage( " Report output to hv_report.txt <br>" );
    break;
  case MENU_GLOBAL_FDBK_RESET:
    if ( index == NUMTEL ) {
      for ( int i=0; i<NUMTEL; ++i ) {
	fControl[i]->globalFeedbackReset();
      }
    } else {
      fControl[index]->globalFeedbackReset();
    }
    break;
  case MENU_GLOBAL_FDBK_ON:
    if ( index == NUMTEL ) {
      for ( int i=0; i<NUMTEL; ++i ) {
	fControl[i]->globalFeedbackOn();
      }
    } else {
      fControl[index]->globalFeedbackOn();
    }
    break;
  case MENU_GLOBAL_FDBK_OFF:
    if ( index == NUMTEL ) {
      for ( int i=0; i<NUMTEL; ++i ) {
	fControl[i]->globalFeedbackOff();
      }
    } else {
      fControl[index]->globalFeedbackOff();
    }
    break;
  }
}


/** Called when an item on the file menu is selected
 * \arg \c id - Menu item identifer (::EnumMenu)
 */
void VHVFrame::fileMenu(int id)
{

  int index = fTab->currentPageIndex();
  QString filename;
  QString message;
  FILE *file;
  char tmp[200];

  switch (id) {
  case MENU_LOAD:
    if ( index == NUMTEL ) {
      for ( int i=0; i<NUMTEL; ++i ) { 
	if ( kActiveTelescope[i] ) {   
	  sprintf( tmp, "T%d Load HV Settings", i+1 );
	  filename = QFileDialog::getOpenFileName
	    ( kDefaultHVFile[i], "HV files (*.hv)", this, 
	      "Open", QString( tmp ) );
	  if (!filename.isNull()) {
	    statusBar()->message("Loading HV file\n",3000);
	    fControl[i]->loadHV( filename.latin1() );
	    message = filename;
	    message.prepend( " Loaded HV from file: " );
	    message.append( "<br>" );
	    sendMessage( message );
	  }
	}
      }
    } else {
      if ( kActiveTelescope[index] ) {   
	sprintf( tmp, "T%d Load HV Settings", index+1 );
	filename = QFileDialog::getOpenFileName
	  ( kDefaultHVFile[index], "HV files (*.hv)", this, 
	    "Open", QString( tmp ) );
	if (!filename.isNull()) {
	  statusBar()->message("Loading HV file\n",3000);
	  fControl[index]->loadHV( filename.latin1() );
	  message = filename;
	  message.prepend( " Loaded HV from file: " );
	  message.append( "<br>" );
	  sendMessage( message );
	}
      }
    }
    break;
  case MENU_SAVE:    
    if ( index == NUMTEL ) {
      for ( int i=0; i<NUMTEL; ++i ) {    
	if ( kActiveTelescope[i] ) {   
	  sprintf( tmp, "T%d Save HV Settings", i+1 );
	  filename = QFileDialog::getSaveFileName
	    ( kDefaultHVFile[i], "HV files (*.hv)", this, 
	      "Save", QString( tmp ) );
	  if (!filename.isNull()) {
	    statusBar()->message("Saving HV file\n",3000);
	    fControl[i]->saveHV( filename.latin1() );
	    message = filename;
	    message.prepend( " Saving HV to file: " );
	    message.append( "<br>" );
	    sendMessage( message );
	  }
	}
      }
    } else {
      if ( kActiveTelescope[index] ) {   
	sprintf( tmp, "T%d Save HV Settings", index+1 );
	filename = QFileDialog::getSaveFileName
	  ( kDefaultHVFile[index], "HV files (*.hv)", this, 
	    "Save", QString( tmp ) );
	if (!filename.isNull()) {
	  statusBar()->message("Saving HV file\n",3000);
	  fControl[index]->saveHV( filename.latin1() );
	  message = filename;
	  message.prepend( " Saving HV to file: " );
	  message.append( "<br>" );
	  sendMessage( message );
	}
      }
    }
    break;
  case MENU_EXIT:
    cout << "hvac:  Quitting hvac and all instances of vhv." << endl;
    fTimer->stop();
    for ( int i=0; i<NUMTEL; ++i ) {    
      fControl[i]->allOff(); 
      fControl[i]->disconnectFromHVAC();
      fControl[i]->quitVHV();
      fControl[i]->disconnect();
      qApp->quit();
    }
    break;
  case MENU_TELESCOPE_EXIT:
    if ( index == NUMTEL ) {
      for ( int i=0; i<NUMTEL; ++i ) {    
	cout << "hvac:  Quitting vhv on all telescopes." << endl;
	fControl[i]->allOff(); 
	fControl[i]->disconnectFromHVAC();
	fControl[i]->quitVHV();
	fControl[i]->disconnect();
      }
    } else {
      cout << "hvac:  Quitting Telescope " << index+1 << " vhv." << endl;
      fControl[index]->allOff(); 
      fControl[index]->disconnectFromHVAC();
      fControl[index]->quitVHV();
      fControl[index]->disconnect();
    }
    break;
  case MENU_HVAC_EXIT:
    fTimer->stop();
    cout << "hvac:  Quitting hvac -- any instances of vhv will be left "
	 << "running!"
	 << endl;
    if ( index == NUMTEL ) {
      for ( int i=0; i<NUMTEL; ++i ) {    
	fControl[i]->allOff(); 
	fControl[i]->releaseStarControl();
	fControl[i]->disconnectFromHVAC();
	fControl[i]->disconnect();
      }
    } else {
      fControl[index]->allOff(); 
      fControl[index]->releaseStarControl();
      fControl[index]->disconnectFromHVAC();
      fControl[index]->disconnect();
    }
    qApp->quit();
    break;
  }
}


/** Called when an item on the db menu is selected
 * \arg \c id - Menu item identifer (::EnumMenu)
 */
void VHVFrame::dbMenu(int id)
{
  int index = fTab->currentPageIndex();
  QString message;
  char tmpStr[200];
  switch (id) {
  case MENU_LOGGING_ON:
    if ( index == NUMTEL ) {
      for ( int i=0; i<NUMTEL; ++i ) {    
	kDBLogging[i] = true;
	fControl[i]->loggingOn();
	sprintf( tmpStr, " Tel %d:  Logging to DB is now ON. <br>", i+1 );
	sendMessage( QString( tmpStr ) );
      }
    } else {
      kDBLogging[index] = true;
      fControl[index]->loggingOn();
      sprintf( tmpStr, " Tel %d:  Logging to DB is now ON. <br>", index+1 );
      sendMessage( QString( tmpStr ) );      
    }
    break;
  case MENU_LOGGING_OFF:
    if ( index == NUMTEL ) {
      for ( int i=0; i<NUMTEL; ++i ) {    
	kDBLogging[i] = false;
	fControl[i]->loggingOff();
	sprintf( tmpStr, " Tel %d:  Logging to DB is now OFF. <br>", i+1 );
	sendMessage( QString( tmpStr ) );
      }
    } else {
      kDBLogging[index] = false;
      fControl[index]->loggingOff();
      sprintf( tmpStr, " Tel %d:  Logging to DB is now OFF. <br>", 
	       index+1 );
      sendMessage( QString( tmpStr ) );      
    }
    break;    
  case MENU_LOGGING_PERIOD: {
      QFrame *popup = new QFrame( this ,0, WType_Popup);
      popup->setFrameStyle( QFrame::WinPanel | QFrame::Raised );
      popup->resize(400,100);
      fLineInput = new QLineEdit( popup );
      connect( fLineInput, SIGNAL( returnPressed() ), this, 
             SLOT( setStatusPeriod() ));
      fLineInput->setGeometry(10, 10, 380, 30);
      fLineInput->setValidator(new VIntValidator(this, "", 3, 60));
      fLineInput->setFocus();
      sprintf( tmpStr, "All Tel: Set status logging period (sec); ints [3,60] allowed - reset to 60 when done!" );
      QPushButton *b = new QPushButton( tmpStr, popup );
      b->setGeometry(10, 50, 380, 30);
      connect( b, SIGNAL( clicked() ), this, SLOT( setStatusPeriod() ) );
      popup->move(QCursor::pos());
      popup->show();
  }
      break;
  case MENU_READ_DB:
    if ( index == NUMTEL ) {
      for ( int i=0; i<NUMTEL; ++i ) {    
	if ( kActiveTelescope[i] ) {
	  sprintf( tmpStr, " Tel %d:  Reading voltages from database...", 
		   i+1 );
	  sendMessage( QString( tmpStr ) );
	  fControl[i]->readVoltagesFromDB();
	  sendMessage( "  done <br>" );
	}
      }
    } else {
      if ( kActiveTelescope[index] ) {
	sprintf( tmpStr, " Tel %d:  Reading voltages from database...", 
		 index+1 );
	sendMessage( QString( tmpStr ) );
	fControl[index]->readVoltagesFromDB();
	sendMessage( "  done <br>" );
      }
    }
    break;
  case MENU_WRITE_DB:    
    if ( index == NUMTEL ) {
      for ( int i=0; i<NUMTEL; ++i ) {    
	if ( kActiveTelescope[i] ) {
	  sprintf( tmpStr, " Tel %d:  Writing voltages to database...", 
		   i+1 );
	  sendMessage( QString( tmpStr ) );
	  fControl[i]->writeVoltagesToDB();
	  sendMessage( "  done <br>" );
	}
      }
    } else {
      if ( kActiveTelescope[index] ) {
	sprintf( tmpStr, " Tel %d:  Writing voltages to database...", 
		 index+1 );
	sendMessage( QString( tmpStr ) );
	fControl[index]->writeVoltagesToDB();
	sendMessage( "  done <br>" );
      }
    }
    break;
  case MENU_READ_MAP:    
    if ( index == NUMTEL ) {
      for ( int i=0; i<NUMTEL; ++i ) {    
	sprintf( tmpStr, " Tel %d:  Reading HV map from database...", 
		 i+1 );
	sendMessage( QString( tmpStr ) );
	fControl[i]->readCameraMapFromDB();
	sendMessage( "  done <br>" );
      }
    } else {
      sprintf( tmpStr, " Tel %d:  Reading HV map from database...", 
	       index+1 );
      sendMessage( QString( tmpStr ) );
      fControl[index]->readCameraMapFromDB();
      sendMessage( "  done <br>" );
    }
    break;
  case MENU_WRITE_MAP:    
    if ( index == NUMTEL ) {
      for ( int i=0; i<NUMTEL; ++i ) {    
	sprintf( tmpStr, " Tel %d:  Writing HV map to database...", 
		 i+1 );
	sendMessage( QString( tmpStr ) );
	fControl[i]->writeCameraMapToDB();
	sendMessage( "  done <br>" );
      }
    } else {
      sprintf( tmpStr, " Tel %d:  Writing HV map to database...", 
	       index+1 );
      sendMessage( QString( tmpStr ) );
      fControl[index]->writeCameraMapToDB();
      sendMessage( "  done <br>" );
    }
    break;
  }
}


/** Called when an item on the HV Params menu is selected.
 * \arg \c id - Menu item identifier (::EnumMenu)
 */
void VHVFrame::hvParamsMenu(int id)
{
  char tmp[200];
  int index = fTab->currentPageIndex();
  switch (id) {
  case MENU_SCALE_VOLTAGES:
    {
      QFrame *popup = new QFrame( this ,0, WType_Popup);
      popup->setFrameStyle( QFrame::WinPanel | QFrame::Raised );
      popup->resize(230,100);
      fLineInput = new QLineEdit( popup );
      connect( fLineInput, SIGNAL( returnPressed() ), this, 
	       SLOT( scaleVoltages() ));
      fLineInput->setGeometry(10, 10, 210, 30);
      fLineInput->setValidator(new VFloatValidator(this, "", 0, 200));
      fLineInput->setFocus();
      if ( index == NUMTEL ) {
	sprintf( tmp, "All Tel: Scale Voltages by ..." );
      } else {
	sprintf( tmp, "T%d: Scale Voltages by ...", index+1 );
      }
      cout << tmp << endl;
      QPushButton *b = new QPushButton( tmp, popup );
      b->setGeometry(10, 50, 210, 30);
      connect( b, SIGNAL( clicked() ), this, SLOT( scaleVoltages() ) );
      
      popup->move(QCursor::pos());
      popup->show();
      break;
    } // end case MENU_SCALE_VOLTAGES
  case MENU_OFFSET_VOLTAGES:
    {
      QFrame *popup = new QFrame( this ,0, WType_Popup);
      popup->setFrameStyle( QFrame::WinPanel | QFrame::Raised );
      popup->resize(130,100);
      fLineInput = new QLineEdit( popup );
      connect( fLineInput, SIGNAL( returnPressed() ), this, 
	       SLOT( offsetVoltages() ));
      fLineInput->setGeometry(10, 10, 110, 30);
      fLineInput->setValidator(new VFloatValidator(this, "", -MAXVOLTS, MAXVOLTS));
      fLineInput->setFocus();
      if ( index == NUMTEL ) {
	sprintf( tmp, "All Tel: Offset Voltages" );
      } else {
	sprintf( tmp, "T%d: Offset Voltages", index+1 );
      }      
      QPushButton *b = new QPushButton( tmp, popup );
      b->setGeometry(10, 50, 110, 30);
      connect( b, SIGNAL( clicked() ), this, SLOT( offsetVoltages() ) );
      
      popup->move(QCursor::pos());
      popup->show();
      break;
    } // end case MENU_OFFSET_VOLTAGES
  case MENU_SET_ALL_VOLTAGES:
    {
      QFrame *popup = new QFrame( this ,0, WType_Popup);
      popup->setFrameStyle( QFrame::WinPanel | QFrame::Raised );
      popup->resize(130,100);
      fLineInput = new QLineEdit( popup );
      connect( fLineInput, SIGNAL( returnPressed() ), this, 
	       SLOT( setAllVoltages() ));
      fLineInput->setGeometry(10, 10, 110, 30);
      fLineInput->setValidator(new VFloatValidator(this, "", 0, MAXVOLTS));
      fLineInput->setFocus();      
      if ( index == NUMTEL ) {
	sprintf( tmp, "All Tel: Set All Voltages" );
      } else {
	sprintf( tmp, "T%d: Set All Voltages", index+1 );
      }      
      QPushButton *b = new QPushButton( tmp, popup );
      b->setGeometry(10, 50, 110, 30);
      connect( b, SIGNAL( clicked() ), this, SLOT( setAllVoltages() ) );
      
      popup->move(QCursor::pos());
      popup->show();
      break;
    } // end case MENU_SET_ALL_VOLTAGES
  case MENU_AUTORESPONSE:
    {
      int index = fTab->currentPageIndex();
      if ( index == NUMTEL ) {
	for ( int i=0; i<NUMTEL; ++i ) {
	  if ( kActiveTelescope[i] ) {
	    fControl[i]->getVHVClient()->getAutoResponseParams();
	    char tmp[200];
	    sprintf( tmp, "T%d AutoResponse Params", i+1 );
	    VAutoResponseDialog* autoResponseDialog = 
	      new VAutoResponseDialog( this, fControl[i], tmp );
	    int ret = autoResponseDialog->exec();
	    if ( ret == QDialog::Accepted ) {
	      fControl[i]->getData()
		->setThreshold( autoResponseDialog->getThresholdValue() );
	      fControl[i]->getData()->
		setHiThreshold( autoResponseDialog->getHiThresholdValue() );
	      fControl[i]->getData()->
		setPatternThreshold( autoResponseDialog->
				     getPatternThresholdValue() );
	      fControl[i]->getData()->
		setPatternMultiplicity( autoResponseDialog->
					getPatternMultiplicityValue() );
	      fControl[i]->getData()
		->setCheckTime( autoResponseDialog->getTimeValue() );
	      fControl[i]->getData()
		->setRecentTime( autoResponseDialog->getRecentTimeValue() );
	      fControl[i]->getData()
		->setRestoreMargin( autoResponseDialog->getRestoreMarginValue() );
	      fControl[i]->getData()
		->setPatternRestoreMargin( autoResponseDialog->
					   getPatternRestoreMarginValue() );
	      kAutoResponse[i] = autoResponseDialog->isAutoCheckBoxChecked();
	      kStarRestore[i] = autoResponseDialog->isRestoreCheckBoxChecked();
	      fControl[i]->getVHVClient()->setAutoResponseParams();
	    } // end if accepted
	    delete autoResponseDialog;
	  } // end if active
	} // end for NUMTEL
      } else {
	if ( kActiveTelescope[index] ) {
	  fControl[index]->getVHVClient()->getAutoResponseParams();
	  char tmp[200];
	  sprintf( tmp, "T%d AutoResponse Params", index+1 );
	  VAutoResponseDialog* autoResponseDialog = 
	    new VAutoResponseDialog( this, fControl[index], tmp );
	  int ret = autoResponseDialog->exec();
	  if ( ret == QDialog::Accepted ) {
	    fControl[index]->getData()
	      ->setThreshold( autoResponseDialog->getThresholdValue() );
	    fControl[index]->getData()->
	      setHiThreshold( autoResponseDialog->getHiThresholdValue() );
	    fControl[index]->getData()->
	      setPatternThreshold( autoResponseDialog->
				   getPatternThresholdValue() );
	    fControl[index]->getData()->
	      setPatternMultiplicity( autoResponseDialog->
				      getPatternMultiplicityValue() );
	    fControl[index]->getData()
	      ->setCheckTime( autoResponseDialog->getTimeValue() );
	    fControl[index]->setRecentTimeLimit( autoResponseDialog->getRecentTimeValue() );
	    fControl[index]->getData()
	      ->setRestoreMargin( autoResponseDialog->getRestoreMarginValue() );
	    fControl[index]->getData()
	      ->setPatternRestoreMargin( autoResponseDialog->
					 getPatternRestoreMarginValue() );
	    kAutoResponse[index] = autoResponseDialog->isAutoCheckBoxChecked();
	    kStarRestore[index] = autoResponseDialog
	      ->isRestoreCheckBoxChecked();
	    fControl[index]->getVHVClient()->setAutoResponseParams();
	  } // end if accepted
	  delete autoResponseDialog;
	} // end if active
      } // end if NUMTEL
      // Create dialog box for AutoResponse parameters.
      // Show dialog box, which returns when closed.
      // Check how user has closed the dialog.  Set parameter values.
      break;
    } // end case MENU_AUTORESPONSE
  case MENU_FEEDBACK:
    {
      int index = fTab->currentPageIndex();
      if ( index == NUMTEL ) {
	for ( int i=0; i<NUMTEL; ++i ) {
	  if ( kActiveTelescope[i] ) {
	    fControl[i]->getVHVClient()->getFeedbackParams();
	    char tmp[200];
	    sprintf( tmp, "T%d Feedback Params", i+1 );
	    // Create dialog box for Feedback parameters.
	    VFeedbackDialog* feedbackDialog = 
	      new VFeedbackDialog( this, tmp, i );
	    // Show dialog box, which returns when closed.
	    int ret = feedbackDialog->exec();
	    // Check how user has closed the dialog.  Set parameter values.
	    if ( ret == QDialog::Accepted ) {
	      fControl[i]->getData()
		->setThreshold( feedbackDialog->getThresholdValue() );
	      fControl[i]->getData()->
		setHiThreshold( feedbackDialog->getHiThresholdValue() );
	      fControl[i]->getData()->
		setPatternThreshold( feedbackDialog->
				     getPatternThresholdValue() );
	      fControl[i]->getData()->
		setPatternMultiplicity( feedbackDialog->
					getPatternMultiplicityValue() );
	      cout << "New RestoreMarginValue is "
		   << feedbackDialog->getRestoreMarginValue() << endl;
	      fControl[i]->getData()
		->setRestoreMargin( feedbackDialog->getRestoreMarginValue() );
	      fControl[i]->getData()
		->setMeasurementsPerCycle( feedbackDialog->getMeasurementsPerCycleValue() );
	      fControl[i]->getData()
		->setMinTimeBeforeRestore( feedbackDialog->getMinTimeBeforeRestoreValue() );
	      bool needToUpdateLevels = false;
	      if ( feedbackDialog->getNumberOfFeedbackLevelsValue() !=
		   fControl[i]->getData()->getNumberOfFeedbackLevels() ) {
		fControl[i]->getData()
		  ->setNumberOfFeedbackLevels( feedbackDialog->getNumberOfFeedbackLevelsValue() );
		needToUpdateLevels = true;
	      }
	      if ( fabs( feedbackDialog->getGainScalingValue() -
			 fControl[i]->getData()->getGainScaling() ) > 0.001 ) {
		fControl[i]->getData()
		  ->setGainScaling( feedbackDialog->getGainScalingValue() );
		needToUpdateLevels = true;
	      }
	      if ( fabs( feedbackDialog->getSafetyFactorValue() -
			 fControl[i]->getData()->getSafetyFactor() ) > 0.001 ) {
		fControl[i]->getData()
		  ->setSafetyFactor( feedbackDialog->getSafetyFactorValue() );
		needToUpdateLevels = true;
	      }
	      if ( needToUpdateLevels ) {
		fControl[i]->globalRefreshVoltageLevels();
	      }
	      fControl[i]->getData()
		->setRestoreAttemptTime( feedbackDialog->getRestoreAttemptTimeValue() );
	      fControl[i]->getData()
		->setMinAbsPositionChange( feedbackDialog->getMinAbsPositionChangeValue() );
	      fControl[i]->getData()
		->setPatternDistanceDrop( feedbackDialog->getPatternDistanceDropValue() );
	      fControl[i]->getData()
		->setPSF( feedbackDialog->getPSFValue() );
	      fControl[i]->getData()
		->setCyclesPerFeedbackTest( feedbackDialog->getCyclesPerFeedbackTestValue() );
	      //fControl[i]->getData()
	      //	->setPMTIndex( feedbackDialog->getPMTIndexValue() );
	      if ( feedbackDialog->isFeedbackChecked() ) {
		fControl[i]->globalFeedbackOn();
	      } else {
		fControl[i]->globalFeedbackOff();
	      }
	      fControl[i]->getData()
		->setPMTIndex( feedbackDialog->getPMTIndexValue() );
	      kOldSchool[i] = feedbackDialog->isOldSchoolChecked();
	      fControl[i]->getVHVClient()->setFeedbackParams();
	    } // end if accepted
	    delete feedbackDialog;
	  } // end if active
	} // end for NUMTEL
      } else {
	if ( kActiveTelescope[index] ) {
	  fControl[index]->getVHVClient()->getFeedbackParams();
	  char tmp[200];
	  sprintf( tmp, "T%d Feedback Params", index+1 );
	  // Create dialog box for Feedback parameters.
	  VFeedbackDialog* feedbackDialog = 
	    new VFeedbackDialog( this, tmp, index );
	  // Show dialog box, which returns when closed.
	  int ret = feedbackDialog->exec();
	  // Check how user has closed the dialog.  Set parameter values.
	  if ( ret == QDialog::Accepted ) {
	    fControl[index]->getData()
	      ->setThreshold( feedbackDialog->getThresholdValue() );
	    fControl[index]->getData()->
	      setHiThreshold( feedbackDialog->getHiThresholdValue() );
	    fControl[index]->getData()->
	      setPatternThreshold( feedbackDialog->
				   getPatternThresholdValue() );
	    fControl[index]->getData()->
	      setPatternMultiplicity( feedbackDialog->
				      getPatternMultiplicityValue() );
	      cout << "New RestoreMarginValue is "
		   << feedbackDialog->getRestoreMarginValue() << endl;
	    fControl[index]->getData()
	      ->setRestoreMargin( feedbackDialog->getRestoreMarginValue() );
	    fControl[index]->getData()
	      ->setMeasurementsPerCycle( feedbackDialog->getMeasurementsPerCycleValue() );
	    fControl[index]->getData()
	      ->setMinTimeBeforeRestore( feedbackDialog->getMinTimeBeforeRestoreValue() );
	    bool needToUpdateLevels = false;
	    if ( feedbackDialog->getNumberOfFeedbackLevelsValue() !=
		 fControl[index]->getData()->getNumberOfFeedbackLevels() ) {
	      fControl[index]->getData()
		->setNumberOfFeedbackLevels( feedbackDialog->getNumberOfFeedbackLevelsValue() );
	      needToUpdateLevels = true;
	    }
	    if ( fabs( feedbackDialog->getGainScalingValue() -
		       fControl[index]->getData()->getGainScaling() ) > 0.001 ) {
	      fControl[index]->getData()
		->setGainScaling( feedbackDialog->getGainScalingValue() );
	      needToUpdateLevels = true;
	    }
	    if ( fabs( feedbackDialog->getSafetyFactorValue() -
		       fControl[index]->getData()->getSafetyFactor() ) > 0.001 ) {
	      fControl[index]->getData()
		->setSafetyFactor( feedbackDialog->getSafetyFactorValue() );
	      needToUpdateLevels = true;
	    }
	    if ( needToUpdateLevels ) 
	      fControl[index]->globalRefreshVoltageLevels();
	    fControl[index]->getData()
	      ->setRestoreAttemptTime( feedbackDialog->getRestoreAttemptTimeValue() );
	    fControl[index]->getData()
	      ->setMinAbsPositionChange( feedbackDialog->getMinAbsPositionChangeValue() );
	    fControl[index]->getData()
	      ->setPatternDistanceDrop( feedbackDialog->getPatternDistanceDropValue() );
	    fControl[index]->getData()
	      ->setPSF( feedbackDialog->getPSFValue() );
	    fControl[index]->getData()
	      ->setCyclesPerFeedbackTest( feedbackDialog->getCyclesPerFeedbackTestValue() );
	    //fControl[index]->getData()
	    //  ->setPMTIndex( feedbackDialog->getPMTIndexValue() );
	    if ( feedbackDialog->isFeedbackChecked() ) {
	      fControl[index]->globalFeedbackOn();
	    } else {
	      fControl[index]->globalFeedbackOff();
	    }
	    fControl[index]->getData()
	      ->setPMTIndex( feedbackDialog->getPMTIndexValue() );
	    kOldSchool[index] = feedbackDialog->isOldSchoolChecked();
	    fControl[index]->getVHVClient()->setFeedbackParams();
	  } // end if accepted
	  delete feedbackDialog;
	} // end if active
      } // end else on index
      break;
    } // end case MENU_FEEDBACK
      //   case MENU_BROKEN:
      //     {
      //       QFrame *popup = new QFrame( this ,0, WType_Popup);
      //       popup->setFrameStyle( QFrame::WinPanel | QFrame::Raised );
      //       popup->resize(130,100);
      
      //       fLineInput = new QLineEdit( popup );
      //       connect( fLineInput, SIGNAL( returnPressed() ), this, 
      // 	       SLOT( markDisabled() ));
      //       fLineInput->setGeometry(10, 10, 110, 30);
      //       fLineInput->setValidator(new VIntValidator(this, "", 0, 500));
      //       fLineInput->setFocus();
      
      //       QPushButton *b = new QPushButton( "Enter Pixel to Disable", popup );
      //       b->setGeometry(10, 50, 110, 30);
      //       connect( b, SIGNAL( clicked() ), this, SLOT( markDisabled() ) );
      
      //       popup->move(QCursor::pos());
      //       popup->show();
      //       break;
      //     } // end case MENU_BROKEN
      //   case MENU_UNBROKEN:
      //     {
      //       QFrame *popup = new QFrame( this ,0, WType_Popup);
      //       popup->setFrameStyle( QFrame::WinPanel | QFrame::Raised );
      //       popup->resize(130,100);
      
      //       fLineInput = new QLineEdit( popup );
      //       connect( fLineInput, SIGNAL( returnPressed() ), this, 
      // 	       SLOT( unmarkDisabled() ));
      //       fLineInput->setGeometry(10, 10, 110, 30);
      //       fLineInput->setValidator(new VIntValidator(this, "", 0, 500));
      //       fLineInput->setFocus();
      
      //       QPushButton *b = new QPushButton( "Enter Pixel to Enable", popup );
      //       b->setGeometry(10, 50, 110, 30);
      //       connect( b, SIGNAL( clicked() ), this, SLOT( unmarkDisabled() ) );
      
      //       popup->move(QCursor::pos());
      //       popup->show();
      //       break;
      //     } // end case MENU_UNBROKEN
      //   case MENU_FLATFIELD:
      //     {
      //       // Create dialog box for FlatField parameters.
      //       VFlatFieldDialog* flatFieldDialog =
      // 	new VFlatFieldDialog( this );
      //       // Show dialog box, which returns when closed.
      //       int ret = flatFieldDialog->exec();
      //       break;
      //     } // end case MENU_FLATFIELD
  case MENU_HILOINNER: {
    int index = fTab->currentPageIndex();
    if ( index == NUMTEL ) {
      for ( int i=0; i<NUMTEL; ++i ) {
	fControl[i]->setUpHiLoInner();
      }
    } else {
      fControl[index]->setUpHiLoInner();
    }
    break;
  } // end case MENU_HILOINNER
  case MENU_HILOOUTER: {
    int index = fTab->currentPageIndex();
    if ( index == NUMTEL ) {
      for ( int i=0; i<NUMTEL; ++i ) {
	fControl[i]->setUpHiLoOuter();
      }
    } else {
      fControl[index]->setUpHiLoOuter();
    }
    break;
  } // end case MENU_HILOOUTER
  } // end switch
} // end VHVFrame::hvParamsMenu(int id)


  /** Called when the "Scale Voltages" element of the HVParams menu is selected.
   **/
void VHVFrame::scaleVoltages()
{
  cout << "in VHVFrame::scaleVoltages()" <<endl;
  int index = fTab->currentPageIndex();
  bool ok;
  int pos;
  float percent;

  QString t = fLineInput->text();
  if (fLineInput->validator()->
      validate(t,pos) != QValidator::Acceptable) return;
  
  percent = t.toFloat(&ok);
  QFrame *popup = (QFrame *)fLineInput->parent();
  popup->close();
  
  if ( index == NUMTEL ) {
    for ( int i=0; i<NUMTEL; ++i ) {
      fControl[i]->scaleVoltages( percent );
    }
  } else {
    fControl[index]->scaleVoltages( percent );
  }
  fChannelReport->update();
  fBoardReport->update();  

}

/** Called when the "Offset Voltages" element of the HVParams menu is selected.
 **/
void VHVFrame::offsetVoltages()
{
  int index = fTab->currentPageIndex();
  bool ok;
  int pos;
  int voltage;

  QString t = fLineInput->text();

  if (fLineInput->validator()->
      validate(t,pos) != QValidator::Acceptable) return;
  
  voltage = t.toFloat(&ok);
  QFrame *popup = (QFrame *)fLineInput->parent();
  popup->close();
  
  if ( index == NUMTEL ) {
    for ( int i=0; i<NUMTEL; ++i ) {
      fControl[i]->offsetVoltages( voltage );
    }
  } else {
    fControl[index]->offsetVoltages( voltage );
  }

  fChannelReport->update();
  fBoardReport->update();  

}


/** Called when the "SetAll Voltages" element of the HVParams menu is selected.
 **/
void VHVFrame::setAllVoltages()
{
  int index = fTab->currentPageIndex();
  bool ok;
  int pos;
  int voltage;

  QString t = fLineInput->text();

  if (fLineInput->validator()->
      validate(t,pos) != QValidator::Acceptable) return;
  
  voltage = t.toFloat(&ok);
  QFrame *popup = (QFrame *)fLineInput->parent();
  popup->close();
  
  if ( index == NUMTEL ) {
    for ( int i=0; i<NUMTEL; ++i ) {
      fControl[i]->setAllVoltages( voltage );
    }
  } else {
    fControl[index]->setAllVoltages( voltage );
  }

  fChannelReport->update();
  fBoardReport->update();  

}

/** Called when the "Disable Pixel" element of the HVParams menu 
 *  is selected.
 **/
/** Called when the "Disable Pixel" element of the context menu 
 *  is selected.
 **/
void VHVFrame::markDisabled()
{
  //   bool ok;
  //   int pos;
  //   QString t = fLineInput->text();

  //   if (fLineInput->validator()->
  //       validate(t,pos) != QValidator::Acceptable) return;
  //   QFrame *popup = (QFrame *)fLineInput->parent();
  //   popup->close();
  //   fControl[0]->markDisabled( t.toInt( &ok, 10 ) );
  //   fChannelReport->update();
  //   fBoardReport->update();  
  fControl[fSelectedPMT->getTelescopeID()]
    ->markDisabled( fSelectedPMT->getChannel() );
}


/** Called when the "Disable Pixel" element of the HVParams menu 
 *  is selected.
 **/
/** Called when the "Disable Pixel" element of the context menu 
 *  is selected.
 **/
void VHVFrame::unmarkDisabled()
{
  //   bool ok;
  //   int pos;
  //   QString t = fLineInput->text();

  //   if (fLineInput->validator()->
  //       validate(t,pos) != QValidator::Acceptable) return;
  //   QFrame *popup = (QFrame *)fLineInput->parent();
  //   popup->close();
  //   fControl[0]->unmarkDisabled( t.toInt( &ok, 10 ) );
  //   fChannelReport->update();
  //   fBoardReport->update();  
  fControl[fSelectedPMT->getTelescopeID()]
    ->unmarkDisabled( fSelectedPMT->getChannel() );
}


void VHVFrame::markAsNoAuto()
{
  fControl[fSelectedPMT->getTelescopeID()]
    ->markPixelNoAuto( fSelectedPMT->getChannel() );
}


void VHVFrame::unmarkAsNoAuto()
{
  fControl[fSelectedPMT->getTelescopeID()]
    ->unmarkPixelNoAuto( fSelectedPMT->getChannel() );
}


/** Called when an item on the star menu is selected
 * \arg \c id - Menu item identifer (::EnumMenu)
 */
void VHVFrame::starMenu(int id)
{

  int index = fTab->currentPageIndex();
  double mag;

  switch (id) {
  case MENU_SHOW_STARS:
    if ( index == NUMTEL ) {
      for ( int i=0; i<NUMTEL; ++i ) {
	fControl[i]->drawStars(); 
      }
    } else {
      fControl[index]->drawStars();
    }
    break;
  case MENU_HIDE_STARS:
    if ( index == NUMTEL ) {
      for ( int i=0; i<NUMTEL; ++i ) {
	fControl[i]->hideStars(); 
      }
    } else {
      fControl[index]->hideStars();
    }
    break;
  case MENU_SHOW_ARCS:
    if ( index == NUMTEL ) {
      for ( int i=0; i<NUMTEL; ++i ) {
	fControl[i]->showArcs(); 
      }
    } else {
      fControl[index]->showArcs();
    }
    break;
  case MENU_HIDE_ARCS:
    if ( index == NUMTEL ) {
      for ( int i=0; i<NUMTEL; ++i ) {
	fControl[i]->hideArcs(); 
      }
    } else {
      fControl[index]->hideArcs();
    }
    break;
  case MENU_CHANGE_MAG:
    mag=5.0;
    fControl[0]->changeMag( mag );
    break;
  }
}

/** Add text to the message window.
 * \arg \c mess - Message to add
 */
void VHVFrame::sendMessage(const QString& mess) 
{ 
  //   const int buffer = 1000;
  time_t tmpTime;
  time( &tmpTime );
  char tmpString[200];
  strftime( tmpString, 200,
            "%T:  ",
            gmtime( &tmpTime ) );
  QString t = fMessages->text() + QString( tmpString ) + mess;
  //  QString t = fMessages->text() + mess;
  //   if (t.length() > buffer) {
  //     int start = t.find("",t.length()-buffer);
  //     t.remove(0,start);
  //   }
  fMessages->setText( t );
  fMessages->sync();
  int width, height;
  width = fMessages->contentsWidth();
  height = fMessages->contentsHeight();
  //   cout << "(width, height) = (" << width << ", " << height 
  //        << ") visible height = " << fMessages->visibleHeight() << endl;
  fMessages->ensureVisible( width, height );
  kLogFile << mess << endl;
  // QScrollBar *sb = fMessages->verticalScrollBar();
  // sb->setValue(sb->maxValue());  
  // sb->setValue(sb->maxValue()*10);  
  
}


/** Called when a display option button is clicked
 * \arg \c id - Display option identifer (::EnumDisplay)
 */
void VHVFrame::optionButtonClicked(int id)
{
  int index = fTab->currentPageIndex();  
  fDisplayOption = id;
  cout << "switching to button ID " << fDisplayOption << endl;
  if ( index == NUMTEL ) {
    for ( int i=0; i<NUMTEL; ++i ) {
      fControl[i]->redraw();
    }
  } else {
    fControl[index]->redraw();
  }
}


/** Return the currently selected display option (::EnumDisplay)
 */
int VHVFrame::getDisplayOpt()
{
  return( fDisplayOption );
}


/** Popup box to type in a PMT number for selection
 */
void VHVFrame::menuPmtSelect( int index ) {
  
  //  fSelectedPMT = 0;

  QFrame *popup = new QFrame( this ,0, WType_Popup);
  popup->setFrameStyle( QFrame::WinPanel | QFrame::Raised );
  popup->resize(130,100);
    
  fLineInput = new QLineEdit( popup );
  connect( fLineInput, SIGNAL( returnPressed() ), this, SLOT( textInputChannel() ));
  fLineInput->setGeometry(10, 10, 110, 30);
  fLineInput->setValidator(new VIntValidator(this, "", 0, fControl[index]->getNumberOfPMTs()));
  fLineInput->setFocus();
  
  QPushButton *b = new QPushButton( "Select PMT", popup );
  b->setGeometry(10, 50, 110, 30);
  connect( b, SIGNAL( clicked() ), this, SLOT( textInputChannel() ) );
  
  popup->move(QCursor::pos());
  popup->show();
  
}


/** Left or right mouse click on a PMT icon
 * - left - select this PMT
 * - right - pull down a context menu
 * \arg \c p - The selected VPMTIcon
 * \arg \c e - The Qt mouse click event
 * \arg \c index - The selected telescope
 */
void VHVFrame::pmtSelect(VPMTIcon *p, QMouseEvent *e, int index ) 
{
  fSelectedPMT = p;
  if ( p->getTelescopeID() == index ) {
    fControl[index]->drawSelected(p);
    fChannelReport->update();  
    fBoardReport->update();   
    if (e->button() == RightButton) contextMenu();
  }
}


/** Pull down a PMT context menu (on right mouse button click) 
 */
void VHVFrame::contextMenu()
{
  
  QPopupMenu* context = new QPopupMenu(this); 
  
  char text[20];
  int ch = fSelectedPMT->getChannel();
  int index = fSelectedPMT->getTelescopeID();

  sprintf(text,"T%d  Pixel %d ", index+1, ch+1 );
  QLabel *label = new QLabel( text, this );
  context->insertItem(label,MENU_NO_OPT);
  context->insertSeparator();

  if (fSelectedPMT->getData()->getPower(ch) == ON) {
    context->insertItem("Power &Down",MENU_DOWN);
  } else {
    context->insertItem("Power &Up",MENU_UP); 
  }
  context->insertItem("Set &Voltage",MENU_VOLTS);
  if (fSelectedPMT->getData()->getSuppressed(ch) == RESTORED) {
    context->insertItem("Suppress &Channel",MENU_SUPPRESS);
  } else {
    context->insertItem("Restore &Channel",MENU_RESTORE);
  }  
  if ( fSelectedPMT->getData()->getDisabled( ch ) ) {
    context->insertItem( "UnMark Disabled", MENU_UNBROKEN );
  } else {
    context->insertItem( "Mark Disabled", MENU_BROKEN );
  }
  if ( fSelectedPMT->getData()->getAutoStatus( ch ) ) {
    context->insertItem( "Enable AutoResponse", MENU_YESAUTO );
  } else {
    context->insertItem( "Disable AutoResponse", MENU_NOAUTO );
  }
  connect( context, SIGNAL(activated(int)),
	   this, SLOT(contextAction(int)) );
  
  context->exec(QCursor::pos());
  
}


/** Take action based on a PMT context menu selection.
 * \arg \c id - Menu item indentifier (::EnumMenu)
 */
void VHVFrame::contextAction( int id )
{
  int ch = fSelectedPMT->getChannel();
  int index = fSelectedPMT->getTelescopeID();

  switch( id ) {
  case MENU_UP:
    fControl[index]->setPower( ch, ON );
    break;
  case MENU_DOWN:
    fControl[index]->setPower( ch, OFF );
    break;
  case MENU_SUPPRESS:  
    fControl[index]->suppressPixel( ch );    
    break;
  case MENU_RESTORE:
    fControl[index]->restorePixel( ch );    
    break;
  case MENU_VOLTS: {
    QFrame *popup = new QFrame( this ,0, WType_Popup);
    popup->setFrameStyle( QFrame::WinPanel | QFrame::Raised );
    popup->resize(130,100);
    fLineInput = new QLineEdit( popup );
    connect( fLineInput, SIGNAL( returnPressed() ), 
	     this, SLOT( textInputVoltage() ));
    fLineInput->setGeometry(10, 10, 110, 30);
    fLineInput->setValidator(new VIntValidator(this, "", MINVOLTS, MAXVOLTS));
    fLineInput->setFocus();
    QPushButton *b = new QPushButton( "Set Voltage", popup );
    b->setGeometry(10, 50, 110, 30);
    connect( b, SIGNAL( clicked() ), this, SLOT( textInputVoltage() ) );
    popup->move(QCursor::pos());
    popup->show();
    break;
  } // end case MENU_VOLTS
  case MENU_BROKEN: {
    markDisabled();
    break;
  } // end case MENU_BROKEN
  case MENU_UNBROKEN: {
    unmarkDisabled();
    break;
  } // end case MENU_UNBROKEN
  case MENU_NOAUTO:
    {
      markAsNoAuto();
      break;
    } // end case MENU_NOAUTO
  case MENU_YESAUTO:
    {
      unmarkAsNoAuto();
      break;
    } // end case MENU_YESAUTO
  }
 
  fChannelReport->update();
  fBoardReport->update();  
}

/** Process text input. Used for setting voltage.
 */
void VHVFrame::textInputVoltage()
{

  bool ok;
  int pos;
  
  QString t = fLineInput->text();

  if (fLineInput->validator()->
      validate(t,pos) != QValidator::Acceptable) return;
  
  // This is a set voltage text input
  //   int index = fTab->currentPageIndex();
  int index = fSelectedPMT->getTelescopeID();
  int ch = fSelectedPMT->getChannel();
  fControl[index]->pixelSetVReq( ch, t.toInt(&ok,10) );
  
  QFrame *popup = (QFrame *)fLineInput->parent();
  popup->close();
  
  fChannelReport->update();
  fBoardReport->update();  
}

/** Process text input. Used for select-channel text.
 */
void VHVFrame::textInputChannel()
{
  int index = fTab->currentPageIndex();
  if ( index == NUMTEL ) { // ambiguous case, so use telescope of currently
    // selected PMT.
    index = fSelectedPMT->getTelescopeID();
  }
  bool ok;
  int pos;
  
  QString t = fLineInput->text();

  if (fLineInput->validator()->
      validate(t,pos) != QValidator::Acceptable) return;
  
  // This is a select channel text input
  VPMTIcon* p = fControl[index]->getPMT(t.toInt(&ok,10)-1);
  QPoint posMouse;
  QMouseEvent *mouse = new QMouseEvent(QEvent::MouseButtonPress, 
				       posMouse, 0, 0);
  emit pmtSelect( p, mouse, index );
  
  QFrame *popup = (QFrame *)fLineInput->parent();
  popup->close();
  
  fChannelReport->update();
  fBoardReport->update();  
}


/** Return pointer to the currently selected PMT (VPMTIcon).
 */
VPMTIcon* VHVFrame::getSelected() { return( fSelectedPMT ); }


/** Update the channel and board reports, status LEDs and update and 
 * redraw the camera display.
 */
void VHVFrame::updateAll()
{
  time_t tmpTime;
  time( &tmpTime );
  char tmpString[200];
  strftime( tmpString, 200,
	    "%Y%m%d %T",
	    gmtime( &tmpTime ) );
  kLogFile << tmpString << ": updateAll(): ";
  //   cout << tmpString << ": updateAll(): " << endl;

  // Read latest info from individual telescopes.
  for ( int i=0; i<NUMTEL; ++i ) {
    fControl[i]->updateData();
  }
  // Update right-hand control/display.
  fTelStatus->update();
  fChannelReport->update();
  fBoardReport->update();  
  fStarPanel->update();
  // Redraw camera(s).
  int index = fTab->currentPageIndex();
  if ( index == NUMTEL ) {
    for ( int i=0; i<NUMTEL; ++i ) {
      if ( kActiveTelescope[i] ) {
	fControl[i]->redraw();
	fCamInfo[i]->redraw();
	for ( int j=0; j<MAX_PMTS; ++j ) {
	  fPMTIndicator[i][j]->redraw();
	}
      }
    }
  } else {
    if ( kActiveTelescope[index] ) {
      fControl[index]->redraw();
      fCamInfo[index]->redraw();
      for ( int j=0; j<MAX_PMTS; ++j ) {
	fPMTIndicator[index][j]->redraw();
      }
    }
  }
  // Check that currents are still updating on individual telescopes.
  checkCurrentsChanged();
  // Make sure display reads IPMT (if it's been away for more than 1 minute)
  if ( fDisplayOption != IPMT ){
    if ( fDisplayNotIPMTFlag ) {
      if ( fStartTimeNotIPMT+60 < time( 0 ) ) {
	// Time to return to IPMT display
	fBG->setButton( IPMT );
	optionButtonClicked( IPMT );
	sendMessage( "Returning to PMT Current Display.<br>" );
	fDisplayNotIPMTFlag = false;
      }
    } else {
      fDisplayNotIPMTFlag = true;
      fStartTimeNotIPMT = time( 0 );
    }
  } else {
    fDisplayNotIPMTFlag = false;
  }
  // Update time.
  fUpdateTime->update();
  kLogFile << endl;
}


/** Returns a pointer to the currently active fControl object.
 */
VHVControl* VHVFrame::getControl()
{
  int index = fTab->currentPageIndex();
  if ( index == NUMTEL ) {
    index = fSelectedPMT->getTelescopeID();
  }
  return fControl[index];
}


/** Returns a pointer to the currently active fControl object.
 */
VHVControl* VHVFrame::getControl( int index )
{
  if ( index == NUMTEL ) {
    index = fSelectedPMT->getTelescopeID();
  }
  return fControl[index];
}


/** Returns state of flag telling whether still in VHVFrame constructor.
 */
bool VHVFrame::getStillInConstructor()
{
  return fStillInConstructor;
}


/** Check that PMT currents are updating by assuming that the average will
 *  not stay constant to better than 1e-4 over 10 seconds.
 */
void VHVFrame::checkCurrentsChanged() 
{ 
  for ( int i=0; i<NUMTEL; ++i ) 
    {
      if ( kActiveTelescope[i] ) 
	{
	  // Update currents.
	  for ( int k=0; k<MAX_PMTS; ++k )
	    {
	      for ( int j=0; j<CURRSAMPLES-1; ++j ) 
		{
		  fCurrent[i][k][j] = fCurrent[i][k][j+1];
		}
	      fCurrent[i][k][CURRSAMPLES-1] = fControl[i]->getData()->getCurrent(k);
	    }
	  // Test currents.
	  //       cout << i << "  " << time(0) << "  " << fCurrentWarningTime[i] 
	  // 	   << "  " << time(0) - fCurrentWarningTime[i] << endl;
	  //      if ( ) {
	  if ( time(0) - fCurrentWarningTime[i] > CURRSAMPLES ) 
	    {
	      bool changed = false;
	      for ( int k=0; k<MAX_PMTS; ++k ) 
		{
		  for ( int j=0; j<CURRSAMPLES-1; ++j ) 
		    {
		      if ( abs( fCurrent[i][k][j] - fCurrent[i][k][j+1] ) > 1.e-5 ) 
			{
			  changed = true;
			  break;
			}
		    }
		  if ( changed == true ) break;
		}
	      // Send a warning if currents not changed.
	      if ( !changed && i!=0 ) {
		ostringstream os;
		os.str("");
		os << "!!! Tel " << i+1 << ": Currents unchanged in last "
		   << CURRSAMPLES/2 << " seconds!  Check vhv-vdcmon communcations "
		   << "and restart vhv if necessary. !!!<br>";
		sendMessage( QString( os.str().c_str() ) );
		fCurrentWarningTime[i] = time( 0 );
		fCamInfo[i]->drawAlert();
	      }
	      else if ( !changed && i==0 ) {
		ostringstream os;
		os.str("");
		os << "Tel " << i+1 << ": Currents unchanged in last "
		   << CURRSAMPLES/2 << " seconds, but this is not unusual - verify T1 vhv gui is updating; if not, restart between runs. If it is, then likely a currmon hw problem.<br>";
		sendMessage( QString( os.str().c_str() ) );
		fCurrentWarningTime[i] = time( 0 );
		fCamInfo[i]->drawAlert();
	      }
	      else {
		fCamInfo[i]->hideAlert();
	      }
	    }
	}
    }
}


void VHVFrame::setStatusPeriod()
{
  bool ok;
  int pos;
  int period;

  QString t = fLineInput->text();

  if (fLineInput->validator()->
      validate(t,pos) != QValidator::Acceptable) return;
  
  period = t.toInt(&ok,10);
  QFrame *popup = (QFrame *)fLineInput->parent();
  popup->close();
  
  for ( int i=0; i<NUMTEL; ++i ) 
    {
      if ( kActiveTelescope[i] ) 
        {
          fControl[i]->getData()->setStatusPeriod( period );
        }
    }
}



/*****************************************************************************/

/** \class VIntValidator VHVFrame.h
    \brief A simple class inherited from QValidator to accept an
    integer in the range minval to maxval.
*/

/** Constructor - define minimum and maximum values.
 * \arg \c parent - Qt parent widget
 * \arg \c name   - Widget indentifier
 * \arg \c minval - Minimum allowable value
 * \arg \c maxval - Maximum allowable value
 */
VIntValidator::VIntValidator(QWidget *parent, const char * name,
			     int minval, int maxval) : QValidator(parent,name) 
{

  fMin = minval;
  fMax = maxval;
  
}

/** Destructor
 */
VIntValidator::~VIntValidator() { }

/** Reset minimum and maximum values.
 * \arg \c minval - Minimum allowable value
 * \arg \c maxval - Maximum allowable value
 */
void VIntValidator::setup(int minval, int maxval) 
{

  fMin = minval;
  fMax = maxval;

}

/** Validation function - return a QValidator state depending on
 * the acceptablity of the input string.
 * \arg \c str - String to be parsed
 * \arg \c pos - Position of cursor in the string
 */
QValidator::State VIntValidator::validate( QString &str, int &pos) const
{
  bool ok;
  int value = str.toInt( &ok, 10 );
  QValidator::State state;
  if (ok == FALSE) state = Invalid;
  if ((value <= fMax) && (value >= fMin)) {
    state = Acceptable;
  } else {
    state = Intermediate;
  }

  fflush(stdout);

  return(state);
}

/*****************************************************************************/
/*****************************************************************************/

/** \class VFloatValidator VHVFrame.h
    \brief A simple class inherited from QValidator to accept a
    floating point value in the range minval to maxval.
*/

/** Constructor - define minimum and maximum values.
 * \arg \c parent - Qt parent widget
 * \arg \c name   - Widget indentifier
 * \arg \c minval - Minimum allowable value
 * \arg \c maxval - Maximum allowable value
 */
VFloatValidator::VFloatValidator(QWidget *parent, const char * name,
				 double minval, double maxval) 
  : QValidator(parent,name) 
{
  
  fMin = minval;
  fMax = maxval;
  
}

/** Destructor
 */
VFloatValidator::~VFloatValidator() { }

/** Reset minimum and maximum values.
 * \arg \c minval - Minimum allowable value
 * \arg \c maxval - Maximum allowable value
 */
void VFloatValidator::setup(double minval, double maxval) 
{

  fMin = minval;
  fMax = maxval;

}

/** Validation function - return a QValidator state depending on
 * the acceptablity of the input string.
 * \arg \c str - String to be parsed
 * \arg \c pos - Position of cursor in the string
 */
QValidator::State VFloatValidator::validate( QString &str, int &pos) const
{
  bool ok;
  double value = str.toDouble( &ok );
  QValidator::State state;
  if (ok == FALSE) state = Invalid;
  if ((value <= fMax) && (value >= fMin)) {
    state = Acceptable;
  } else {
    state = Intermediate;
  }

  fflush(stdout);

  return(state);
}

/*****************************************************************************/

/** \class VChannelReport VHVFrame.h
    \brief A widget to display information on the currently selected channel
*/

/** Constructor - generates a channel report panel
 * \arg \c mf     - Frame on which to draw the panel
 * \arg \c parent - Qt parent widget
 * \arg \c name   - Widget indentifier
 */
VChannelReport::VChannelReport(VHVFrame *mf, QWidget* parent, const char* name) 
  : QGroupBox(parent,name)
{
  
  fMainFrame = mf;
  
  setTitle("No channel selected");  
  QFont aFont( "Helvetica", 12, QFont::Normal );
  //setFont( aFont );

  //  QGridLayout* layout = new QGridLayout(this,2,3,15);
  QGridLayout* layout = new QGridLayout(this,4,4,5);
  layout->setRowSpacing( 0, 15 );
  fVoltBox = new QSpinBox( MINVOLTS, MAXVOLTS, 10,
			   this, "voltage selection" );
  fVoltBox->setValue( MINVOLTS );
  fVoltBox->setMinimumSize( fVoltBox->sizeHint() );
  fVoltBox->setMaximumHeight( fVoltBox->minimumSize().height() );  
  connect( fVoltBox, SIGNAL( valueChanged(int) ), 
	   this, SLOT( voltageChanged(int) ) );

  QLabel *label = new QLabel("      VReq",this);
  layout->addWidget( label,   1, 0 );
  layout->addWidget( fVoltBox, 1, 1 );
  
  fVSetLabel = new QLabel("VSet      0 V",this);
  layout->addWidget( fVSetLabel, 1, 2 );
  
  fVTrueLabel = new QLabel("VTru      0 V",this);
  layout->addWidget( fVTrueLabel, 1, 3 );
  
  //label = new QLabel("Current",this);
  fCurrentLabel = new QLabel("0.0 uA",this);
  //layout->addWidget( label, 2, 0 );
  layout->addWidget( fCurrentLabel, 2, 0 );

  fStatusLabel = new QLabel("OK",this);
  layout->addWidget( fStatusLabel, 2, 1 );

  fFeedbackLabel = new QLabel( "Gain   100%", this );
  layout->addWidget( fFeedbackLabel, 2, 2 );

  //fPowerButton = new QPushButton("On/Off",this);
  fPowerButton = new QPushButton("Power",this);
  fPowerButton->setToggleButton( TRUE );
  // label = new QLabel("Power",this);
  // layout->addWidget( label, 2, 0 );
  //layout->addWidget( fPowerButton, 2, 1 );
  layout->addWidget( fPowerButton, 2, 3 );
  connect( fPowerButton, SIGNAL( clicked() ), this, SLOT( powerChanged() ) );
  // connect( fPowerButton, SIGNAL( clicked() ), this, SLOT( powerChanged() ) );

  update();

}

/** Destructor 
 */
VChannelReport::~VChannelReport()
{

  delete fVoltBox;
  delete fVTrueLabel;
  delete fCurrentLabel;
  delete fPowerButton;
  delete fVSetLabel;
  delete fFeedbackLabel;

}

/** Update the information displayed on the panel
 */
void VChannelReport::update()
{

  VPMTIcon *p = fMainFrame->getSelected();
  
  char str[40];
  int ch = p->getChannel();
  int index = p->getTelescopeID();
  VHVData *data = p->getData();

  int hvchan,hvslot,hvcrate;   
  data->getCrateAndSlot(ch,hvcrate,hvchan,hvslot);
  sprintf(str," T%d Pixel %3d  (Crate %2d, Module %2d, Channel %2d)", 
	  index+1, ch+1, hvcrate, hvslot, hvchan );  
  setTitle(str);

  sprintf(str,"Curr  %6.1f uA",data->getCurrent(ch));
  fCurrentLabel->setText(str);
  sprintf(str,"VSet  %6d V",data->getVSet(ch));
  fVSetLabel->setText(str);
  sprintf(str,"VTrue  %6d V",data->getVTrue(ch));
  fVTrueLabel->setText(str);
  if ( data->getFeedbackLevel( ch ) <= data->getNumberOfFeedbackLevels() ) {
    sprintf( str, "Gain  %5d\%", 
	     (int)(100*pow( data->getGainScaling(),
			    data->getFeedbackLevel( ch ) ) ) );
  } else {
    sprintf( str, "Gain  Supp!" );
  }
  fFeedbackLabel->setText( str );
  
  fVoltBox->blockSignals( TRUE );
  fVoltBox->setValue( data->getVReq(ch) );
  fVoltBox->blockSignals( FALSE );

  if (data->getPower(ch) == OFF) {
    fPowerButton->setOn( FALSE );
  } else if (data->getPower(ch) == ON) {
    fPowerButton->setOn( TRUE );
  }
  
  string states[16] = { "ON", "RUp ", "RDn ", "OVC!!!",
			"OVV", "UNV!!!", "Ext Trip", "OverHVMax",
			"Ext DIS", "Int Trip", "Cal Err", "unused",
			"UNC", "OVV Prot", "PWR Fail", "Temp Fail" };
//   const int shift = 8*sizeof( unsigned short ) - 1;
//   const unsigned short mask = 1 << shift;
//   unsigned short pix1Status = data->getHWStatus( ch );
//   cout << "shift = " << shift << "  1st mask = " << mask
//        << "  pix1Status = " << pix1Status << "  ";
//   for ( unsigned i=1; i<=shift+1; ++i )
//     {
//       if ( pix1Status & mask )
// 	{
// 	  cout << "1";
// 	}
//       else
// 	{
// 	  cout << "0";
// 	}
//     }
//   cout << "  ";
//   for ( unsigned i=1; i<=shift+1; ++i )
//     {
//       if ( pix1Status & mask )
// 	{
// 	  cout << states[i];
// 	}
//     }
//   cout << endl;
  bitset<16> status( data->getHWStatus(ch) );
  ostringstream os("");
  if      ( status.test(3) ) os << "OVC!!!";
  else if ( status.test(5) ) os << "UNV!!!";
  else if ( status.test(8) ) os << "Ext DIS";
  else if ( status.test(4) ) os << "OVV";
  else if ( status.test(6) ) os << "Ext Trip";
  else if ( status.test(7) ) os << "OverHVMax";
  else if ( status.test(9) ) os << "Int Trip";
  else if ( status.test(10)) os << "Cal Err";
  else if ( status.test(12)) os << "UNC";
  else if ( status.test(13)) os << "OVV Prot";
  else if ( status.test(14)) os << "PWR Fail";
  else if ( status.test(15)) os << "Temp Fail";
  else if ( status.test(1) ) os << "RUp ";
  else if ( status.test(2) ) os << "RDn ";
  else if ( status.test(0) ) os << "OK";
  else                       os << "OFF";
  fStatusLabel->setText( os.str().c_str() );
}


/** Voltage changed by user in panel input box.
 *  Update the PMT information and redraw.
 * \arg \c  value - new high voltage value (Volts)
 */
void VChannelReport::voltageChanged(int value)
{
  VPMTIcon *p = fMainFrame->getSelected();
  if (p) {
    int ch = p->getChannel();
    p->getData()->setVReq(ch, value);
    p->draw();
  }
}


/** Power status of the currently selected PMT has changed
 *  Update the PMT information.
 */
void VChannelReport::powerChanged()
{

  VPMTIcon *p = fMainFrame->getSelected();
  if (p) {
    int ch = p->getChannel();
    int index = p->getTelescopeID();
    if ( p->getData()->getPower( ch ) ) {
      fMainFrame->getControl()->setPower( ch, OFF );
    } else {
      fMainFrame->getControl()->setPower( ch, ON );
    }
  }
}


/*****************************************************************************/

/** \class VBoardReport VHVFrame.h
    \brief A widget to display information on the HV board associated with
    the currently selected channel.
*/


/** Constructor - generates a HV board report panel
 * \arg \c mf     - Frame on which to draw the panel
 * \arg \c parent - Qt parent widget
 * \arg \c name   - Widget indentifier
 */
VBoardReport::VBoardReport(VHVFrame *mf, QWidget* parent, const char* name) : QGroupBox(parent,name)
{
  
  fMainFrame = mf;
  
  setTitle("No board selected");  
  QFont aFont( "Helvetica", 12, QFont::Normal );
  //setFont( aFont );

  //  QGridLayout* layout = new QGridLayout(this,2,3,15);
  QGridLayout* layout = new QGridLayout(this,4,6,5);
  layout->setRowSpacing( 0, 15 );
  fVoltBox = new QSpinBox( MINVOLTS, MAXVOLTS, 10,
			   this, "voltage selection" );
  fVoltBox->setValue( MINVOLTS );
  fVoltBox->setMinimumSize( fVoltBox->sizeHint() );
  fVoltBox->setMaximumHeight( fVoltBox->minimumSize().height() );  
  connect( fVoltBox, SIGNAL( valueChanged(int) ), 
	   this, SLOT( voltageChanged(int) ) );

  QLabel *label = new QLabel("VReq",this);
  layout->addWidget( label,   1, 0 );
  layout->addWidget( fVoltBox, 1, 1 );
  
  fVSetLabel = new QLabel("VSet     0 V",this);
  layout->addWidget( fVSetLabel, 1, 2 );
  
  fVTrueLabel = new QLabel("VTru     0 V",this);
  layout->addWidget( fVTrueLabel, 1, 3 );

  fTempLabel = new QLabel("Temp     0 C",this);
  layout->addWidget( fTempLabel, 1, 4 );
  

  fRampUpBox = new QSpinBox( 0, 500, 10,
			     this, "ramp up speed" );
  fRampUpBox->setValue( 0 );
  fRampUpBox->setMinimumSize( fRampUpBox->sizeHint() );
  fRampUpBox->setMaximumHeight( fRampUpBox->minimumSize().height() );  
  connect( fRampUpBox, SIGNAL( valueChanged(int) ), 
	   this, SLOT( rampUpChanged(int) ) );
  QLabel *ruplabel = new QLabel("Ramp up",this);
  fRampUpLabel = new QLabel("0 V/s",this);
  layout->addWidget( ruplabel, 2, 0 );
  layout->addWidget( fRampUpBox, 2, 1 );
  layout->addWidget( fRampUpLabel, 2, 2 );

  fRampDownBox = new QSpinBox( 0, 500, 10,
			       this, "ramp up speed" );
  fRampDownBox->setValue( 0 );
  fRampDownBox->setMinimumSize( fRampDownBox->sizeHint() );
  fRampDownBox->setMaximumHeight( fRampDownBox->minimumSize().height() );  
  connect( fRampDownBox, SIGNAL( valueChanged(int) ), 
	   this, SLOT( rampDownChanged(int) ) );
  QLabel *rdownlabel = new QLabel("Ramp down",this);
  fRampDownLabel = new QLabel("0 V/s",this);
  layout->addWidget( rdownlabel, 2, 3 );
  layout->addWidget( fRampDownBox, 2, 4 );
  layout->addWidget( fRampDownLabel, 2, 5 );

  label = new QLabel("Curr (max)",this);
  fCurrentLabel = new QLabel("0.0 uA",this);
  fCurrentBox = new QSpinBox( 0, 3000, 10, this, "current limit" );
  layout->addWidget( label, 3, 0 );
  layout->addWidget( fCurrentBox, 3, 1 );
  layout->addWidget( fCurrentLabel, 3, 2 );

  //label = new QLabel( "Limit BoardV", this );
  fLimitCheckBox = new QCheckBox( "Limit BoardV", this );
  fLimitCheckBox->setChecked( true );
  layout->addMultiCellWidget( fLimitCheckBox, 3, 3, 3, 4 );
  connect( fLimitCheckBox, SIGNAL( toggled() ), 
	   this, SLOT( checkboxChanged() ) );
  fPowerButton = new QPushButton("Power",this);
  fPowerButton->setToggleButton( TRUE );
  layout->addWidget( fPowerButton, 3, 5 );
  connect( fPowerButton, SIGNAL( clicked() ), this, SLOT( powerChanged() ) );

  update();

}

/** Destructor
 */
VBoardReport::~VBoardReport()
{
  
  delete fVoltBox;
  delete fVTrueLabel;
  delete fCurrentLabel;
  delete fPowerButton;
  delete fRampUpBox;
  delete fRampDownBox;
  delete fRampUpLabel;
  delete fRampDownLabel;
  delete fTempLabel;
  delete fVSetLabel;

}

/** Update all quantities displayed on the HV board report panel 
 */
void VBoardReport::update()
{

  VPMTIcon *p = fMainFrame->getSelected();
  int ch = p->getChannel();
  int index = p->getTelescopeID();
  int hvchan,slot,crate;
  VHVData *data = p->getData();
  data->getCrateAndSlot(ch, crate, hvchan, slot);
  
  char str[40];
  sprintf(str,"T%d Crate %2d, Board %2d", index+1, crate, slot );
  setTitle(str);
  int boardIndex = data->getBoardIndex(crate, slot);
  sprintf(str,"%7.1f uA",1000.0*data->getBoardCurrent(boardIndex));
  fCurrentLabel->setText(str);
  sprintf(str,"VTru %6d V",data->getBoardVTrue(boardIndex));
  fVTrueLabel->setText(str);
  sprintf(str,"VSet %6d V",data->getBoardVSet(boardIndex));
  fVSetLabel->setText(str);
  sprintf(str,"%4d V/s",data->getRampUp(boardIndex));
  fRampUpLabel->setText(str);
  sprintf(str,"%4d V/s",data->getBoardRampDown(boardIndex));
  fRampDownLabel->setText(str);
  
  fVoltBox->blockSignals( TRUE );
  fVoltBox->setValue( data->getBoardVReq(boardIndex) );
  fVoltBox->blockSignals( FALSE );

  fRampUpBox->blockSignals( TRUE );
  fRampUpBox->setValue( data->getBoardRampUp(boardIndex) );
  fRampUpBox->blockSignals( FALSE );

  fRampDownBox->blockSignals( TRUE );
  fRampDownBox->setValue( data->getBoardRampDown(boardIndex) );
  fRampDownBox->blockSignals( FALSE );

  if (data->getBoardPower(boardIndex) == OFF) {
    fPowerButton->setOn( FALSE );
  } else if (data->getBoardPower(boardIndex) == ON) {
    fPowerButton->setOn( TRUE );
  }
  sprintf( str, "Temp %5d C", data->getBoardTemp( boardIndex ) );
  fTempLabel->setText( str );
  checkboxChanged();

}



void VBoardReport::checkboxChanged()
{
  //cout << "checking" << endl;
  if ( fLimitCheckBox->isChecked() )
    {
      for ( int i=0; i<NUMTEL; ++i )
	{
	  if ( kActiveTelescope[i] )
	    {
	      fMainFrame->getControl(i)->getData()->
		enableLimitBoardVoltages();
	    }
	}
    }
  else
    {
      for ( int i=0; i<NUMTEL; ++i )
	{
	  if ( kActiveTelescope[i] )
	    {
	      fMainFrame->getControl(i)->getData()->
		disableLimitBoardVoltages();
	    }
	}
    }
}


/** Voltage on the currently selected HV board has changed.
 *  Update the board information.
 * \arg \c  value - new HV board supply voltage (Volts)
 */
void VBoardReport::voltageChanged(int value)
{

  VPMTIcon *p = fMainFrame->getSelected();

  if (p) {
    int ch = p->getChannel();
    int hvchan, slot, crate;
    VHVData *data = p->getData();
    data->getCrateAndSlot(ch, crate, hvchan, slot);
    int boardIndex = data->getBoardIndex(crate, slot);
    data->setBoardVReq(boardIndex, value);
  }

}

/** Current limit on the currently selected HV board has changed.
 *  Update the board information.
 * \arg \c  value - new current limit value (uA)
 */
void VBoardReport::currentChanged(int value)
{

  VPMTIcon *p = fMainFrame->getSelected();

  if (p) {
    int ch = p->getChannel();
    int hvchan, slot, crate;
    VHVData *data = p->getData();
    data->getCrateAndSlot(ch, crate, hvchan, slot);
    int boardIndex = data->getBoardIndex(crate, slot);
    data->setBoardIMax(boardIndex, value);
  }

}

/** Ramp up speed of the currently selected HV board has changed.
 *  Update the board information.
 * \arg \c  value - new ramp up speed (Volts/second)
 */
void VBoardReport::rampUpChanged(int value)
{

  VPMTIcon *p = fMainFrame->getSelected();

  if (p) {
    int ch = p->getChannel();
    int hvchan, slot, crate;
    VHVData *data = p->getData();
    data->getCrateAndSlot(ch, crate, hvchan, slot);
    int boardIndex = data->getBoardIndex(crate, slot);
    data->setBoardRampUp(boardIndex, value);
  }

}

/** Ramp down speed of the currently selected HV board has changed.
 *  Update the board information.
 * \arg \c  value - new ramp down speed (Volts/second)
 */
void VBoardReport::rampDownChanged(int value)
{

  VPMTIcon *p = fMainFrame->getSelected();

  if (p) {
    int ch = p->getChannel();
    int hvchan, slot, crate;
    VHVData *data = p->getData();
    data->getCrateAndSlot(ch, crate, hvchan, slot);
    int boardIndex = data->getBoardIndex(crate, slot);
    data->setBoardRampDown(boardIndex, value);
  }

}

/** Power status of the currently selected HV board changed.
 *  Update the board information.
 */
void VBoardReport::powerChanged()
{
  //   cout << "in powerChanged" << endl;
  VPMTIcon *p = fMainFrame->getSelected();
  if (p) {
    VHVData *data = p->getData();
    int ch = p->getChannel();
    //    int index = p->getTelescopeID();
    int hvchan, slot, crate;
    int cr, chdummy, sl;
    VHVControl* control = fMainFrame->getControl();
    data->getCrateAndSlot(ch, crate, hvchan, slot);
    int boardIndex = data->getBoardIndex( crate, slot );
    cout << "VBoardReport::powerChanged(): changing power for boardIndex="
	 << boardIndex << endl;
    if ( data->getBoardPower( boardIndex ) ) {
      control->setBoardPower( boardIndex, OFF );
      // If turning board off, also turn off all channels on that board.
      //       cout << "turned board off, so let's turn off channels " << endl;
      for ( int i=0; i<data->getNumberOfPMTs(); i++ ) {
	data->getCrateAndSlot( i, cr, chdummy, sl );
	if ( ( cr == crate ) && ( sl == slot ) ) {
	  // This channel is on same board.
	  // 	  cout << "turning off pixel " << i+1 << endl;
	  control->setPower( i, OFF );
	}
      }
    } else {
      control->setBoardPower( boardIndex, ON );
    }
    //     cout << "toggling power" << endl;
  }
}


/*****************************************************************************/

/** \class VTelStatus VHVFrame.h
    \brief A widget to display status LEDs for each telescope
*/

/** Constructor - generates a star report/control panel
 * \arg \c mf      - Frame on which to draw the panel
 * \arg \c control - Associated VHVControl object
 * \arg \c parent  - Qt parent widget
 */
VTelStatus::VTelStatus( VHVFrame *mf, QWidget* parent ) 
  : QGroupBox( parent, "telStatus" ) 
{
    
  fMainFrame = mf;
  //   fControl = control;

  setTitle( "");
  QFont aFont( "Helvetica", 12, QFont::Normal );
  //setFont( aFont );

  QGridLayout* box = new QGridLayout(this, 5, 5, 5, 0 );
  //   box->setRowSpacing( 0, 0 );

  box->addWidget( fHVCr0Label = new QLabel( "HV Cr 0", this ), 1, 0 ); 
  box->addWidget( fHVCr1Label = new QLabel( "HV Cr 1", this ), 2, 0 ); 
  box->addWidget( fCMLabel =    new QLabel( "Curr Mon", this ), 3, 0 ); 
  box->addWidget( fDBLabel =    new QLabel( "DB Log", this ), 4, 0 ); 
  for ( int i=0; i<NUMTEL; ++i ) {
    char tmp[10];
    sprintf( tmp, "Tel %d", i+1 );
    box->addWidget( fTLabel[i] = new QLabel( tmp, this ), 0, i+1 ); 
    box->addWidget( fTCr0LED[i] = new VLED( this ,"" ), 1, i+1 ); 
    box->addWidget( fTCr1LED[i] = new VLED( this ,"" ), 2, i+1 ); 
    box->addWidget( fTCMLED[i] = new VLED( this ,"" ), 3, i+1 ); 
    box->addWidget( fTDBLED[i] = new VLED( this ,"" ), 4, i+1 ); 
  }  
}

/** Destructor
 */
VTelStatus::~VTelStatus()
{
  delete fHVCr0Label;
  delete fHVCr1Label;
  delete fCMLabel;
  delete fDBLabel;
  for ( int i=0; i<NUMTEL; ++i ) {
    delete fTLabel[i];
    delete fTCr0LED[i];
    delete fTCr1LED[i];
    delete fTCMLED[i]; 
    delete fTDBLED[i]; 
  }
}


void VTelStatus::update()
{
  for ( int i=0; i<NUMTEL; ++i ) {
    if ( kActiveTelescope[i] ) {
      fTCr0LED[i]->setState( fMainFrame->getControl( i )
			     ->getData()->getCrateStatus( 0 ) );
      fTCr1LED[i]->setState( fMainFrame->getControl( i )
			     ->getData()->getCrateStatus( 1 ) );
      fTDBLED[i]->setState( kDBLogging[i] );
      fTCMLED[i]->setState( fMainFrame->getControl( i )
			    ->getData()->getCurrentsStatus() );
    }
  }
}

/*****************************************************************************/

/** \class VStarPanel VHVFrame.h
    \brief A widget to display information on the star display
*/

/** Constructor - generates a star report/control panel
 * \arg \c mf      - Frame on which to draw the panel
 * \arg \c control - Associated VHVControl object
 * \arg \c parent  - Qt parent widget
 */
VStarPanel::VStarPanel(VHVFrame *mf, VHVControl *control[NUMTEL], 
		       QWidget* parent) 
  : QGroupBox(parent,"starPanel") 
{
    
  fMainFrame = mf;
  for ( int i=0; i<NUMTEL; ++i ) {
    fControl[i] = control[i];
  }

  setTitle( "Star Options");

  QGridLayout* box = new QGridLayout(this, 5, 4, 5 );
  box->setRowSpacing( 0, 15 );
  //   cout << "fontMetrics = " << fontMetrics().height() << endl;
  fStarsButton = new QPushButton( "Show Stars", this );
  fStarsButton->setToggleButton( TRUE );
  fStarsButton->setOn( true );
  //  fStarsButton->setMaximumSize( 60, 50 ); 
  box->addMultiCellWidget( fStarsButton, 1, 1, 0, 1 );
  connect( fStarsButton, SIGNAL( clicked() ), this, SLOT( showStars() ) );

  fArcsButton = new QPushButton( "Show Labels", this );
  fArcsButton->setToggleButton( TRUE );
  fArcsButton->setOn( true );
  //fArcsButton->setEnabled( false );
  box->addMultiCellWidget( fArcsButton, 1, 1, 2, 3 );
  connect( fArcsButton, SIGNAL( clicked() ), this, SLOT( showArcs() ) );

  QLabel* label = new QLabel("RA", this);
  box->addWidget( label, 2, 0 );  
  QFont aFont( "Helvetica", 12, QFont::Normal );
  //setFont( aFont );
  fRAInput = new QLineEdit( this );
  connect( fRAInput, SIGNAL( returnPressed() ), this, SLOT( changedRA() ));
  fRAInput->setMaximumSize( 50, 50);
  fRAInput->setValidator(new VFloatValidator(this, "", 0.0, 360.0));
  //fRAInput->setFont( aFont );
  char tstring[20];
  sprintf( tstring, "%6.3f", 
	   fControl[fMainFrame->getSelected()->getTelescopeID()]
	   ->getStarField()->getRA() );
  fRAInput->setText( tstring );
  box->addWidget( fRAInput, 2, 1 );

  QLabel* label2 = new QLabel("Dec.", this);
  box->addWidget( label2, 2, 2 );
  fDecInput = new QLineEdit( this );
  connect( fDecInput, SIGNAL( returnPressed() ), this, SLOT( changedDec() ));
  fDecInput->setMaximumSize( 50, 50);
  fDecInput->setValidator(new VFloatValidator(this, "", -90.0, 90.0));
  //fDecInput->setFont( aFont );
  sprintf( tstring, "%6.2f", 
	   fControl[fMainFrame->getSelected()->getTelescopeID()]
	   ->getStarField()->getDec() );
  fDecInput->setText( tstring );
  box->addWidget( fDecInput, 2, 3 );
  
  QLabel* label3 = new QLabel("Target", this );
  box->addWidget( label3, 3, 0 );
  fListBox = new QListBox( this );
  fListBox->setSelectionMode( QListBox::Single );
  ifstream sources( "/usr/local/veritas/SP05/hvac/DataFiles/sources_and_stars.trk", 
		    ios::in );  
  string shortname, fullname;
  string ra_now, dec_now;
  string ra2000, dec2000;
  float epoch2000;
  float epochnotsure;
  vector<float> rahr, ramin, rasec;
  vector<float> decdeg, decmin, decsec;
  vector<float> ve2000;
  vector<float> venotsure;
  fRAHR.push_back( 0 );
  fRAMIN.push_back( 0 );
  fRASEC.push_back( 0 );
  fDecDEG.push_back( 0 );
  fDecMIN.push_back( 0 );
  fDecSEC.push_back( 0. );  
  fListBox->insertItem( "Database" );
  fRAHR.push_back( 0 );
  fRAMIN.push_back( 0 );
  fRASEC.push_back( 0 );
  fDecDEG.push_back( 0 );
  fDecMIN.push_back( 0 );
  fDecSEC.push_back( 0. );  
  fListBox->insertItem( "Manual" );
  while ( sources >> shortname >> fullname 
	  >> ra_now >> dec_now 
	  >> epoch2000 
	  >> ra2000 >> dec2000 
	  >> epochnotsure ) {
    //     cout << shortname << "  " << fullname  << "  " << ra_now  << "  " 
    // 	 << dec_now  << "  " << epoch2000 << endl;
    //     if ( shortname == "CR" ) cout << "ra_sec:  " << ra_now.substr(6,4).c_str()
    // 				  << endl;
    int length = dec_now.size();
    // The "_now" variables are really J2000 - the "2000" variables are for
    // some other epoch (varies for each source).
    fRAHR.push_back( (float)atof( ra_now.substr(0,2).c_str() ) );
    fRAMIN.push_back( (float)atof( ra_now.substr(3,2).c_str() ) );
    fRASEC.push_back( (float)atof( ra_now.substr(6,4).c_str() ) );
    if ( length == 8 ) {
      fDecDEG.push_back( (float)atof( dec_now.substr(0,2).c_str() ) );
      fDecMIN.push_back( (float)atof( dec_now.substr(3,2).c_str() ) );
      fDecSEC.push_back( (float)atof( dec_now.substr(6,2).c_str() ) );
    } else if ( length == 9 ) {
      fDecDEG.push_back( (float)atof( dec_now.substr(0,3).c_str() ) );
      fDecMIN.push_back( (float)atof( dec_now.substr(4,2).c_str() ) );
      fDecSEC.push_back( (float)atof( dec_now.substr(7,2).c_str() ) );
      //       cout << shortname << " " << (float)atof( dec_now.substr(0,3).c_str() )
      // 	   << endl;
    }
    ve2000.push_back( epoch2000 );
    venotsure.push_back( epochnotsure );
    fListBox->insertItem( fullname.c_str() );
  }
  sources.close();
  //   fListBox->setRowMode( QListBox::FixedNumber );
  fListBox->setMaximumHeight( 50 );
  fListBox->setCurrentItem( 0 ); // default to manual, initially.
  box->addMultiCellWidget( fListBox, 3, 3, 1, 3 );

  QButtonGroup* bg = new QButtonGroup( 1, Qt::Horizontal, 
				       this, "starRadioGroup" );
  bg->hide();
  //   QHBox* innerbox = new QHBox( this, "starRadioBox" );

  QRadioButton* rb1 = new QRadioButton( this, "offbefore" );
  rb1->setText( "OFF (RA-30m)" );
  bg->insert( rb1, OFFBEFORE );
  box->addWidget( rb1, 4, 1 );

  QRadioButton *rb2 = new QRadioButton( this, "on" );
  rb2->setText( "ON src" );
  rb2->setChecked( TRUE );
  fStarOffset = 0.; // corresponds to on-source display.
  bg->insert( rb2, ONSOURCE );
  box->addWidget( rb2, 4, 0 );
  
  QRadioButton* rb3 = new QRadioButton( this, "offafter" );
  rb3->setText( "OFF (RA+30m)" );
  bg->insert( rb3, OFFAFTER );
  box->addWidget( rb3, 4, 2 );

  //   box->addMultiCellWidget( innerbox, 4, 4, 0, 3 );
  //   box->addMultiCellWidget( bg, 4, 4, 0, 3 );
  connect( bg, SIGNAL(clicked(int)), SLOT(optionButtonClicked(int)) );


  // Commenting out the star-magnitude limit slider for now, to save space.
  // No reason not to restore the following lines if we decide we need it.
  // Also need to increase number of rows in "box" from 5 to 6, though.

  //   QLabel* label4 = new QLabel("Min. Magnitude",this);
  //   box->addMultiCellWidget( label4, 5, 5, 0, 1 );
  
  //   fSlider = new QSlider( MINMAG, MAXMAG, 1, MAXMAG, 
  //   			 QSlider::Horizontal, this, "magnitude select" );
  //   connect( fSlider, SIGNAL( valueChanged(int) ), this, SLOT( changedMaxMag(int) )); 
  //   fSlider->setTickmarks( QSlider::Below );
  //   box->addMultiCellWidget( fSlider, 5, 5, 2, 3 );
  
}

/** Destructor
 */
VStarPanel::~VStarPanel()
{

  delete fSlider;
  delete fRAInput;
  delete fDecInput;
  delete fStarsButton;
  delete fArcsButton;

}

/** Update all quantities displayed on the star report/control panel 
 */
void VStarPanel::update() 
{

  changedCoords();

}

/** Stellar magnitude limit has changed - update display.
 * \arg \c value - new maximum magnitude
 */
void VStarPanel::changedMaxMag(int value) 
{

  fMainFrame->sendMessage("  Changing magnitude display limit <br>");
  for ( int i=0; i<NUMTEL; ++i ) {
    if ( kActiveTelescope[i] ) {
      fControl[i]->getStarField()->changeMagnitude((double)value);
    }
  }
}

/** Target Right Ascension has changed.
 */
void VStarPanel::changedRA()
{
  fListBox->clearSelection();
  fListBox->setCurrentItem( 1 );
  //   cout << "VStarPanel::changedRA()" << endl;
  changedCoords();
}

/** Target declination has changed.
 */
void VStarPanel::changedDec()
{
  fListBox->clearSelection();
  fListBox->setCurrentItem( 1 );
  //   cout << "VStarPanel::changedDec()" << endl;
  changedCoords(); 
}

/** Target coordinates have changed.
 */
void VStarPanel::changedCoords()
{
  char tmp[200];
  double ra, dec, az, el;
  VDBPOS::StatusInfo status;
  int srcInd = fListBox->currentItem();
  if ( srcInd == 0 ) { // read position from database
    try {
      int index = fMainFrame->getSelected()->getTelescopeID();
      VDBPOS::TargetInfo target = VDBPOS::getTarget( index );
//       if ( target.mode == "tracking" ) {
// 	ra = target.angle1;
// 	dec = target.angle2;
//       } else {
// 	double el = target.angle1;
// 	double az = target.angle2;
// 	double gstrad = MJD2GMST( getCurrentMJD() )/1.0027379;
// 	double lstdeg = gstrad*RAD2DEG + kLongitude;
// 	if ( lstdeg < 0 ) lstdeg += 360.;
// 	if ( lstdeg > 360 ) lstdeg -= 360.;
// 	dec = RAD2DEG*asin( sin(el)*sin(DEG2RAD*kLatitude) + 
// 			    cos(el)*cos(DEG2RAD*kLatitude)*cos(az) );
// 	ra = lstdeg - RAD2DEG*asin( -cos(el)*sin(az)/cos(DEG2RAD*dec) );
	// 	QString mess1;
	// 	mess1 = (string)" VStarPanel::changedCoords(): Mode is fixed "
	// 	  + (string)"(not tracking) so star positions may be wrong " 
	// 	  + (string)"by half a pixel.<br>";
	// 	fMainFrame->sendMessage( mess1 );
	//       cout << "T" << index+1 << ":  el=" << el*RAD2DEG 
	//            << "deg   az=" << az*RAD2DEG 
	// 	      << "deg" << "  ra=" << ra << "deg  dec=" << dec 
	//            << "deg  gstrad=" << RAD2DEG*gstrad<< "deg  lstdeg=" 
	//             << lstdeg << endl;
	status = VDBPOS::getStatus( index );
	el = status.elevation_meas;
	az = status.azimuth_meas;
	//	VAAzElRADecXY *coordtrans = new VAAzElRADecXY( kLongitude*DEG2RAD,
	//						       kLatitude*DEG2RAD );
	//	VATime now;
	//	now.setFromMSTimeStamp( status.timestamp ); // use tracking timestamp
	//	coordtrans->AzEl2RADec2000( az, el, now, ra, dec );
	double now = convertDBTimeToMJDdouble( status.timestamp );
	AzEl2RADec2000( az, el, now, ra, dec );
//       }
      ra *= RAD2DEG;
      dec *= RAD2DEG;
// 	      	cout << "Long=" << kLongitude 
// 	      	     << "  Lat=" << kLatitude
// 	      	     << "  Az=" << az*RAD2DEG
// 	      	     << "  El=" << el*RAD2DEG
// 	      	     << "  ra=" << ra*DEG2RAD
// 	      	     << "  dec=" << dec*DEG2RAD
// 	      	     << "  RA=" << ra
// 	      	     << "  Dec=" << dec
// 		     << "  mjddbl=" << now
// 	      	     << "  " << status.timestamp
// 	      	     << endl;
      sprintf( tmp, "%.2f", ra );
      fRAInput->setText( QString( tmp ) );
      sprintf( tmp, "%.2f", dec );
      fDecInput->setText( QString( tmp ) );
    }
    catch(VException & e) {
      cerr << e << endl;
      QString message;
      message = (string)" VStarPanel::changedCoords(): Reading pointing "
	+ (string)"direction from DB failed. <br>";
      fMainFrame->sendMessage( message );
    }
  } else if ( srcInd == 1 ) { // read from text entry boxes
    QString RAString = fRAInput->text();
    QString DecString = fDecInput->text();
    int pos;
    
      if ( (fRAInput->validator()->
	    validate(RAString,pos) != QValidator::Acceptable) ||
	   (fDecInput->validator()->
	    validate(DecString,pos) != QValidator::Acceptable) ) return;
    
    bool ok;
    ra = RAString.toDouble(&ok);
    dec = DecString.toDouble(&ok);
  } else { // read from source list
    ra  = 15*( fRAHR[srcInd] +   fRAMIN[srcInd]/60. +  fRASEC[srcInd]/3600. );
    dec = fDecDEG[srcInd] + fDecMIN[srcInd]/60. + fDecSEC[srcInd]/3600.;
    sprintf( tmp, "%.2f", ra );
    fRAInput->setText( QString( tmp ) );
    sprintf( tmp, "%.2f", dec );
    fDecInput->setText( QString( tmp ) );
  }
  //   fMainFrame->sendMessage("  Changing target RA, Dec <br>");
  //   cout << "srcIndex: " << srcInd << "  ra:  " << ra << "  dec:  " << dec 
  //        << "  " << fRAHR[srcInd] << "  " << fRAMIN[srcInd] << "  " 
  //        << fRASEC[srcInd] << endl;

  // If we're not to display stars that correspond to the selecting pointing,
  // then offset the RA.
  if ( fStarOffset == OFFBEFORE ) {
    ra = ra - 7.5;
  } else if ( fStarOffset == OFFAFTER ) {
    ra = ra + 7.5;
  }
  for ( int i=0; i<NUMTEL; ++i ) {
    if ( kActiveTelescope[i] ) {
      fControl[i]->getStarField()->setPointing( ra, dec );
      fControl[i]->getData()->setRA( ra );
      fControl[i]->getData()->setDec( dec );
    }
  }
}

/** Toggle the display of stars (VStar) on the camera display
 */
void VStarPanel::showStars() 
{

  if (starsOn()) {
    fMainFrame->starMenu(MENU_SHOW_STARS);
  } else {
    fMainFrame->starMenu(MENU_HIDE_STARS);
  }

}

/** Toggle the display of star arcs (VStarArc) on the camera display
 */
void VStarPanel::showArcs() 
{

  if (arcsOn()) {
    fMainFrame->starMenu(MENU_SHOW_ARCS);
  } else {
    fMainFrame->starMenu(MENU_HIDE_ARCS);
  }

}

/** Return status of the display of stars (VStar) on the camera display
 */
bool VStarPanel::starsOn() 
{
  return(fStarsButton->isOn());
}

/** Return status of the display of star arcs (VStarArc) on the camera display
 */
bool VStarPanel::arcsOn() 
{
  return(fArcsButton->isOn());
}

/** Turn on the display of stars (VStar) on the camera display
 */
void VStarPanel::setStarsOn( bool on )
{
 
  fStarsButton->setOn( on );
 
}

/** Turn on the display of star arcs (VStarArc) on the camera display
 */
void VStarPanel::setArcsOn( bool on )
{

  fArcsButton->setOn( on );

}

/** Called when a star display option button is clicked
 * \arg \c id - Display option identifer (::EnumStarOffset)
 */
void VStarPanel::optionButtonClicked(int id)
{
  
  fStarOffset = id;

}


/*****************************************************************************/
/**Misc. functions ***/

/** Generate (and return a pointer to) a set of display option buttons. 
 * \arg \c p - Qt widget on which the buttons should appear
 */

QButtonGroup* OptionButtons(QWidget *p)
{
    
  QButtonGroup* bg = new QButtonGroup( p, "radioGroup" );
  bg->setTitle( "Display Options" );

  // Create a layout for the radio buttons
  //  QGridLayout* box = new QGridLayout(bg,2,2,bg->fontMetrics().height()+5);
  QGridLayout* box = new QGridLayout(bg,2,4,5);
  box->setRowSpacing( 0, 15 );
//   cout << "fontMetrics = " << bg->fontMetrics().height() << endl;

  QRadioButton *rb = new QRadioButton( bg );
  rb->setText( "i(PMT)" );
  box->addWidget( rb, 1, 0 );
  
  rb = new QRadioButton( bg );
  rb->setText( "V(Req)" );
  box->addWidget( rb, 2, 0 );
  rb->setChecked( TRUE );
  
  rb = new QRadioButton( bg );
  rb->setText( "V(Set)" );
  box->addWidget( rb, 1, 1 );

  rb = new QRadioButton( bg );
  rb->setText( "V(True)" );
  box->addWidget( rb, 2, 1 );

  rb = new QRadioButton( bg );
  rb->setText( "DV(True-Req)" );
  box->addWidget( rb, 1, 2 );

  rb = new QRadioButton( bg );
  rb->setText( "DV(Set-Req)" );
  box->addWidget( rb, 2, 2 );

  rb = new QRadioButton( bg );
  rb->setText( "Power" );
  box->addWidget( rb, 1, 3 );

  rb = new QRadioButton( bg );
  rb->setText( "i(Board)" );
  box->addWidget( rb, 2, 3 );

  return(bg);

}

/*****************************************************************************/

/** \class VLED VHVFrame.h
 *  \brief Red, green or yellow LED implemented using QPixmap.
 */

/** Constructor - load LED pixmaps from .png files
 * \arg \c parent - Qt parent widget
 * \arg \c name   - Widget indentifier
 */
VLED::VLED(QWidget *parent, const char *name) : QLabel("", parent, name)
{
  
  fGreen = new QPixmap
    ("/usr/local/veritas/SP05/hvac/DataFiles/pixmaps/LEDGreen_small.png");
  fRed = new QPixmap
    ("/usr/local/veritas/SP05/hvac/DataFiles/pixmaps/LEDRed_small.png");
  fYellow = new QPixmap
    ("/usr/local/veritas/SP05/hvac/DataFiles/pixmaps/LEDYellow_small.png");
  fState = VERROR;
  setState( VERROR );

}

/** Destructor */
VLED::~VLED()
{

  delete fGreen;
  delete fRed;
  delete fYellow;

}

/** Set the state (colour) of the LED
 * \arg \c state - LED state
 */
void VLED::setState(int state) 
{
  
  fState = state;
  switch (state) {
  case VOK:
    setPixmap(*fGreen);
    break;
  case VERROR:
    setPixmap(*fRed);
    break;
  default:
    setPixmap(*fRed);
    break;
  }

  show();
  
}

/*****************************************************************************/

/** \class VTimeWidget VHVFrame.h
 *  \brief Display the time and a label (in LCD style numbers)
 */

/** Constructor 
 * \arg \c label  - Label to appear next to time
 * \arg \c parent - Qt parent widget
 */
VTimeWidget::VTimeWidget(QString label, QWidget *parent) : QWidget(parent)
{

  QHBoxLayout *layout = new QHBoxLayout( this, 0 );
  fLabel = new QLabel(label,this);
  layout->addWidget(fLabel);
  
  fTime = fTime.currentTime();
  fText = new QLCDNumber(8,this,"LCDNumber");
  fText->display(fTime.toString());

  fText->setSegmentStyle(QLCDNumber::Flat);
  fText->setFrameStyle(QFrame::NoFrame);
  fText->setMaximumSize(70,200);

  layout->addWidget(fText);  
  
  fLabel->show();
  fText->show();
  show();
}

/** Destructor */
VTimeWidget::~VTimeWidget()
{

  delete fText;
  delete fLabel;

}

/** Update the time shown in the display */
void VTimeWidget::update()
{
  
  fTime = fTime.currentTime();
  fText->display(fTime.toString());
  fText->show();
  show();
  
}

/*****************************************************************************/

/** \class VAutoResponseDialog VHVFrame.h
 *  \brief Set parameters for autoresponse to PMT currents.
 */

/** Constructor 
 * \arg \c label  - Label to appear next to time
 * \arg \c parent - Qt parent widget
 */
VAutoResponseDialog::VAutoResponseDialog(VHVFrame *parent, 
					 VHVControl* control, 
					 const char * name ) 
  : QDialog(parent )
{
  fControl = control;
  setCaption( name );

  fAutoResponseCheckBox = new QCheckBox( "Turn AutoResponse ON", this );
  fAutoResponseCheckBox->setGeometry( 10, 10, 200, 30 );
  fAutoResponseCheckBox->setChecked( kAutoResponse[fControl->getTelescopeID()] );

  fRestoreCheckBox = new QCheckBox( "Restore when Stars Clear", this );
  fRestoreCheckBox->setGeometry( 10, 40, 200, 30 );
  fRestoreCheckBox->setChecked( kStarRestore[fControl->getTelescopeID()] );

  QString timeText( "Transient Check Time (ms)..." );
  fTimeLabel = new QLabel( timeText, this );
  fTimeLabel->setGeometry( 10, 70, 200, 30 );
  fTimeSpinBox = new QSpinBox( 0, 20000, 100, this, "Transient Check Time" );
  fTimeSpinBox->setGeometry( 250, 75, 100, 20 );
  fTimeSpinBox->setValue( fControl->getData()->getCheckTime() );

  QString thresholdText( "PMT Current Standard Threshold (uA)" );
  fThresholdLabel = new QLabel( thresholdText, this );
  fThresholdLabel->setGeometry( 10, 100, 200, 30 );
  fThresholdSpinBox = new QSpinBox( 0, 125, 1, this, "Current Threshold" );
  fThresholdSpinBox->setGeometry( 250, 105, 100, 20 );
  fThresholdSpinBox->setValue( fControl->getData()->
			       getThreshold() );

  QString hiThresholdText( "PMT Current Immediate Kill Threshold (uA)" );
  fHiThresholdLabel = new QLabel( hiThresholdText, this );
  fHiThresholdLabel->setGeometry( 10, 130, 200, 30 );
  fHiThresholdSpinBox = new QSpinBox( 0, 125, 1, this, "Hi Threshold" );
  fHiThresholdSpinBox->setGeometry( 250, 135, 100, 20 );
  fHiThresholdSpinBox->setValue( fControl->getData()->
				 getHiThreshold() );

  QString patternThresholdText( "PMT Current Pattern Threshold (uA)" );
  fPatternThresholdLabel = new QLabel( patternThresholdText, this );
  fPatternThresholdLabel->setGeometry( 10, 160, 200, 30 );
  fPatternThresholdSpinBox = new QSpinBox( 0, 125, 1, this, 
					   "Pattern Threshold" );
  fPatternThresholdSpinBox->setGeometry( 250, 165, 100, 20 );
  fPatternThresholdSpinBox->setValue( fControl->getData()->
				      getPatternThreshold() );

  QString patternMultiplicityText( "PMT Pattern Multiplicity..." );
  fPatternMultiplicityLabel = new QLabel( patternMultiplicityText, this );
  fPatternMultiplicityLabel->setGeometry( 10, 190, 200, 30 );
  fPatternMultiplicitySpinBox = new QSpinBox( 2, 4, 1, this, 
					      "Pattern Multiplicity" );
  fPatternMultiplicitySpinBox->setGeometry( 250, 195, 100, 20 );
  fPatternMultiplicitySpinBox->setValue( fControl->getData()->
					 getPatternMultiplicity() );

  QString recentTimeText( "Recent Time Limit (s)........" );
  fRecentTimeLabel = new QLabel( recentTimeText, this );
  fRecentTimeLabel->setGeometry( 10, 220, 200, 30 );
  fRecentTimeSpinBox = new QSpinBox( 0, 200, 1, this, "Recent Time Limit" );
  fRecentTimeSpinBox->setGeometry( 250, 225, 100, 20 );
  fRecentTimeSpinBox->setValue( fControl->getData()->getRecentTime() );

  QString restoreText( "Restore Margin (0.01 degrees)" );
  fRestoreMarginLabel = new QLabel( restoreText, this );
  fRestoreMarginLabel->setGeometry( 10, 250, 200, 30 );
  fRestoreMarginSpinBox = new QSpinBox( 0, 50, 1, this, "Restore Margin" );
  fRestoreMarginSpinBox->setGeometry( 250, 255, 100, 20 );
  fRestoreMarginSpinBox
    ->setValue( (int)(100*fControl->getData()->
		      getRestoreMargin() + 0.5) );

  QString patternRestoreText( "Pattern Restore Margin (0.01 degrees)" );
  fPatternRestoreMarginLabel = new QLabel( patternRestoreText, this );
  fPatternRestoreMarginLabel->setGeometry( 10, 280, 200, 30 );
  fPatternRestoreMarginSpinBox = new QSpinBox( 0, 50, 1, this,
					       "Pattern Restore Margin" );
  fPatternRestoreMarginSpinBox->setGeometry( 250, 285, 100, 20 );
  fPatternRestoreMarginSpinBox
    ->setValue( (int)(100*fControl->
		      getData()->
		      getPatternRestoreMargin() + 0.5) );

  QPushButton* okbutton = new QPushButton( "OK", this );
  okbutton->setGeometry( 10, 320, 100, 30 );
  connect( okbutton, SIGNAL( clicked() ), this, SLOT( accept() ) );
  QPushButton* cancelbutton = new QPushButton( "CANCEL", this );
  cancelbutton->setGeometry( 120, 320, 100, 30 );
  connect( cancelbutton, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

VAutoResponseDialog::~VAutoResponseDialog() {}

bool VAutoResponseDialog::isAutoCheckBoxChecked() const 
{ 
  return fAutoResponseCheckBox->isChecked(); 
}

bool VAutoResponseDialog::isRestoreCheckBoxChecked() const 
{ 
  return fRestoreCheckBox->isChecked(); 
}

int VAutoResponseDialog::getTimeValue() const
{
  return fTimeSpinBox->value();
}

int VAutoResponseDialog::getThresholdValue() const
{
  return fThresholdSpinBox->value();
}

int VAutoResponseDialog::getHiThresholdValue() const
{
  return fHiThresholdSpinBox->value();
}

int VAutoResponseDialog::getPatternThresholdValue() const
{
  return fPatternThresholdSpinBox->value();
}

int VAutoResponseDialog::getPatternMultiplicityValue() const
{
  return fPatternMultiplicitySpinBox->value();
}

int VAutoResponseDialog::getRecentTimeValue() const
{
  return fRecentTimeSpinBox->value();
}

float VAutoResponseDialog::getRestoreMarginValue() const
{
  return fRestoreMarginSpinBox->value()/100.;
}

float VAutoResponseDialog::getPatternRestoreMarginValue() const
{
  return fPatternRestoreMarginSpinBox->value()/100.;
}


/*****************************************************************************/

/** \class VFlatFieldDialog VHVFrame.h
 *  \brief Display the time and a label (in LCD style numbers)
 */

/** Constructor 
 * \arg \c label  - Label to appear next to time
 * \arg \c parent - Qt parent widget
 */
VFlatFieldDialog::VFlatFieldDialog(VHVFrame *parent) 
  : QDialog(parent)
{
  fParent = parent;

  QString feedbackGainText( "Feedback Loop Gain" );
  fFeedbackGainLabel = new QLabel( feedbackGainText, this );
  fFeedbackGainLabel->setGeometry( 10, 10, 150, 30 );
  fFeedbackGainSpinBox = new QSpinBox( 0, 200, 1, this, 
				       "Feedback Loop Gain" );
  fFeedbackGainSpinBox->setGeometry( 170, 15, 100, 20 );
  fFeedbackGainSpinBox->
    setValue( (int)100*parent->getControl()->getData()->getLoopGain() );
  //   QDoubleValidator gainValidator( 0., 2., 2, this, "harry" );
  //   fFeedbackGainSpinBox->setValidator( &gainValidator );

  QString targetText( "Target Current (uA)" );
  fTargetLabel = new QLabel( targetText, this );
  fTargetLabel->setGeometry( 10, 40, 150, 30 );
  fTargetSpinBox = new QSpinBox( 0, 2000, 1, this, "Target Current" );
  fTargetSpinBox->setGeometry( 170, 45, 100, 20 );
  fTargetSpinBox->
    setValue( (int)100*parent->getControl()->getData()->getTargetCurrent() );
  //   QDoubleValidator targetValidator( 0., 20., 2, this, "tom" );
  //   fTargetSpinBox->setValidator( &targetValidator );

  QString PMTIndexText( "PMT Index (10^x)" );
  fPMTIndexLabel = new QLabel( PMTIndexText, this );
  fPMTIndexLabel->setGeometry( 10, 70, 150, 30 );
  fPMTIndexSpinBox = new QSpinBox( 0, 1000, 1, this, "PMT Index (10^x)" );
  fPMTIndexSpinBox->setGeometry( 170, 75, 100, 20 );
  fPMTIndexSpinBox->
    setValue( (int)100*parent->getControl()->getData()->getPMTIndex() );
  //   QDoubleValidator PMTIndexValidator( 0., 10., 2, this, "bob" );
  //   fPMTIndexSpinBox->setValidator( &PMTIndexValidator );

  QString MaxVoltageText( "Max Allowed Voltage (V)" );
  fMaxVoltageLabel = new QLabel( MaxVoltageText, this );
  fMaxVoltageLabel->setGeometry( 10, 100, 150, 30 );
  fMaxVoltageSpinBox = new QSpinBox( 0, 1400, 1, this, 
				     "Max Allowed Voltage (V)" );
  fMaxVoltageSpinBox->setGeometry( 170, 105, 100, 20 );
  fMaxVoltageSpinBox->
    setValue( parent->getControl()->getData()->getMaxAllowedVoltage() );

  fZeroAmpsCheckBox = new QCheckBox( "Include Pixels with 0uA", this );
  fZeroAmpsCheckBox->setGeometry( 10, 130, 200, 30 );
  fZeroAmpsCheckBox->
    setChecked( parent->getControl()->getData()->getZeroAmpsFlag() );

  QPushButton* setbutton = new QPushButton( "SET", this );
  setbutton->setGeometry( 10, 160, 60, 30 );
  connect( setbutton, SIGNAL( clicked() ), this, SLOT( setValues() ) );

  QPushButton* ffbutton = new QPushButton( "FlatField", this );
  ffbutton->setGeometry( 75, 160, 60, 30 );
  connect( ffbutton, SIGNAL( clicked() ), this, SLOT( flatField() ) );

  QPushButton* okbutton = new QPushButton( "Done", this );
  okbutton->setGeometry( 140, 160, 60, 30 );
  connect( okbutton, SIGNAL( clicked() ), this, SLOT( accept() ) );
}

VFlatFieldDialog::~VFlatFieldDialog() {}

bool VFlatFieldDialog::isZeroAmpsBoxChecked() const 
{ 
  return fZeroAmpsCheckBox->isChecked(); 
}


int VFlatFieldDialog::getFeedbackGainValue() const
{
  return fFeedbackGainSpinBox->value()/100;
}

int VFlatFieldDialog::getTargetCurrentValue() const
{
  return fTargetSpinBox->value()/100;
}

int VFlatFieldDialog::getPMTIndexValue() const
{
  return fPMTIndexSpinBox->value()/100;
}

int VFlatFieldDialog::getMaxVoltageValue() const
{
  return fMaxVoltageSpinBox->value();
}

void VFlatFieldDialog::flatField()
{
  kFlatField = true;
}

void VFlatFieldDialog::setValues()
{
  fParent->getControl()->getData()->setLoopGain( getFeedbackGainValue() );
  fParent->getControl()->getData()->setPMTIndex( getPMTIndexValue() );
  fParent->getControl()->getData()->setZeroAmpsFlag( isZeroAmpsBoxChecked() );
  fParent->getControl()->getData()->setMaxAllowedVoltage( getMaxVoltageValue() );
  fParent->getControl()->getData()->setTargetCurrent( getTargetCurrentValue() );
}


/*****************************************************************************/


VSpinBox::VSpinBox( int minValue, int maxValue, int step, 
		    QWidget* parent, const char* name ) : 
  QSpinBox( parent, name )
{
  fDoubleValidator = new QDoubleValidator( minValue, maxValue, 2, this );
//   setValidator( 0 );
  setValidator( fDoubleValidator );
}


VSpinBox::~VSpinBox()
{
  delete fDoubleValidator;
}


QString VSpinBox::mapValueToText( int value )
{
  //   cout << "value=" << value << "  " << value/100 << "  " << value%100 << endl;
//   return QString( "%1.%2" )
//     .arg( value/100 )
//     .arg( value%100 );
  stringstream textstream("");
  //textstream << value/100 << "." << value/10 << value%10;
  textstream << fixed << setprecision(2) << 1.*value/100.;
  //cout << "mapValueToText:  val=" << value << "  text=" << textstream.str() << endl;
  return QString( textstream.str() );
}

int VSpinBox::mapTextToValue( bool *ok )
{
  *ok = true;
  int val = round( 100 * text().toFloat( ok ) );
  //cout << "mapTextToValue:  text=" << text() 
  //     << "  toFloat=" << text().toFloat( ok ) 
  //     << "  val=" << val << endl;
  //   cout << "val=" << val << endl;
  return val;
}


/*****************************************************************************/


/*****************************************************************************/

/** \class VFeedbackDialog VHVFrame.h
 *  \brief Display the time and a label (in LCD style numbers)
 */

/** Constructor 
 * \arg \c label  - Label to appear next to time
 * \arg \c parent - Qt parent widget
 */
VFeedbackDialog::VFeedbackDialog(VHVFrame *parent, const char *name=0,
				 int telID=0) 
  : QDialog(parent,name)
{
  QFont aFont( "Helvetica", 12, QFont::Normal );
  QFont bFont( "Helvetica", 16, QFont::Bold );
  setFont( aFont );
  fTelID = telID;

  fTitleLab = new QLabel( name, this );
  fTitleLab->setFont( bFont );
  fTitleLab->setGeometry( 10, 10, 400, 50 );

  fFeedbackCBX = new QCheckBox( "Enable Feedback", this );
  fFeedbackCBX->setGeometry( 10, 60, 200, 30 );
  fFeedbackCBX->setChecked( parent->getControl()->getData()->isFeedbackEnabled() );

  fOldSchoolCBX = new QCheckBox( "Use Old (non-Feedback) Mode", this );
  fOldSchoolCBX->setGeometry( 490, 60, 200, 30 );
  fOldSchoolCBX->setChecked( kOldSchool[fTelID] );
  cout << "kOldSchool[" << fTelID << "] = " << kOldSchool[fTelID] << endl;

  QString thresholdText( "PMT Current Standard Threshold (uA)" );
  fThresholdLab = new QLabel( thresholdText, this );
  fThresholdLab->setGeometry( 10, 90, 330, 30 );
  fThresholdSBX = new QSpinBox( 0, 125, 1, this, "Current Threshold" );
  fThresholdSBX->setGeometry( 380, 95, 70, 20 );
  fThresholdSBX->setValue( parent->getControl()->getData()->
			   getThreshold() );

  QString hiThresholdText( "PMT Current Immediate Kill Threshold (uA)" );
  fHiThresholdLab = new QLabel( hiThresholdText, this );
  fHiThresholdLab->setGeometry( 10, 120, 330, 30 );
  fHiThresholdSBX = new QSpinBox( 0, 125, 1, this, "Hi Threshold" );
  fHiThresholdSBX->setGeometry( 380, 125, 70, 20 );
  fHiThresholdSBX->setValue( parent->getControl()->getData()->
			     getHiThreshold() );

  QString patternThresholdText( "PMT Current Pattern Threshold (uA)" );
  fPatternThresholdLab = new QLabel( patternThresholdText, this );
  fPatternThresholdLab->setGeometry( 10, 150, 330, 30 );
  fPatternThresholdSBX = new QSpinBox( 0, 125, 1, this, 
				       "Pattern Threshold" );
  fPatternThresholdSBX->setGeometry( 380, 155, 70, 20 );
  fPatternThresholdSBX->setValue( parent->getControl()->getData()->
				  getPatternThreshold() );

  QString patternMultiplicityText( "PMT Pattern Multiplicity..." );
  fPatternMultiplicityLab = new QLabel( patternMultiplicityText, this );
  fPatternMultiplicityLab->setGeometry( 10, 180, 330, 30 );
  fPatternMultiplicitySBX = new QSpinBox( 2, 4, 1, this, 
					  "Pattern Multiplicity" );
  fPatternMultiplicitySBX->setGeometry( 380, 185, 70, 20 );
  fPatternMultiplicitySBX->setValue( parent->getControl()->getData()->
				     getPatternMultiplicity() );

  QString timeText( "Measurements Per Feedback Cycle..." );
  fMeasurementsPerCycleLab = new QLabel( timeText, this );
  fMeasurementsPerCycleLab->setGeometry( 10, 210, 330, 30 );
  fMeasurementsPerCycleSBX = new QSpinBox( 1, 100, 1, this, "Transient Check Time" );
  fMeasurementsPerCycleSBX->setGeometry( 380, 215, 70, 20 );
  fMeasurementsPerCycleSBX->setValue( parent->getControl()->
				      getData()->getMeasurementsPerCycle() );

  QString cycTest( "Cycles per Feedback Test" );
  fCyclesPerFeedbackTestLab = new QLabel( cycTest, this );
  fCyclesPerFeedbackTestLab->setGeometry( 10, 240, 330, 30 );
  fCyclesPerFeedbackTestSBX = new QSpinBox( 1, 80, 1, this, "Restore Margin" );
  fCyclesPerFeedbackTestSBX->setGeometry( 380, 245, 70, 20 );
  fCyclesPerFeedbackTestSBX->setValue( parent->getControl()->getData()->
				       getCyclesPerFeedbackTest() );
  
  QString numberOfFeedbackLevelsText( "Number of Feedback Levels" );
  fNumberOfFeedbackLevelsLab = new QLabel( numberOfFeedbackLevelsText, this );
  fNumberOfFeedbackLevelsLab->setGeometry( 10, 270, 330, 30 );
  fNumberOfFeedbackLevelsSBX = new QSpinBox( 0, 5, 1, this,
					     "Pattern Restore Margin" );
  //fNumberOfFeedbackLevelsSBX->setEnabled( false );
  fNumberOfFeedbackLevelsSBX->setGeometry( 380, 275, 70, 20 );
  fNumberOfFeedbackLevelsSBX->setValue( parent->getControl()->
					getData()->getNumberOfFeedbackLevels() );

  QString psfText( "PSF (degrees)" );
  fPSFLab = new QLabel( psfText, this );
  fPSFLab->setGeometry( 10, 300, 330, 30 );
  fPSFSBX = new VSpinBox( 1, 50, 1, this, "Restore Margin" );
  fPSFSBX->setGeometry( 380, 305, 70, 20 );
  fPSFSBX->setValue( (int)(100*RAD2DEG*parent->getControl()->getData()->
			   getPSF() + 0.5) );

  QString restoreText( "Restore Margin (degrees)" );
  fRestoreMarginLab = new QLabel( restoreText, this );
  fRestoreMarginLab->setGeometry( 490, 90, 330, 30 );
  fRestoreMarginSBX = new VSpinBox( 0, 50, 1, this, "Restore Margin" );
  fRestoreMarginSBX->setGeometry( 860, 95, 70, 20 );
  fRestoreMarginSBX->setValue( (int)(100*RAD2DEG*parent->getControl()->getData()->
				     getRestoreMargin() + 0.5) );
  
  QString pattdropText( "Drop from Pattern Distance (degrees)" );
  fPatternDistanceDropLab = new QLabel( pattdropText, this );
  fPatternDistanceDropLab->setGeometry( 490, 120, 330, 30 );
  fPatternDistanceDropSBX = new VSpinBox( 0, 50, 1, this, "Restore Margin" );
  fPatternDistanceDropSBX->setGeometry( 860, 125, 70, 20 );
  fPatternDistanceDropSBX
    ->setValue( (int)(100*RAD2DEG*parent->getControl()->getData()->
		      getPatternDistanceDrop() + 0.5) );

  QString minabsposText( "Killed Pix Min Abs Pos Change (degrees)" );
  fMinAbsPositionChangeLab = new QLabel( minabsposText, this );
  fMinAbsPositionChangeLab->setGeometry( 490, 150, 330, 30 );
  fMinAbsPositionChangeSBX = new VSpinBox( 0, 100, 1, this, "Restore Margin" );
  fMinAbsPositionChangeSBX->setGeometry( 860, 155, 70, 20 );
  fMinAbsPositionChangeSBX
    ->setValue( (int)(100*RAD2DEG*parent->getControl()->getData()->
		      getMinAbsPositionChange() + 0.5) );
  
  QString recentTimeText( "Min Time in Lowered State (sec)" );
  fMinTimeBeforeRestoreLab = new QLabel( recentTimeText, this );
  fMinTimeBeforeRestoreLab->setGeometry( 490, 180, 330, 30 );
  fMinTimeBeforeRestoreSBX = new QSpinBox( 0, 1200, 1, this, "Recent Time Limit" );
  fMinTimeBeforeRestoreSBX->setGeometry( 860, 185, 70, 20 );
  fMinTimeBeforeRestoreSBX->setValue( parent->getControl()->
				      getData()->getMinTimeBeforeRestore() );

  QString killedrestoreText( "Killed Pix Restore Attempt Time (sec)" );
  fRestoreAttemptTimeLab = new QLabel( killedrestoreText, this );
  fRestoreAttemptTimeLab->setGeometry( 490, 210, 330, 30 );
  fRestoreAttemptTimeSBX = new QSpinBox( 0, 1200, 1, this, "Restore Attempt Time" );
  fRestoreAttemptTimeSBX->setGeometry( 860, 215, 70, 20 );
  fRestoreAttemptTimeSBX->setValue( parent->getControl()->getData()->
				    getRestoreAttemptTime() );

  QString gainscaleText( "Gain Scaling" );
  fGainScalingLab = new QLabel( gainscaleText, this );
  fGainScalingLab->setGeometry( 490, 240, 330, 30 );
  fGainScalingSBX = new VSpinBox( 10, 100, 1, this, "Gain Scaling" );
  fGainScalingSBX->setGeometry( 860, 245, 70, 20 );
  fGainScalingSBX->setValue( (int)(100*parent->getControl()->getData()->
				   getGainScaling() + 0.5) );

  QString pmtText( "PMT Index" );
  fPMTIndexLab = new QLabel( pmtText, this );
  fPMTIndexLab->setGeometry( 490, 270, 330, 30 );
  fPMTIndexSBX = new VSpinBox( 500, 1000, 1, this, "PMT Index" );
  fPMTIndexSBX->setGeometry( 860, 275, 70, 20 );
  fPMTIndexSBX->setEnabled( false );
  cout << "Found PMT Index = " << parent->getControl()->getData()->
    getPMTIndex()
       << "  Translating to " << (int)(100*parent->getControl()->getData()->
				getPMTIndex() + 0.5)
       << endl;
  fPMTIndexSBX->setValue( (int)(100*parent->getControl()->getData()->
				getPMTIndex() + 0.5) );

  QString safetyText( "Safety Factor" );
  fSafetyFactorLab = new QLabel( safetyText, this );
  fSafetyFactorLab->setGeometry( 490, 300, 330, 30 );
  fSafetyFactorSBX = new VSpinBox( 0, 100, 1, this, "Safety Factor" );
  fSafetyFactorSBX->setGeometry( 860, 305, 70, 20 );
  fSafetyFactorSBX->setValue( (int)(100*parent->getControl()->getData()->
				    getSafetyFactor() + 0.5) );

  QPushButton* okbutton = new QPushButton( "OK", this );
  okbutton->setGeometry( 10, 370, 100, 30 );
  connect( okbutton, SIGNAL( clicked() ), this, SLOT( accept() ) );
  QPushButton* cancelbutton = new QPushButton( "CANCEL", this );
  cancelbutton->setGeometry( 120, 370, 100, 30 );
  connect( cancelbutton, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

VFeedbackDialog::~VFeedbackDialog() {}

bool VFeedbackDialog::isFeedbackChecked() const 
{ 
  return fFeedbackCBX->isChecked(); 
}


bool VFeedbackDialog::isOldSchoolChecked() const 
{ 
  return fOldSchoolCBX->isChecked(); 
}


int VFeedbackDialog::getThresholdValue() const
{
  return fThresholdSBX->value();
}


int VFeedbackDialog::getHiThresholdValue() const
{
  return fHiThresholdSBX->value();
}


int VFeedbackDialog::getPatternThresholdValue() const
{
  return fPatternThresholdSBX->value();
}


int VFeedbackDialog::getPatternMultiplicityValue() const
{
  return fPatternMultiplicitySBX->value();
}


float VFeedbackDialog::getRestoreMarginValue() const
{
  return fRestoreMarginSBX->value()*DEG2RAD/100.;
}


int VFeedbackDialog::getMeasurementsPerCycleValue() const
{
  return fMeasurementsPerCycleSBX->value();
}


int VFeedbackDialog::getMinTimeBeforeRestoreValue() const
{
  return fMinTimeBeforeRestoreSBX->value();
}


int VFeedbackDialog::getNumberOfFeedbackLevelsValue() const
{
  return fNumberOfFeedbackLevelsSBX->value();
}


float VFeedbackDialog::getGainScalingValue() const
{
  return fGainScalingSBX->value()/100.;
}


float VFeedbackDialog::getSafetyFactorValue() const
{
  return fSafetyFactorSBX->value()/100.;
}


int VFeedbackDialog::getRestoreAttemptTimeValue() const
{
  return fRestoreAttemptTimeSBX->value();
}


float VFeedbackDialog::getMinAbsPositionChangeValue() const
{
  return fMinAbsPositionChangeSBX->value()*DEG2RAD/100.;
}


float VFeedbackDialog::getPatternDistanceDropValue() const
{
  return fPatternDistanceDropSBX->value()*DEG2RAD/100.;
}


float VFeedbackDialog::getPSFValue() const
{
  return fPSFSBX->value()*DEG2RAD/100.;
}


int VFeedbackDialog::getCyclesPerFeedbackTestValue() const
{
  return fCyclesPerFeedbackTestSBX->value();
}


float VFeedbackDialog::getPMTIndexValue() const
{
  return fPMTIndexSBX->value()/100.;
}



