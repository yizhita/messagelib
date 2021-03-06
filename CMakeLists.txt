cmake_minimum_required(VERSION 3.5)
set(PIM_VERSION "5.10.90")

if (POLICY CMP0053)
    cmake_policy(SET CMP0053 NEW)
endif()

project(Messagelib VERSION ${PIM_VERSION})

option(MIMETREEPARSER_ONLY_BUILD "Build only mimetreeparser" FALSE)
option(KDEPIM_ENTERPRISE_BUILD "Enable features specific to the enterprise branch, which are normally disabled. Also, it disables many components not needed for Kontact such as the Kolab client." FALSE)

option(DKIM_CHECKER_BUILD "DKIM CHECKER (experimental)" FALSE)

set(KF5_MIN_VERSION "5.56.0")


set(MESSAGELIB_LIB_VERSION ${PIM_VERSION})
set(AKONADIMIME_LIB_VERSION "5.10.90")

set(QT_REQUIRED_VERSION "5.10.0")

set(AKONADI_VERSION "5.10.90")
set(GRANTLEETHEME_LIB_VERSION "5.10.90")
set(GRAVATAR_LIB_VERSION "5.10.90")
set(IDENTITYMANAGEMENT_LIB_VERSION "5.10.90")
set(KCONTACTS_LIB_VERSION "5.10.90")
set(KDEPIM_APPS_LIB_VERSION "5.10.90")
set(KLDAP_LIB_VERSION "5.10.90")
set(KMAILTRANSPORT_LIB_VERSION "5.10.90")
set(KMBOX_LIB_VERSION "5.10.90")
set(KMIME_LIB_VERSION "5.10.90")
set(KPIMTEXTEDIT_LIB_VERSION "5.10.90")
set(LIBKDEPIM_LIB_VERSION "5.10.90")
set(LIBKLEO_LIB_VERSION "5.10.90")
set(PIMCOMMON_LIB_VERSION "5.10.90")
set(GPGME_LIB_VERSION "1.8.0")
set(AKONADI_CONTACT_VERSION "5.10.90")

if (${MIMETREEPARSER_ONLY_BUILD})
   set(ECM_VERSION "5.26.0")
   set(KMIME_LIB_VERSION "5.1.40")
else()
    set(ECM_VERSION ${KF5_MIN_VERSION})
endif()

find_package(ECM ${ECM_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${Messagelib_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})
set(LIBRARY_NAMELINK)
include(GenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(ECMQtDeclareLoggingCategory)
include(ECMAddTests)



find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Gui Test)

find_package(KF5Codecs ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5I18n ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Mime ${KMIME_LIB_VERSION} CONFIG REQUIRED)
find_package(QGpgme ${GPGME_LIB_VERSION} CONFIG REQUIRED)

if (NOT ${MIMETREEPARSER_ONLY_BUILD})
   find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Widgets Network PrintSupport WebEngine WebEngineWidgets)
   find_package(KF5Archive ${KF5_MIN_VERSION} CONFIG REQUIRED)
   find_package(KF5Completion ${KF5_MIN_VERSION} CONFIG REQUIRED)
   find_package(KF5Config ${KF5_MIN_VERSION} CONFIG REQUIRED)
   find_package(KF5ConfigWidgets ${KF5_MIN_VERSION} CONFIG REQUIRED)
   find_package(KF5IconThemes ${KF5_MIN_VERSION} CONFIG REQUIRED)
   find_package(KF5ItemViews ${KF5_MIN_VERSION} CONFIG REQUIRED)
   find_package(KF5JobWidgets ${KF5_MIN_VERSION} CONFIG REQUIRED)
   find_package(KF5KIO ${KF5_MIN_VERSION} CONFIG REQUIRED)
   find_package(KF5Service ${KF5_MIN_VERSION} CONFIG REQUIRED)
   find_package(KF5Sonnet ${KF5_MIN_VERSION} CONFIG REQUIRED)
   find_package(KF5TextWidgets ${KF5_MIN_VERSION} CONFIG REQUIRED)
   find_package(KF5WidgetsAddons ${KF5_MIN_VERSION} CONFIG REQUIRED)
   find_package(KF5WindowSystem ${KF5_MIN_VERSION} CONFIG REQUIRED)
   find_package(KF5XmlGui ${KF5_MIN_VERSION} CONFIG REQUIRED)
   find_package(KF5SyntaxHighlighting ${KF5_MIN_VERSION} CONFIG REQUIRED)
   find_package(KF5DBusAddons ${KF5_MIN_VERSION} CONFIG REQUIRED)

   find_package(Grantlee5 "5.1" CONFIG REQUIRED)

   find_package(KF5Akonadi ${AKONADI_VERSION} CONFIG REQUIRED)
   find_package(KF5AkonadiMime ${AKONADIMIME_LIB_VERSION} CONFIG REQUIRED)
   find_package(KF5Contacts ${KCONTACTS_LIB_VERSION} CONFIG REQUIRED)
   find_package(KF5AkonadiContact ${AKONADI_CONTACT_VERSION} CONFIG REQUIRED)
   find_package(KF5FollowupReminder ${KDEPIM_APPS_LIB_VERSION} CONFIG REQUIRED)
   find_package(KF5GrantleeTheme ${GRANTLEETHEME_LIB_VERSION} CONFIG REQUIRED)
   find_package(KF5Gravatar ${GRAVATAR_LIB_VERSION} CONFIG REQUIRED)
   find_package(KF5IdentityManagement ${IDENTITYMANAGEMENT_LIB_VERSION} CONFIG REQUIRED)
   find_package(KF5KaddressbookGrantlee ${KDEPIM_APPS_LIB_VERSION} CONFIG REQUIRED)
   find_package(KF5Ldap ${KLDAP_LIB_VERSION} CONFIG REQUIRED)
   find_package(KF5LibkdepimAkonadi ${LIBKDEPIM_LIB_VERSION} CONFIG REQUIRED)
   find_package(KF5Libkleo ${LIBKLEO_LIB_VERSION} CONFIG REQUIRED)
   find_package(KF5MailTransportAkonadi ${KMAILTRANSPORT_LIB_VERSION} CONFIG REQUIRED)
   find_package(KF5Mbox ${KMBOX_LIB_VERSION} CONFIG REQUIRED)
   find_package(KF5PimCommonAkonadi ${PIMCOMMON_LIB_VERSION} CONFIG REQUIRED)
   find_package(KF5PimTextEdit ${KPIMTEXTEDIT_LIB_VERSION} CONFIG REQUIRED)
   find_package(KF5SendLater ${KDEPIM_APPS_LIB_VERSION} CONFIG REQUIRED)

   find_package(KF5AkonadiSearch "5.10.90" CONFIG REQUIRED)
   set_package_properties(KF5AkonadiSearch PROPERTIES DESCRIPTION "The Akonadi Search libraries" URL "http://www.kde.org" TYPE REQUIRED PURPOSE "Provides search capabilities in KMail and Akonadi")

endif()

#add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)

if(BUILD_TESTING)
   add_definitions(-DBUILD_TESTING)
endif()


add_subdirectory(mimetreeparser)
if (NOT ${MIMETREEPARSER_ONLY_BUILD})
   add_subdirectory(messageviewer)
   add_subdirectory(templateparser)
   add_subdirectory(messagecomposer)
   add_subdirectory(messagecore)
   add_subdirectory(messagelist)
   add_subdirectory(webengineviewer)
endif()

install( FILES messagelib.renamecategories messagelib.categories DESTINATION ${KDE_INSTALL_CONFDIR} )
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
