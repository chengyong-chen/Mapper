#pragma once

#include "array.h"

///////////////////////////////////////////////////////////////////////////////
// Property pages
typedef enum
{
	AGR_PPN_APPLY = WM_APP+400,
	AGR_PPN_CANCEL,
	AGR_PPN_CLOSED
} AGR_E_PPN;

///////////////////////////////////////////////////////////////////////////////
// General

#define HTTPS_LOGIN_TXT L"HTTP (HTTPS login)"

typedef struct
{
	CString sName;

	CString sValue;

	CString sDescription;

	CString m_strUID;

	CString sDefaultValue;

	CString sParentFolder;

	CString sSection;

	CString sAppliesToDB;
} AGRMMCCCPARAMETER, *PAGRMMCCCPARAMETER;

typedef struct
{
	CString m_strUID;

	CString sPassword;

	CString sClass; //Users or Groups
} AGRMMCWINUSERACCOUNT, *PAGRMMCWINUSERACCOUNT;

typedef enum
{
	AGR_E_GROUP,
	AGR_E_USERACCOUNT,
	AGR_E_SYSTEMACCOUNT
} AGR_E_ACCOUNTTYPE;

typedef struct
{
	AGR_E_ACCOUNTTYPE eAccountType;

	CString sName;

	CString sFullName;

	CString sDomain;

	CString sSID;

	CString sStatus;

	CString sDescription;

	// user info
	CString sAgrUserId;

	CString sAgrUserName;

	CString sAgrUserDescription;

	// Used internally in the SelectGroupsAndUsers dialog
	bool bIsSelected;
} AGRMMCUSERACCOUNTS, *PAGRMMCUSERACCOUNTS;

typedef struct
{
	CString sName;

	CString sDescription;

	CString sVersion;

	CString sServicePack;

	CString sInstallDate;
} AGRMMCVERSIONINFO, *PAGRMMCVERSIONINFO;

typedef struct
{
	bool bUseADS;

	long iUseNds; // 0=Never, 1=autodetect, 2=always
} AGRMMCUSERPREF, *PAGRMMCUSERPREF;

typedef struct
{
	CString sName;

	CString sValue;
} AGRMMCPERFORMANCEDATA, *PAGRMMCPERFORMANCEDATA;

typedef struct
{
	bool bUserAlert;

	bool bAdminAlert;

	bool bAdminAlertWhenNoUserAddress;

	CString sAdminMailAddress;
} AGRMMCMAILCONFIGINFO, *PAGRMMCMAILCONFIGINFO;

typedef struct
{
	CString sUserId;

	CString m_strUID;

	CString sDescription;
} AGRMMCMAILUSERINFO, *PAGRMMCMAILUSERINFO;

///////////////////////////////////////////////////////////////////////////////
// Business Server

typedef struct
{
	// Job information
	CString sJobId;

	CString sName;

	CString sDescription;

	CString sExecutable;

	CString sFunctionId;

	long iOrderNumber;

	CString sOwner;

	long iPriority;

	CString sQueue;

	CString sReportType;

	CString sStartTime;

	CString sEndTime;

	CString sElapsedTime;

	CString sStatus;

	CString sTimeSubmitted;

	CString sMessage;

	// Process Information
	long iProcessId;

	CString sExecutablePath;

	CString sCreationDate;

	long iKernelModeTime;

	CAgrArray<AGRMMCPERFORMANCEDATA, AGRMMCPERFORMANCEDATA> aPerfData;
} AGRMMCSERVERJOBINFO, *PAGRMMCSERVERJOBINFO;

// For internal (in-house) manual configuration
typedef struct
{
	// In-Parameters
	bool bLocalConfig;

	CString sComputerName;

	bool bBusinessServerExists;

	bool bCentralConfigServerExists;

	bool bWebServicesExists;

	bool bManualCreateBusinessServer;

	bool bManualCreateCentralConfigServer;

	bool bManualCreateWebServices;

	//System config
	CString sInstallPath;

#if !defined(_WIN64)
	// x64
	bool box64Installed;

	bool bShow64ConfigOptions; // Show x64 config options (true when 32-bits AMC runs against a computer with 64-bits OS)
	bool bEnablex64Config; // Enable duplicated x64 data sources
	CString sox64Dir; // InstallPath for the x64 registry hive
	bool bRequirex64Duplicate; // Enable/disable warnings if x64 WMI is inaccessible
	bool bWebServicesx64Exists; // Web services exists in x64 reg hive ("Web Application")
	bool bManualCreateWebServicesx64;// Create Web Services on the x64 reg hive
#endif

	// Central Config Server
	CString sDefaultClientLocation;

	CString sDefaultDBCabLocation;

	CString sDefaultCrystalReportLocation;

	bool bCreateCCSRegSettings;

	// Web Services
	CString sAppInstallPath;

	CString sWebInstallPath;

	bool bCreateWebRegSettings;
} AGRMMCMANUALCONFIGINFO, *PAGRMMCMANUALCONFIGINFO;

typedef struct
{
	CString sFileName;

	CString sPath;
} AGRMMCASQLINFO, *PAGRMMCASQLINFO;

typedef struct
{
	CString sFile;

	CString sTable;
} AGRMMCDBCOPYTABLEINFO, *PAGRMMCDBCOPYTABLEINFO;

typedef struct
{
	CString sLogFilePath;

	CString sLstFileName;

	CString sWorkingDir;

	bool bTerminateIfError;

	bool bVerbose;

	bool bAppend;
} AGRMMCASQLCONFIGINFO, *PAGRMMCASQLCONFIGINFO;

typedef struct
{
	bool bSysindexes;

	bool bAagindexes;

	CString sTableList;

	CString sRecreationLocation;
} AGRMMCINDEXCONFIGINFO, *PAGRMMCINDEXCONFIGINFO;

typedef struct
{
	CString sExecDir;

	CString sDirection;

	//    DatabaseType   iDbType;
	CString sInitStr;

	CString sConnectInfo;

	CString sDatabase;

	CString sCopyApp;

	CString sListFile;

	CString sDirectory;

	bool bUseBlkCopy;

	long iArraySize;

	CString m_strUID;

	CString sDbServer;

	bool bWhereFlagB;

	bool bNullConstraints;

	bool bNullConstraintsNull;

	bool bUseNParsing;

	//in
	bool bAgrtid; //sql only
	bool bCreViews;

	bool bAppendData;

	CString sTableLoc;

	CString sIndexLoc;

	bool bProcedures;

	bool bDisplayOnly;

	//out
	CString sWhereClause;

	bool bNoTriggerWarnings;

	bool bViewsAsTables;

	CAgrArray<CString, CString&> asColumnNames;

	CAgrArray<CString, CString&> asTableLocations;

	bool bWhereFlagZero;

	//internal
	bool bPrevSession;
} AGRMMCDBCOPYINFO, *PAGRMMCDBCOPYINFO;

typedef struct
{
	CString sParam;

	CString sValue;
} AGRMMCDBCOPYPARAM, *PAGRMMCDBCOPYPARAM;

///////////////////////////////////////////////////////////////////////////////
// Web Services

typedef struct
{
	CString sWAMUserName;

	CString sWAMUserPass;

	long iAppPoolIdentityType;
} AGRMMCAPPPOOLINFO, *PAGRMMCAPPPOOLINFO;

typedef struct
{
	CString sCommonName;

	CString sValidity;

	CString sSite;

	CString sPort;

	bool bCheckForHash;
} AGRMMCCERTINFO, *PAGRMMCCERTINFO;

///////////////////////////////////////////////////////////////////////////////
// Central Config

typedef struct
{
	CString sName;

	CString sDescription;

	bool bInstall;

	bool bUninstall;

	bool bDelayable;

	bool bReboot;
} AGRMMCDISTRIBUTIONRULE, *PAGRMMCDISTRIBUTIONRULE;

typedef struct
{
	CString sName;

	CString sNetworkShareName;

	CString sClientLocation;

	bool bCopyClientFiles;

	bool bCopyCommonFiles;

	bool bCopyDBFiles;

	bool bCopyCRWFiles;

	bool bCopyHelpFiles;

	CString sDSName;

	CString sDBName;

	CString sDisplayName;

	CString sDBType;

	CString sConnectInfo;

	CString sUsername;

	CString sPassword;

	CString sInit;

	CString sDriver;

	// Oracle (tnsnames.ora) 
	CString sDBNetworkProtocol; // TCP
	CString sHostName; // Database Host Name
	CString sEndpoint; // Port Number or Named Pipe
	CString sInfo1; // Oracle Host Name for Oracle
	CString sInfo2; // Reserved for future use

	// Crystal Report
	CString sCRWLocation;

	// Advanced
	bool bIntegratedSecurity;

	bool bUnifiedLogon;

	bool bBlankUserClient;

	bool bForceRegistry;

	CString sInitLanguage;

	bool bSetTrust;

	bool bCreateHelpSettings;
} AGRMMCCENTRALCONFIGINFO, *PAGRMMCCENTRALCONFIGINFO;

typedef struct
{
	CString sShareName;

	CString sShareLocation;

	CString sStartupType;

	CString sAccount;

	CString sPassword;
} AGRMMCSOFTWAREDISTRSERVICEINFO, *PAGRMMCSOFTWAREDISTRSERVICEINFO;

typedef struct
{
	CString sFileName;

	CString sProductName;

	CString sProductCode;

	CString sUpgradeCode;

	CString sProductVersion;

	CString sDescription;

	CString sPackageCode;

	bool bIsStandAlone;

	CString sMSIParameters;
} AGRMMCMSIPACKAGEINFO, *PAGRMMCMSIPACKAGEINFO;

typedef struct
{
	CString sName;

	CString sShareLocation;

	CString sShareName;
} AGRMMCCCCLIENTINFO, *PAGRMMCCCCLIENTINFO;

typedef struct
{
	CString sDefHelpLoc;

	CString sDefCliLoc;

	CString sDefDBCABLoc;
} AGRMMCCCFOLDERPROPS, *PAGRMMCCCFOLDERPROPS;

///////////////////////////////////////////////////////////////
// Server Queue

typedef struct
{
	CString sName;

	CString m_strServer;

	long iDefaultFlag;

	CString sDescription;

	CString m_strUID;

	long iSlots;

	long iTimer;

	long iBFlag;

	CString sExecutable;

	CString sParameters;

	CString sStatus;

	CString sMetaFileLocation;

	CString sLoggerConfigFile;
} AGRMMCSERVERQUEUEINFO, *PAGRMMCSERVERQUEUEINFO;

///////////////////////////////////////////////////////////////
// Printers

typedef struct
{
	CString sForm;

	CString sNode;

	CString sPlace;

	CString sPrintDest;

	int iPrintFlag;

	CString sPrintQueue;

	CString sPrinter;

	int iReportCols;

	CString sReportFlag;

	int iReportRows;
} AGRMMCAAGPRINTDEF, *PAGRMMCAAGPRINTDEF;

typedef struct
{
	CString sDll;

	CString sDesc;
} AGRMMCPRINTHANDLER, *PAGRMMCPRINTHANDLER;

typedef enum
{
	eUnknown,
	eLocalQueue,
	eOldDevice,
	eOldCommand,
	ePrintDevice,
	eExtHandler
} AGR_PRINTERQUEUE_TYPE;

typedef struct
{
	CString sPrinterDevice;

	CString sPrinterPlugIn;

	CString sExeFile;

	CString sQueueName;

	CString sQueueDesc;

	CString sMetaFileLocation;

	AGR_PRINTERQUEUE_TYPE eType;
} AGRMMCPRINTERQUEUEINFO, *PAGRMMCPRINTERQUEUEINFO;

typedef struct
{
	CString sForm;

	int iPrintFlag;

	int iReportCols;

	int iReportRows;
} PRINTDEF_ROW, *PPRINTDEF_ROW;

typedef struct
{
	AGR_PRINTERQUEUE_TYPE eType;

	CString sDescription; // place
	CString sName; // printer
	CString sExternalCommand; // print_dest
	CString sDevice; // print_queue
	CString sExternalHandler; // print_dest
	bool bDoPrint; // print_flag
	bool bRunAsync; // Marked with :async - for ePrinterDevice only
	bool bHasError; // Error during information retrieval
	CString sErrorTitle; // Short Error message
	CString sErrorMessage; // Error message
	bool bHasWarning; // Warning during information retrieval
	CString sWarningTitle; // Short Warning message
	CString sWarningMessage; // Warning message
	bool bHasInformation; // Information message generated to be shown to admin
	CString sInfoTitle; // Short Information message
	CString sInfoMessage; // Information message
} AGRMMCPRINTERQUEUE, *PAGRMMCPRINTERQUEUE;

///////////////////////////////////////////////////////////////
//System Config
typedef struct
{
	CString sConfigGroup;

	CString sConfigName;

	CString sDisplayGroup;

	CString m_strUID;

	CString sValue;

	long iBflag;

	CString sClient;

	long iTitleNo;

	CString sDescription;

	CString sValueSellist;

	CString sValueSellistDescr;

	CString sShowParams;

	CString sHideParams;

	long iMinValue;

	long iMaxValue;

	long iMaxLen;

	CString sStatus;

	long iSeqNo;

	CString sUserId;
} AGRMMCSYSTEMCONFIGINFO, *PAGRMMCSYSTEMCONFIGINFO;

// UTILITY Structs
typedef struct
{
	CString sName;

	CString sDescription;

	CString m_strServer;

	CString sLastUser;

	CString sDatabase;

	CString sDriver;
} AGRMMCODBCINFO, *PAGRMMCODBCINFO;

typedef struct
{
	CString sUser_id;

	CString sClient;

	CString sResponsible;

	CString sName;

	CString sResourceId;
} AGRRESPONSIBLE, *PAGRRESPONSIBLE;

//UPGRADE structs
typedef struct
{
	CString sFileVersion;

	CString sBuildMachine;

	CString sLegalCopyright;

	CString sOriginalFilename;

	CString sProductName;

	CString sProductVersion;
} AGRUPGRADE_PROJECT_VERSION, *PAGRUPGRADE_PROJECT_VERSION;

typedef struct
{
	CString sLocalPath;

	AGRUPGRADE_PROJECT_VERSION defaultVerProperties;
} AGRUPGRADE_PROPERTIES, *PAGRUPGRADE_PROPERTIES;

//Bespoke
typedef struct
{
	CString sName;

	CString sValue;

	CString sModule;

	long iOnFlag;

	long iOverRidden;
} AGRMMCBESPOKEINFO, *PAGRMMCBESPOKEINFO;

//used for blob copy
typedef struct
{
	CString sFileName;

	CString sTableName;

	CString sWhereClause;

	bool bStarted;

	bool bFinished;

	CString sMsg;

	CAgrArray<CString, LPCTSTR> aFullLog;
} BLOB_COPY_INFO, *PBLOB_COPY_INFO;

// Customized Web Nodes
typedef struct
{
	CString sName; // Name of the node
	CString sURL; // URL for the node
	bool bReg; // Stored in registry or db
	bool bAllUsers; // Available for current or all users
	bool bForNodeGuid; // Show for parent node with name or guid (type)
	CString sParentNodeGuid; // Parent node GUID (node type)
	CString sParentNodeName; // Parent node Name
	bool bAdded; // Node should be added
	bool bModified; // Node is modified and should be saved
	bool bDeleted; // Node should be deleted
	CString sOrgName; // Original name used for previous save - used to delete when name has changed
	bool bOrgReg; // Originally stored in registry
	bool bOrgAllUsers; // Originally available for all users or current
	bool bOrgForNodeGuid; // Originally show for parent with name of guid
} AGR_CUSTOM_WEBNODE, *PAGR_CUSTOM_WEBNODE;

//Table info
typedef struct
{
	CString sName;

	CString sDesc;
} TABLEDESC;

//sysconfig parameter dependecy
typedef struct
{
	bool bShow;

	CString sValue;

	CString sConfigGroup;

	CString sRefConfigName;
} PARAMDEPENDENCY, *PPARAMDEPENDENCY;

//Logger Application definition (Logger.Applications.Meta.xml)
typedef struct
{
	CString sName;

	CString sFileName;

	CString sPath; /*Can contain macros*/
	CString sDescription;

	CString sLoggerConfigFileName; /*Can contain macros*/
	CString sDbType;
} AGRMMCLOGGERAPPLICATIONS, *PAGRMMCLOGGERAPPLICATIONS;

typedef struct
{
	CString sStatus;

	CString sConfigName;

	long lResetInterval;

	long lMaxValue;
} AGRMMC_MAILQUEUE_PROFILE;

//Mail Queue
typedef struct
{
	CString sName;

	CString sFriendlyName;

	CString sDescription;

	CString sValue;

	CString sDefaultValue;
} AGRMMC_SYSTEM_PARAMETER;

#define AGRMMC_SYSTEM_PARAMETER_COUNT(C) sizeof(C)/sizeof(AGRMMC_SYSTEM_PARAMETER)

// Web Config
typedef struct
{
	CString sName;

	CString m_strUID;

	CString sValue;

	CString sDescription;
} AGR_WEBAPPSETTING;
