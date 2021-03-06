
add_definitions(-DTRANSLATION_DOMAIN=\"libmessagecomposer\")



if(BUILD_TESTING)
    add_subdirectory( imagescaling/autotests )
    add_subdirectory( imagescaling/tests )
    add_subdirectory( composer-ng/autotests )
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/part)

configure_file(config-messagecomposer.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-messagecomposer.h)

set( messagecomposer_job_src
    job/jobbase.cpp
    job/contentjobbase.cpp
    job/attachmentjob.cpp
    job/singlepartjob.cpp
    job/multipartjob.cpp
    job/maintextjob.cpp
    job/signjob.cpp
    job/encryptjob.cpp
    job/signencryptjob.cpp
    job/transparentjob.cpp
    job/inserttextfilejob.cpp
    job/skeletonmessagejob.cpp
    job/aliasesexpandjob.cpp
    job/emailaddressresolvejob.cpp
    job/attachmentfrompublickeyjob.cpp
    job/distributionlistexpandjob.cpp
    job/savecontactpreferencejob.cpp
    job/attachmentvcardfromaddressbookjob.cpp
    job/attachmentclipboardjob.cpp
    )

set( messagecomposer_composer_src
    composer/composer.cpp
    composer/signaturecontroller.cpp
    composer/composerlineedit.cpp
    composer/composerviewbase.cpp
    composer/keyresolver.cpp
    )

set( messagecomposer_recipient_src
    recipient/recipientspicker.cpp
    recipient/recipient.cpp
    recipient/recipientline.cpp
    recipient/recipientseditor.cpp
    recipient/recipientseditorsidewidget.cpp
    recipient/kwindowpositioner.cpp
    recipient/distributionlistdialog.cpp
    recipient/recipientseditormanager.cpp
    recipient/recipientspickerwidget.cpp
    )

set( messagecomposer_imagescaling_src
    imagescaling/imagescaling.cpp
    imagescaling/imagescalingwidget.cpp
    imagescaling/imagescalingutils.cpp
    imagescaling/imagescalingselectformat.cpp
    )

set( messagecomposer_part_src
    part/messagepart.cpp
    part/globalpart.cpp
    part/infopart.cpp
    part/textpart.cpp
    )

set( messagecomposer_attachment_src
    attachment/attachmentcontrollerbase.cpp
    attachment/attachmentmodel.cpp
    )

set( messagecomposer_helper_src
    helper/messagehelper.cpp
    helper/messagefactoryng.cpp
    helper/messagefactoryforwardjob.cpp
    helper/messagefactoryreplyjob.cpp
    )

set( messagecomposer_sender_src
    sender/akonadisender.cpp
    )

set(messagecomposer_followupreminder_SRCS
    followupreminder/followupreminderselectdatedialog.cpp
    followupreminder/followupremindercreatejob.cpp
    )

set(messagecomposer_richtextcomposerng_SRCS
    composer-ng/richtextcomposerng.cpp
    composer-ng/richtextcomposersignatures.cpp
    )

set(messagecomposer_plugineditor_SRCS
    plugineditor/plugineditormanager.cpp
    plugineditor/plugineditor.cpp
    plugineditor/plugineditorinterface.cpp
    plugineditor/pluginactiontype.cpp
    )

set(messagecomposer_plugineditorcheckbeforesend_SRCS
   plugineditorcheckbeforesend/plugineditorcheckbeforesend.cpp
   plugineditorcheckbeforesend/plugineditorcheckbeforesendinterface.cpp
   plugineditorcheckbeforesend/plugineditorcheckbeforesendmanager.cpp
   plugineditorcheckbeforesend/plugineditorcheckbeforesendconfigurewidget.cpp
   plugineditorcheckbeforesend/plugineditorcheckbeforesendparams.cpp
   )

set(messagecomposer_plugineditorinit_SRCS
   plugineditorinit/plugineditorinitconfigurewidget.cpp
   plugineditorinit/plugineditorinit.cpp
   plugineditorinit/plugineditorinitmanager.cpp
   plugineditorinit/plugineditorinitinterface.cpp
   )

set(messagecomposer_plugineditorconverttext_SRCS
   plugineditorconverttext/plugineditorconverttextconfigurewidget.cpp
   plugineditorconverttext/plugineditorconverttext.cpp
   plugineditorconverttext/plugineditorconverttextmanager.cpp
   plugineditorconverttext/plugineditorconverttextinterface.cpp
   plugineditorconverttext/plugineditorconverterinitialdata.cpp
   plugineditorconverttext/plugineditorconverterbeforeconvertingdata.cpp
   )
set(messagecomposer_plugineditorgrammar_SRCS
    plugineditorgrammar/plugineditorgrammarmanager.cpp
    plugineditorgrammar/plugineditorgrammarcustomtoolsviewinterface.cpp
    )

set( messagecomposer_src
    ${messagecomposer_plugineditorconverttext_SRCS}
    ${messagecomposer_plugineditorinit_SRCS}
    ${messagecomposer_plugineditor_SRCS}
    ${messagecomposer_richtextcomposerng_SRCS}
    ${messagecomposer_part_src}
    ${messagecomposer_imagescaling_src}
    ${messagecomposer_job_src}
    ${messagecomposer_composer_src}
    ${messagecomposer_recipient_src}
    ${messagecomposer_attachment_src}
    ${messagecomposer_helper_src}
    ${messagecomposer_sender_src}
    ${messagecomposer_followupreminder_SRCS}
    ${messagecomposer_plugineditorcheckbeforesend_SRCS}
    ${messagecomposer_plugineditorgrammar_SRCS}
    utils/util.cpp
    settings/messagecomposersettings.cpp
    )

ki18n_wrap_ui(messagecomposer_src
    imagescaling/ui/imagescalingwidget.ui
    )
ecm_qt_declare_logging_category(messagecomposer_src HEADER messagecomposer_debug.h IDENTIFIER MESSAGECOMPOSER_LOG CATEGORY_NAME org.kde.pim.messagecomposer)


if(KDEPIM_ENTERPRISE_BUILD)
    set(WARN_TOOMANY_RECIPIENTS_DEFAULT true)
    set(ALLOW_SEMICOLON_AS_ADDRESS_SEPARATOR_DEFAULT true)
else()
    set(WARN_TOOMANY_RECIPIENTS_DEFAULT false)
    set(ALLOW_SEMICOLON_AS_ADDRESS_SEPARATOR_DEFAULT false)
endif()

configure_file(settings/messagecomposer.kcfg.cmake ${CMAKE_CURRENT_BINARY_DIR}/messagecomposer.kcfg)

kconfig_add_kcfg_files(messagecomposer_src
    settings/messagecomposersettings_base.kcfgc
    )

add_library( KF5MessageComposer ${messagecomposer_src} )
generate_export_header(KF5MessageComposer BASE_NAME messagecomposer)
add_library(KF5::MessageComposer ALIAS KF5MessageComposer)
target_link_libraries(KF5MessageComposer
    PUBLIC
    KF5::Mime
    KF5::MessageCore
    KF5::PimCommon
    KF5::AkonadiCore
    KF5::IdentityManagement
    KF5::AkonadiMime
    KF5::Libkleo
    KF5::MessageViewer
    PRIVATE
    KF5::MailTransportAkonadi
    KF5::PimTextEdit
    KF5::TemplateParser
    KF5::AkonadiWidgets
    KF5::LibkdepimAkonadi
    KF5::KIOCore
    KF5::I18n
    KF5::Completion     # for KComboBox
    KF5::KIOWidgets     # for KIO::JobUiDelegate
    KF5::KIOFileWidgets # for KEncodingDialog
    KF5::XmlGui         # for KActionCollection
    KF5::SonnetUi
    Grantlee5::TextDocument
    KF5::CalendarCore   # for KCalCore/Todo
    KF5::SendLater
    KF5::FollowupReminder
    KF5::Archive
    KF5::Contacts
    KF5::SonnetCore
    )

target_include_directories(KF5MessageComposer INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/MessageComposer/;${KDE_INSTALL_INCLUDEDIR_KF5}/messagecomposer>")

set_target_properties(KF5MessageComposer PROPERTIES
    VERSION ${MESSAGECOMPOSER_VERSION_STRING}
    SOVERSION ${MESSAGECOMPOSER_SOVERSION}
    EXPORT_NAME MessageComposer
    )

install(TARGETS
    KF5MessageComposer
    EXPORT KF5MessageComposerTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS} ${LIBRARY_NAMELINK}
    )


ecm_generate_headers(MessageComposer_Camelplugineditor_HEADERS
    HEADER_NAMES
    PluginEditor
    PluginActionType
    PluginEditorInterface
    PluginEditorManager
    REQUIRED_HEADERS MessageComposer_plugineditor_HEADERS
    PREFIX MessageComposer
    RELATIVE plugineditor
    )

ecm_generate_headers(MessageComposer_Camelplugineditorinit_HEADERS
    HEADER_NAMES
    PluginEditorInitConfigureWidget
    PluginEditorInit
    PluginEditorInitManager
    PluginEditorInitInterface
    REQUIRED_HEADERS MessageComposer_plugineditorinit_HEADERS
    PREFIX MessageComposer
    RELATIVE plugineditorinit
    )

ecm_generate_headers(MessageComposer_Camelplugineditorconverttext_HEADERS
    HEADER_NAMES
    PluginEditorConvertTextConfigureWidget
    PluginEditorConvertText
    PluginEditorConvertTextManager
    PluginEditorConvertTextInterface
    PluginEditorConverterInitialData
    PluginEditorConverterBeforeConvertingData
    REQUIRED_HEADERS MessageComposer_plugineditorconverttext_HEADERS
    PREFIX MessageComposer
    RELATIVE plugineditorconverttext
    )

ecm_generate_headers(MessageComposer_Camelplugineditorgrammar_HEADERS
    HEADER_NAMES
    PluginEditorGrammarManager
    PluginEditorGrammarCustomToolsViewInterface
    REQUIRED_HEADERS MessageComposer_plugineditorgrammar_HEADERS
    PREFIX MessageComposer
    RELATIVE plugineditorgrammar
    )


ecm_generate_headers(MessageComposer_Camelplugineditorcheckbeforesend_HEADERS
    HEADER_NAMES
    PluginEditorCheckBeforeSend
    PluginEditorCheckBeforeSendInterface
    PluginEditorCheckBeforeSendManager
    PluginEditorCheckBeforeSendConfigureWidget
    PluginEditorCheckBeforeSendParams
    REQUIRED_HEADERS MessageComposer_plugineditorcheckbeforesend_HEADERS
    PREFIX MessageComposer
    RELATIVE plugineditorcheckbeforesend
    )


ecm_generate_headers(MessageComposer_Camelcaseattachement_HEADERS
    HEADER_NAMES
    AttachmentModel
    AttachmentControllerBase
    REQUIRED_HEADERS MessageComposer_attachement_HEADERS
    PREFIX MessageComposer
    RELATIVE attachment
    )

ecm_generate_headers(MessageComposer_Camelcasecomposer_HEADERS
    HEADER_NAMES
    Composer
    ComposerLineEdit
    ComposerViewBase
    SignatureController
    REQUIRED_HEADERS MessageComposer_composer_HEADERS
    PREFIX MessageComposer
    RELATIVE composer
    )

ecm_generate_headers(MessageComposer_Camelcasecomposerng_HEADERS
    HEADER_NAMES
    RichTextComposerNg
    RichTextComposerSignatures
    REQUIRED_HEADERS MessageComposer_composerng_HEADERS
    PREFIX MessageComposer
    RELATIVE composer-ng
    )

ecm_generate_headers(MessageComposer_Camelcasesender_HEADERS
    HEADER_NAMES
    AkonadiSender
    MessageSender
    REQUIRED_HEADERS MessageComposer_sender_HEADERS
    PREFIX MessageComposer
    RELATIVE sender
    )

ecm_generate_headers(MessageComposer_Camelcaseutils_HEADERS
    HEADER_NAMES
    Util
    Kleo_Util
    REQUIRED_HEADERS MessageComposer_utils_HEADERS
    PREFIX MessageComposer
    RELATIVE utils
    )

ecm_generate_headers(MessageComposer_Camelcasehelper_HEADERS
    HEADER_NAMES
    MessageHelper
    MessageFactoryNG
    REQUIRED_HEADERS MessageComposer_helper_HEADERS
    PREFIX MessageComposer
    RELATIVE helper
    )

ecm_generate_headers(MessageComposer_Camelcasesettings_HEADERS
    HEADER_NAMES
    MessageComposerSettings
    REQUIRED_HEADERS MessageComposer_settings_HEADERS
    PREFIX MessageComposer
    RELATIVE settings
    )

ecm_generate_headers(MessageComposer_Camelcasepart_HEADERS
    HEADER_NAMES
    TextPart
    GlobalPart
    InfoPart
    MessagePart
    REQUIRED_HEADERS MessageComposer_part_HEADERS
    PREFIX MessageComposer
    RELATIVE part
    )

ecm_generate_headers(MessageComposer_Camelcasefollowupreminder_HEADERS
    HEADER_NAMES
    FollowupReminderCreateJob
    FollowUpReminderSelectDateDialog
    REQUIRED_HEADERS MessageComposer_followupreminder_HEADERS
    PREFIX MessageComposer
    RELATIVE followupreminder
    )

ecm_generate_headers(MessageComposer_Camelcaserecipient_HEADERS
    HEADER_NAMES
    Recipient
    RecipientsEditor
    RecipientLine
    REQUIRED_HEADERS MessageComposer_recipient_HEADERS
    PREFIX MessageComposer
    RELATIVE recipient
    )

ecm_generate_headers(MessageComposer_Camelcaseimagescaling_HEADERS
    HEADER_NAMES
    ImageScalingWidget
    REQUIRED_HEADERS MessageComposer_imagescaling_HEADERS
    PREFIX MessageComposer
    RELATIVE imagescaling
    )

ecm_generate_headers(MessageComposer_Camelcasejob_HEADERS
    HEADER_NAMES
    JobBase
    AbstractEncryptJob
    ContentJobBase
    InsertTextFileJob
    AttachmentJob
    SinglepartJob
    MainTextJob
    AttachmentFromPublicKeyJob
    MultipartJob
    EncryptJob
    AttachmentVcardFromAddressBookJob
    SignJob
    TransparentJob
    JobBase
    AliasesExpandJob
    SkeletonMessageJob
    AttachmentClipBoardJob
    REQUIRED_HEADERS MessageComposer_job_HEADERS
    PREFIX MessageComposer
    RELATIVE job
    )
ecm_generate_pri_file(BASE_NAME MessageComposer
    LIB_NAME KF5MessageComposer
    DEPS "Mime MessageCore PimCommon Akonadi IdentityManagement AkonadiMime Libkleo" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/MessageComposer
    )


install(FILES
    ${MessageComposer_Camelplugineditorconverttext_HEADERS}
    ${MessageComposer_Camelplugineditorinit_HEADERS}
    ${MessageComposer_Camelplugineditorcheckbeforesend_HEADERS}
    ${MessageComposer_Camelcasecomposer_HEADERS}
    ${MessageComposer_Camelcasecomposerng_HEADERS}
    ${MessageComposer_Camelcasesender_HEADERS}
    ${MessageComposer_Camelcaseutils_HEADERS}
    ${MessageComposer_Camelcasehelper_HEADERS}
    ${MessageComposer_Camelcasesettings_HEADERS}
    ${MessageComposer_Camelcasepart_HEADERS}
    ${MessageComposer_Camelcasefollowupreminder_HEADERS}
    ${MessageComposer_Camelcaserecipient_HEADERS}
    ${MessageComposer_Camelcaseimagescaling_HEADERS}
    ${MessageComposer_Camelcasejob_HEADERS}
    ${MessageComposer_Camelcaseattachement_HEADERS}
    ${MessageComposer_Camelplugineditor_HEADERS}
    ${MessageComposer_Camelplugineditorgrammar_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/MessageComposer
    COMPONENT Devel
    )

install(FILES
    ${MessageComposer_HEADERS}
    ${MessageComposer_plugineditorconverttext_HEADERS}
    ${MessageComposer_plugineditorinit_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/messagecomposer_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/messagecomposersettings_base.h
    ${CMAKE_CURRENT_BINARY_DIR}/messagecomposer_debug.h
    ${MessageComposer_composer_HEADERS}
    ${MessageComposer_composerng_HEADERS}
    ${MessageComposer_sender_HEADERS}
    ${MessageComposer_utils_HEADERS}
    ${MessageComposer_helper_HEADERS}
    ${MessageComposer_settings_HEADERS}
    ${MessageComposer_part_HEADERS}
    ${MessageComposer_followupreminder_HEADERS}
    ${MessageComposer_recipient_HEADERS}
    ${MessageComposer_imagescaling_HEADERS}
    ${MessageComposer_attachement_HEADERS}
    ${MessageComposer_job_HEADERS}
    ${MessageComposer_plugineditor_HEADERS}
    ${MessageComposer_plugineditorcheckbeforesend_HEADERS}
    ${MessageComposer_plugineditorgrammar_HEADERS}

    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/messagecomposer
    COMPONENT Devel
    )

install(FILES
    ${PRI_FILENAME}
    DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

